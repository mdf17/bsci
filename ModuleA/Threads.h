#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>


#include "Server.h"



/*******************************************************************
 * class ComputeThread
 *
 * Inherits: QThread
 * 
 * Pulls frames off Producer's queue as they become ready
 * Parses frames, computes checksum
 * Signals to TcpWriterThreads when it has data ready to be sent
 *******************************************************************/
class ComputeThread : public QThread 
{
    Q_OBJECT
  
  public:
    ComputeThread(unsigned int numFrames, std::shared_ptr<ThreadSafeQueue<FrameT>> dataQueue, QObject *parent = NULL);

    void parseFrame();
    unsigned int computeChecksum(unsigned int data);

    void run() override;

    std::shared_ptr<ThreadSafeQueue<FrameT>> m_dataQueue;

  protected:

    unsigned int parseHeader(const char * packet) {
        quint32 flipped = fromNetworkData(packet);
        if ( (flipped & 0x1) ) {
            //std::cout << m_frameNumber << " Did not see frame sync bit!!!" << std::endl;
        }
        return flipped >> 1;
    }

    unsigned int parseSample(const char * packet) {
        return fromNetworkData(packet);
    }

  signals:
    void checksumReady(ChecksumT checksum);

  private:
    hrclock::time_point m_startTime;
    unsigned int m_frameNumber;
    unsigned int m_numFrames;
    int m_framesPerBlock;
    std::vector<FrameT> m_rawFrames;
    std::vector<unsigned int> m_data;           // 8 channels of sample data

    ChecksumT m_checksum;
};



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
class TcpWriterThread : public QThread
{
    Q_OBJECT

  public:
    TcpWriterThread(qintptr socketDescriptor, QObject *parent);
    ~TcpWriterThread();

    void run() override;

    //void setSocket(QTcpSocket * socket);

  signals:
    void error(QTcpSocket::SocketError socketError);

  public slots:
    void publishChecksum(ChecksumT checksum);
    void disconnect();
  
  private:
    QTcpSocket *m_socket;
    int m_socketDescriptor;
};

class ServerThread : public QThread
{
    Q_OBJECT

  public:
    ServerThread(QObject *parent = 0);
    ~ServerThread();

    void run() override;


  public slots:
    void checksumReady(ChecksumT checksum);

  signals:
    void publishChecksum(ChecksumT checksum);
    void wakeServer();

  private:
    Server *m_tcpServer;

};


#endif
