#include <cstdlib>

#include <QtWidgets/QApplication>
#include <QtCore>
#include <QObject>

#include "Consumer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    const char * sIpAddress = argv[1];

    const char * sPort = argv[2];

    int numConsumers = 1;
    if (argc > 3) {
        numConsumers = atoi(argv[3]);
    }
    QString ipAddress (sIpAddress);
    quint16 port (atoi(sPort));
    std::string ipAddressUtf8 = ipAddress.toUtf8().constData();

    std::cout << "Connecting to IP Address: " << ipAddressUtf8 << " Port " << port << std::endl;

    QList<Consumer *> consumers;
    QList<QThread *> threads;

    for (int i = 0; i < numConsumers; i++) {
        QThread * thread = new QThread;
        Consumer * consumer = new Consumer(std::to_string(i), QHostAddress(ipAddress), port); 
        consumer->moveToThread(thread);
        QObject::connect(thread, SIGNAL(started()), consumer, SLOT(init()));
        consumers << consumer;
        threads << thread;
        thread->start();
    }

    return app.exec();
}
