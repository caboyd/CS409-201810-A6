//
//  ObjShaders.cpp
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
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "ObjShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif

#include "../GetGlutWithShaders.h"
#include "../glm/glm.hpp"

#include "ObjStringParsing.h"
#include "LightingManager.h"
#include "ObjShader.h"

using namespace std;
using namespace ObjLibrary;
using namespace ObjLibrary::ObjShader;
namespace
{
	//
	//  ShaderUniformsLit
	//
	//  An extension to the ShaderUniforms record that also
	//    stores the locations of the uniforms used for lights.
	//

	struct ShaderUniformsLit : public ShaderUniforms
	{
		unsigned int m_light_position;
		unsigned int m_light_ambient;
		unsigned int m_light_diffuse;
		unsigned int m_light_specular;
		unsigned int m_light_attenuation;

		unsigned int m_a_is_light_enabled;
	};


	//
	//  ShaderUniformsLitShadow
	//
	//  An extension to the ShaderUniformsLit record that also
	//    stores the locations of the uniforms used for shadows
	//

	struct ShaderUniformsLitShadow : public ShaderUniformsLit
	{
		unsigned int m_shadow_map_space_matrix;
		unsigned int m_shadow_map;
		unsigned int m_shadow_distance;
	};


	const string SHADER_PATH_DEFAULT = "lib/ObjLibrary/";
	const string SHADER_NAME_BASE    = "ObjShader";

	bool g_is_loaded = false;

	unsigned int   g_shader_program_unlit;
	ShaderUniforms g_shader_uniforms_unlit;

	unsigned int   g_shader_program_no_lights;
	ShaderUniforms g_shader_uniforms_no_lights;

	unsigned int      g_shader_program_1_light;
	ShaderUniformsLit g_shader_uniforms_1_light;

	unsigned int      g_shader_program_many_lights;
	ShaderUniformsLit g_shader_uniforms_many_lights;

	unsigned int      g_shader_program_1_light_1_shadow;
	ShaderUniformsLitShadow g_shader_uniforms_1_light_1_shadow;

	unsigned int      g_shader_program_many_lights_1_shadow;
	ShaderUniformsLitShadow g_shader_uniforms_many_lights_1_shadow;

	const bool DEBUGGING_LOAD_SHADERS     = false;
	const bool DEBUGGING_SHOW_SHADER_TEXT = false;



	//
	//  initShaderProgram
	//
	//  Purpose: To load and initialize one of the standard
	//           ObjLibrary shader programs from the specified
	//           vertex and fragment shaders.
	//  Parameter(s):
	//    <1> vertex_filename: The filename (including path) of
	//                         the vertex shader
	//    <2> fragment_filename: The filename (including path)
	//                           of the fragment shader
	//  Precondition(s):
	//    <1> vertex_filename != ""
	//    <2> fragment_filename != ""
	//  Returns: If the shaders are loaded, compiled, and linked
	//           correctly, a handle to the combined shader
	//           program is returned.  Otherwise, 0 is returned.
	//  Side Effect: The shaders in files vertex_filename and
	//               fragment_filename are loaded, compiled, and
	//               linked into a single shader program.
	//               Vertex attributes are bound.  If anything
	//               goes wrong, an error message will be
	//               printed.
	//

	unsigned int initShaderProgram (const string& vertex_filename,
	                                const string& fragment_filename)
	{
		assert(vertex_filename != "");
		assert(fragment_filename != "");

		unsigned int shader_program_id;

		if(DEBUGGING_LOAD_SHADERS)
			cout << "\t\tEntered initShaderProgram" << endl;

		shader_program_id = initProgramStart(vertex_filename, fragment_filename);
		if(shader_program_id == 0)
			return 0;

		if(DEBUGGING_LOAD_SHADERS)
			cout << "\t\t\tAbout to bind attributes" << endl;

		// ...(program, input slot index, shader input parameter name)
		glBindAttribLocation(shader_program_id, 0, "vertex");
		glBindAttribLocation(shader_program_id, 1, "tex_coord");
		glBindAttribLocation(shader_program_id, 2, "normal");
		glBindAttribLocation(shader_program_id, 3, "vertex1");
		glBindAttribLocation(shader_program_id, 4, "normal1");

		bool is_good = initProgramEnd(shader_program_id);
		if(!is_good)
			return 0;

		if(DEBUGGING_LOAD_SHADERS)
			cout << "\t\t\tFinished binding attributes" << endl;

		if(DEBUGGING_LOAD_SHADERS)
			cout << "\t\tFinished initShaderProgram successfully" << endl;

		return shader_program_id;
	}

