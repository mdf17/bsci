#ifndef FRAMEPARSER_H
#define FRAMEPARSER_H

#include "Common.h"

/*******************************************************************
 * class FrameParser
 *
 * Inherits: QObject
 * 
 * Pulls frames off Producer's queue as they become ready
 * Parses frames, computes checksum
 * Signals to TcpWriter Threads when it has data ready to be sent
 *******************************************************************/
class FrameParser : public QObject
{
    Q_OBJECT
  
  public:
    FrameParser(const SharedQueue<PacketT>&, QObject *parent = NULL);

    unsigned int parseHeader(const char *);
    unsigned int parseSample(const char *);

    void setNumPackets(unsigned int n) { m_numPackets = n; }

    SharedQueue<PacketT> m_inputDataQueue;


  signals:
    void checksumReady(ChecksumT checksum);
    void finished();

  public slots:
    void parseFrames();
    void close();

  private:
    hrclock::time_point m_startTime;
    unsigned int m_numPackets;
    unsigned int m_packetsPerFrame;
    unsigned int m_packetHeaderSize;
    unsigned int m_packetSampleSize;
};

#endif
