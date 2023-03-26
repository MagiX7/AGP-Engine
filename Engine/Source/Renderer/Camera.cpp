#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, float verticalFov, float camAspectRatio)
	: position(pos), yFov(verticalFov), aspectRatio(camAspectRatio)
{
	this->target = target;

	nearClip = 0.01f;
	farClip = 1000.0f;
	ReCalculateMatrices();
}

Camera::~Camera()
{
}

void Camera::SetViewportSize(uint32_t width, uint32_t height)
{
	aspectRatio = (float)width / (float)height;
	ReCalculateMatrices();
}

void Camera::ReCalculateMatrices()
{
	proj = glm::perspective(yFov, aspectRatio, nearClip, farClip);
	const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	view = glm::inverse(transform);

	direction = glm::normalize(position - target);
	right = glm::normalize(glm::cross({ 0,1,0 }, direction));
	this->up = glm::cross(direction, right);
	forward = glm::normalize(glm::cross(up, right));

	viewProj = proj * view;
}
