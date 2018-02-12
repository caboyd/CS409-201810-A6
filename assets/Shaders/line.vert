uniform mat4 model_view_projection_matrix;

layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec4 a_color;

out vec4 color;

void main()
{
	gl_Position = model_view_projection_matrix * vec4(a_vertex, 1.0);
		
	color = a_color;
}