	//
	//  setShaderUnifromsMaterial
	//
	//  Purpose: To set the locations of the non-light-related
	//           shader uniforms for one of the standard
	//           ObjLibrary shaders.
	//  Parameter(s):
	//    <1> shader_program_id: The handle for the shader
	//                           program
	//    <2> r_uniforms: A reference to the ShaderUniforms to
	//                    fill with the uniform locations
	//  Precondition(s):
	//    <1> shader_program_id != 0
	//  Returns: N/A
	//  Side Effect: The values in r_uniforms are filled with
	//               the locations of the uniforms in shader
	//               program shader_program_id.  Shader
	//               uniforms for lights are not set.
	//

	void setShaderUnifromsMaterial (unsigned int shader_program_id,
	                                ShaderUniforms& r_uniforms)
	{
		assert(shader_program_id != 0);

		r_uniforms.m_tween_enabled					= glGetUniformLocation(shader_program_id, "tween_enabled");
		r_uniforms.m_tween_factor					= glGetUniformLocation(shader_program_id, "tween_factor");

		r_uniforms.m_model_matrix				    = glGetUniformLocation(shader_program_id, "model_matrix");
		r_uniforms.m_view_matrix					= glGetUniformLocation(shader_program_id, "view_matrix");
		r_uniforms.m_model_view_projection_matrix   = glGetUniformLocation(shader_program_id, "model_view_projection_matrix");

		r_uniforms.m_camera_pos						= glGetUniformLocation(shader_program_id, "camera_pos");

		r_uniforms.m_transparency = glGetUniformLocation(shader_program_id, "transparency");
		r_uniforms.m_emission     = glGetUniformLocation(shader_program_id, "emission_colour");
		r_uniforms.m_ambient      = glGetUniformLocation(shader_program_id,  "ambient_colour");
		r_uniforms.m_diffuse      = glGetUniformLocation(shader_program_id,  "diffuse_colour");
		r_uniforms.m_specular     = glGetUniformLocation(shader_program_id, "specular_colour");
		r_uniforms.m_shininess    = glGetUniformLocation(shader_program_id, "shininess");

		r_uniforms.m_is_seperate_specular = glGetUniformLocation(shader_program_id, "is_seperate_specular");

		r_uniforms.m_transparency_texture = glGetUniformLocation(shader_program_id, "transparency_texture");
		r_uniforms.m_emission_texture     = glGetUniformLocation(shader_program_id,     "emission_texture");
		r_uniforms.m_ambient_texture      = glGetUniformLocation(shader_program_id,      "ambient_texture");
		r_uniforms.m_diffuse_texture      = glGetUniformLocation(shader_program_id,      "diffuse_texture");
		r_uniforms.m_specular_texture     = glGetUniformLocation(shader_program_id,     "specular_texture");
		r_uniforms.m_shininess_texture    = glGetUniformLocation(shader_program_id,    "shininess_texture"); 

		r_uniforms.m_transparency_channel = glGetUniformLocation(shader_program_id, "transparency_channel");
		r_uniforms.m_shininess_channel    = glGetUniformLocation(shader_program_id,    "shininess_channel");

		r_uniforms.m_a_is_texture_active  = glGetUniformLocation(shader_program_id, "a_is_texture_active");
	}

	//
	//  setShaderUniformsLighting
	//
	//  Purpose: To set the locations of the light-related
	//           shader uniforms for one of the standard
	//           ObjLibrary shaders.
	//  Parameter(s):
	//    <1> shader_program_id: The handle for the shader
	//                           program
	//    <2> r_uniforms: A reference to the ShaderUniformsLit
	//                    to fill with the uniform locations
	//  Precondition(s):
	//    <1> shader_program_id != 0
	//  Returns: N/A
	//  Side Effect: The light-related values in r_uniforms are
	//               filled with the locations of the uniforms
	//               in shader program shader_program_id.  The
	//               other uniform locations are not set.
	//

