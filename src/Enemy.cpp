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
}

void Enemy::Retarget()
{
	_transform.LookAt(_player->GetPosition());
	_retargetTimer = RandomFloat(_retargetMin, _retargetMax);
}

void Enemy::Update(float dt)
{
	if (!_alive) return;

	_retargetTimer -= dt;
	if (_retargetTimer < 0.0f) Retarget();

	_transform.MoveForward(_speed * dt);
	_sprite.Update(dt);
}

void Enemy::Activate(Vector2 position)
{
	_alive = true;
	_transform.position = position;
	_retargetTimer = 0.0f;
}

void Enemy::Deactivate()
{
	_alive = false;
	_transform.position = GameConfig::OFFSCREEN_POSITION;
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