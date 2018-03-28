#include "LineRenderer.h"
#include "lib/ObjLibrary/ObjShader.h"

void LineRenderer::init()
{
	//Init shader
	line_program_id = ObjLibrary::ObjShader::initProgramStart(shader_folder + line_shader_vert,
		shader_folder + line_shader_frag);
	ObjLibrary::ObjShader::initProgramEnd(line_program_id);
	mvp_id = glGetUniformLocation(line_program_id, "model_view_projection_matrix");

	// generate and bind the vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// generate and bind the buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Set the shader attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), nullptr);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_TRUE, sizeof(Point), reinterpret_cast<void*>(12));
	glBindVertexArray(0);
}


void LineRenderer::draw(const std::vector<Point>& vertexData, const glm::mat4& vp_matrix) const
{
	glUseProgram(line_program_id);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data
	const size_t num_verts = vertexData.size();
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &vertexData[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(vp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	glBindVertexArray(0);
}

void LineRenderer::preAllocateLine(unsigned size)
{
	points.reserve(size);
}

void LineRenderer::addLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color)
{
	points.emplace_back(p1, color);
	points.emplace_back(p2, color);

}

void LineRenderer::drawLinesAndClear(const glm::mat4& vp_matrix)
{
	const size_t num_verts = points.size();
	if (num_verts == 0) return;
	glUseProgram(line_program_id);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data

	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &points[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(vp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	glBindVertexArray(0);
	points.clear();

}

void LineRenderer::drawPointList(const std::vector<Point>& point_list, const glm::mat4& vp_matrix) const
{
	const size_t num_verts = point_list.size();
	if (num_verts == 0) return;
	glUseProgram(line_program_id);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// fill with data
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * num_verts, &point_list[0], GL_STREAM_DRAW);
	glUniformMatrix4fv(mvp_id, 1, false, &(vp_matrix[0][0]));

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, num_verts);
	glBindVertexArray(0);

}

void LineRenderer::draw(const std::vector<Vector3>& points, const glm::vec4& color, const glm::mat4& vp_matrix) const
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

void LineRenderer::draw(const Vector3& p1, const Vector3& p2, const glm::vec4& color, const glm::mat4& vp_matrix)  const
{
	std::vector<Point> vertex_data;
	vertex_data.resize(2);
	vertex_data[0] = Point(glm::vec3(p1), color);
	vertex_data[1] = Point(glm::vec3(p2), color);
	draw(vertex_data, vp_matrix);
}