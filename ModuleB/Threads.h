#ifndef THREADS_H
#define THREADS_H

#include <iostream>
#include <fstream>
#include <bitset>
#include <memory>

#include <QtCore/QObject>
#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>
#include "Common.h"



/*******************************************************************
 * class TcpReaderThread
 *
 * Inherits: QThread
 * 
 * manages TCP socket
 *******************************************************************/
//class TcpReaderThread : public QThread
//{
//    Q_OBJECT
//
//  public:
//    TcpReaderThread(QHostAddress ipAddress, quint16 port, QObject *parent);
//    ~TcpReaderThread();
//
//    void run() override;
//
//  signals:
//    void error(QTcpSocket::SocketError socketError);
//    void blockReady(const char * block);
//
//  public slots:
//    void disconnect();
//    void readyRead();
//  
//  private:
//    QHostAddress m_ipAddress;
//    quint16 m_port;
//    QMutex mtx;
//    //QWaitCondition cond;
//
//    QTcpSocket * m_socket;
//    int socketDescriptor;
//};

class FileWriterThread : public QThread
{
    Q_OBJECT

  public:
    FileWriterThread(const std::string id, std::shared_ptr<ThreadSafeQueue<QByteArray>> buffer, QObject *parent);
    ~FileWriterThread();

    void run() override;

    void processNextBlock();

    std::shared_ptr<ThreadSafeQueue<QByteArray>> frameBuffer;

  private:
    std::string m_id;

    QMutex mtx;
    //QWaitCondition cond;

    std::ofstream m_outputFile;

};

#endif
