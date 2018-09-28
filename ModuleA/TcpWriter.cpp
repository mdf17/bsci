#include "TcpWriter.h"

#include <QHostAddress>

TcpWriter::TcpWriter(qintptr socketDescriptor, QObject *parent) 
    : QObject(parent), 
      m_dataQueue(), 
      m_socketDescriptor(socketDescriptor), 
      m_tcpPacketSize(TCP_PACKET_SIZE),
      m_connectedToHost(false)
{
    m_id = QString::number(socketDescriptor);
}

TcpWriter::~TcpWriter()
{
    delete m_socket;
}

void TcpWriter::init()
{
    m_socket = new QTcpSocket;

    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    QObject::connect(m_socket, &QTcpSocket::disconnected, this, &TcpWriter::disconnect);
    QObject::connect(m_socket, &QTcpSocket::readyRead, this, &TcpWriter::readyRead, Qt::DirectConnection);

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }

    m_connectedToHost = true;

    Config * config = Config::instance();

    std::string sTcpPacketSize;
    if (config->lookup("tcpPacketSize", sTcpPacketSize)) {
        m_tcpPacketSize = atoi(sTcpPacketSize.c_str());
    }
}

void TcpWriter::disconnect()
{
    std::cout << "TCPWriter: disconnected!" << std::endl;
    m_connectedToHost = false;
    m_socket->close();
    emit finished();
    emit disconnected(m_id);
}

void TcpWriter::readyRead()
{
    QByteArray data = m_socket->readAll();

    write();
}

void TcpWriter::enqueueChecksum(ChecksumT checksum)
{
    m_dataQueue.push(checksum);
}

void TcpWriter::write()
{
    ChecksumT checksum;
    int framesPerBlock = m_tcpPacketSize / sizeof(ChecksumT);
    m_dataQueue.setQueueBudget(100*framesPerBlock);
    while (m_connectedToHost) {
        while (m_dataQueue.size() < framesPerBlock)
            ;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStreamVersion);

        for (int i = 0; i < framesPerBlock; ++i) {
            m_dataQueue.getElementNonblocking(checksum);

            for (unsigned int i = 0; i < NUM_CHANNELS; ++i) {
                out << checksum.sum[i];
            }
            out << checksum.timestamp;
            //std::cout << "t=" << checksum.timestamp << ": " << checksum.sum[0] << ", " << checksum.sum[7] << std::endl;
        }

        int nBytes = m_socket->write(block);
        if (nBytes == 0)
            std::cout << "Failed to write output" << std::endl;
        m_socket->flush();
    }
}
