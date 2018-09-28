QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

TARGET = ModuleB

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

HEADERS += FileWriter.h Consumer.h

SOURCES += main.cpp FileWriter.cpp Consumer.cpp

INCLUDE_PATH += /usr/include/qt5

# install
target.path = ../bin
INSTALLS += target
