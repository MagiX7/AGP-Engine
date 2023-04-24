#include "Camera.h"
#include "../Input.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#define CAMERA_SPEED 20.0f

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

void Camera::Update(float dt)
{
	bool recalc = false;

	if (Input::IsKeyPressed(KeyCode::KEY_W))
	{
		position += forward * dt * CAMERA_SPEED;
		recalc = true;
	}
	
	if (Input::IsKeyPressed(KeyCode::KEY_S))
	{
		position -= forward * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (Input::IsKeyPressed(KeyCode::KEY_A))
	{
		position -= right * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (Input::IsKeyPressed(KeyCode::KEY_D))
	{
		position += right * dt * CAMERA_SPEED;
		recalc = true;
	}

	if (Input::IsKeyPressed(KeyCode::KEY_E))
	{
		position.y += dt * CAMERA_SPEED;
		recalc = true;
	}

	if (Input::IsKeyPressed(KeyCode::KEY_Q))
	{
		position.y -= dt * CAMERA_SPEED;
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
