#include "Minimap.hpp"
#include "GameConfig.hpp"

#include "EnemyManager.hpp"
#include "Player.hpp"


void Minimap::Init(const Player& player, const EnemyManager& enemies)
{
	_x = GameConfig::BASE_W - GameConfig::MINIMAP_SIZE - GameConfig::MINIMAP_PAD;
	_y = GameConfig::BASE_H - GameConfig::MINIMAP_SIZE - GameConfig::MINIMAP_PAD;

	_scaleX = (float)GameConfig::MINIMAP_SIZE / GameConfig::MAP_W;
	_scaleY = (float)GameConfig::MINIMAP_SIZE / GameConfig::MAP_H;

	_player = &player;
	_enemies = &enemies;
}

void Minimap::drawDot(Vector2 worldPos, Color color) const
{
	int pX = (int)(_x + worldPos.x * _scaleX);
	int pY = (int)(_y + worldPos.y * _scaleY);

	DrawCircle(pX, pY, 2, color);
}

void Minimap::Draw() const
{
	DrawRectangle(_x, _y, GameConfig::MINIMAP_SIZE, GameConfig::MINIMAP_SIZE, ColorAlpha(BLACK, 0.6f));
	
	DrawRectangleLines(_x, _y, GameConfig::MINIMAP_SIZE, GameConfig::MINIMAP_SIZE, GRAY);

	for (const auto& enemy : _enemies->GetPool())
	{
		if (!enemy->IsAlive()) continue;
		drawDot(enemy->GetPosition(), RED);
	}

	drawDot(_player->GetPosition(), DARKBLUE);
}