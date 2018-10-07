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
#ifdef TRUTH_FILE
    FILE * truth = fopen("com_mod_output_truth.bin", "wb");
#endif
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
#ifdef TRUTH_FILE
        if (checksum.timestamp > 10 && checksum.timestamp < 11) {
            //std::cout << "writing checksum " << numChecksums << std::endl;
            size_t retval = fwrite(&checksum.timestamp, sizeof (double), 1, truth);
            if (retval != 1) {
                std::cout << "Error writing timestamp" << std::endl;
            }
            //std::cout << checksum.timestamp << std::endl;
            retval = fwrite(&checksum.sum, sizeof(unsigned int), 8, truth);
            if (retval != 8) {
                std::cout << "Error writing checksums" << std::endl;
            }
            for(int i = 0; i < 8; i++) {
                //std::cout << checksum.sum[i] << std::endl;
            }
        } else if (checksum.timestamp > 11 && truth) {
            std::cout << "closing out file" << std::endl;
            fclose(truth);
            truth = NULL;
        }
#endif


        emit checksumReady(checksum);
    }
}
