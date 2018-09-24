#include <QtWidgets/QApplication>
#include <QtCore>

#include "Producer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Producer::instance()->init();

    return app.exec();
}
