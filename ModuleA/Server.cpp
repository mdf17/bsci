#include "Server.h"
#include "Threads.h"

Q_GLOBAL_STATIC(Server, server)

Server *Server::instance()
{
    return server();
}

//Server::~Server()
//{
//    std::cout << "~Server()" << std::endl;
//}

void Server::heartbeat()
{
    //std::cout << "Server::heartbeat()" << std::endl;
    if (isListening())
        std::cout << "listening..." << std::endl;

    waitForNewConnection(-1);

    //emit newConnection(qintptr(10));
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    //if (m_writers.size() >= m_maxConnections) {
    //    std::cout << "Received connection request but already at max open connections" << std::endl;
    //    return;
    //}
    
    std::cout << "Incoming connection on " << socketDescriptor << " !" << std::endl;

    //QTcpSocket * socket = new QTcpSocket;

    //if (!socket->setSocketDescriptor(socketDescriptor)) {
    //    std::cout << "Error opening socket!" << std::endl;
    //    return;
    //}

    TcpWriterThread *t = new TcpWriterThread(socketDescriptor, this);

    //t->setSocket(socket);

    //socket->moveToThread(t);

    connect(this, &Server::checksumReady, t, &TcpWriterThread::publishChecksum, Qt::DirectConnection);
    //connect(this, &Server::checksumReady, t, &TcpWriterThread::publishChecksum);
    //connect(this, &Producer::disconnect, t, &TcpWriterThread::disconnect);
    //connect(this, &Producer::quit, t, &TcpWriterThread::deleteLater);
    t->start();
}

void Server::publishChecksum(ChecksumT checksum)
{
    std::cout << "Server: received publishChecksum signal" << std::endl;
    emit checksumReady(checksum);
}
