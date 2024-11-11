#pragma once

#include "Rectangle.h"
#include "Constants.h"
#include <cmath>

struct InputData;

class Game;

class Player
{
public:
    Player(Game *pGame);
    virtual ~Player() {}

    void initialise() {};
    void move(InputData inputData, float deltaTime);
    void updatePhysics(float deltaTime);
    void update(float deltaTime);

    int getCoins() const { return m_coins; };

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = isDead; }

    // 0.5 to center the player on the tile
    void setSpawnPoint(sf::Vector2f spawnPoint) { m_position = sf::Vector2f(spawnPoint.x + 0.5f, spawnPoint.y + 0.5f); }

    void updateSpeed(float deltaTime) { m_moveSpeed = deltaTime * 100; }
    void updateTurnSpeed(float deltaTime) { m_rotationSpeed = deltaTime / 1000 / 150; }

    float getDirX() { return m_dirX; }
    float getDirY() { return m_dirY; }
    float getPlaneX() { return m_planeX; }
    float getPlaneY() { return m_planeY; }

    sf::Vector2f getPosition() { return m_position; }

    void setPosition(sf::Vector2f position) { m_position = position; }

private:
    bool m_isGrounded;
    bool m_isDead = false;
    float m_jumpTimer = 0.0f;
    Game *m_pGame;
    int m_coins = 0;

    sf::Vector2f m_position;
    float m_dirX = 1;
    float m_dirY = 0.000001;
    float m_planeX = 0;
    float m_planeY = 0.66;
    float m_spawnX;
    float m_spawnY;
    float m_moveSpeed;
    float m_rotationSpeed = PlayerRotationSpeed;
    int m_camHeight;
    int m_crouching;
    int m_busy;
    float m_verticalFOV = VerticalFOVDiv;
    float m_rotation = 0.0f;
};
