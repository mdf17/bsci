#include <cstring>

#include "Consumer.h"

Consumer::Consumer(const std::string id, const QHostAddress ipAddress, const quint16 port)
    : QObject(), m_id(id), m_ipAddress(ipAddress), m_port(port)
{ 
    frameBuffer.reset(new ThreadSafeQueue<QByteArray>(MAX_QUEUE_SIZE));
}

Consumer::~Consumer()
{
}

void Consumer::quit()
{
    m_socket->disconnectFromHost();
    m_socket->waitForDisconnected();
    m_socket->deleteLater();
    m_writer->wait();
}

void Consumer::init()
{
    std::cout << "Consumer::init()" << std::endl;
    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::disconnected, this, &Consumer::disconnected);
    connect(this, &Consumer::error, this, &Consumer::catchSocketError);

    //m_socket->setReadBufferSize(600);
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    m_socket->connectToHost(m_ipAddress, m_port, QIODevice::ReadWrite);
    connect(m_socket, &QTcpSocket::readyRead, this, &Consumer::readBlock);

    const int timeout = 5 * 1000;
    if (!m_socket->waitForConnected(timeout)) {
        emit error(m_socket->error());
        return;
    }

    if (m_socket->state() == QTcpSocket::ConnectedState) {
        std::cout << "Connected on socket " << m_socket->socketDescriptor() << std::endl;
        std::cout << "TcpWriterThread: Socket Address: " << to_string(m_socket->peerAddress().toString()) << std::endl;
        std::cout << "TcpWriterThread: Socket Name: " << to_string(m_socket->peerName()) << std::endl;
        std::cout << "TcpWriterThread: Socket Port: " << m_socket->peerPort() << std::endl;
        connectedToHost = true;
    }
    m_writer = new FileWriterThread(m_id, frameBuffer, this);
    m_writer->start(QThread::LowPriority);
    
    QString hello("HELLO");
    QByteArray out = hello.toLatin1();
    m_socket->write(out);
}

void Consumer::readBlock()
{
     std::cout << "Got readyRead! signal from socket, bytes available = " << m_socket->bytesAvailable() << std::endl;
     QDataStream in(m_socket);
     in.setVersion(QDataStreamVersion);

     QByteArray block;
     in >> block;
     frameBuffer->push_back(block);
}

void Consumer::disconnected()
{
    std::cout << "Consumer::disconnected()!" << std::endl;
    connectedToHost = false;
}

void Consumer::catchSocketError()
{
    QString errorString = m_socket->errorString();
    std::cout << "Socket Error: " << errorString.toUtf8().constData() << std::endl;
}

