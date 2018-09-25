#include "ConnectionManager.h"

#include <QDateTime>

static const int MaxConnections = MAX_THREADS;

Q_GLOBAL_STATIC(ConnectionManager, connectionManager)

ConnectionManager *ConnectionManager::instance()
{
    return connectionManager();
}

bool ConnectionManager::canAddConnection() const
{
    return (connections.size() < MaxConnections);
}

void ConnectionManager::addConnection(QTcpSocket *client)
{
    connections << client;
}

void ConnectionManager::removeConnection(QTcpSocket *client)
{
    connections.remove(client);
}

int ConnectionManager::maxConnections() const
{
    return MaxConnections;
}

QByteArray ConnectionManager::clientId() const
{
    if (id.isEmpty()) {
        // Generate peer id
        qint64 startupTime = QDateTime::currentSecsSinceEpoch();

        id += QString::asprintf("-QT%04x-", QT_VERSION >> 8).toLatin1();
        id += QByteArray::number(startupTime, 10);
        id += QByteArray(20 - id.size(), '-');
    }
    return id;
}
