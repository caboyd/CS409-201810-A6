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
#include "lib/ObjLibrary/ObjShader.h"
#include <vector>
using ObjLibrary::Vector3;
using namespace ObjLibrary;


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
	std::vector<Point> points;

	GLuint VAO, VBO;
	GLuint line_program_id;

	//Uniform location
	GLuint mvp_id;
public:

	LineRenderer() = default;


	//Initializes the shader program and VAO
	void init();

	//Draws Lines connecting the specified 2 points using the giving color and vp matrix
	void draw(const Vector3& p1, const Vector3& p2, const glm::vec4& color, glm::mat4& vp_matrix) const;

	//Draws Lines connecting the specified pooints using the giving color and mvp matrix
	void draw(const std::vector<Vector3>& points, const glm::vec4& color, glm::mat4& vp_matrix) const;

	//Draws Lines using the specified point vertexData, and mxp_matrix
	//Each line requires two vertexes, to draw a line between 3 points, the second point must be duplicated
	void draw(const std::vector<Point>& vertexData, glm::mat4& vp_matrix) const;


	void addLine(const Vector3& p1, const Vector3& p2, const glm::vec4& color);

	void drawLinesAndClear(glm::mat4& vp_matrix);
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


inline void LineRenderer::draw(const std::vector<Point>& vertexData, glm::mat4& vp_matrix) const
{
	glUseProgram(line_program_id);

	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data
	const size_t num_verts = vertexData.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &vertexData[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(vp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	//glBindVertexArray(0);
}

inline void LineRenderer::addLine(const Vector3& p1, const Vector3& p2, const glm::vec4& color)
{
	points.emplace_back(glm::vec3(p1), color);
	points.emplace_back(glm::vec3(p2), color);

}

inline void LineRenderer::drawLinesAndClear(glm::mat4& vp_matrix)
{
	const size_t num_verts = points.size();
	if(num_verts ==0) return;
	glUseProgram(line_program_id);

	//glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &points[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(vp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	points.clear();
	//glBindVertexArray(0);
}

inline void LineRenderer::draw(const std::vector<Vector3>& points, const glm::vec4& color, glm::mat4& vp_matrix) const
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
	draw(vertex_data, vp_matrix);
}

inline void LineRenderer::draw(const Vector3& p1, const Vector3& p2, const glm::vec4& color, glm::mat4& vp_matrix)  const
{
	std::vector<Point> vertex_data;
	vertex_data.resize(2);
	vertex_data[0] = Point(glm::vec3(p1), color);
	vertex_data[1] = Point(glm::vec3(p2), color);
	draw(vertex_data, vp_matrix);
}