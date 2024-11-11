#include "Player.h"
#include "InputHandler.h"
#include "Constants.h"
#include "Coin.h"
#include <vector>
#include "Game.h"
#include "Door.h"
#include <cmath>

Player::Player(Game *pGame) : m_pGame(pGame)
{
}

void Player::move(InputData inputData, float deltaTime)
{
    // Handle mouse movement for rotation
    m_rotation += inputData.m_mouseDelta.x * 0.001;

    // Keep rotation between 0 and 2π
    m_rotation += 2 * M_PI;
    while (m_rotation >= 2 * M_PI)
        m_rotation -= 2 * M_PI;

    // Update direction vector based on rotation
    m_dirX = cos(m_rotation);
    m_planeX = cos(m_rotation + M_PI / 2);
    m_dirY = sin(m_rotation);
    m_planeY = sin(m_rotation + M_PI / 2);
    // printf("Rotation: %f\n", m_rotation);
    // printf("Direction: %f, %f\n", m_dirX, m_dirY);

    // Forward movement
    if (inputData.m_movingUp)
    {
        float nextY = m_position.y + m_dirY * m_moveSpeed / 20;
        float nextX = m_position.x + m_dirX * m_moveSpeed / 20;

        // Only check collision if we're still in bounds
        if (nextY < GridHeight && nextY > 0 && nextX < GridWidth && nextX > 0)
        {
            // Check if next position would hit a wall
            if (MapArray1[int(nextY) * GridWidth + int(nextX)] < 1)
            {
                m_position.y = nextY;
                m_position.x = nextX;
            }
        }
    }

    // Backward movement
    if (inputData.m_movingDown)
    {
        float nextY = m_position.y - m_dirY * m_moveSpeed / 20;
        float nextX = m_position.x - m_dirX * m_moveSpeed / 20;

        if (nextY < GridHeight && nextY > 0 && nextX < GridWidth && nextX > 0)
        {
            if (MapArray1[int(nextY) * GridWidth + int(nextX)] < 1)
            {
                m_position.y = nextY;
                m_position.x = nextX;
            }
        }
    }

    // Strafe left
    if (inputData.m_movingLeft)
    {
        float nextX = m_position.x - m_planeX * m_moveSpeed / 20;
        float nextY = m_position.y - m_planeY * m_moveSpeed / 20;

        if (nextX < GridWidth && nextX > 0 && nextY < GridHeight && nextY > 0)
        {
            if (MapArray1[int(nextY) * GridWidth + int(nextX)] < 1)
            {
                m_position.x = nextX;
                m_position.y = nextY;
            }
        }
    }

    // Strafe right
    if (inputData.m_movingRight)
    {
        float nextX = m_position.x + m_planeX * m_moveSpeed / 20;
        float nextY = m_position.y + m_planeY * m_moveSpeed / 20;

        if (nextX < GridWidth && nextX > 0 && nextY < GridHeight && nextY > 0)
        {
            if (MapArray1[int(nextY) * GridWidth + int(nextX)] < 1)
            {
                m_position.x = nextX;
                m_position.y = nextY;
            }
        }
    }
    // printf("Position: %f, %f\n", m_position.x, m_position.y);
}

void Player::updatePhysics(float deltaTime)
{
}

void Player::damage(float amount)
{
    m_health = std::max(0.0f, m_health - amount);
    m_timeSinceLastHit = 0.0f;
}

void Player::update(float deltaTime)
{
    updateSpeed(deltaTime);

    // Update health regeneration
    m_timeSinceLastHit += deltaTime;

    if (m_timeSinceLastHit > m_regenDelay && m_health < m_maxHealth)
    {
        m_health = std::min(m_maxHealth, m_health + m_regenRate * deltaTime);
    }
}
