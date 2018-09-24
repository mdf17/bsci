#ifndef SERVER_H
#define SERVER_H


#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkSession>

#include "Common.h"


class QTcpServer;
class QNetworkSession;



class Server : public QTcpServer
{
    Q_OBJECT

  public:
    inline Server() { std::cout << "Server::Server()" << std::endl; }
    static Server *instance();
    //~Server();

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  signals:
    void checksumReady(ChecksumT checksum);

  public slots:
    void publishChecksum(ChecksumT checksum);
    void heartbeat();

  private:

};





#endif
