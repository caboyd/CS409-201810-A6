#pragma once
#include "lib/GetGlutWithShaders.h"
#include <string>
#include "lib/ObjLibrary/Vector3.h"

using ObjLibrary::Vector3;


struct Point
{
	glm::vec3 pos;
	glm::vec4 color;
	Point() = default;
	Point(glm::vec3 pos, glm::vec4 color) :pos(pos), color(color) {}
};

class LineRenderer
{
private:
	const std::string shader_folder = "assets/shaders/";
	const std::string line_shader_vert = "line.vert";
	const std::string line_shader_frag = "line.frag";

	GLuint VAO, VBO;
	GLuint line_program_id;


	//Uniform location
	GLuint mvp_id;
public:

	LineRenderer() = default;


	void init();

	void draw(const Point& p1, const Point& p2, glm::mat4& mvp_matrix) const;

	void draw(std::vector<Vector3> points, glm::vec4 color, glm::mat4& mvp_matrix) const;

	void draw(std::vector<Point> vertexData, glm::mat4& mvp_matrix) const;

	void draw(const Vector3& p1, const Vector3& p2, glm::mat4& mvp_matrix) const;
};

inline void LineRenderer::init()
{
	//Init shader
	line_program_id = ObjShader::initProgramStart(shader_folder + line_shader_vert,
		shader_folder + line_shader_frag);
	ObjShader::initProgramEnd(line_program_id);
	mvp_id = glGetUniformLocation(line_program_id, "model_view_projection_matrix");

	// generate and bind the vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// generate and bind the buffer object
	glGenBuffers(1, &VBO);

	glBindVertexArray(0);
}

inline void LineRenderer::draw(const Point& p1, const Point& p2, glm::mat4& mvp_matrix) const
{
	std::vector<Point> vertex_data;
	vertex_data.push_back(p1);
	vertex_data.push_back(p2);
	// fill with data
	draw(vertex_data, mvp_matrix);
}

inline void LineRenderer::draw(std::vector<Point> vertexData, glm::mat4& mvp_matrix) const
{
	glUseProgram(line_program_id);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)12);

	// fill with data
	const size_t num_verts = vertexData.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &vertexData[0], GL_STATIC_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(mvp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	glBindVertexArray(0);
}

inline void LineRenderer::draw(const Vector3& p1, const Vector3& p2, glm::mat4& mvp_matrix) const
{
	draw(Point(glm::vec3(p1), glm::vec4(1, 1, 1, 1.0)), Point(glm::vec3(p2), glm::vec4(1, 1, 1, 1.0)), mvp_matrix);
}

inline void LineRenderer::draw(std::vector<Vector3> points, glm::vec4 color, glm::mat4& mvp_matrix) const
{
	std::vector<Point> vertex_data;
	vertex_data.resize(points.size());
	for (unsigned int i = 0; i < points.size(); i++)
		vertex_data[i] = Point(glm::vec3(points[i]), color);

	draw(vertex_data, mvp_matrix);
}
