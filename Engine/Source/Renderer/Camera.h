#pragma once

#include "../platform.h"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera() {};
	Camera(const glm::vec3& pos, const glm::vec3& target, float verticalFov, float camAspectRatio);
	virtual ~Camera();

	void Update(float dt);

	void SetViewportSize(uint32_t width, uint32_t height);

	inline const glm::mat4& GetProjection() { return proj; }
	inline const glm::mat4& GetView() { return view; }
	inline const glm::mat4& GetViewProj() { return viewProj; }

	float GetNearClip() const { return nearClip; }
	void SetNearClip(float value) { nearClip = value; ReCalculateMatrices(); }

	float GetFarClip() const { return farClip; }
	void SetFarClip(float value) { farClip = value; ReCalculateMatrices(); }

	float GetVerticalFov() const { return yFov; }
	void SetVerticalFov(float value) { yFov = value; ReCalculateMatrices(); }


	const glm::vec3& GetPosition() const { return position; }
	glm::vec3& GetPosition() { return position; }
	void SetPosition(glm::vec3 value) { position = value; ReCalculateMatrices(); }

	glm::vec3& GetRotation() { return rotation; }
	void SetRotation(glm::vec3 value) { rotation = value; ReCalculateMatrices(); }

	//inline const glm::vec3& GetDirection()const { return direction; }
	inline const glm::vec3& GetUp() const { return up; }
	void SetUp(const glm::vec3& newUp) { up = newUp; ReCalculateMatrices(); }
	inline const glm::vec3& GetRight() const { return right; }
	void SetRight(const glm::vec3& newRight) { right = newRight; ReCalculateMatrices(); }
	inline const glm::vec3& GetForward() const { return forward; }
	void SetForward(const glm::vec3& newForward) { forward = newForward; ReCalculateMatrices(); }

	inline float GetAspectRatio() { return aspectRatio; }

private:
	void ReCalculateMatrices();

private:
	glm::mat4 proj;
	glm::mat4 view;
	glm::mat4 viewProj;

	glm::vec3 position = glm::vec3(0);
	glm::vec3 rotation = glm::vec3(0);
	float yaw = -90, pitch = 0;

	//glm::vec3 direction = glm::vec3(0);
	glm::vec3 up = glm::vec3(0);
	glm::vec3 right = glm::vec3(0);
	glm::vec3 forward = glm::vec3(0);
	glm::vec3 target = glm::vec3(0);

	float yFov = 60.0f;
	float aspectRatio;
	float nearClip;
	float farClip;

};