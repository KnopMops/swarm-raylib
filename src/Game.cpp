#include "algorithm"

#include "Game.hpp"
#include "GameInput.hpp"
#include "GameConfig.hpp"

#include "ResourceManager.hpp"
#include "ResourceKeys.hpp"


Game::Game() : _player(RK::PLAYER)
{
	const Texture2D& background = RM::get().GetTexture(RK::GAME_BG);

	GameConfig::MAP_H = (float)background.height;
	GameConfig::MAP_W = (float)background.width;

	_collisionMap.Init(RK::GAME_BG_COLLISION);

	_player.SetPosition(GameConfig::MapCenter());
	_player.SetCollisionMap(&_collisionMap);

	_camera.zoom = 1.0f;
	_camera.offset = { GameConfig::HALF_BASE_W, GameConfig::HALF_BASE_H };
	_camera.target = GameConfig::MapCenter();

	_enemies.Init(&_player);
	startWave(_wave);
};

Game::~Game() {};

bool Game::HandleInput()
{
	if (IsKeyPressed(KEY_Q)) return true;

	if (IsKeyPressed(KEY_ESCAPE))
	{
		if (IsCursorHidden()) EnableCursor();
		else DisableCursor();
	}

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		if (!IsCursorHidden()) DisableCursor();
	}

	if (IsKeyPressed(KEY_F1)) 
		GameConfig::SHOW_DEBUG = !GameConfig::SHOW_DEBUG;

	if (_gameState == GameState::GameOver && IsKeyPressed(KEY_R))
		restart();

	if (_gameState == GameState::Playing && _enemies.IsBatchComplete() && IsKeyPressed(KEY_L)) 
		{
			_wave++;
			_enemies.SpawnBatch(GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * _wave);
		}

	return false;
}

void Game::restart()
{
	_player.Reset();
	_player.SetPosition(GameConfig::MapCenter());
	_bullets.DeactivateAll();
	_enemies.DeactivateAll();
	_gameState = GameState::Playing;
	_wave = 1;
	startWave(1);
}

void Game::startWave(int n)
{
	_wave = n;
	_enemies.SpawnBatch(GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * _wave);
}

void Game::updateCollisions()
{
	for (auto& bullet : _bullets.GetPool())
	{
		if (!bullet->IsAlive()) continue;

		for (auto& enemy : _enemies.GetPool())
		{
			if (!enemy->IsAlive() || !enemy->CanBeHit()) continue;

			if (bullet->GetCollider().IsCollidingWith(enemy->GetCollider()))
			{
				bullet->Deactivate();
				enemy->Kill();
				break;
			}
		}
	}

	for (auto& enemy : _enemies.GetPool())
	{
		if (!enemy->IsAlive() || !enemy->CanBeHit()) continue;

		if (_player.GetCollider().IsCollidingWith(enemy->GetCollider()))
		{
			_player.Hit();
		}
	}
}

void Game::updateWaves()
{
	//
}

void Game::updateShooting()
{
	if (GI::get().State().shoot)
	{
		_bullets.Spawn(_player.GetFiringPosition(), GI::get().State().aimAngle);
	}
}

void Game::updateEntities(float dt)
{
	GI::get().Update();

	_player.Update(dt);
	_bullets.Update(dt);
	_enemies.Update(dt);
}

void Game::updateCamera()
{
	_camera.target = _player.GetPosition();
	_camera.target.x = std::clamp(_camera.target.x, GameConfig::HALF_BASE_W, GameConfig::MAP_W - GameConfig::HALF_BASE_W);
	_camera.target.y = std::clamp(_camera.target.y, GameConfig::HALF_BASE_H, GameConfig::MAP_H - GameConfig::HALF_BASE_H);
}

void Game::Update(float dt)
{
	if (_gameState != GameState::Playing) return;

	updateEntities(dt);
	updateShooting();
	updateCollisions();
	updateWaves();
	updateCamera();

	if (_player.IsDead()) _gameState = GameState::GameOver;
}

