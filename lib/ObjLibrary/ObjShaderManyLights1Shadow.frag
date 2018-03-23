//
//  ObjShaderManyLights.frag
//
//  One of the standard fragment shaders used by the ObjLibrary.
//    This fragment shader is used when there are more than one
//    active lights.
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
//	Updated to allow for shadows using shadow maps;
//	light_position[0] is the light that will cause directional shadows
//

const int LIGHT_COUNT = 8;

uniform float transparency;
uniform vec3  emission_colour;
uniform vec3   ambient_colour;
uniform vec3   diffuse_colour;
uniform vec3  specular_colour;
uniform float shininess;
uniform mat4 view_matrix;

uniform bool is_seperate_specular;

uniform sampler2D transparency_texture;
uniform sampler2D     emission_texture;
uniform sampler2D      ambient_texture;
uniform sampler2D      diffuse_texture;
uniform sampler2D     specular_texture;
uniform sampler2D    shininess_texture;
uniform sampler2DShadow     shadow_map;

uniform int       transparency_channel;
uniform int          shininess_channel;

uniform bool[6]           a_is_texture_active;
uniform bool[LIGHT_COUNT] a_is_light_enabled;

uniform vec4 light_position[LIGHT_COUNT];
uniform vec3 light_ambient[LIGHT_COUNT];
uniform vec3 light_diffuse[LIGHT_COUNT];
uniform vec3 light_specular[LIGHT_COUNT];
uniform vec3 light_attenuation[LIGHT_COUNT];

in vec3 position;
in vec2 tex_coord2;
in vec3 normal2;
in vec3 to_camera;
in vec4 shadow_coord;
in vec3 light_direction_cameraspace;
in vec3 normal_cameraspace;

layout(location = 0) out vec4 FragColor;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Returns a random number based on a vec3 and an int.
float random(vec3 seed, int i){
	vec4 seed4 = vec4(seed,i);
	float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
	return fract(sin(dot_product) * 43758.5453);
}

