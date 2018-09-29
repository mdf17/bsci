#ifndef CONSUMER_H
#define CONSUMER_H

#include <QThread>
#include <QHostAddress> 
#include <QTcpSocket>
#include <QDialog>
#include "FileWriter.h"

class QCloseEvent;
class QLabel;

// Module B - Consumer class
// Can be many Consumers for a single Producer
class Consumer : public QDialog
{
    Q_OBJECT

  public:
    Consumer(const std::string id, QWidget *parent = 0);
    ~Consumer();

    SharedQueue<QByteArray> frameBuffer;

  protected:
    void closeEvent(QCloseEvent *event) override;

  signals:
    void error(QTcpSocket::SocketError socketError);
    void finished();
    void signalClosed();

  public slots:
    void init();
    void readBlock();
    void disconnected();
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
    FileWriter *m_writer;
    QThread *m_writerThread;

    bool connectedToHost;

    QLabel *idLabel;
    QLabel *connectionStatusLabel;
};

#endif
