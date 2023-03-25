#include "Material.h"

#include "../Renderer/Shader.h"

Material::Material(std::string materialName) : name(materialName)
{
	shader = std::make_shared<Shader>("Assets/Shaders/model.glsl", "MODEL");
}

Material::~Material()
{
}
