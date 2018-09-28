#ifndef TCPWRITER_H
#define TCPWRITER_H

#include <QObject>
#include <QTcpSocket>

#include "Common.h"


/*******************************************************************
 * class TcpWriter
 *
 * Inherits: QObject
 * 
 * Completes request to open a TCP Connection 
 * readyRead() slot waits for a checksumReady() signal from parser thread
 * writes checksum/timestamp data to socket for client to read
 * manages TCP socket
 *******************************************************************/
class TcpWriter : public QObject
{
    Q_OBJECT

  public:
    TcpWriter(qintptr socketDescriptor, QObject *parent = 0);
    ~TcpWriter();

    void write();

    void enqueueChecksum(ChecksumT checksum);
    ThreadSafeQueue<ChecksumT> m_dataQueue;

    QString id() { return m_id; }

  signals:
    void error(QTcpSocket::SocketError socketError);
    void finished();
    void disconnected(QString id);

  public slots:
    void init();
    void disconnect();
    void readyRead();

  private:
    int m_socketDescriptor;
    QTcpSocket * m_socket;
    int m_tcpPacketSize;
    bool m_connectedToHost;
    QString m_id;
};


#endif
