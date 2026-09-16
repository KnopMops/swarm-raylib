#pragma once

#include "raylib.h"
#include "raymath.h"

#include "SwarmUtils.hpp"


struct Transform2D 
{
	Vector2 position = { 0, 0 };
	float rotation = 0.0f;
	float scale = 1.0f;

	void MoveForward(float distance)
	{
		position += Direction(rotation) * distance;
	}

	void Translate(Vector2 offset)
	{
		position += offset;
	}


	void LookAt(Vector2 target)
	{
		rotation = AngleToTargetDeg(position, target);
	}
};