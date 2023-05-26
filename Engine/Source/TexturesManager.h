#pragma once

#include "Renderer/Texture2D.h"

#include <unordered_set>
#include <memory>

class TexturesManager
{
public:
	//TexturesManager();
	//virtual ~TexturesManager();

	static void AddTexture(std::shared_ptr<Texture2D> texture);
	[[nodiscard]] static const std::unordered_set<std::shared_ptr<Texture2D>>& GetTextures() { return textures; }

	static void LoadTextures();

private:

	static std::unordered_set<std::shared_ptr<Texture2D>> textures;

};
