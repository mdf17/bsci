#include <iomanip>

#include "Threads.h"




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
