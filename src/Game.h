#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include "Constants.h"

#include "LaserShot.h"
#include "Minimap.h"

class Player;
class GameInput;
class Vampire;

struct LaserShot;
class Minimap;
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

    sf::RenderWindow *getWindow() const { return m_pWindow; }

    sf::Text *getContinueText() { return &m_continueText; }

    Player *getPlayer() { return m_pPlayer.get(); }

    void vampireSpawner(float deltaTime);

    sf::Texture *getVampTexture() { return &m_vampTexture; }

    void shootLaser();

    const LaserShot &getLaserShot() const { return m_laserShot; }

    const std::vector<std::unique_ptr<Vampire>> &getVampires() const { return m_pVampires; }

    void showMessage(const std::string &msg, float duration);
    void slowVampires();

private:
    std::unique_ptr<Player> m_pPlayer;

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

    LaserShot m_laserShot;

    float m_messageTimer = 3.0f;
    bool m_showStartMessage = true;
    sf::Text m_startMessage;

    float m_survivalTime = 0.0f;
    sf::Text m_gameOverText;
    sf::Text m_finalScoreText;

    float m_timerAnimationTime = 2.0f;

    float m_baseVampireHealth = 100.0f;

    float m_baseVampireSpeed = VampireSpeed;

    struct Message
    {
        std::string text;
        float duration;
    };
    std::vector<Message> m_messages;

    float m_vampireSpeedMultiplier = 1.0f;
    float m_slowTimer = 0.0f;

    Minimap m_minimap;
};
