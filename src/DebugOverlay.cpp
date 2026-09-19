#include "DebugOverlay.hpp"

#include "Player.hpp"
#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "GameConfig.hpp"

#include "GameInput.hpp"

#include "raylib.h"


void DebugOverlay::Init(const Player& player, const BulletManager& bulletManager, const EnemyManager& enemyManager, const Camera2D& camera)
{
	_player = &player;
	_bulletManager = &bulletManager;
	_enemyManager = &enemyManager;
	_camera = &camera;
}

void DebugOverlay::Draw(const Font& font) const
{
	if (!GameConfig::SHOW_DEBUG) return;

	int boxY = GameConfig::BASE_H - 155;   // панель стала выше — сдвинули вверх
	int lineGap = 22;

	int fontSize = 20;
	int textX = 10;

	const MovementState& input = GI::get().State();

	// Панель выше на одну строку (было 115, стало 137)
	DrawRectangle(0, boxY - 5, 400, 137, ColorAlpha(BLACK, 0.7f));

	DrawTextEx(font,
			TextFormat("XY камеры: %.0f, %.0f", _camera->target.x, _camera->target.y),
			{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, YELLOW);

	boxY += lineGap;

	DrawTextEx(font,
			TextFormat("Игрок: XY: %.0f, %.0f, Скорость: %.0f",
					_player->GetPosition().x, _player->GetPosition().y,
					_player->GetPlayerSpeed()),
			{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, YELLOW);

	boxY += lineGap;

	DrawTextEx(font,
			TextFormat("Передвижение: %.1f,%.1f", input.moveDir.x, input.moveDir.y),
			{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, YELLOW);

	boxY += lineGap;

	DrawTextEx(font,
			TextFormat("Патроны: %d/%d, Враги: %d/%d",
					_bulletManager->CountAlive(), _bulletManager->GetPoolTotal(),
					_enemyManager->CountAlive(), _enemyManager->GetPoolTotal()),
			{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, YELLOW);

	boxY += lineGap;

	// НОВОЕ: сразу показываем, сколько врагов заказано на текущую волну
	// (это число фиксировано на всю волну и не уменьшается при спавне/убийстве),
	// а также сколько ещё предстоит выпустить из очереди.
	DrawTextEx(font,
			TextFormat("Волна: всего %d, в очереди %d",
					_enemyManager->GetBatchTotal(),
					_enemyManager->GetBatchRemaining()),
			{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, ORANGE);

	boxY += lineGap;

	const char* fpsText = TextFormat("FPS: %d", GetFPS());
	DrawTextEx(font, fpsText,
		{ (float)textX, (float)boxY }, (float)fontSize, 0.0f, YELLOW);
}