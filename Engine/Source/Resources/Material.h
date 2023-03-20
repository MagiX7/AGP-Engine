#pragma once

#include <string>
#include <glm/vec3.hpp>

struct Material
{
	std::string name;
	glm::vec3 albedo;
	glm::vec3 emissive;
	float smoothness;
	uint32_t albedoTextureIndex;
	uint32_t emissiveTextureIndex;
	uint32_t specularTextureIndex;
	uint32_t normalsTextureIndex;
	uint32_t bumpTextureIndex;
};