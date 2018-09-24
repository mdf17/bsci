#include <cstring>

#include "Consumer.h"

Consumer::Consumer(const std::string id, const QHostAddress ipAddress, const quint16 port) 
    : m_id(id), m_ipAddress(ipAddress), m_port(port)
{ 
    frameBuffer.reset(new ThreadSafeQueue<QByteArray>(MAX_QUEUE_SIZE));

    m_socket = new QTcpSocket(this);
    m_socket->setReadBufferSize(1520);
    m_socket->connectToHost(m_ipAddress, m_port, QIODevice::ReadOnly);
    connect(m_socket, &QTcpSocket::readyRead, this, &Consumer::readBlock);

    const int timeout = 5 * 1000;
    if (!m_socket->waitForConnected(timeout)) {
        emit error(m_socket->error());
        return;
    }

    if (m_socket->state() == QTcpSocket::ConnectedState) {
        std::cout << "Connected on socket " << m_socket->socketDescriptor() << std::endl;
    }
}

Consumer::~Consumer()
{
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    m_socket->deleteLater();
    m_writer->wait();
}

void Consumer::init()
{
    std::cout << "Consumer::init()" << std::endl;
    m_writer = new FileWriterThread(m_id, frameBuffer, this);
    m_writer->start();
    m_socket->waitForReadyRead(-1);
}

void Consumer::readBlock()
{
     std::cout << "Got readyRead! signal from socket, bytes available = " << m_socket->bytesAvailable() << std::endl;
     //QDataStream in(m_socket);
     //in.setVersion(QDataStreamVersion);

     //std::cout << "Got readyRead! signal from socket, bytes available = " << m_socket->bytesAvailable() << std::endl;

     //QByteArray block;
     QByteArray block = m_socket->readAll();

     //in >> block;

     frameBuffer->push_back(block);
}

void Consumer::run()
{
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

