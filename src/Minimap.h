#pragma once

#include <SFML/Graphics.hpp>

class Game;
class Player;

class Minimap
{
public:
    Minimap(Game *pGame);
    void drawMinimap(sf::RenderTarget &target, sf::Image &buffer) const;

private:
    Game *m_pGame;
    Player *m_pPlayer;
};
