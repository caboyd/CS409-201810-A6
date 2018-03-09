#pragma once

#include "lib/ObjLibrary/ModelWithShader.h"
#include "lib/ObjLibrary/KeyframeModelWithShader.h"
#include "lib/ObjLibrary/ObjModel.h"

#include "DepthTexture.h"

extern DepthTexture depth_texture;

enum class Player_State
{
	Standing,
	Jumping,
	Falling,
	Running,
	Reversing
};

enum class Run_State
{
	Start = 0,
	Run1 = 1,
	Run2 = 2,
	Run3 = 3,
	Run4 = 4,
	Run5 = 5,
	StartReverse = 6
};

struct Frame
{
	ObjLibrary::KeyframeModelWithShader model;
	float duration; // in Milliseconds;
};


using namespace ObjLibrary;

class PlayerAnimatedModel
{
	
private:
	const std::string model_folder = "assets/Models/";
	ObjLibrary::ModelWithShader stand_model;
	ObjLibrary::ModelWithShader jump_model;
	Frame run_frames[7];
	float time_into_frame;
	Run_State run_state;
	Player_State state;
public:
	PlayerAnimatedModel() = default;

	void init();


	void updateAnimation(double delta_time);

	void draw(const glm::mat4x4& model_matrix,
	          const glm::mat4x4& view_matrix,
	          const glm::mat4x4& model_view_projection_matrix,
	          const glm::vec3& camera_position) const;

	void drawToDepth() const;


	void transitionTo(Player_State new_state);
};


