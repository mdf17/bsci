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
    m_dataQueue.reset(new ThreadSafeQueue<FrameT>(MAX_QUEUE_SIZE));

    //QNetworkConfigurationManager manager;
    //if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
    //    // Get saved network configuration
    //    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    //    settings.beginGroup(QLatin1String("QtNetwork"));
    //    const QString id = settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
    //    settings.endGroup();

    //    // If the saved network configuration is not currently discovered use the system default
    //    QNetworkConfiguration config = manager.configurationFromIdentifier(id);
    //    if ((config.state() & QNetworkConfiguration::Discovered) !=
    //        QNetworkConfiguration::Discovered) {
    //        config = manager.defaultConfiguration();
    //    }

    //    m_networkSession = new QNetworkSession(config, this);
    //    connect(m_networkSession, &QNetworkSession::opened, this, &Producer::sessionOpened);

    //    std::cout << "Opening network session." << std::endl;
    //    m_networkSession->open();
    //} else {
    //    sessionOpened();
    //} 

    m_server = new ServerThread(this->thread());

    //connect(this, SIGNAL(checksumReady(ChecksumT)), m_server, SLOT(checksumReady(ChecksumT)), Qt::DirectConnection);

    m_rateController = new RateController;
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

    delete m_rateController;
}

bool Producer::connectToDataStream(const std::string& inputFile)
{
    std::cout << "Producer::connectToDataStream()" << std::endl;
    std::ifstream in(inputFile, std::ifstream::ate | std::ifstream::binary);
    m_streamSize = in.tellg(); 
    m_numFrames = m_streamSize / m_frameSize;
    in.close();

    m_dataStream.open(inputFile.c_str(), std::ios::in | std::ios::binary);

    m_parser = new ComputeThread(m_numFrames, m_dataQueue, this);
    //connect(m_parser, &ComputeThread::checksumReady, this, &Producer::forwardChecksumReady, Qt::DirectConnection);
    connect(m_parser, SIGNAL(checksumReady(ChecksumT)), m_server, SLOT(checksumReady(ChecksumT)), Qt::DirectConnection);
    //connect(m_parser, SIGNAL(checksumReady(ChecksumT)), m_server, SLOT(checksumReady(ChecksumT)));
    connect(this, &Producer::quit, m_parser, &ComputeThread::deleteLater);

    m_parser->start();

    return m_dataStream.is_open();
}

void Producer::sessionOpened()
{
    //std::cout << "Producer::sessionOpened()" << std::endl;
    //// Save the used configuration
    //if (m_networkSession) {
    //    QNetworkConfiguration config = m_networkSession->configuration();
    //    QString id;
    //    if (config.type() == QNetworkConfiguration::UserChoice)
    //        id = m_networkSession->sessionProperty(QLatin1String("UserChoiceConfiguration")).toString();
    //    else
    //        id = config.identifier();
    //    
    //    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    //    settings.beginGroup(QLatin1String("QtNetwork"));
    //    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    //    settings.endGroup();
    //}

    //if (!listen()) {
    //    std::cout << "Unable to start the server: " << std::endl;
    //    return;
    //}

    //QString ipAddress;
    //QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    //// use the first non-localhost IPv4 address
    //for (int i = 0; i < ipAddressesList.size(); ++i) {
    //    if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
    //        ipAddress = ipAddressesList.at(i).toString();
    //        std::cout << "IP Address is not localhost!" << std::endl;
    //        break;
    //    }
    //}

    //if (ipAddress.isEmpty())
    //        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    //std::string ipAddressUtf8 = ipAddress.toUtf8().constData();
    //std::cout << "The server is running on IP " << ipAddressUtf8 << " port " << serverPort() << std::endl;

    //if (isListening())
    //    std::cout << "The server is listening..." << std::endl;

    //std::cout << "Max pending connections: " << maxPendingConnections() << std::endl;
}

// timestamp passed down from event loop
void Producer::readPacket(const double& timestamp)
{
    unsigned int pos = m_bytesRead % m_streamSize;
    //std::cout << "Producer::readPacket(" << std::setprecision(10) << timestamp << ")" << std::endl;
    // streampos pointer wraps around to beginning of file
    //std::cout << "Stream Size = " << m_streamSize << ", bytes read = " << m_bytesRead << std::endl;
    m_dataStream.seekg(pos); 
    //std::cout << "seekg = " << pos << std::endl;
    // read a frame
    m_dataStream.read(m_packet.data(), m_frameSize);
    //std::cout << "read " << m_frameSize << " bytes." << std::endl;
    // timestamp the data
    m_dataQueue->push_back(FrameT(m_packet, timestamp));

    //std::cout << "push " << m_dataQueue->size() << std::endl;
    //std::cout << "push frame to queue" << std::endl;
    m_bytesRead += m_frameSize;
    m_frameNumber++;
    //std::cout << "done" << std::endl;
}

