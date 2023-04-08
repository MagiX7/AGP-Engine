#pragma once

#include "Buffer.h"

#include <memory>

class VertexArray
{
public:
	VertexArray();
	virtual ~VertexArray();

	void Bind();
	void Unbind();

	void SetVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

private:
	unsigned int id;
	std::shared_ptr<VertexBuffer> vbo;
	std::shared_ptr<IndexBuffer> ibo;

};