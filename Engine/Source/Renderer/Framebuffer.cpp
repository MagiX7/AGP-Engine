#include "Framebuffer.h"

#include <iostream>
#include <glad/glad.h>

Framebuffer::Framebuffer(FramebufferAttachments attInfo, float w, float h)
	: width(w), height(h), attachmentsInfo(attInfo)
{
	id = 0;
	Create();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &id);
	glDeleteTextures(1, &colorAttachment);
	glDeleteTextures(1, &normalsAttachment);
	glDeleteTextures(1, &positionAttachment);
	glDeleteTextures(1, &depthAttachment);
	glDeleteRenderbuffers(1, &depthRenderbuffer);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, width, height);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(float w, float h)
{
	if (w != width || height != h)
	{
		width = w;
		height = h;
		Create();
	}
}

void Framebuffer::Create()
{
	if (id > 0)
	{
		glDeleteFramebuffers(1, &id);
		glDeleteTextures(1, &colorAttachment);
		glDeleteTextures(1, &normalsAttachment);
		glDeleteTextures(1, &positionAttachment);
		glDeleteTextures(1, &depthAttachment);
		glDeleteRenderbuffers(1, &depthRenderbuffer);
	}

	glGenFramebuffers(1, &id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);

	//glGenRenderbuffers(1, &renderedBufferRenderer);
	//glBindRenderbuffer(GL_RENDERBUFFER, renderedBufferRenderer);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16, width, height);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderedBufferRenderer);


	if (attachmentsInfo.includeColor)
	{
		glGenTextures(1, &colorAttachment);
		glBindTexture(GL_TEXTURE_2D, colorAttachment);
		if (attachmentsInfo.colorChannels == 1)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_FLOAT, 0);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
	}

	if (attachmentsInfo.includeNormals)
	{
		glGenTextures(1, &normalsAttachment);
		glBindTexture(GL_TEXTURE_2D, normalsAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalsAttachment, 0);
	}

	if (attachmentsInfo.includePosition)
	{
		glGenTextures(1, &positionAttachment);
		glBindTexture(GL_TEXTURE_2D, positionAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, positionAttachment, 0);
	}

	if (attachmentsInfo.includeDepth)
	{
		glGenRenderbuffers(1, &depthRenderbuffer);
		glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);


		glGenTextures(1, &depthAttachment);
		glBindTexture(GL_TEXTURE_2D, depthAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, depthAttachment, 0);
	}
	
	if (attachmentsInfo.includeMetallic)
	{
		glGenTextures(1, &metallicAttachment);
		glBindTexture(GL_TEXTURE_2D, metallicAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, metallicAttachment, 0);
	}
	
	if (attachmentsInfo.includeRoughness)
	{
		glGenTextures(1, &roughnessAttachment);
		glBindTexture(GL_TEXTURE_2D, roughnessAttachment);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, roughnessAttachment, 0);
	}

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (status)
		{
			case GL_FRAMEBUFFER_UNDEFINED: std::cout << "FRAMEBUFFER UNDEFINED" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: std::cout << "FRAMEBUFFER INCOMPLETE ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: std::cout << "FRAMEBUFFER INCOMPLETE MISSING ATTACHMENT" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: std::cout << "FRAMEBUFFER INCOMPLETE DRAW BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: std::cout << "FRAMEBUFFER INCOMPLETE READ BUFFER" << std::endl; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: std::cout << "FRAMEBUFFER UNSUPPORTED" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: std::cout << "FRAMEBUFFER INCOMPLETE MULTISAMPLE" << std::endl; break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: std::cout << "FRAMEBUFFER INCOMPLETE LAYER TARGETS" << std::endl; break;
			default: std::cout << "UNKNOWN FRAMEBUFFER STATUS ERROR" << std::endl; break;
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
