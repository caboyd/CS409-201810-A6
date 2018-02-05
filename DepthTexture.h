#pragma once
#include "lib/GetGlutWithShaders.h"
#include "lib/ObjLibrary/ObjShader.h"

using namespace ObjLibrary;

const std::string shader_folder = "assets/shaders/";
const std::string quad_shader_vert = "passthrough.vert";
const std::string quad_shader_frag = "simpletexture.frag";
const std::string depthRTT_vert = "depthRTT.vert";
const std::string depthRTT_frag = "depthRTT.frag";

//Manages the depth buffer and texture for use with generating shadows
class DepthTexture
{
	GLuint quad_program_id;
	GLuint depth_program_id;
	GLuint quad_vertexbuffer;
	GLuint depth_frame_buffer;
	GLuint depth_texture;
	GLint tex_id;
	GLint depth_matrix_id;

	bool initialized;
	unsigned int texture_size;

public:
	DepthTexture();

	//Changes the texture dimensions in pixels, recommended size of 2048 or 4096 or 8192
	void setShadowMapSize(unsigned int size);

	//Changes the OpenGL state to start rendering to the depth texture buffer
	void startRenderToDepthTexture() const;

	//Returns the location of the depth matrix uniform in the shader program
	unsigned int getDepthMatrixUniformLocation() const;

	//Returns the location of the texture on the GPU
	unsigned int getTexture() const;

	//Renders the depth texture to the screen
	//Does not work for Sampler2DShadow
	//Must comment out in init function
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//and Change shader to Sampler2D
	void renderDepthTextureToQuad(unsigned int offsetX, unsigned int offsetY, unsigned int width,
	                              unsigned int height) const;

	void init(const unsigned int shadow_map_size);
};
