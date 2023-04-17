#pragma once

#include <glm/vec2.hpp>

struct FramebufferAttachments
{
	bool includeColor = true;
	bool includeNormals = true;
	bool includePosition = true;
	bool includeDepth = true;
};

class Framebuffer
{
public:
	Framebuffer(FramebufferAttachments attInfo, float w, float h);
	virtual ~Framebuffer();

	void Bind();
	void Unbind();

	void Resize(float w, float h);

	inline unsigned int GetId() { return id; }
	inline unsigned int GetColorAttachment() { return colorAttachment; }
	inline unsigned int GetNormalsAttachment() { return normalsAttachment; }
	inline unsigned int GetPositionAttachment() { return positionAttachment; }
	inline unsigned int GetDepthAttachment() { return depthAttachment; }

	glm::vec2 GetSize() { return { width, height }; }

private:
	void Create();

private:
	unsigned int id;
	unsigned int colorAttachment;
	unsigned int normalsAttachment;
	unsigned int positionAttachment;
	unsigned int depthAttachment;
	unsigned int depthRenderbuffer;
	//unsigned int renderedBufferRenderer;

	float width, height;

	FramebufferAttachments attachmentsInfo;

};