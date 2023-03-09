#pragma once
#include "../globalOpenGL.h"
#include <string>

static const GLenum draw_bufer[] =
{
	GL_COLOR_ATTACHMENT0,			// vertexMaterialColorID
	GL_COLOR_ATTACHMENT1,			// vertexNormalID
	GL_COLOR_ATTACHMENT2,			// vertexWorldPositionID
	GL_COLOR_ATTACHMENT3,			// vertexSpecularID
	GL_COLOR_ATTACHMENT4,			// vertexRefractionID
	GL_COLOR_ATTACHMENT5			// vertexCubeMapID
};

static const GLenum targets[] =
{
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
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
	GLuint vertexRefractionID;
	GLuint vertexCubeMapID;
	GLuint depthTextureID;
	GLint width;
	GLint height;
	GLuint renderID;
	

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


