//
//  ObjShaderNoLights.frag
//
//  One of the standard fragment shaders used by the ObjLibrary.
//    This fragment shader is used when lighting is enabled, but
//    no lights are active.
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

uniform float transparency;
uniform vec3  emission_colour;

uniform sampler2D transparency_texture;
uniform sampler2D     emission_texture;

uniform int       transparency_channel;

uniform bool[6]   a_is_texture_active;

in vec3 position;
in vec2 tex_coord2;
in vec3 normal2;
in vec3 to_camera;

layout(location = 0) out vec4 FragColor;

void main()
{
	float transparency_on_texture = texture(transparency_texture, tex_coord2)[transparency_channel];
	vec3      emission_on_texture = texture(    emission_texture, tex_coord2).rgb;

	float transparency2    = transparency;
	vec3  emission_surface = emission_colour;

	if(a_is_texture_active[0]) transparency2    *= transparency_on_texture;
	if(a_is_texture_active[1]) emission_surface *=     emission_on_texture;

	if(transparency2 <= 0.0)
		discard;

	FragColor = vec4(emission_surface, transparency2);
}
