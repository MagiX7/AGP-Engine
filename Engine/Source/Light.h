#pragma once

#include <glm/glm.hpp>

enum class LightType
{
	NONE,
	DIRECTIONAL,
	POINT,
	SPOT
};

class Light
{
public:
	Light() {}
	Light(LightType lightType, const glm::vec3& position);
	virtual ~Light();

	void OnImGuiRender();

	inline const glm::vec3& GetDiffuse() { return diffuse; }
	float GetIntensity() { return intensity; }

	// Direction for directionals or position for the rest
	const glm::vec3& GetPosition();

	LightType GetType() { return type; }

private:

	LightType type;
	glm::vec3 diffuse = glm::vec3(1);
	float intensity = 1.0f;
	
	// Directional
	glm::vec3 direction;

	// Point
	glm::vec3 position;
	float radius;

};