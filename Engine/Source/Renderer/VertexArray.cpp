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

    int offset = 0;
    const VertexBufferLayout& layout = vertexBuffer->GetLayout();
    for (int i = 0; i < layout.attributes.size(); i++)
    {
        const VertexBufferAttribute& attribute = layout.attributes[i];
        glVertexAttribPointer(i,
            attribute.componentCount,
            GL_FLOAT,
            attribute.normalized ? GL_TRUE : GL_FALSE,
            layout.stride, (const void*)(attribute.offset)
            /*(void*)(offset * sizeof(GL_FLOAT))*/);
        glEnableVertexAttribArray(i);

        offset += attribute.componentCount;
    }

    vbo = vertexBuffer;

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ScreenSpaceVertex), (void*)(sizeof(float) * 3));
    //glEnableVertexAttribArray(1);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->screenSpaceIbo);
    
}

void VertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(id);
    ibo = indexBuffer;
    ibo->Bind();
}
