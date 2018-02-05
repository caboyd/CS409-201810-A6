//
//  MaterialForShader.cpp
//
//  This file is part of the Shaders extension to the
//    ObjLibrary, by Richard Hamilton, which is copyright
//    Hamilton 2009-2016.
//
//  You may use these files for any purpose as long as you do
//    not explicitly claim them as your own work or object to
//    other people using them.
//
//  If you are destributing the source files, you must not
//    remove this notice.  If you are only destributing compiled
//    code, no credit is required.
//
//  A (theoretically) up-to-date version of the ObjLibrary can
//    be found at:
//  http://infiniplix.ca/resources/obj_library/
//

#include <cassert>
#include <iostream>

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "MaterialForShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "MaterialForShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"
#include "../glm/glm.hpp"

#include "Vector3.h"  // for printing vectors
#include "ObjShader.h"
#include "MaterialForShader.h"

using namespace std;
using namespace ObjLibrary;
namespace
{
	const glm::vec3 RED(1.0f, 0.0f, 0.0f);

	const bool DEBUGGING_ACTIVATE = false;
}



const glm::vec3 MaterialForShader :: ZERO(0.0f, 0.0f, 0.0f);
const glm::vec3 MaterialForShader :: ONE (1.0f, 1.0f, 1.0f);



MaterialForShader :: MaterialForShader ()
		: m_transparency(1.0),
		  m_emission(RED),
		  m_ambient (ZERO),
		  m_diffuse (ZERO),
		  m_specular(ZERO),
		  m_shininess(1.0f),

		  m_transparency_texture(NO_TEXTURE),
		  m_emission_texture    (NO_TEXTURE),
		  m_ambient_texture     (NO_TEXTURE),
		  m_diffuse_texture     (NO_TEXTURE),
		  m_specular_texture    (NO_TEXTURE),
		  m_shininess_texture   (NO_TEXTURE),

		  m_transparency_channel(CHANNEL_RED),
		  m_shininess_channel   (CHANNEL_RED),
		  m_is_seperate_specular(false)
{
	updateActiveTextures();

	assert(invariant());
}

MaterialForShader :: MaterialForShader (float            transparency,
                                        unsigned int     transparency_texture,
                                        unsigned int     transparency_channel,
                                        const glm::vec3& colour,
                                        unsigned int     colour_texture)
		: m_transparency(transparency),
		  m_emission(colour),
		  m_ambient (ZERO),
		  m_diffuse (ZERO),
		  m_specular(ZERO),
		  m_shininess(1.0f),

		  m_transparency_texture(transparency_texture),
		  m_emission_texture    (colour_texture),
		  m_ambient_texture     (NO_TEXTURE),
		  m_diffuse_texture     (NO_TEXTURE),
		  m_specular_texture    (NO_TEXTURE),
		  m_shininess_texture   (NO_TEXTURE),

		  m_transparency_channel(transparency_channel),
		  m_shininess_channel   (CHANNEL_RED),
		  m_is_seperate_specular(false)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(colour, ZERO)));
	assert(glm::all(glm::   lessThanEqual(colour, ONE)));

	updateActiveTextures();

	assert(invariant());
}

