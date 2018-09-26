#include "FileReader.h"


FileReader::FileReader(SharedQueue<FrameT> dataQueue, QObject *parent) 
  : QObject(parent),
    m_frameNumber(-1),
    m_frameTime(0.),
    m_frameRate(FRAME_RATE),
    m_streamSize(0),
    m_bytesRead(0),
    m_frameSize(FRAME_SIZE),
    m_numFrames(0),
    m_packet()
{
    m_dataQueue = dataQueue;
}

void FileReader::close()
{
    m_dataStream.close();
}

bool FileReader::connectToDataStream(const std::string& inputFile)
{
    std::cout << "FileReader::connectToDataStream()" << std::endl;
    std::ifstream in(inputFile, std::ifstream::ate | std::ifstream::binary);
    m_streamSize = in.tellg(); 
    m_numFrames = m_streamSize / m_frameSize;
    in.close();

    m_dataStream.open(inputFile.c_str(), std::ios::in | std::ios::binary);

    return m_dataStream.is_open();
}

void FileReader::read() 
{
    std::cout << "FileReader::read()" << std::endl;
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
    unsigned int pos = m_bytesRead % m_streamSize;
    //std::cout << "FileReader::readPacket(" << std::setprecision(10) << timestamp << ")" << std::endl;
    // streampos pointer wraps around to beginning of file
    //std::cout << "Stream Size = " << m_streamSize << ", bytes read = " << m_bytesRead << std::endl;
    m_dataStream.seekg(pos); 
    //std::cout << "seekg = " << pos << std::endl;
    // read a frame
    m_dataStream.read(m_packet.data(), m_frameSize);
    //std::cout << "read " << m_frameSize << " bytes." << std::endl;
    // timestamp the data
    m_dataQueue->push_back(FrameT(m_packet, timestamp));

    //std::cout << "push " << m_dataQueue->size() << std::endl;
    //std::cout << "push frame to queue" << std::endl;
    m_bytesRead += m_frameSize;
    m_frameNumber++;
    //std::cout << "done" << std::endl;
}
