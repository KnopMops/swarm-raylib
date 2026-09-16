#pragma once

#include "Transform2D.hpp"
#include "raylib.h"


struct CircleCollider
{
	float radius = 16.0f;
	const Transform2D* transform = nullptr;

	void Init(float r, const Transform2D& t) { radius = r; transform = &t; };

	bool IsCollidingWith(const CircleCollider& other) const;

	void DrawDebug() const;
};