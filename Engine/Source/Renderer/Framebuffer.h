#pragma once

#include <glm/vec2.hpp>

struct FramebufferAttachments
{
	bool includeColor = true;
	int colorChannels = 4;
	bool includeNormals = true;
	bool includePosition = true;
	bool includeDepth = true;
	bool includeMetallic = false;
	bool includeRoughness = false;
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
	inline unsigned int GetMetallicAttachment() { return metallicAttachment; }
	inline unsigned int GetRoughnessAttachment() { return roughnessAttachment; }

	glm::vec2 GetSize() { return { width, height }; }

private:
	void Create();

private:
	unsigned int id = -1;
	unsigned int colorAttachment = -1;
	unsigned int normalsAttachment = -1;
	unsigned int positionAttachment = -1;
	unsigned int depthAttachment = -1;
	unsigned int depthRenderbuffer = -1;
	unsigned int metallicAttachment = -1;
	unsigned int roughnessAttachment = -1;
	//unsigned int renderedBufferRenderer;

	float width, height;

	FramebufferAttachments attachmentsInfo;

};