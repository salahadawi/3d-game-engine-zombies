#include "Vampire.h"
// #include "Constants.h"
#include "Game.h"
#include "Player.h"
#include "MathUtils.h"

Vampire::Vampire(Game *game, sf::Vector2f position) : m_pGame(game), m_position(position)
{
}

void Vampire::update(float deltaTime)
{
    // Player *pPlayer = m_pGame->getPlayer();

    // if (collidesWith(pPlayer))
    //     pPlayer->setIsDead(true);

    sf::Vector2f playerPosition = m_pGame->getPlayer()->getPosition();
    sf::Vector2f direction = VecNormalized(playerPosition - m_position);
    direction *= VampireSpeed * deltaTime;
    m_position += direction;

    // printf("Vampire position: %f, %f\n", m_position.x, m_position.y);
}