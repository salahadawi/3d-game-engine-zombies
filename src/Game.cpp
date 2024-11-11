#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include "InputHandler.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Rectangle.h"
#include "Coin.h"
#include "Door.h"

Game::Game() : m_state(State::PLAYING),
               m_pClock(std::make_unique<sf::Clock>()),
               m_pPlayer(std::make_unique<Player>(this)),
               m_pDoor(std::make_unique<Door>(this)),
               m_score(0),
               m_clearedLevels(0)
{

    m_pGameInput = std::make_unique<GameInput>(this, m_pPlayer.get());
}

Game::~Game()
{
}

bool Game::initialise(sf::RenderWindow &window)
{
    m_pWindow = &window;
    m_pClock->restart();
    // std::string assetPath = Resources::getAssetPath();
    if (!m_font.loadFromFile(ResourceManager::getFilePath("Action_Man.ttf")))
    {
        std::cerr << "Unable to load font" << std::endl;
        return false;
    }

    m_continueText.setFont(m_font);
    m_continueText.setString("Continue");
    m_continueText.setFillColor(sf::Color::White);
    m_continueText.setPosition(ScreenWidth / 2 - m_continueText.getLocalBounds().getSize().x / 2, ScreenHeight / 2 - m_continueText.getLocalBounds().getSize().y / 2);

    // Initialize shapes from TileMap
    resetLevel();
    return true;
}

void Game::resetLevel()
{
    m_pCoins.clear();
    m_pRectangles.clear();

    m_pPlayer->setIsDead(false);
    m_pDoor->setTriggered(false);

    // const sf::Vector2f tileSize(TileSizeX, TileSizeY);

    // for (int y = 0; y < GridSize; y++)
    // {
    //     for (int x = 0; x < GridSize; x++)
    //     {
    //         int i = x + y * GridSize;

    //         const sf::Vector2f worldPos = sf::Vector2f(x * tileSize.x, y * tileSize.y);
    //         switch (tileMap[i])
    //         {
    //         case eTile::eCoin:
    //             m_pCoins.push_back(std::make_unique<Coin>(CoinRadius, worldPos));
    //             break;
    //         case eTile::eBlock:
    //             m_pRectangles.push_back(std::make_unique<Rectangle>(tileSize, worldPos));
    //             break;
    //         case eTile::ePlayerSpawn:
    //             m_pPlayer->setPosition(worldPos);
    //             break;
    //         case eTile::eDoor:
    //             m_pDoor->setPosition(worldPos);
    //             break;
    //         default:
    //             break;
    //         }
    //     }
    // }

    // set player spawn point
    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            if (MapArray1[x + y * GridWidth] == PlayerSpawnPoint)
            {
                m_pPlayer->setSpawnPoint(sf::Vector2f(x, y));
            }
        }
    }
}

