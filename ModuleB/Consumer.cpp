#include <cstring>

#include "Consumer.h"

Consumer::Consumer(const std::string id, const QHostAddress ipAddress, const quint16 port) 
    : m_id(id), m_ipAddress(ipAddress), m_port(port)
{ 
    frameBuffer.reset(new ThreadSafeQueue<QByteArray>(MAX_QUEUE_SIZE));
    //m_reader = new TcpReaderThread(m_ipAddress, m_port, this);
    m_socket = new QTcpSocket;

    m_socket->connectToHost(m_ipAddress, m_port, QIODevice::ReadOnly);

    const int timeout = 5 * 1000;
    if (!m_socket->waitForConnected(timeout)) {
        emit error(m_socket->error());
        return;
    }

    if (m_socket->state() == QTcpSocket::ConnectedState) {
        std::cout << "Connected on socket " << m_socket->socketDescriptor() << std::endl;
    }

    m_writer = new FileWriterThread(id, frameBuffer, this);
}

Consumer::~Consumer()
{
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    m_socket->deleteLater();
}

void Consumer::init()
{
    std::cout << "Consumer::init()" << std::endl;
    m_writer->start();
}

void Consumer::run()
{
    //const int numBlocks = 256;
    //const int blockSize = FRAME_SIZE * FRAMES_PER_BLOCK;
    //char buf[blockSize * numBlocks];
    QDataStream in(m_socket);
    in.setVersion(QDataStreamVersion);

    const int timeout = 5 * 1000;
    std::cout << " Entering infinite loop" << std::endl;
    while (true) {

        if (m_socket->state() == QTcpSocket::ConnectedState) {
            std::cout << "Connected on socket " << m_socket->socketDescriptor() << std::endl;
        }
        if (!m_socket->waitForReadyRead(timeout)) {
        std::cout << "Got no data" << std::endl;
            continue;
        }

        std::cout << "Got data!" << std::endl;
            
        QByteArray block;

        in >> block;

        frameBuffer->push_back(block);
    }
}

