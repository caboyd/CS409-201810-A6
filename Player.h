#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "PlayerAnimatedModel.h"
#include "CoordinateSystem.h"
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "MathHelper.h"

using ObjLibrary::Vector3;

static const Vector3 PLAYER_OFFSET = Vector3(0.0f, 0.8f, 0.0f);
	static const Vector3 PLAYER_INIT_POS = Vector3(0.0f, 0.0f, 0.0f) + PLAYER_OFFSET;
	static const Vector3 PLAYER_INIT_FORWARD = Vector3(0, 0, -1);



class Player
{
public:
	
	CoordinateSystem coordinate_system;
public:
	PlayerAnimatedModel model;


public:
	Player();

	void init();

	void updateAnimation(double delta_time);

	void resetPosition();;

	void transitionAnimationTo(Player_State state);

	void setPosition(Vector3 pos);

	void draw(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix,
		const glm::vec3& camera_position) const;

	void drawToDepth();
};
