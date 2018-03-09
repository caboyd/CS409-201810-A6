// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertex;
layout(location = 3) in vec3 vertex1;
// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;
uniform float tween_factor;
uniform bool tween_enabled;

void main()
{
    vec4 vert = vec4(vertex,1);
	if(tween_enabled)
	{
        vert =  vec4(mix(vertex,vertex1, tween_factor),1);
    }

	gl_Position =  depthMVP * vert;
}


