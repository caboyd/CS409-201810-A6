#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "PlayerAnimatedModel.h"
#include "CoordinateSystem.h"
#include "lib/glm/glm.hpp"
#include "World.h"

using ObjLibrary::Vector3;

static const Vector3 PLAYER_OFFSET = Vector3(0.0f, 0.8f, 0.0f);
static const Vector3 PLAYER_INIT_POS = Vector3(0.0f, 0.0f, 0.0f) + PLAYER_OFFSET;
static const Vector3 PLAYER_INIT_FORWARD = Vector3(0, 0, -1);

//Meters per 1000 milliseconds
static const  int JUMP_FORWARD_SPEED = 8.0;
static const int JUMP_UP_SPEED =  12.0;


class Player
{
public:
	//Position and Orientation
	CoordinateSystem coordinate_system;




	PlayerAnimatedModel model;
private:
	Vector3 velocity;
	const float radius = 0.25;
	bool jumping;

public:
	Player();

	void init();

	void updateAnimation(double delta_time);
	void update(const World& world, double delta_time);

	void reset();;

	void transitionAnimationTo(Player_State state);

	void setPosition(Vector3 pos);
	Vector3 getPosition() const;

	void draw(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix,
		const glm::vec3& camera_position) const;

	void drawToDepth(const glm::mat4x4& depth_view_projection_matrix) const;

	float getRadius() const;

	void addAcceleration(const Vector3& a);
	void jump();

};
