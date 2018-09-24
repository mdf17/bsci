#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtNetwork/QTcpSocket>


#include "Common.h"

class FileReaderThread : public QThread
{
    Q_OBJECT

  public:
    FileReaderThread(SharedQueue<FrameT> dataQueue, QObject *parent = NULL);

    bool connectToDataStream(const std::string& inputFile);
    void readPacket(const double& timestamp);

    void run() override;

    SharedQueue<FrameT> m_dataQueue;
    
  private:
    unsigned int m_frameNumber;     // frame counter
    double m_frameTime;             // timestamp (s)
    double m_frameRate;             // Hz

    hrclock::time_point m_startTime;

    // File I/O Members
    std::ifstream m_dataStream;     // file (data) stream
    unsigned int m_streamSize;      // total size of file, in bytes
    unsigned int m_bytesRead;       // number of bytes read so far
    unsigned int m_frameSize;       // size of input packet in bytes
    unsigned int m_numFrames;       // number of frames  = stream size / frame size
    PacketT m_packet;
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
    ComputeThread(unsigned int numFrames, SharedQueue<FrameT> inputDataQueue, SharedQueue<ChecksumT> outputDataQueue, QObject *parent = NULL);

    void parseFrame();
    unsigned int computeChecksum(unsigned int data);

    void run() override;

    SharedQueue<FrameT> m_inputDataQueue;
    SharedQueue<ChecksumT> m_outputDataQueue;

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
    TcpWriterThread(qintptr socketDescriptor, SharedQueue<ChecksumT> dataQueue, QObject *parent);
    ~TcpWriterThread();

    void run() override;

    SharedQueue<ChecksumT> m_dataQueue;

  signals:
    void error(QTcpSocket::SocketError socketError);

  private:
    int m_socketDescriptor;
};


#endif
