#include "Threads.h"


ComputeThread::ComputeThread(unsigned int numFrames, std::shared_ptr<ThreadSafeQueue<FrameT>> dataQueue, QObject *parent) : QThread(parent), m_numFrames(numFrames), m_data(NUM_CHANNELS), m_checksum()
{
    m_frameNumber = -1;

    m_framesPerBlock = FRAMES_PER_BLOCK;
    m_dataQueue = dataQueue;
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
    ChecksumT checksum;

    //std::cout << "ComputeThread::parseFrame()" << std::endl;
    if(m_dataQueue->empty())
        return;

    //std::cout << "ComputeThread::parseFrame()" << std::endl;
    //for (int i = 0; i < m_framesPerBlock; ++i) {
        FrameT frame = m_dataQueue->pop_front();
        //std::cout << "pop " << m_dataQueue->size() << std::endl;
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

            //std::cout << "HEADER " << frameNumber << " sum = " << m_checksum.sum << std::endl;

            // wrap around to beginning
            if (frameNumber == m_numFrames-1) {
                m_frameNumber = -1;
            }
        }
        // clear internal framebuffer
        m_rawFrames.clear();
        // publish the checksum to the writer threads
        std::cout << "ComputeThread::checksumReady!" << std::endl;
        qRegisterMetaType<ChecksumT>("ChecksumT");
        emit checksumReady(checksum);
    }
}

void ComputeThread::run() 
{
    std::cout << "ComputeThread::run()" << std::endl;
    //using Timestep = READ_RATE;
    //m_startTime = hrclock::now();
    //hrclock::time_point next = m_startTime + Timestep{1};
    while (true) {
        parseFrame();
        //while (hrclock::now() < next)
        //    ;
        //next += Timestep{1};
    }
    //exec();
}


TcpWriterThread::TcpWriterThread(qintptr socketDescriptor, QObject *parent) : QThread(parent), m_socketDescriptor(socketDescriptor)
{
    std::cout << "TcpWriterThread()" << std::endl;
    //m_socket = new QTcpSocket;
}

TcpWriterThread::~TcpWriterThread()
{
    std::cout << "~TcpWriterThread()" << std::endl;
}


void TcpWriterThread::run()
{
    m_socket = new QTcpSocket;

    std::cout << "TcpWriterThread::run()" << std::endl;

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        std::cout << "Error opening socket!" << std::endl;
        return;
    }

    qRegisterMetaType<ChecksumT>("ChecksumT");

    /*QTcpSocket socket;*/

    //if (!m_socket.setSocketDescriptor(m_socketDescriptor)) {
    //    emit error(m_socket.error());
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

void TcpWriterThread::publishChecksum(ChecksumT checksum)
{
    ChecksumT copy = checksum;
    std::cout << "TcpWriterThread: Received publishChecksum signal" << std::endl;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStreamVersion);
    std::cout << "writing to data stream. sum = " << copy.sum << ", time = " << copy.timestamp << std::endl;
    out << copy.sum;
    out << copy.timestamp;
    std::cout << "done" << std::endl;
    if (!m_socket->write(block))
        std::cout << "Failed to write output" << std::endl;
    std::cout << "Returning!" << std::endl;
}


ServerThread::ServerThread(QObject *parent) : QThread(parent)
{
    //qDebug() << qobject_cast<QObject *>(this);
    //m_tcpServer = new Server(this);
}

ServerThread::~ServerThread()
{
    //m_tcpServer->close();
}

void ServerThread::checksumReady(ChecksumT checksum)
{
    std::cout << "ServerThread: Received checksumReady signal" << std::endl;
    ChecksumT copy = checksum;
    qRegisterMetaType<ChecksumT>("ChecksumT");
    emit publishChecksum(copy); 
}

void ServerThread::run()
{
    std::cout << "ServerThread::run()" << std::endl;

    Server *server = Server::instance();
    connect(this, SIGNAL(publishChecksum(ChecksumT)), server, SLOT(publishChecksum(ChecksumT)), Qt::DirectConnection);
    //connect(this, SIGNAL(publishChecksum(ChecksumT)), server, SLOT(publishChecksum(ChecksumT)), Qt::QueuedConnection);
    connect(this, &ServerThread::wakeServer, server, &Server::heartbeat);

    //if (!m_tcpServer->listen()) {
    if (!server->listen()) {
        std::cout << "Unable to start the server." << std::endl;
        return;
    }
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
 
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
 
    //QString ipAddress = m_tcpServer->serverAddress().toString();
    //QString ipAddress = server.serverAddress().toString();
    std::string ipAddressUtf8 = ipAddress.toUtf8().constData();
    std::cout << "The server is running on IP: " << ipAddressUtf8 << " port: " << server->serverPort() << std::endl;


    while(true) {

        //;
        //struct timespec ts = { 1, 0 };
        //nanosleep(&ts, NULL);
        //emit wakeServer();
        server->waitForNewConnection(-1);
    }
    
    //exec(); 
}
