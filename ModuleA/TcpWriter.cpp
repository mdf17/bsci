#include "TcpWriter.h"

#include <QHostAddress>

TcpWriter::TcpWriter(qintptr socketDescriptor, QObject *parent) : QObject(parent), m_socketDescriptor(socketDescriptor), m_framesPerBlock(TCP_PACKET_SIZE/sizeof(ChecksumT))
{
    std::cout << "TcpWriter()" << std::endl;
    std::cout << "Frames per block: " << m_framesPerBlock << std::endl;
}

TcpWriter::~TcpWriter()
{
    std::cout << "~TcpWriter()" << std::endl;
}

void TcpWriter::init()
{
    std::cout << "TcpWriter::init()" << std::endl;

    m_socket = new QTcpSocket;

    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpWriter::disconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpWriter::readyRead, Qt::DirectConnection);

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }
}

void TcpWriter::disconnected()
{
    std::cout << "TCPWriterThread: disconnected!" << std::endl;
    m_socket->deleteLater();
    exit(0);
}

void TcpWriter::readyRead()
{
    std::cout << "Got readyRead! signal" << std::endl;
    QByteArray data = m_socket->readAll();

    write();
}

void TcpWriter::enqueueChecksum(ChecksumT checksum)
{
    std::cout << "Grabbing checksum! " << checksum.sum << ", " << checksum.timestamp << std::endl;
    m_dataQueue.push_back(checksum);
}

void TcpWriter::write()
{
    std::cout << "TcpWriter::write()" << std::endl;
    while (true) {
        while (m_dataQueue.size() < m_framesPerBlock)
            ;//std::cout << "Data Queue has " << m_dataQueue.size() << " elements" << std::endl;

        std::cout << "TcpWriter: Got ChecksumT from queue" << std::endl;
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStreamVersion);

        for (int i = 0; i < m_framesPerBlock; ++i) {
            ChecksumT checksum = m_dataQueue.dequeue();
            out << checksum.sum;
            out << checksum.timestamp;
        }

        std::cout << "TcpWriter: Connected to socket " << m_socket->socketDescriptor() << std::endl;
        std::cout << "TcpWriter: Socket Address: " << to_string(m_socket->peerAddress().toString()) << std::endl;
        std::cout << "TcpWriter: Socket Name: " << to_string(m_socket->peerName()) << std::endl;
        std::cout << "TcpWriter: Socket Port: " << m_socket->peerPort() << std::endl;

        int nBytes = m_socket->write(block);
        if (nBytes == 0)
            std::cout << "Failed to write output" << std::endl;
        m_socket->flush();
        std::cout << "Wrote " << nBytes << " bytes to socket" << std::endl;
    }
}
