QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

CONFIG += debug

TARGET = ModuleA

INCLUDEPATH += ../common

HEADERS += Threads.h Server.h Producer.h

SOURCES += main.cpp RateController.cpp Threads.cpp Server.cpp Producer.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
