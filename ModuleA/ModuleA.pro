QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

CONFIG += debug

TARGET = ModuleA

INCLUDEPATH += ../common

HEADERS += Config.h FileReader.h FrameParser.h TcpWriter.h Server.h

SOURCES += main.cpp Config.cpp FileReader.cpp FrameParser.cpp TcpWriter.cpp Server.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = ../bin
INSTALLS += target
