#pragma once

#include <glm/vec2.hpp>

class Framebuffer
{
public:
	Framebuffer(float w, float h);
	virtual ~Framebuffer();

	void Bind();
	void Unbind();

	void Resize(float w, float h);

	inline unsigned int GetId() { return id; }
	inline unsigned int GetColorAttachment() { return colorAttachment; }
	inline unsigned int GetDepthAttachment() { return depthAttachment; }
	inline unsigned int GetNormalsAttachment() { return normalsAttachment; }

	glm::vec2 GetSize() { return { width, height }; }

private:
	void Create();

private:
	unsigned int id;
	unsigned int colorAttachment;
	unsigned int normalsAttachment;
	unsigned int depthAttachment;

	float width, height;

};