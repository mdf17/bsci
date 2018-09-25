#include <cstdlib>
#include <numeric>
#include <iostream>
#include <iomanip>

#include <QtNetwork/QNetworkConfigurationManager>
#include <QSettings>

#include "Producer.h"

Q_GLOBAL_STATIC(Producer, producer)

Producer::Producer()
  : m_frameNumber(0),
    m_frameTime(0.), 
    m_frameRate(FRAME_RATE), 
    m_tcpPacketSize(TCP_PACKET_SIZE),
    m_maxConnections(MAX_THREADS),
    m_streamSize(0),
    m_bytesRead(0),
    m_frameSize(FRAME_SIZE),
    m_numFrames(0)
{
    std::cout << "Producer()" << std::endl;
    m_inputDataQueue.reset(new ThreadSafeQueue<FrameT>(MAX_INPUT_QUEUE_SIZE));
    m_outputDataQueue.reset(new ThreadSafeQueue<ChecksumT>(MAX_OUTPUT_QUEUE_SIZE));

    m_reader = new FileReaderThread(m_inputDataQueue, this);
    connect(this, &Producer::quit, m_reader, &FileReaderThread::deleteLater);
}

Producer * Producer::instance()
{
    return producer();
}

Producer::~Producer()
{
    std::cout << "~Producer()" << std::endl;
    // Tell writers to close their TCP Sockets
    emit disconnect();

    m_dataStream.close();

    // Tell child threads to die
    emit quit();
}

void Producer::init()
{
    std::cout << "Producer::init()" << std::endl; 

    // Attempt to start the server
    if (!listen()) {
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
    

    //std::string ipAddressUtf8 = ipAddress.toUtf8().constData();
    std::cout << "The server is running on IP: " << serverAddress().toString().toUtf8().constData() << " " << serverPort() << std::endl;

    // Start the File Reader
    if (m_reader->connectToDataStream("com_mod_input.bin")) {
        m_parser = new ComputeThread(m_numFrames, m_inputDataQueue, m_outputDataQueue, this);
        connect(this, &Producer::quit, m_parser, &ComputeThread::deleteLater);
    }
    m_parser->start();
    m_reader->start();

    // Start accepting new connections
    while (true) {
        waitForNewConnection(10000);
        //if (hasPendingConnections()) {
        //    std::cout << "Producer: server has pending connections!" << std::endl; 
        //} else {
        //    std::cout << "Producer: no pending connections :(" << std::endl;
        //}
    }
}

void Producer::incomingConnection(qintptr socketDescriptor)
{
    std::cout << "Incoming connection on " << socketDescriptor << " !" << std::endl;
    
    //QTcpSocket * socket = new QTcpSocket;

    //socket->setSocketDescriptor(socketDescriptor);
    //   
    //int nRet = socket->write("HELLO");
    //std::cout << "Wrote " << nRet << " bytes to socket" << std::endl;

    //socket->close();

    //delete socket;

    TcpWriterThread *t = new TcpWriterThread(socketDescriptor, m_outputDataQueue, this);
    connect(this, &Producer::quit, t, &TcpWriterThread::deleteLater);
    t->start();
}

