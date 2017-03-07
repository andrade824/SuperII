QT += core gui widgets multimedia

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

LIBS += -lsfml-graphics -lsfml-network -lsfml-window -lsfml-system -lsfml-audio

SOURCES += main.cpp \
    instrs_6502.cpp \
    Cpu.cpp \
    Memory.cpp \
    SystemBus.cpp \
    MainWindow.cpp \
    Video.cpp \
    character_rom.cpp \
    EmulatorCore.cpp \
    Keyboard.cpp \
    Speaker.cpp \
    SettingsDialog.cpp \
    DisassemblyWindow.cpp \
    CpuRegistersWindow.cpp \
    ViewMemoryWindow.cpp \
    DiskImage.cpp

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
    character_rom.h \
    EmulatorCore.h \
    Keyboard.h \
    Speaker.h \
    IState.h \
    SettingsDialog.h \
    DisassemblyWindow.h \
    CpuRegistersWindow.h \
    ViewMemoryWindow.h \
    DiskImage.h

FORMS += \
    MainWindow.ui \
    SettingsDialog.ui \
    DisassemblyWindow.ui \
    CpuRegistersWindow.ui \
    ViewMemoryWindow.ui
