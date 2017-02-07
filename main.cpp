#include "EmulatorCore.h"
#include "MainWindow.h"
#include "Video.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <QApplication>

bool LoadExecutable(Memory &mem, const std::string &path);

/**
 * How many frames per second to update the video hardware at.
 */
static constexpr float FPS = 60.0f;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EmulatorCore emulator;

    MainWindow window(emulator.GetVideo());
    window.show();

    /**
     * Keep processing events and running the emulator until the main window
     * is closed.
     */
    sf::Clock clock;
    while(window.isVisible())
    {
        clock.restart();
        app.processEvents();
        emulator.RunFrame(FPS);

        sf::Time delta = sf::seconds(1.0f / FPS) - clock.getElapsedTime();

        if(delta.asSeconds() > 0)
            sf::sleep(delta);
    }

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

    return 0;
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
