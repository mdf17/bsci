QT += core network widgets

CONFIG += c++11

TARGET = ModuleA

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

HEADERS += FileReader.h FrameParser.h TcpWriter.h Server.h

SOURCES += main.cpp FileReader.cpp FrameParser.cpp TcpWriter.cpp Server.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
