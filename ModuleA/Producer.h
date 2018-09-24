#ifndef PRODUCER_H
#define PRODUCER_H

#include <memory>
#include <fstream>

#include <QtCore/QtGlobal>

#include "Threads.h"


class RateController : public QObject
{
    Q_OBJECT

  public:
    explicit RateController(QObject *parent = NULL);

    void run();

  private:

    hrclock::time_point m_startTime;
};


// read binary input file
//
// loop at constant frame rate
//
// Enqueue the data with a timestamp
//
// manage TCP connections
class Producer : public QObject
{
    Q_OBJECT

  public:
    Producer();

    static Producer *instance();

    ~Producer();

    void init() { 
        std::cout << "Producer::init()" << std::endl; 
        m_server->start();
        connectToDataStream("com_mod_input.bin"); 
        m_rateController->run();
    }

    // Make public because this will be shared by ComputeThread
    std::shared_ptr<ThreadSafeQueue<FrameT>> m_dataQueue;

    bool connectToDataStream(const std::string& inputFile);

    void readPacket(const double& timestamp);

  signals:
    void disconnect();
    void quit();
    //void checksumReady(ChecksumT checksum);

  public slots:
    //void forwardChecksumReady(ChecksumT checksum);
    void sessionOpened();

  private:

    //QTcpServer *m_tcpServer = nullptr;
    QNetworkSession *m_networkSession = nullptr;

    // Thread pool for each open TCP connection
    //QList<TcpWriterThread *> m_writers;

    // Single thread to offload parsing of packets into header+data
    ComputeThread *m_parser = nullptr;

    // Enforces constant datastream read rate
    RateController *m_rateController;

    ServerThread *m_server = nullptr;

    unsigned int m_frameNumber;     // frame counter
    double m_frameTime;             // timestamp (s)
    double m_frameRate;             // Hz
    unsigned int m_tcpPacketSize;   // size of TCP packet to send
    int m_maxConnections;           // max number of allowable connections

    // File I/O Members
    std::ifstream m_dataStream;     // file (data) stream
    unsigned int m_streamSize;      // total size of file, in bytes
    unsigned int m_bytesRead;       // number of bytes read so far
    unsigned int m_frameSize;       // size of input packet in bytes
    unsigned int m_numFrames;       // number of frames  = stream size / frame size
    PacketT m_packet;               // most recently read packet
};

#endif
