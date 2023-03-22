#pragma once

#include "Texture2D.h"

#include <string>
#include <glm/vec3.hpp>

#include <memory>

class Material
{
public:
	Material(std::string materialName);
	virtual ~Material();

	inline const std::shared_ptr<Texture2D>& GetAlbedoMap() { return albedoMap; }


private:
	std::string name;
	glm::vec3 albedo;
	glm::vec3 emissive;
	float smoothness;

	std::shared_ptr<Texture2D> albedoMap;



	uint32_t albedoTextureIndex;
	uint32_t emissiveTextureIndex;
	uint32_t specularTextureIndex;
	uint32_t normalsTextureIndex;
	uint32_t bumpTextureIndex;
};