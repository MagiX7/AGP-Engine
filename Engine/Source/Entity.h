#pragma once

#include "Resources/Model.h"

#include <string>
#include <glm/glm.hpp>

class Entity
{
public:
	Entity(const std::string& name, const std::shared_ptr<Model>& model);
	virtual ~Entity();

	inline const std::string& GetName() { return name; }
	inline void SetName(const std::string& value) { name = value; }

	std::shared_ptr<Model> GetModel() { return model; }

	void SetPosition(const glm::vec3& pos) { position = pos; UpdateTransform(); }
	inline const glm::vec3& GetPosition() const { return position; }
	inline glm::vec3& GetPosition() { return position; }

	void SetRotation(const glm::vec3& rot) { eulerAngles = rot; UpdateTransform(); }
	inline const glm::vec3& GetRotation() const { return eulerAngles; }
	inline glm::vec3& GetRotation() { return eulerAngles; }

	void SetScale(const glm::vec3& sca) { scale = sca; UpdateTransform(); }
	inline const glm::vec3& GetScale() const { return scale; }
	inline glm::vec3& GetScale() { return scale; }

	void UpdateTransform();
	const glm::mat4& GetTransform();

public:
	uint32_t localParamsOffset;
	uint32_t localParamsSize;

private:
	std::shared_ptr<Model> model;
	std::string name;

	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 eulerAngles;
	glm::vec3 scale;
};