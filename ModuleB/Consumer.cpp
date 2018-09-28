#include <cstring>

#include "Consumer.h"

Consumer::Consumer(const std::string id)
    : QObject(), m_id(id)
{ 
    frameBuffer.reset(new ThreadSafeQueue<QByteArray>(MAX_QUEUE_SIZE));
}

Consumer::~Consumer()
{
}

void Consumer::init()
{
    std::cout << "Consumer::init()" << std::endl;

    std::string sIpAddress;
    if (!Config::instance()->lookup("ipAddress", sIpAddress)) {
        std::cout << "Did not get an IP address!" << std::endl;
        return;
    }
    m_ipAddress = QHostAddress(QString::fromStdString(sIpAddress));

    std::string sPort;
    if (!Config::instance()->lookup("port", sPort)) {
        std::cout << "Did not get a port number!" << std::endl;
        return;
    }
    m_port = atoi(sPort.c_str());

    std::cout << "Connecting to IP Address: " << sIpAddress << " Port " << sPort << std::endl;

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
        std::cout << "Consumer: Socket Address: " << to_string(m_socket->peerAddress().toString()) << std::endl;
        std::cout << "Consumer: Socket Name: " << to_string(m_socket->peerName()) << std::endl;
        std::cout << "Consumer: Socket Port: " << m_socket->peerPort() << std::endl;
        connectedToHost = true;
    }

    m_writer = new FileWriter(m_id, frameBuffer);
    m_writerThread = new QThread;
    m_writer->moveToThread(m_writerThread);
    QObject::connect(m_writerThread, &QThread::started, m_writer, &FileWriter::init);
    QObject::connect(this, &Consumer::finished, m_writerThread, &QThread::deleteLater);
    //m_writerThread->start(QThread::LowPriority);
    m_writerThread->start();
    
    QString hello("HELLO");
    QByteArray out = hello.toLatin1();
    m_socket->write(out);
}

void Consumer::readBlock()
{
     //std::cout << "Got readyRead! signal from socket, bytes available = " << m_socket->bytesAvailable() << std::endl;
     QByteArray block = m_socket->readAll();
     frameBuffer->push(block);
}

void Consumer::disconnected()
{
    std::cout << "Consumer::disconnected()!" << std::endl;
    connectedToHost = false;
    m_socket->disconnectFromHost();
    m_socket->deleteLater();
    m_writerThread->wait();
    emit finished();
}

void Consumer::catchSocketError()
{
    QString errorString = m_socket->errorString();
    std::cout << "Socket Error: " << errorString.toUtf8().constData() << std::endl;
}

