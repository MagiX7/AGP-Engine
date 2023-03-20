#pragma once

#include "../Renderer/Vertex.h"
#include "../Renderer/Buffer.h"
#include "../Renderer/VertexArray.h"

#include <vector>
#include <stdint.h>

struct Submesh
{
	VertexBufferLayout vertexBufferLayout;
	std::vector<float> vertices;
	std::vector<uint32_t> indices;
	uint32_t vertexOffset;
	uint32_t indexOffset;

	std::vector<VertexArray> vaos;
};

struct Mesh
{
	std::vector<Submesh> submeshes;
	std::shared_ptr<VertexBuffer> vbo;
	std::shared_ptr<IndexBuffer> ibo;
};

struct Model
{
	std::shared_ptr<Mesh> mesh;
	std::vector<Material> materials;
};