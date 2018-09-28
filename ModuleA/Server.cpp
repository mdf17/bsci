#include <cstdlib>
#include <numeric>
#include <iostream>

#include <QtNetwork/QNetworkSession>
#include <QtNetwork/QNetworkConfigurationManager>
#include <QSettings>

#include "Server.h"
#include "Config.h"

Q_GLOBAL_STATIC(Server, server)

Server::Server() : m_maxConnections(MAX_THREADS)
{
}

Server * Server::instance()
{
    return server();
}

Server::~Server()
{
    // Tell writers to close their TCP Sockets
    emit disconnect();

    // Tell child threads to die
    emit quit();
}

void Server::init()
{
    Config *config = Config::instance();
    std::string maxConn;
    if (config->lookup("maxConnections", maxConn)) {
        m_maxConnections = atoi(maxConn.c_str());
    }
    setMaxPendingConnections(m_maxConnections);

    QHostAddress hostAddress = QHostAddress::Any; 
    std::string ip;
    if (config->lookup("ipAddress", ip)) {
        hostAddress = QHostAddress(QString::fromStdString(ip));
    }
    quint16 hostPort = 0;
    std::string port;
    if (config->lookup("port", port)) {
        hostPort = atoi(port.c_str());
    }

    // Attempt to start the server
    if (!listen(hostAddress, hostPort)) {
        std::cout << "Unable to start the server." << std::endl;
        return;
    }

    //QString ipAddress;
    //QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //// use the first non-localhost IPv4 address
    //for (int i = 0; i < ipAddressesList.size(); ++i) {
    //    if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
    //        ipAddressesList.at(i).toIPv4Address()) {
    //        ipAddress = ipAddressesList.at(i).toString();
    //        break;
    //    }
    //}

    //// if we did not find one, use IPv4 localhost
    //if (ipAddress.isEmpty())
    //    ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    //std::string ipAddressUtf8 = ipAddress.toUtf8().constData();
    std::cout << "Server::init(): Server running on IP: " << serverAddress().toString().toUtf8().constData() << " " 
                                                 << serverPort() << std::endl;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    std::cout << "Incoming connection on " << socketDescriptor << " !" << std::endl;
    
    qRegisterMetaType<ChecksumT>();
    TcpWriter *writer = new TcpWriter(socketDescriptor);
    QThread *thread = new QThread;
    QObject::connect(thread, &QThread::started, writer, &TcpWriter::init);
    writer->moveToThread(thread);
    QObject::connect(this, &Server::quit, thread, &QThread::deleteLater);
    thread->start();
    m_writers << writer;
    std::cout << m_writers.size() << " Open Connections!!!" << std::endl;
}

void Server::forwardChecksum(ChecksumT checksum)
{
    //std::cout << "Server::forwardChecksum" << std::endl;
    m_checksum = checksum;
    foreach (TcpWriter *writer, m_writers)
        writer->enqueueChecksum(checksum);
}
