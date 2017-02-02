#include "Cpu.h"
#include "instrs_6502.h"
#include "MainWindow.h"
#include "Memory.h"
#include "SystemBus.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include <QApplication>

bool LoadExecutable(Memory &mem, const std::string &path);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*while(1)
    {
        sf::Clock;
        a.processEvents();
        Emulator.RunFrame();
        check clock
        sleep if i have to
    }*/

//    Memory mem(0, 0xFFFF, false);
//    SystemBus bus;

//    bus.Register(&mem);

//    /**
//     * Run test.bin at 0x4000, address 0x0210 should be 0xFF for passing.
//     *
//     * Run ehbasic.bin at 0xC000.
//     *
//     * To change the offset, you'll need to modify Memory.cpp's LoadMemory
//     * method.
//     */
//    if(LoadExecutable(mem, "asm/test.bin")) {
//        Cpu cpu(bus, instrs_6502);
//        cpu.Execute(10000000);

//        unsigned int value = bus.Read(0x210);
//        std::cout << "Value: 0x" << std::hex << value << std::endl;
//    }

    return a.exec();
}

bool LoadExecutable(Memory &mem, const std::string &path)
{
    bool file_loaded = true;
    uint32_t filesize = 0;

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        filesize = file.tellg();
        assert(filesize != 0);

        uint8_t *file_data = new uint8_t[filesize];

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(file_data), filesize);

        mem.LoadMemory(file_data, filesize);

        delete file_data;
    } else {
        file_loaded = false;
        std::cout << "Failed to open executable '" << path << "'" << std::endl;
    }

    file.close();

    return file_loaded;
}
