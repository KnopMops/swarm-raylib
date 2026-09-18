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

	_minimap.Init(_player, _enemies);
	_debugOverlay.Init(_player, _bullets, _enemies, _camera);

	_collisionMap.Init(RK::GAME_BG_COLLISION);

	_player.SetPosition(GameConfig::MapCenter());
	_player.SetCollisionMap(&_collisionMap);

	_camera.zoom = 1.0f;
	_camera.offset = { GameConfig::HALF_BASE_W, GameConfig::HALF_BASE_H };
	_camera.target = GameConfig::MapCenter();

	_enemies.Init(&_player);
	startWave(_wave);

	_lifeTex = &RM::get().GetTexture(RK::PLAYER);
	_lifeSrc = { 0, 0, (float)_lifeTex->width, (float)_lifeTex->height };

	_lastHealth      = _player.GetHealth();
	_blinkPrevHealth = _lastHealth;
	_blinkTimer      = 0.0f;
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

	if (_gameState == GameState::Playing
		&& _wavePaused
		&& _enemies.IsBatchComplete()
		&& IsKeyPressed(KEY_L))
	{
		startWave(_wave + 1);
	}

	return false;
}

void Game::restart()
{
	_player.Reset();
	_player.SetPosition(GameConfig::MapCenter());
	_bullets.DeactivateAll();
	_enemies.DeactivateAll();
	_healthPotions.DeactivateAll();

	_gameState = GameState::Playing;
	startWave(1);

	_lastHealth      = _player.GetHealth();
	_blinkPrevHealth = _lastHealth;
	_blinkTimer      = 0.0f;
}

void Game::startWave(int n)
{
	_wave = n;
	_waveTime = GameConfig::WAVE_TIME_LIMIT;
	_wavePaused = false;
	_waveStarting = true;
	_healthPotions.DeactivateAll();
	_pauseTimer = 0.0f;
}

void Game::spawnWaveEnemies()
{
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

				const bool justDied = enemy->Kill();

				if (justDied
					&& GetRandomValue(0, 100) <= GameConfig::HEALTH_DROP_CHANCE)
				{
					_healthPotions.Spawn(enemy->GetPosition());
				}

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

	for (auto& healthPotion : _healthPotions.GetPool())
	{
		if (!healthPotion->IsAlive()) continue;

		if (_player.GetCollider().IsCollidingWith(healthPotion->GetCollider()))
		{
			_player.Heal(1);
			healthPotion->Deactivate();
		}
	}
}

void Game::updateWaves(float dt)
{
	if (_wavePaused)
	{
		_pauseTimer += dt;
		if (_pauseTimer >= GameConfig::WAVE_PAUSE)
			startWave(_wave + 1);
		return;
	}

	if (_waveStarting)
	{
		_pauseTimer += dt;
		if (_pauseTimer >= GameConfig::WAVE_PAUSE)
		{
			_waveStarting = false;
			_pauseTimer = 0.0f;
			spawnWaveEnemies();
		}
		return;
	}

	if (_enemies.IsBatchComplete())
	{
		_wavePaused = true;
		_pauseTimer = 0.0f;
		return;
	}

	_waveTime -= dt;
	if (_waveTime <= 0.0f)
	{
		_waveTime = 0.0f;
		_gameState = GameState::GameOver;
	}
}

void Game::updateShooting()
{
	if (GI::get().State().shoot)
	{
		_bullets.Spawn(_player.GetFiringPosition(), GI::get().State().aimAngle);
	}
}

void Game::updateHealthBlink(float dt)
{
	const int current = _player.GetHealth();

	if (current < _lastHealth)
	{
		// игрок получил урон — запускаем мигание «потерянных» иконок
		_blinkPrevHealth = _lastHealth;
		_blinkTimer      = HEALTH_BLINK_DURATION;
		_lastHealth      = current;
	}
	else if (current > _lastHealth)
	{
		// подлечился — просто мгновенно обновляем, без мигания
		_lastHealth      = current;
		_blinkPrevHealth = current;
		_blinkTimer      = 0.0f;
	}

	if (_blinkTimer > 0.0f)
	{
		_blinkTimer -= dt;
		if (_blinkTimer < 0.0f) _blinkTimer = 0.0f;
	}
}

