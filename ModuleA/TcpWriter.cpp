#include "TcpWriter.h"
#include "Config.h"

#include <QHostAddress>

TcpWriter::TcpWriter(qintptr socketDescriptor, QObject *parent) : QObject(parent), m_dataQueue(), m_socketDescriptor(socketDescriptor), m_framesPerBlock(TCP_PACKET_SIZE/sizeof(ChecksumT))
{
}

TcpWriter::~TcpWriter()
{
    std::cout << "~TcpWriter()" << std::endl;
    delete m_socket;
}

void TcpWriter::init()
{
    m_socket = new QTcpSocket;

    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    connect(m_socket, &QTcpSocket::disconnected, this, &TcpWriter::disconnect);
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpWriter::readyRead, Qt::DirectConnection);

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        emit error(m_socket->error());
        return;
    }

    connectedToHost = true;

    Config * config = Config::instance();

    std::string sTcpPacketSize;
    if (config->lookup("tcpPacketSize", sTcpPacketSize)) {
        int tcpPacketSize = atoi(sTcpPacketSize.c_str());
        m_framesPerBlock = tcpPacketSize / sizeof(ChecksumT);
    }
    m_dataQueue.setQueueBudget(100*m_framesPerBlock);
}

void TcpWriter::disconnect()
{
    std::cout << "TCPWriter: disconnected!" << std::endl;
    connectedToHost = false;
    m_socket->close();
}

void TcpWriter::readyRead()
{
    QByteArray data = m_socket->readAll();

    write();
}

void TcpWriter::enqueueChecksum(ChecksumT checksum)
{
    std::cout << "enqueueChecksum" << std::endl;
    if (connectedToHost)
        m_dataQueue.push_back(checksum);
}

void TcpWriter::write()
{
    while (connectedToHost) {
        while (m_dataQueue.size() < m_framesPerBlock)
            ;

        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        out.setVersion(QDataStreamVersion);

        for (int i = 0; i < m_framesPerBlock; ++i) {
            ChecksumT checksum = m_dataQueue.pop_front();

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
