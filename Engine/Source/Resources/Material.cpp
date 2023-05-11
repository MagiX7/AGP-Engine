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
	//if (Application::GetInstance().GetRenderPath() == RenderPath::FORWARD)
	{
		shader->Bind();
		shader->SetUniformVec3f("albedoColor", albedoColor);
		shader->SetUniform1f("smoothness", smoothness);

		if (albedoMap)
		{
			albedoMap->Bind(0);
			shader->SetUniform1i("uAlbedoMap", 0);
		}

		if (normalMap)
		{
			normalMap->Bind(1);
			shader->SetUniform1i("uNormalMap", 1);
		}
	}
	//else
	{

	}

}

void Material::Unbind()
{
}
