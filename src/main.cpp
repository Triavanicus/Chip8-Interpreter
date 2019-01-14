#include "Chip8.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>

int main(int argc, char* args[])
{
    if (argc <= 1)
    {
        std::cout << "Too few arguments, needs filename." << std::endl;
        return 0;
    }

    char* filename = args[1];
    sf::FileInputStream file = {};
    if (!file.open(filename))
    {
        std::cout << "File not found: " << filename << std::endl;
        return 0;
    }

    uint dataSize = 0x1000 - 0x200;
    if (file.getSize() > dataSize)
    {
        std::cout << "File to large, max file size is " << dataSize
                  << ", file size is " << file.getSize() << std::endl;
    }

    u8* data = new u8[dataSize]();
    file.read(data, file.getSize());

    const int windowWidth = 1280;
    const int windowHeight = 720;
    const int chip8Width = 64;
    const int chip8Height = 32;

    sf::RenderWindow window = {};
    window.create(
        sf::VideoMode(windowWidth, windowHeight), "Chip8 Interpreter");

    auto chip8 = Chip8();
    chip8.load(data, dataSize);

    auto chip8Clock = sf::Clock();
    auto chip8TimerClock = sf::Clock();

    auto pixelWidth = (float) windowWidth / chip8Width;
    auto pixelHeight = (float) windowHeight / chip8Height;
    sf::RectangleShape pixel = sf::RectangleShape(
        sf::Vector2f(pixelWidth, pixelHeight));

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); }
        }

        if (chip8Clock.getElapsedTime().asMicroseconds() >= 1000)
        {
            chip8.cycle();
            chip8Clock.restart();
        }

        if (chip8TimerClock.getElapsedTime().asMilliseconds() >= 16)
        {
            if (chip8.soundTimer > 0) chip8.soundTimer--;
            if (chip8.delayTimer > 0) chip8.delayTimer--;

            chip8TimerClock.restart();
        }

        if (chip8.drawFlag)
        {
            window.clear();
            {
                for (auto y = 0; y < chip8Height; y++)
                    for (auto x = 0; x < chip8Width; x++)
                    {
                        if (chip8.pixels[x + y * chip8Width] > 0)
                        {
                            pixel.setPosition(x * pixelWidth, y * pixelHeight);
                            window.draw(pixel);
                        }
                    }
            }
            window.display();
            chip8.drawFlag = false;
        }
    }

    delete[] data;
    return 0;
}
