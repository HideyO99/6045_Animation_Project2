#include "cFBO.h"

cFBO::cFBO()
{
	ID = 0;
	colorTextureID = 0;
	vertexWorldPositionID = 0;
	depthTextureID = 0;
	width = -1;
	height = -1;
}

cFBO::~cFBO()
{
}

bool cFBO::init(int width, int height, std::string& error)
{
	this->width = width;
	this->height = height;

	GLfloat black[] = { 0.f,0.f,0.f,0.f };

	glGenFramebuffers(1, &(this->ID));
	glBindFramebuffer(GL_FRAMEBUFFER, this->ID);

	//color buffer(texture)
	glGenTextures(1, &(this->colorTextureID));
	glBindTexture(GL_TEXTURE_2D, this->colorTextureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, this->width, this->height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,black);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//vertex world position
	glGenTextures(1, &(this->vertexWorldPositionID));
	glBindTexture(GL_TEXTURE_2D, this->vertexWorldPositionID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, this->width, this->height);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//depth buffer
	glGenTextures(1, &(this->depthTextureID));
	glBindTexture(GL_TEXTURE_2D, this->depthTextureID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, this->width, this->height);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, this->colorTextureID,0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, this->vertexWorldPositionID, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, this->depthTextureID, 0);

	glDrawBuffers(2, draw_bufer);

	//check FrameBuffer status is ready
	bool bFBOReadyStatus = false;
	
	switch (glCheckFramebufferStatus(GL_FRAMEBUFFER))
	{
	case GL_FRAMEBUFFER_COMPLETE:
		bFBOReadyStatus = true;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
		error = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
		error = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
		error = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
		error = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
		error = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_UNSUPPORTED:
		error = "GL_FRAMEBUFFER_UNSUPPORTED";
		bFBOReadyStatus = false;
		break;
	case GL_FRAMEBUFFER_UNDEFINED:
		error = "GL_FRAMEBUFFER_UNDEFINED";
		bFBOReadyStatus = false;
		break;
	default:
		bFBOReadyStatus = false;
		break;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return bFBOReadyStatus;
}

void cFBO::shutdown()
{
	glDeleteTextures(1, &(this->colorTextureID));
	glDeleteTextures(1, &(this->depthTextureID));
	glDeleteTextures(1, &(this->vertexWorldPositionID));
	glDeleteFramebuffers(1, &(this->ID));
}

bool cFBO::reset(int width, int height, std::string& error)
{
	this->shutdown();
	bool result = this->init(width, height, error);
	return result;
}

void cFBO::clearBuffer(int bufferIndex)
{
	this->clearColorBuffer(bufferIndex);
	this->clearDepthBuffer(bufferIndex);
	this->clearStencilBuffer(bufferIndex);
}

void cFBO::clearColorBuffer(int bufferIndex)
{
	glViewport(0, 0, this->width, this->height);
	GLfloat black[] = { 0.f,0.f,0.f,1.f };
	glClearBufferfv(GL_COLOR, bufferIndex, black);
}

void cFBO::clearDepthBuffer(int bufferIndex)
{
	GLfloat one = 1.f;
	glClearBufferfv(GL_DEPTH, bufferIndex, &one);

	GLfloat vertexWorld[] = { 0.f,0.f,0.f,0.f };
	glClearBufferfv(GL_COLOR, 1, vertexWorld);
}

void cFBO::clearStencilBuffer(int bufferIndex)
{
	glStencilMask(0xff);

	glClearBufferfi(GL_DEPTH_STENCIL, 0, 1.f, 0);
}

int cFBO::getMaxColorAttachment()
{
	int maxColorAttach = 0;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttach);

	return maxColorAttach;
}

int cFBO::getMaxDrawBuffer()
{
	int maxDrawBuffer = 0;
	glGetIntegerv(GL_MAX_DRAW_BUFFERS, &maxDrawBuffer);
	return maxDrawBuffer;
}
