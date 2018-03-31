#include "Bat.h"

#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"
#include "SphereRenderer.h"

extern SphereRenderer g_sphere_renderer;

Bat::Bat(const ModelWithShader& model, const Player& player, const World& world, const Vector3& position) :Entity(model, position)
{
	this->model = &model;
	this->player = &player;
	this->world = &world;
	this->coordinate_system.setPosition(position);
	this->coordinate_system.setOrientation(Vector3(0, 0, -1), Vector3(0, 1, 0));
	this->state = Bat_State::EXPLORE;
	this->model_scalar = Vector3(1, 1, 1);
}

void Bat::update(double delta_time)
{
	switch (state)
	{
	case Bat_State::EXPLORE:
	{

		break;
	}
	}
}

void Bat::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix,
	const glm::vec3& camera_position) const
{
	const Vector3& pos = coordinate_system.getPosition();
	const Vector3& forward = coordinate_system.getForward();

	//Draw the bat
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = translate(model_matrix, glm::vec3(pos));
	model_matrix = rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)),
		glm::vec3(coordinate_system.getUp()));
	model_matrix = glm::scale(model_matrix, glm::vec3(model_scalar));
	const glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

	//g_sphere_renderer.draw(glm::vec3(0, 1, 0), model_matrix, view_matrix, mvp_matrix);

	model->draw(model_matrix, view_matrix, mvp_matrix, camera_position);
}

void Bat::drawToDepth(const glm::mat4x4& depth_view_projection_matrix) const
{
	const Vector3& pos = coordinate_system.getPosition();
	const Vector3& forward = coordinate_system.getForward();

	glm::mat4x4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(pos));
	model_matrix = rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)),
		glm::vec3(coordinate_system.getUp()));
	model_matrix = glm::scale(model_matrix, glm::vec3(model_scalar));

	glm::mat4x4 depth_mvp = depth_view_projection_matrix * model_matrix;
	g_depth_texture.setDepthMVP(depth_mvp);

	unsigned int mat_count = model->getMaterialCount();
	for (unsigned int i = 0; i < mat_count; i++)
	{
		const unsigned int mesh_count = model->getMeshCount(i);
		for (unsigned int j = 0; j < mesh_count; j++)
		{
			const ObjLibrary::MeshWithShader& m = model->getMesh(i, j);

			m.draw();
		}

	}
}
