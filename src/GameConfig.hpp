#pragma once

#include "raylib.h"

namespace GameConfig
{
	//
	inline bool SHOW_DEBUG = false;

	//
	constexpr float OFFSCREEN_POS = -9999.0f;
	constexpr Vector2 OFFSCREEN_POSITION = { OFFSCREEN_POS, OFFSCREEN_POS };

	//
	constexpr float BOUNDS_MARGIN = 100.0f;

	//
	constexpr int BASE_W = 1280;
	constexpr int BASE_H = 720;

	//
	inline float MAP_W = 0.0f;
	inline float MAP_H = 0.0f;

	inline bool IsOutsideMap(Vector2 pos)
	{
		return pos.x < -BOUNDS_MARGIN || pos.x > MAP_W + BOUNDS_MARGIN || pos.y < - BOUNDS_MARGIN || pos.y > MAP_H + BOUNDS_MARGIN;
	}

	//
	constexpr int PLAYER_MAX_HEALTH = 3;
	constexpr float PLAYER_INV_TIME = 1.5f;

	constexpr float PLAYER_COLLIDER_RADIUS = 22.0f;
	constexpr float PLAYER_SCALE = 0.3f;
	constexpr float PLAYER_SPEED = 200.0f;
	constexpr float AIM_SENSITIVITY = 0.15f;

	constexpr Vector2 PLAYER_PIVOT = { 0.31f, 0.58f };
	constexpr Vector2 PLAYER_MUZZLE_OFFSET = { 54.0f, 8.0f };

	//
	constexpr unsigned int WALKABLE_THRESHOLD = 240;

	//
	constexpr int WAVE_ENEMY_BASE = 8;
	constexpr int WAVE_ENEMY_RAMP = 4;
}