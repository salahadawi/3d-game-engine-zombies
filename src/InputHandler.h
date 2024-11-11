#pragma once

#include "Controller.h"
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
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
    bool m_mouseLeftPressed = false;

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
    void onMousePressed(sf::Mouse::Button button);
    void onMouseReleased(sf::Mouse::Button button);

private:
    InputData m_inputData;
    Game *m_pGame;
    Player *m_pPlayer;

    // Mouse tracking variables
    sf::Vector2i m_lastMousePos;
    bool m_firstMouse = true;
};
