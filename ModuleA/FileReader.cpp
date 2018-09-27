#include "FileReader.h"


FileReader::FileReader(SharedQueue<PacketT> dataQueue, QObject *parent) 
  : QObject(parent),
    m_frameTime(0.),
    m_frameRate(FRAME_RATE),           // default value
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
    fclose(m_dataStream);
}

bool FileReader::connectToDataStream()
{
    std::cout << "FileReader::connectToDataStream()" << std::endl;
    //std::ifstream in(m_inputFile, std::ifstream::ate | std::ifstream::binary);
    //m_streamSize = in.tellg(); 
    if ( (m_dataStream = fopen(m_inputFile.c_str(), "rb")) ) {
        fseek(m_dataStream, 0L, SEEK_END);
        m_streamSize = ftell(m_dataStream);
        //m_numPackets = m_streamSize / m_packetSize;
        rewind(m_dataStream);
    } else {
        return false;
    }

    //m_dataStream.open(m_inputFile.c_str(), std::ios::in | std::ios::binary);

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
    //unsigned int pos = m_bytesRead % m_streamSize;
    //std::cout << "FileReader::readPacket(" << std::setprecision(10) << timestamp << ")" << std::endl;
    // streampos pointer wraps around to beginning of file
    //std::cout << "Stream Size = " << m_streamSize << ", bytes read = " << m_bytesRead << std::endl;
    //m_dataStream.seekg(pos); 
    //std::cout << "seekg = " << pos << std::endl;
    // read a frame
    //m_dataStream.read(m_packetData.data(), m_packetSize);
    int packet = fread(m_packetData.data(), m_packetSize, 1, m_dataStream);
    //std::cout << "read " << m_frameSize << " bytes." << std::endl;
    // timestamp the data
    if (packet) {
        m_dataQueue->push_back(PacketT(m_packetData, timestamp));

        //std::cout << "push " << m_dataQueue->size() << std::endl;
        //std::cout << "push frame to queue" << std::endl;
        m_bytesRead += m_packetSize;
        if (m_bytesRead == m_streamSize) {
            m_bytesRead = 0;
            rewind(m_dataStream);
        }
    }
    //std::cout << "done" << std::endl;
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
