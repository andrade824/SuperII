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

bool LoadRom(EmulatorCore &emu, const std::string &path);

/**
 * How many frames per second to update the video hardware at.
 */
static constexpr int FPS = 60;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EmulatorCore emulator;
    //LoadRom(emulator, "asm/apple2_rom.bin");

    MainWindow window(emulator.GetVideo());
    window.show();

    /**
     * Keep processing GUI events and running the emulator until the main
     * window is closed.
     */
    sf::Clock clock;
    while(window.isVisible())
    {
        window.SetStatusText(QString("FPS: %1").arg(
            static_cast<int>(1.0f / clock.getElapsedTime().asSeconds())));

        clock.restart();

        emulator.RunFrame(FPS);
        app.processEvents();

        sf::Time delta = sf::seconds(1.0f / FPS) - clock.getElapsedTime();

        if(delta.asSeconds() > 0)
            sf::sleep(delta);
    }

    return 0;
}

bool LoadRom(EmulatorCore &emu, const std::string &path)
{
    bool file_loaded = true;
    uint32_t filesize = 0;

    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        filesize = file.tellg();
        assert(filesize == EmulatorCore::ROM_SIZE);

        uint8_t file_data[EmulatorCore::ROM_SIZE];

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(file_data), filesize);

        emu.LoadRom(file_data);
    } else {
        file_loaded = false;
        std::cout << "Failed to open ROM at '" << path << "'" << std::endl;
    }

    file.close();

    return file_loaded;
}
