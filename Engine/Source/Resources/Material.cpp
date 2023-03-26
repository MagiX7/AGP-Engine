#include "Material.h"

#include "../Renderer/Shader.h"

Material::Material(std::string materialName) : name(materialName)
{
	shader = std::make_shared<Shader>("Assets/Shaders/model.glsl", "MODEL");
}

Material::~Material()
{
}

void Material::Bind(const glm::mat4& viewProjection, const glm::mat4& model)
{
	shader->Bind();
	shader->SetUniformMatrix4f("viewProj", viewProjection);
	shader->SetUniformMatrix4f("model", model);

	shader->SetUniformVec3f("albedoColor", albedoColor);
	
	if (albedoMap)
	{
		albedoMap->Bind(0);
		shader->SetUniform1i("albedoMap", 0);
	}

	if (normalMap)
	{
		normalMap->Bind(1);
		shader->SetUniform1i("normalMap", 1);
	}

}

void Material::Unbind()
{
}
