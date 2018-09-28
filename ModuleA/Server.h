#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

#include "Common.h"

//class QTcpServer;

// Server class
//
// inherits QTcpServer
//
// Signals Producer App to spawn TCP writer threads
// on new connection
//
class Server : public QTcpServer
{
    Q_OBJECT

  public:
    Server();
    static Server *instance();
    ~Server();

    void init();

  signals:
    void disconnect();
    void quit();
    void openNewConnection(qintptr socketDescriptor);

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  private:

    // size of TCP packet to send
    unsigned int m_tcpPacketSize;

    // max number of allowable connections
    int m_maxConnections;
};

#endif
