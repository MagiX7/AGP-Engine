#include "Buffer.h"
#include "Vertex.h"

#include <glm/gtc/type_ptr.hpp>

// Vertex Buffer ===========================================================
//template <typename T>
VertexBuffer::VertexBuffer(float* vertices, uint32_t verticesCount) : count(verticesCount)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesCount, vertices, GL_STATIC_DRAW);
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
IndexBuffer::IndexBuffer(uint32_t* indices, uint32_t indicesCount) : count(indicesCount)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indicesCount, indices, GL_STATIC_DRAW);
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

UniformBuffer::UniformBuffer(int maxUniformBufferSize, int maxUniformBlockAlignment)
    : maxBufferSize(maxUniformBufferSize), uniformBlockAlignment(maxUniformBlockAlignment)
{
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, maxUniformBufferSize, NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
}

//void UniformBuffer::AlignHead()
//{
//    bufferHead = Align(bufferHead, uniformBlockAlignment);
//}

void UniformBuffer::Bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
}

void UniformBuffer::BindRange(uint32_t slot, uint32_t offset, uint32_t size)
{
    glBindBufferRange(GL_UNIFORM_BUFFER, slot, id, offset, size);
}

void UniformBuffer::Unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Map()
{
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    data = (uint8_t*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    bufferHead = 0;
}

void UniformBuffer::Unmap()
{
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Push1i(int value)
{
    PushAlignedData(&value, sizeof(value), 4);
}

void UniformBuffer::Push1f(float value)
{
    PushAlignedData(&value, sizeof(value), 4);
}

void UniformBuffer::PushVector2f(const glm::vec2& value)
{
    PushAlignedData(glm::value_ptr(value), sizeof(value), sizeof(glm::vec4));
}

void UniformBuffer::PushVector3f(const glm::vec3& value)
{
    PushAlignedData(glm::value_ptr(value), sizeof(value), sizeof(glm::vec4));
}

void UniformBuffer::PushVector4f(const glm::vec4& value)
{
    PushAlignedData(glm::value_ptr(value), sizeof(value), sizeof(glm::vec4));
}

void UniformBuffer::PushMatrix3f(const glm::mat3& value)
{
    PushAlignedData(glm::value_ptr(value), sizeof(value), sizeof(glm::vec4));
}

void UniformBuffer::PushMatrix4f(const glm::mat4& value)
{
    PushAlignedData(glm::value_ptr(value), sizeof(value), sizeof(glm::vec4));
}

void UniformBuffer::PushAlignedData(const void* incomingDdata, uint32_t size, uint32_t alignment)
{
    assert(data != NULL, "The buffer must be mapped first");
    AlignHead(alignment);
    memcpy((uint8_t*)data + bufferHead, incomingDdata, size);
    bufferHead += size;
}

void UniformBuffer::AlignHead(uint32_t alignment)
{
    assert(IsPowerOf2(alignment), "The alignment must be a power of 2");
    bufferHead = Align(bufferHead, alignment);
}

bool UniformBuffer::IsPowerOf2(uint32_t value)
{
    return value && !(value & (value - 1));
}

uint32_t UniformBuffer::Align(uint32_t value, uint32_t alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

