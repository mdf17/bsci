#ifndef FRAMEPARSER_H
#define FRAMEPARSER_H

#include <QtCore/QObject>

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
    FrameParser(SharedQueue<PacketT>, QObject *parent = NULL);

    unsigned int parseHeader(const char *);
    unsigned int parseSample(const char *);

    void setNumPackets(unsigned int n) { m_numPackets = n; }

    SharedQueue<PacketT> m_inputDataQueue;


  signals:
    void checksumReady(ChecksumT checksum);

  public slots:
    void parseFrames();

  private:
    hrclock::time_point m_startTime;
    unsigned int m_packetNumber;
    unsigned int m_numPackets;
    int m_packetsPerFrame;
};

#endif