
layout (location = 0) in vec3 a_vertex;
layout (location = 1) in vec3 a_position;
layout (location = 2) in float a_size;
layout (location = 3) in vec4 a_color;

out vec4 color;
out vec3 vpos;
out vec2 UV;

// Values that stay constant for the whole mesh.
uniform vec3 camera_right_worldspace;
uniform vec3 camera_up_worldspace;
uniform mat4 view_projection_matrix;
// Model-View-Projection matrix, but without the Model (the position is in BillboardPos; the orientation depends on the camera)

void main()
{
    vec3 vertex_position_worldspace = 
		a_position
		+ camera_right_worldspace * a_vertex.x * a_size
		+ camera_up_worldspace * a_vertex.y * a_size;

    // Output position of the vertex
	gl_Position =  view_projection_matrix * vec4(vertex_position_worldspace, 1.0f);

    // UV of the vertex. No special space for this one.
	UV = a_vertex.xy + vec2(0.5, 0.5);
    color = a_color;
}