	void setShaderUniformsLighting (unsigned int shader_program_id,
	                                ShaderUniformsLit& r_uniforms)
	{
		r_uniforms.m_light_position    = glGetUniformLocation(shader_program_id, "light_position");
		r_uniforms.m_light_ambient     = glGetUniformLocation(shader_program_id, "light_ambient");
		r_uniforms.m_light_diffuse     = glGetUniformLocation(shader_program_id, "light_diffuse");
		r_uniforms.m_light_specular    = glGetUniformLocation(shader_program_id, "light_specular");
		r_uniforms.m_light_attenuation = glGetUniformLocation(shader_program_id, "light_attenuation");

		r_uniforms.m_a_is_light_enabled = glGetUniformLocation(shader_program_id, "a_is_light_enabled");
	}

	void setShaderUniformsShadow(unsigned int shader_program_id,
		ShaderUniformsLitShadow& r_uniforms)
	{
		r_uniforms.m_shadow_map_space_matrix	= glGetUniformLocation(shader_program_id, "shadow_map_space_matrix");
		r_uniforms.m_shadow_map					= glGetUniformLocation(shader_program_id, "shadow_map");
		r_uniforms.m_shadow_distance = glGetUniformLocation(shader_program_id, "shadow_distance");
	}
}



bool ObjShader :: isLoaded ()
{
	return g_is_loaded;
}

