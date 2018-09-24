#ifndef TYPES_H
#define TYPES_H

#include <array>
#include <iostream>
#include <chrono>
#include <memory>

#include <QtCore/QQueue>
#include <QtCore/QMetaType>
#include <QtCore/QDataStream>
#include <QtCore/QMutex>
#include <QtCore/QDebug>

//const unsigned int  FRAME_RATE      = 64000;
const unsigned int  FRAME_RATE       = 1; //64000;
const unsigned int  MAX_THREADS      = 8;
const unsigned int  NUM_CHANNELS     = 8;
const unsigned int  HEADER_SIZE      = 4;    // bytes
const unsigned int  SAMPLE_SIZE      = 4;    // bytes
const unsigned int  FRAME_SIZE       = HEADER_SIZE + SAMPLE_SIZE*NUM_CHANNELS;
const unsigned int  BYTE             = 8;
const unsigned int  BIT              = 1;
const unsigned int  MAX_QUEUE_SIZE   = 2000;
const unsigned int  MAX_INPUT_QUEUE_SIZE   = MAX_QUEUE_SIZE;
const unsigned int  MAX_OUTPUT_QUEUE_SIZE   = MAX_QUEUE_SIZE;
const unsigned int  TCP_PACKET_SIZE  = 512;
const unsigned int  FRAMES_PER_BLOCK = TCP_PACKET_SIZE / FRAME_SIZE; 

const QDataStream::Version QDataStreamVersion = QDataStream::Qt_5_6;

// assume for now that char is 8 bits
typedef std::array<char, FRAME_SIZE> PacketT;

using hrclock = std::chrono::high_resolution_clock;
using READ_RATE = std::chrono::duration<hrclock::rep, std::ratio<1, FRAME_RATE>>;
using WRITE_RATE = std::chrono::duration<hrclock::rep, std::ratio<1, FRAME_RATE/2>>;



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



struct FrameT {
    PacketT packet;
    double frameTime;
    FrameT() {}
    FrameT(const PacketT& p, const double& t) : packet(p), frameTime(t) {}
};

struct ChecksumT {
    double timestamp;
    unsigned int sum;
};

Q_DECLARE_METATYPE(FrameT)
Q_DECLARE_METATYPE(ChecksumT)


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

  private:
    QMutex mtx;
    QQueue<T> m_queue;
    unsigned int m_queueBudget;
    unsigned int m_size;
};

template<typename T> 
using SharedQueue = std::shared_ptr<ThreadSafeQueue<T>>;


#endif
