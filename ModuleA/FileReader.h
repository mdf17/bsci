#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <QtCore/QObject>

#include "Common.h"

class FileReader : public QObject
{
    Q_OBJECT

  public:
    FileReader(SharedQueue<PacketT> dataQueue, QObject *parent = NULL);

    bool connectToDataStream(const std::string& inputFile);
    void readPacket(const double& timestamp);

    SharedQueue<PacketT> m_dataQueue;

    unsigned int getNumPackets() { return m_numPackets; }
    unsigned int getNumFrames() { return m_numPackets / m_packetsPerFrame; }
    

    void read();

  public slots:
    void close();
    
  private:
    unsigned int m_packetCounter;   // packet counter
    double m_frameTime;             // timestamp (s)
    double m_frameRate;             // Hz

    hrclock::time_point m_startTime;

    // File I/O Members
    std::ifstream m_dataStream;     // file (data) stream
    unsigned int m_streamSize;      // total size of file, in bytes
    unsigned int m_bytesRead;       // number of bytes read so far
    unsigned int m_packetSize;      // size of input packet in bytes
    unsigned int m_packetsPerFrame; // packets to a frame
    unsigned int m_numPackets;      // number of frames = stream size / packet size
    unsigned int m_frameSize;       // size of input frame in bytes
    PacketDataT m_packetData;
};

#endif