const ShaderUniforms& ObjShader :: activateShader ()
{
	// chose which lights to use

	if(LightingManager::isEnabled())
	{
		unsigned int light_to_use = LightingManager::getSingleLightEnabled();

		// chose which shader programs to use

		if(light_to_use == LightingManager::NO_LIGHTS_ENABLED)
		{
			// no active lights
			glUseProgram(g_shader_program_no_lights);
			return g_shader_uniforms_no_lights;
		}
		else if(light_to_use != LightingManager::MANY_LIGHTS_ENABLED)
		{
	
			if(LightingManager::isShadowEnabled())
			{
				// 1 light source, with shadows

				glUseProgram(g_shader_program_1_light_1_shadow);

				const glm::vec4& position = LightingManager::getLightPosition(light_to_use);
				const glm::vec3& ambient = LightingManager::getLightAmbient(light_to_use);
				const glm::vec3& diffuse = LightingManager::getLightDiffuse(light_to_use);
				const glm::vec3& specular = LightingManager::getLightSpecular(light_to_use);
				const glm::vec3& attenuation = LightingManager::getLightAttenuation(light_to_use);

				glUniform4fv(g_shader_uniforms_1_light_1_shadow.m_light_position, 1, &(position.x));
				glUniform3fv(g_shader_uniforms_1_light_1_shadow.m_light_ambient, 1, &(ambient.x));
				glUniform3fv(g_shader_uniforms_1_light_1_shadow.m_light_diffuse, 1, &(diffuse.x));
				glUniform3fv(g_shader_uniforms_1_light_1_shadow.m_light_specular, 1, &(specular.x));
				glUniform3fv(g_shader_uniforms_1_light_1_shadow.m_light_attenuation, 1, &(attenuation.x));

				//Shadow stuff
				const float shadow_distance = LightingManager::getShadowDistance();
				glUniform1f(g_shader_uniforms_1_light_1_shadow.m_shadow_distance, shadow_distance);
				const unsigned int shadow_map = LightingManager::getShadowMap();
				glUniform1i(g_shader_uniforms_1_light_1_shadow.m_shadow_map, 6);
				//Texture 6 is for shadow_map
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, shadow_map);

				glm::mat4x4 shadow_map_space_matrix = LightingManager::getShadowMapSpaceMatrix();
				glUniformMatrix4fv(g_shader_uniforms_1_light_1_shadow.m_shadow_map_space_matrix, 1, false, &(shadow_map_space_matrix[0][0]));

				return g_shader_uniforms_1_light_1_shadow;
			}else
			{	
				//1 light source, no shadows
				glUseProgram(g_shader_program_1_light);

				const glm::vec4& position = LightingManager::getLightPosition(light_to_use);
				const glm::vec3& ambient = LightingManager::getLightAmbient(light_to_use);
				const glm::vec3& diffuse = LightingManager::getLightDiffuse(light_to_use);
				const glm::vec3& specular = LightingManager::getLightSpecular(light_to_use);
				const glm::vec3& attenuation = LightingManager::getLightAttenuation(light_to_use);

				glUniform4fv(g_shader_uniforms_1_light.m_light_position, 1, &(position.x));
				glUniform3fv(g_shader_uniforms_1_light.m_light_ambient, 1, &(ambient.x));
				glUniform3fv(g_shader_uniforms_1_light.m_light_diffuse, 1, &(diffuse.x));
				glUniform3fv(g_shader_uniforms_1_light.m_light_specular, 1, &(specular.x));
				glUniform3fv(g_shader_uniforms_1_light.m_light_attenuation, 1, &(attenuation.x));

				return g_shader_uniforms_1_light;
			}


		}
		else
		{
			// multiple light sources
			assert(light_to_use == LightingManager::MANY_LIGHTS_ENABLED);

			if(LightingManager::isShadowEnabled())
			{
				glUseProgram(g_shader_program_many_lights_1_shadow);

				glUniform4fv(g_shader_uniforms_many_lights_1_shadow.m_light_position,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightPositionAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights_1_shadow.m_light_ambient,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightAmbientAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights_1_shadow.m_light_diffuse,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightDiffuseAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights_1_shadow.m_light_specular,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightSpecularAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights_1_shadow.m_light_attenuation,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightAttenuationAll()[0].x));
				glUniform1iv(g_shader_uniforms_many_lights_1_shadow.m_a_is_light_enabled,
					LightingManager::LIGHT_COUNT_MAX,
					LightingManager::isLightEnabledAll());

				//Shadow stuff
				const float shadow_distance = LightingManager::getShadowDistance();
				glUniform1f(g_shader_uniforms_many_lights_1_shadow.m_shadow_distance, shadow_distance);
				const unsigned int shadow_map = LightingManager::getShadowMap();
				glUniform1i(g_shader_uniforms_many_lights_1_shadow.m_shadow_map, 6);
				//Texture 6 is for shadow_map
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, shadow_map);

				glm::mat4x4 shadow_map_space_matrix = LightingManager::getShadowMapSpaceMatrix();
				glUniformMatrix4fv(g_shader_uniforms_many_lights_1_shadow.m_shadow_map_space_matrix, 1, false, &(shadow_map_space_matrix[0][0]));
				
				return g_shader_uniforms_many_lights_1_shadow;
			}else
			{
				glUseProgram(g_shader_program_many_lights);

				glUniform4fv(g_shader_uniforms_many_lights.m_light_position,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightPositionAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights.m_light_ambient,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightAmbientAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights.m_light_diffuse,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightDiffuseAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights.m_light_specular,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightSpecularAll()[0].x));
				glUniform3fv(g_shader_uniforms_many_lights.m_light_attenuation,
					LightingManager::LIGHT_COUNT_MAX,
					&(LightingManager::getLightAttenuationAll()[0].x));
				glUniform1iv(g_shader_uniforms_many_lights.m_a_is_light_enabled,
					LightingManager::LIGHT_COUNT_MAX,
					LightingManager::isLightEnabledAll());

				return g_shader_uniforms_many_lights;
			}


		}
	}
	else
	{
		// not using lighting
		glUseProgram(g_shader_program_unlit);
		return g_shader_uniforms_unlit;
	}
}



bool ObjShader :: load ()
{
	assert(!isLoaded());

	//  DOES THIS WORK?  <|>
	return load(SHADER_PATH_DEFAULT);
}

