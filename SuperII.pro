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
    memory.cpp \
    cpu.cpp \
    instrs_6502.cpp

OTHER_FILES += \
    TODO.txt \
    asm/*

HEADERS += \
    memory.h \
    cpu.h \
    ireadwrite.h \
    instrs_6502.h
