#pragma once

#include "raylib.h"
#include "cmath"


inline Vector2 Direction(float angleDeg)
{
	float rad = angleDeg * DEG2RAD;
	return { cosf(rad), sinf(rad) };
};

inline float RandomFloat(float min, float max)
{
	return min + (max - min) * (float)GetRandomValue(0, RAND_MAX) / (float)RAND_MAX;
}

inline float AngleToTargetDeg(Vector2 from, Vector2 to)
{
	Vector2 delta = to - from;
	return atan2f(delta.y, delta.x) * RAD2DEG;
}