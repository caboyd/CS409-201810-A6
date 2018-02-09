//
//  ObjShader.h
//
//  A module to manage the standard shaers used by the
//    ObjLibrary.
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

#ifndef OBJ_LIBRARY_OBJ_SHADER_H
#define OBJ_LIBRARY_OBJ_SHADER_H

#include <string>

#include "ObjSettings.h"
#ifndef OBJ_LIBRARY_GLM_INTERACTION
	#error "ObjShader can only be used with OBJ_LIBRARY_GLM_INTERACTION #defined"
#endif
#ifndef OBJ_LIBRARY_SHADER_DISPLAY
	#error "ObjShader can only be used with OBJ_LIBRARY_SHADER_DISPLAY #defined"
#endif



namespace ObjLibrary
{

//
//  ObjShader
//
//  A namesapce to manage the standard shaders used by the
//    ObjLibrary.
//
//  To use the shaders, your main function should look somthing
//    like this:
//
//    int main (int argc, char** argv)
//    {
//        ...
//        glutInit(&argc, argv);
//        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
//        glutCreateWindow("Window Title Here");
//  ->    gl3wInit();
//        ...
//  ->    ObjShaders::load();
//        ...
//        glutMainLoop();
//        return 1;
//    }
//
//  Some of the functions in this namespace may also be useful
//    for loading other shaders.  To use the initProgramStart
//    and initProgramEnd functions to load and initialize a
//    shader program with vertex and fragment shaders, use code
//    similar to the following:
//
//      unsigned int shader_program_id;
//      shader_program_id = ObjShader::initProgramStart(
//                                           vertex_filename,
//                                           fragment_filename);
//      glBindAttribLocation(shader_program_id, 0, "varying0");
//      glBindAttribLocation(shader_program_id, 1, "varying1");
//      ...
//      ObjShader::initProgramEnd(shader_program_id);
//      // shader_program_id is ready to use
//

namespace ObjShader
{
struct ShaderUniforms;  // forwrd declaration



//
//  SHADER_MAJOR_VERSION
//
//  The major version number for the shaders to be loaded.  If
//    the shaders are version 4.0, the major shader version
//    number is 4.
//
//  This may be different from the major version number for
//    OpenGL.
//

const unsigned int SHADER_MAJOR_VERSION = 3;

//
//  SHADER_MINOR_VERSION
//
//  The minor version number for the shaders to be loaded.  If
//    the shaders are version 4.0, the minor shader version
//    number is 0.
//
//  This may be different from the minor version number for
//    OpenGL.
//

