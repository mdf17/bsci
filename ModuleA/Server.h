#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <fstream>

#include <QtCore/QtGlobal>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>

#include "Common.h"

#include "TcpWriter.h"

class QTcpServer;

// read binary input file
//
// loop at constant frame rate
//
// Enqueue the data with a timestamp
//
// manage TCP connections
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

  public slots:
    void forwardChecksum(ChecksumT checksum);

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  private:

    // Thread pool for each open TCP connection
    QList<TcpWriter *> m_writers;

    unsigned int m_tcpPacketSize;   // size of TCP packet to send
    int m_maxConnections;           // max number of allowable connections
    ChecksumT m_checksum;
};

#endif
