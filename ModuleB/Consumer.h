#ifndef CONSUMER_H
#define CONSUMER_H

#include <string>


#include "Threads.h"

// Module B - Consumer class
// Can be many Consumers for a single Producer
class Consumer : public QObject
{
    Q_OBJECT

  public:
    Consumer(const std::string id, const QHostAddress hostAddress, const quint16 port);
    ~Consumer();

    // connect to A via socket
    //
    // receive checksums for all channels
    //
    // output data to file (stdout)

    void init();
    void run();

    std::shared_ptr<ThreadSafeQueue<QByteArray>> frameBuffer;

  signals:
    void error(QTcpSocket::SocketError socketError);
    void disconnect();
    void quit();

  private:
    std::string m_id;

    QHostAddress m_ipAddress;
    quint16 m_port;

    QTcpSocket *m_socket;
    FileWriterThread *m_writer;
};

#endif
