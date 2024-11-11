#include <SFML/Graphics.hpp>
#include <vector>
#include "Player.h"
#include "Vampire.h"
#include "Constants.h"
#include "Game.h"
#include "Minimap.h"

class Game;

Minimap::Minimap(Game *pGame) : m_pGame(pGame), m_pPlayer(pGame->getPlayer())
{
}

void Minimap::drawMinimap(sf::RenderTarget &target, sf::Image &buffer) const
{
    // draw minimap
    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            sf::Color color;
            if (MapArray1[x + y * GridWidth] == 1)
                color = sf::Color(116, 60, 39); // Brown for walls
            else if (MapArray1[x + y * GridWidth] == 3)
                color = sf::Color(128, 0, 128); // Purple for zombie spawns
            else
                color = sf::Color(51, 28, 17); // Darker brown for floor

            // Draw 10x10 block for each map point
            for (int blockY = 0; blockY < 10; blockY++)
            {
                for (int blockX = 0; blockX < 10; blockX++)
                {
                    buffer.setPixel(x * 10 + blockX, y * 10 + blockY, color);
                }
            }
        }
    }

    sf::Texture texture;
    texture.loadFromImage(buffer);
    sf::Sprite sprite(texture);
    target.draw(sprite);

    sf::CircleShape playerShape(5);
    playerShape.setFillColor(sf::Color::Green);
    playerShape.setPosition(m_pPlayer->getPosition().x * 10 - 5, m_pPlayer->getPosition().y * 10 - 5);
    target.draw(playerShape);

    // draw cone showing player direction
    sf::VertexArray cone(sf::TriangleFan, 3);

    // Cone point at center of player circle
    cone[0].position = sf::Vector2f(m_pPlayer->getPosition().x * 10, m_pPlayer->getPosition().y * 10);

    // Calculate perpendicular vector for cone width
    float perpX = -m_pPlayer->getDirY();
    float perpY = m_pPlayer->getDirX();

    // Two points forming cone base
    cone[1].position = sf::Vector2f(
        m_pPlayer->getPosition().x * 10 + m_pPlayer->getDirX() * 20 + perpX * 8,
        m_pPlayer->getPosition().y * 10 + m_pPlayer->getDirY() * 20 + perpY * 8);
    cone[2].position = sf::Vector2f(
        m_pPlayer->getPosition().x * 10 + m_pPlayer->getDirX() * 20 - perpX * 8,
        m_pPlayer->getPosition().y * 10 + m_pPlayer->getDirY() * 20 - perpY * 8);

    // Set colors
    cone[0].color = sf::Color::Yellow;
    cone[0].color.a = 128;
    cone[1].color = sf::Color::Yellow;
    cone[1].color.a = 128;
    cone[2].color = sf::Color::Yellow;
    cone[2].color.a = 128;

    target.draw(cone);

    // draw vampires in minimap
    for (auto &vampire : m_pGame->getVampires())
    {
        sf::CircleShape vampireShape(5);
        vampireShape.setFillColor(sf::Color::Red);
        vampireShape.setPosition(vampire->getPosition().x * 10 - 5, vampire->getPosition().y * 10 - 5);
        target.draw(vampireShape);
    }

    // Draw laser shot if active
    const auto &laserShot = m_pGame->getLaserShot();
    if (laserShot.active)
    {

        // Draw laser in minimap (keep this thinner)
        sf::VertexArray minimapLaser(sf::Lines, 2);
        minimapLaser[0].position = sf::Vector2f(m_pGame->getLaserShot().startX * 10, m_pGame->getLaserShot().startY * 10);
        minimapLaser[1].position = sf::Vector2f(
            (laserShot.startX + laserShot.dirX * laserShot.distance) * 10,
            (laserShot.startY + laserShot.dirY * laserShot.distance) * 10);
        minimapLaser[0].color = sf::Color(0, 255, 0, 255 * (laserShot.lifetime / LASER_LIFETIME));
        minimapLaser[1].color = sf::Color(0, 255, 0, 255 * (laserShot.lifetime / LASER_LIFETIME));

        target.draw(minimapLaser);
    }
}