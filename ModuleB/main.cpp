#include <cstdlib>

#include <QtWidgets/QApplication>
#include <QtCore>
#include <QObject>

#include "Config.h"
#include "Consumer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Config::instance()->init("../common/configParams.txt");

    int numConsumers = 1;
    if (argc > 1) {
        numConsumers = atoi(argv[1]);
    }

    QList<Consumer *> consumers;
    QList<QThread *> threads;

    for (int i = 0; i < numConsumers; i++) {
        QThread * thread = new QThread;
        Consumer * consumer = new Consumer(std::to_string(i));
        consumer->moveToThread(thread);
        QObject::connect(thread, SIGNAL(started()), consumer, SLOT(init()));
        consumers << consumer;
        threads << thread;
        thread->start();
    }

    return app.exec();
}
