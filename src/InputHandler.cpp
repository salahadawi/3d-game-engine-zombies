#include "InputHandler.h"
#include "Player.h"
#include "Game.h"

#include <SFML/Graphics.hpp>

GameInput::GameInput(Game *pGame, Player *pPlayer) : m_pGame(pGame), m_pPlayer(pPlayer)
{
}

GameInput::~GameInput()
{
}

void GameInput::update(float deltaTime)
{
    m_pPlayer->move(m_inputData, deltaTime);
}

void GameInput::onKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::W)
    {
        m_inputData.m_movingUp = true;
    }
    else if (key == sf::Keyboard::S)
    {
        m_inputData.m_movingDown = true;
    }
    else if (key == sf::Keyboard::A)
    {
        m_inputData.m_movingLeft = true;
    }
    else if (key == sf::Keyboard::D)
    {
        m_inputData.m_movingRight = true;
    }
    else if (key == sf::Keyboard::Escape)
    {
        Game::State state = m_pGame->togglePause();

        sf::RenderWindow *pWindow = m_pGame->getWindow();
        if (state == Game::State::PLAYING)
        {
            pWindow->setMouseCursorVisible(false);
            pWindow->setMouseCursorGrabbed(true);
        }
        else
        {
            pWindow->setMouseCursorVisible(true);
            pWindow->setMouseCursorGrabbed(false);
        }
    }
    else if (key == sf::Keyboard::Z)
    {
        if (m_pPlayer->upgradeGunDamage())
        {
            m_pGame->showMessage("Gun Damage Upgraded! (+" +
                                     std::to_string(static_cast<int>(m_pPlayer->getGunDamage())) + " damage)",
                                 1.5f);
        }
        else
        {
            m_pGame->showMessage("Need $" +
                                     std::to_string(m_pPlayer->getDamageUpgradeCost()) +
                                     " for damage upgrade!",
                                 1.5f);
        }
    }
    else if (key == sf::Keyboard::X)
    {
        if (m_pPlayer->slowVampires())
        {
            m_pGame->slowVampires();
            m_pGame->showMessage("Vampires Slowed!", 1.5f);
        }
        else
        {
            m_pGame->showMessage("Need $" +
                                     std::to_string(m_pPlayer->getSlowUpgradeCost()) +
                                     " to slow vampires!",
                                 1.5f);
        }
    }
    else if (key == sf::Keyboard::Left)
    {
        m_inputData.m_turningLeft = true;
    }
    else if (key == sf::Keyboard::Right)
    {
        m_inputData.m_turningRight = true;
    }
    else if (key == sf::Keyboard::Space)
    {
        if (m_pGame->getState() == Game::State::PLAYING)
        {
            m_pGame->shootLaser();
        }
    }
}

void GameInput::onKeyReleased(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::W)
    {
        m_inputData.m_movingUp = false;
    }
    else if (key == sf::Keyboard::S)
    {
        m_inputData.m_movingDown = false;
    }
    else if (key == sf::Keyboard::A)
    {
        m_inputData.m_movingLeft = false;
    }
    else if (key == sf::Keyboard::D)
    {
        m_inputData.m_movingRight = false;
    }
    else if (key == sf::Keyboard::Left)
    {
        m_inputData.m_turningLeft = false;
    }
    else if (key == sf::Keyboard::Right)
    {
        m_inputData.m_turningRight = false;
    }
}

InputData GameInput::getInput(sf::RenderWindow &window)
{
    // Mouse input
    sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);

    if (m_pGame->getState() == Game::State::PLAYING)
    {
        if (m_firstMouse)
        {
            m_lastMousePos = currentMousePos;
            m_firstMouse = false;
        }

        // Calculate mouse movement delta
        m_inputData.m_mouseDelta.x = float(currentMousePos.x - m_lastMousePos.x);
        m_inputData.m_mouseDelta.y = float(currentMousePos.y - m_lastMousePos.y);

        // Update last mouse position
        m_lastMousePos = currentMousePos;

        // Center the mouse cursor
        sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
        sf::Mouse::setPosition(windowCenter, window);
        m_lastMousePos = windowCenter;
    }
    return m_inputData;
}

void GameInput::onMousePressed(sf::Mouse::Button button)
{
    if (button == sf::Mouse::Left)
    {
        m_inputData.m_mouseLeftPressed = true;

        if (m_pGame->getState() == Game::State::PLAYING)
        {
            m_pGame->shootLaser();
        }
        else if (m_pGame->getState() == Game::State::PAUSED)
        {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(*m_pGame->getWindow());
            if (m_pGame->getContinueText()->getGlobalBounds().contains(sf::Vector2f(currentMousePos)))
            {
                m_pGame->togglePause();
            }
        }
    }
}

void GameInput::onMouseReleased(sf::Mouse::Button button)
{
    if (button == sf::Mouse::Left)
        m_inputData.m_mouseLeftPressed = false;
}
