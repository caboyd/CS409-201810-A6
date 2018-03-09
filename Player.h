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
	Player()
	{
		resetPosition();
	}

	void init()
	{
		model.init();
	}

	void updateAnimation(float delta_time)
	{
		model.updateAnimation(delta_time);
	}

	void resetPosition()
	{
		coordinate_system.setPosition(PLAYER_INIT_POS);
		coordinate_system.setOrientation(PLAYER_INIT_FORWARD);
	};

	void transitionAnimationTo(Player_State state)
	{
		model.transitionTo(state);
	}

	void setPosition(Vector3 pos)
	{
		coordinate_system.setPosition(pos + PLAYER_OFFSET);
	}

	void draw(const glm::mat4x4& view_matrix,
		const glm::mat4x4& projection_matrix,
		const glm::vec3& camera_position) const
	{
		const Vector3& pos = coordinate_system.getPosition();
		const Vector3& forward = coordinate_system.getForward();
		//Draw the player
		glm::mat4 model_matrix = glm::mat4();
		model_matrix = glm::translate(model_matrix, glm::vec3(pos));
		model_matrix = glm::rotate(model_matrix, (float(atan2(forward.x, forward.z)) - float(MathHelper::M_PI_2)), glm::vec3(coordinate_system.getUp()));;



		model.draw(model_matrix, view_matrix, projection_matrix, camera_position);
	}

	void drawToDepth()
	{
		model.drawToDepth();
	}



};
