#include "Chip8.hpp"

#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window = {};
    window.create(sf::VideoMode(1280, 720), "Chip8 Interpreter");

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) { window.close(); }
        }

        window.clear();
        window.display();
    }
    return 0;
}
