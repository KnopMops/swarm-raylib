#pragma once

#include "raylib.h"
#include "string"


class CollisionMap
{
public:
	void Init(const std::string& name);
	void Unload();
	bool IsWalkable(float x, float y) const;
	void DrawDebug() const;

private:
	bool IsWalkablePixel(int px, int py) const;

	const Image* _img = nullptr;
	Texture2D _debugTex = {}; 
};