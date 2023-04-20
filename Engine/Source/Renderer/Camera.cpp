#include "Camera.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define CAMERA_SPEED 200.0f

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, float verticalFov, float camAspectRatio)
	: position(pos), yFov(verticalFov), aspectRatio(camAspectRatio)
{
	this->target = target;

	nearClip = 0.1f;
	farClip = 100.0f;
	ReCalculateMatrices();
}

Camera::~Camera()
{
}

void Camera::Update(Input& input, float dt)
{
	bool recalc = false;

	if (input.keys[K_W] == ButtonState::BUTTON_PRESSED)
	{
		position += forward * dt * CAMERA_SPEED;
		recalc = true;
	}
	
	if (input.keys[Key::K_S] == BUTTON_PRESSED)
	{
		position -= forward * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (input.keys[Key::K_A] == BUTTON_PRESSED)
	{
		position -= right * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (input.keys[Key::K_D] == BUTTON_PRESSED)
	{
		position += right * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (recalc)
		ReCalculateMatrices();

}

void Camera::SetViewportSize(uint32_t width, uint32_t height)
{
	aspectRatio = (float)width / (float)height;
	ReCalculateMatrices();
}

void Camera::ReCalculateMatrices()
{
	proj = glm::perspective(glm::radians(yFov), aspectRatio, nearClip, farClip);
	const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	view = glm::inverse(transform);

	direction = glm::normalize(position - target);
	right = glm::normalize(glm::cross({ 0,1,0 }, direction));
	up = glm::cross(direction, right);
	forward = glm::normalize(glm::cross(up, right));

	viewProj = proj * view;
}
