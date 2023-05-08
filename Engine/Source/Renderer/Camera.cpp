#include "Camera.h"
#include "../Engine.h"
//#include "../Input.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

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
	static glm::vec2 lastMousePos = { 0,0 };
	auto& input = Application::GetInstance().input;

	bool recalc = false;
	if (input.mouseButtons[MouseButton::RIGHT] == BUTTON_PRESS)
	{
		float speed = CAMERA_SPEED;
		if (input.keys[K_LEFT_SHIFT])
			speed *= 2.0f;

		if (input.keys[K_W] == BUTTON_PRESS)
		{
			position += forward * dt * speed;
			recalc = true;
		}

		if (input.keys[K_S] == BUTTON_PRESS)
		{
			position -= forward * dt * speed;
			recalc = true;
		}

		if (input.keys[K_A] == BUTTON_PRESS)
		{
			position -= right * dt * speed;
			recalc = true;
		}

		if (input.keys[K_D] == BUTTON_PRESS)
		{
			position += right * dt * speed;
			recalc = true;
		}

		if (input.keys[K_E] == BUTTON_PRESS)
		{
			position.y += dt * speed;
			recalc = true;
		}

		if (input.keys[K_Q] == BUTTON_PRESS)
		{
			position.y -= dt * speed;
			recalc = true;
		}


		glm::vec2 delta = input.mousePos - lastMousePos;
		if (delta.x != 0)
		{
			pitch += delta.x * dt * 0.1f;
			recalc = true;
		}
		if (delta.y != 0)
		{
			yaw += /*(up.y > 0.0f ? 1.0f : -1.0f) **/ delta.y * dt * 0.1f;
			recalc = true;
		}
	}

	lastMousePos = input.mousePos;

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
	//const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z);
	const glm::mat4 transform = glm::translate(glm::mat4(1.0), position) * glm::toMat4(glm::quat(glm::vec3(-yaw, -pitch, 0.0)));
	view = glm::inverse(transform);

	glm::vec3 direction = glm::normalize(position - target);
	right = glm::normalize(glm::cross({ 0,1,0 }, direction));
	up = glm::cross(direction, right);
	forward = glm::normalize(glm::cross(up, right));

	viewProj = proj * view;
}
