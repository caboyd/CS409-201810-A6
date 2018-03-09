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

//
//	Modified by Chris Boyd
//	Updated to allow for shadows using shadow maps;
//
//
const int LIGHT_COUNT = 8;
uniform mat4 model_matrix;
uniform mat4 view_matrix;
uniform mat4 model_view_projection_matrix;
uniform mat4 shadow_map_space_matrix;
uniform vec3 camera_pos;
uniform float shadow_distance;
uniform vec4 light_position[LIGHT_COUNT];

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
out vec4 shadow_coord;
out vec3 light_direction_cameraspace;
out vec3 normal_cameraspace;

const float transition_distance = 5.0;

void main()
{
    vec3 new_normal = normal;
    vec3 new_vertex = vertex;
    if(tween_enabled)
	{
		new_normal = mix(normal,normal1,tween_factor);      
        new_vertex = mix(vertex,vertex1,tween_factor);
    }
	
    //(OUT) Output position of the vertex in clip space
	gl_Position = model_view_projection_matrix * vec4(new_vertex, 1.0);

    vec4 world_position = model_matrix * vec4(new_vertex, 1.0);

    //(OUT) Position of vertex in world sapce
	position  = world_position.xyz;

    //(OUT)
	tex_coord2 = tex_coord;

    //(OUT) Update the normal to be the inverse transpose of the model matrix.
    normal2    = mat3(transpose(inverse(model_matrix))) * new_normal;
    normal_cameraspace  = (view_matrix * vec4(normal2,0)).xyz;

    //(OUT)
	to_camera  = normalize(camera_pos - world_position.xyz);
    //to_camera = (inverse(view_matrix) * vec4(0.0,0.0,0.0,1.0)).xyz - world_position.xyz;
	light_direction_cameraspace = (view_matrix * vec4(light_position[0].xyz,0)).xyz;

    //(OUT) Calculate world space coords that map this vertex to the shadow_map
	//The vertex may not appear in the shadow_map and will have no shadow
	shadow_coord = shadow_map_space_matrix * world_position;

    //Shaodw_coord.w will be used to fade in and out shadows softly when they are far from camera
	vec4 to_camera_view_space = view_matrix * world_position;
    float distance = length(to_camera_view_space.xyz);

	distance = distance - (shadow_distance - transition_distance);
    distance = distance / transition_distance;
    shadow_coord.w = clamp(1.0 - distance, 0.0, 1.0);
}











