#include "Model.h"

#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include <iostream>

//Submesh::Submesh(std::vector<float> vertices, std::vector<uint32_t> indices)
//{
//
//}
//
//Submesh::~Submesh()
//{
//}

// ==============================================================

Mesh::Mesh(const std::string& meshName, std::vector<float> vertices, std::vector<uint32_t> indices, VertexBufferLayout layout)
	: name(meshName), material(nullptr)
{
	vao = std::make_shared<VertexArray>();
	
	vbo = std::make_shared<VertexBuffer>(&vertices[0], vertices.size());
	vbo->SetLayout(layout);
	vao->SetVertexBuffer(vbo);

	ibo = std::make_shared<IndexBuffer>(&indices[0], indices.size());
	vao->SetIndexBuffer(ibo);

	//ubo = std::make_shared<UniformBuffer>();
}

Mesh::~Mesh()
{
	vertices.clear();
	indices.clear();
}

void Mesh::Draw(const glm::mat4& viewProj, const glm::mat4& model)
{
	//if (!material)
	//{
	//	std::cout << "Assign a material to mesh " << name << std::endl;
	//	return;
	//}
	material->Bind(viewProj, model);

	vao->Bind();
	ibo->Bind();
	glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, 0);
	ibo->Unbind();
	vao->Unbind();

	material->Unbind();
}

// ==============================================================

Model::Model(const std::string& modelName)
	: name(modelName), position(glm::vec3(0, 0, 0)), eulerAngles(glm::vec3(0.0f)), scale(glm::vec3(1.0f))
{
	UpdateTransform();
}

Model::~Model()
{
	meshes.clear();
}

void Model::Draw(const glm::mat4& viewProj)
{
	for (auto mesh : meshes)
		mesh->Draw(viewProj, transform);
}

void Model::UpdateTransform()
{
	transform = glm::translate(glm::mat4(1.0), position)
		* glm::eulerAngleXYZ(eulerAngles.x, eulerAngles.y, eulerAngles.z)
		* glm::scale(glm::mat4(1.0f), scale);
}

const glm::mat4& Model::GetTransform()
{
	return transform;
}

