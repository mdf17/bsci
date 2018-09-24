QT += core network widgets
# requires(qtConfig(filedialog))

CONFIG += c++11

TEMPLATE = subdirs
SUBDIRS = ModuleA ModuleB
#ModuleA.depends = common
#ModuleB.depends = common


INCLUDE_PATH += /usr/include/qt5

# install
target.path = .
INSTALLS += target
