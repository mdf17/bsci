#ifndef FILEREADER_H
#define FILEREADER_H

#include <fstream>
#include <QtCore/QObject>

#include "Common.h"

class FileReader : public QObject
{
    Q_OBJECT

  public:
    FileReader(SharedQueue<FrameT> dataQueue, QObject *parent = NULL);

    bool connectToDataStream(const std::string& inputFile);
    void readPacket(const double& timestamp);

    SharedQueue<FrameT> m_dataQueue;

    unsigned int getNumFrames() { return m_numFrames; }
    
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

#endif
