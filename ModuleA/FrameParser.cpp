#include "FrameParser.h"

FrameParser::FrameParser(const SharedQueue<PacketT>& inputDataQueue, QObject *parent) 
    : QObject(parent), 
      m_numPackets(0), 
      m_packetsPerFrame(PACKETS_PER_FRAME),
      m_packetHeaderSize(sizeof(PacketHeader)),
      m_packetSampleSize(sizeof(PacketSample))
{
    m_inputDataQueue = inputDataQueue;
}


void FrameParser::close()
{
    emit finished();
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
        unsigned int packetsRead = 0;
        static unsigned int lastPacketNumber = -1;

        while (packetsRead < m_packetsPerFrame) {

            while(!m_inputDataQueue->getElementNonblocking(packet))
                ;

            unsigned int packetNumber = parseHeader(packet.data.data());
            if (packetNumber != lastPacketNumber + 1) {
                std::cout << "New packet " << packetNumber << " does not follow previous packet number " << lastPacketNumber << std::endl;
                return;
            }

            for(size_t i = 0; i < NUM_CHANNELS; ++i) {
                checksum.sum[i] += parseSample(packet.data.data() + m_packetHeaderSize + i*m_packetSampleSize);
            }

            packetsRead++;
            lastPacketNumber = packetNumber;

            if (packetNumber == m_numPackets - 1) {
                lastPacketNumber = -1;
            }
        }

        checksum.timestamp = packet.timestamp;

        emit checksumReady(checksum);
    }
}
