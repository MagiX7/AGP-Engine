#include "VertexArray.h"

#include <glad/glad.h>

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind()
{
    glBindVertexArray(id);
}

void VertexArray::Unbind()
{
    glBindVertexArray(0);
}

void VertexArray::SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer)
{
    glBindVertexArray(id);
    vertexBuffer->Bind();

    const VertexBufferLayout& layout = vertexBuffer->GetLayout();
    for (int i = 0; i < layout.attributes.size(); i++)
    {
        const VertexBufferAttribute& attribute = layout.attributes[i];
        glVertexAttribPointer(i,
            attribute.componentCount,
            GL_FLOAT,
            attribute.normalized ? GL_TRUE : GL_FALSE,
            layout.stride, (const void*)(attribute.offset));
        glEnableVertexAttribArray(i);
    }

    vbo = vertexBuffer;
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(id);
    ibo = indexBuffer;
    ibo->Bind();
}