	const unsigned int SHADER_MINOR_VERSION = 3;



//
//  isLoaded
//
//  Purpose: To determine if the ObjLibrary shaders have been
//           loaded.
//  Parameter(s): N/A
//  Precondition(s): N/A
//  Returns: Whether ther ObjLibrary shaders have been loaded.
//  Side Effect: N/A
//

bool isLoaded ();

//
//  activateShader
//
//  Purpose: To actvate the appropriate ObjLibrary shaders for
//           the current lights.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> isLoaded()
//  Returns: Whether the shaders were loaded successfully.
//  Side Effect: If OpenGL lighting is disabled or there are no
//               OpenGL lights active, the flat lighting shaders
//               are activated.  If there exactly one OpenGL
//               light active, the shaders for 1 light are
//               activated.  Otherwise, the shaders for many
//               lights are activated.
//

const ShaderUniforms& activateShader ();

//
//  load
//
//  Purpose: To load the ObjLibrary shaders.
//  Parameter(s): N/A
//  Precondition(s):
//    <1> !isLoaded()
//  Returns: Whether the shaders were loaded successfully.
//  Side Effect: The ObjLibrary shaders are loaded if possible.
//               If this function returns false, the shaders are
//               left in an undefined state and an error message
//               may be printed to stderr.
//

bool load ();

//
//  load
//
//  Purpose: To load the ObjLibrary shaders from a differnt file
//           location.  You will need this function if you put
//           the shader files in a different directory.  This
//           includes if you put the whole ObjLibrary in a
//           different directory.
//  Parameter(s):
//    <1> file_path: The path to where the shader files are
//  Precondition(s):
//    <1> !isLoaded()
//    <2> ObjStringParsing::isValidPath(file_path)
//  Returns: Whether the shaders were loaded successfully.
//  Side Effect: The ObjLibrary shaders are loaded with path
//               file_path if possible.  If this function
//               returns false, the shaders are left in an
//               undefined state and an error message will be
//               printed to stderr.
//

bool load (const std::string& file_path);



//
//  Helper Function: calculateVersionDirective
//
//  Purpose: To calculate the shader version directive for the
//           specified shader version.
//  Parameter(s):
//    <1> major: The major shader version
//    <2> minor: The minor shader version
//  Precondition(s):
//    <1> minor < 100
//  Returns: The shader directive for shader version major,
//           minor.  This is a newline-terminated string to be
//           prepended to the shader text.
//  Side Effect: N/A
//

std::string calculateVersionDirective (unsigned int major,
                                       unsigned int minor);

//
//  Helper Function: printShaderInfoLog
//
//  Purpose: To print (hopefully) useful error messages if
//           the specified shader fails to compile.
//  Parameter(s):
//    <1> shader: The shader id
//  Precondition(s):
//    <1> shader != 0
//  Returns: N/A
//  Side Effect: If there is an info log associated with shader
//               shader, it is printed to stderr.  Otherwise,
//               there is no effect.
//  Note: This function was adapted from OpenGL Shading Language
//        3rd Edition, p215-216.
//

void printShaderInfoLog (int shader);

//
//  Helper Function: printProgramInfoLog
//
//  Purpose: To print (hopefully) useful error messages if
//           the shaders in the specified shader program fail to
//           link.
//  Parameter(s):
//    <1> shader_program: The shader program id
//  Precondition(s):
//    <1> shader_program != 0
//  Returns: N/A
//  Side Effect: If there is an info log associated with shader
//               program shader_program, it is printed to
//               stderr.  Otherwise, there is no effect.
//  Note: This function was adapted from OpenGL Shading Language
//        3rd Edition, p215-216.
//

void printProgramInfoLog (int shader);

//
//  Helper Function: readShader
//
//  Purpose: To read the specified shader form a file and
//           convert it to a string.
//  Parameter(s):
//    <1> filename: The name of the shader file to read,
//                  including its path
//  Precondition(s):
//    <1> filename != ""
//  Returns: The contents of file filename.
//  Side Effect: N/A
//

std::string readShader (const std::string& filename);

//
//  Helper Function: compileShader
//
//  Purpose: To compile the specified shader of the specified
//           version and type.
//  Parameter(s):
//    <1> shader_type: The type of shader
//    <2> shader_filename: The name of the shader file
//    <3> version_directive: The version directive for the
//                           shader
//    <4> shader_text: The text of the shader file
//  Precondition(s):
//    <1> shader_type == GL_VERTEX_SHADER ||
//        shader_type == GL_FRAGMENT_SHADER ||
//        shader_type == GL_GEOMETRY_SHADER ||
//        shader_type == GL_TESS_CONTROL_SHADER ||
//        shader_type == GL_TESS_EVALUATION_SHADER ||
//        shader_type == GL_COMPUTE_SHADER
//    <2> shader_filename != ""
//    <3> version_directive != ""
//    <4> shader_text != ""
//  Returns: If the shader compiles correctly a handle to the
//           compiled shader is returned.  Otherwise, 0 is
//           returned.
//  Side Effect: Shader shader_text is compiled as a shader of
//               type shader_type and the version specified in
//               version_directive.  If anything goes wrong, an
//               error message including the shader filename
//               will be printed.
//

unsigned int compileShader (
                           unsigned int shader_type,
                           const std::string& shader_filename,
                           const std::string& version_directive,
                           const std::string& shader_text);

//
//  Helper Function: loadShader
//
//  Purpose: To read a shader of the specified version and type
//           from the specified file and then compile it.
//          compile the specified shader of the specified
//           version and type.
//  Parameter(s):
//    <1> shader_type: The type of shader
//    <2> filename: The name of the shader file
//    <3> version_directive: The version directive for the
//                           shader
//  Precondition(s):
//    <1> shader_type == GL_VERTEX_SHADER ||
//        shader_type == GL_FRAGMENT_SHADER ||
//        shader_type == GL_GEOMETRY_SHADER ||
//        shader_type == GL_TESS_CONTROL_SHADER ||
//        shader_type == GL_TESS_EVALUATION_SHADER ||
//        shader_type == GL_COMPUTE_SHADER
//    <2> filename != ""
//    <3> version_directive != ""
//  Returns: If the shader is loaded and compiled correctly, a
//           handle to the compiled shader is returned.
//           Otherwise, 0 is returned.
//  Side Effect: The text of the file filename is loaded and
//               compiled as a shader of type shader_type and
//               the version specified in version_directive.  If
//               anything goes wrong, an error message will be
//               printed.
//

unsigned int loadShader (unsigned int shader_type,
                         const std::string& filename,
                         const std::string& version_directive);

//
//  Helper Function: initProgramStart
//
//  Purpose: To begin creation of a shader program including the
//           specified vertex and fragment shaders.
//  Parameter(s):
//    <1> vertex_filename: The filename (including path) of the
//                         vertex shader
//    <2> fragment_filename: The filename (including path) of
//                           the fragment shader
//  Precondition(s):
//    <1> vertex_filename != ""
//    <2> fragment_filename != ""
//  Returns: If the shaders are loaded and compiled correctly, a
//           handle to the combined shader program is returned.
//           Otherwise, 0 is returned.
//  Side Effect: The shaders in files vertex_filename and
//               fragment_filename are loaded, compiled, and
//               attached to a single shader program.  If
//               anything goes wrong, an error message will be
//               printed.
//  Note: This function was adapted from
//        https://www.opengl.org/wiki/Tutorial1:_Rendering_shapes_with_glDrawRangeElements,_VAO,_VBO,_shaders_(C%2B%2B_/_freeGLUT)
//

unsigned int initProgramStart (
                          const std::string& vertex_filename,
                          const std::string& fragment_filename);

//
//  Helper Function: initProgramStart
//
//  Purpose: To begin creation of a shader program including the
//           specified vertex, geometry, and fragment shaders.
//  Parameter(s):
//    <1> vertex_filename: The filename (including path) of the
//                         vertex shader
//    <2> geometry_filename: The filename (including path) of
//                           the geometry shader
//    <3> fragment_filename: The filename (including path) of
//                           the fragment shader
//  Precondition(s):
//    <1> vertex_filename != ""
//    <2> geometry_filename != ""
//    <3> fragment_filename != ""
//  Returns: If the shaders are loaded and compiled correctly, a
//           handle to the combined shader program is returned.
//           Otherwise, 0 is returned.
//  Side Effect: The shaders in files vertex_filename,
//               geometry_filename, and fragment_filename are
//               loaded, compiled, and attached to a single
//               shader program.  If anything goes wrong, an
//               error message will be printed.
//

unsigned int initProgramStart (
                          const std::string& vertex_filename,
                          const std::string& geometry_filename,
                          const std::string& fragment_filename);

//
//  Helper Function: initProgramEnd
//
//  Purpose: To end the creation of the specified shader
//           program.
//  Parameter(s):
//    <1> shader_program: The handle for the shader program
//  Precondition(s):
//    <1> shader_program != 0
//  Returns: If the shader program is linked correctly, true is
//           returned.  Otherwise, false is returned.
//  Side Effect: The shaders in shader program shader_program
//               are linked.  If anything goes wrong, an error
//               message will be printed.
//

bool initProgramEnd (unsigned int shader_program);



//
//  ShaderUniforms
//
//  A record storing the locations of the uniforms for one of
//    the standard ObjLibrary shader programs.  This record does
//    not include any of the lighting uniforms.
//

struct ShaderUniforms
{
	unsigned int m_model_matrix;
	unsigned int m_view_matrix;
	unsigned int m_model_view_projection_matrix;
	unsigned int m_camera_pos;

	unsigned int m_transparency;
	unsigned int m_emission;
	unsigned int m_ambient;
	unsigned int m_diffuse;
	unsigned int m_specular;
	unsigned int m_shininess;

	unsigned int m_is_seperate_specular;

	unsigned int m_transparency_texture;
	unsigned int m_emission_texture;
	unsigned int m_ambient_texture;
	unsigned int m_diffuse_texture;
	unsigned int m_specular_texture;
	unsigned int m_shininess_texture; 

	unsigned int m_transparency_channel;
	unsigned int m_shininess_channel;

	unsigned int m_a_is_texture_active;
};



}  // end of namespace ObjShader

}  // end of namespace ObjLibrary

#endif
