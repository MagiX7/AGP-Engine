#pragma once

#include <glm/glm.hpp>

#include <glad/glad.h>

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
	VertexBuffer(float* vertices, uint32_t verticesCount);
	virtual ~VertexBuffer();

	void Bind();
	void Unbind();

	inline unsigned int GetId() { return id; }

	void SetLayout(const VertexBufferLayout& bufferLayout);
	inline const VertexBufferLayout& GetLayout() { return layout; }

private:
	unsigned int id;
	uint32_t count;

	VertexBufferLayout layout;
};

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	virtual ~IndexBuffer();

	void Bind();
	void Unbind();

	inline const uint32_t& GetCount() const { return count; }

private:
	unsigned int id;
	uint32_t count;
};


// TODO: Have some sort of struct like VertexBufferLayout
class UniformBuffer
{
public:
	UniformBuffer(int maxUniformBufferSize, int maxUniformBlockAlignment);
	virtual ~UniformBuffer();

	//template<typename T>
	//void Map(const float* bufferData)
	//{
	//	glBindBuffer(GL_UNIFORM_BUFFER, id);
	//	data = (uint8_t*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
	//	bufferHead = 0;

	//	memcpy(data + bufferHead, &bufferData, sizeof(T));
	//	bufferHead += sizeof(T);

	//	glUnmapBuffer(GL_UNIFORM_BUFFER);
	//	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//	blockOffset = 0;
	//	blockSize = bufferHead;
	//}
	//void Map(void* bufferData, uint32_t size);
	//void Map(const glm::mat4& viewProj, const glm::mat4& model);
	
	void Bind();
	void BindRange(uint32_t slot, uint32_t offset, uint32_t size);
	void Unbind();

	void Map();
	void Unmap();

	void Push1i(int value);
	void Push1f(float value);
	void PushVector3f(const glm::vec3& value);
	void PushVector4f(const glm::vec4& value);
	void PushMatrix3f(const glm::mat3& value);
	void PushMatrix4f(const glm::mat4& value);

private:
	void PushAlignedData(const void* data, uint32_t size, uint32_t alignment);
	void AlignHead(uint32_t alignment);
	bool IsPowerOf2(uint32_t value);
	uint32_t Align(uint32_t value, uint32_t alignment);

private:
	unsigned int id;
	int maxSize;
	int maxAlignment;
	uint8_t* data;
	uint32_t bufferHead = 0;

	uint32_t blockOffset = 0;
	uint32_t blockSize = 0;

};

