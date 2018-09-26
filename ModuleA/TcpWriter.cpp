#include "TcpWriter.h"


TcpWriter::TcpWriter(qintptr socketDescriptor, QObject *parent) : QObject(parent), m_socketDescriptor(socketDescriptor)
{
    std::cout << "TcpWriter()" << std::endl;
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
    QByteArray data = m_socket->readAll();

    write();
}

void enqueueChecksum(ChecksumT checksum)
{
    m_dataQueue.push_back(checksum);
}

void TcpWriter::write()
{
    while (true) {
        while (m_dataQueue->size() < m_framesPerBlock)
            ;

        std::cout << "TcpWriter: Got ChecksumT from queue" << std::endl;
        QByteArray block;
        QDataStream out(&block, QIODevice::ReadWrite);
        out.setVersion(QDataStreamVersion);

        for (int i = 0; i < m_framesPerBlock; ++i) {
            ChecksumT checksum = m_dataQueue->pop_front();
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
