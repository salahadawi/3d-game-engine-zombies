#pragma once

#include "Controller.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Player;
class Game;

struct InputData
{
    bool m_movingUp = false;
    bool m_movingDown = false;
    bool m_movingLeft = false;
    bool m_movingRight = false;
    sf::Vector2f m_mouseDelta{0.0f, 0.0f};

    bool hasInputs() { return m_movingUp || m_movingDown || m_movingLeft || m_movingRight; }
};

class GameInput
{
public:
    GameInput(Game *pGame, Player *pPlayer);
    ~GameInput();

    void update(float deltaTime);
    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    InputData getInput(sf::RenderWindow &window);
    sf::Vector2f getMouseDelta() { return m_inputData.m_mouseDelta; }

private:
    InputData m_inputData;
    Game *m_pGame;
    Player *m_pPlayer;

    // Add mouse tracking variables
    sf::Vector2i m_lastMousePos;
    bool m_firstMouse = true;
};
