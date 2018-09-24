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

    void init();
    void run(); // override;

    std::shared_ptr<ThreadSafeQueue<QByteArray>> frameBuffer;

  signals:
    void error(QTcpSocket::SocketError socketError);
    void quit();

  public slots:
    void readBlock();
    void disconnected();
    void socketChanged();
    void catchSocketError();

  private:
    // Allow multiple Consumers to exist
    std::string m_id;

    // Host IP Address and port number
    QHostAddress m_ipAddress;
    quint16 m_port;

    // connect to A via socket
    // receive checksums for all channels
    QTcpSocket *m_socket;

    // output data to file (stdout)
    FileWriterThread *m_writer;

    bool connectedToHost;
};

#endif