void main()
{
	vec3 normalized_normal    = normalize(normal2);
	vec3 normalized_to_camera = normalize(to_camera);

	//Objects not in show have full light
	float light_factor = 1.0;

	//Computes a z-bias to remove z-fighting and shadow acne
	vec3 n = normalize( normal_cameraspace);
    vec3 l = normalize( light_direction_cameraspace ) ;
	//l = normalize(light_position[0].xyz);
	//n = normalized_normal;
	float cosTheta = clamp( dot( n,l ), 0,1 );
	float bias = 0.002*tan(acos(cosTheta));
	bias = clamp(bias, 0.0, 0.005);

	// Sample the shadow map up to 16 times
	// TODO - Should implement early bailing
	for (int i=0;i<16;i++){
		// use either :
		//  - Always the same samples.
		//    Gives a fixed pattern in the shadow, but no noise
		int index = i;
		//  - A random sample, based on the pixel's screen location. 
		//    No banding, but the shadow moves with the camera, which looks weird.
		//int index = int(16.0*random(gl_FragCoord.xyy, i))%16;
		//  - A random sample, based on the pixel's position in world space.
		//    The position is rounded to the millimeter to avoid too much aliasing
		//int index = int(16.0*random(floor(position.xyz*1000.0), i))%16;
		
		//A light fully in the shadow will have the light_factor reduced by 0.8
		//Multiple by shadow_coord.w to fade shadows in and out, and vertexes not in the shadow map will have full light.
		//Use poissonDisk to remove hard edges on shadow
		//subtract by bias to remove shadow acne and z-fighting		

		//NOTE: Use this for Sampler2DShadow textures
		light_factor -= (shadow_coord.w) * (1.0/16.0)*(1.0-texture( shadow_map, vec3(shadow_coord.xy + poissonDisk[index]/3000.0,  (shadow_coord.z-bias/shadow_coord.w)) ));

		//NOTE: Use this for Sampler2D textures
		//if ( texture( shadow_map, shadow_coord.xy + poissonDisk[index]/3000.0).z  <  (shadow_coord.z-bias/shadow_coord.w) ){
		//	light_factor-= (shadow_coord.w) * (1.0/16.0);
		//}
	}
	

	float transparency_on_texture = texture(transparency_texture, tex_coord2)[transparency_channel];
	vec3      emission_on_texture = texture(    emission_texture, tex_coord2).rgb;
	vec3       ambient_on_texture = texture(     ambient_texture, tex_coord2).rgb;
	vec3       diffuse_on_texture = texture(     diffuse_texture, tex_coord2).rgb;
	vec3      specular_on_texture = texture(    specular_texture, tex_coord2).rgb;
	float    shininess_on_texture = texture(   shininess_texture, tex_coord2)[shininess_channel];

	float transparency2    = transparency;
	vec3  emission_surface = emission_colour;
	vec3   ambient_surface =  ambient_colour;
	vec3   diffuse_surface =  diffuse_colour;
	vec3  specular_surface = specular_colour;
	float shininess2       = shininess;

	if(a_is_texture_active[0]) transparency2    *= transparency_on_texture;
	if(a_is_texture_active[1]) emission_surface *=     emission_on_texture;
	if(a_is_texture_active[2])  ambient_surface *=      ambient_on_texture;
	if(a_is_texture_active[3])  diffuse_surface *=      diffuse_on_texture;
	if(a_is_texture_active[4]) specular_surface *=     specular_on_texture;
	if(a_is_texture_active[5]) shininess2       *=    shininess_on_texture;

	vec3 basic_colour      = emission_surface;
	vec4 seperate_specular = vec4(0.0, 0.0, 0.0, 0.0);

	// calculate the effects of each light

	for(int i = 0; i < LIGHT_COUNT; i++)
		if(a_is_light_enabled[i])
		{
			vec3 light_direction_from;
			float attenuation = 1.0;
			if(light_position[i].w == 0)
				light_direction_from = normalize(light_position[i].xyz);
			else
			{
				vec3 offset_position = light_position[i].xyz - position;
				light_direction_from = normalize(offset_position);
				float light_distance = length(offset_position);
				attenuation = 1.0 / (light_attenuation[i].x +
				                     light_attenuation[i].y * light_distance +
				                     light_attenuation[i].z * light_distance * light_distance);
			}

			//  Phong-Blinn shading

			vec3 half_angle_direction = normalize(normalized_to_camera + light_direction_from);

			//Multiply diffuse and specular lighting by light_factor to reduce light in shadows
			float  ambient_intensity = attenuation;
			float  diffuse_intensity = light_factor * attenuation * max(0, dot(normalized_normal, light_direction_from));
			float specular_intensity = light_factor * attenuation * pow(max(0, dot(normalized_normal, half_angle_direction)), shininess2);

			basic_colour += ambient_surface * light_ambient[i] * ambient_intensity +
			                diffuse_surface * light_diffuse[i] * diffuse_intensity;

			vec3 specular_colour = specular_surface * light_specular[i];
			if(is_seperate_specular)
			{
				float specular_max = max(max(specular_colour.r, specular_colour.g), specular_colour.b);
				if(specular_max > 0.0)
				{
					specular_intensity *= specular_max;
					specular_colour    /= specular_max;

					seperate_specular = vec4(seperate_specular.rgb * seperate_specular.a +
					                         specular_colour       * specular_intensity,
					                         seperate_specular.a + specular_intensity);
				}
				// else no specular highlight
			}
			else
				basic_colour += specular_colour * specular_intensity;
		}

	// calculate the final colour

	if(seperate_specular.a > 1.0)
		seperate_specular.a = 1.0;

	float basic_transparency = transparency2 * (1.0 - seperate_specular.a);

	if(basic_transparency + seperate_specular.a <= 0.0)
		discard;

	FragColor = vec4(basic_colour, basic_transparency) + seperate_specular;
}
