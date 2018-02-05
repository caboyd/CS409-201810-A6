#include "DepthTexture.h"
#include <cassert>
#include <iostream>


DepthTexture::DepthTexture()
{
	initialized = false;
}

void DepthTexture::setShadowMapSize(unsigned size)
{
	texture_size = size;
}

void DepthTexture::startRenderToDepthTexture() const
{
	assert(initialized);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);
	glViewport(0, 0, texture_size, texture_size);
	// Render on the whole framebuffer, complete from the lower left corner to the upper right
	glUseProgram(depth_program_id);
}

unsigned DepthTexture::getDepthMatrixUniformLocation() const
{
	return depth_matrix_id;
}

unsigned DepthTexture::getTexture() const
{
	return depth_texture;
}

void DepthTexture::renderDepthTextureToQuad(unsigned offsetX, unsigned offsetY, unsigned width,
                                                   unsigned height) const
{
	assert(initialized);
	glViewport(offsetX, offsetY, width, height); // Render on a corner of the screen

	// Use render to quad shader
	glUseProgram(quad_program_id);

	// Bind the texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	// Set our "renderedTexture" sampler to use Texture Unit 0
	glUniform1i(tex_id, 0);

	// 1st attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glVertexAttribPointer(
		0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
		3, // size
		GL_FLOAT, // type
		GL_FALSE, // normalized?
		0, // stride
		(void*)0 // array buffer offset
	);
	// Draw the 2 trianges
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

	glDisableVertexAttribArray(0);
}

void DepthTexture::init(const unsigned shadow_map_size)
{
	initialized = true;
	this->texture_size = shadow_map_size;

	// Quad program is used to render a texture to the screen
	quad_program_id = ObjLibrary::ObjShader::initProgramStart(shader_folder + quad_shader_vert,
	                                                          shader_folder + quad_shader_frag);
	ObjShader::initProgramEnd(quad_program_id);
	tex_id = glGetUniformLocation(quad_program_id, "texture1");

	//Depth program is used to render anything to a depth texture for shadow mapping
	depth_program_id = ObjShader::initProgramStart(shader_folder + depthRTT_vert, shader_folder + depthRTT_frag);
	ObjShader::initProgramEnd(depth_program_id);
	depth_matrix_id = glGetUniformLocation(depth_program_id, "depthMVP");


	//Set up a frame buffer and texture for depth texture renduring for shadow mapping

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	depth_frame_buffer = 0;
	glGenFramebuffers(1, &depth_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer);

	// The texture we're going to render to
	glGenTextures(1, &depth_texture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, depth_texture);

	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadow_map_size, shadow_map_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
	             0);

	//Image filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);
	glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "frame buffer error" << std::endl;

	static const GLfloat g_quad_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	glGenBuffers(1, &quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
}
