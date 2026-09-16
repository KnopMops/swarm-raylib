#include "CollisionMap.hpp"
#include "GameConfig.hpp"

#include "ResourceManager.hpp"

void CollisionMap::Init(const std::string& name) 
{
	_img = &RM::get().GetImage(name);

	if (!_img || !_img->data) return;

    // Бинарная debug-маска: пиксель-в-пиксель совпадает с IsWalkable
    Image dbg = GenImageColor(_img->width, _img->height, BLANK);

    for (int y = 0; y < _img->height; ++y)
	{
		for (int x = 0; x < _img->width; ++x)
		{
			Color c = GetImageColor(*_img, x, y);
			bool walk = c.r >= GameConfig::WALKABLE_THRESHOLD;

			if (walk)
				ImageDrawPixel(&dbg, x, y, Color{ 255, 255, 255, 60 });   // белая панель, alpha = 60/255
			else
				ImageDrawPixel(&dbg, x, y, Color{  60, 120, 255, 110 });  // синяя панель, alpha = 110/255
		}
	}

    _debugTex = LoadTextureFromImage(dbg);
    SetTextureFilter(_debugTex, TEXTURE_FILTER_POINT); // без сглаживания!
    UnloadImage(dbg);
}

void CollisionMap::Unload()
{
    if (_debugTex.id != 0)
    {
        UnloadTexture(_debugTex);
        _debugTex = {};
    }
}

bool CollisionMap::IsWalkable(float x, float y) const 
{
	if (!_img || !_img->data) return false;
    return IsWalkablePixel((int)x, (int)y);
}

bool CollisionMap::IsWalkablePixel(int px, int py) const
{
    if (px < 0 || px >= _img->width || py < 0 || py >= _img->height)
        return false;

    Color pixel = GetImageColor(*_img, px, py);
    return pixel.r >= GameConfig::WALKABLE_THRESHOLD;
}

void CollisionMap::DrawDebug() const
{
    if (_debugTex.id == 0) return;

    BeginBlendMode(BLEND_ALPHA);
    DrawTexture(_debugTex, 0, 0, WHITE);
    EndBlendMode();
}