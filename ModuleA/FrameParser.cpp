
#include <iomanip>
#include <bitset>


#include "FrameParser.h"

FrameParser::FrameParser(SharedQueue<FrameT> inputDataQueue, SharedQueue<ChecksumT> outputDataQueue, QObject *parent) 
    : QObject(parent), m_numFrames(0), m_data(NUM_CHANNELS)
{
    m_frameNumber = -1;

    m_framesPerBlock = FRAMES_PER_BLOCK;
    m_inputDataQueue = inputDataQueue;
    m_outputDataQueue = outputDataQueue;
}

unsigned int FrameParser::computeChecksum(unsigned int data)
{
    unsigned int lower = 0, upper = 0;
    lower = data & (0xffff);
    upper = data >> 2*BYTE;
    return lower + upper;
}

unsigned int FrameParser::parseHeader(const char * packet) 
{
    quint32 flipped = fromNetworkData(packet);
    if ( (flipped & 0x1) ) {
        //std::cout << m_frameNumber << " Did not see frame sync bit!!!" << std::endl;
    }
    return flipped >> 1;
}

unsigned int FrameParser::parseSample(const char * packet) 
{
    return fromNetworkData(packet);
}


void FrameParser::parseFrames()
{
    while (true) {
        ChecksumT checksum;

        //std::cout << "FrameParser::parseFrame()" << std::endl;
        while(m_inputDataQueue->empty())
            ;

        //std::cout << "FrameParser::parseFrame()" << std::endl;
        FrameT frame = m_inputDataQueue->pop_front();
        //std::cout << "pop " << m_inputDataQueue->size() << std::endl;

        unsigned int frameNumber = parseHeader(frame.packet.data());
        if (frameNumber != m_frameNumber + 1) {
            std::cout << "New frame " << frameNumber << " does not follow previous frame number " << m_frameNumber << std::endl;
            return;
        }

        m_frameNumber = frameNumber;

        for(size_t i = 0; i < NUM_CHANNELS; ++i) {
            m_data[i] = parseSample(frame.packet.data() + HEADER_SIZE + SAMPLE_SIZE*i);
            //std::cout << "Frame " << m_frameNumber 
            //          << " Channel " << i << ": " 
            //          << std::bitset<32>(m_data[i]) 
            //          << std::endl;
            checksum.sum += computeChecksum(m_data[i]);
        }

        checksum.timestamp = frame.frameTime;

        if (m_frameNumber == m_numFrames - 1) {
            m_frameNumber = -1;
        }
        std::cout << "HEADER " << frameNumber << " sum = " << checksum.sum << std::endl;
        emit checksumReady(checksum);
    }
}
