#include <cstdlib>
#include <numeric>
#include <iostream>

#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QSettings>

#include "Producer.h"

Q_GLOBAL_STATIC(Producer, producer)

Producer::Producer()
  : m_maxConnections(MAX_THREADS),
    m_frameSize(FRAME_SIZE),
    m_numFrames(0)
{
    std::cout << "Producer()" << std::endl;
}

Producer * Producer::instance()
{
    return producer();
}

Producer::~Producer()
{
    std::cout << "~Producer()" << std::endl;
    // Tell writers to close their TCP Sockets
    emit disconnect();

    // Tell child threads to die
    emit quit();
}

void Producer::init()
{
    std::cout << "Producer::init()" << std::endl; 

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
}

void Producer::incomingConnection(qintptr socketDescriptor)
{
    std::cout << "Incoming connection on " << socketDescriptor << " !" << std::endl;
    TcpWriter *writer = new TcpWriter(socketDescriptor);
    QObject::connect(this, &Server::checksumReady, writer, &TcpWriter::enqueueChecksum);
    QThread *thread = new QThread;
    writer->moveToThread(thread);
    connect(thread, SIGNAL(started()), writer, SLOT(init()));
    connect(this, &Producer::quit, t, &QThread::deleteLater);
    t->start();
    m_writers << writer;

    if (m_writers.size() == m_maxConnections)
        stopAccepting();
}

void Producer::forwardChecksum(ChecksumT checksum)
{
    emit checksumReady(checksum);
}
