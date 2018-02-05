//
//  LightingManager.h
//
//  A module to keep track of the lights used when displaying
//    ObjModels with shaders.
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

#ifndef OBJ_LIBRARY_LIGHTING_MANAGER_H
#define OBJ_LIBRARY_LIGHTING_MANAGER_H

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "LightingManager can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif

#include "../glm/glm.hpp"



namespace ObjLibrary
{

//
//  LightingManager
//
//  A namespace to keep track of the lights used when displaying
//    ObjModels with shaders.  If you are using fixed pipeline
//    rendering, the LightingManger will run without crashing,
//    but will not control the OpenGL lights.  If you are using
//    fixed-pipeline rendering, I recommend using the built-in
//    OpenGL lights instead.
//
//  I had hoped to use the GL_LIGHT0, etc. values from fixed
//    pipeline OpenGL to represent the lights.  However, these
//    have been removed in newer versions of OpenGL (e.g. 4.4).
//    The lights here are similar, but cannot be used as
//    spotlights.
//
//  Lighting can be enabled or disabled.  If it is disabled,
//    lighting is not used.  If it is enabled, each light can
//    be enabled or disabled.  If lighting is enabled but all
//    lights are disabled, the world will be largely dark.  By
//    default, lighting and all lights are disabled.
//
//  These functions encapsulate a global state.
//
//  Note that diabling lighting does not chnage the properties
//    of the lights, nor does it prevent them being changed or
//    queried.
//
//  Note the models with ilumination mode 0 will be rendered
//    without lighting even if lighting is enabled, so they may
//    look out-of-place.
//

namespace LightingManager
{
//
//  LIGHT_COUNT_MAX
//
//  The maximum number og lights that can be displayed.  This
//    value must always match the LIGHT_COUNT constant in the
//    ShaderManyLights.frag shader.
//
//  Be warned that increasing this value will slow down
//    rendering.
//

const unsigned int LIGHT_COUNT_MAX = 8;

//
//  NO_LIGHTS_ENABLED
//
//  A constant indicating that no lights are currently enabled.
//

const unsigned int NO_LIGHTS_ENABLED = ~0u;

//
//  MANY_LIGHTS_ENABLED
//
//  A constant indicating that more than 1 light is currently
//    enabled.
//

const unsigned int MANY_LIGHTS_ENABLED = ~1u;


//
//  SHADOWS_ENABLED
//
//  A constant indicating that shadows are enabled
//

const unsigned int SHADOWS_ENABLED = ~2u;

//
//  ZERO
//
//  The glm::vec3 constant (0, 0, 0).
//

const glm::vec3 ZERO(0.0f, 0.0f, 0.0f);



//
//  isEnabled
//
//  Purpose: To determine if lighting is currently enabled.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether lighting is enabled.
//  Side Effect: N/A
//

bool isEnabled ();

//
//  isShadowEnabled
//
//  Purpose: To determine if the specified light is enabled.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: Whether light light is enabled.
//  Side Effect: N/A
//

bool isShadowEnabled();

//
//  isLightEnabled
//
//  Purpose: To determine if the specified light is enabled.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: Whether light light is enabled.
//  Side Effect: N/A
//

bool isLightEnabled (unsigned int light);

//
//  isLightPositional
//
//  Purpose: To determine if the specified light is a position
//           light.  Otherwise, it is a directional light.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: Whether light light is a positional light.
//  Side Effect: N/A
//

bool isLightPositional (unsigned int light);

//
//  getLightPosition
//
//  Purpose: To determine the position or direction of the
//           specified light, whichever is applicable.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: If light light is a positional light, its position
//           is returned with a w component of 1.0.  Otherwise,
//           the light light is a directional light and its
//           direction is returned with a w component of 0.0.
//  Side Effect: N/A
//

const glm::vec4& getLightPosition (unsigned int light);

//
//  getLightAmbient
//  getLightDiffuse
//  getLightSpecular
//
//  Purpose: To determine the ambient/diffuse/specular colour of
//           the specified light.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: The ambient/diffuse/specular colour of light light.
//  Side Effect: N/A
//

const glm::vec3& getLightAmbient  (unsigned int light);
const glm::vec3& getLightDiffuse  (unsigned int light);
const glm::vec3& getLightSpecular (unsigned int light);

//
//  getLightAttenuation
//
//  Purpose: To determine the attenuation factors for the
//           specified light.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: The attenuation factors for light light.  The x
//           component represents the constant term, the y
//           component the linear term, and the z component the
//           quadratic term.
//  Side Effect: N/A
//

const glm::vec3& getLightAttenuation (unsigned int light);

//
//  setEnabled
//
//  Purpose: To change whether lighting is currently enabled.
//  Parameter(s):
//    <1> is_lighting: Whether lighting should be enabled.
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: If is_lighting == true, lighting is marked as
//               enabled.  Otherwise, it is marked as disabled.
//

void setEnabled (bool is_lighting);

//
//  setShadowEnabled
//
//  Purpose: To change whether shadows are currently enabled.
//  Parameter(s):
//    <1> is_lighting: Whether shadows should be enabled.
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: If is_shadow == true, shadows is marked as
//               enabled.  Otherwise, it is marked as disabled.
//

void setShadowEnabled(bool is_shadow);

//
//  setLightEnabled
//
//  Purpose: To change whether the specified light is enabled.
//  Parameter(s):
//    <1> light: Which light
//    <2> is_enabled: Whether the light should be enabled.
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: N/A
//  Side Effect: If is_enabled == true, light light is marked as
//               enabled.  Otherwise, it is marked as disabled.
//

void setLightEnabled (unsigned int light,
                      bool is_enabled);

//
//  setLightPositional
//
//  Purpose: To change the specified light to be a positional
//           light at the specified position.
//  Parameter(s):
//    <1> light: Which light
//    <2> position: The position for the light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: N/A
//  Side Effect: Light light is set to be a positional light at
//               position position.
//

void setLightPositional (unsigned int light,
                         const glm::vec3& position);

//
//  setLightDirectional
//
//  Purpose: To change the specified light to be a directional
//           light with the specified direction.
//  Parameter(s):
//    <1> light: Which light
//    <2> direction: The direction for the light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//    <2> length(direction) > 0.0f
//  Returns: N/A
//  Side Effect: Light light is set to be a direction light
//               with direction direction.
//

void setLightDirectional (unsigned int light,
                          const glm::vec3& direction);

//
//  setLightColour
//
//  Purpose: To change the colour of specified light to the
//           specified colour.
//  Parameter(s): N/A
//    <1> light: Which light
//    <2> colour: The colour for the light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//    <2> glm::all(glm::greaterThanEqual(colour, ZERO))
//  Returns: N/A
//  Side Effect: Light light is set to have ambient, diffuse,
//               and specular colours of colour colour.
//  Note: The colour values are permitted to exceed 1.0 so that
//        lights that are bright even after attenuation are can
//        be specified.
//

void setLightColour (unsigned int light,
                     const glm::vec3& colour);

//
//  setLightColour
//
//  Purpose: To change the specified light to have the specified
//           ambient, diffuse, and specular colours.
//  Parameter(s): N/A
//    <1> light: Which light
//    <2> ambient: The ambient colour for the light
//    <3> diffuse: The diffuse colour for the light
//    <4> specular: The specular colour for the light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//    <2> glm::all(glm::greaterThanEqual(ambient,  ZERO))
//    <3> glm::all(glm::greaterThanEqual(diffuse,  ZERO))
//    <4> glm::all(glm::greaterThanEqual(specular, ZERO))
//  Returns: N/A
//  Side Effect: Light light is set to have an ambient colour of
//               ambient, a diffuse colour of diffuse, and a
//               specular colour of specular.
//  Note: The colour values are permitted to exceed 1.0 so that
//        lights that are bright even after attenuation are can
//        be specified.
//

void setLightColour (unsigned int light,
                     const glm::vec3& ambient,
                     const glm::vec3& diffuse,
                     const glm::vec3& specular);

//
//  setLightAttenuation
//
//  Purpose: To change the attenuation factors for the specified
//           light.
//  Parameter(s):
//    <1> light: Which light
//    <2> constant: The constant attenuation term
//    <3> linear: The linear attenuation term
//    <4> quadratic: The quadratic attenuation term
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//    <2> constant  >= 0.0f
//    <3> linear    >= 0.0f
//    <4> quadratic >= 0.0f
//    <5> constant + linear + quadratic > 0.0f
//    <6> (constant  == 1.0f &&
//         linear    == 0.0f &&
//         quadratic == 0.0f) || isLightPositional(light)
//  Returns: N/A
//  Side Effect: The constant attenuation factor for light light
//               is set to constant, the linear attenuation
//               factor to linear, and the quadratic attenuation
//               factor to quadratic.  If the distance to the
//               light is d, the light strength after
//               attenuation is:
//
//                  1.0 / (constant          +
//                         linear    * d     +
//                         quadratic * d * d   )
//

void setLightAttenuation (unsigned int light,
                          float constant,
                          float linear,
                          float quadratic);

//
//  setLightAttenuationNone
//
//  Purpose: To change the specified light to not attenuate with
//           distance.
//  Parameter(s):
//    <1> light: Which light
//  Precondition(s):
//    <1> light < LIGHT_COUNT_MAX
//  Returns: N/A
//  Side Effect: The constant attenuation factor for light light
//               is set to 1.0, and the linear and quadratic
//               attenuation factors are set to 0.0.  This makes
//               the light not attenuate with distance.
//

void setLightAttenuationNone (unsigned int light);

////////////////////////////////////////////////////////////////
//
//  Shader Initialization Functions
//
//  These functions are used to initlaize shader uniforms
//    efficently.
//

//
//  getSingleLightEnabled
//
//  Purpose: To determine the index of the single light that is
//           enabled, if applicable.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: If no lights are enabled, NO_LIGHTS_ENABLED is
//           returned.  If exactly one light is enabled, the
//           index of that light is returned.  Otherwise,
//           MANY_LIGHTS_ENABLED is returned.
//  Side Effect: N/A
//

unsigned int getSingleLightEnabled ();

//
//  isLightEnabledAll
//
//  Purpose: To determine which lights are enabled, formatted as
//           an array of integers.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether the lights are enabled.
//  Side Effect: N/A
//  Note: The array returned is statically allocated and must
//        not be freed.
//

const int* isLightEnabledAll ();

//
//  getLightPositionAll
//
//  Purpose: To determine the positions/directions of all the
//           lights, whichever is applicable in each case,
//           formatted as an array.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The positions/directions of the lights, formatted
//           as an array.  The w component is 1.0 for the
//           positional lights and 0.0 for the directional
//           lights.
//  Side Effect: N/A
//  Note: The array returned is statically allocated and must
//        not be freed.
//

const glm::vec4* getLightPositionAll ();

//
//  getLightAmbientAll
//  getLightDiffuseAll
//  getLightSpecularAll
//
//  Purpose: To determine the ambient/diffuse/specular colour of
//           all the lights, formatted as an array.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The ambient/diffuse/specular colours of the lights.
//  Side Effect: N/A
//  Note: The array returned is statically allocated and must
//        not be freed.
//

const glm::vec3* getLightAmbientAll  ();
const glm::vec3* getLightDiffuseAll  ();
const glm::vec3* getLightSpecularAll ();

//
//  getLightAttenuationAll
//
//  Purpose: To determine the attenuation factors for the all
//           the lights, formatted as an array.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: The attenuation factors for all the lights.  The x
//           components represents the constant terms, the y
//           components the linear terms, and the z components
//           the quadratic terms.
//  Side Effect: N/A
//  Note: The array returned is statically allocated and must
//        not be freed.
//

const glm::vec3* getLightAttenuationAll ();


//
//	Purpose: Getters and setters for the shadow map,
//		shadow_map_space_matrix and shadow distance
//
//
void setShadowMap(unsigned int shadow_map);

void setShadowMapSpaceMatrix(glm::mat4x4& shadow_map_space_matrix);

void setShadowDistance(float distance);

unsigned int getShadowMap();

glm::mat4x4& getShadowMapSpaceMatrix();

float getShadowDistance();


}  // end of namespace LightingManager

}  // end of namespace ObjLibrary

#endif
