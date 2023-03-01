#pragma once
#include "../globalOpenGL.h"
#include <string>

static const GLenum draw_bufer[] =
{
	GL_COLOR_ATTACHMENT0,			// vertexMaterialColorID
	GL_COLOR_ATTACHMENT1,			// vertexNormalID
	GL_COLOR_ATTACHMENT2,			// vertexWorldPositionID
	GL_COLOR_ATTACHMENT3			// vertexSpecularID
};

class cFBO
{
public:
	cFBO();
	~cFBO();

	GLuint ID;
	GLuint vertexMaterialColorID;
	GLuint vertexNormalID;
	GLuint vertexWorldPositionID;
	GLuint vertexSpecularID;
	GLuint depthTextureID;
	GLint width;
	GLint height;

	bool init(int width, int height, std::string& error);
	void shutdown();
	bool reset(int width, int height, std::string& error);

	void clearBuffer(bool clearColor, bool clearDepth);
	void clearColorBuffer(int bufferIndex);
	void clearDepthBuffer(int bufferIndex);
	void clearStencilBuffer();

	int getMaxColorAttachment();
	int getMaxDrawBuffer();
	

private:

};


