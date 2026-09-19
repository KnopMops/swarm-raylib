#include "HudOverlay.hpp"

#include "GameConfig.hpp"

#include "cmath"


void HudOverlay::Init(const Texture2D* lifeTex, Rectangle lifeSrc)
{
	_lifeTex = lifeTex;
	_lifeSrc = lifeSrc;
}

void HudOverlay::Draw(const Font& font, const HudState& s)
{
	if (s.wavePaused)   drawWavePausedScreen(font, s);
	if (s.waveStarting) drawWaveStartingScreen(font, s);

	if (!s.wavePaused && !s.waveStarting)
		drawMainHud(font, s);
}

// ------------------------------------------------------------
// Экран между волнами (успех / разозление)
// ------------------------------------------------------------
void HudOverlay::drawWavePausedScreen(const Font& font, const HudState& s) const
{
	if (s.showEnrageIntro)
	{
		// Чёрный фон + кровь + красный пульсирующий текст
		DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, BLACK);
		DrawBloodEffect(1.0f);

		const float t      = (s.pauseTimer / GameConfig::WAVE_PAUSE);
		const float fadeIn = (t * 4.0f < 1.0f) ? (t * 4.0f) : 1.0f;
		const float pulse  = 0.75f + 0.25f * sinf(s.pauseTimer * 10.0f);
		const float a      = fadeIn * pulse;

		const char* title = "НЕ НУЖНО БЫЛО ЗЛИТЬ СУЩНОСТЬ";
		const float titleFont = 56.0f;
		Vector2 titleSize = MeasureTextEx(font, title, titleFont, 0.0f);
		Vector2 titlePos  = {
			(GameConfig::BASE_W - titleSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f - 60.0f
		};

		Color titleColor = {
			(unsigned char)220,
			(unsigned char)30,
			(unsigned char)30,
			(unsigned char)(255.0f * a)
		};
		DrawTextEx(font, title, titlePos, titleFont, 0.0f, titleColor);

		const char* hint = "Следующая волна будет усилена, а книги исчезнут навсегда";
		const float hintFont = 24.0f;
		Vector2 hintSize = MeasureTextEx(font, hint, hintFont, 0.0f);
		Vector2 hintPos  = {
			(GameConfig::BASE_W - hintSize.x) * 0.5f,
			GameConfig::BASE_H * 0.5f + 40.0f
		};

		Color hintColor = {
			(unsigned char)255,
			(unsigned char)80,
			(unsigned char)80,
			(unsigned char)(255.0f * a)
		};
		DrawTextEx(font, hint, hintPos, hintFont, 0.0f, hintColor);
	}
	else
	{
		// Обычный экран успешного завершения волны
		DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.7f));

		const char* title = "ВЫ УБИЛИ ВСЕХ ВРАГОВ!";
		Vector2 titleSize = MeasureTextEx(font, title, 60.0f, 0.0f);
		DrawTextEx(font, title,
			{ (GameConfig::BASE_W - titleSize.x) * 0.5f,
			  GameConfig::BASE_H * 0.5f - 80.0f },
			60.0f, 0.0f, GREEN);

		float remain = GameConfig::WAVE_PAUSE - s.pauseTimer;
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
}