bool ObjShader :: load (const std::string& file_path)
{
	assert(!isLoaded());
	assert(ObjStringParsing::isValidPath(file_path));

	if(DEBUGGING_LOAD_SHADERS)
	{
		cout << "Entered ObjShader::load" << endl;
		cout << "\tAbout to test if shaders version " << SHADER_MAJOR_VERSION
		     << "." << SHADER_MINOR_VERSION << " are supported" << endl;
	}

	bool is_supported = (bool)(gl3wIsSupported(SHADER_MAJOR_VERSION, SHADER_MINOR_VERSION) != 0);
	if(!is_supported)
	{
		cerr << "gl3w " << SHADER_MAJOR_VERSION << "." << SHADER_MINOR_VERSION << " is not supported" << endl;
		return false;
	}

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\tYes" << endl;

	string shader_name_path_base   = file_path + SHADER_NAME_BASE;
	string shader_name_path_vertex = shader_name_path_base + ".vert";

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\tAbout to load unlit shader" << endl;

	g_shader_program_unlit = initShaderProgram(shader_name_path_vertex,
	                                           shader_name_path_base + "Unlit.frag");
	if(g_shader_program_unlit == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_unlit, g_shader_uniforms_unlit);

	if(DEBUGGING_LOAD_SHADERS)
	{
		cout << "\t\tFinished loading unlit shader successfully" << endl;
		cout << "\tAbout to load no lights shader" << endl;
	}

	g_shader_program_no_lights = initShaderProgram(shader_name_path_vertex,
	                                               shader_name_path_base + "NoLights.frag");
	if(g_shader_program_no_lights == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_no_lights, g_shader_uniforms_no_lights);

	if(DEBUGGING_LOAD_SHADERS)
	{
		cout << "\t\tFinished loading no lights shader successfully" << endl;
		cout << "\tAbout to load 1 light shader" << endl;
	}

	g_shader_program_1_light = initShaderProgram(shader_name_path_vertex,
	                                             shader_name_path_base + "1Light.frag");
	if(g_shader_program_1_light == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_1_light, g_shader_uniforms_1_light);
	setShaderUniformsLighting(g_shader_program_1_light, g_shader_uniforms_1_light);

	if(DEBUGGING_LOAD_SHADERS)
	{
		cout << "\t\tFinished loading 1 light shader successfully" << endl;
		cout << "\tAbout to load many lights shader" << endl;
	}

	g_shader_program_many_lights = initShaderProgram(shader_name_path_vertex,
	                                                 shader_name_path_base + "ManyLights.frag");
	if(g_shader_program_many_lights == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_many_lights, g_shader_uniforms_many_lights);
	setShaderUniformsLighting(g_shader_program_many_lights, g_shader_uniforms_many_lights);

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\tFinished loading many lights shader successfully" << endl;

	g_shader_program_1_light_1_shadow = initShaderProgram(shader_name_path_base + "Shadow.vert",
		shader_name_path_base + "1Light1Shadow.frag");
	if (g_shader_program_1_light_1_shadow == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_1_light_1_shadow, g_shader_uniforms_1_light_1_shadow);
	setShaderUniformsLighting(g_shader_program_1_light_1_shadow, g_shader_uniforms_1_light_1_shadow);
	setShaderUniformsShadow(g_shader_program_1_light_1_shadow, g_shader_uniforms_1_light_1_shadow);

	if (DEBUGGING_LOAD_SHADERS)
		cout << "\t\tFinished loading 1 light 1 shadow shader successfully" << endl;

	g_shader_program_many_lights_1_shadow = initShaderProgram(shader_name_path_base + "Shadow.vert",
		shader_name_path_base + "ManyLights1Shadow.frag");
	if (g_shader_program_many_lights_1_shadow == 0)
		return false;
	setShaderUnifromsMaterial(g_shader_program_many_lights_1_shadow, g_shader_uniforms_many_lights_1_shadow);
	setShaderUniformsLighting(g_shader_program_many_lights_1_shadow, g_shader_uniforms_many_lights_1_shadow);
	setShaderUniformsShadow(g_shader_program_many_lights_1_shadow, g_shader_uniforms_many_lights_1_shadow);

	if (DEBUGGING_LOAD_SHADERS)
		cout << "\t\tFinished loading many lights 1 shadow shader successfully" << endl;

	g_is_loaded = true;

	if(DEBUGGING_LOAD_SHADERS)
		cout << "Finished ObjShader::load successfully" << endl;

	assert(isLoaded());
	return true;
}



string ObjShader :: calculateVersionDirective (unsigned int major,
                                               unsigned int minor)
{
	stringstream version_stream;
	version_stream << "#version ";

	version_stream << major << minor;
	if(minor < 10)
		version_stream << "0";

	version_stream << "\n";
	return version_stream.str();
}

// From OpenGL Shading Language 3rd Edition, p215-216
void ObjShader :: printShaderInfoLog (int shader)
{
	assert(shader != 0);

	int infoLogLen = 0;
 
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
 
	if(infoLogLen > 0)
	{
		int charsWritten = 0;
		GLchar* infoLog = new GLchar[infoLogLen];
		glGetShaderInfoLog(shader, infoLogLen, &charsWritten, infoLog);
		cerr << "Shader infoLog:" << endl;
		cerr << infoLog << endl;
		delete[] infoLog;
	}
}

