#pragma once

#include "raylib.h"

#include "Transform2D.hpp"
#include "Sprite.hpp"
#include "Movement.hpp"


class CollisionMap;

class Player
{
public:
	Player(const std::string& textureName);

	void Draw() const;
	void Update(float delta);

	Vector2 GetPosition() const;
	Vector2 GetFiringPosition() const;

	void SetPosition(Vector2 position);
	void SetCollisionMap(const CollisionMap* collisionMap);

private:
	Transform2D _transform;
	Sprite _sprite;
	Movement _movement;

	const CollisionMap* _collisionMap = nullptr;

	Vector2 _muzzleOffset = { 0.0f, 0.0f };
};