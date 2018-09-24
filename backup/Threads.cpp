#include "Producer.h"
#include "Consumer.h"

ComputeThread::ComputeThread(unsigned int numFrames, QObject *parent) : QThread(parent), m_numFrames(numFrames), m_data(NUM_CHANNELS), m_checksum()
{
    m_frameNumber = -1;
    m_checksum.sum = 0;
    m_checksum.timestamp = 0.;
    //m_numFrames = Producer::instance()->getNumFrames();
    Producer * producer = qobject_cast<Producer *>(parent);

    m_framesPerPacket = producer->getTCPPacketSize() / producer->getFrameSize();
    m_dataQueue = producer->m_dataQueue;
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
    //m_rawFrames.clear();
    m_checksum.sum = 0;

    //std::cout << "ComputeThread::parseFrame()" << std::endl;
    //if(Producer::instance()->getQueueSize() < m_framesPerPacket)
    if(Producer::instance()->getQueueSize() == 0)
        return;

    //for (int i = 0; i < m_framesPerPacket; ++i) {
        FrameT frame = Producer::instance()->m_dataQueue->pop_front();
        //std::cout << "pop " << Producer::instance()->getQueueSize() << std::endl;
        m_rawFrames.push_back(frame);
    //}

    if (m_rawFrames.size() == (unsigned int)m_framesPerPacket) {
        for (int i = 0; i < m_framesPerPacket; ++i) {
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
                m_checksum.sum += computeChecksum(m_data[i]);
            }

            m_checksum.timestamp = frame.frameTime;

            //std::cout << "HEADER " << frameNumber << " sum = " << m_checksum.sum << std::endl;

            // wrap around to beginning
            if (frameNumber == m_numFrames-1) {
                m_frameNumber = -1;
            }
        }
        // clear internal framebuffer
        m_rawFrames.clear();
        // publish the checksum to the writer threads
        emit checksumReady(m_checksum);
    }
}

void ComputeThread::run() 
{
    std::cout << "ComputeThread::run()" << std::endl;
    using Timestep = READ_RATE;
    m_startTime = hrclock::now();
    hrclock::time_point next = m_startTime + Timestep{1};
    while (true) {
        parseFrame();
        while (hrclock::now() < next)
            ;
        next += Timestep{1};
    }
    //exec();
}


TcpWriterThread::TcpWriterThread(QObject *parent) : QThread(parent)
{
    m_socket = Producer::instance()->nextPendingConnection();
}

TcpWriterThread::~TcpWriterThread()
{
    std::cout << "~TcpWriterThread()" << std::endl;
    delete m_socket;
    m_socket = nullptr;
}

void TcpWriterThread::run()
{
    //if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
    //    emit error(m_socket->error());
    //    return;
    //}

    // make this thread a loop, stay alive
    exec();
}

void TcpWriterThread::disconnect()
{
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
}

void TcpWriterThread::publishChecksum(const ChecksumT& checksum)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStreamVersion);
    out << checksum.sum;
    out << checksum.timestamp;
    m_socket->write(block);
}

TcpReaderThread::TcpReaderThread(const QString hostName, const quint16 port, QObject *parent) : QThread(parent), m_hostName(hostName), m_port(port)
{
    m_socket = new QTcpSocket;
}

TcpReaderThread::~TcpReaderThread()
{
    std::cout << "~TcpReaderThread()" << std::endl;
    //delete m_socket;
    //m_socket = nullptr;
}

void TcpReaderThread::disconnect()
{
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    m_socket->deleteLater();
}

void TcpReaderThread::readyRead()
{
    mtx.lock();
    QByteArray packet = m_socket->readAll();
    mtx.unlock();

    emit 
}

void TcpReaderThread::run()
{
    m_socket->connectToHost(m_hostName, m_port);
    const int timeout = 5 * 1000;
    if (!m_socket->waitForConnected(timeout)) {
        emit error(m_socket->error());
        return;
    }

    const int numBlocks = 256;
    const int blockSize = FRAME_SIZE * FRAMES_PER_BLOCK;
    char buf[blockSize * numBlocks];
    QDataStream in(m_socket);
    in.setVersion(QDataStreamVersion);

    int i = 0;

    while (true) {

        if (!m_socket->waitForReadyRead(timeout)) {
            continue;
        }
            
        in.readRawData(buf+i*blockSize, blockSize);

        emit blockReady(buf + i*blockSize);
        
        i++;
        if (i == blockSize) i = 0;
    }

}

FileWriterThread::FileWriterThread(const std::string consumerId, QObject *parent)
    : QThread(parent), m_id(consumerId)
{
    Consumer * consumer = qobject_cast<Consumer *>(parent);
    frameBuffer = consumer->frameBuffer;

    std::string fileName = "./data/Consumer_" + m_id + "_output.bin";
    m_outputFile.open(fileName, std::ios::out | std::ios::binary);
}

FileWriterThread::~FileWriterThread()
{
    m_outputFile.close();
}

void FileWriterThread::processNextBlock()
{
    if(frameBuffer->size() == 0)
        return;

    QByteArray block = frameBuffer->pop_front();

    // calc block size
    // calc number of frames in block
    //
    // for each frame
    //   parse header
    //   for each channel
    //     parse checksum
    //   write to file
    
}

void FileWriterThread::run()
{
    std::cout << "FileWriterThread::run()" << std::endl;
    while (true) {
        processNextBlock();
    }
}
