#ifndef PRODUCER_H
#define PRODUCER_H

#include <memory>
#include <fstream>

#include <QtCore/QtGlobal>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>

#include "Common.h"


class QTcpServer;

// read binary input file
//
// loop at constant frame rate
//
// Enqueue the data with a timestamp
//
// manage TCP connections
class Producer : public QTcpServer
{
    Q_OBJECT

  public:
    Producer();

    static Producer *instance();

    ~Producer();

    void init();

  signals:
    void disconnect();
    void quit();
    void checksumReady(ChecksumT checksum);

  public slots:
    void forwardChecksum(ChecksumT checksum);

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  private:

    // Thread pool for each open TCP connection
    QList<QThread *> m_writers;

    unsigned int m_tcpPacketSize;   // size of TCP packet to send
    int m_maxConnections;           // max number of allowable connections
};

#endif
