#pragma once

struct LaserShot
{
    float startX, startY; // Starting position
    float dirX, dirY;     // Direction
    float distance;       // Distance to hit
    float lifetime;       // How long to show the laser
    bool active;          // Whether there's currently an active laser
};