// ------------------------------------------------------------
// Экран старта волны («ВОЛНА N»)
// ------------------------------------------------------------
void HudOverlay::drawWaveStartingScreen(const Font& font, const HudState& s) const
{
	DrawRectangle(0, 0, GameConfig::BASE_W, GameConfig::BASE_H, ColorAlpha(BLACK, 0.5f));

	float remain = GameConfig::WAVE_PAUSE - s.pauseTimer;
	if (remain < 0.0f) remain = 0.0f;

	const char* title = TextFormat("ВОЛНА %d", s.wave);
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

// ------------------------------------------------------------
// Обычный HUD во время волны
// ------------------------------------------------------------
void HudOverlay::drawMainHud(const Font& font, const HudState& s)
{
	int totalSeconds = (int)s.waveTime;
	if (totalSeconds < 0) totalSeconds = 0;
	int minutes = totalSeconds / 60;
	int seconds = totalSeconds % 60;

	// Фаза книг возможна только когда сущность НЕ разозлена
	const bool booksPhase =
		(!s.enraged && s.enemiesBatchComplete && s.aliveBooks > 0);

	// Верхняя центральная панель: волна/время | количество книг
	const char* timeText = booksPhase
		? TextFormat("Собери все книги: %d:%02d   |   Книг: %d",
			minutes, seconds, s.aliveBooks)
		: TextFormat("Волна %d: %d:%02d   |   Книг: %d",
			s.wave, minutes, seconds, s.aliveBooks);

	const float fontSize = 28.0f;
	Vector2 timeSize = MeasureTextEx(font, timeText, fontSize, 0.0f);

	const float padX = 16.0f;
	const float padY = 6.0f;
	const float topY = 8.0f;
	const float panelH = timeSize.y + padY * 2.0f;

	float panelW = timeSize.x + padX * 2.0f;
	float panelX = (GameConfig::BASE_W - panelW) * 0.5f;

	DrawRectangleRounded(
		{ panelX, topY, panelW, panelH },
		0.3f, 8, ColorAlpha(BLUE, 0.55f)
	);

	Color timeColor = (s.waveTime <= 10.0f) ? RED : WHITE;
	DrawTextEx(font, timeText,
		{ panelX + padX, topY + padY },
		fontSize, 0.0f, timeColor);

	if (booksPhase)
	{
		const float hintFont = 18.0f;
		const char* hint = "Иначе сущность разозлится и все последующие волны будут усилены на 2.5";

		Vector2 hintSize = MeasureTextEx(font, hint, hintFont, 0.0f);
		Vector2 hintPos  = {
			(GameConfig::BASE_W - hintSize.x) * 0.5f,
			topY + panelH + 6.0f
		};

		DrawTextEx(font, hint, hintPos, hintFont, 0.0f, ColorAlpha(RED, 0.9f));
	}

	// Правая панель: иконки жизней
	const float rightPanelW = 200.0f;
	const float rightPanelX = GameConfig::BASE_W - rightPanelW - 12.0f;

	DrawRectangleRounded(
		{ rightPanelX, topY, rightPanelW, panelH },
		0.3f, 8, ColorAlpha(BLUE, 0.55f)
	);

	drawLifeIcons(s, rightPanelX, rightPanelW, topY, panelH);
}

// ------------------------------------------------------------
// Иконки жизней с миганием и затуханием
// ------------------------------------------------------------
void HudOverlay::drawLifeIcons(const HudState& s,
                               float rightPanelX, float rightPanelW,
                               float topY, float panelH)
{
	if (_lifeTex == nullptr) return;
	if (_lifeSrc.width <= 0.0f || _lifeSrc.height <= 0.0f) return;

	const int currentHealth = s.currentHealth;
	const bool blinking     = (s.blinkTimer > 0.0f);

	// Пока идёт анимация — раскладываем иконки по СТАРОМУ количеству,
	// чтобы живые иконки не «прыгали» по панели.
	const int layoutCount = blinking ? s.blinkPrevHealth : currentHealth;

	if (layoutCount <= 0) return;

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

	const float blinkT = blinking
		? (s.blinkTimer / HEALTH_BLINK_DURATION)
		: 0.0f;

	_lifeDest.clear();
	_lifeDest.reserve(layoutCount);

	for (int i = 0; i < layoutCount; ++i)
	{
		Vector2 pos    = { startX + i * (iconW + lifeGap), iconY };
		Rectangle dest = { pos.x, pos.y, iconW, iconH };

		Color tint = WHITE;

		if (i >= currentHealth)
		{
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

// ------------------------------------------------------------
// Оверлей «ИГРА ОКОНЧЕНА»
// ------------------------------------------------------------
void HudOverlay::DrawGameOverOverlay(const Font& font) const
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

// ------------------------------------------------------------
// Кровавый эффект по бокам
// ------------------------------------------------------------
void HudOverlay::DrawBloodEffect(float intensity) const
{
	if (intensity <= 0.0f) return;

	const float pulse = 0.80f + 0.20f * sinf(GetTime() * 5.0f);

	const int stripW  = 110;
	const int screenH = (int)GameConfig::BASE_H;
	const int screenW = (int)GameConfig::BASE_W;

	for (int i = 0; i < stripW; ++i)
	{
		const float t = 1.0f - (float)i / (float)stripW;

		const float wobble = 1.0f + 0.15f * sinf(i * 0.35f + GetTime() * 3.0f);

		float local = t * t * pulse * intensity * wobble;
		if (local < 0.0f) local = 0.0f;
		if (local > 1.0f) local = 1.0f;

		const unsigned char a = (unsigned char)(220.0f * local);
		if (a == 0) continue;

		Color c = { (unsigned char)170, (unsigned char)0, (unsigned char)0, a };

		DrawRectangle(i, 0, 1, screenH, c);
		DrawRectangle(screenW - 1 - i, 0, 1, screenH, c);
	}
}