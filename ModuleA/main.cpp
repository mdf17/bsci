#include <QApplication>
#include <QtCore>

#include "Producer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Config::instance()->init("../common/configParams.txt");

    QApplication::setApplicationDisplayName(Server::tr("BSCI Server"));

    Producer p;
    p.init();
    p.show();

    return app.exec();
}
