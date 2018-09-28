QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11 strict_c++
CONFIG += debug

TEMPLATE = subdirs
SUBDIRS = common ModuleA ModuleB
ModuleA.depends = common
ModuleB.depends = common


INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
