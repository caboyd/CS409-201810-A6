//
//  LightingManager.cpp
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

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "LightingManager can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif

#include "../glm/glm.hpp"

#include "LightingManager.h"

using namespace ObjLibrary;
using namespace ObjLibrary::LightingManager;
namespace
{
	bool g_is_lighting_enabled = false;
	bool g_is_shadow_enabled = false;

	// stores bools, but in int format to match OpenGL shader reuirements
	int       ga_is_light_enabled [LightingManager::LIGHT_COUNT_MAX];

	glm::vec4 ga_light_position   [LightingManager::LIGHT_COUNT_MAX];
	glm::vec3 ga_light_ambient    [LightingManager::LIGHT_COUNT_MAX];
	glm::vec3 ga_light_diffuse    [LightingManager::LIGHT_COUNT_MAX];
	glm::vec3 ga_light_specular   [LightingManager::LIGHT_COUNT_MAX];
	glm::vec3 ga_light_attenuation[LightingManager::LIGHT_COUNT_MAX];

	glm::mat4x4 ga_shadow_map_space_matrix;
	unsigned int ga_shadow_map;
	float ga_shadow_distance;
	
}



bool LightingManager :: isEnabled ()
{
	return g_is_lighting_enabled;
}

bool LightingManager::isShadowEnabled()
{
	return g_is_shadow_enabled;
}

bool LightingManager :: isLightEnabled (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	// make absolutely sure that our ints really are storing bools
	assert(ga_is_light_enabled[light] == (int)(true) ||
	       ga_is_light_enabled[light] == (int)(false));

	if(ga_is_light_enabled[light] != 0)  // i.e. is true
		return true;
	else
		return false;
}

bool LightingManager :: isLightPositional (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	if(ga_light_position[light].w == 1.0f)
		return true;
	else
	{
		assert(ga_light_position[light].w == 0.0f);
		return false;
	}
}

const glm::vec4& LightingManager :: getLightPosition (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	return ga_light_position[light];
}

const glm::vec3& LightingManager :: getLightAmbient  (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	return ga_light_ambient[light];
}

const glm::vec3& LightingManager :: getLightDiffuse  (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	return ga_light_diffuse[light];
}

const glm::vec3& LightingManager :: getLightSpecular (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	return ga_light_specular[light];
}

const glm::vec3& LightingManager :: getLightAttenuation (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	return ga_light_attenuation[light];
}



void LightingManager :: setEnabled (bool is_lighting)
{
	g_is_lighting_enabled = is_lighting;
}

void LightingManager::setShadowEnabled(bool is_shadow)
{
	g_is_shadow_enabled = is_shadow;
}

void LightingManager :: setLightEnabled (unsigned int light,
                                         bool is_enabled)
{
	assert(light < LIGHT_COUNT_MAX);

	ga_is_light_enabled[light] = is_enabled;
}

void LightingManager :: setLightPositional (unsigned int light,
                                            const glm::vec3& position)
{
	assert(light < LIGHT_COUNT_MAX);

	ga_light_position[light] = glm::vec4(position, 1.0f);
}

void LightingManager :: setLightDirectional (unsigned int light,
                                             const glm::vec3& direction)
{
	assert(light < LIGHT_COUNT_MAX);
	assert(length(direction) > 0.0f);

	ga_light_position[light] = glm::vec4(direction, 0.0f);
}

void LightingManager :: setLightColour (unsigned int light,
                                        const glm::vec3& colour)
{
	assert(light < LIGHT_COUNT_MAX);
	assert(glm::all(glm::greaterThanEqual(colour, ZERO)));

	ga_light_ambient [light] = colour;
	ga_light_diffuse [light] = colour;
	ga_light_specular[light] = colour;
}

void LightingManager :: setLightColour (unsigned int light,
                                        const glm::vec3& ambient,
                                        const glm::vec3& diffuse,
                                        const glm::vec3& specular)
{
	assert(light < LIGHT_COUNT_MAX);
	assert(glm::all(glm::greaterThanEqual(ambient, ZERO)));
	assert(glm::all(glm::greaterThanEqual(diffuse, ZERO)));
	assert(glm::all(glm::greaterThanEqual(specular, ZERO)));

	ga_light_ambient [light] = ambient;
	ga_light_diffuse [light] = diffuse;
	ga_light_specular[light] = specular;
}

void LightingManager :: setLightAttenuation (unsigned int light,
                                             float constant,
                                             float linear,
                                             float quadratic)
{
	assert(light < LIGHT_COUNT_MAX);
	assert(constant  >= 0.0f);
	assert(linear    >= 0.0f);
	assert(quadratic >= 0.0f);
	assert(constant + linear + quadratic > 0.0f);
	assert((constant == 1.0f && linear == 0.0f && quadratic == 0.0f) || isLightPositional(light));

	ga_light_attenuation[light] = glm::vec3(constant, linear, quadratic);
}

void LightingManager :: setLightAttenuationNone (unsigned int light)
{
	assert(light < LIGHT_COUNT_MAX);

	ga_light_attenuation[light] = glm::vec3(1.0f, 0.0f, 0.0f);
}



////////////////////////////////////////////////////////////////
//
//  Shader Initialization Functions
//
//  These functions are used to initlaize shader uniforms
//    efficently.
//

unsigned int LightingManager :: getSingleLightEnabled ()
{
	unsigned int single_light = NO_LIGHTS_ENABLED;

	for(unsigned int i = 0; i < LIGHT_COUNT_MAX; i++)
		if(ga_is_light_enabled[i])
		{
			if(single_light == NO_LIGHTS_ENABLED)
				single_light = i;
			else
				single_light = MANY_LIGHTS_ENABLED;
		}

	return single_light;
}

const int* LightingManager :: isLightEnabledAll ()
{
	return ga_is_light_enabled;
}

const glm::vec4* LightingManager :: getLightPositionAll ()
{
	return ga_light_position;
}

const glm::vec3* LightingManager :: getLightAmbientAll ()
{
	return ga_light_ambient;
}

const glm::vec3* LightingManager :: getLightDiffuseAll ()
{
	return ga_light_diffuse;
}

const glm::vec3* LightingManager :: getLightSpecularAll ()
{
	return ga_light_specular;
}

const glm::vec3* LightingManager :: getLightAttenuationAll ()
{
	return ga_light_attenuation;
}

void LightingManager::setShadowMap(unsigned int shadow_map)
{
	ga_shadow_map = shadow_map;
}

void LightingManager::setShadowMapSpaceMatrix(glm::mat4x4& shadow_map_space_matrix)
{
	ga_shadow_map_space_matrix = shadow_map_space_matrix;
}

void LightingManager::setShadowDistance(float distance)
{
	ga_shadow_distance = distance;
}

unsigned LightingManager::getShadowMap()
{
	return  ga_shadow_map;
}

glm::mat4x4& LightingManager::getShadowMapSpaceMatrix()
{
	return ga_shadow_map_space_matrix;
}

float LightingManager::getShadowDistance()
{
	return ga_shadow_distance;
}



