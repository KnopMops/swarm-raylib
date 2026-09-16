#include "Player.hpp"
#include "GameConfig.hpp"
#include "GameInput.hpp"

#include "raymath.h"


static constexpr float INVINCIBILITY_SPEED_BOOST = 1.5f;

Player::Player(const std::string& textureName)
{
	_sprite.Init(textureName);
	
	_sprite.pivot = GameConfig::PLAYER_PIVOT;
	_transform.scale = GameConfig::PLAYER_SCALE;
	_movement.speed = GameConfig::PLAYER_SPEED;
	_muzzleOffset = GameConfig::PLAYER_MUZZLE_OFFSET;

	_collider.Init(GameConfig::PLAYER_COLLIDER_RADIUS, _transform);

	_maxHealth = GameConfig::PLAYER_MAX_HEALTH;
	_health = GameConfig::PLAYER_MAX_HEALTH;

	_invTime = GameConfig::PLAYER_INV_TIME;
}

float Player::GetPlayerSpeed() const 
{
    return _movement.speed * (IsInvincible() ? INVINCIBILITY_SPEED_BOOST : 1.0f);	
}

void Player::Update(float delta)
{
	const float speedMultiplier = IsInvincible() ? INVINCIBILITY_SPEED_BOOST : 1.0f;

	_movement.Update(_transform, GI::get().State(), delta, _collisionMap, speedMultiplier);

	if (_invTimer > 0.0f)
		_invTimer -= delta;
}

void Player::SetPosition(Vector2 position)
{
	_transform.position = position;
}

Vector2 Player::GetPosition() const
{
	return _transform.position;
}

void Player::Hit()
{
	if (_invTimer > 0.0f || _health <= 0) return;

	_health--;
	_invTimer = _invTime;
}

Vector2 Player::GetFiringPosition() const
{
	float rad = _transform.rotation * DEG2RAD;
	Vector2 rotated = Vector2Rotate(_muzzleOffset, rad);
	return Vector2Add(_transform.position, rotated);
}

void Player::SetCollisionMap(const CollisionMap* collisionMap)
{
	_collisionMap = collisionMap;
}

void Player::Draw() const
{
	_sprite.Draw(_transform, IsInvincible() ? ColorAlpha(RED, fabsf(sinf(GetTime() * 10.0f))) : WHITE);
	_collider.DrawDebug();
}