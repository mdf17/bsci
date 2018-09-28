#ifndef FILEREADER_H
#define FILEREADER_H

#include "Common.h"

class FileReader : public QObject
{
    Q_OBJECT

  public:
    FileReader(const SharedQueue<PacketT>& dataQueue, QObject *parent = NULL);

    //bool connectToDataStream(const std::string& inputFile);
    bool connectToDataStream();
    void readPacket(const double& timestamp);

    SharedQueue<PacketT> m_dataQueue;

    unsigned int getNumPackets() { return m_numPackets; }

    void read();
    void readConfigFile();

  signals:
    void finished();

  public slots:
    void close();


    
  private:
    // for timestamping the data
    hrclock::time_point m_startTime;

    // File I/O Members
    std::string m_inputFile;        // input file name
    FILE *m_dataStream;             // file (data) stream (use raw C, much faster than fstream)
    unsigned int m_streamSize;      // total size of file, in bytes
    unsigned int m_bytesRead;       // number of bytes read so far
    unsigned int m_packetSize;      // size of input packet in bytes
    unsigned int m_packetsPerFrame; // packets to a frame
    unsigned int m_numPackets;      // number of frames = stream size / packet size
};

#endif
