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

    QString ipAddress (sIpAddress);
    quint16 port (atoi(sPort));
    std::string ipAddressUtf8 = ipAddress.toUtf8().constData();

    std::cout << "Connecting to IP Address: " << ipAddressUtf8 << " Port " << port << std::endl;

    //std::vector<Consumer *> consumers;

    //for (unsigned int i = 0; i < MAX_THREADS; ++i) {
        
        //std::string id = std::to_string(i);


        QThread * thread = new QThread;
        Consumer * consumer = new Consumer("0", QHostAddress(ipAddress), port); 
        consumer->moveToThread(thread);
        QObject::connect(thread, SIGNAL(started()), consumer, SLOT(init()));
        thread->start();
        //consumers.push_back(consumer);
    //}

    std::cout << "exec()" << std::endl;

    return app.exec();
}
