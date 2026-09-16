#include "CircleCollider.hpp"
#include "GameConfig.hpp"

#include "raymath.h"

void CircleCollider::DrawDebug() const
{
	if (!GameConfig::SHOW_DEBUG) return;

	DrawCircleLines((int)transform->position.x, (int)transform->position.y, radius, GREEN);
}

bool CircleCollider::IsCollidingWith(const CircleCollider& other) const
{
	if (!transform || !other.transform) return false;

	float distSq = Vector2DistanceSqr(transform->position, other.transform->position);
	float radiusSum = radius + other.radius;

	return distSq < radiusSum * radiusSum;
}