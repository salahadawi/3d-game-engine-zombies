#include "Vampire.h"
#include "Game.h"
#include "Player.h"
#include "MathUtils.h"
#include "Constants.h"

Vampire::Vampire(Game *game, sf::Vector2f position) : m_pGame(game), m_position(position)
{
}

void Vampire::update(float deltaTime)
{
    if (m_attackCooldown > 0)
    {
        m_attackCooldown -= deltaTime;
    }

    sf::Vector2f playerPosition = m_pGame->getPlayer()->getPosition();
    sf::Vector2f direction = VecNormalized(playerPosition - m_position);

    sf::Vector2f oldPosition = m_position;

    // Calculate next position using current speed
    sf::Vector2f nextPosition = m_position;
    float moveAmount = m_speed * deltaTime;

    // Try to move in X direction
    float nextX = m_position.x + direction.x * moveAmount;
    if (nextX > 0 && nextX < GridWidth)
    {
        if (MapArray1[int(m_position.y) * GridWidth + int(nextX)] != 1)
        {
            nextPosition.x = nextX;
        }
    }

    // Try to move in Y direction
    float nextY = m_position.y + direction.y * moveAmount;
    if (nextY > 0 && nextY < GridHeight)
    {
        if (MapArray1[int(nextY) * GridWidth + int(m_position.x)] != 1)
        {
            nextPosition.y = nextY;
        }
    }

    // Update position
    m_position = nextPosition;

    // Check for collision with player
    sf::Vector2f toPlayer = playerPosition - m_position;
    float distanceToPlayer = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

    if (distanceToPlayer < 0.5f)
    {
        if (m_attackCooldown <= 0) // Only deal damage if attack is ready
        {
            m_pGame->getPlayer()->damage(25.0f);
            m_attackCooldown = ATTACK_DELAY;

            if (m_pGame->getPlayer()->getHealth() <= 0)
            {
                m_pGame->getPlayer()->setIsDead(true);
            }
        }
        m_position = oldPosition;
    }

    // Check for collisions with other vampires
    for (const auto &otherVampire : m_pGame->getVampires())
    {
        if (otherVampire.get() != this) // Don't check collision with self
        {
            sf::Vector2f toOtherVampire = otherVampire->getPosition() - m_position;
            float distanceToVampire = sqrt(toOtherVampire.x * toOtherVampire.x + toOtherVampire.y * toOtherVampire.y);

            if (distanceToVampire < 0.5f)
            {
                m_position = oldPosition;
                break;
            }
        }
    }
}