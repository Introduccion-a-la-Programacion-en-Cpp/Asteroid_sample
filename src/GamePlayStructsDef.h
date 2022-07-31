#pragma once
#include "StructsDef.h"



typedef struct _SpaceShip {
	Sprite* sprite;
	SDL_FPoint currentSpaceshipDir = { 0.0f, 0.0f };
	float currentSpaceshipVelocity = 0.0f;
	float shoot_cooldown = 0.0f;
}SpaceShip;

typedef _SpaceShip Ammo;
typedef vector<Ammo> SpaceShipAmmo;


typedef struct _Asteroid {
	Sprite* sprite;
	SDL_FPoint currentSpaceshipDir = { 0.0f, 0.0f };
	float currentSpaceshipVelocity = 0.0f;
	float scale = 1.0f;
} Asteroid;

typedef vector<Asteroid> EnemiesAsteroid;