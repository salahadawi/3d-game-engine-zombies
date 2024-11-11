#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cstdio>

#include "InputHandler.h"
#include "Player.h"
#include "ResourceManager.h"
#include "Vampire.h"
#include "LaserShot.h"
#include "Minimap.h"

Game::Game() : m_state(State::PLAYING),
               m_pClock(std::make_unique<sf::Clock>()),
               m_pPlayer(std::make_unique<Player>(this)),
               m_score(0),
               m_clearedLevels(0),
               m_minimap(Minimap(this))
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

    m_startMessage.setFont(m_font);
    m_startMessage.setString("Vampires are invading the building, survive!");
    m_startMessage.setCharacterSize(30);
    m_startMessage.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = m_startMessage.getLocalBounds();
    m_startMessage.setPosition(
        (ScreenWidth - textBounds.width) / 2,
        (ScreenHeight - textBounds.height) / 3);

    m_gameOverText.setFont(m_font);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(50);
    m_gameOverText.setFillColor(sf::Color::Red);

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
    if (m_state == State::GAME_OVER)
        return;

    m_pPlayer->setIsDead(false);

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
        m_survivalTime += deltaTime;

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
            m_finalScoreText.setString("You survived for " + std::to_string(static_cast<int>(m_survivalTime)) + " seconds.");

            sf::FloatRect scoreBounds = m_finalScoreText.getLocalBounds();
            m_finalScoreText.setPosition(
                (ScreenWidth - scoreBounds.width) / 2,
                ScreenHeight / 2);
        }

        if (m_timerAnimationTime > 0.0f)
        {
            m_timerAnimationTime = std::max(0.0f, m_timerAnimationTime - deltaTime);
        }

        // Increase base vampire health over time
        m_baseVampireHealth = std::min(m_baseVampireHealth + HealthIncreaseRate * deltaTime,
                                       MaxVampireHealth);

        // Increase base vampire speed over time
        m_baseVampireSpeed = std::min(m_baseVampireSpeed + SpeedIncreaseRate * deltaTime,
                                      MaxVampireSpeed);

        for (auto &vampire : m_pVampires)
        {
            vampire->setSpeed(m_baseVampireSpeed);
        }

        // Update slow effect
        if (m_slowTimer > 0)
        {
            m_slowTimer -= deltaTime;
            if (m_slowTimer <= 0)
            {
                m_vampireSpeedMultiplier = 1.0f;
            }
        }

        for (auto it = m_messages.begin(); it != m_messages.end();)
        {
            it->duration -= deltaTime;
            if (it->duration <= 0)
            {
                it = m_messages.erase(it);
            }
            else
            {
                ++it;
            }
        }

        m_baseVampireSpeed = std::min(m_baseVampireSpeed + SpeedIncreaseRate * deltaTime,
                                      MaxVampireSpeed) *
                             m_vampireSpeedMultiplier;
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
                sf::Color wallColor;
                if (MapArray1[mapY * GridWidth + mapX] == 3) // Spawner
                {
                    wallColor = (side == 1)
                                    ? sf::Color(64, 0, 64)
                                    : sf::Color(128, 0, 128);
                }
                else // Normal wall
                {
                    wallColor = (side == 1)
                                    ? sf::Color(82, 43, 28)
                                    : sf::Color(116, 60, 39);
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
            float spriteX = vampire->getPosition().x - m_pPlayer->getPosition().x;
            float spriteY = vampire->getPosition().y - m_pPlayer->getPosition().y;

            float invDet = 1.0f / (m_pPlayer->getPlaneX() * m_pPlayer->getDirY() - m_pPlayer->getDirX() * m_pPlayer->getPlaneY());
            float transformX = invDet * (m_pPlayer->getDirY() * spriteX - m_pPlayer->getDirX() * spriteY);
            float transformY = invDet * (-m_pPlayer->getPlaneY() * spriteX + m_pPlayer->getPlaneX() * spriteY);

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
                if (transformY > 0)
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

            int healthBarY = drawStartY - FixedHealthBarHeight - 2;

            if (healthBarY >= 0)
            {
                int healthBarX = spriteScreenX - FixedHealthBarWidth / 2;

                for (int x = 0; x < FixedHealthBarWidth; x++)
                {
                    for (int y = 0; y < FixedHealthBarHeight; y++)
                    {
                        if (healthBarX + x >= 0 && healthBarX + x < ScreenWidth)
                        {
                            buffer.setPixel(healthBarX + x, healthBarY + y, sf::Color(255, 0, 0));
                        }
                    }
                }

                int healthWidth = int(FixedHealthBarWidth * (vampire->getHealth() / vampire->getMaxHealth()));
                for (int x = 0; x < healthWidth; x++)
                {
                    for (int y = 0; y < FixedHealthBarHeight; y++)
                    {
                        if (healthBarX + x >= 0 && healthBarX + x < ScreenWidth)
                        {
                            buffer.setPixel(healthBarX + x, healthBarY + y, sf::Color(0, 255, 0));
                        }
                    }
                }
            }
        }

        m_minimap.drawMinimap(target, buffer);

        sf::Sprite rayGunSprite(m_rayGunTexture);
        rayGunSprite.setScale(0.8f, 0.8f);

        // Calculate gun position with bobbing effect based on player position
        float xOffset = std::sin(m_pPlayer->getPosition().x * 2.0f) * 5.0f;
        float yOffset = std::cos(m_pPlayer->getPosition().y * 2.0f) * 5.0f;

        rayGunSprite.setPosition(ScreenWidth - 256 + xOffset, ScreenHeight - 256 + yOffset);
        target.draw(rayGunSprite);

        if (m_laserShot.active)
        {
            float progress = 1.0f - (m_laserShot.lifetime / LaserLifetime);

            sf::Vector2f startPos(ScreenWidth - 240, ScreenHeight - 200);
            sf::Vector2f endPos(ScreenWidth / 2 + 50, ScreenHeight / 2 + 30);
            sf::Vector2f currentStart = startPos + (endPos - startPos) * progress;

            float laserLength = 50.0f;

            sf::Vector2f direction = endPos - startPos;
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            direction /= length;
            sf::Vector2f currentEnd = currentStart + direction * laserLength;

            const int laserThickness = 4;
            const float spacing = 2.0f;

            for (int i = 0; i < laserThickness; i++)
            {
                sf::VertexArray laserLine(sf::Lines, 2);

                float perpX = -direction.y * (i - laserThickness / 2.0f) * spacing;
                float perpY = direction.x * (i - laserThickness / 2.0f) * spacing;

                laserLine[0].position = sf::Vector2f(
                    currentStart.x + perpX,
                    currentStart.y + perpY);

                laserLine[1].position = sf::Vector2f(
                    currentEnd.x + perpX,
                    currentEnd.y + perpY);

                // Green laser with fade out
                sf::Color laserColor(0, 255, 0, 255 * (m_laserShot.lifetime * 2 / LaserLifetime));
                laserLine[0].color = laserColor;
                laserLine[1].color = laserColor;

                target.draw(laserLine);
            }

            sf::VertexArray minimapLaser(sf::Lines, 2);
            minimapLaser[0].position = sf::Vector2f(m_laserShot.startX * 10, m_laserShot.startY * 10);
            minimapLaser[1].position = sf::Vector2f(
                (m_laserShot.startX + m_laserShot.dirX * m_laserShot.distance) * 10,
                (m_laserShot.startY + m_laserShot.dirY * m_laserShot.distance) * 10);
            minimapLaser[0].color = sf::Color(0, 255, 0, 255 * (m_laserShot.lifetime / LaserLifetime));
            minimapLaser[1].color = sf::Color(0, 255, 0, 255 * (m_laserShot.lifetime / LaserLifetime));

            target.draw(minimapLaser);
        }

        if (m_state == State::PAUSED)
        {
            sf::RectangleShape halfTransparent(sf::Vector2f(ScreenWidth, ScreenHeight));
            halfTransparent.setFillColor(sf::Color(0, 0, 0, 192));
            target.draw(halfTransparent);

            target.draw(m_continueText);
        }

        const int healthBarWidth = 200;
        const int healthBarHeight = 20;
        const int healthBarX = (ScreenWidth - healthBarWidth) / 2;
        const int healthBarY = ScreenHeight - healthBarHeight - 20;
        const int borderThickness = 2;

        sf::RectangleShape border(sf::Vector2f(healthBarWidth + 2 * borderThickness,
                                               healthBarHeight + 2 * borderThickness));
        border.setPosition(healthBarX - borderThickness, healthBarY - borderThickness);
        border.setFillColor(sf::Color::Black);
        target.draw(border);

        sf::RectangleShape healthBarBg(sf::Vector2f(healthBarWidth, healthBarHeight));
        healthBarBg.setPosition(healthBarX, healthBarY);
        healthBarBg.setFillColor(sf::Color(200, 0, 0));
        target.draw(healthBarBg);

        float healthPercent = m_pPlayer->getHealth() / m_pPlayer->getMaxHealth();
        sf::RectangleShape healthBar(sf::Vector2f(healthBarWidth * healthPercent, healthBarHeight));
        healthBar.setPosition(healthBarX, healthBarY);

        sf::Color healthColor = m_pPlayer->isRegenerating()
                                    ? sf::Color(100, 255, 100)
                                    : sf::Color(0, 255, 0);
        healthBar.setFillColor(healthColor);
        target.draw(healthBar);

        sf::Text healthText;
        healthText.setFont(m_font);
        healthText.setString(std::to_string((int)m_pPlayer->getHealth()) + "/" +
                             std::to_string((int)m_pPlayer->getMaxHealth()));
        healthText.setCharacterSize(16);
        healthText.setFillColor(sf::Color::White);

        sf::FloatRect textBounds = healthText.getLocalBounds();
        healthText.setPosition(
            healthBarX + (healthBarWidth - textBounds.width) / 2,
            healthBarY + (healthBarHeight - textBounds.height) / 2);
        target.draw(healthText);

        if (m_showStartMessage)
        {
            // Draw semi-transparent background for better readability
            sf::RectangleShape messageBg(sf::Vector2f(ScreenWidth, 60));
            messageBg.setPosition(0, m_startMessage.getPosition().y - 10);
            messageBg.setFillColor(sf::Color(0, 0, 0, 128));
            target.draw(messageBg);

            sf::Text fadeMessage = m_startMessage;
            fadeMessage.setFillColor(sf::Color(255, 255, 255,
                                               static_cast<sf::Uint8>(255 * std::min(m_messageTimer, 1.0f))));
            target.draw(fadeMessage);
        }

        sf::Text timerText;
        timerText.setFont(m_font);
        timerText.setFillColor(sf::Color::White);
        timerText.setString(std::to_string(static_cast<int>(m_survivalTime)));

        float animProgress = 1.0f - (m_timerAnimationTime / TimerAnimationDuration);

        float scale = InitialTimerScale + (2.0f - InitialTimerScale) * (animProgress * animProgress);

        timerText.setCharacterSize(static_cast<unsigned int>(36 * scale));

        float startX = ScreenWidth / 2.0f;
        float startY = ScreenHeight / 2.0f;
        float endX = ScreenWidth - 40.0f;
        float endY = 10.0f;

        float currentX = startX + (endX - startX) * (animProgress * animProgress);
        float currentY = startY + (endY - startY) * (animProgress * animProgress);

        sf::FloatRect textBounds2 = timerText.getLocalBounds();
        timerText.setOrigin(textBounds2.width / 2.0f, textBounds2.height / 2.0f);
        timerText.setPosition(currentX, currentY);

        target.draw(timerText);

        sf::Text moneyLabel;
        moneyLabel.setFont(m_font);
        moneyLabel.setString("MONEY:");
        moneyLabel.setCharacterSize(20);
        moneyLabel.setFillColor(sf::Color::Yellow);
        moneyLabel.setPosition(
            healthBarX,
            healthBarY - 40);
        target.draw(moneyLabel);

        sf::Text moneyText;
        moneyText.setFont(m_font);
        moneyText.setString("$" + std::to_string(m_pPlayer->getMoney()));
        moneyText.setCharacterSize(24);
        moneyText.setFillColor(sf::Color::Yellow);

        sf::FloatRect moneyBounds = moneyText.getLocalBounds();
        moneyText.setPosition(
            moneyLabel.getPosition().x + moneyLabel.getLocalBounds().width + 10,
            healthBarY - 42);
        target.draw(moneyText);

        // Draw messages
        float messageY = ScreenHeight / 3.0f;
        for (const auto &msg : m_messages)
        {
            sf::Text messageText;
            messageText.setFont(m_font);
            messageText.setString(msg.text);
            messageText.setCharacterSize(24);
            messageText.setFillColor(sf::Color::Yellow);

            sf::FloatRect textBounds = messageText.getLocalBounds();
            messageText.setPosition(
                (ScreenWidth - textBounds.width) / 2,
                messageY);

            if (msg.duration < 0.5f)
            {
                messageText.setFillColor(sf::Color(255, 255, 0,
                                                   static_cast<sf::Uint8>(255 * (msg.duration / 0.5f))));
            }

            target.draw(messageText);
            messageY += 30; // Space between messages
        }

        // Draw purchase information in bottom left
        if (m_state == State::PLAYING)
        {
            const int TEXT_OFFSET_Y = 60;
            const int LINE_HEIGHT = 25;

            std::vector<sf::Text> purchaseTexts;

            sf::Text damageText;
            damageText.setFont(m_font);
            damageText.setCharacterSize(20);
            damageText.setFillColor(sf::Color::Yellow);
            damageText.setString("[Z] Upgrade Damage ($" +
                                 std::to_string(m_pPlayer->getDamageUpgradeCost()) +
                                 ")");

            sf::Text slowText;
            slowText.setFont(m_font);
            slowText.setCharacterSize(20);
            slowText.setFillColor(sf::Color::Yellow);
            slowText.setString("[X] Slow Vampires ($" +
                               std::to_string(m_pPlayer->getSlowUpgradeCost()) +
                               ")");

            damageText.setPosition(20, ScreenHeight - TEXT_OFFSET_Y);
            slowText.setPosition(20, ScreenHeight - TEXT_OFFSET_Y + LINE_HEIGHT);

            target.draw(damageText);
            target.draw(slowText);
        }
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

void Game::vampireSpawner(float deltaTime)
{
    // Don't spawn if at max vampires or during slow effect
    if (m_spawnCount >= MaxVampires || m_slowTimer > 0)
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

                (*it)->damage(m_pPlayer->getGunDamage());

                if ((*it)->isDead())
                {
                    // Award money equal to vampire's max health
                    int moneyReward = static_cast<int>((*it)->getMaxHealth());
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

    m_laserShot = {
        startX, startY,
        dirX, dirY,
        distance,
        LaserLifetime,
        true};
}

void Game::showMessage(const std::string &msg, float duration)
{
    m_messages.push_back({msg, duration});
}

void Game::slowVampires()
{
    m_vampireSpeedMultiplier = 0.5f; // Slow to 50% speed
    m_slowTimer = SlowDuration;
}
