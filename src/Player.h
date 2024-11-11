#pragma once

#include "Constants.h"
#include <cmath>
#include <SFML/Graphics.hpp>

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

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = isDead; }

    void setSpawnPoint(sf::Vector2f spawnPoint) { m_position = sf::Vector2f(spawnPoint.x + 0.5f, spawnPoint.y + 0.5f); }

    void updateSpeed(float deltaTime) { m_moveSpeed = deltaTime * 100; }
    void updateTurnSpeed(float deltaTime) { m_rotationSpeed = deltaTime / 1000 / 150; }

    float getDirX() { return m_dirX; }
    float getDirY() { return m_dirY; }
    float getPlaneX() { return m_planeX; }
    float getPlaneY() { return m_planeY; }

    sf::Vector2f getPosition() { return m_position; }

    void setPosition(sf::Vector2f position) { m_position = position; }

    float getHealth() const { return m_health; }
    float getMaxHealth() const { return m_maxHealth; }
    void damage(float amount);
    bool isRegenerating() const { return m_timeSinceLastHit > m_regenDelay; }

    void addMoney(int amount) { m_money += amount; }
    int getMoney() const { return m_money; }

    // Add upgrade methods
    bool upgradeGunDamage();
    bool slowVampires();

    float getGunDamage() const { return m_gunDamage; }

    int getDamageUpgradeCost() const { return 200 + (m_damageUpgradeCount * 100); }
    int getSlowUpgradeCost() const { return 300 + (m_slowUpgradeCount * 150); }

private:
    bool m_isDead = false;
    float m_jumpTimer = 0.0f;
    Game *m_pGame;

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

    float m_health = 100.0f;
    const float m_maxHealth = 100.0f;
    float m_timeSinceLastHit = 0.0f;
    const float m_regenDelay = 3.0f;
    const float m_regenRate = 20.0f;

    int m_money = 0;

    // Upgrade system with scaling prices
    float m_gunDamage = 34.0f;
    const float DAMAGE_UPGRADE_AMOUNT = 17.0f;
    int m_damageUpgradeCount = 0;
    int m_slowUpgradeCount = 0;
};
