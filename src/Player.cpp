#include "Player.hpp"
#include "GameConfig.hpp"
#include "GameInput.hpp"

#include "raymath.h"


Player::Player(const std::string& textureName)
{
	_sprite.Init(textureName);
	
	_sprite.pivot = GameConfig::PLAYER_PIVOT;
	_transform.scale = GameConfig::PLAYER_SCALE;
	_movement.speed = GameConfig::PLAYER_SPEED;
	_muzzleOffset = GameConfig::PLAYER_MUZZLE_OFFSET;
}

void Player::Update(float delta)
{
	_movement.Update(_transform, GI::get().State(), delta, _collisionMap);
}

void Player::SetPosition(Vector2 position)
{
	_transform.position = position;
}

Vector2 Player::GetPosition() const
{
	return _transform.position;
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
	_sprite.Draw(_transform);
}