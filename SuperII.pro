QT += core gui widgets

CONFIG += c++11 warn_on
OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = build

TARGET = SuperII

TEMPLATE = app

win32 {
    LIBS += -LC:\Developer\SFML\lib
    INCLUDEPATH += C:\Developer\SFML\include
}

LIBS += -lsfml-audio -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system

SOURCES += main.cpp \
    instrs_6502.cpp \
    Cpu.cpp \
    Memory.cpp \
    SystemBus.cpp \
    MainWindow.cpp \
    Video.cpp \
    QSfmlCanvas.cpp

OTHER_FILES += \
    TODO.txt \
    asm/*

HEADERS += \
    instrs_6502.h \
    Cpu.h \
    IMemoryMapped.h \
    Memory.h \
    SystemBus.h \
    MainWindow.h \
    Video.h \
    QSfmlCanvas.h

FORMS += \
    MainWindow.ui
