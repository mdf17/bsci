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

    std::string id;
    if (argc > 1) {
        id = std::string(argv[1]);
    }

    Consumer c(id);
    c.init();
    c.show();

    return app.exec();
}
