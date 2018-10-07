QT += core network widgets

CONFIG += c++11

TARGET = testApp

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

SOURCES += testModuleB.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