void Game::drawGameOverOverlay(const Font& font)
{
			DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.7f));
		
			const char* title = "ИГРА ОКОНЧЕНА";

			Vector2 titleSize = MeasureTextEx(font, title, 60.0f, 0.0f);

			DrawTextEx(font, title,
			{ (GameConfig::BASE_W - titleSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f - 60.0f },
			60.0f, 0.0f, RED);

			const char* prompt = "Нажмите R чтобы возродится или M чтобы выйти в главное меню.";
			Vector2 promptSize = MeasureTextEx(font, prompt, 32.0f, 0.0f);
			DrawTextEx(font, prompt,
			{ (GameConfig::BASE_W - promptSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f + 12.0f },
			32.0f, 0.0f, WHITE);
}

void Game::drawDebug(const Font& font)
{
	if (GameConfig::SHOW_DEBUG)
		{
			DrawRectangle(0, GameConfig::BASE_H - 32, GameConfig::BASE_W, 32,
						ColorAlpha(DARKBLUE, 0.6f));

			DrawTextEx(font,
				TextFormat("XY камеры: %.0f, %.0f", _camera.target.x, _camera.target.y),
				{ 12, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			DrawTextEx(font,
				TextFormat("Игрок: XY: %.0f, %.0f, Скорость: %.0f",
						_player.GetPosition().x, _player.GetPosition().y,
						_player.GetPlayerSpeed()),
				{ 300, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			DrawTextEx(font,
				TextFormat("Волна: %d, Здоровье: %d/%d, Патроны: %d/%d, Враги: %d/%d", _wave,
						_player.GetHealth(), _player.GetMaxHealth(),
						_bullets.CountAlive(), _bullets.GetPoolTotal(),
						_enemies.CountAlive(), _enemies.GetPoolTotal()),
				{ 600, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			const char* fpsText = TextFormat("FPS: %d", GetFPS());
			Vector2 fpsSize = MeasureTextEx(font, fpsText, 20, 0.0f);
			DrawTextEx(font, fpsText,
				{ GameConfig::BASE_W - fpsSize.x - 12, GameConfig::BASE_H - 24 },
				20, 0.0f, LIME);

		}

	if (_gameState == GameState::Playing && _enemies.IsBatchComplete())
		{
			DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.7f));

			const char* title = "ВЫ УБИЛИ ВСЕХ ВРАГОВ!";

			Vector2 titleSize = MeasureTextEx(font, title, 60.0f, 0.0f);

			DrawTextEx(font, title,
			{ (GameConfig::BASE_W - titleSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f - 60.0f },
			60.0f, 0.0f, GREEN);

			const char* prompt = "Чтобы начать новую волну нажмите L";
			Vector2 promptSize = MeasureTextEx(font, prompt, 32.0f, 0.0f);
			DrawTextEx(font, prompt,
			{ (GameConfig::BASE_W - promptSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f + 12.0f },
			32.0f, 0.0f, WHITE);
		}
}

void Game::drawWorld()
{
	BeginMode2D(_camera);

	DrawTexture(RM::get().GetTexture(RK::GAME_BG), 0, 0, WHITE);
	DrawTexture(RM::get().GetTexture(RK::GAME_FG), 0, 0, WHITE);

	_player.Draw();
	_bullets.Draw();
	_enemies.Draw();

	if (GameConfig::SHOW_DEBUG)
        _collisionMap.DrawDebug();

	EndMode2D();
}

void Game::Draw(RenderTexture2D& canvas)
{
	const Font& font = RM::get().GetFont(RK::FONT_MAIN);

	BeginTextureMode(canvas);
	ClearBackground(BLACK);

	drawWorld();
	drawDebug(font);

	if (_gameState == GameState::GameOver) drawGameOverOverlay(font);

	EndTextureMode();
}