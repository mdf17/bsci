#include <QtWidgets>

#include "Producer.h"

Producer::Producer(QWidget *parent) : QDialog(parent), serverStatusLabel(new QLabel), connectionStatusLabel(new QLabel)
{
    auto quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    QObject::connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    serverStatusLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    connectionStatusLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);

    QVBoxLayout *mainLayout = nullptr;
    if (QGuiApplication::styleHints()->showIsFullScreen() || QGuiApplication::styleHints()->showIsMaximized()) {
        auto outerVerticalLayout = new QVBoxLayout(this);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
        auto outerHorizontalLayout = new QHBoxLayout;
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        auto groupBox = new QGroupBox(QGuiApplication::applicationDisplayName());
        mainLayout = new QVBoxLayout(groupBox);
        outerHorizontalLayout->addWidget(groupBox);
        outerHorizontalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored));
        outerVerticalLayout->addLayout(outerHorizontalLayout);
        outerVerticalLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Ignored, QSizePolicy::MinimumExpanding));
    } else {
        mainLayout = new QVBoxLayout(this);
    }

    mainLayout->addWidget(serverStatusLabel);
    mainLayout->addWidget(connectionStatusLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(QGuiApplication::applicationDisplayName());

    QObject::connect(this, &Producer::connectionClosed, this, &Producer::updateDialog);
}

void Producer::closeEvent(QCloseEvent *event)
{
    event->accept();
    std::cout << "Closing..." << std::endl;
    emit signalClosed();
}

void Producer::init()
{
    server = Server::instance();

    qRegisterMetaType<ChecksumT>();
    qRegisterMetaType<qintptr>();

    std::string sMaxInputQueueSize;
    unsigned int maxInputQueueSize = MAX_INPUT_QUEUE_SIZE;
    if (Config::instance()->lookup("maxInputQueueSize", sMaxInputQueueSize)) {
        maxInputQueueSize = atoi(sMaxInputQueueSize.c_str());
    }

    std::string sMaxOutputQueueSize;
    unsigned int maxOutputQueueSize = MAX_OUTPUT_QUEUE_SIZE;
    if (Config::instance()->lookup("maxTcpWriterQueueSize", sMaxOutputQueueSize)) {
        maxOutputQueueSize = atoi(sMaxOutputQueueSize.c_str());
    }

    // initialize with max size
    inputDataQueue.reset(new ThreadSafeQueue<PacketT>(maxInputQueueSize));
    outputDataQueue.reset(new ThreadSafeQueue<ChecksumT>(maxOutputQueueSize));

    // Initialize the File Reader
    readerThread = new QThread;
    reader = new FileReader(inputDataQueue);
    if (!reader->connectToDataStream()) {
        std::cout << "Unable to open data stream!" << std::endl;
        return;
    }
    reader->moveToThread(readerThread);
    QObject::connect(readerThread, &QThread::started, reader, &FileReader::read);
    QObject::connect(this, &Producer::signalClosed, reader, &FileReader::close);
    QObject::connect(reader, &FileReader::finished, readerThread, &QThread::quit);
    QObject::connect(reader, &FileReader::finished, reader, &QObject::deleteLater);
    QObject::connect(readerThread, &QThread::finished, readerThread, &QThread::deleteLater);
    

    // Initialize the Frame Parser
    parserThread = new QThread;
    parser = new FrameParser(inputDataQueue);
    // Needed for when header numbers wrap around to beginning
    parser->setNumPackets(reader->getNumPackets());
    QObject::connect(parser, &FrameParser::checksumReady, this, &Producer::forwardChecksum, Qt::QueuedConnection);
    parser->moveToThread(parserThread);
    QObject::connect(parserThread, &QThread::started, parser, &FrameParser::parseFrames);
    QObject::connect(this, &Producer::signalClosed, parser, &FrameParser::close);
    QObject::connect(parser, &FrameParser::finished, parserThread, &QThread::quit);
    QObject::connect(parser, &FrameParser::finished, parser, &QObject::deleteLater);
    QObject::connect(parserThread, &QThread::finished, parserThread, &QThread::deleteLater);

    // Initialize the server (begin listening)
    QObject::connect(server, &Server::openNewConnection, this, &Producer::incomingConnection);
    server->init();

    serverStatusLabel->setText(tr("The server is running on IP: %1 port: %2\n"
                                            "Run ModuleB now.")
                                     .arg(server->serverAddress().toString()).arg(server->serverPort()));

    connectionStatusLabel->setText(tr("Currently no Open Connections\n"));

    // Begin streaming data
    parserThread->start();
    readerThread->start(QThread::HighPriority);
}

void Producer::updateDialog()
{
    connectionStatusLabel->setText(tr("Currently %1 Open Connections").arg(tcpWriters.size()));
}

void Producer::incomingConnection(qintptr socketDescriptor)
{
    TcpWriter *writer = new TcpWriter(socketDescriptor);
    QThread *thread = new QThread;
    QObject::connect(thread, &QThread::started, writer, &TcpWriter::init);
    writer->moveToThread(thread);
    QObject::connect(writer, &TcpWriter::finished, thread, &QThread::quit);
    QObject::connect(writer, &TcpWriter::finished, writer, &QObject::deleteLater);
    QObject::connect(writer, &TcpWriter::disconnected, this, &Producer::closeConnection, Qt::DirectConnection);
    QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
    tcpWriters << writer;
    tcpWriterThreads << thread;
    std::cout << "Producer: " << tcpWriters.size() << " Open Connections!!!" << std::endl;
    connectionStatusLabel->setText(tr("Currently %1 Open Connections").arg(tcpWriters.size()));
}

void Producer::closeConnection(QString id)
{
    int i = 0;
    foreach (TcpWriter *writer, tcpWriters) {
        if (writer->id() == id) {
            std::cout << "Deleting connection " << to_string(writer->id()) << std::endl;
            tcpWriters.removeAt(i);
        }
        i++;
    }

    tcpWriterThreads.removeAt(i);
    emit updateDialog();
}

void Producer::forwardChecksum(ChecksumT checksum)
{
    foreach(TcpWriter *writer, tcpWriters)
        writer->enqueueChecksum(checksum);
}
