//
//  MaterialForShader.h
//
//  A module to represent the material information needed to
//    display a model using shaders.
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

#ifndef OBJ_LIBRARY_MATERIAL_FOR_SHADER_H
#define OBJ_LIBRARY_MATERIAL_FOR_SHADER_H

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "MaterialForShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "MaterialForShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../glm/glm.hpp"

#include "ObjShader.h"



namespace ObjLibrary
{

//
//  MaterialForShader
//
//  A class to represent the material information needed to
//    display a model using shaders.
//
//  The transparency value is alpha transparency, with a value
//    of 1.0 representing fully opaque and 0.0 representing
//    invisible.
//
//  Class Invariant:
//    <1> m_transparency >= 0.0f
//    <2> m_transparency <= 1.0f
//    <3> glm::all(glm::greaterThanEqual(m_emission, ZERO))
//    <4> glm::all(glm::   lessThanEqual(m_emission, ONE))
//    <5> glm::all(glm::greaterThanEqual(m_ambient,  ZERO))
//    <6> glm::all(glm::   lessThanEqual(m_ambient,  ONE))
//    <7> glm::all(glm::greaterThanEqual(m_diffuse,  ZERO))
//    <8> glm::all(glm::   lessThanEqual(m_diffuse,  ONE))
//    <9> glm::all(glm::greaterThanEqual(m_specular, ZERO))
//    <10>glm::all(glm::   lessThanEqual(m_specular, ONE))
//    <11>m_shininess >= 0.0f
//    <12>m_transparency_channel < CHANNEL_COUNT
//    <13>m_shininess_channel    < CHANNEL_COUNT
//    <14>ma_is_texture_active[0] !=
//                        (m_transparency_texture == NO_TEXTURE)
//    <15>ma_is_texture_active[1] !=
//                        (    m_emission_texture == NO_TEXTURE)
//    <16>ma_is_texture_active[2] !=
//                        (     m_ambient_texture == NO_TEXTURE)
//    <17>ma_is_texture_active[3] !=
//                        (     m_diffuse_texture == NO_TEXTURE)
//    <18>ma_is_texture_active[4] !=
//                        (    m_specular_texture == NO_TEXTURE)
//    <19>ma_is_texture_active[5] !=
//                        (   m_shininess_texture == NO_TEXTURE)
//

class MaterialForShader
{
public:
//
//  TEXTURE_COUNT
//
//  The number of textures that the ObjLibrary Shaders can use.
//

	static const unsigned int TEXTURE_COUNT = 7;

//
//  NO_TEXTURE
//
//  The constant indicating that no texture should be used for
//    the specified material component.
//

	static const unsigned int NO_TEXTURE = 0;

//
//  CHANNEL_COUNT
//
//  The number of colour channels that can be used what
//    calculating a scalar value from a texture.  These are used
//    for transparency and shininess.
//

	static const unsigned int CHANNEL_COUNT = 3;

//
//  CHANNEL_RED
//
//  A constant indicating that the red channel of the texture
//    should be used.
//

	static const unsigned int CHANNEL_RED = 0;

//
//  CHANNEL_GREEN
//
//  A constant indicating that the green channel of the texture
//    should be used.
//

	static const unsigned int CHANNEL_GREEN = 1;

//
//  CHANNEL_BLUE
//
//  A constant indicating that the blue channel of the texture
//    should be used.
//

	static const unsigned int CHANNEL_BLUE = 2;

//
//  ZERO
//
//  The glm::vec3 constant (0, 0, 0).
//

	static const glm::vec3 ZERO;

//
//  ONE
//
//  The glm::vec3 constant (1, 1, 1).
//

	static const glm::vec3 ONE;

public:
//
//  Default Constructor
//
//  Purpose: To create a MaterialForShader with default values.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new MaterialForShader is created that
//               represents an flat red material.
//

	MaterialForShader ();

//
//  Flat Colour Constructor
//
//  Purpose: To create a new MaterialForShader to represent a
//           flat material with the specified transparency and
//           colour.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> colour: The colour
//    <5> colour_texture: The colour texture map
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(colour, ZERO))
//    <5> glm::all(glm::   lessThanEqual(colour, ONE))
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, the new
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the channel of transparency_texture indicated
//               by transparency_channel.  The colur is
//               calculated the same way.
//

