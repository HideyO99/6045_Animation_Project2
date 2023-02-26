#pragma once
#include "../globalOpenGL.h"
#include <string>

static const GLenum draw_bufer[] =
{
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1		// vertex world position
};

class cFBO
{
public:
	cFBO();
	~cFBO();

	GLuint ID;
	GLuint colorTextureID;
	GLuint vertexWorldPositionID;
	GLuint depthTextureID;
	GLint width;
	GLint height;

	bool init(int width, int height, std::string& error);
	void shutdown();
	bool reset(int width, int height, std::string& error);

	void clearBuffer(int bufferIndex);
	void clearColorBuffer(int bufferIndex);
	void clearDepthBuffer(int bufferIndex);
	void clearStencilBuffer(int bufferIndex);

	int getMaxColorAttachment();
	int getMaxDrawBuffer();
	

private:

};


