#pragma once
#include <SFML/System/Vector2.hpp>
#include <algorithm>

#include "Constants.h"

class Game;

class Vampire
{
public:
    Vampire(Game *game, sf::Vector2f position);
    void update(float deltaTime);
    sf::Vector2f getPosition() const { return m_position; }
    float getHealth() const { return m_health; }
    void damage(float amount) { m_health = std::max(0.0f, m_health - amount); }
    bool isDead() const { return m_health <= 0; }
    void setMaxHealth(float health)
    {
        m_maxHealth = health;
        m_health = health;
    }
    float getMaxHealth() const { return m_maxHealth; }
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }

private:
    Game *m_pGame;
    sf::Vector2f m_position;
    float m_health = 100.0f;
    float m_attackCooldown = 0.0f;
    const float ATTACK_DELAY = 1.0f;
    float m_maxHealth = 100.0f;
    float m_speed = VampireSpeed;
};
