#include "Camera.h"
#include "../Engine.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

#define CAMERA_SPEED 60.0f

Camera::Camera(const glm::vec3& pos, const glm::vec3& target, float verticalFov, float camAspectRatio)
	: position(pos), yFov(verticalFov), aspectRatio(camAspectRatio)
{
	this->target = target;

	nearClip = 0.1f;
	farClip = 1000.0f;
	forward = glm::normalize(target - position);
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

		if (delta.x != 0 || delta.y != 0)
		{
			glm::quat rotate = glm::quat(glm::vec3(glm::sign(-position.z) * delta.y, -delta.x, 0) * dt * 0.05f);
			forward = glm::normalize(rotate * forward);
			recalc = true;
		}

		isOrbitting = false;
	}

	if (input.keys[Key::K_LEFT_ALT] == BUTTON_PRESS
		&& input.mouseButtons[MouseButton::LEFT] == BUTTON_PRESS)
	{
		glm::vec2 delta = input.mousePos - lastMousePos;
		
		if (delta.x != 0 || delta.y != 0)
		{
			glm::quat rotate = glm::quat(glm::vec3(glm::sign(-position.z) * delta.y, -delta.x, 0) * dt * 0.2f);
			position = glm::normalize(rotate) * position;
			forward = glm::normalize(target - position);

			isOrbitting = true;
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
	right = glm::normalize(glm::cross({ 0,1,0 }, forward));
	up = glm::cross(forward, right);
	right = glm::normalize(glm::cross(forward, up));

	proj = glm::perspective(glm::radians(yFov), aspectRatio, nearClip, farClip);
	view = glm::lookAt(position, position + forward, up);
	
	viewProj = proj * view;
}
