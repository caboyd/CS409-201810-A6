#pragma once
#include "lib/GetGlutWithShaders.h"
#include "lib/ObjLibrary/Vector3.h"
#include "lib/glm/glm.hpp"
#include <vector>
using ObjLibrary::Vector3;

/**
 *	LineRenderer
 *
 *	Draws a line specified by points in 3D space.
 *	Perspective determined by an MVP matrix parameter
 *	Must be initialized before use
 *
 */
class LineRenderer
{
public:
	struct Point
	{
		glm::vec3 pos{};
		glm::vec4 color{};
		Point() = default;
		Point(glm::vec3 pos, glm::vec4 color) :pos(pos), color(color) {}
		Point(Vector3 pos, glm::vec4 color):pos(glm::vec3(pos)), color(color){};
	};
private:

	const std::string shader_folder = "assets/shaders/";
	const std::string line_shader_vert = "line.vert";
	const std::string line_shader_frag = "line.frag";
	std::vector<Point> points;

	GLuint VAO{}, VBO{};
	GLuint line_program_id{};

	//Uniform location
	GLuint mvp_id{};
public:

	LineRenderer() = default;


	//Initializes the shader program and VAO
	void init();

	//Reserve memory in the points vector
	void preAllocateLine(unsigned size);

	//Draws Lines connecting the specified 2 points using the giving color and vp matrix
	void draw(const Vector3& p1, const Vector3& p2, const glm::vec4& color, glm::mat4& vp_matrix) const;

	//Draws Lines connecting the specified pooints using the giving color and mvp matrix
	void draw(const std::vector<Vector3>& points, const glm::vec4& color, glm::mat4& vp_matrix) const;

	//Draws Lines using the specified point vertexData, and mxp_matrix
	//Each line requires two vertexes, to draw a line between 3 points, the second point must be duplicated
	void draw(const std::vector<Point>& vertexData, glm::mat4& vp_matrix) const;

	//Add a line to the points vector
	void addLine(const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color);

	//Draws the lines in the points vector and clears it
	void drawLinesAndClear(const glm::mat4& vp_matrix);

	//Draws the lines of the provided point_list vector
	void drawPointList(const std::vector<Point>& point_list, const glm::mat4& vp_matrix) const;
};

