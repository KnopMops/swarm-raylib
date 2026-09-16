#include "Enemy.hpp"
#include "ResourceKeys.hpp"
#include "Player.hpp"
#include "GameConfig.hpp"

Enemy::Enemy()
{
	_sprite.Init(RK::COCKROACH_MOVE, 64, 64, 8, 8.0f);
	_sprite.rotationOffset = 90.0f;
	_transform.scale = 1.0f;
	_transform.rotation = 180.0f;

	_collider.Init(30.0f, _transform);

	health = 2;
}

void Enemy::Kill()
{
	if (_state == EnemyState::Dying) return;

	health -= 1;
	
	if (health <= 0) {
		_state = EnemyState::Dying;
		_dyingTimer = 0.6f;

		TraceLog(LOG_INFO, "Enemy dyied");
	}
}

void Enemy::Retarget()
{
	_transform.LookAt(_player->GetPosition());
	_retargetTimer = RandomFloat(_retargetMin, _retargetMax);
}

void Enemy::Update(float dt)
{
	if (!_alive) return;

	switch (_state)
	{
	case EnemyState::Moving:
		_retargetTimer -= dt;
		if (_retargetTimer < 0.0f) Retarget();

		_transform.MoveForward(_speed * dt);
		_sprite.Update(dt);

		break;

	case EnemyState::Dying:
		_dyingTimer -= dt;
		if (_dyingTimer < 0.0f) Deactivate();
	
	default:
		break;
	}
}

void Enemy::Activate(Vector2 position)
{
	_alive = true;
	_transform.position = position;
	_retargetTimer = 0.0f;

	_state = EnemyState::Moving;

	health = 2;
}

void Enemy::Deactivate()
{
	if (health > 0) health -= 1;
	else {
		_alive = false;
		_transform.position = GameConfig::OFFSCREEN_POSITION;
	}
}

void Enemy::Draw()
{
	if (!_alive) return;
	_sprite.Draw(_transform);
	_collider.DrawDebug();
}

void Enemy::SetPosition(Vector2 position)
{
	_transform.position = position;
}

void Enemy::SetPlayer(const Player* player)
{
	_player = player;
}