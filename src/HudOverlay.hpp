#pragma once

#include "raylib.h"

#include <vector>


// Снимок состояния, которое нужно для отрисовки HUD.
// Game заполняет его каждый кадр — HudOverlay о самой игре ничего не знает.
struct HudState
{
	// Волна/время
	int   wave      = 1;
	float waveTime  = 0.0f;

	// Переходы между волнами
	bool  wavePaused    = false;
	bool  waveStarting  = false;
	float pauseTimer    = 0.0f;

	// Разозление сущности
	bool  enraged         = false;
	bool  showEnrageIntro = false;

	// Условия завершения волны
	bool  enemiesBatchComplete = false;
	int   aliveBooks           = 0;

	// Здоровье и мигание иконок
	int   currentHealth       = 0;
	int   blinkPrevHealth     = 0;
	float blinkTimer          = 0.0f;
};

class HudOverlay
{
public:
	static constexpr float HEALTH_BLINK_DURATION = 0.45f;

	void Init(const Texture2D* lifeTex, Rectangle lifeSrc);

	void Draw(const Font& font, const HudState& state);

	void DrawGameOverOverlay(const Font& font) const;
	void DrawBloodEffect(float intensity) const;

private:
	void drawWavePausedScreen(const Font& font, const HudState& s) const;
	void drawWaveStartingScreen(const Font& font, const HudState& s) const;
	void drawMainHud(const Font& font, const HudState& s);
	void drawLifeIcons(const HudState& s,
	                   float rightPanelX, float rightPanelW,
	                   float topY, float panelH);

	const Texture2D* _lifeTex = nullptr;
	Rectangle        _lifeSrc = {};

	std::vector<Rectangle> _lifeDest;
};