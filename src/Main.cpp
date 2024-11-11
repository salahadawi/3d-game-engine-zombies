#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <memory>
#include <iostream>

#include "Game.h"
#include "ResourceManager.h"

int main(int argc, char *argv[])
{
    // ResourceManager Must be Instantiated here -- DO NOT CHANGE
    ResourceManager::init(argv[0]);

    sf::RenderWindow window(sf::VideoMode(ScreenWidth, ScreenHeight), "Platformer");
    window.setKeyRepeatEnabled(false);
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);

    std::unique_ptr<Game> pGame = std::make_unique<Game>();
    if (!pGame->initialise(window.getView().getSize()))
    {
        std::cerr << "Game Failed to initialise" << std::endl;
        return 1;
    }

    sf::Clock clock;
    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                // "close requested" event: we close the window
                window.close();
                break;
            case sf::Event::KeyPressed:
                pGame->onKeyPressed(event.key.code);
                break;
            case sf::Event::KeyReleased:
                pGame->onKeyReleased(event.key.code);
                break;
            case sf::Event::MouseMoved:
                pGame->getInput(window);
                break;
            default:
                break;
            }
        }

        sf::Time elapsedTime = clock.getElapsedTime();
        clock.restart();
        pGame->update(elapsedTime.asSeconds());

        // clear the window with black color
        window.clear(sf::Color::Black);

        // fill bottom half of screen with floor
        sf::RectangleShape floor(sf::Vector2f(ScreenWidth, ScreenHeight / 2));
        floor.setFillColor(sf::Color(33, 33, 33));
        floor.setPosition(sf::Vector2f(0, ScreenHeight / 2));
        window.draw(floor);

        window.draw(*pGame.get());

        // end the current frame
        window.display();
    }

    return 0;
}
