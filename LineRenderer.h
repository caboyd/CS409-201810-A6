/**
 *	LineRenderer
 *	
 *	Draws a line specified by points in 3D space.
 *	Perspective determined by an MVP matrix parameter
 *	Must be initialized before use
 *
 */



#pragma once
#include "lib/GetGlutWithShaders.h"
#include <string>
#include "lib/ObjLibrary/Vector3.h"

using ObjLibrary::Vector3;



class LineRenderer
{
private:
struct Point
{
	glm::vec3 pos;
	glm::vec4 color;
	Point() = default;
	Point(glm::vec3 pos, glm::vec4 color) :pos(pos), color(color) {}
};


	const std::string shader_folder = "assets/shaders/";
	const std::string line_shader_vert = "line.vert";
	const std::string line_shader_frag = "line.frag";

	GLuint VAO, VBO;
	GLuint line_program_id;

	//Uniform location
	GLuint mvp_id;
public:

	LineRenderer() = default;

	
	//Initializes the shader program and VAO
	void init();


	//Draws Lines connecting the specified pooints using the giving color and mvp matrix
	void draw(std::vector<Vector3> points, glm::vec4 color, glm::mat4& mvp_matrix) const;

	//Draws Lines using the specified point vertexData, and mxp_matrix
	//Each line requires two vertexes, to draw a line between 3 points, the second point must be duplicated
	void draw(std::vector<Point> vertexData, glm::mat4& mvp_matrix) const;

};

inline void LineRenderer::init()
{
	//Init shader
	line_program_id = ObjShader::initProgramStart(shader_folder + line_shader_vert,
		shader_folder + line_shader_frag);
	ObjShader::initProgramEnd(line_program_id);
	mvp_id = glGetUniformLocation(line_program_id, "model_view_projection_matrix");

	// generate and bind the vao
	//glGenVertexArrays(1, &VAO);
	//glBindVertexArray(VAO);
	
	// generate and bind the buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Set the shader attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)12);


	//glBindVertexArray(0);
}


inline void LineRenderer::draw(std::vector<Point> vertexData, glm::mat4& mvp_matrix) const
{
	glUseProgram(line_program_id);

	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data
	const size_t num_verts = vertexData.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &vertexData[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(mvp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	//glBindVertexArray(0);
}

inline void LineRenderer::draw(std::vector<Vector3> points, glm::vec4 color, glm::mat4& mvp_matrix) const
{
	std::vector<Point> vertex_data;
	vertex_data.resize(points.size() * 2 - 2);
	unsigned int count = 0;
	for (unsigned int i = 0; i < points.size(); i++)
	{
		vertex_data[count++] = Point(glm::vec3(points[i]), color);
		if (i > 0 && i < points.size() - 1)
			vertex_data[count++] = Point(glm::vec3(points[i]), color);

	}
	draw(vertex_data, mvp_matrix);
}
