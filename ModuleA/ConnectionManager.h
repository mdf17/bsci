#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H


#include <QByteArray>
#include <QSet>
#include <QtNetwork/QTcpSocket>

#include "Common.h"

class ConnectionManager
{
public:
    static ConnectionManager *instance();

    bool canAddConnection() const;
    void addConnection(QTcpSocket *connection);
    void removeConnection(QTcpSocket *connection);
    int maxConnections() const;
    QByteArray clientId() const;

 private:
    QSet<QTcpSocket *> connections;
    mutable QByteArray id;
};

#endif
