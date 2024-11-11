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
    sf::Vector2f playerPosition = m_pGame->getPlayer()->getPosition();
    sf::Vector2f direction = VecNormalized(playerPosition - m_position);

    // Calculate next position
    sf::Vector2f nextPosition = m_position;
    float moveAmount = VampireSpeed * deltaTime;

    // Try to move in X direction
    float nextX = m_position.x + direction.x * moveAmount;
    if (nextX > 0 && nextX < GridWidth)
    {
        if (MapArray1[int(m_position.y) * GridWidth + int(nextX)] < 1)
        {
            nextPosition.x = nextX;
        }
    }

    // Try to move in Y direction
    float nextY = m_position.y + direction.y * moveAmount;
    if (nextY > 0 && nextY < GridHeight)
    {
        if (MapArray1[int(nextY) * GridWidth + int(m_position.x)] < 1)
        {
            nextPosition.y = nextY;
        }
    }

    // Update position
    m_position = nextPosition;

    // Check for collision with player
    sf::Vector2f toPlayer = playerPosition - m_position;
    float distanceToPlayer = sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);
    if (distanceToPlayer < 0.5f) // Collision radius
    {
        m_pGame->getPlayer()->setIsDead(true);
    }
}