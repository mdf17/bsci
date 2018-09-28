#include "FileReader.h"


FileReader::FileReader(SharedQueue<PacketT> dataQueue, QObject *parent) 
  : QObject(parent),
    m_dataStream(NULL),
    m_streamSize(0),
    m_bytesRead(0),
    m_packetSize(PACKET_SIZE),
    m_packetsPerFrame(PACKETS_PER_FRAME),
    m_numPackets(0),
    m_frameSize(m_packetSize * m_packetsPerFrame),
    m_packetData()
{
    m_dataQueue = dataQueue;
    readConfigFile();
}

void FileReader::close()
{
    std::cout << "FileReader::close()" << std::endl;
    fclose(m_dataStream);
}

bool FileReader::connectToDataStream()
{
    std::cout << "FileReader::connectToDataStream(" << m_inputFile << ")" << std::endl;
    if ( (m_dataStream = fopen(m_inputFile.c_str(), "rb")) ) {
        fseek(m_dataStream, 0L, SEEK_END);
        m_streamSize = ftell(m_dataStream);
        m_numPackets = m_streamSize / m_packetSize;
        rewind(m_dataStream);
    } else {
        return false;
    }

    return true;
}

void FileReader::read() 
{
    using Timestep = READ_RATE;
    std::chrono::duration<double> timestamp;
    m_startTime = hrclock::now();
    hrclock::time_point next = m_startTime + Timestep{1};
    while(true) {
        timestamp = hrclock::now() - m_startTime;
        readPacket(timestamp.count());
        while (hrclock::now() < next)
            ;
        next += Timestep{1};
    }
}

// timestamp passed down from event loop
void FileReader::readPacket(const double& timestamp)
{
    // read a frame
    int packet = fread(m_packetData.data(), m_packetSize, 1, m_dataStream);
    if (packet) {
        m_dataQueue->push_back(PacketT(m_packetData, timestamp));

        m_bytesRead += m_packetSize;
        if (m_bytesRead == m_streamSize) {
            m_bytesRead = 0;
            rewind(m_dataStream);
        }
    }
}

void FileReader::readConfigFile()
{
    Config * config = Config::instance();

    std::string value;
    if (config->lookup("inputFile", value)) {
        m_inputFile = value;
        std::cout << "Setting input datastream file to " << m_inputFile << std::endl;
    } else {
        m_inputFile = "com_mod_input.bin";
    }
}
