QT += core network widgets

CONFIG += c++11

TARGET = ModuleA

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

#DEFINES += TRUTH_FILE

HEADERS += FileReader.h FrameParser.h TcpWriter.h Server.h Producer.h

SOURCES += main.cpp FileReader.cpp FrameParser.cpp TcpWriter.cpp Server.cpp Producer.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
