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

	_minimap.Init(_player, _enemies, _books, _healthPotions);
	_debugOverlay.Init(_player, _bullets, _enemies, _camera);

	_collisionMap.Init(RK::GAME_BG_COLLISION);

	_player.SetPosition(GameConfig::MapCenter());
	_player.SetCollisionMap(&_collisionMap);

	_camera.zoom = 1.0f;
	_camera.offset = { GameConfig::HALF_BASE_W, GameConfig::HALF_BASE_H };
	_camera.target = GameConfig::MapCenter();

	_enemies.Init(&_player);
	_books.Init(&_player);

	startWave(_wave);

	// Передаём HUD-у иконку жизни один раз при инициализации
	{
		const Texture2D& lifeTex = RM::get().GetTexture(RK::PLAYER);
		Rectangle lifeSrc = { 0, 0, (float)lifeTex.width, (float)lifeTex.height };
		_hud.Init(&lifeTex, lifeSrc);
	}

	_enraged         = false;
	_enrageOffset    = 0.0f;
	_showEnrageIntro = false;

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

	// Пропуск паузы клавишей L.
	// Заблокировано во время чёрно-красной сцены разозления — её надо просмотреть.
	if (_gameState == GameState::Playing
		&& _wavePaused
		&& !_showEnrageIntro
		&& _enemies.IsBatchComplete()
		&& IsKeyPressed(KEY_L)
		&& (_enraged || _books.CountAlive() == 0))
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
	_enemies.CancelBatch();
	_enemies.DeactivateAll();
	_healthPotions.DeactivateAll();
	_books.DeactivateAll();

	_gameState = GameState::Playing;

	_enraged         = false;
	_enrageOffset    = 0.0f;
	_showEnrageIntro = false;

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

	// Пока сущность не разозлена — спавним книги.
	// В разозлённой фазе книг нет: цель — только убить всех врагов.
	if (!_enraged)
	{
		int idx = std::clamp(_wave - 1, 0, (int)GameConfig::WAVE_BOOK_COUNTS.size() - 1);
		_books.SpawnBatch(GameConfig::WAVE_BOOK_COUNTS[idx]);
	}
}

void Game::spawnWaveEnemies()
{
	const int base = GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * _wave;

	int total = base;

	// Постоянная добавка от разозления. Благодаря тому, что _enrageOffset
	// был вычислен при первом разозлении как (natural * 1.5), получаем:
	//   усиленная волна:  base + base*1.5         = base * 2.5
	//   следующая:        (base+RAMP) + base*1.5  = base*2.5 + RAMP
	//   и т.д. — просто прибавляем RAMP каждую волну.
	if (_enraged)
		total = (int)((float)base + _enrageOffset + 0.5f);

	_enemies.SpawnBatch(total);
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

	for (auto& book : _books.GetPool())
	{
		if (!book->IsAlive()) continue;

		if (_player.GetCollider().IsCollidingWith(book->GetCollider()))
		{
			book->Deactivate();
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
		{
			// Чёрно-красную сцену разозления показываем только один раз
			_showEnrageIntro = false;
			startWave(_wave + 1);
		}
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

	// Условие завершения волны:
	// - не разозлены: враги добиты И все книги собраны
	// - разозлены:    только враги добиты (книг нет)
	const bool enemiesDone = _enemies.IsBatchComplete();
	const bool booksDone   = _enraged || (_books.CountAlive() == 0);

	if (enemiesDone && booksDone)
	{
		_wavePaused = true;
		_pauseTimer = 0.0f;
		return;
	}

	_waveTime -= dt;
	if (_waveTime <= 0.0f)
	{
		_waveTime = 0.0f;

		// Провал по времени. Разозление срабатывает только один раз —
		// при первом провале с несобранными книгами.
		if (!_enraged && _books.CountAlive() > 0)
		{
			_enraged         = true;
			_showEnrageIntro = true;

			// Усиленной будет следующая волна (текущая + 1).
			const int boostWave = _wave + 1;
			const int natural   = GameConfig::WAVE_ENEMY_BASE
			                    + GameConfig::WAVE_ENEMY_RAMP * boostWave;

			_enrageOffset = (float)natural * 1.5f;
		}

		// Чистим поле, чтобы следующая волна стартовала без «хвостов».
		_enemies.CancelBatch();
		_enemies.DeactivateAll();
		_books.DeactivateAll();

		_wavePaused = true;
		_pauseTimer = 0.0f;
		return;
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
		_blinkPrevHealth = _lastHealth;
		_blinkTimer      = HudOverlay::HEALTH_BLINK_DURATION;
		_lastHealth      = current;
	}
	else if (current > _lastHealth)
	{
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

void Game::drawWorld()
{
	BeginMode2D(_camera);

	DrawTexture(RM::get().GetTexture(RK::GAME_BG), 0, 0, WHITE);
	DrawTexture(RM::get().GetTexture(RK::GAME_FG), 0, 0, WHITE);

	_player.Draw();
	_bullets.Draw();
	_enemies.Draw();
	_healthPotions.Draw();
	_books.Draw();

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

	// Заполняем снимок состояния и передаём в HUD
	HudState hud;
	hud.wave                 = _wave;
	hud.waveTime             = _waveTime;
	hud.wavePaused           = _wavePaused;
	hud.waveStarting         = _waveStarting;
	hud.pauseTimer           = _pauseTimer;
	hud.enraged              = _enraged;
	hud.showEnrageIntro      = _showEnrageIntro;
	hud.enemiesBatchComplete = _enemies.IsBatchComplete();
	hud.aliveBooks           = _books.CountAlive();
	hud.currentHealth        = _player.GetHealth();
	hud.blinkPrevHealth      = _blinkPrevHealth;
	hud.blinkTimer           = _blinkTimer;

	if (_gameState == GameState::Playing)
		_hud.Draw(font, hud);

	// Кровь по бокам, пока сущность разозлена.
	// Во время игры — лёгкая (0.4), в момент интро — сильная (1.0).
	if (_enraged)
	{
		const float intensity = _showEnrageIntro ? 1.0f : 0.4f;
		_hud.DrawBloodEffect(intensity);
	}

	_minimap.Draw();
	_debugOverlay.Draw(font);

	if (_gameState == GameState::GameOver)
		_hud.DrawGameOverOverlay(font);

	EndTextureMode();
}