void Game::update(float deltaTime)
{
    if (m_state == State::PLAYING)
    {

        m_pGameInput->update(deltaTime);
        m_pPlayer->updatePhysics(deltaTime);
        m_pPlayer->update(deltaTime);
    }

    if (m_pPlayer->isDead())
        resetLevel();
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    //  Draw texts.
    if (m_state == State::PAUSED)
    {
        sf::Text startText;
        startText.setFont(m_font);
        startText.setString("Game Start!");
        startText.setFillColor(sf::Color::White);
        startText.setPosition(80.0f, 80.0f);
        startText.setStyle(sf::Text::Bold);
        target.draw(startText);
    }
    else
    {
        sf::Text coinText;
        coinText.setFont(m_font);
        coinText.setFillColor(sf::Color::White);
        coinText.setStyle(sf::Text::Bold);
        coinText.setString(std::to_string(m_pPlayer->getCoins()));
        // coinText.setColor(sf::Color::Yellow);
        coinText.setPosition(sf::Vector2f(ScreenWidth - coinText.getLocalBounds().getSize().x, 0));
        target.draw(coinText);
    }

    // Draw player.
    // m_pPlayer->draw(target, states);

    //  Draw world.
    // for (auto &temp : m_pCoins)
    // {
    //     temp->draw(target, states);
    // }
    // for (auto &temp : m_pRectangles)
    // {
    //     temp->draw(target, states);
    // }

    // for (int y = 0; y < GridHeight; y++)
    // {

    sf::Image buffer;
    buffer.create(ScreenWidth, ScreenHeight, sf::Color(0, 0, 0, 0));

    for (int x = 0; x < ScreenWidth; x++)
    {

        float cameraX = 2 * x / (float)ScreenWidth - 1;
        float rayDirX = m_pPlayer->getDirX() + cameraX * m_pPlayer->getPlaneX();
        float rayDirY = m_pPlayer->getDirY() + cameraX * m_pPlayer->getPlaneY();

        int mapX = (int)m_pPlayer->getPosition().x;
        int mapY = (int)m_pPlayer->getPosition().y;

        float deltaDistX = std::abs(1 / rayDirX);
        float deltaDistY = std::abs(1 / rayDirY);

        int stepX;
        int stepY;
        float sideDistX;
        float sideDistY;
        if (rayDirX < 0)
        {
            stepX = -1;
            sideDistX = (m_pPlayer->getPosition().x - mapX) * deltaDistX;
        }
        else
        {
            stepX = 1;
            sideDistX = (mapX + 1.0 - m_pPlayer->getPosition().x) * deltaDistX;
        }
        if (rayDirY < 0)
        {
            stepY = -1;
            sideDistY = (m_pPlayer->getPosition().y - mapY) * deltaDistY;
        }
        else
        {
            stepY = 1;
            sideDistY = (mapY + 1.0 - m_pPlayer->getPosition().y) * deltaDistY;
        }

        int hit = 0;
        int side = 0;
        while (hit == 0)
        {
            if (sideDistX < sideDistY)
            {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            }
            else
            {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            if (mapY >= GridHeight || mapX >= GridWidth || mapY < 0 || mapX < 0)
                break;
            if (MapArray1[mapX + mapY * GridWidth] > 0)
                hit = 1;
        }

        float perpWallDist;
        if (side == 0)
        {
            perpWallDist = (mapX - m_pPlayer->getPosition().x + (1 - stepX) / 2) / rayDirX;
        }
        else
        {
            perpWallDist = (mapY - m_pPlayer->getPosition().y + (1 - stepY) / 2) / rayDirY;
        }

        int lineHeight = (int)(ScreenHeight / perpWallDist);
        int drawStart = -lineHeight / VerticalFOVDiv + ScreenHeight / 2;
        if (drawStart < 0)
            drawStart = 0;
        int drawEnd = lineHeight / VerticalFOVDiv + ScreenHeight / 2;
        if (drawEnd >= ScreenHeight)
            drawEnd = ScreenHeight - 1;

        float wallX;
        int wallSide;
        if (side == 0)
        {
            wallX = m_pPlayer->getPosition().y + perpWallDist * rayDirY;
            wallSide = (rayDirX < 0) ? 0 : 1;
        }
        else
        {
            wallX = m_pPlayer->getPosition().x + perpWallDist * rayDirX;
            wallSide = (rayDirY < 0) ? 0 : 1;
        }

        if (hit == 1)
        {
            // Dark brown for walls, slightly darker for shaded walls
            sf::Color wallColor = (side == 1)
                                      ? sf::Color(82, 43, 28)   // Darker brown for shaded walls
                                      : sf::Color(116, 60, 39); // Brown for lit walls

            for (int y = drawStart; y < drawEnd; y++)
                buffer.setPixel(x, y, wallColor);
        }

        else
        {
        }
    }

    // draw minimap
    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            sf::Color color = (MapArray1[x + y * GridWidth] == 1)
                                  ? sf::Color(116, 60, 39) // Brown for walls
                                  : sf::Color(51, 28, 17); // Darker brown for floor
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

    if (m_state == State::PAUSED)
    {
        sf::RectangleShape halfTransparent(sf::Vector2f(ScreenWidth, ScreenHeight));
        halfTransparent.setFillColor(sf::Color(0, 0, 0, 192));
        target.draw(halfTransparent);

        target.draw(m_continueText);
    }
}

void Game::onKeyPressed(sf::Keyboard::Key key)
{
    m_pGameInput->onKeyPressed(key);
}

void Game::onKeyReleased(sf::Keyboard::Key key)
{
    m_pGameInput->onKeyReleased(key);
}

void Game::getInput(sf::RenderWindow &window)
{
    m_pGameInput->getInput(window);
}

void Game::onMousePressed(sf::Mouse::Button button)
{
    m_pGameInput->onMousePressed(button);
}

void Game::onMouseReleased(sf::Mouse::Button button)
{
    m_pGameInput->onMouseReleased(button);
}

std::vector<Coin *> Game::getCoins()
{
    std::vector<Coin *> pCoins;

    for (auto &temp : m_pCoins)
    {
        pCoins.push_back(temp.get());
    }
    return pCoins;
}

std::vector<Rectangle *> Game::getRectangles() const
{
    std::vector<Rectangle *> pRectangles;

    for (auto &pRectangle : m_pRectangles)
    {
        pRectangles.push_back(pRectangle.get());
    }
    return (pRectangles);
}

Door *Game::getDoor()
{
    return m_pDoor.get();
}
