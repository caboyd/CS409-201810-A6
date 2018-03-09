//
//  ObjShader.vert
//
//  The standard vertex shader used by the ObjLibrary.  This
//    same vertex shader is always used to display the models,
//    regardless of the fragment shader used.
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

//
//	Modified by Chris Boyd
//	Updated to match uniforms required for the shadow shader
//
//

uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 model_view_projection_matrix;
uniform vec3 camera_pos;

uniform bool tween_enabled;
uniform float tween_factor;

in vec3 vertex;
in vec2 tex_coord;
in vec3 normal;
in vec3 vertex1;
in vec3 normal1;

out vec3 position;
out vec2 tex_coord2;
out vec3 normal2;
out vec3 to_camera;

void main()
{
    vec3 new_normal = normal;
    vec3 new_vertex = vertex;
    if(tween_enabled)
	{
		new_normal = mix(normal,normal1,tween_factor);      
        new_vertex = mix(vertex,vertex1,tween_factor);
    }
	
	vec4 world_position = model_matrix * vec4(new_vertex, 1.0);

	position   = world_position.xyz;
	tex_coord2 = tex_coord;
	normal2    = mat3(transpose(inverse(model_matrix))) * new_normal;
	to_camera  = normalize(camera_pos - world_position.xyz);

	gl_Position = model_view_projection_matrix * vec4(new_vertex, 1.0);
}