void Game::updateEntities(float dt)
{
	GI::get().Update();

	_player.Update(dt);
	_bullets.Update(dt);
	_enemies.Update(dt);
	_healthPotions.Update(dt);
}

void Game::updateCamera()
{
	_camera.target = _player.GetPosition();
	_camera.target.x = std::clamp(_camera.target.x, GameConfig::HALF_BASE_W, GameConfig::MAP_W - GameConfig::HALF_BASE_W);
	_camera.target.y = std::clamp(_camera.target.y, GameConfig::HALF_BASE_H, GameConfig::MAP_H - GameConfig::HALF_BASE_H);
}

void Game::Update(float dt)
{
	updateHealthBlink(dt);

	if (_gameState != GameState::Playing) return;

	const bool frozen = _wavePaused || _waveStarting;

	if (!frozen)
	{
		updateEntities(dt);
		updateShooting();
		updateCollisions();
	}

	updateWaves(dt);
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

void Game::drawHud(const Font& font)
{
	const bool playing = (_gameState == GameState::Playing);

	if (playing && _wavePaused)
	{
		DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.7f));

		const char* title = "ВЫ УБИЛИ ВСЕХ ВРАГОВ!";
		Vector2 titleSize = MeasureTextEx(font, title, 60.0f, 0.0f);
		DrawTextEx(font, title,
		{ (GameConfig::BASE_W - titleSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f - 80.0f },
		60.0f, 0.0f, GREEN);

		float remain = GameConfig::WAVE_PAUSE - _pauseTimer;
		if (remain < 0.0f) remain = 0.0f;

		const char* next = TextFormat("Следующая волна начнётся через %.1f с", remain);
		Vector2 nextSize = MeasureTextEx(font, next, 32.0f, 0.0f);
		DrawTextEx(font, next,
		{ (GameConfig::BASE_W - nextSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f + 12.0f },
		32.0f, 0.0f, WHITE);

		const char* hint = "Нажмите L чтобы начать сейчас";
		Vector2 hintSize = MeasureTextEx(font, hint, 24.0f, 0.0f);
		DrawTextEx(font, hint,
		{ (GameConfig::BASE_W - hintSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f + 56.0f },
		24.0f, 0.0f, GRAY);
	}

	if (playing && _waveStarting)
	{
		DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.5f));

		float remain = GameConfig::WAVE_PAUSE - _pauseTimer;
		if (remain < 0.0f) remain = 0.0f;

		const char* title = TextFormat("ВОЛНА %d", _wave);
		Vector2 titleSize = MeasureTextEx(font, title, 80.0f, 0.0f);
		DrawTextEx(font, title,
		{ (GameConfig::BASE_W - titleSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f - 80.0f },
		80.0f, 0.0f, YELLOW);

		const char* countdown = TextFormat("%.1f", remain);
		Vector2 cdSize = MeasureTextEx(font, countdown, 72.0f, 0.0f);
		DrawTextEx(font, countdown,
		{ (GameConfig::BASE_W - cdSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f },
		72.0f, 0.0f, WHITE);

		const char* hint = "Приготовьтесь!";
		Vector2 hintSize = MeasureTextEx(font, hint, 28.0f, 0.0f);
		DrawTextEx(font, hint,
		{ (GameConfig::BASE_W - hintSize.x) * 0.5f,
		  GameConfig::BASE_H * 0.5f + 80.0f },
		28.0f, 0.0f, LIGHTGRAY);
	}

	if (playing && !_wavePaused && !_waveStarting)
	{
		int totalSeconds = (int)_waveTime;
		if (totalSeconds < 0) totalSeconds = 0;
		int minutes = totalSeconds / 60;
		int seconds = totalSeconds % 60;

		const char* timeText = TextFormat("Волна %d: %d:%02d", _wave, minutes, seconds);

		const float fontSize = 28.0f;
		Vector2 timeSize = MeasureTextEx(font, timeText, fontSize, 0.0f);

		// отступы панели
		const float padX = 16.0f;
		const float padY = 6.0f;
		const float topY = 8.0f;
		const float panelH = timeSize.y + padY * 2.0f;

		// панель с текстом волны/времени — по центру сверху
		float panelW = timeSize.x + padX * 2.0f;
		float panelX = (GameConfig::BASE_W - panelW) * 0.5f;

		DrawRectangleRounded(
			{ panelX, topY, panelW, panelH },
			0.3f, 8, ColorAlpha(BLUE, 0.55f)
		);

		Color timeColor = (_waveTime <= 10.0f) ? RED : WHITE;
		DrawTextEx(font, timeText,
			{ panelX + padX, topY + padY },
			fontSize, 0.0f, timeColor);

		// ------------------------------------------------------------
		// Панель справа: иконки жизней игрока (с миганием при потере)
		// ------------------------------------------------------------
		const float rightPanelW = 200.0f;
		const float rightPanelX = GameConfig::BASE_W - rightPanelW - 12.0f;

		DrawRectangleRounded(
			{ rightPanelX, topY, rightPanelW, panelH },
			0.3f, 8, ColorAlpha(BLUE, 0.55f)
		);

		if (_lifeTex != nullptr && _lifeSrc.width > 0.0f && _lifeSrc.height > 0.0f)
		{
			const int currentHealth = _player.GetHealth();
			const bool blinking     = (_blinkTimer > 0.0f);

			// Пока идёт анимация — раскладываем иконки по СТАРОМУ количеству,
			// чтобы живые иконки не «прыгали» по панели.
			const int layoutCount = blinking ? _blinkPrevHealth : currentHealth;

			if (layoutCount > 0)
			{
				// внутренние отступы панели под иконки
				const float lifePadX = 10.0f;
				const float lifePadY = 4.0f;
				const float lifeGap  = 4.0f;

				const float availW = rightPanelW - lifePadX * 2.0f;
				const float availH = panelH      - lifePadY * 2.0f;

				float slotW = (availW - lifeGap * (layoutCount - 1)) / (float)layoutCount;
				const float slotH = availH;

				if (slotW <= 0.0f)
					slotW = availW / (float)layoutCount;

				const float scaleW = slotW        / _lifeSrc.width;
				const float scaleH = slotH        / _lifeSrc.height;
				const float scale  = (scaleW < scaleH) ? scaleW : scaleH;

				const float iconW = _lifeSrc.width  * scale;
				const float iconH = _lifeSrc.height * scale;

				const float totalW = iconW * layoutCount + lifeGap * (layoutCount - 1);
				const float startX = rightPanelX + (rightPanelW - totalW) * 0.5f;
				const float iconY  = topY + (panelH - iconH) * 0.5f;

				// прогресс анимации: 1 -> 0
				const float blinkT = blinking
					? (_blinkTimer / HEALTH_BLINK_DURATION)
					: 0.0f;

				_lifeDest.clear();
				_lifeDest.reserve(layoutCount);

				for (int i = 0; i < layoutCount; ++i)
				{
					Vector2 pos    = { startX + i * (iconW + lifeGap), iconY };
					Rectangle dest = { pos.x, pos.y, iconW, iconH };

					Color tint = WHITE;

					// иконки с индексом >= currentHealth — это те, что «уходят»
					if (i >= currentHealth)
					{
						// быстрое мигание + плавное затухание
						const float phase = sinf((1.0f - blinkT) * 30.0f);
						const float osc   = (phase > 0.0f) ? 1.0f : 0.2f;
						float alpha       = blinkT * osc;

						if (alpha < 0.0f) alpha = 0.0f;
						if (alpha > 1.0f) alpha = 1.0f;

						tint.a = (unsigned char)(255.0f * alpha);
					}

					_lifeDest.push_back(dest);
					DrawTexturePro(*_lifeTex, _lifeSrc, dest,
						{ 0.0f, 0.0f }, 0.0f, tint);
				}
			}
		}
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
	_healthPotions.Draw();

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
	drawHud(font);

	_minimap.Draw();
	_debugOverlay.Draw(font);

	if (_gameState == GameState::GameOver) drawGameOverOverlay(font);

	EndTextureMode();
}