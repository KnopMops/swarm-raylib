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


int main() {

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(GameConfig::BASE_W, GameConfig::BASE_H, "Swarm");
	SetTargetFPS(60);

	DisableCursor();

	RM::get().Load();

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

	EnemyManager enemies;
	enemies.Init(&player);
	enemies.Spawn({ GameConfig::MAP_W * 0.5f + 200.0f, GameConfig::MAP_H * 0.5f });

	while (!WindowShouldClose()) 
	{

		if (IsKeyPressed(KEY_F1)) 
			GameConfig::SHOW_DEBUG = !GameConfig::SHOW_DEBUG;

		if (IsKeyDown(KEY_L)) 
		{
			enemies.Spawn({
				RandomFloat(0.0f, GameConfig::MAP_W),
				RandomFloat(0.0f, GameConfig::MAP_H)
			});
		}

		if (IsKeyPressed(KEY_F2)) 
			enemies.DeactivateAll();

		GI::get().Update();

		float dt = GetFrameTime();

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
				if (!enemy->IsAlive()) continue;

				if (bullet->GetCollider().IsCollidingWith(enemy->GetCollider()))
				{

					bullet->Deactivate();
					enemy->Deactivate();
					break;
				}
			}
		}

		camera.target = player.GetPosition();

		camera.target.x = std::clamp(camera.target.x, halfW, GameConfig::MAP_W - halfW);
		camera.target.y = std::clamp(camera.target.y, halfH, GameConfig::MAP_H - halfH);

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
			DrawRectangle(0, GameConfig::BASE_H - 32, GameConfig::BASE_W, 32, ColorAlpha(DARKBLUE, 0.6f));

			DrawText(TextFormat("CameraXY: %.0f, %.0f", camera.target.x, camera.target.y), 256, GameConfig::BASE_H - 24, 20, LIME);
			DrawText(TextFormat("PlayerXY: %.0f, %.0f", player.GetPosition().x, player.GetPosition().y), 12, GameConfig::BASE_H - 24, 20, LIME);
			DrawText(TextFormat("Rotation: %.1f", GI::get().State().aimAngle), 512, GameConfig::BASE_H - 24, 20, LIME);
			DrawText(TextFormat("Bullets: %d/%d, Enemies: %d/%d", bullets.CountAlive(), bullets.GetPoolTotal(), enemies.CountAlive(), enemies.GetPoolTotal()), 700, GameConfig::BASE_H - 24, 20, LIME);
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