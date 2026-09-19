#pragma once

#include "raylib.h"

#include "Player.hpp"
#include "CollisionMap.hpp"

#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "HealthPotionManager.hpp"
#include "BookManager.hpp"

#include "Minimap.hpp"
#include "DebugOverlay.hpp"
#include "HudOverlay.hpp"


enum class GameState { Playing, GameOver };

class Game
{
public:
	Game();
	~Game();

	bool HandleInput();

	void Update(float dt);
	void Draw(RenderTexture2D& canvas);

private:
	void drawWorld();

	void updateEntities(float dt);
	void updateCamera();
	void updateShooting();
	void updateHealthBlink(float dt);

	void startWave(int n);
	void spawnWaveEnemies();
	void updateWaves(float dt);

	void restart();

	void updateCollisions();

	Player _player;
	CollisionMap _collisionMap;
	Camera2D _camera = {};

	BulletManager _bullets;
	EnemyManager _enemies;
	HealthPotionManager _healthPotions;
	BookManager _books;

	Minimap _minimap;
	DebugOverlay _debugOverlay;
	HudOverlay _hud;

	int _wave = 1;
	float _waveTime = 0.0f;
	bool _wavePaused = false;
	bool _waveStarting = false;
	float _pauseTimer = 0.0f;

	// Разозление сущности
	bool  _enraged         = false;
	float _enrageOffset    = 0.0f;
	bool  _showEnrageIntro = false;

	// Мигание иконок здоровья
	int   _lastHealth      = 0;
	int   _blinkPrevHealth = 0;
	float _blinkTimer      = 0.0f;

	GameState _gameState = GameState::Playing;
};