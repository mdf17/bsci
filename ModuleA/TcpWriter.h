#ifndef TCPWRITER_H
#define TCPWRITER_H

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtNetwork/QTcpSocket>

#include "Common.h"


/*******************************************************************
 * class TcpWriterThread
 *
 * Inherits: QThread
 * 
 * Completes request to open a TCP Connection 
 * Waits for ComputeThread to emit a checksumReady() signal
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
    QQueue<ChecksumT> m_dataQueue;

  signals:
    void error(QTcpSocket::SocketError socketError);

  public slots:
    void init();
    void disconnected();
    void readyRead();

  private:
    int m_socketDescriptor;
    QTcpSocket * m_socket;
    unsigned int m_framesPerBlock;
};


#endif
