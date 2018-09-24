#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtNetwork/QTcpSocket>
#include "Types.h"

// Reads a 32bit unsigned int from data in network order.
static inline quint32 fromNetworkData(const char *packet)
{
    const unsigned char *upacket = (const unsigned char *)packet;
    //std::cout << std::bitset<8>(upacket[0]) << std::endl;
    return (quint32(upacket[0])           )
        | (quint32(upacket[1])  <<   BYTE )
        | (quint32(upacket[2])  << 2*BYTE )
        | (quint32(upacket[3])  << 3*BYTE );
}

// Writes a 32bit unsigned int from num to data in network order.
static inline void toNetworkData(quint32 num, char *data)
{
    unsigned char *udata = (unsigned char *)data;
    udata[3] = (num & 0xff);
    udata[2] = (num & 0xff00) >> 8;
    udata[1] = (num & 0xff0000) >> 16;
    udata[0] = (num & 0xff000000) >> 24;
}



template<typename T>
class ThreadSafeQueue
{
  public:
    ThreadSafeQueue(unsigned int maxQueueSize) : m_queueBudget(maxQueueSize), m_size(0) { }

    void push_back(const T& data) {
        if (m_size < m_queueBudget) {
            mtx.lock();
            m_queue.enqueue(data);
            m_size++;
            mtx.unlock();
            //emit dataReady();
        } else {
            std::cout << "queue full!!!" << std::endl;
        }
    }

    T pop_front() {
        QMutexLocker locker(&mtx);
        T retval;
        if (m_size > 0) {
            retval = m_queue.dequeue(); 
            m_size--;
        }
        return retval;
    }

    bool empty() {
        QMutexLocker locker(&mtx);
        return m_queue.isEmpty();
    }

    int size() { 
        QMutexLocker locker(&mtx);
        return m_queue.size(); 
    }

  //signals:
    //void dataReady();

  private:
    QMutex mtx;
    QQueue<T> m_queue;
    unsigned int m_queueBudget;
    unsigned int m_size;
};


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
    ComputeThread(unsigned int numFrames, QObject *parent = NULL);

    void parseFrame();
    unsigned int computeChecksum(unsigned int data);

    void run() override;

    std::shared_ptr<ThreadSafeQueue<FrameT>> m_dataQueue;

  protected:

    unsigned int parseHeader(const char * packet) {
        quint32 flipped = fromNetworkData(packet);
        if ( (flipped & 0x1) ) {
            std::cout << m_frameNumber << " Did not see frame sync bit!!!" << std::endl;
        }
        return flipped >> 1;
    }

    unsigned int parseSample(const char * packet) {
        return fromNetworkData(packet);
    }

  signals:
    void checksumReady(const ChecksumT& checksum);

  private:
    hrclock::time_point m_startTime;
    unsigned int m_frameNumber;
    unsigned int m_numFrames;
    int m_framesPerPacket;
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
    TcpWriterThread(QObject *parent);
    ~TcpWriterThread();

    void run() override;

  signals:
    void error(QTcpSocket::SocketError socketError);

  public slots:
    void publishChecksum(const ChecksumT& checksum);
    void disconnect();
  
  private:
    QTcpSocket * m_socket;
    int socketDescriptor;
};


/*******************************************************************
 * class TcpReaderThread
 *
 * Inherits: QThread
 * 
 * manages TCP socket
 *******************************************************************/
class TcpReaderThread : public QThread
{
    Q_OBJECT

  public:
    TcpReaderThread(QString hostName, quint16 port, QObject *parent);
    ~TcpReaderThread();

    void run() override;

  signals:
    void error(QTcpSocket::SocketError socketError);
    void blockReady(const char * block);

  public slots:
    void disconnect();
    void readyRead();
  
  private:
    QString m_hostName;
    quint16 m_port;
    QMutex mtx;
    //QWaitCondition cond;

    QTcpSocket * m_socket;
    int socketDescriptor;
};

class FileWriterThread : public QThread
{
    Q_OBJECT

  public:
    FileWriterThread(const std::string id, QObject *parent);
    ~FileWriterThread();

    void run() override;

    void processNextBlock();

    std::shared_ptr<ThreadSafeQueue<QByteArray>> frameBuffer;

  private:
    std::string m_id;

    QString m_hostName;
    quint16 m_port;
    QMutex mtx;
    //QWaitCondition cond;

    std::ofstream m_outputFile;

};

#endif
