#include "Player.h"
#include "MathHelper.h"
#include "World.h"

Player::Player()
{}

void Player::init()
{

	reset();
	model.init();
}

void Player::updateAnimation(double delta_time)
{
	model.updateAnimation(delta_time);
}

void Player::update(const World& world, double delta_time)
{
	Vector3 new_pos = coordinate_system.getPosition() + velocity * delta_time / 1000;
	coordinate_system.setPosition(new_pos);

	float friction = world.getFrictionAtPosition(float(new_pos.x), float(new_pos.z));
	velocity = velocity * pow(friction, delta_time / 1000.0);

	float min_slope = world.getSlopeFactorAtPosition(float(new_pos.x), float(new_pos.z));

	float player_height = world.getHeightAtCirclePosition(float(new_pos.x), float(new_pos.z), radius);

	
	float min_height = player_height;
	Vector2 min_dir;
	Vector2 dir(1, 0);

	for (unsigned int i = 0; i < 60; i++)
	{
		dir.rotate(i * MathHelper::M_2PI / 60.0);
		const Vector2 pos(new_pos.x + dir.x * 0.01, new_pos.z + dir.y * 0.01);
		float h = world.getHeightAtPointPosition(float(pos.x), float(pos.y));
		if (h < min_height)
		{
			min_height = h;
			min_dir = dir;
		}
	}

	float slope = (player_height - min_height) / 0.01;

	if (slope > min_slope)
	{
		float a = (slope - min_slope) * 10.0f / 1000.0f;
		Vector3 accel(min_dir.x, 0, min_dir.y);
		accel *= a;
		addAcceleration(accel);
	}


}

void Player::reset()
{
	coordinate_system.setPosition(PLAYER_INIT_POS);
	coordinate_system.setOrientation(PLAYER_INIT_FORWARD);
	velocity = Vector3::ZERO;
}

void Player::transitionAnimationTo(Player_State state)
{
	model.transitionTo(state);
}

void Player::setPosition(Vector3 pos)
{
	coordinate_system.setPosition(pos + PLAYER_OFFSET);
}

void Player::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix,
	const glm::vec3& camera_position) const
{
	const Vector3& pos = coordinate_system.getPosition();
	const Vector3& forward = coordinate_system.getForward();
	//Draw the player
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = translate(model_matrix, glm::vec3(pos));
	model_matrix = rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)),
		glm::vec3(coordinate_system.getUp()));


	model.draw(model_matrix, view_matrix, projection_matrix, camera_position);
}

void Player::drawToDepth()
{
	model.drawToDepth();
}

float Player::getRadius() const
{
	return radius;
}

void Player::addAcceleration(const Vector3& a)
{
	velocity += a;
}
