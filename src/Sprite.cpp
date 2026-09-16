#include "Sprite.hpp"

#include "ResourceManager.hpp"
#include "GameConfig.hpp"

void Sprite::Init(const std::string& textureName) 
{
	texture = &RM::get().GetTexture(textureName);

	frameWidth = texture->width;
	frameHeight = texture->height;

	frameDuration = 0.0f;

	sourceRects.clear();
	sourceRects.push_back({ 0.0f, 0.0f, (float)frameWidth, (float)frameHeight });
}

void Sprite::Init(const std::string& textureName, int fw, int fh, int count, float fps, bool looping) 
{
	texture = &RM::get().GetTexture(textureName);

	frameWidth = fw;
	frameHeight = fh;
	frameCount = count;

	loop = looping;

	frameDuration = (fps > 0.0f) ? 1.0 / fps : 0.0f;

	int cols = texture->width / frameWidth;

	sourceRects.clear();
	sourceRects.reserve(frameCount);

	for (int i = 0; i < frameCount; i++)
	{
		int col = i % cols;
		int row = i / cols;

		Rectangle r = {
			(float)(col * frameWidth), (float)(row * frameHeight),
			(float)frameWidth, (float)frameHeight
		};

		sourceRects.push_back(r);
	}
}

void Sprite::Reset()
{
	timer = 0.0f;
	currentFrame = 0;
	finished = false;
}

void Sprite::Update(float dt)
{
	if (frameDuration <= 0.0f || frameCount <= 1 || finished) return;

	timer += dt;

	if (timer > frameDuration)
	{
		timer = 0.0f;
		currentFrame++;

		if (currentFrame >= frameCount)
		{
			if (loop) currentFrame = 0;
			else 
			{
				finished = true;
				currentFrame = frameCount - 1;
			}
		}
	}
}

void Sprite::Draw(const Transform2D& transform, Color tint) const 
{

	if (!texture) return;

	float w = frameWidth * transform.scale;
	float h = frameHeight * transform.scale;

	Rectangle source = sourceRects[currentFrame];

	Rectangle dest = {
		transform.position.x, transform.position.y, w, h
	};

	Vector2 origin = { w * pivot.x, h * pivot.y };

	DrawTexturePro(*texture, source, dest, origin, transform.rotation + rotationOffset, tint);

	if (GameConfig::SHOW_DEBUG)
		DrawCircle((int)transform.position.x, (int)transform.position.y, 2.0f, RED);

}