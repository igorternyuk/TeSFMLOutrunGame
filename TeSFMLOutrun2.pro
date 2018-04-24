TEMPLATE = app
CONFIG += c++1z
CONFIG -= app_bundle
CONFIG -= qt
LIBS += `pkg-config --libs sfml-all`
SOURCES += main.cpp \
    game.cpp

HEADERS += \
    game.hpp \
    resourcemanager.hpp

DISTFILES += \
    resourcemanager.impl
