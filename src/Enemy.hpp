#pragma once

#include "Transform2D.hpp"
#include "Sprite.hpp"
#include "CircleCollider.hpp"


class Player;

enum class EnemyState
{
	Moving,
	Dying
};

class Enemy
{
public:
	Enemy();
	void Update(float dt);
	void Draw();
	void SetPosition(Vector2 position);
	void SetPlayer(const Player *player);

	Vector2 GetPosition() const { return _transform.position; };

	bool IsAlive() const { return _alive; }
	void Deactivate();

	void Kill();
	bool CanBeHit() const { return _state == EnemyState::Moving; };

	void Activate(Vector2 pos);

	const CircleCollider& GetCollider() const { return _collider; };

private:
	Transform2D _transform;

	Sprite _spriteMove;
	Sprite _spriteDeath;

	CircleCollider _collider;

	const Player* _player = nullptr;

	EnemyState _state = EnemyState::Moving;

	float _speed = 80.0f;
	float _retargetTimer = 0.0f;
	float _retargetMin = 1.0f;
	float _retargetMax = 2.0f;

	void Retarget();

	bool _alive = true;

	int health = 2; 
};