	MaterialForShader (float            transparency,
	                   unsigned int     transparency_texture,
	                   unsigned int     transparency_channel,
	                   const glm::vec3& colour,
	                   unsigned int     colour_texture);

//
//  No Specular Constructor
//
//  Purpose: To create a new MaterialForShader to represent a
//           material with the specified transparency and
//           emission, ambient, and diffuse colours.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> emission: The emission colour
//    <5> emission_texture: The emission texture map
//    <6> ambient: The ambient colour
//    <7> ambient_texture: The ambient texture map
//    <8> diffuse: The diffuse colour
//    <9> diffuse_texture: The diffuse texture map
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(emission, ZERO))
//    <5> glm::all(glm::   lessThanEqual(emission, ONE))
//    <6> glm::all(glm::greaterThanEqual(ambient,  ZERO))
//    <7> glm::all(glm::   lessThanEqual(ambient,  ONE))
//    <8> glm::all(glm::greaterThanEqual(diffuse,  ZERO))
//    <9> glm::all(glm::   lessThanEqual(diffuse,  ONE))
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, the new
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the channel of transparency_texture indicated
//               by transparency_channel.  The emission,
//               ambient, and diffuse colours are calculated the
//               same way.
//

	MaterialForShader (float            transparency,
	                   unsigned int     transparency_texture,
	                   unsigned int     transparency_channel,
	                   const glm::vec3& emission,
	                   unsigned int     emission_texture,
	                   const glm::vec3& ambient,
	                   unsigned int     ambient_texture,
	                   const glm::vec3& diffuse,
	                   unsigned int     diffuse_texture);

//
//  initSpecular
//
//  Purpose: To create a new MaterialForShader to represent a
//           material with the specified transparency, the
//           specified emission, ambient, diffuse, and specular
//           colours, and the specified shininess.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> emission: The emission colour
//    <5> emission_texture: The emission texture map
//    <6> ambient: The ambient colour
//    <7> ambient_texture: The ambient texture map
//    <8> diffuse: The diffuse colour
//    <9> diffuse_texture: The diffuse texture map
//    <10>specular: The specular colour
//    <11>specular_texture: The specular texture map
//    <12>shininess: The shininess
//    <13>shininess_texture: The shininess texture map
//    <14>shininess_channel: Which colour channel to use for
//                           shininess
//    <15>is_seperate_specular: Whether the specular highlight
//                              should ignore the transparency
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(emission, ZERO))
//    <5> glm::all(glm::   lessThanEqual(emission, ONE))
//    <6> glm::all(glm::greaterThanEqual(ambient,  ZERO))
//    <7> glm::all(glm::   lessThanEqual(ambient,  ONE))
//    <8> glm::all(glm::greaterThanEqual(diffuse,  ZERO))
//    <9> glm::all(glm::   lessThanEqual(diffuse,  ONE))
//    <10>glm::all(glm::greaterThanEqual(specular, ZERO))
//    <11>glm::all(glm::   lessThanEqual(specular, ONE))
//    <12>shininess >= 0.0f
//    <13>shininess_channel < CHANNEL_COUNT
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, the new
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the channel of transparency_texture indicated
//               by transparency_channel.  The emission,
//               ambient, diffuse, and specular colours are
//               calculated the same way, as is the shininess.
//               If is_seperate_specular == true, the specular
//               highlights will be calculated seperately and
//               added in without transparency.  Otherwise, the
//               specular highlights will be affected by
//               transparency normally.
//

	MaterialForShader (float            transparency,
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
	                   bool             is_seperate_specular);

//
//  Copy Constructor
//
//  Purpose: To create a MaterialForShader as a copy of another.
//  Parameter(s):
//    <1> original: The MaterialForShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: A new MaterialForShader is created that
//               represents the same material as original.
//

	MaterialForShader (const MaterialForShader& original);

//
//  Destructor
//
//  Purpose: To safely destroy a MaterialForShader without
//           memory leaks.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: All dynamically allocated memory is freed.
//

	~MaterialForShader ();

//
//  Assignment Operator
//
//  Purpose: To modify this MaterialForShader to be a copy of
//           another.
//  Parameter(s):
//    <1> original: The MaterialForShader to copy
//  Precondition(s): N/A
//  Returns: A reference to this MaterialForShader.
//  Side Effect: This MaterialForShader is set to be a copy of
//               original.
//

	MaterialForShader& operator= (
	                         const MaterialForShader& original);

