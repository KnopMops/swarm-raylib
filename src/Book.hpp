#pragma once

#include "raylib.h"
#include "raymath.h"

#include "PoolObject.hpp"

#include "Transform2D.hpp"
#include "Sprite.hpp"

#include "CircleCollider.hpp"

#include "cmath"

class Book : public PoolObject
{
public:
	Book();

	void Update(float dt) override {};
	void Draw() override;
	void Deactivate() override;
	
	void Activate(Vector2 pos, const char* textureName);
	
	Vector2 GetPosition() const { return _transform.position; };
	
	const CircleCollider& GetCollider() const { return _collider; };

private:
	Transform2D _transform;
	Sprite _sprite;
	CircleCollider _collider;
};