MaterialForShader :: MaterialForShader (float            transparency,
                                        unsigned int     transparency_texture,
                                        unsigned int     transparency_channel,
                                        const glm::vec3& emission,
                                        unsigned int     emission_texture,
                                        const glm::vec3& ambient,
                                        unsigned int     ambient_texture,
                                        const glm::vec3& diffuse,
                                        unsigned int     diffuse_texture)
		: m_transparency(transparency),
		  m_emission(emission),
		  m_ambient (ambient),
		  m_diffuse (diffuse),
		  m_specular(ZERO),
		  m_shininess(1.0f),

		  m_transparency_texture(transparency_texture),
		  m_emission_texture    (emission_texture),
		  m_ambient_texture     (ambient_texture),
		  m_diffuse_texture     (diffuse_texture),
		  m_specular_texture    (NO_TEXTURE),
		  m_shininess_texture   (NO_TEXTURE),

		  m_transparency_channel(transparency_channel),
		  m_shininess_channel   (CHANNEL_RED),
		  m_is_seperate_specular(false)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(emission, ZERO)));
	assert(glm::all(glm::   lessThanEqual(emission, ONE)));
	assert(glm::all(glm::greaterThanEqual(ambient,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(ambient,  ONE)));
	assert(glm::all(glm::greaterThanEqual(diffuse,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(diffuse,  ONE)));

	updateActiveTextures();

	assert(invariant());
}

MaterialForShader :: MaterialForShader (float            transparency,
                                        unsigned int     transparency_texture,
                                        unsigned int     transparency_channel,
                                        const glm::vec3& emission,
                                        unsigned int     emission_texture,
                                        const glm::vec3& ambient,
                                        unsigned int     ambient_texture,
                                        const glm::vec3& diffuse,
                                        unsigned int     diffuse_texture,
                                        const glm::vec3& specular,
                                        unsigned int     specular_texture,
                                        float            shininess,
                                        unsigned int     shininess_texture,
                                        unsigned int     shininess_channel,
                                        bool             is_seperate_specular)
		: m_transparency(transparency),
		  m_emission(emission),
		  m_ambient (ambient),
		  m_diffuse (diffuse),
		  m_specular(specular),
		  m_shininess(shininess),

		  m_transparency_texture(transparency_texture),
		  m_emission_texture    (emission_texture),
		  m_ambient_texture     (ambient_texture),
		  m_diffuse_texture     (diffuse_texture),
		  m_specular_texture    (specular_texture),
		  m_shininess_texture   (shininess_texture),

		  m_transparency_channel(transparency_channel),
		  m_shininess_channel   (shininess_channel),
		  m_is_seperate_specular(is_seperate_specular)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(emission, ZERO)));
	assert(glm::all(glm::   lessThanEqual(emission, ONE)));
	assert(glm::all(glm::greaterThanEqual(ambient,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(ambient,  ONE)));
	assert(glm::all(glm::greaterThanEqual(diffuse,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(diffuse,  ONE)));
	assert(glm::all(glm::greaterThanEqual(specular, ZERO)));
	assert(glm::all(glm::   lessThanEqual(specular, ONE)));
	assert(shininess >= 0.0f);
	assert(shininess_channel < CHANNEL_COUNT);
	updateActiveTextures();

	assert(invariant());
}

MaterialForShader :: MaterialForShader (const MaterialForShader& original)
{
	copy(original);

	assert(invariant());
}

MaterialForShader :: ~MaterialForShader ()
{
}

MaterialForShader& MaterialForShader :: operator= (const MaterialForShader& original)
{
	if(&original != this)
		copy(original);

	assert(invariant());
	return *this;
}



const ObjShader::ShaderUniforms& MaterialForShader :: activate () const
{
	assert(ObjShader::isLoaded());

	const ObjShader::ShaderUniforms& uniforms = ObjShader::activateShader();
	activate(uniforms);

	return uniforms;
}

const ObjShader::ShaderUniforms& MaterialForShader :: activate (const glm::mat4x4& model_matrix,
                                                                const glm::mat4x4& view_matrix,
																const glm::mat4x4& model_view_projection_matrix,
                                                                const glm::vec3& camera_position) const
{
	assert(ObjShader::isLoaded());

	const ObjShader::ShaderUniforms& uniforms = ObjShader::activateShader();
	activate(uniforms);

	glUniformMatrix4fv(uniforms.m_model_matrix,					1, false, &(model_matrix[0][0]));
	glUniformMatrix4fv(uniforms.m_view_matrix,					1, false, &(view_matrix[0][0]));
	glUniformMatrix4fv(uniforms.m_model_view_projection_matrix,	1, false, &(model_view_projection_matrix[0][0]));
	glUniform3fv      (uniforms.m_camera_pos,					1,        &(camera_position.x));

	return uniforms;
}

const ObjShader::ShaderUniforms& MaterialForShader :: activate (const glm::mat4x4& model_matrix,
                                                                const glm::mat4x4& view_matrix,
																const glm::mat4x4& model_view_projection_matrix) const
{
	assert(ObjShader::isLoaded());

	return activate(model_matrix, view_matrix, model_view_projection_matrix, glm::vec3());
}

void MaterialForShader :: activate (const ObjShader::ShaderUniforms& uniforms) const
{
	//
	//  Step 1: Set transparency
	//

	if(DEBUGGING_ACTIVATE)
	{
		cout << "Activating material" << endl;
		cout << "\tTransparency: " << m_transparency << endl;
	}

	glUniform1f(uniforms.m_transparency, m_transparency);

	//
	//  Step 2: Set colour/lighting information
	//

	if(DEBUGGING_ACTIVATE)
	{
		// thse are being implictily typecast to Vector3s so they can be printed
		cout << "\tEmission:     " << m_emission  << endl;
		cout << "\tAmbient:      " << m_ambient   << endl;
		cout << "\tDiffuse:      " << m_diffuse   << endl;
		cout << "\tSpecular:     " << m_specular  << endl;
		cout << "\tShininess:    " << m_shininess << endl;
	}

	glUniform3fv(uniforms.m_emission, 1, &(m_emission.x));
	glUniform3fv(uniforms.m_ambient,  1, &(m_ambient .x));
	glUniform3fv(uniforms.m_diffuse,  1, &(m_diffuse .x));
	glUniform3fv(uniforms.m_specular, 1, &(m_specular.x));
	glUniform1f (uniforms.m_shininess,     m_shininess  );

	if(m_is_seperate_specular)
	{
		if(DEBUGGING_ACTIVATE)
			cout << "\tUsing seperate specular" << endl;
		glUniform1i(uniforms.m_is_seperate_specular, GL_TRUE);
	}
	else
		glUniform1i(uniforms.m_is_seperate_specular, GL_FALSE);

	//
	//  Step 3: Set textures
	//

	glActiveTexture(GL_TEXTURE0);  // transparency
	glBindTexture(GL_TEXTURE_2D, m_transparency_texture);

	glActiveTexture(GL_TEXTURE1);  // emission
	glBindTexture(GL_TEXTURE_2D, m_emission_texture);

	glActiveTexture(GL_TEXTURE2);  // ambient
	glBindTexture(GL_TEXTURE_2D, m_ambient_texture);

	glActiveTexture(GL_TEXTURE3);  // diffuse
	glBindTexture(GL_TEXTURE_2D, m_diffuse_texture);

	glActiveTexture(GL_TEXTURE4);  // specular
	glBindTexture(GL_TEXTURE_2D, m_specular_texture);

	glActiveTexture(GL_TEXTURE5);  // shininess
	glBindTexture(GL_TEXTURE_2D, m_shininess_texture);

	if(DEBUGGING_ACTIVATE)
	{
		if(m_transparency_texture != NO_TEXTURE)
			cout << "\t\tUsing transparency texture (channel " << m_transparency_channel << ")" << endl;
		if(m_emission_texture     != NO_TEXTURE)
			cout << "\t\tUsing emission  texture" << endl;
		if(m_ambient_texture      != NO_TEXTURE)
			cout << "\t\tUsing ambient   texture" << endl;
		if(m_diffuse_texture      != NO_TEXTURE)
			cout << "\t\tUsing diffuse   texture" << endl;
		if(m_specular_texture     != NO_TEXTURE)
			cout << "\t\tUsing specular  texture" << endl;
		if(m_shininess_texture    != NO_TEXTURE)
			cout << "\t\tUsing shininess texture (channel " << m_shininess_channel << ")" << endl;
	}

	//
	//  Step 4: Set which textures to use for what
	//

	// location of the sampler, slot of the texture
	glUniform1i(uniforms.m_transparency_texture, 0);
	glUniform1i(uniforms.m_emission_texture,     1);
	glUniform1i(uniforms.m_ambient_texture,      2);
	glUniform1i(uniforms.m_diffuse_texture,      3);
	glUniform1i(uniforms.m_specular_texture,     4);
	glUniform1i(uniforms.m_shininess_texture,    5);


	glUniform1i(uniforms.m_transparency_channel, m_transparency_channel);
	glUniform1i(uniforms.m_shininess_channel,    m_shininess_channel);

	glUniform1iv(uniforms.m_a_is_texture_active, 6, ma_is_texture_active);
}



void MaterialForShader :: initDefault ()
{
	m_transparency = 1.0;
	m_emission     = RED;
	m_ambient      = ZERO;
	m_diffuse      = ZERO;
	m_specular     = ZERO;
	m_shininess    = 1.0f;

	m_transparency_texture = NO_TEXTURE;
	m_emission_texture     = NO_TEXTURE;
	m_ambient_texture      = NO_TEXTURE;
	m_diffuse_texture      = NO_TEXTURE;
	m_specular_texture     = NO_TEXTURE;
	m_shininess_texture    = NO_TEXTURE;

	updateActiveTextures();

	m_transparency_channel = CHANNEL_RED;
	m_shininess_channel    = CHANNEL_RED;
	m_is_seperate_specular = false;

	assert(invariant());
}

void MaterialForShader :: initFlat (float            transparency,
                                    unsigned int     transparency_texture,
                                    unsigned int     transparency_channel,
                                    const glm::vec3& colour,
                                    unsigned int     colour_texture)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(colour, ZERO)));
	assert(glm::all(glm::   lessThanEqual(colour, ONE)));

	m_transparency = transparency;
	m_emission     = colour;
	m_ambient      = ZERO;
	m_diffuse      = ZERO;
	m_specular     = ZERO;
	m_shininess    = 1.0f;

	m_transparency_texture = transparency_texture;
	m_emission_texture     =       colour_texture;
	m_ambient_texture      = NO_TEXTURE;
	m_diffuse_texture      = NO_TEXTURE;
	m_specular_texture     = NO_TEXTURE;
	m_shininess_texture    = NO_TEXTURE;

	updateActiveTextures();

	m_transparency_channel = transparency_channel;
	m_shininess_channel    = CHANNEL_RED;
	m_is_seperate_specular = false;

	assert(invariant());
}

