#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstdio>

#include "InputHandler.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Rectangle.h"
#include "Coin.h"
#include "Door.h"
#include "Vampire.h"

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
    if (!m_vampTexture.loadFromFile(ResourceManager::getFilePath("vampire.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }
    if (!m_rayGunTexture.loadFromFile(ResourceManager::getFilePath("raygun.png")))
    {
        std::cerr << "Unable to load texture" << std::endl;
        return false;
    }

    m_continueText.setFont(m_font);
    m_continueText.setString("Continue");
    m_continueText.setFillColor(sf::Color::White);
    m_continueText.setPosition(ScreenWidth / 2 - m_continueText.getLocalBounds().getSize().x / 2, ScreenHeight / 2 - m_continueText.getLocalBounds().getSize().y / 2);

    // Initialize shapes from TileMap
    resetLevel();

    // Setup start message
    m_startMessage.setFont(m_font);
    m_startMessage.setString("Vampires are invading the building, survive!");
    m_startMessage.setCharacterSize(30);
    m_startMessage.setFillColor(sf::Color::White);

    // Center the message
    sf::FloatRect textBounds = m_startMessage.getLocalBounds();
    m_startMessage.setPosition(
        (ScreenWidth - textBounds.width) / 2,
        (ScreenHeight - textBounds.height) / 3);

    // Setup game over texts
    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(50);
    m_gameOverText.setFillColor(sf::Color::Red);

    // Center game over text
    sf::FloatRect gameOverBounds = m_gameOverText.getLocalBounds();
    m_gameOverText.setPosition(
        (ScreenWidth - gameOverBounds.width) / 2,
        ScreenHeight / 3);

    m_finalScoreText.setFont(m_font);
    m_finalScoreText.setCharacterSize(30);
    m_finalScoreText.setFillColor(sf::Color::White);

    return true;
}

void Game::resetLevel()
{
    // Don't reset if game is over
    if (m_state == State::GAME_OVER)
        return;

    m_pPlayer->setIsDead(false);

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
        // Update survival time
        m_survivalTime += deltaTime;

        // Update start message timer
        if (m_showStartMessage)
        {
            m_messageTimer -= deltaTime;
            if (m_messageTimer <= 0)
            {
                m_showStartMessage = false;
            }
        }

        m_pGameInput->update(deltaTime);
        m_pPlayer->updatePhysics(deltaTime);
        m_pPlayer->update(deltaTime);

        vampireSpawner(deltaTime);
        for (auto &temp : m_pVampires)
        {
            temp->update(deltaTime);
        }

        // Update laser shot
        if (m_laserShot.active)
        {
            m_laserShot.lifetime -= deltaTime;
            if (m_laserShot.lifetime <= 0)
            {
                m_laserShot.active = false;
            }
        }

        if (m_pPlayer->isDead())
        {
            m_state = State::GAME_OVER;

            // Format final score text
            m_finalScoreText.setString("You survived for " + std::to_string(static_cast<int>(m_survivalTime)) + " seconds.");

            // Center final score text
            sf::FloatRect scoreBounds = m_finalScoreText.getLocalBounds();
            m_finalScoreText.setPosition(
                (ScreenWidth - scoreBounds.width) / 2,
                ScreenHeight / 2);
        }

        // Update timer animation
        if (m_timerAnimationTime > 0.0f)
        {
            m_timerAnimationTime = std::max(0.0f, m_timerAnimationTime - deltaTime);
        }

        // Increase base vampire health over time
        m_baseVampireHealth = std::min(m_baseVampireHealth + HEALTH_INCREASE_RATE * deltaTime,
                                       MAX_VAMPIRE_HEALTH);

        // Increase base vampire speed over time
        m_baseVampireSpeed = std::min(m_baseVampireSpeed + SPEED_INCREASE_RATE * deltaTime,
                                      MAX_VAMPIRE_SPEED);

        // Update existing vampires' speed
        for (auto &vampire : m_pVampires)
        {
            vampire->setSpeed(m_baseVampireSpeed);
        }
    }
}

void Game::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    if (m_state == State::GAME_OVER)
    {
        // Draw dark overlay
        sf::RectangleShape overlay(sf::Vector2f(ScreenWidth, ScreenHeight));
        overlay.setFillColor(sf::Color(0, 0, 0, 192));
        target.draw(overlay);

        // Draw game over text
        target.draw(m_gameOverText);
        target.draw(m_finalScoreText);
    }
    else
    {
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

            HitType hit = HitType::NONE;
            int side = 0;
            while (hit == HitType::NONE)
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
                    hit = HitType::WALL;
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

            if (hit == HitType::WALL)
            {
                // Choose color based on map tile type
                sf::Color wallColor;
                if (MapArray1[mapY * GridWidth + mapX] == 3) // Spawner
                {
                    wallColor = (side == 1)
                                    ? sf::Color(64, 0, 64)    // Darker purple for shaded spawners
                                    : sf::Color(128, 0, 128); // Purple for lit spawners
                }
                else // Normal wall
                {
                    wallColor = (side == 1)
                                    ? sf::Color(82, 43, 28)   // Darker brown for shaded walls
                                    : sf::Color(116, 60, 39); // Brown for lit walls
                }

                for (int y = drawStart; y < drawEnd; y++)
                {
                    float intensity = (LightingStrength / perpWallDist);
                    if (intensity < 1)
                        buffer.setPixel(x, y, sf::Color(wallColor.r * intensity, wallColor.g * intensity, wallColor.b * intensity));
                    else
                        buffer.setPixel(x, y, wallColor);
                }
            }
        }
        // Draw vampires as sprites
        sf::Image vampImage = m_vampTexture.copyToImage();

        // Create a vector of vampires sorted by distance
        std::vector<std::pair<float, Vampire *>> sortedVampires;
        for (auto &vampire : m_pVampires)
        {
            float spriteX = vampire->getPosition().x - m_pPlayer->getPosition().x;
            float spriteY = vampire->getPosition().y - m_pPlayer->getPosition().y;
            float distToVampire = spriteX * spriteX + spriteY * spriteY; // Square distance is fine for sorting
            sortedVampires.push_back({distToVampire, vampire.get()});
        }

        // Sort vampires from farthest to nearest
        std::sort(sortedVampires.begin(), sortedVampires.end(),
                  [](auto &a, auto &b)
                  { return a.first > b.first; });

        // Draw vampires in sorted order
        for (auto &[dist, vampire] : sortedVampires)
        {
            // Get vampire position relative to player
            float spriteX = vampire->getPosition().x - m_pPlayer->getPosition().x;
            float spriteY = vampire->getPosition().y - m_pPlayer->getPosition().y;

            // Transform sprite with the inverse camera matrix
            float invDet = 1.0f / (m_pPlayer->getPlaneX() * m_pPlayer->getDirY() - m_pPlayer->getDirX() * m_pPlayer->getPlaneY());
            float transformX = invDet * (m_pPlayer->getDirY() * spriteX - m_pPlayer->getDirX() * spriteY);
            float transformY = invDet * (-m_pPlayer->getPlaneY() * spriteX + m_pPlayer->getPlaneX() * spriteY);

            // Don't render if behind camera
            if (transformY <= 0)
                continue;

            // Check if vampire is behind a wall using ray casting
            float distToVampire = sqrt(spriteX * spriteX + spriteY * spriteY);
            float rayDirX = spriteX / distToVampire;
            float rayDirY = spriteY / distToVampire;

            float deltaDistX = std::abs(1.0f / rayDirX);
            float deltaDistY = std::abs(1.0f / rayDirY);

            int mapX = int(m_pPlayer->getPosition().x);
            int mapY = int(m_pPlayer->getPosition().y);

            float sideDistX, sideDistY;
            int stepX, stepY;

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

            // Ray casting loop to check for walls
            bool hitWall = false;
            float distToWall = 0.0f;
            while (!hitWall && distToWall < distToVampire)
            {
                if (sideDistX < sideDistY)
                {
                    sideDistX += deltaDistX;
                    mapX += stepX;
                    distToWall = (mapX - m_pPlayer->getPosition().x + (1 - stepX) / 2) / rayDirX;
                }
                else
                {
                    sideDistY += deltaDistY;
                    mapY += stepY;
                    distToWall = (mapY - m_pPlayer->getPosition().y + (1 - stepY) / 2) / rayDirY;
                }

                // Check if we hit a wall
                if (mapX >= 0 && mapX < GridWidth && mapY >= 0 && mapY < GridHeight)
                {
                    if (MapArray1[mapY * GridWidth + mapX] > 0)
                    {
                        hitWall = true;
                    }
                }
            }

            // Skip rendering this vampire if it's behind a wall
            if (hitWall && distToWall < distToVampire)
            {
                continue;
            }

            int spriteScreenX = int((ScreenWidth / 2) * (1 + transformX / transformY));

            // Calculate sprite dimensions on screen
            int spriteHeight = abs(int(ScreenHeight / transformY));
            int drawStartY = -spriteHeight / 2 + ScreenHeight / 2;
            if (drawStartY < 0)
                drawStartY = 0;
            int drawEndY = spriteHeight / 2 + ScreenHeight / 2;
            if (drawEndY >= ScreenHeight)
                drawEndY = ScreenHeight - 1;

            int spriteWidth = abs(int(ScreenHeight / transformY));
            int drawStartX = -spriteWidth / 2 + spriteScreenX;
            if (drawStartX < 0)
                drawStartX = 0;
            int drawEndX = spriteWidth / 2 + spriteScreenX;
            if (drawEndX >= ScreenWidth)
                drawEndX = ScreenWidth - 1;

            // Draw the sprite
            for (int stripe = drawStartX; stripe < drawEndX; stripe++)
            {
                if (transformY > 0) // Check if in front of camera
                {
                    int texX = int(256 * (stripe - (-spriteWidth / 2 + spriteScreenX)) * vampImage.getSize().x / spriteWidth) / 256;

                    for (int y = drawStartY; y < drawEndY; y++)
                    {
                        int d = (y) * 256 - ScreenHeight * 128 + spriteHeight * 128;
                        int texY = ((d * vampImage.getSize().y) / spriteHeight) / 256;

                        sf::Color pixelColor = vampImage.getPixel(texX, texY);
                        if (pixelColor.a > 0) // Only draw non-transparent pixels
                        {
                            // Apply distance-based lighting
                            float lightIntensity = std::min(LightingStrength / transformY, 1.0f);
                            pixelColor.r = uint8_t(pixelColor.r * lightIntensity);
                            pixelColor.g = uint8_t(pixelColor.g * lightIntensity);
                            pixelColor.b = uint8_t(pixelColor.b * lightIntensity);

                            buffer.setPixel(stripe, y, pixelColor);
                        }
                    }
                }
            }

            // Draw health bar above vampire
            const int FIXED_HEALTH_BAR_WIDTH = 40; // Fixed width in pixels
            const int FIXED_HEALTH_BAR_HEIGHT = 4; // Fixed height in pixels

            int healthBarY = drawStartY - FIXED_HEALTH_BAR_HEIGHT - 2;

            if (healthBarY >= 0) // Only draw if health bar is on screen
            {
                // Center the health bar above the vampire
                int healthBarX = spriteScreenX - FIXED_HEALTH_BAR_WIDTH / 2;

                // Draw background (red)
                for (int x = 0; x < FIXED_HEALTH_BAR_WIDTH; x++)
                {
                    for (int y = 0; y < FIXED_HEALTH_BAR_HEIGHT; y++)
                    {
                        if (healthBarX + x >= 0 && healthBarX + x < ScreenWidth)
                        {
                            buffer.setPixel(healthBarX + x, healthBarY + y, sf::Color(255, 0, 0));
                        }
                    }
                }

                // Draw health (green)
                int healthWidth = int(FIXED_HEALTH_BAR_WIDTH * (vampire->getHealth() / vampire->getMaxHealth()));
                for (int x = 0; x < healthWidth; x++)
                {
                    for (int y = 0; y < FIXED_HEALTH_BAR_HEIGHT; y++)
                    {
                        if (healthBarX + x >= 0 && healthBarX + x < ScreenWidth)
                        {
                            buffer.setPixel(healthBarX + x, healthBarY + y, sf::Color(0, 255, 0));
                        }
                    }
                }
            }
        }

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

        // Draw player in minimap
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
        for (auto &vampire : m_pVampires)
        {
            sf::CircleShape vampireShape(5);
            vampireShape.setFillColor(sf::Color::Red);
            vampireShape.setPosition(vampire->getPosition().x * 10 - 5, vampire->getPosition().y * 10 - 5);
            target.draw(vampireShape);
        }

        sf::Sprite rayGunSprite(m_rayGunTexture);
        rayGunSprite.setScale(0.8f, 0.8f);

        // Calculate gun position with bobbing effect based on player position
        float xOffset = std::sin(m_pPlayer->getPosition().x * 2.0f) * 5.0f;
        float yOffset = std::cos(m_pPlayer->getPosition().y * 2.0f) * 5.0f;

        rayGunSprite.setPosition(ScreenWidth - 256 + xOffset, ScreenHeight - 256 + yOffset);
        target.draw(rayGunSprite);

        // Draw laser shot if active
        if (m_laserShot.active)
        {
            // Calculate laser progress (0 to 1) based on lifetime
            float progress = 1.0f - (m_laserShot.lifetime / LASER_LIFETIME);

            // Start position (ray gun)
            sf::Vector2f startPos(ScreenWidth - 240, ScreenHeight - 200);

            // End position (screen center)
            sf::Vector2f endPos(ScreenWidth / 2 + 50, ScreenHeight / 2 + 30);

            // Calculate current position of laser based on progress
            sf::Vector2f currentStart = startPos + (endPos - startPos) * progress;

            // Laser length (shorter than full distance)
            float laserLength = 50.0f; // Adjust this value to change laser length

            // Calculate laser end point
            sf::Vector2f direction = endPos - startPos;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length; // Normalize
            sf::Vector2f currentEnd = currentStart + direction * laserLength;

            // Draw thick laser using multiple lines
            const int laserThickness = 4;
            const float spacing = 2.0f;

            for (int i = 0; i < laserThickness; i++)
            {
                sf::VertexArray laserLine(sf::Lines, 2);

                // Offset perpendicular to laser direction for thickness
                float perpX = -direction.y * (i - laserThickness / 2.0f) * spacing;
                float perpY = direction.x * (i - laserThickness / 2.0f) * spacing;

                laserLine[0].position = sf::Vector2f(
                    currentStart.x + perpX,
                    currentStart.y + perpY);

                laserLine[1].position = sf::Vector2f(
                    currentEnd.x + perpX,
                    currentEnd.y + perpY);

                // Green laser with fade out
                sf::Color laserColor(0, 255, 0, 255 * (m_laserShot.lifetime * 2 / LASER_LIFETIME));
                laserLine[0].color = laserColor;
                laserLine[1].color = laserColor;

                target.draw(laserLine);
            }

            // Draw laser in minimap (keep this thinner)
            sf::VertexArray minimapLaser(sf::Lines, 2);
            minimapLaser[0].position = sf::Vector2f(m_laserShot.startX * 10, m_laserShot.startY * 10);
            minimapLaser[1].position = sf::Vector2f(
                (m_laserShot.startX + m_laserShot.dirX * m_laserShot.distance) * 10,
                (m_laserShot.startY + m_laserShot.dirY * m_laserShot.distance) * 10);
            minimapLaser[0].color = sf::Color(0, 255, 0, 255 * (m_laserShot.lifetime / LASER_LIFETIME));
            minimapLaser[1].color = sf::Color(0, 255, 0, 255 * (m_laserShot.lifetime / LASER_LIFETIME));

            target.draw(minimapLaser);
        }

        if (m_state == State::PAUSED)
        {
            sf::RectangleShape halfTransparent(sf::Vector2f(ScreenWidth, ScreenHeight));
            halfTransparent.setFillColor(sf::Color(0, 0, 0, 192));
            target.draw(halfTransparent);

            target.draw(m_continueText);
        }

        // Draw player health bar at bottom center
        const int healthBarWidth = 200;
        const int healthBarHeight = 20;
        const int healthBarX = (ScreenWidth - healthBarWidth) / 2;  // Center horizontally
        const int healthBarY = ScreenHeight - healthBarHeight - 20; // 20 pixels from bottom
        const int borderThickness = 2;

        // Draw border
        sf::RectangleShape border(sf::Vector2f(healthBarWidth + 2 * borderThickness,
                                               healthBarHeight + 2 * borderThickness));
        border.setPosition(healthBarX - borderThickness, healthBarY - borderThickness);
        border.setFillColor(sf::Color::Black);
        target.draw(border);

        // Draw background (red)
        sf::RectangleShape healthBarBg(sf::Vector2f(healthBarWidth, healthBarHeight));
        healthBarBg.setPosition(healthBarX, healthBarY);
        healthBarBg.setFillColor(sf::Color(200, 0, 0));
        target.draw(healthBarBg);

        // Draw health (green)
        float healthPercent = m_pPlayer->getHealth() / m_pPlayer->getMaxHealth();
        sf::RectangleShape healthBar(sf::Vector2f(healthBarWidth * healthPercent, healthBarHeight));
        healthBar.setPosition(healthBarX, healthBarY);

        // Color based on regeneration state
        sf::Color healthColor = m_pPlayer->isRegenerating()
                                    ? sf::Color(100, 255, 100) // Brighter green when regenerating
                                    : sf::Color(0, 255, 0);    // Normal green
        healthBar.setFillColor(healthColor);
        target.draw(healthBar);

        // Draw health text
        sf::Text healthText;
        healthText.setFont(m_font);
        healthText.setString(std::to_string((int)m_pPlayer->getHealth()) + "/" +
                             std::to_string((int)m_pPlayer->getMaxHealth()));
        healthText.setCharacterSize(16);
        healthText.setFillColor(sf::Color::White);

        // Center text in health bar
        sf::FloatRect textBounds = healthText.getLocalBounds();
        healthText.setPosition(
            healthBarX + (healthBarWidth - textBounds.width) / 2,
            healthBarY + (healthBarHeight - textBounds.height) / 2);
        target.draw(healthText);

        // Draw start message if active
        if (m_showStartMessage)
        {
            // Draw semi-transparent background for better readability
            sf::RectangleShape messageBg(sf::Vector2f(ScreenWidth, 60));
            messageBg.setPosition(0, m_startMessage.getPosition().y - 10);
            messageBg.setFillColor(sf::Color(0, 0, 0, 128));
            target.draw(messageBg);

            // Draw message with fade out
            sf::Text fadeMessage = m_startMessage;
            fadeMessage.setFillColor(sf::Color(255, 255, 255,
                                               static_cast<sf::Uint8>(255 * std::min(m_messageTimer, 1.0f))));
            target.draw(fadeMessage);
        }

        // Draw timer with animation
        sf::Text timerText;
        timerText.setFont(m_font);
        timerText.setFillColor(sf::Color::White);
        timerText.setString(std::to_string(static_cast<int>(m_survivalTime)));

        // Calculate animation progress (0 to 1)
        float animProgress = 1.0f - (m_timerAnimationTime / TIMER_ANIMATION_DURATION);

        // Calculate current scale using easing
        float scale = INITIAL_TIMER_SCALE + (2.0f - INITIAL_TIMER_SCALE) * (animProgress * animProgress);

        // Set character size with scale
        timerText.setCharacterSize(static_cast<unsigned int>(36 * scale));

        // Calculate positions
        float startX = ScreenWidth / 2.0f;
        float startY = ScreenHeight / 2.0f;
        float endX = ScreenWidth - 40.0f;
        float endY = 10.0f;

        // Interpolate position with easing
        float currentX = startX + (endX - startX) * (animProgress * animProgress);
        float currentY = startY + (endY - startY) * (animProgress * animProgress);

        // Center the text at its current position
        sf::FloatRect textBounds2 = timerText.getLocalBounds();
        timerText.setOrigin(textBounds2.width / 2.0f, textBounds2.height / 2.0f);
        timerText.setPosition(currentX, currentY);

        target.draw(timerText);

        // Draw money counter above health bar
        sf::Text moneyLabel;
        moneyLabel.setFont(m_font);
        moneyLabel.setString("MONEY:");
        moneyLabel.setCharacterSize(20);
        moneyLabel.setFillColor(sf::Color::Yellow);
        moneyLabel.setPosition(
            (ScreenWidth - healthBarWidth) / 2, // Same x-alignment as health bar
            healthBarY - 40);                   // 40 pixels above health bar
        target.draw(moneyLabel);

        sf::Text moneyText;
        moneyText.setFont(m_font);
        moneyText.setString("$" + std::to_string(m_pPlayer->getMoney()));
        moneyText.setCharacterSize(24);
        moneyText.setFillColor(sf::Color::Yellow);

        // Position money amount next to "MONEY:" label
        sf::FloatRect moneyBounds = moneyText.getLocalBounds();
        moneyText.setPosition(
            moneyLabel.getPosition().x + moneyLabel.getLocalBounds().width + 10, // 10 pixels after label
            healthBarY - 42);                                                    // Slightly adjusted to align with label
        target.draw(moneyText);
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

void Game::vampireSpawner(float deltaTime)
{
    if (m_spawnCount >= MaxVampires)
        return;

    if (m_vampireCooldown > 0.0f)
    {
        m_vampireCooldown -= deltaTime;
        return;
    }

    // Find all spawn points in the map
    std::vector<sf::Vector2f> spawnPoints;
    for (int y = 0; y < GridHeight; y++)
    {
        for (int x = 0; x < GridWidth; x++)
        {
            if (MapArray1[y * GridWidth + x] == 3)
            {
                spawnPoints.push_back(sf::Vector2f(x + 0.5f, y + 0.5f));
            }
        }
    }

    // Pick a random spawn point
    int randomIndex = rand() % spawnPoints.size();
    sf::Vector2f spawnPosition = spawnPoints[randomIndex];

    // Create vampire with current base health and speed
    auto vampire = std::make_unique<Vampire>(this, spawnPosition);
    vampire->setMaxHealth(m_baseVampireHealth);
    vampire->setSpeed(m_baseVampireSpeed);
    m_pVampires.push_back(std::move(vampire));

    m_spawnCount++;
    if (m_spawnCount % 5 == 0)
    {
        m_nextVampireCooldown -= 0.1f;
    }
    m_vampireCooldown = m_nextVampireCooldown;
}

void Game::shootLaser()
{
    if (m_state != State::PLAYING || m_laserShot.active)
        return;

    // Use player's position and direction for the laser
    float startX = m_pPlayer->getPosition().x;
    float startY = m_pPlayer->getPosition().y;
    float dirX = m_pPlayer->getDirX();
    float dirY = m_pPlayer->getDirY();

    // Raycasting for laser hit detection
    float deltaDistX = std::abs(1.0f / dirX);
    float deltaDistY = std::abs(1.0f / dirY);

    int mapX = int(startX);
    int mapY = int(startY);

    float sideDistX, sideDistY;
    int stepX, stepY;

    if (dirX < 0)
    {
        stepX = -1;
        sideDistX = (startX - mapX) * deltaDistX;
    }
    else
    {
        stepX = 1;
        sideDistX = (mapX + 1.0f - startX) * deltaDistX;
    }
    if (dirY < 0)
    {
        stepY = -1;
        sideDistY = (startY - mapY) * deltaDistY;
    }
    else
    {
        stepY = 1;
        sideDistY = (mapY + 1.0f - startY) * deltaDistY;
    }

    // Perform DDA
    float distance = 0.0f;
    bool hit = false;
    bool hitVampire = false;
    int side;

    while (!hit && distance < 20.0f) // Maximum laser range of 20 units
    {
        if (sideDistX < sideDistY)
        {
            sideDistX += deltaDistX;
            mapX += stepX;
            side = 0;
            distance = (mapX - startX + (1 - stepX) / 2) / dirX;
        }
        else
        {
            sideDistY += deltaDistY;
            mapY += stepY;
            side = 1;
            distance = (mapY - startY + (1 - stepY) / 2) / dirY;
        }

        // Check for wall hits
        if (mapX >= 0 && mapX < GridWidth && mapY >= 0 && mapY < GridHeight)
        {
            if (MapArray1[mapY * GridWidth + mapX] > 0)
            {
                hit = true;
            }
        }

        // Check for vampire hits
        float currentX = startX + dirX * distance;
        float currentY = startY + dirY * distance;

        for (auto it = m_pVampires.begin(); it != m_pVampires.end();)
        {
            sf::Vector2f vampPos = (*it)->getPosition();
            float dx = vampPos.x - currentX;
            float dy = vampPos.y - currentY;
            float distToVamp = sqrt(dx * dx + dy * dy);

            if (distToVamp < 0.5f) // Hit radius
            {
                // Damage vampire
                (*it)->damage(34.0f);

                // Remove vampire if dead and award money
                if ((*it)->isDead())
                {
                    // Award money equal to vampire's max health
                    int moneyReward = static_cast<int>((*it)->getMaxHealth()); // Linear scaling: 100 money for 100hp, 500 for 500hp
                    m_pPlayer->addMoney(moneyReward);
                    it = m_pVampires.erase(it);

                    m_spawnCount--;
                }
                else
                {
                    ++it;
                }

                hitVampire = true;
                hit = true;
                break;
            }
            else
            {
                ++it;
            }
        }
    }

    // Set the new laser shot
    m_laserShot = {
        startX, startY, // start position
        dirX, dirY,     // direction
        distance,       // distance to hit
        LASER_LIFETIME, // lifetime
        true            // active
    };
}
