# Zombie Survival Raycasting Game

https://github.com/user-attachments/assets/6ac988ae-1a5e-4012-87eb-2bedafcea499

A fast-paced survival game built with SFML using raycasting! Your objective is to survive for as long as possible while zombies relentlessly target you.

## Gameplay

Navigate through a 3D environment with classic FPS-style controls. Zombies will track you down, and you'll need to survive by shooting, upgrading your weapon, and slowing down the enemies when necessary. 

## Controls

- **WASD** - Move
- **Mouse/Arrow keys** - Turn camera
- **Left mouse click/Spacebar** - Shoot
- **Z** - Upgrade gun
- **X** - Slow down zombies
- **Esc** - Pause game

> **Note:** Playing with a mouse is recommended for a more familiar and immersive FPS experience.

## Customization

If you’d like to tweak the camera controls to your preference, you can modify the constants in `Constants.h`.

## Installation and Setup

1. Install SFML on your system if you haven't already:
    ```bash
    # For Ubuntu/Debian-based systems
    sudo apt-get install libsfml-dev
    ```
2. Compile:
    ```bash
    cmake -B build && ./incremental_build # Ctrl + C to quit once build 100% finished
    ./build/bin/survive
    ```
2. Run:
    ```bash
    ./build/bin/survive
    ```
