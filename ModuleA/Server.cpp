#include "Server.h"

Q_GLOBAL_STATIC(Server, server)

Server::Server() : m_maxConnections(MAX_CONNECTIONS)
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

    std::cout << "Server::init(): Server running on IP: " << serverAddress().toString().toUtf8().constData() << " " 
                                                          << serverPort() << std::endl;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    std::cout << "Server: Incoming connection on " << socketDescriptor << " !" << std::endl;
    emit openNewConnection(socketDescriptor);
}
