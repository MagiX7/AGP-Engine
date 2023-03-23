#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(const glm::vec3& pos, float verticalFov, float camAspectRatio)
	: position(pos), yFov(verticalFov), aspectRatio(camAspectRatio)
{
	near = 0.01f;
	far = 1000.0f;
	ReCalculateMatrices();
}

Camera::~Camera()
{
}

void Camera::ReCalculateMatrices()
{
	const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	view = glm::inverse(transform);

	proj = glm::perspective(yFov, aspectRatio, near, far);
	
}
