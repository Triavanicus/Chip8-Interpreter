#include "Chip8.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>
void drawPixels(sf::RenderWindow& window,
    sf::RectangleShape& pixel,
    u8 pixels[],
    int width,
    int height,
    float pixelWidth,
    float pixelHeight)
{
    for (auto y = 0; y < height; y++)
        for (auto x = 0; x < width; x++)
        {
            if ((pixels[(x / 8) + (y * 8)] >> (7 - (x % 8))) & 0b1)
            {
                pixel.setPosition(x * pixelWidth, y * pixelHeight);
                window.draw(pixel);
            }
        }
}

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

    u8 oldChip8Pixels[(64 / 8 * 32)] = {};

    sf::Keyboard::Key keys[0x10] = {
        sf::Keyboard::X,    // 0
        sf::Keyboard::Num1, // 1
        sf::Keyboard::Num2, // 2
        sf::Keyboard::Num3, // 3
        sf::Keyboard::Q,    // 4
        sf::Keyboard::W,    // 5
        sf::Keyboard::E,    // 6
        sf::Keyboard::A,    // 7
        sf::Keyboard::S,    // 8
        sf::Keyboard::D,    // 9
        sf::Keyboard::Z,    // a
        sf::Keyboard::C,    // b
        sf::Keyboard::Num4, // c
        sf::Keyboard::R,    // d
        sf::Keyboard::F,    // e
        sf::Keyboard::V     // f
    };

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); }
        }

        for (auto i = 0; i < 0x10; i++)
            chip8.keys[i] = sf::Keyboard::isKeyPressed(keys[i]);

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
                drawPixels(window,
                    pixel,
                    oldChip8Pixels,
                    chip8Width,
                    chip8Height,
                    pixelWidth,
                    pixelHeight);
                drawPixels(window,
                    pixel,
                    chip8.pixels,
                    chip8Width,
                    chip8Height,
                    pixelWidth,
                    pixelHeight);
                std::copy(chip8.pixels, chip8.pixels + 256, oldChip8Pixels);
                window.display();
                chip8.drawFlag = false;
            }
        }
    }
    delete[] data;
    return 0;
}
