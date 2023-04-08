#include "Entity.h"

#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

Entity::Entity(const std::string& name, const std::shared_ptr<Model>& model)
{
	this->model = model;
	this->name = name;

	position = eulerAngles = glm::vec3(0);
	scale = glm::vec3(1);
	UpdateTransform();
}

Entity::~Entity()
{
	name.clear();
}

void Entity::UpdateTransform()
{
	transform = glm::translate(glm::mat4(1.0), position)
		* glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z)
		* glm::scale(glm::mat4(1.0f), scale);
}

const glm::mat4& Entity::GetTransform()
{
	return transform;
}