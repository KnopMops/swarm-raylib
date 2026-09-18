#include "Book.hpp"

#include "GameConfig.hpp"

Book::Book()
{
	_transform.scale = 0.16f;

	_collider.Init(28.0f, _transform);
}

void Book::Activate(Vector2 pos, const char* textureName)
{
	_sprite.Init(textureName);
	_alive = true;
	_transform.position = pos;
}

void Book::Deactivate()
{
	_alive = false;
	_transform.position = GameConfig::OFFSCREEN_POSITION;
}

void Book::Draw()
{
	if (!_alive) return;
	_sprite.Draw(_transform);
	_collider.DrawDebug();
}