//
//  activate
//
//  Purpose: To activate the appropriate ObjLibrary standard
//           shader and set the uniforms to match this
//           MaterialForShader.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> ObjShader::isLoaded()
//  Returns: The locations for the uniforms.
//  Side Effect: The appropriate standard ObjLibrary shader for
//               the current lighting is activated and shader
//               uniforms are set according to this
//               MaterialForShader.
//

	const ObjShader::ShaderUniforms& activate () const;

//
//  activate
//
//  Purpose: To activate the appropriate ObjLibrary standard
//           shader and set the uniforms to match this
//           MaterialForShader and the specified
//           transformation matrixes and camera position.
//  Parameter(s):
//    <1> model_matrix: The model transformation matrix
//    <2> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//    <3> camera_position: The position of the camera in world
//                         space
//  Precondition(s):
//    <1> ObjShader::isLoaded()
//  Returns: The locations for the uniforms.
//  Side Effect: The appropriate standard ObjLibrary shader for
//               the current lighting is activated.  Shader
//               uniforms for materials are set according to this
//               MaterialForShader.  The model transformation
//               matrix is set to model_matrix, view-projection
//               matrix is set to view_perspective_matrix, and
//               the camera is assumed to be at position
//               camera_pos.
//

	const ObjShader::ShaderUniforms& activate (
	                 const glm::mat4x4& model_matrix,
	                 const glm::mat4x4& view_matrix,
					 const glm::mat4x4& model_view_projection_matrix,
	                 const glm::vec3& camera_position) const;

//
//  activate
//
//  Purpose: To activate the appropriate ObjLibrary standard
//           shader and set the uniforms to match this
//           MaterialForShader and the specified
//           transformation matrixes.  This is a conveniance
//           function for when the models do not have specular
//           highlights (e.g. because they are unlit).
//  Parameter(s):
//    <1> model_matrix: The model transformation matrix
//    <2> view_perspective_matrix: The product of the view and
//                                 projection matrixes
//  Precondition(s):
//    <1> ObjShader::isLoaded()
//  Returns: The locations for the uniforms.
//  Side Effect: The appropriate standard ObjLibrary shader for
//               the current lighting is activated.  Shader
//               uniforms for materials are set according to this
//               MaterialForShader.  The model transformation
//               matrix is set to model_matrix, view-projection
//               matrix is set to view_perspective_matrix, and
//               the camera is assumed to be at the origin.
//

	const ObjShader::ShaderUniforms& activate (
	          const glm::mat4x4& model_matrix,
	          const glm::mat4x4& view_matrix,
		      const glm::mat4x4& model_view_projection_matrix) const;

//
//  activate
//
//  Purpose: To set the shader uniforms to match this
//           MaterialForShader if one of the standard ObjLibrary
//           shaders has already been activated.
//  Parameter(s):
//    <1> uniforms: The locations for the uniforms
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The shader uniforms for the standard ObjLibrary
//               shaders are set according to this
//               MaterialForShader.  The uniforms are assumed to
//               be at the locations specified by uniforms.
//

	void activate (
	           const ObjShader::ShaderUniforms& uniforms) const;

//
//  initDefault
//
//  Purpose: To initialize this MaterialForShader with default
//           values.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: This MaterialForShader is set to represent a
//               opaque flat red material.  It is identical to a
//               MaterialForShader created with the default
//               constructor.
//

	void initDefault ();

//
//  initFlat
//
//  Purpose: To initialize this MaterialForShader to represent a
//           flat material with the specified transparency and
//           colour.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> colour: The colour
//    <5> colour_texture: The colour texture map
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(colour, ZERO))
//    <5> glm::all(glm::   lessThanEqual(colour, ONE))
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, this
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the red component of transparency_texture.  The
//               colour is calculated the same way.
//

	void initFlat (float            transparency,
	               unsigned int     transparency_texture,
	               unsigned int     transparency_channel,
	               const glm::vec3& colour,
	               unsigned int     colour_texture);

//
//  initNoSpecular
//
//  Purpose: To initialize this MaterialForShader to represent a
//           material with the specified transparency and
//           emission, ambient, and diffuse colours.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> emission: The emission colour
//    <5> emission_texture: The emission texture map
//    <6> ambient: The ambient colour
//    <7> ambient_texture: The ambient texture map
//    <8> diffuse: The diffuse colour
//    <9> diffuse_texture: The diffuse texture map
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(emission, ZERO))
//    <5> glm::all(glm::   lessThanEqual(emission, ONE))
//    <6> glm::all(glm::greaterThanEqual(ambient,  ZERO))
//    <7> glm::all(glm::   lessThanEqual(ambient,  ONE))
//    <8> glm::all(glm::greaterThanEqual(diffuse,  ZERO))
//    <9> glm::all(glm::   lessThanEqual(diffuse,  ONE))
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, this
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the red component of transparency_texture.  The
//               emission, ambient, and diffuse colours are
//               calculated the same way.
//

	void initNoSpecular (float            transparency,
	                     unsigned int     transparency_texture,
	                     unsigned int     transparency_channel,
	                     const glm::vec3& emission,
	                     unsigned int     emission_texture,
	                     const glm::vec3& ambient,
	                     unsigned int     ambient_texture,
	                     const glm::vec3& diffuse,
	                     unsigned int     diffuse_texture);

