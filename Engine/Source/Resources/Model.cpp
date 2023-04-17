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
}

Mesh::~Mesh()
{
	vertices.clear();
	indices.clear();
}

void Mesh::Draw(bool bindMaterial)
{
	//if (!material)
	//{
	//	std::cout << "Assign a material to mesh " << name << std::endl;
	//	return;
	//}

	if (bindMaterial)
		material->Bind();

	vao->Bind();
	ibo->Bind();
	glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, 0);
	ibo->Unbind();
	vao->Unbind();

	if (bindMaterial)
		material->Unbind();
}

// ==============================================================

Model::Model(const std::string& modelName)
	: name(modelName)
{
}

Model::~Model()
{
	meshes.clear();
}

void Model::Draw(bool bindMaterial)
{
	for (auto mesh : meshes)
		mesh->Draw(bindMaterial);
}



