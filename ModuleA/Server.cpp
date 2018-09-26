#include <cstdlib>
#include <numeric>
#include <iostream>

#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QSettings>

#include "TcpWriter.h"
#include "Server.h"

Q_GLOBAL_STATIC(Server, producer)

Server::Server() : m_maxConnections(MAX_THREADS)
{
    std::cout << "Server()" << std::endl;
}

Server * Server::instance()
{
    return producer();
}

Server::~Server()
{
    std::cout << "~Server()" << std::endl;
    // Tell writers to close their TCP Sockets
    emit disconnect();

    // Tell child threads to die
    emit quit();
}

void Server::init()
{
    std::cout << "Server::init()" << std::endl; 

    // Attempt to start the server
    if (!listen()) {
        std::cout << "Unable to start the server." << std::endl;
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }

    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    

    //std::string ipAddressUtf8 = ipAddress.toUtf8().constData();
    std::cout << "The server is running on IP: " << serverAddress().toString().toUtf8().constData() << " " 
                                                 << serverPort() << std::endl;

    setMaxPendingConnections(m_maxConnections);
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    std::cout << "Incoming connection on " << socketDescriptor << " !" << std::endl;
    
    qRegisterMetaType<ChecksumT>();
    TcpWriter *writer = new TcpWriter(socketDescriptor);
    QThread *thread = new QThread;
    QObject::connect(this, &Server::checksumReady, writer, &TcpWriter::enqueueChecksum, Qt::QueuedConnection);
    QObject::connect(thread, &QThread::started, writer, &TcpWriter::init);
    writer->moveToThread(thread);
    QObject::connect(this, &Server::quit, thread, &QThread::deleteLater);
    thread->start();
    std::cout << "returned to incomingConnection()" << std::endl;
    m_writers << thread;
}

void Server::forwardChecksum(ChecksumT checksum)
{
    m_checksum = checksum;
    if (!m_writers.isEmpty()) {
        std::cout << "Forward checksum! " << checksum.sum << ", " << checksum.timestamp << std::endl;
        emit checksumReady(m_checksum);
    }
}
