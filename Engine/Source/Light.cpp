#include "Light.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

Light::Light(LightType lightType, const glm::vec3& position, const glm::vec3& color)
	: type(lightType), diffuse(color)
{
	switch(type)
	{
		case LightType::DIRECTIONAL:
		{
			direction = glm::normalize(position);
			name = "Directional Light";
			break;
		}
		case LightType::POINT:
		{
			name = "Point Light";
			this->position = position;
			break;
		}
		case LightType::SPOT:
		{
			name = "Spot Light";
			this->position = position;
			break;
		}
	}
}

Light::~Light()
{
}

void Light::OnImGuiRender()
{
	ImGui::ColorPicker3("Diffuse", glm::value_ptr(diffuse));
	ImGui::DragFloat("Intensity", &intensity, 0.01f);

	switch (type)
	{
		case LightType::DIRECTIONAL:
		{
			ImGui::DragFloat3("Direction", glm::value_ptr(direction), 0.1f);
			break;
		}

		case LightType::POINT:
		{
			ImGui::DragFloat3("Position", glm::value_ptr(position), 0.1f);
			break;
		}

	}
}

const glm::vec3& Light::GetPosition()
{
	switch (type)
	{
		case LightType::DIRECTIONAL: return glm::normalize(direction); break;
		case LightType::POINT: case LightType::SPOT: return position; break;
	}
}
