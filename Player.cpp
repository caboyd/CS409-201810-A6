#include "Player.h"
#include "MathHelper.h"
#include "World.h"
#include "DepthTexture.h"

extern DepthTexture depth_texture;

Player::Player()
{}

void Player::init(const World& world)
{

	reset(world);
	model.init();
}

void Player::updateAnimation(double delta_time)
{
	model.updateAnimation(delta_time);
}

void Player::update(const World& world, double delta_time_seconds)
{

	Vector3 old_pos = getPosition();
	Vector3 new_pos = old_pos + (velocity * delta_time_seconds);

	float height = world.getHeightAtCirclePosition(float(new_pos.x), float(new_pos.z), radius);

	if (jumping)
	{

		velocity = velocity + Vector3(0, -9.8,0) * delta_time_seconds;

		//Check collision with disk
		if (world.isCylinderCollisionWithDisk(Vector3(new_pos.x, new_pos.y , new_pos.z) + PLAYER_OFFSET, radius, 0.8f))
		{
			if (world.isCylinderCollisionWithDisk(Vector3(old_pos.x, old_pos.y, old_pos.z) + PLAYER_OFFSET, radius, 0.8f))
			{
				//Landing
				jumping = false;
				model.transitionTo(Player_State::Standing);
				velocity.y = 0;
				new_pos.y = height;
			} else
			{
				//Hit side of disk while falling
				velocity.x = 0;
				velocity.z = 0;
				new_pos.x = old_pos.x;
				new_pos.z = old_pos.z;
			}
		}

	} else
	{
		if (world.isOnDisk(float(new_pos.x), float(new_pos.z), radius))
		{
			//Not Falling

			//Set height
			new_pos.y = height;
			velocity.y = 0;

			//Apply friction
			float friction = world.getFrictionAtPosition(float(new_pos.x), float(new_pos.z));
			velocity = velocity * pow(friction, delta_time_seconds);

			//Apply Sliding
			float min_slope = world.getSlopeFactorAtPosition(float(new_pos.x), float(new_pos.z));
			float player_height = float(new_pos.y);
			float min_height = float(new_pos.y);
			Vector2 min_dir;
			
			for (unsigned int i = 0; i < 60; i++)
			{
				Vector2 dir(1, 0);
				dir.rotate(MathHelper::M_2PI * (i/60.0));
				const Vector2 pos(new_pos.x + dir.x * 0.01, new_pos.z + dir.y * 0.01);
				float h = world.getHeightAtPointPosition(float(pos.x), float(pos.y));
				if (h < min_height)
				{
					min_height = h;
					min_dir = dir;
				}
			}

			float slope = (player_height - min_height) / 0.01f;

			if (slope > min_slope)
			{
				float a =( (slope - min_slope) * 10.0f) * float(delta_time_seconds);
				Vector3 accel(min_dir.x, 0, min_dir.y);
				accel *= a;
				addAcceleration(accel);
			}
		} else
		{
			//Falling
			model.transitionTo(Player_State::Falling);
			jumping = true;
		}



	}

	//Update position to new position
	setPosition(new_pos);




}

void Player::reset(const World& world)
{
	Vector3 pos =world.disks[0]->position;
	pos.y = world.getHeightAtPointPosition(float(pos.x),float(pos.z));
	coordinate_system.setPosition(pos);
	coordinate_system.setOrientation(PLAYER_INIT_FORWARD);
	velocity = Vector3::ZERO;
	jumping = false;
}

void Player::transitionAnimationTo(Player_State state)
{
	if (!jumping)
		model.transitionTo(state);
}

bool Player::isStanding() const
{
	return ( model.getState() == Player_State::Standing);
}

void Player::setPosition(Vector3 pos)
{
	coordinate_system.setPosition(pos);
}

Vector3 Player::getPosition() const
{
	return coordinate_system.getPosition();
}

void Player::draw(const glm::mat4x4& view_matrix, const glm::mat4x4& projection_matrix,
	const glm::vec3& camera_position) const
{
	Vector3 pos = coordinate_system.getPosition();
	pos.y += 0.8;
	const Vector3& forward = coordinate_system.getForward();
	//Draw the player
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = translate(model_matrix, glm::vec3(pos));
	model_matrix = rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)),
		glm::vec3(coordinate_system.getUp()));


	model.draw(model_matrix, view_matrix, projection_matrix, camera_position);
}

void Player::drawToDepth(const glm::mat4x4& depth_view_projection_matrix) const
{
	Vector3 pos = coordinate_system.getPosition();
	pos.y += 0.8;
	Vector3 forward = coordinate_system.getForward();
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(pos));

	model_matrix = glm::rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)), glm::vec3(coordinate_system.getUp()));;
	glm::mat4 depth_mvp = depth_view_projection_matrix * model_matrix;

	g_depth_texture.setDepthMVP(depth_mvp);
	model.drawToDepth();
}

float Player::getRadius() const
{
	return radius;
}

void Player::addAcceleration(const Vector3& a)
{
	if (!jumping)
		velocity += a;
}

void Player::jump()
{
	if(jumping) return;
	velocity = coordinate_system.getForward() * JUMP_FORWARD_SPEED + Vector3(0, JUMP_UP_SPEED, 0);
	model.transitionTo(Player_State::Jumping);
	jumping = true;

}
