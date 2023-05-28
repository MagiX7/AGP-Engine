#include "Camera.h"
#include "../Engine.h"
//#include "../Input.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>

#define CAMERA_SPEED 40.0f

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, float verticalFov, float camAspectRatio)
	: position(pos), yFov(verticalFov), aspectRatio(camAspectRatio)
{
	this->target = target;

	nearClip = 0.1f;
	farClip = 1000.0f;
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
			position += glm::vec3(0,1,0) * dt * speed;
			recalc = true;
		}

		if (input.keys[K_Q] == BUTTON_PRESS)
		{
			position -= glm::vec3(0, 1, 0) * dt * speed;
			recalc = true;
		}


		glm::vec2 delta = input.mousePos - lastMousePos;
		if (delta.x != 0)
		{
			yaw += delta.x * dt * 4.0f;
			recalc = true;
		}
		if (delta.y != 0)
		{
			pitch -= /*(up.y > 0.0f ? 1.0f : -1.0f) **/ delta.y * dt * 4.0f;
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

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	forward = front;

	right = glm::normalize(glm::cross({ 0,1,0 }, forward));
	up = glm::cross(forward, right);
	right = glm::normalize(glm::cross(forward, up));

	view = glm::lookAt(position, position + forward, up);
	
	viewProj = proj * view;
}
