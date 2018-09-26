QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

CONFIG += debug

TARGET = ModuleA

INCLUDEPATH += ../common

HEADERS += FileReader.h FrameParser.h TcpWriter.h Producer.h

SOURCES += main.cpp FileReader.cpp FrameParser.cpp TcpWriter.cpp Producer.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = ../bin
INSTALLS += target
