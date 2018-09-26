#include <QtWidgets/QApplication>
#include <QtCore>

#include "Producer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Server *server = Server::instance();

    SharedQueue<FrameT> inputDataQueue;
    SharedQueue<ChecksumT> outputDataQueue;
    // initialize with max size

    // Initialize the File Reader
    QThread *readerThread = new QThread(QThread::HighPriority);
    FileReader *reader = new FileReader(inputDataQueue);
    if (!reader->connectToDataStream("com_mod_input.bin")) {
        std::cout << "Unable to open data stream!" << std::endl;
        return 1;
    }
    reader->moveToThread(readerThread);
    QObject::connect(readerThread, &QThread::started, reader, &FileReader::read);
    QObject::connect(server, &Server::quit, reader, &FileReader::close);
    QObject::connect(server, &Server::quit, readerThread, &QThread::deleteLater);
    

    // Initialize the Frame Parser
    QThread *parserThread = new QThread;
    FrameParser *parser = new FrameParser(inputDataQueue, outputDataQueue);
    parser->setNumFrames(reader->getNumFrames());
    QObject::connect(parser, &FrameParser::checksumReady, server, &Server::forwardChecksum);
    parser->moveToThread(parserThread);
    QObject::connect(parserThread, &QThread::started, parser, &FrameParser::parseFrames);
    QObject::connect(server, &Server::quit, parserThread, &QThread::deleteLater);

    // Initialize the server (begin listening)
    server->init();

    // Begin streaming data
    parserThread->start();
    readerThread->start();


    return app.exec();
}
