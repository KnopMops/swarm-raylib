#include "Enemy.hpp"
#include "ResourceKeys.hpp"
#include "Player.hpp"
#include "GameConfig.hpp"

void Enemy::Init(const EnemyDef& def)
{
	_spriteMove.Init(
		def.move.textureKey, def.move.frameWidth, def.move.frameHeight,
		def.move.frameCount, def.move.framesPerSecond, def.move.loop
	);
	_spriteMove.rotationOffset = def.move.rotationOffset;

	_spriteDeath.Init(
		def.death.textureKey, def.death.frameWidth, def.death.frameHeight,
		def.death.frameCount, def.death.framesPerSecond, def.death.loop
	);
	_spriteDeath.rotationOffset = def.death.rotationOffset;

	_transform.scale = def.scale;
	_transform.rotation = 0.0f;

	_speed = def.speed;
	_collider.Init(def.colliderRadius, _transform);

	_retargetMin = def.retargetMin;
	_retargetMax = def.retargetMax;
}

bool Enemy::Kill()
{
	if (_state == EnemyState::Dying) return false;

	health -= 1;

	if (health <= 0)
	{
		_state = EnemyState::Dying;
		_spriteDeath.Reset();
		return true;
	}

	return false;
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
		break;

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
	_alive = false;
	_transform.position = GameConfig::OFFSCREEN_POSITION;
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
		break;

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