
#include <iomanip>
#include <bitset>


#include "FrameParser.h"

FrameParser::FrameParser(SharedQueue<PacketT> inputDataQueue, SharedQueue<ChecksumT> outputDataQueue, QObject *parent) 
    : QObject(parent), 
      m_packetNumber(-1), 
      m_numPackets(0), 
      m_packetsPerFrame(PACKETS_PER_FRAME)
{
    m_inputDataQueue = inputDataQueue;
    m_outputDataQueue = outputDataQueue;
}

unsigned int FrameParser::parseHeader(const char * packet) 
{
    quint32 flipped = fromNetworkData(packet);
    if ( (flipped & 0x1) ) {
        //std::cout << m_packetNumber << " frame sync bit!!!" << std::endl;
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
        PacketT packet;
        int numPackets = 0;

        while (numPackets < m_packetsPerFrame) {

            while(m_inputDataQueue->empty())
                ;

            packet = m_inputDataQueue->pop_front();

            unsigned int packetNumber = parseHeader(packet.data.data());
            if (packetNumber != m_packetNumber + 1) {
                std::cout << "New packet " << packetNumber << " does not follow previous packet number " << m_packetNumber << std::endl;
                return;
            }

            m_packetNumber = packetNumber;

            for(size_t i = 0; i < NUM_CHANNELS; ++i) {
                //std::cout << "Frame " << m_frameNumber 
                //          << " Channel " << i << ": " 
                //          << std::bitset<32>(m_data[i]) 
                //          << std::endl;
                checksum.sum[i] += parseSample(packet.data.data() + HEADER_SIZE + SAMPLE_SIZE*i);
            }
            //std::cout << "HEADER " << packetNumber << std::endl;
            
            numPackets++;

            if (packetNumber == m_numPackets - 1) {
                m_packetNumber = -1;
            }
        }

        checksum.timestamp = packet.timestamp;

        emit checksumReady(checksum);
    }
}