//
//  initSpecular
//
//  Purpose: To initialize this MaterialForShader to represent a
//           material with the specified transparency, the
//           specified emission, ambient, diffuse, and specular
//           colours, and the specified shininess.
//  Parameter(s):
//    <1> transparency: The transparency
//    <2> transparency_texture: The transparency texture map
//    <3> transparency_channel: Which colour channel to use for
//                              transparency
//    <4> emission: The emission colour
//    <5> emission_texture: The emission texture map
//    <6> ambient: The ambient colour
//    <7> ambient_texture: The ambient texture map
//    <8> diffuse: The diffuse colour
//    <9> diffuse_texture: The diffuse texture map
//    <10>specular: The specular colour
//    <11>specular_texture: The specular texture map
//    <12>shininess: The shininess
//    <13>shininess_texture: The shininess texture map
//    <14>shininess_channel: Which colour channel to use for
//                           shininess
//    <15>is_seperate_specular: Whether the specular highlight
//                              should ignore the transparency
//  Precondition(s):
//    <1> transparency >= 0.0f
//    <2> transparency <= 1.0f
//    <3> transparency_channel < CHANNEL_COUNT
//    <4> glm::all(glm::greaterThanEqual(emission, ZERO))
//    <5> glm::all(glm::   lessThanEqual(emission, ONE))
//    <6> glm::all(glm::greaterThanEqual(ambient,  ZERO))
//    <7> glm::all(glm::   lessThanEqual(ambient,  ONE))
//    <8> glm::all(glm::greaterThanEqual(diffuse,  ZERO))
//    <9> glm::all(glm::   lessThanEqual(diffuse,  ONE))
//    <10>glm::all(glm::greaterThanEqual(specular, ZERO))
//    <11>glm::all(glm::   lessThanEqual(specular, ONE))
//    <12>shininess >= 0.0f
//    <13>shininess_channel < CHANNEL_COUNT
//  Returns: N/A
//  Side Effect: If transparency_texture == NO_TEXTURE, this
//               MaterialForShader is set to have a transparency
//               of transparency.  Otherwise, it is set to have
//               a transparency of transparency multiplied by
//               the red component of transparency_texture.  The
//               emission, ambient, diffuse, and specular
//               colours are calculated the same way, as is the
//               shininess.
//

	void initSpecular (float            transparency,
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
	                   bool             is_seperate_specular);

private:
//
//  updateActiveTextures
//
//  Purpose: To update the list of which textures this
//           MaterialForShader uses.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The contents of ma_is_texture_active are
//               recalculated.
//

	void updateActiveTextures ();

//
//  copy
//
//  Purpose: To set this MaterialForShader to be a copy of the
//           specified MaterialForShader.
//  Parameter(s):
//    <1> original: The MaterialForShader to copy
//  Precondition(s): N/A
//  Returns: N/A
//  Side Effect: The contents of this MaterialForShader are
//               replaced by a copy of original.
//

	void copy (const MaterialForShader& original);

//
//  invariant
//
//  Purpose: To determine if the class invariant is true.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether the class invariant is true.
//  Side Effect: N/A
//

	bool invariant () const;

private:
	float     m_transparency;
	glm::vec3 m_emission;
	glm::vec3 m_ambient;
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	float     m_shininess;

	unsigned int m_transparency_texture;
	unsigned int m_emission_texture;
	unsigned int m_ambient_texture;
	unsigned int m_diffuse_texture;
	unsigned int m_specular_texture;
	unsigned int m_shininess_texture;

	// these are really bools, but OpenGL requires ints
	unsigned int m_transparency_channel;
	unsigned int m_shininess_channel;
	int  ma_is_texture_active[TEXTURE_COUNT];

	bool m_is_seperate_specular;
};



}  // end of namespace ObjLibrary

#endif
