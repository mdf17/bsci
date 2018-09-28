QT += core

CONFIG += c++11

TARGET = common

TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += ../common

HEADERS += Config.h Common.h

SOURCES += Config.cpp 

INCLUDE_PATH += /usr/include/qt5

# install
target.path = ../bin
INSTALLS += target
