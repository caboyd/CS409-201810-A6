#include "Bat.h"

#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"
#include "SphereRenderer.h"
#include "Collision.h"

extern SphereRenderer g_sphere_renderer;

Bat::Bat(const ModelWithShader& model, const Player& player, const World& world, const Vector3& position) :Entity(model, position)
{
	this->model = &model;
	this->player = &player;
	this->world = &world;
	this->model_scalar = Vector3(1, 1, 1);
	this->state = Bat_State::EXPLORE;

	this->coordinate_system.setPosition(position);
	this->velocity = Vector3::getRandomUnitVectorXZ() * S_MAX;
	this->coordinate_system.setOrientation(velocity.getNormalized());
	this->target_position = world.getRandomXZPosition();
	this->target_position.y = 15.0f;
}

void Bat::update(double delta_time_seconds)
{
	if (state == Bat_State::DEAD) return;
	ignore_timer -= float(delta_time_seconds);
	if (ignore_timer < 0) ignore_timer = 0;


	if(world->isCylinderCollisionWithDisk(coordinate_system.position,radius,0))
	{
		state = Bat_State::DEAD;
		ignore_timer = 1.0f;
		return;
	}

	if (Collision::cylinderIntersection(coordinate_system.position, radius + 20.0f, half_height + 20.0f,
		player->coordinate_system.position, player->getRadius(), player->getHalfHeight()))
	{
		state = Bat_State::PURSUE;
	} else
	{
		state = Bat_State::EXPLORE;
	}

	switch (state)
	{
	case Bat_State::EXPLORE:
	{
		explore(delta_time_seconds);
		break;
	}
	case Bat_State::PURSUE:
	{
		pursue(delta_time_seconds);
		break;
	}
	default:;
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

void Bat::seek(double delta_time_seconds)
{
	Vector3 R = target_position - coordinate_system.position;
	Vector3 D = R.getNormalized() * S_MAX;

	//Desired steering velocity
	Vector3 S = D - velocity;// + Vector3::getRandomInRange(0.0001f);


	//Desired Acceleration in steering direction
	Vector3 A_desired = S / delta_time_seconds;

	//Actual Acceleration
	Vector3 A = MathHelper::truncate(A_desired, A_MAX);

	//Update velocity and position
	velocity = MathHelper::truncate(velocity + A * delta_time_seconds, S_MAX);
	//velocity = MathHelper::minVector(velocity, S_MIN);
	coordinate_system.position += velocity * delta_time_seconds;

	coordinate_system.forward = velocity.getNormalized();

}

void Bat::explore(double delta_time_seconds)
{
	if (Collision::cylinderIntersection(coordinate_system.position, radius, half_height,
		target_position, 2.0f, 1.0f))
	{
		target_position = world->getRandomXZPosition();
		target_position.y = 15.0f;
	}
	seek(delta_time_seconds);
}

void Bat::pursue(double delta_time_seconds)
{
	//Determine how many seconds away from target
	Vector3 d = player->getPosition() - coordinate_system.position;
	float s = float(d.getNorm()) * 0.3f;

	//predicted player position
	Vector3 T = player->getPosition() + player->getVelocity() * s;

	target_position = T;
	target_position.y += player->getHalfHeight();
	seek(delta_time_seconds);
}
