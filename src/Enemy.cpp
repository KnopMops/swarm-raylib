#include "Enemy.hpp"
#include "ResourceKeys.hpp"
#include "Player.hpp"
#include "GameConfig.hpp"

Enemy::Enemy()
{
	_spriteMove.Init(RK::COCKROACH_MOVE, 64, 64, 8, 8.0f);
	_spriteMove.rotationOffset = 90.0f;

	_spriteDeath.Init(RK::COCKROACH_DEATH, 64, 64, 32, 64.0f, false);
	_spriteDeath.rotationOffset = 90.0f;

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
		_spriteDeath.Reset();
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
		_spriteMove.Update(dt);

		break;

	case EnemyState::Dying:
		_spriteDeath.Update(dt);
		if (_spriteDeath.finished) Deactivate();
	
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

	_spriteDeath.Reset();
	_spriteMove.Reset();

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

	switch (_state)
	{
	case EnemyState::Moving:
		_spriteMove.Draw(_transform);

		break;

	case EnemyState::Dying:
		_spriteDeath.Draw(_transform);
	
	default:
		break;
	}

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