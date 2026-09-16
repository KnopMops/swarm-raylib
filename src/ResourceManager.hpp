#pragma once

#include "unordered_map"
#include "string"

#include "raylib.h"


class ResourceManager
{
public:

	static ResourceManager& instance() 
	{
		static ResourceManager inst;
		return inst;
	}

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator = (const ResourceManager&) = delete;

	void Load();
	void Unload();

	const Texture2D& GetTexture(const std::string& name) const;
	const Image& GetImage(const std::string& name) const;
	const Font& GetFont   (const std::string& key) const;

private:
	ResourceManager() = default;

	void loadTexture(const std::string& name, const std::string& path);
	void loadImage(const std::string& name, const std::string& path);

	std::unordered_map<std::string, Texture2D> _textures;
	std::unordered_map<std::string, Image> _images;
	std::unordered_map<std::string, Font> m_fonts;
};

namespace RM
{
	inline ResourceManager& get() { return ResourceManager::instance(); }
}