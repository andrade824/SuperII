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

/**
 * How many frames per second to update the video hardware at.
 */
static constexpr int FPS = 60;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    EmulatorCore emulator;

    MainWindow window(emulator);
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
