#include <QtWidgets/QApplication>
#include <QtCore>

#include "Server.h"
#include "FileReader.h"
#include "FrameParser.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Config::instance()->init("../common/configParams.txt");

    Server *server = Server::instance();

    qRegisterMetaType<ChecksumT>();

    SharedQueue<PacketT> inputDataQueue;
    SharedQueue<ChecksumT> outputDataQueue;
    // initialize with max size
    inputDataQueue.reset(new ThreadSafeQueue<PacketT>(MAX_INPUT_QUEUE_SIZE));
    outputDataQueue.reset(new ThreadSafeQueue<ChecksumT>(MAX_OUTPUT_QUEUE_SIZE));

    // Initialize the File Reader
    QThread *readerThread = new QThread;
    FileReader *reader = new FileReader(inputDataQueue);
    if (!reader->connectToDataStream()) {
        std::cout << "Unable to open data stream!" << std::endl;
        return 1;
    }
    reader->moveToThread(readerThread);
    QObject::connect(readerThread, &QThread::started, reader, &FileReader::read);
    QObject::connect(server, &Server::quit, reader, &FileReader::close);
    QObject::connect(server, &Server::quit, readerThread, &QThread::deleteLater);
    

    // Initialize the Frame Parser
    QThread *parserThread = new QThread;
    FrameParser *parser = new FrameParser(inputDataQueue);
    // Needed for when header numbers wrap around to beginning
    parser->setNumPackets(reader->getNumPackets());
    QObject::connect(parser, &FrameParser::checksumReady, server, &Server::forwardChecksum, Qt::QueuedConnection);
    parser->moveToThread(parserThread);
    QObject::connect(parserThread, &QThread::started, parser, &FrameParser::parseFrames);
    QObject::connect(server, &Server::quit, parserThread, &QThread::deleteLater);

    // Initialize the server (begin listening)
    server->init();

    // Begin streaming data
    parserThread->start();
    readerThread->start(QThread::HighPriority);


    return app.exec();
}