// adapted from function above
void ObjShader :: printProgramInfoLog (int shader_program)
{
	assert(shader_program != 0);

	int infoLogLen = 0;
 
	glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &infoLogLen);
 
	if(infoLogLen > 0)
	{
		int charsWritten = 0;
		GLchar* infoLog = new GLchar[infoLogLen];
		glGetProgramInfoLog(shader_program, infoLogLen, &charsWritten, infoLog);
		cerr << "Shader program infoLog:" << endl;
		cerr << infoLog << endl;
		delete[] infoLog;
	}
}

string ObjShader :: readShader (const string& filename)
{
	assert(filename != "");

	ifstream infile(filename.c_str());

	//
	//  A magic line to load the whole file in one command
	//    using iterators.  I don't know a less-confusing way to
	//    do it.
	//
	//  Thank you, Stamatis Katsaganis!
	//

	return string(istreambuf_iterator<char>(infile), istreambuf_iterator<char>());
}

unsigned int ObjShader :: compileShader (unsigned int shader_type,
                                         const string& shader_filename,
                                         const string& version_directive,
                                         const string& shader_text)
{
	assert(shader_type == GL_VERTEX_SHADER ||
	       shader_type == GL_FRAGMENT_SHADER ||
	       shader_type == GL_GEOMETRY_SHADER ||
	       shader_type == GL_TESS_CONTROL_SHADER ||
	       shader_type == GL_TESS_EVALUATION_SHADER ||
	       shader_type == GL_COMPUTE_SHADER);
	assert(shader_filename != "");
	assert(version_directive != "");
	assert(shader_text != "");

	const unsigned int SHADER_FILE_COUNT = 2;

	const char* aa_shader_text[SHADER_FILE_COUNT] =
	{
		version_directive.c_str(),
		shader_text.c_str(),
	};
	int a_shader_length[SHADER_FILE_COUNT] =
	{	// we need to explictly typecast to signed int for newer C++
		(int)(version_directive.size()),
		(int)(shader_text.size()),
	};

	GLuint shader_handle = glCreateShader(shader_type);
	if(shader_handle == 0)
	{
		cerr << "Error: Shader could not be created" << endl;
		return 0;
	}

	glShaderSource(shader_handle, SHADER_FILE_COUNT, aa_shader_text, a_shader_length);

	GLint compiled;
	glCompileShader(shader_handle);
	glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compiled);

	if(compiled == FALSE)
	{
		cerr << "Error: Shader \"" << shader_filename << "\" could not be compiled" << endl;
		printShaderInfoLog(shader_handle);
		return 0;
	}

	return shader_handle;
}

unsigned int ObjShader :: loadShader (unsigned int shader_type,
                                      const std::string& filename,
                                      const std::string& version_directive)
{
	assert(shader_type == GL_VERTEX_SHADER ||
	       shader_type == GL_FRAGMENT_SHADER ||
	       shader_type == GL_GEOMETRY_SHADER ||
	       shader_type == GL_TESS_CONTROL_SHADER ||
	       shader_type == GL_TESS_EVALUATION_SHADER ||
	       shader_type == GL_COMPUTE_SHADER);
	assert(filename != "");
	assert(version_directive != "");

	string shader_type_string;
	if(DEBUGGING_LOAD_SHADERS)
	{
		     if(shader_type == GL_VERTEX_SHADER)          shader_type_string = "vertex";
		else if(shader_type == GL_FRAGMENT_SHADER)        shader_type_string = "fragment";
		else if(shader_type == GL_GEOMETRY_SHADER)        shader_type_string = "geometry";
		else if(shader_type == GL_TESS_CONTROL_SHADER)    shader_type_string = "tesselation control";
		else if(shader_type == GL_TESS_EVALUATION_SHADER) shader_type_string = "tesselation evaluation";
		else if(shader_type == GL_COMPUTE_SHADER)         shader_type_string = "compute";

		cout << "\t\t\t\tAbout to compile " << shader_type_string << " shader" << endl;
	}

	string shader_text = readShader(filename);

	if(DEBUGGING_SHOW_SHADER_TEXT)
	{
		cout << "\t\t\t\t" << shader_type_string << " shader text" << endl;
		cout << endl;
		cout << "--------------------------------" << endl;
		cout << shader_text;
		cout << "--------------------------------" << endl;
		cout << endl;
	}

	if(shader_text.empty())
	{
		cerr << "Error: File \"" << filename << "\" (" << shader_type_string
		     << " shader) could not be loaded correctly" << endl;
		return 0;
	}

	GLuint shader = compileShader(shader_type, filename, version_directive, shader_text);

	if(shader == 0)
	{
		cerr << "Error: File \"" << filename << "\" (" << shader_type_string
		     << " shader) could not be compiled correctly" << endl;
		return 0;
	}

	assert(shader != 0);
	return shader;
}

