#include "HealthPotion.hpp"

#include "ResourceKeys.hpp"
#include "GameConfig.hpp"

#include "raylib.h"
#include "raymath.h"
#include "cmath"

#include "SwarmUtils.hpp"

HealthPotion::HealthPotion()
{
    _sprite.Init(RK::HEALTH_POTION);
    _transform.scale = 0.18f;

    _collider.Init(22.0f, _transform);
}

void HealthPotion::Activate(Vector2 pos)
{
    _alive = true;
    _transform.position = pos;
}

void HealthPotion::Deactivate()
{
    _alive = false;
    _transform.position = GameConfig::OFFSCREEN_POSITION;
}

void HealthPotion::Update(float dt)
{
    
}

void HealthPotion::Draw()
{
    if (!_alive) return;

    _sprite.Draw(_transform);
    _collider.DrawDebug();
}