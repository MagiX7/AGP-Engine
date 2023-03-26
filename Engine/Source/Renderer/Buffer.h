#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <stdint.h>

struct VertexBufferAttribute
{
	uint8_t location;
	uint8_t componentCount;
	uint8_t offset;
	bool normalized = false;
};

struct VertexBufferLayout
{
	std::vector<VertexBufferAttribute> attributes;
	uint8_t stride = 0;
};

class VertexBuffer
{
public:
	//template <typename T>
	VertexBuffer(float* vertices, uint16_t verticesCount);
	virtual ~VertexBuffer();

	void Bind();
	void Unbind();

	inline unsigned int GetId() { return id; }

	void SetLayout(const VertexBufferLayout& bufferLayout);
	inline const VertexBufferLayout& GetLayout() { return layout; }

private:
	unsigned int id;
	uint16_t count;

	VertexBufferLayout layout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint16_t count);
	virtual ~IndexBuffer();

	void Bind();
	void Unbind();

	inline const uint16_t& GetCount() const { return count; }

private:
	unsigned int id;
	uint16_t count;
};


// TODO: Have some sort of struct like VertexBufferLayout
class UniformBuffer
{
public:
	UniformBuffer(int maxUniformBufferSize);
	virtual ~UniformBuffer();

	void Map(void* bufferData, uint32_t size);
	void Map(const glm::mat4& viewProj, const glm::mat4& model);
	
	void Bind();
	void Unbind();

private:
	unsigned int id;
	int maxSize;
	uint8_t* data;
	uint32_t bufferHead = 0;
};

