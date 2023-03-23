#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	Camera() {};
	Camera(const glm::vec3& pos, float verticalFov, float camAspectRatio);
	virtual ~Camera();

	inline const glm::mat4& GetProjection() { return proj; }
	inline const glm::mat4& GetView() { return view; }

private:
	void ReCalculateMatrices();

private:
	glm::mat4 proj;
	glm::mat4 view;

	glm::vec3 position;
	glm::vec3 rotation;

	float yFov;
	float aspectRatio;
	float near;
	float far;

};