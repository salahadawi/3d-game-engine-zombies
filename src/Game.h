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
class Vampire;

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
        GAME_OVER
    };

    enum class HitType
    {
        NONE,
        WALL,
        VAMPIRE
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

    Player *getPlayer() { return m_pPlayer.get(); }

    void vampireSpawner(float deltaTime);

    sf::Texture *getVampTexture() { return &m_vampTexture; }

    void shootLaser();

private:
    std::unique_ptr<Player> m_pPlayer;
    std::unique_ptr<Door> m_pDoor;

    std::vector<std::unique_ptr<Rectangle>> m_pRectangles;
    std::vector<std::unique_ptr<Coin>> m_pCoins;
    std::vector<std::unique_ptr<Vampire>> m_pVampires;

    State m_state;
    std::unique_ptr<sf::Clock> m_pClock;
    std::unique_ptr<GameInput> m_pGameInput;

    int m_clearedLevels;

    int m_score;

    sf::Font m_font;

    sf::RenderWindow *m_pWindow;

    sf::Text m_continueText;

    float m_vampireCooldown = 0.0f;
    float m_nextVampireCooldown = 2.0f;
    int m_spawnCount = 0;

    sf::Texture m_vampTexture;
    sf::Texture m_rayGunTexture;

    struct LaserShot
    {
        float startX, startY; // Starting position
        float dirX, dirY;     // Direction
        float distance;       // Distance to hit
        float lifetime;       // How long to show the laser
        bool active;          // Whether there's currently an active laser
    };

    LaserShot m_laserShot;            // Single laser shot instead of vector
    const float LASER_LIFETIME = 0.1; // Laser visible for 0.1 seconds

    float m_messageTimer = 3.0f; // Show message for 3 seconds
    bool m_showStartMessage = true;
    sf::Text m_startMessage;

    float m_survivalTime = 0.0f;
    sf::Text m_gameOverText;
    sf::Text m_finalScoreText;

    void formatTime(float seconds, char *buffer) const;
};
