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
	if (Application::GetInstance().GetRenderPath() == RenderPath::FORWARD)
	{
		shader->Bind();
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
	else
	{

	}

}

void Material::Unbind()
{
}
