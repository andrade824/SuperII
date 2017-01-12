QT += core
QT -= gui

CONFIG += c++11

TARGET = SuperII
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    memory.cpp \
    cpu.cpp \
    instrs_6502.cpp

DISTFILES += \
    TODO.txt \
    asm/a.o65 \
    asm/test.s

HEADERS += \
    memory.h \
    cpu.h \
    ireadwrite.h \
    instrs_6502.h
