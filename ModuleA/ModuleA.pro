QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

CONFIG += debug

TARGET = ModuleA

INCLUDEPATH += ../common

HEADERS += Threads.h Producer.h

SOURCES += main.cpp Threads.cpp Producer.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
