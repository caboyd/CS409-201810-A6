#include "Player.h"


Player::Player()
{

}

void Player::init()
{
	resetPosition();
	model.init();
}

void Player::updateAnimation(double delta_time)
{
	model.updateAnimation(delta_time);
}

void Player::resetPosition()
{
	coordinate_system.setPosition(PLAYER_INIT_POS);
	coordinate_system.setOrientation(PLAYER_INIT_FORWARD);
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
