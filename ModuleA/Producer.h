#ifndef PRODUCER_H
#define PRODUCER_H

#include <memory>
#include <fstream>

#include <QtCore/QtGlobal>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QNetworkSession>

#include "Threads.h"


class RateController : public QObject
{
    Q_OBJECT

  public:
    inline RateController() { }

    static RateController *instance();

    void run();

  private:

    hrclock::time_point m_startTime;
};

class QTcpServer;
class QNetworkSession;

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

    // Make public because this will be shared by ComputeThread
    std::shared_ptr<ThreadSafeQueue<FrameT>> m_inputDataQueue;
    std::shared_ptr<ThreadSafeQueue<ChecksumT>> m_outputDataQueue;

    //bool connectToDataStream(const std::string& inputFile);

    //void readPacket(const double& timestamp);

  signals:
    void disconnect();
    void quit();

  public slots:
    //void openStream();

  protected:
    void incomingConnection(qintptr socketDescriptor) override;

  private:

    // Thread pool for each open TCP connection
    //QList<TcpWriterThread *> m_writers;


    // File reader thread
    FileReaderThread *m_reader = nullptr; 

    // Single thread to offload parsing of packets into header+data
    ComputeThread *m_parser = nullptr;

    // Enforces constant datastream read rate
    //RateController *m_rateController;

    //ServerThread *m_server = nullptr;

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
