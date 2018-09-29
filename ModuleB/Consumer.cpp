#include <cstring>

#include <QtWidgets>

#include "Consumer.h"

Consumer::Consumer(const std::string id, QWidget *parent)
    : QDialog(parent), m_id(id), idLabel(new QLabel), connectionStatusLabel(new QLabel)
{ 
    frameBuffer.reset(new ThreadSafeQueue<QByteArray>(MAX_QUEUE_SIZE));
    auto quitButton = new QPushButton(tr("Quit"));
    quitButton->setAutoDefault(false);
    QObject::connect(quitButton, &QAbstractButton::clicked, this, &QWidget::close);

    auto buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(quitButton);
    buttonLayout->addStretch(1);

    idLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
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

    mainLayout->addWidget(idLabel);
    mainLayout->addWidget(connectionStatusLabel);
    mainLayout->addLayout(buttonLayout);

    setWindowTitle(QGuiApplication::applicationDisplayName());

    idLabel->setText(tr("Consumer %1\n").arg(QString::fromStdString(m_id)));
}

Consumer::~Consumer()
{
    delete m_socket;
}

void Consumer::closeEvent(QCloseEvent *event)
{
    event->accept();
    std::cout << "Closing..." << std::endl;
    emit signalClosed();
}

void Consumer::init()
{
    std::cout << "Consumer::init()" << std::endl;

    std::string sIpAddress;
    if (!Config::instance()->lookup("ipAddress", sIpAddress)) {
        std::cout << "Did not get an IP address!" << std::endl;
        return;
    }
    m_ipAddress = QHostAddress(QString::fromStdString(sIpAddress));

    std::string sPort;
    if (!Config::instance()->lookup("port", sPort)) {
        std::cout << "Did not get a port number!" << std::endl;
        return;
    }
    m_port = atoi(sPort.c_str());

    std::cout << "Connecting to IP Address: " << sIpAddress << " Port " << sPort << std::endl;

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::disconnected, this, &Consumer::disconnected);
    connect(this, &Consumer::error, this, &Consumer::catchSocketError);

    //m_socket->setReadBufferSize(600);
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_socket->connectToHost(m_ipAddress, m_port, QIODevice::ReadWrite);
    connect(m_socket, &QTcpSocket::readyRead, this, &Consumer::readBlock);

    const int timeout = 5 * 1000;
    if (!m_socket->waitForConnected(timeout)) {
        emit error(m_socket->error());
        return;
    }

    if (m_socket->state() == QTcpSocket::ConnectedState) {
        connectedToHost = true;
        connectionStatusLabel->setText(tr("Connected on IP: %1 port: %2\n")
                                     .arg(m_socket->peerAddress().toString()).arg(m_socket->peerPort()));
    }

    m_writer = new FileWriter(m_id, frameBuffer);
    m_writerThread = new QThread;
    m_writer->moveToThread(m_writerThread);
    QObject::connect(m_writerThread, &QThread::started, m_writer, &FileWriter::init);
    QObject::connect(this, &Consumer::signalClosed, m_writer, &FileWriter::close);
    QObject::connect(m_writer, &FileWriter::finished, m_writer, &QObject::deleteLater);
    QObject::connect(this, &Consumer::finished, m_writerThread, &QThread::quit);
    QObject::connect(m_writerThread, &QThread::finished, m_writerThread, &QThread::deleteLater);
    //m_writerThread->start(QThread::LowPriority);
    m_writerThread->start();
    
    QString hello("HELLO");
    QByteArray out = hello.toLatin1();
    m_socket->write(out);
}

void Consumer::readBlock()
{
     //std::cout << "Got readyRead! signal from socket, bytes available = " << m_socket->bytesAvailable() << std::endl;
     QByteArray block = m_socket->readAll();
     frameBuffer->push(block);
}

void Consumer::disconnected()
{
    connectionStatusLabel->setText(tr("Disconnected\n"));
    connectedToHost = false;
    m_socket->disconnectFromHost();
    emit finished();
}

void Consumer::catchSocketError()
{
    QString errorString = m_socket->errorString();
    std::cout << "Socket Error: " << errorString.toUtf8().constData() << std::endl;
}

