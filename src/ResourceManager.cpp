#include "ResourceManager.hpp"
#include "ResourceKeys.hpp"

#include "stdexcept"


void ResourceManager::Unload()
{
	for (auto& [name, tex] : _textures)
		UnloadTexture(tex);

	for (auto& [name, img] : _images)
		UnloadImage(img);

	for (auto& [key, font] : m_fonts)
        UnloadFont(font);

    m_fonts.clear();
	_textures.clear();
	_images.clear();

	TraceLog(LOG_INFO, "ResourceManager: Unloaded all textures and images");
}


void ResourceManager::Load()
{
	std::string fontPath = std::string(GetApplicationDirectory())
                         + "/../assets/fonts/Roboto-Regular.ttf";

	int codepoints[512] = { 0 };
    int count = 0;

    // ASCII 32..126 (пробел, цифры, латиница, базовая пунктуация)
    for (int i = 32; i <= 126; i++)
        codepoints[count++] = i;

    // Кириллица: U+0400 .. U+04FF (включая Ё, ё, украинские/белорусские буквы)
    for (int i = 0x400; i <= 0x4FF; i++)
        codepoints[count++] = i;

    Font font = LoadFontEx(fontPath.c_str(), 48, codepoints, count);

    if (font.texture.id == 0)
        throw std::runtime_error("Failed to load font: " + fontPath);

    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
    m_fonts[RK::FONT_MAIN] = font;

    TraceLog(LOG_INFO, "ResourceManager: Loaded font with %d codepoints, texture.id=%u",
             count, font.texture.id);

	ChangeDirectory(TextFormat("%s/../assets/images", GetApplicationDirectory()));


	if (!FileExists(fontPath.c_str()))
        throw std::runtime_error("Font not found: " + fontPath);

	loadTexture(RK::PLAYER, "survivor-idle_shotgun_0.png");
	loadTexture(RK::GAME_BG, "Floor.png");
	loadTexture(RK::GAME_FG, "Walls.png");
	loadTexture(RK::BULLET, "bullet.png");

	loadTexture(RK::COCKROACH_MOVE, "cockroach-move.png");
	loadTexture(RK::COCKROACH_DEATH, "cockroach-death.png");

	loadImage(RK::GAME_BG_COLLISION, "gameBgCollision.png");

	TraceLog(LOG_INFO, "ResourceManager: Loaded %d images", (int)_images.size());
	TraceLog(LOG_INFO, "ResourceManager: Loaded %d fonts", (int)m_fonts.size());
}

void ResourceManager::loadTexture(const std::string& name, const std::string& path)
{
	Texture2D tex = LoadTexture(path.c_str());

	if (tex.id == 0) 
		throw std::runtime_error("Failed to load texture: " + path);

	_textures.emplace(name, std::move(tex));
}

void ResourceManager::loadImage(const std::string& name, const std::string& path)
{
	Image img = LoadImage(path.c_str());

	if (img.data == nullptr) 
		throw std::runtime_error("Failed to load image: " + path);

	_images.emplace(name, std::move(img));
}

const Texture2D& ResourceManager::GetTexture(const std::string& name) const
{
	auto it = _textures.find(name);
	
	if (it == _textures.end())
		throw std::runtime_error("Texture not found: '" + name + "'");

	return it->second;
}

const Image& ResourceManager::GetImage(const std::string& name) const
{
	auto it = _images.find(name);
	
	if (it == _images.end())
		throw std::runtime_error("Image not found: '" + name + "'");

	return it->second;
}

const Font& ResourceManager::GetFont(const std::string& key) const
{
    return m_fonts.at(key);
}