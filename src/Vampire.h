#pragma once
#include <SFML/System/Vector2.hpp>

class Game;

class Vampire
{
public:
    Vampire(Game *game, sf::Vector2f position);
    void update(float deltaTime);
    sf::Vector2f getPosition() const { return m_position; }

private:
    Game *m_pGame;
    sf::Vector2f m_position;
};
