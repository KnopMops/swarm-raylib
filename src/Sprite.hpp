#pragma once

#include "raylib.h"
#include "Transform2D.hpp"

#include "string"
#include "vector"


struct Sprite 
{
	const Texture2D *texture = nullptr;
	Vector2 pivot = { 0.5f, 0.5f };
	
	float rotationOffset = 0.0f;

	int frameWidth = 0;
	int frameHeight = 0;
	int frameCount = 1;

	int currentFrame = 0;

	float timer = 0.0f;
	float frameDuration = 0.0f;

	std::vector<Rectangle> sourceRects;

	void Init(const std::string& textureName);
	void Init(const std::string& textureName, int fw, int fh, int count, float fps);
	void Draw(const Transform2D& transform) const;
	void Update(float dt);
	void Reset();
};