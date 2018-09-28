#ifndef PRODUCER_H
#define PRODUCER_H

#include <QDialog>

#include "Server.h"
#include "TcpWriter.h"
#include "FileReader.h"
#include "FrameParser.h"

class QCloseEvent;
class QLabel;

class Producer : public QDialog
{
    Q_OBJECT

  public:
    Producer(QWidget *parent = 0);

    SharedQueue<PacketT> inputDataQueue;
    SharedQueue<ChecksumT> outputDataQueue;

    void init();

  protected:
    void closeEvent(QCloseEvent *event) override;

  signals:
    void signalClosed();
    void connectionClosed();

  public slots:
    void forwardChecksum(ChecksumT checksum);
    void incomingConnection(qintptr socketDescriptor);
    void closeConnection(QString id);

  private slots:
    void updateDialog();

  private:

    Server *server;

    QThread *readerThread;
    FileReader *reader;

    QThread *parserThread;
    FrameParser *parser;

    QList<TcpWriter *> tcpWriters;
    QList<QThread *> tcpWriterThreads;

    QLabel *serverStatusLabel;
    QLabel *connectionStatusLabel;

};

#endif
