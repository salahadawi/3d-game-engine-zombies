#pragma once

#include "Rectangle.h"
#include "Constants.h"

struct InputData;

class Game;

class Player : public Rectangle
{
public:
    Player(Game *pGame);
    virtual ~Player() {}

    bool initialise() {};
    void move(InputData inputData, float deltaTime);
    void updatePhysics(float deltaTime);
    void update(float deltaTime);

    int getCoins() const { return m_coins; };

    bool isDead() const { return m_isDead; }
    void setIsDead(bool isDead) { m_isDead = isDead; }

    // 0.5 to center the player on the tile
    void setSpawnPoint(sf::Vector2f spawnPoint) { setPosition(sf::Vector2f(spawnPoint.x + 0.5f, spawnPoint.y + 0.5f)); }

    void updateSpeed(float deltaTime) { m_move_speed = deltaTime / 1000 / 150; }
    void updateTurnSpeed(float deltaTime) { m_rotation_speed = deltaTime / 1000 / 150; }

    float getDirX() { return m_dir_x; }
    float getDirY() { return m_dir_y; }
    float getPlaneX() { return m_plane_x; }
    float getPlaneY() { return m_plane_y; }

    float getX() { return m_x; }
    float getY() { return m_y; }

private:
    bool m_isGrounded;
    bool m_isDead = false;
    float m_jumpTimer = 0.0f;
    Game *m_pGame;
    int m_coins = 0;

    float m_x;
    float m_y;
    float m_dir_x = 1;
    float m_dir_y = 0.000001;
    float m_plane_x = 0;
    float m_plane_y = 0.66;
    float m_spawn_x;
    float m_spawn_y;
    float m_move_speed;
    float m_rotation_speed = PlayerRotationSpeed;
    int m_cam_height;
    int m_crouching;
    int m_busy;
    float m_vertical_fov = VerticalFOVDiv;
};
