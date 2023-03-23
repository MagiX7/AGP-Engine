#include "Model.h"

#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

//Submesh::Submesh(std::vector<float> vertices, std::vector<uint32_t> indices)
//{
//
//}
//
//Submesh::~Submesh()
//{
//}

// ==============================================================

Mesh::Mesh(std::vector<float> vertices, std::vector<uint32_t> indices, VertexBufferLayout layout) : material(nullptr)
{
	vao = std::make_shared<VertexArray>();
	
	vbo = std::make_shared<VertexBuffer>(&vertices[0], vertices.size());
	//vertexBufferLayout = layout;
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

void Mesh::Draw()
{
	vao->Bind();
	ibo->Bind();
	glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, 0);
	ibo->Unbind();
	vao->Unbind();
}

// ==============================================================

Model::Model() : transform(glm::mat4(1.0f))
{
	
}

Model::~Model()
{
	meshes.clear();
}

void Model::Draw()
{
	for (auto mesh : meshes)
		mesh->Draw();
}

const glm::mat4& Model::GetTransform()
{
	return glm::translate(glm::mat4(1.0), position);
}

