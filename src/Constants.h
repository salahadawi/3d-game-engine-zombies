#pragma once

const int ScreenWidth = 800;
const int ScreenHeight = 600;
const float VerticalFOVDiv = 2.0;

const float Gravity = 180.0f;
const float JumpYVelocity = -230.0f;

const float LightingStrength = 2.0f;

const float PlayerHeight = 60.0f;
const float PlayerWidth = 40.0f;
const float PlayerSpeed = 155.0f;
const float MoveSpeed = 0.1;  // use this later for calculating player movement speed
const float TurnSpeed = 0.05; // use this later for calculating player turn speed
const float PlayerJumpTime = 0.7f;
const float PlayerRotationSpeed = 0.05;
const float PlayerSpawnPoint = -1;

const float VampireHeight = 32.0f;
const float VampireWidth = 32.0f;
const float VampireSpeed = 1.0f;

const int MaxVampires = 20;

const float CoinRadius = 17.0f;

const float LASER_LIFETIME = 0.1; // Laser visible for 0.1 seconds

const float TimerAnimationDuration = 2.0f;
const float InitialTimerScale = 4.0f; // Initial size multiplier

const float HealthIncreaseRate = 10.0f; // HP increase per second
const float MaxVampireHealth = 500.0f;	// Maximum vampire health

const float SpeedIncreaseRate = 0.5f; // Speed increase per second
const float MaxVampireSpeed = VampireSpeed * 3.0f;

const int GridHeight = 16;
const int GridWidth = 32;

// const float TileSizeX = ScreenWidth / (float)GridSize;
// const float TileSizeY = ScreenHeight / (float)GridSize;

enum eTile
{
	eEmpty = 0,
	eBlock = 1,
	eCoin = 2,
	ePlayerSpawn = 3,
	eDoor = 4
};

const int LevelCount = 2;
// Level 1 definition
// const int MapArray1[GridHeight * GridWidth]{
// 	1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 1, -1, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1,
// 	1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
// 	1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1};

const int MapArray1[GridHeight * GridWidth]{
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

// Camera control constants
const float MouseTurnSpeed = 0.001f;  // Mouse sensitivity
const float KeyboardTurnSpeed = 2.0f; // Keyboard turn rate (radians per second)