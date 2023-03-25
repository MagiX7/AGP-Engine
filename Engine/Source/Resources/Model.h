#pragma once

#include "../Renderer/Vertex.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/VertexArray.h"
#include "Material.h"

#include <vector>
#include <stdint.h>

//struct Submesh
//{
//	VertexBufferLayout vertexBufferLayout;
//	std::vector<float> vertices;
//	std::vector<uint32_t> indices;
//	uint32_t vertexOffset;
//	uint32_t indexOffset;
//
//	std::vector<VertexArray> vaos;
//};

//class Submesh
//{
//public:
//	Submesh(std::vector<float> vertices, std::vector<uint32_t> indices);
//	virtual ~Submesh();
//
//	inline const VertexBufferLayout& GetVertexBufferLayout() { return vertexBufferLayout; }
//	inline const std::vector<float>& GetVertices() { return vertices; }
//	inline const std::vector<uint32_t>& GetVertices() { return indices; }
//
//
//private:
//	VertexBufferLayout vertexBufferLayout;
//	std::vector<float> vertices;
//	std::vector<uint32_t> indices;
//	uint32_t vertexOffset;
//	uint32_t indexOffset;
//
//	std::shared_ptr<VertexArray> vertexArray;
//	//std::vector<VertexArray> vaos;
//};


//struct Mesh
//{
//	std::vector<Submesh> submeshes;
//	std::shared_ptr<VertexBuffer> vbo;
//	std::shared_ptr<IndexBuffer> ibo;
//};

class Mesh
{
public:
	Mesh() {};
	Mesh(const std::string& name, std::vector<float> vertices, std::vector<uint32_t> indices, VertexBufferLayout layout);
	virtual ~Mesh();

	void Draw();

	void SetMaterial(std::shared_ptr<Material> mat) { material = mat; }
		
	inline const std::shared_ptr<VertexBuffer>& GetVertexBuffer() { return vbo; }
	inline const std::shared_ptr<IndexBuffer>& GetIndexBuffer() { return ibo; }

private:
	std::string name;

	std::shared_ptr<VertexArray> vao;
	std::shared_ptr<VertexBuffer> vbo;
	std::shared_ptr<IndexBuffer> ibo;
	std::shared_ptr<Material> material;

	std::vector<float> vertices;
	std::vector<uint32_t> indices;
};


class Model
{
public:
	Model(const std::string& name);
	virtual ~Model();

	inline const std::vector<std::shared_ptr<Mesh>>& GetMeshes() { return meshes; }

	inline const std::vector<Material>& GetMaterials() { return materials; }
	inline Material& GetFirstMaterial() { return materials[0]; }

	inline void AddMesh(std::shared_ptr<Mesh> mesh) { meshes.push_back(mesh); }

	void Draw();

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

private:
	std::string name;
	std::vector<std::shared_ptr<Mesh>> meshes;
	std::vector<Material> materials;

	glm::mat4 transform;
	glm::vec3 position;
	glm::vec3 eulerAngles;
	glm::vec3 scale;
};
