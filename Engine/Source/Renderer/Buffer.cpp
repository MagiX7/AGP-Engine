#include "Buffer.h"
#include "Vertex.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

// Vertex Buffer ===========================================================
//template <typename T>
VertexBuffer::VertexBuffer(float* vertices, uint16_t verticesCount) : count(verticesCount)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(/*T*/float) * verticesCount, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &id);
}

void VertexBuffer::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetLayout(const VertexBufferLayout& bufferLayout)
{
    layout = bufferLayout;
}

// Vertex Buffer ===========================================================

// Index Buffer ============================================================
IndexBuffer::IndexBuffer(uint32_t* indices, uint16_t indicesCount) : count(indicesCount)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * count, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

IndexBuffer::~IndexBuffer()
{
    glDeleteBuffers(1, &id);
}

void IndexBuffer::Bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::Unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Index Buffer ============================================================

UniformBuffer::UniformBuffer(int maxUniformBufferSize) : maxSize(maxUniformBufferSize)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, maxUniformBufferSize, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
}

void UniformBuffer::Map(void* bufferData, uint32_t size)
{
    data = (uint8_t*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    bufferHead = 0;

    memcpy(data + bufferHead, bufferData, size);
    bufferHead += size;

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Map(const glm::mat4& viewProj, const glm::mat4& model)
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    data = (uint8_t*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    bufferHead = 0;

    memcpy(data + bufferHead, glm::value_ptr(viewProj), sizeof(glm::mat4));
    bufferHead += sizeof(glm::mat4);

    memcpy(data + bufferHead, glm::value_ptr(model), sizeof(glm::mat4));
    bufferHead += sizeof(glm::mat4);

    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void UniformBuffer::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

