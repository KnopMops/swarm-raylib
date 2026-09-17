#pragma once

#include "raylib.h"

class Player;
class EnemyManager;

class Minimap
{
public:
	void Init(const Player& player, const EnemyManager& enemies);
	void Draw() const;

private:
	void drawDot(Vector2 worldPos, Color color) const;

	const Player* _player = nullptr;
	const EnemyManager* _enemies = nullptr;

	float _x = 0.0f;
	float _y = 0.0f;

	float _scaleX = 0.0f;
	float _scaleY = 0.0f;
};