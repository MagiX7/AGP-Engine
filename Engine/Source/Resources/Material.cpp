#include "Material.h"

#include "../engine.h"

#include "../Renderer/Shader.h"

Material::Material(std::string materialName) : name(materialName)
{
	shader = std::make_shared<Shader>("Assets/Shaders/model.glsl", "MODEL");
}

Material::~Material()
{
}

void Material::Bind()
{
	shader->Bind();
	shader->SetUniformVec3f("uAlbedoColor", albedoColor);
	//shader->SetUniform1f("smoothness", smoothness);

	shader->SetUniform1i("hasAlbedoMap", albedoMap ? 1 : 0);
	if (albedoMap)
	{
		albedoMap->Bind(0);
		shader->SetUniform1i("uAlbedoMap", 0);
	}

	shader->SetUniform1i("hasNormalMap", normalMap ? 1 : 0);
	if (normalMap)
	{
		normalMap->Bind(1);
		shader->SetUniform1i("uNormalMap", 1);
	}

	shader->SetUniform1i("hasMetallicMap", metallicMap ? 1 : 0);
	if (metallicMap)
	{
		metallicMap->Bind(2);
		shader->SetUniform1i("uMetallicMap", 2);
	}

	shader->SetUniform1i("hasRoughnessMap", specularMap ? 1 : 0);
	if (specularMap)
	{
		specularMap->Bind(3);
		shader->SetUniform1i("uRoughnessMap", 3);
	}
	
}

void Material::Unbind()
{
	shader->Unbind();
}
