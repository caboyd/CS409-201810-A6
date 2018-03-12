#include "PlayerAnimatedModel.h"
#include <lib/glm/gtc/matrix_transform.inl>
#include "MathHelper.h"

void PlayerAnimatedModel::init()
{
	//RUN MODELS
	ObjModel temp[6];
	temp[0].load(model_folder + "cbabe_run_start.obj");
	temp[1].load(model_folder + "cbabe_run_loop0.obj");
	temp[2].load(model_folder + "cbabe_run_loop1.obj");
	temp[3].load(model_folder + "cbabe_run_loop2.obj");
	temp[4].load(model_folder + "cbabe_run_loop3.obj");
	temp[5].load(model_folder + "cbabe_run_loop4.obj");

	run_frames[0].model = temp[0].getKeyframeModelWithShader(temp[1]);
	run_frames[1].model = temp[1].getKeyframeModelWithShader(temp[2]);
	run_frames[2].model = temp[2].getKeyframeModelWithShader(temp[3]);
	run_frames[3].model = temp[3].getKeyframeModelWithShader(temp[4]);
	run_frames[4].model = temp[4].getKeyframeModelWithShader(temp[5]);
	run_frames[5].model = temp[5].getKeyframeModelWithShader(temp[1]);
	run_frames[6].model = temp[1].getKeyframeModelWithShader(temp[0]);

	run_frames[0].duration = 140;
	run_frames[1].duration = 140;
	run_frames[2].duration = 140;
	run_frames[3].duration = 210;
	run_frames[4].duration = 140;
	run_frames[5].duration = 140;
	run_frames[6].duration = 140;

	temp[0].load(model_folder + "cbabe_stand.obj");
	stand_model = temp[0].getModelWithShader();

	temp[0].load(model_folder + "cbabe_jump.obj");
	jump_model = temp[0].getModelWithShader();

	animation_rate = 1;
}

Player_State PlayerAnimatedModel::getState() const
{
	return state;
}

void PlayerAnimatedModel::updateAnimation(double delta_time)
{
	int i = int(run_state);
	switch (state)
	{
	case Player_State::Running:
	case Player_State::Strafing:
	{
		time_into_frame += float(delta_time) * animation_rate;
		//go to next state
		if (time_into_frame > run_frames[i].duration)
		{
			time_into_frame -= run_frames[i].duration;
			run_state = Run_State(int(run_state) + 1);

			if (int(run_state) > int(Run_State::Run5))
				run_state = Run_State::Run1;
		}
		break;
	}
	case Player_State::Reversing:
	{
		time_into_frame -= float(delta_time) * animation_rate;
		//go to next state
		if (time_into_frame < 0)
		{
			run_state = Run_State(int(run_state) - 1);

			if (int(run_state) < int(Run_State::Run1)) run_state = Run_State::Run5;
			i = int(run_state);
			time_into_frame += run_frames[i].duration;
		}
		break;
	}
	}
}

void PlayerAnimatedModel::draw(const glm::mat4x4& model_matrix, const glm::mat4x4& view_matrix,
	const glm::mat4x4& projection_matrix, const glm::vec3& camera_position) const
{
	glm::mat4 model = model_matrix;
	switch (state)
	{
	case Player_State::Running:
	case Player_State::Strafing:
	case Player_State::Reversing:
		model = glm::translate(model, glm::vec3(0, -0.075, 0));
	}

	const glm::mat4 model_view_projection_matrix = projection_matrix * view_matrix * model;
	switch (state)
	{
	case Player_State::Running:
	case Player_State::Strafing:
	case Player_State::Reversing:
	{
		float tween = time_into_frame / run_frames[(int)run_state].duration;
		run_frames[(int)run_state].model.draw(tween, model, view_matrix, model_view_projection_matrix,
			camera_position);
		break;
	}
	case Player_State::Standing:
	{
		stand_model.draw(model, view_matrix, model_view_projection_matrix, camera_position);
		break;
	}
	case Player_State::Jumping:
	case Player_State::Falling:
	{
		jump_model.draw(model, view_matrix, model_view_projection_matrix, camera_position);
		break;
	}
	}
}

void PlayerAnimatedModel::drawToDepth() const
{

	switch (state)
	{
	case Player_State::Running:
	case Player_State::Reversing:
	{
		float tween = time_into_frame / run_frames[(int)run_state].duration;
		g_depth_texture.enableTween(tween);

		const unsigned int mat_count = run_frames[(int)run_state].model.getMaterialCount();
		for (unsigned int i = 0; i < mat_count; i++)
		{
			const unsigned int mesh_count = run_frames[(int)run_state].model.getMeshCount(i);
			for (unsigned int j = 0; j < mesh_count; j++)
			{
				const MeshWithShader& m = run_frames[(int)run_state].model.getMesh(i, j);
				m.draw();
			}
		}
		g_depth_texture.disableTween();
		break;
	}
	case Player_State::Standing:
	{
		const unsigned int mat_count = stand_model.getMaterialCount();
		for (unsigned int i = 0; i < mat_count; i++)
		{
			const unsigned int mesh_count = stand_model.getMeshCount(i);
			for (unsigned int j = 0; j < mesh_count; j++)
			{
				const MeshWithShader& m = stand_model.getMesh(i, j);
				m.draw();
			}
		}
		break;
	}
	case Player_State::Jumping:
	case Player_State::Falling:
	{
		const unsigned int mat_count = jump_model.getMaterialCount();
		for (unsigned int i = 0; i < mat_count; i++)
		{
			const unsigned int mesh_count = jump_model.getMeshCount(i);
			for (unsigned int j = 0; j < mesh_count; j++)
			{
				const MeshWithShader& m = jump_model.getMesh(i, j);
				m.draw();
			}
		}
		break;
	}

	default:;
	}
}

void PlayerAnimatedModel::transitionTo(Player_State new_state)
{
	if (state == new_state) return;

	switch (new_state)
	{
	case Player_State::Strafing:
	case Player_State::Running:
		if (state != Player_State::Strafing && state != Player_State::Running)
		{
			time_into_frame = 0;
			run_state = Run_State::Start;
		}
		break;
	case Player_State::Reversing:
		run_state = Run_State::StartReverse;
		int i = int(run_state);
		time_into_frame = run_frames[i].duration;
		break;
	}

	switch (new_state)
	{
	case Player_State::Reversing:
		animation_rate = 0.6f;
		break;
	case Player_State::Strafing:
		animation_rate = 0.6f;
		break;
	case Player_State::Running:
		animation_rate = 1.0f;
		break;
	}
	state = new_state;
}
