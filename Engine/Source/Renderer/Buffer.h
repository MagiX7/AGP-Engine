#pragma once

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

