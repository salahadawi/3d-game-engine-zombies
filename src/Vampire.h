#pragma once

#include "Rectangle.h"

class Game;

class Vampire
{
public:
    Vampire(Game *game, sf::Vector2f position);
    virtual ~Vampire() {}

    void update(float deltaTime);

    void setIsKilled(bool isKilled) { m_isKilled = isKilled; }
    bool isKilled() { return m_isKilled; }

    sf::Vector2f getPosition() { return m_position; }

private:
    Game *m_pGame;
    bool m_isKilled = false;

    sf::Vector2f m_position;
};
