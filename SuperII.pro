QT += core
QT -= gui

CONFIG += c++11 warn_on
OBJECTS_DIR = build
MOC_DIR = build
DESTDIR = build

TARGET = SuperII
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    instrs_6502.cpp \
    Cpu.cpp \
    Memory.cpp \
    SystemBus.cpp

OTHER_FILES += \
    TODO.txt \
    asm/*

HEADERS += \
    instrs_6502.h \
    Cpu.h \
    IMemoryMapped.h \
    Memory.h \
    SystemBus.h
