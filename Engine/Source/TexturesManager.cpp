#include "TexturesManager.h"

#include <iostream>
#include <filesystem>

std::unordered_set<std::shared_ptr<Texture2D>> TexturesManager::textures = {};

void TexturesManager::AddTexture(std::shared_ptr<Texture2D> texture)
{
	textures.emplace(texture);
}

void TexturesManager::LoadTextures()
{
	for (std::filesystem::recursive_directory_iterator it("Assets"), end; it != end; ++it)
	{
		if (is_directory(it->path()))
			continue;

		auto ext = it->path().extension();
		if (ext == ".png" || ext == ".tga" || ext == ".jpg" || ext == ".jpeg")
		{
			//continue;

			std::cout << it->path() << std::endl;

			auto texture = std::make_shared<Texture2D>(it->path().string());
			textures.emplace(texture);
		}

	}

}