void MaterialForShader :: initNoSpecular (float            transparency,
                                          unsigned int     transparency_texture,
                                          unsigned int     transparency_channel,
                                          const glm::vec3& emission,
                                          unsigned int     emission_texture,
                                          const glm::vec3& ambient,
                                          unsigned int     ambient_texture,
                                          const glm::vec3& diffuse,
                                          unsigned int     diffuse_texture)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(emission, ZERO)));
	assert(glm::all(glm::   lessThanEqual(emission, ONE)));
	assert(glm::all(glm::greaterThanEqual(ambient,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(ambient,  ONE)));
	assert(glm::all(glm::greaterThanEqual(diffuse,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(diffuse,  ONE)));

	m_transparency = transparency;
	m_emission     = emission;
	m_ambient      = ambient;
	m_diffuse      = diffuse;
	m_specular     = ZERO;
	m_shininess    = 1.0f;

	m_transparency_texture = transparency_texture;
	m_emission_texture     =     emission_texture;
	m_ambient_texture      =      ambient_texture;
	m_diffuse_texture      =      diffuse_texture;
	m_specular_texture     = NO_TEXTURE;
	m_shininess_texture    = NO_TEXTURE;

	updateActiveTextures();

	m_transparency_channel = transparency_channel;
	m_shininess_channel    = CHANNEL_RED;
	m_is_seperate_specular = false;

	assert(invariant());
}

void MaterialForShader :: initSpecular (float            transparency,
                                        unsigned int     transparency_texture,
                                        unsigned int     transparency_channel,
                                        const glm::vec3& emission,
                                        unsigned int     emission_texture,
                                        const glm::vec3& ambient,
                                        unsigned int     ambient_texture,
                                        const glm::vec3& diffuse,
                                        unsigned int     diffuse_texture,
                                        const glm::vec3& specular,
                                        unsigned int     specular_texture,
                                        float            shininess,
                                        unsigned int     shininess_texture,
                                        unsigned int     shininess_channel,
                                        bool             is_seperate_specular)
{
	assert(transparency >= 0.0f);
	assert(transparency <= 1.0f);
	assert(transparency_channel < CHANNEL_COUNT);
	assert(glm::all(glm::greaterThanEqual(emission, ZERO)));
	assert(glm::all(glm::   lessThanEqual(emission, ONE)));
	assert(glm::all(glm::greaterThanEqual(ambient,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(ambient,  ONE)));
	assert(glm::all(glm::greaterThanEqual(diffuse,  ZERO)));
	assert(glm::all(glm::   lessThanEqual(diffuse,  ONE)));
	assert(glm::all(glm::greaterThanEqual(specular, ZERO)));
	assert(glm::all(glm::   lessThanEqual(specular, ONE)));
	assert(shininess_channel < CHANNEL_COUNT);
	assert(shininess >= 0.0f);

	m_transparency = transparency;
	m_emission     = emission;
	m_ambient      = ambient;
	m_diffuse      = diffuse;
	m_specular     = specular;
	m_shininess    = shininess;

	m_transparency_texture = transparency_texture;
	m_emission_texture     =     emission_texture;
	m_ambient_texture      =      ambient_texture;
	m_diffuse_texture      =      diffuse_texture;
	m_specular_texture     =     specular_texture;
	m_shininess_texture    =    shininess_texture;

	updateActiveTextures();

	m_transparency_channel = transparency_channel;
	m_shininess_channel    =    shininess_channel;
	m_is_seperate_specular = is_seperate_specular;

	assert(invariant());
}



void MaterialForShader :: updateActiveTextures ()
{
	if(m_transparency_texture != NO_TEXTURE)
		ma_is_texture_active[0] = true;
	else
		ma_is_texture_active[0] = false;

	if(m_emission_texture != NO_TEXTURE)
		ma_is_texture_active[1] = true;
	else
		ma_is_texture_active[1] = false;

	if(m_ambient_texture != NO_TEXTURE)
		ma_is_texture_active[2] = true;
	else
		ma_is_texture_active[2] = false;

	if(m_diffuse_texture != NO_TEXTURE)
		ma_is_texture_active[3] = true;
	else
		ma_is_texture_active[3] = false;

	if(m_specular_texture != NO_TEXTURE)
		ma_is_texture_active[4] = true;
	else
		ma_is_texture_active[4] = false;

	if(m_shininess_texture != NO_TEXTURE)
		ma_is_texture_active[5] = true;
	else
		ma_is_texture_active[5] = false;

	assert(invariant());
}

void MaterialForShader :: copy (const MaterialForShader& original)
{
	m_transparency = original.m_transparency;
	m_emission     = original.m_emission;
	m_ambient      = original.m_ambient;
	m_diffuse      = original.m_diffuse;
	m_specular     = original.m_specular;
	m_shininess    = original.m_shininess;

	m_transparency_texture = original.m_transparency_texture;
	m_emission_texture     = original.m_emission_texture;
	m_ambient_texture      = original.m_ambient_texture;
	m_diffuse_texture      = original.m_diffuse_texture;
	m_specular_texture     = original.m_specular_texture;
	m_shininess_texture    = original.m_shininess_texture;

	m_transparency_channel = original.m_transparency_channel;
	m_shininess_channel    = original.m_shininess_channel;
	for(unsigned int i = 0; i < TEXTURE_COUNT; i++)
		ma_is_texture_active[i] = original.ma_is_texture_active[i];

	m_is_seperate_specular = original.m_is_seperate_specular;

	assert(invariant());
}

bool MaterialForShader :: invariant () const
{
	// test parameter values
	if(m_transparency < 0.0f) return false;
	if(m_transparency > 1.0f) return false;
	if(!glm::all(glm::greaterThanEqual(m_emission, ZERO))) return false;
	if(!glm::all(glm::   lessThanEqual(m_emission, ONE))) return false;
	if(!glm::all(glm::greaterThanEqual(m_ambient,  ZERO))) return false;
	if(!glm::all(glm::   lessThanEqual(m_ambient,  ONE))) return false;
	if(!glm::all(glm::greaterThanEqual(m_diffuse,  ZERO))) return false;
	if(!glm::all(glm::   lessThanEqual(m_diffuse,  ONE))) return false;
	if(!glm::all(glm::greaterThanEqual(m_specular, ZERO))) return false;
	if(!glm::all(glm::   lessThanEqual(m_specular, ONE))) return false;
	if(m_shininess < 0.0f) return false;
	if(m_transparency_channel >= CHANNEL_COUNT) return false;
	if(m_shininess_channel    >= CHANNEL_COUNT) return false;

	// test values in ma_is_texture_active
	if(ma_is_texture_active[0] == (m_transparency_texture == NO_TEXTURE)) return false;
	if(ma_is_texture_active[1] == (    m_emission_texture == NO_TEXTURE)) return false;
	if(ma_is_texture_active[2] == (     m_ambient_texture == NO_TEXTURE)) return false;
	if(ma_is_texture_active[3] == (     m_diffuse_texture == NO_TEXTURE)) return false;
	if(ma_is_texture_active[4] == (    m_specular_texture == NO_TEXTURE)) return false;
	if(ma_is_texture_active[5] == (   m_shininess_texture == NO_TEXTURE)) return false;

	return true;
}

