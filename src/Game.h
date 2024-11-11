#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Constants.h"

class Player;
class Game;
class GameInput;
class Rectangle;
class Coin;
class Door;

namespace sf
{
    class Clock;
}

class Game : public sf::Drawable
{
public:
    enum class State
    {
        PAUSED,
        PLAYING,
    };

    Game();
    ~Game();

    bool initialise(sf::RenderWindow &window);
    void update(float deltaTime);
    void resetLevel();
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

    State getState() const { return m_state; }

    State togglePause() { return m_state = m_state == State::PAUSED ? State::PLAYING : State::PAUSED; }

    void onKeyPressed(sf::Keyboard::Key key);
    void onKeyReleased(sf::Keyboard::Key key);
    void getInput(sf::RenderWindow &window);
    void onMousePressed(sf::Mouse::Button button);
    void onMouseReleased(sf::Mouse::Button button);

    Door *getDoor();
    std::vector<Rectangle *> getRectangles() const;
    std::vector<Coin *> getCoins();

    sf::RenderWindow *getWindow() const { return m_pWindow; }

    sf::Text *getContinueText() { return &m_continueText; }

private:
    std::unique_ptr<Player> m_pPlayer;
    std::unique_ptr<Door> m_pDoor;

    std::vector<std::unique_ptr<Rectangle>> m_pRectangles;
    std::vector<std::unique_ptr<Coin>> m_pCoins;

    State m_state;
    std::unique_ptr<sf::Clock> m_pClock;
    std::unique_ptr<GameInput> m_pGameInput;

    int m_clearedLevels;

    int m_score;

    sf::Font m_font;

    sf::RenderWindow *m_pWindow;

    sf::Text m_continueText;
};
