#include <iomanip>
#include <QtNetwork/QHostAddress>

#include "Threads.h"

FileReaderThread::FileReaderThread(std::shared_ptr<ThreadSafeQueue<FrameT>> dataQueue, QObject *parent) : QThread(parent),
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


bool FileReaderThread::connectToDataStream(const std::string& inputFile)
{
    std::cout << "FileReaderThread::connectToDataStream()" << std::endl;
    std::ifstream in(inputFile, std::ifstream::ate | std::ifstream::binary);
    m_streamSize = in.tellg(); 
    m_numFrames = m_streamSize / m_frameSize;
    in.close();

    m_dataStream.open(inputFile.c_str(), std::ios::in | std::ios::binary);

    return m_dataStream.is_open();
}

void FileReaderThread::run() 
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
void FileReaderThread::readPacket(const double& timestamp)
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


ComputeThread::ComputeThread(unsigned int numFrames, 
        std::shared_ptr<ThreadSafeQueue<FrameT>> inputDataQueue, 
        std::shared_ptr<ThreadSafeQueue<ChecksumT>> outputDataQueue, 
        QObject *parent) 
    : QThread(parent), m_numFrames(numFrames), m_data(NUM_CHANNELS)
{
    m_frameNumber = -1;

    m_framesPerBlock = FRAMES_PER_BLOCK;
    m_inputDataQueue = inputDataQueue;
    m_outputDataQueue = outputDataQueue;
}

unsigned int ComputeThread::computeChecksum(unsigned int data)
{
    unsigned int lower = 0, upper = 0;
    lower = data & (0xffff);
    upper = data >> 2*BYTE;
    return lower + upper;
}

void ComputeThread::parseFrame()
{
    ChecksumT checksum;

    //std::cout << "ComputeThread::parseFrame()" << std::endl;
    if(m_inputDataQueue->empty())
        return;

    //std::cout << "ComputeThread::parseFrame()" << std::endl;
    //for (int i = 0; i < m_framesPerBlock; ++i) {
        FrameT frame = m_inputDataQueue->pop_front();
        //std::cout << "pop " << m_inputDataQueue->size() << std::endl;
        m_rawFrames.push_back(frame);
    //}

    if (m_rawFrames.size() == (unsigned int)m_framesPerBlock) {
        for (int i = 0; i < m_framesPerBlock; ++i) {
            const FrameT& frame = m_rawFrames[i];
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

            //std::cout << "HEADER " << frameNumber << " sum = " << checksum.sum << std::endl;

            // wrap around to beginning
            if (frameNumber == m_numFrames-1) {
                m_frameNumber = -1;
            }
        }
        // clear internal framebuffer
        m_rawFrames.clear();
        m_outputDataQueue->push_back(checksum);
    }
}

void ComputeThread::run() 
{
    std::cout << "ComputeThread::run()" << std::endl;
    while (true) {
        parseFrame();
    }
}


TcpWriterThread::TcpWriterThread(qintptr socketDescriptor, SharedQueue<ChecksumT> dataQueue, QObject *parent) : QThread(parent), m_socketDescriptor(socketDescriptor)
{
    std::cout << "TcpWriterThread()" << std::endl;
    m_dataQueue = dataQueue;
}

TcpWriterThread::~TcpWriterThread()
{
    std::cout << "~TcpWriterThread()" << std::endl;
}

void TcpWriterThread::run()
{
    std::cout << "TcpWriterThread::run()" << std::endl;

    m_socket = new QTcpSocket;

    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpWriterThread::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpWriterThread::readyRead, Qt::DirectConnection);

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }

    exec();
}

void TcpWriterThread::disconnected()
{
    std::cout << "TCPWriterThread: disconnected!" << std::endl;
    m_socket->deleteLater();
    exit(0);
}

void TcpWriterThread::readyRead()
{
    QByteArray data = m_socket->readAll();

    write();
}

//void checksumReady(ChecksumT checksum)
//{
//    m_dataQueue.push(checksum);
//}

void TcpWriterThread::write()
{
    while (true) {
        while (m_dataQueue->empty())
            ;

        ChecksumT checksum = m_dataQueue->pop_front();
        std::cout << "TcpWriterThread: Got ChecksumT from queue" << std::endl;
        QByteArray block;
        QDataStream out(&block, QIODevice::ReadWrite);
        out.setVersion(QDataStreamVersion);
        for (int i = 0; i < 50; i++) {
            out << checksum.sum;
            out << checksum.timestamp;
        }

        std::cout << "TcpWriterThread: Connected to socket " << m_socket->socketDescriptor() << std::endl;
        std::cout << "TcpWriterThread: Socket Address: " << to_string(m_socket->peerAddress().toString()) << std::endl;
        std::cout << "TcpWriterThread: Socket Name: " << to_string(m_socket->peerName()) << std::endl;
        std::cout << "TcpWriterThread: Socket Port: " << m_socket->peerPort() << std::endl;

        int nBytes = m_socket->write(block);
        if (nBytes == 0)
            std::cout << "Failed to write output" << std::endl;
        m_socket->flush();
        std::cout << "Wrote " << nBytes << " bytes to socket" << std::endl;
    }
}
