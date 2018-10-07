#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <iostream>
#include <chrono>

#include <QMetaType>
#include <QDataStream>
#include <QString>

#include "Config.h"
#include "ThreadSafeQueue.h"

// For convenience
typedef unsigned int PacketHeader;
typedef unsigned int PacketSample;
typedef double TimeStamp;

// For writing to/reading from TCP socket
const QDataStream::Version QDataStreamVersion = QDataStream::Qt_5_6;


// Program Constants
const unsigned int  BYTE                            = 8;                                // unitless
const unsigned int  BIT                             = 1;                                // unitless
const unsigned int  FRAME_RATE                      = 64000;                            // Hz
const unsigned int  NUM_CHANNELS                    = 8;                                // unitless
const unsigned int  PACKETS_PER_FRAME               = 128;                              // unitless
const unsigned int  PACKET_SIZE                     = sizeof(PacketHeader) + sizeof(PacketSample)*NUM_CHANNELS; //bytes

// Config Param Default Values
const unsigned int  MAX_CONNECTIONS                 = 8;
const unsigned int  MAX_INPUT_QUEUE_SIZE            = MAX_QUEUE_SIZE;
const unsigned int  MAX_OUTPUT_QUEUE_SIZE           = MAX_QUEUE_SIZE;
const unsigned int  TCP_PACKET_SIZE                 = 1500;                             // bytes
const unsigned int  MAX_FILE_SIZE                   = 1048576;                          // Max Output File size of 1MB

// For controlling data read rate
using hrclock = std::chrono::high_resolution_clock;
using READ_RATE = std::chrono::duration<hrclock::rep, std::ratio<1, FRAME_RATE>>;

// struct PacketT
// Contains the raw packet data and a timestamp value
// assume system in which char is 8 bits
// use fixed-size array
typedef std::array<char, PACKET_SIZE> PacketDataT;
struct PacketT {
    PacketDataT data;       // header + 8 channels
    TimeStamp timestamp;    // in seconds
    PacketT() {}
    PacketT(const PacketDataT& d, const double& t) : data(d), timestamp(t) { }
};

// struct ChecksumT
// Contains the checksums for 8 channels for a Frame of data, plus a timestamp
typedef std::array<unsigned int, NUM_CHANNELS> ChecksumDataT;
struct ChecksumT {
    ChecksumDataT sum;
    TimeStamp timestamp;
    friend std::ostream & operator << (std::ostream &out, const ChecksumT &c); 
};

// to allow passing through signals/slots
Q_DECLARE_METATYPE(ChecksumT)


//////////////////////
// Common functions //
//////////////////////

// Reads a 32bit unsigned int from data in network order.
static inline quint32 fromNetworkData(const char *packet)
{
    const unsigned char *upacket = (const unsigned char *)packet;
    return (quint32(upacket[0])           )
        | (quint32(upacket[1])  <<   BYTE )
        | (quint32(upacket[2])  << 2*BYTE )
        | (quint32(upacket[3])  << 3*BYTE );
}

// Writes a 32bit unsigned int from num to data in network order.
// Unused but good to have the symmetric function
static inline void toNetworkData(quint32 num, char *data)
{
    unsigned char *udata = (unsigned char *)data;
    udata[0] = (num & 0xff);
    udata[1] = (num & 0xff00) >> BYTE;
    udata[2] = (num & 0xff0000) >> 2*BYTE;
    udata[3] = (num & 0xff000000) >> 3*BYTE;
}

// helper for debugging
inline std::string to_string(QString qstr)
{
    return qstr.toUtf8().constData();
}


#endif