// adapted from https://www.opengl.org/wiki/Tutorial1:_Rendering_shapes_with_glDrawRangeElements,_VAO,_VBO,_shaders_(C%2B%2B_/_freeGLUT)
unsigned int ObjShader :: initProgramStart (const string& vertex_filename,
                                            const string& fragment_filename)
{
	assert(vertex_filename != "");
	assert(fragment_filename != "");

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\tEntered ObjShader::initProgramStart (vertex, fragment)" << endl;

	string version_directive = calculateVersionDirective(SHADER_MAJOR_VERSION,
	                                                     SHADER_MINOR_VERSION);

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tCalculated version directive:" << endl;
	if(DEBUGGING_SHOW_SHADER_TEXT)
		cout << version_directive << endl;

	GLuint   vertex_shader = loadShader(GL_VERTEX_SHADER,     vertex_filename, version_directive);
	GLuint fragment_shader = loadShader(GL_FRAGMENT_SHADER, fragment_filename, version_directive);

	if(vertex_shader == 0)
		return 0;
	if(fragment_shader == 0)
		return 0;

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tAbout to create shader program" << endl;

	unsigned int shader_program = glCreateProgram();
	if(shader_program == 0)
	{
		cerr << "Error: Shader program could not be created" << endl;
		return 0;
	}

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tAbout to attach shaders to program" << endl;

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, fragment_shader);

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\tFinished ObjShader::initProgramStart successfully" << endl;

	return shader_program;
}

unsigned int ObjShader :: initProgramStart (const string& vertex_filename,
                                            const string& geometry_filename,
                                            const string& fragment_filename)
{
	assert(vertex_filename   != "");
	assert(geometry_filename != "");
	assert(fragment_filename != "");

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\tEntered ObjShader::initProgramStart (vertex, geometry, fragment)" << endl;

	string version_directive = calculateVersionDirective(SHADER_MAJOR_VERSION,
	                                                     SHADER_MINOR_VERSION);

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tCalculated version directive:" << endl;
	if(DEBUGGING_SHOW_SHADER_TEXT)
		cout << version_directive << endl;

	GLuint   vertex_shader = loadShader(GL_VERTEX_SHADER,     vertex_filename, version_directive);
	GLuint geometry_shader = loadShader(GL_GEOMETRY_SHADER, geometry_filename, version_directive);
	GLuint fragment_shader = loadShader(GL_FRAGMENT_SHADER, fragment_filename, version_directive);

	if(vertex_shader == 0)
		return 0;
	if(geometry_shader == 0)
		return 0;
	if(fragment_shader == 0)
		return 0;

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tAbout to create shader program" << endl;

	unsigned int shader_program = glCreateProgram();
	if(shader_program == 0)
	{
		cerr << "Error: Shader program could not be created" << endl;
		return 0;
	}

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\t\tAbout to attach shaders to program" << endl;

	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, geometry_shader);
	glAttachShader(shader_program, fragment_shader);

	if(DEBUGGING_LOAD_SHADERS)
		cout << "\t\t\tFinished ObjShader::initProgramStart successfully" << endl;

	return shader_program;
}

bool ObjShader :: initProgramEnd (unsigned int shader_program)
{
	assert(shader_program != 0);

	glLinkProgram(shader_program);

	GLint IsLinked;
	glGetProgramiv(shader_program, GL_LINK_STATUS, (GLint *)&IsLinked);
	if(IsLinked == FALSE)
	{
		cout << "Failed to link shaders." << endl;
 		printProgramInfoLog(shader_program);
		return false;
	}

	return true;
}

