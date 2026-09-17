#include "algorithm"
#include "vector"

#include "raylib.h"
#include "raymath.h"

#include "GameConfig.hpp"
#include "ResourceKeys.hpp"
#include "ResourceManager.hpp"

#include "CollisionMap.hpp"
#include "Sprite.hpp"
#include "Movement.hpp"
#include "GameInput.hpp"

#include "BulletManager.hpp"
#include "EnemyManager.hpp"

#include "Player.hpp"


enum class GameState { Playing, GameOver };

int main() {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(GameConfig::BASE_W, GameConfig::BASE_H, "Swarm");
	SetTargetFPS(60);

	DisableCursor();
	SetExitKey(KEY_NULL);

	RM::get().Load();

	const Font& font = RM::get().GetFont(RK::FONT_MAIN);

	Vector2 pos = GetMonitorPosition(1);
	SetWindowPosition(pos.x + 320, pos.y + 180);

	const Texture2D& background = RM::get().GetTexture(RK::GAME_BG);

	GameConfig::MAP_H = (float)background.height;
	GameConfig::MAP_W = (float)background.width;

	CollisionMap collisionMap;
	collisionMap.Init(RK::GAME_BG_COLLISION);

	RenderTexture2D canvas = LoadRenderTexture(GameConfig::BASE_W, GameConfig::BASE_H);
	SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

	float halfW = GameConfig::BASE_W * 0.5f;
	float halfH = GameConfig::BASE_H * 0.5f;

	Player player(RK::PLAYER);
	player.SetPosition({ GameConfig::MAP_W * 0.5f, GameConfig::MAP_H * 0.5f });
	player.SetCollisionMap(&collisionMap);

	Camera2D camera = {};
	camera.zoom = 1.0f;
	camera.target = player.GetPosition();
	camera.offset = { halfW, halfH };

	BulletManager bullets;

	int wave = 1;

	EnemyManager enemies;
	enemies.Init(&player);
	enemies.SpawnBatch(GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * wave);

	GameState gameState = GameState::Playing;

	while (!WindowShouldClose()) 
	{

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

		if (gameState == GameState::Playing && enemies.IsBatchComplete() && IsKeyPressed(KEY_L)) 
		{
			wave++;
			enemies.SpawnBatch(GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * wave);
		}

		if (IsKeyPressed(KEY_F2)) 
			enemies.DeactivateAll();

		if (gameState == GameState::GameOver && IsKeyPressed(KEY_R))
		{
			player.Reset();
			player.SetPosition({ GameConfig::MAP_W * 0.5f, GameConfig::MAP_H * 0.5f });
			bullets.DeactivateAll();
			enemies.DeactivateAll();
			gameState = GameState::Playing;
			wave = 1;
			enemies.SpawnBatch(GameConfig::WAVE_ENEMY_BASE + GameConfig::WAVE_ENEMY_RAMP * wave);
		}

		GI::get().Update();

		float dt = GetFrameTime();

		if (gameState == GameState::Playing)
		{
			if (GI::get().State().shoot)
			{
				bullets.Spawn(player.GetFiringPosition(), GI::get().State().aimAngle);
			}

			player.Update(dt);
			bullets.Update(dt);
			enemies.Update(dt);

			for (auto& bullet : bullets.GetPool())
			{
				if (!bullet->IsAlive()) continue;

				for (auto& enemy : enemies.GetPool())
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

			for (auto& enemy : enemies.GetPool())
			{
				if (!enemy->IsAlive() || !enemy->CanBeHit()) continue;

				if (player.GetCollider().IsCollidingWith(enemy->GetCollider()))
				{
					player.Hit();
				}
			}

			camera.target = player.GetPosition();

			camera.target.x = std::clamp(camera.target.x, halfW, GameConfig::MAP_W - halfW);
			camera.target.y = std::clamp(camera.target.y, halfH, GameConfig::MAP_H - halfH);

			if (player.IsDead()) gameState = GameState::GameOver;
		}

		BeginTextureMode(canvas);
		ClearBackground(BLACK);
		BeginMode2D(camera);
			DrawTexture(background, 0, 0, WHITE);

			if (GameConfig::SHOW_DEBUG)
        		collisionMap.DrawDebug();

			player.Draw();
			bullets.Draw();
			enemies.Draw();

			DrawTexture(RM::get().GetTexture(RK::GAME_FG), 0, 0, WHITE);
		EndMode2D();

		if (GameConfig::SHOW_DEBUG)
		{
			DrawRectangle(0, GameConfig::BASE_H - 32, GameConfig::BASE_W, 32,
						ColorAlpha(DARKBLUE, 0.6f));

			DrawTextEx(font,
				TextFormat("XY камеры: %.0f, %.0f", camera.target.x, camera.target.y),
				{ 12, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			DrawTextEx(font,
				TextFormat("Игрок: XY: %.0f, %.0f, Скорость: %.0f",
						player.GetPosition().x, player.GetPosition().y,
						player.GetPlayerSpeed()),
				{ 300, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			DrawTextEx(font,
				TextFormat("Волна: %d, Здоровье: %d/%d, Патроны: %d/%d, Враги: %d/%d", wave,
						player.GetHealth(), player.GetMaxHealth(),
						bullets.CountAlive(), bullets.GetPoolTotal(),
						enemies.CountAlive(), enemies.GetPoolTotal()),
				{ 600, GameConfig::BASE_H - 24 }, 20, 0.0f, LIME);

			const char* fpsText = TextFormat("FPS: %d", GetFPS());
			Vector2 fpsSize = MeasureTextEx(font, fpsText, 20, 0.0f);
			DrawTextEx(font, fpsText,
				{ GameConfig::BASE_W - fpsSize.x - 12, GameConfig::BASE_H - 24 },
				20, 0.0f, LIME);

		}
			
		if (gameState == GameState::GameOver)
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

		if (gameState == GameState::Playing && enemies.IsBatchComplete())
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
			
		EndTextureMode();

		float scale = std::min(
			(float)GetScreenWidth() / GameConfig::BASE_W, 
			(float)GetScreenHeight() / GameConfig::BASE_H
		);

		float offsetX = (GetScreenWidth() - GameConfig::BASE_W * scale) * 0.5f;
		float offsetY = (GetScreenHeight() - GameConfig::BASE_H * scale) * 0.5f;

		Rectangle src = { 0, 0, (float)GameConfig::BASE_W, -(float)GameConfig::BASE_H };
		Rectangle dest = { offsetX, offsetY, GameConfig::BASE_W * scale, GameConfig::BASE_H * scale };

		BeginDrawing();
			ClearBackground(BLACK);
			DrawTexturePro(canvas.texture, src, dest, { 0, 0 }, 0.0f, WHITE);
		EndDrawing();
	}

	UnloadRenderTexture(canvas);
	collisionMap.Unload();

	RM::get().Unload();

	CloseWindow();

	return 0;

}