#include "Game.h"
#include "lib/ObjLibrary/LightingManager.h"
#include "LineRenderer.h"
#include "TextRenderer.h"
#include "WindowsHelperFunctions.h"
#include "Globals.h"
#include "MathHelper.h"

//Matrix to help with calculating depth texture
const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


void Game::init()
{
	ObjShader::load();

	ObjModel model;
	model.load(ROD_FILENAME);
	this->rod_model = model.getModelWithShader();
	model.load(RING_FILENAME);
	this->ring_model = model.getModelWithShader();
	model.load(SKYBOX_FILENAME);
	skybox_model = model.getModelWithShader();

#ifdef  _WIN32
	//Load the file names of all of the worlds
	levels = getFileNamesInFolder(WORLD_FOLDER);
	for (auto const& file : levels)
		std::cout << "File name:" << file << std::endl;
	std::cout << "Press [Tab] to cycle worlds" << std::endl;
#endif

	//The world that is loaded first
	world.init(WORLD_FOLDER + "Basic.txt");
	//world.init(WORLD_FOLDER + "Dense.txt");
	//world.init(WORLD_FOLDER + "Icy.txt");
	//world.init(WORLD_FOLDER + "Leafy.txt");
	//world.init(WORLD_FOLDER + "Rocky.txt");
	//world.init(WORLD_FOLDER + "Sandy.txt");
	//world.init(WORLD_FOLDER + "Simple.txt");
	//world.init(WORLD_FOLDER + "Small.txt");
	//world.init(WORLD_FOLDER + "Sparse.txt");
	//world.init(WORLD_FOLDER + "Twisted.txt");
	world_graph.init(world);


	player_camera.setPosition(PLAYER_CAMERA_INIT_POS);
	player_camera.setOrientation(CAMERA_INIT_FORWARD);

	overview_camera.setPosition(OVERVIEW_CAMERA_INIT_POS);
	//overview_camera.setOrientation(Vector3(0.0f, -1.0f, 0.0f));

	active_camera = &player_camera;

	pickup_manager.init(&world, &rod_model, &ring_model);
	player.init(world);
	player.coordinate_system.setPosition(world.disks[0]->position);

	//Initialize for shadows
	//Pass the light view matrix to the shadow box
	light_view_matrix = glm::mat4();
	shadow_box.init(&light_view_matrix, &active_camera);

	//Enabled lighting and shadows
	LightingManager::setEnabled(true);
	LightingManager::setShadowEnabled(true);
	LightingManager::setShadowDistance(shadow_box.getShadowDistance());

	//Set the shadow light to be a direction light where the sun is
	LightingManager::setLightEnabled(0, true);
	LightingManager::setLightDirectional(0, SUN_DIR);
	LightingManager::setLightColour(0, Vector3(1.0, 1.0, 1.0));

	//Set an ambient light with full strength
	LightingManager::setLightEnabled(2, true);
	LightingManager::setLightColour(2, Vector3(1.0, 1.0, 1.0));

	//Set a positional light at the players location
	//LightingManager::setLightEnabled(1, true);
	//LightingManager::setLightPositional(1, PLAYER_INIT_POS);
	//LightingManager::setLightColour(1, V3(0.5, 0.5, 0.5));
	//LightingManager::setLightAttenuation(1, 1.5f, 0.09f, 0.032f);


}

void Game::update(double fixed_delta_time)
{
	const float delta_time = float(fixed_delta_time);
	const float delta_time_seconds = float(fixed_delta_time / 1000.0f);

	//PLAYER UPDATE ************************************
	//save old player values for calculations
	bool player_moved = false;
	const Vector3 last_player_pos = player.coordinate_system.getPosition();
	const Vector3 last_player_forward = player.coordinate_system.getForward();

	//move player
	if (g_key_pressed['D'])
		playerAccelerateRight(delta_time_seconds);
	if (g_key_pressed['A'])
		playerAccelerateLeft(delta_time_seconds);
	if (g_key_pressed['W'] || (g_key_pressed[MOUSE_LEFT] && g_key_pressed[MOUSE_RIGHT]) || g_key_pressed[KEY_UP_ARROW])
		playerAccelerateForward(delta_time_seconds);
	if (g_key_pressed['S'] || g_key_pressed[KEY_DOWN_ARROW])
		playerAccelerateBackward(delta_time_seconds);

	//Rotate player
	if (g_key_pressed[KEY_LEFT_ARROW])
		playerTurnLeft(delta_time_seconds);
	if (g_key_pressed[KEY_RIGHT_ARROW])
		playerTurnRight(delta_time_seconds);

	//Space bar
	if (g_key_pressed[32])
		player.jump();

	player.update(world, delta_time_seconds);


	//Reset player position and orientation to defaults
	if (g_key_pressed['R'])
	{
		player.reset(world);
	}


	//Update the position of the rings
	pickup_manager.update(fixed_delta_time);

	//Do collision detection for player and rings/rods
	pickup_manager.checkForPickups(player.coordinate_system.getPosition());

	//If player is moving/turning then lock the camera behind the player by turning camera_float off
	if (player.coordinate_system.getPosition() != last_player_pos)
	{
		player_moved = true;
		camera_float = false;
	}

	if (player.coordinate_system.getForward() != last_player_forward)
		camera_float = false;

	const Vector3& player_forward = player.coordinate_system.getForward();

	if (g_key_pressed['W'] || (g_key_pressed[MOUSE_LEFT] && g_key_pressed[MOUSE_RIGHT]) || g_key_pressed[KEY_UP_ARROW])
	{
		player.transitionAnimationTo(Player_State::Running);
	} else	if (g_key_pressed['D'] || g_key_pressed['A'])
	{
		player.transitionAnimationTo(Player_State::Strafing);
	} else if (g_key_pressed['S'] || g_key_pressed[KEY_DOWN_ARROW])
	{
		player.transitionAnimationTo(Player_State::Reversing);
	} else
	{
		player.transitionAnimationTo(Player_State::Standing);
	}


	//CAMERA STUFF
	//******************************


	//Change to overview camera when 'O' held
	if (g_key_pressed['O'])
		active_camera = &overview_camera;
	else
		active_camera = &player_camera;

	//Arcball rotate around the player on left click
	if (g_key_pressed[MOUSE_LEFT] && !g_key_pressed[MOUSE_RIGHT] && player.isStanding())
	{
		const double angle_y = player_camera.getForward().getAngleNormal(player_camera.getUp());

		if (angle_y > MathHelper::M_PI * 0.92)
			if (g_mouse_dy > 0) g_mouse_dy = 0;
		if (angle_y < MathHelper::M_PI * 0.08)
			if (g_mouse_dy < 0) g_mouse_dy = 0;

		const Vector3 target = player.coordinate_system.getPosition() + PLAYER_CAMERA_OFFSET;
		player_camera.rotateAroundTarget(target, glm::radians(double(-g_mouse_dx)), glm::radians(double(-g_mouse_dy)));
		player_camera.setOrientation(player_camera.getForward().getNormalized(), Vector3(0, 1, 0));
		camera_float = true;
	}


	//Glides the camera back behind the player when left click is released.
	if (!g_key_pressed[MOUSE_LEFT])
	{
		Vector3 new_cam_position = player.coordinate_system.getPosition();
		const Vector3& player_forward = player.coordinate_system.getForward();

		new_cam_position.x -= 4.0f * player_forward.x;
		new_cam_position.z -= 4.0f * player_forward.z;
		new_cam_position += PLAYER_CAMERA_OFFSET;

		Vector3 diff = player_camera.getPosition() - new_cam_position;
		const double norm = diff.getNorm();

		//We want the camera to move faster if farther but to not too fast when far
		//or too slow when near
		if (norm >= 0.005f && camera_float)
		{
			double log_factor = (log(norm + 1) + 1) * (fixed_delta_time / 10.0);
			//Caps the speed
			if (log_factor > 10.0) log_factor = 10.0;

			const Vector3 target = player.coordinate_system.getPosition() + PLAYER_CAMERA_OFFSET;
			player_camera.rotateAroundTargetToPosition(target, new_cam_position, glm::radians(log_factor*0.5));
		} else
		{
			//If camera is near the origin point we can snap it back.
			camera_float = false;
		}
	}

	//Rotate the player left/right when mouse right is down
	if (g_key_pressed[MOUSE_RIGHT])
	{
		player.coordinate_system.rotateAroundUp(-glm::radians(float(g_mouse_dx)) * g_time_scale);
		camera_float = false;
	}

	//Once mouse input has been used we need to set the dx and dy back to zero
	g_mouse_dx = 0;
	g_mouse_dy = 0;

	//If camera is not in free float around player, snap it back behind the player
	if (!camera_float)
	{
		Vector3 new_cam_position = player.coordinate_system.getPosition();
		const Vector3& player_forward = player.coordinate_system.getForward();
		player_camera.setOrientation(player_forward, Vector3(0, 1, 0));
		new_cam_position.x -= 4.0f * player_forward.x;
		new_cam_position.z -= 4.0f * player_forward.z;
		new_cam_position += PLAYER_CAMERA_OFFSET;
		player_camera.setPosition(new_cam_position);
	}

	//Overview camera follows player position
	overview_camera.lookAt(player.coordinate_system.getPosition());



}

void Game::updateAnimations(double delta_time)
{
	player.updateAnimation(delta_time);
}

void Game::displayMovementGraph(const glm::mat4x4& view_matrix,
	const glm::mat4x4& projection_matrix) const
{
	glDisable(GL_DEPTH_TEST);
	Vector3 offset(0, 2.0, 0);
	glm::mat4x4 vp_matrix = projection_matrix * view_matrix;
	for (auto &node : world_graph.node_list)
	{
		for (auto &link : node.node_links)
		{
			g_line_renderer.draw(node.position + offset, world_graph.node_list[link.node_id].position + offset, glm::vec4(0.25 + link.weight / 50.0f, 1.0f - link.weight / 150.0f, 0.0f, 1.0f), vp_matrix);
		}
	}
	glEnable(GL_DEPTH_TEST);
}

void Game::display()
{
	//**************Render to depth texture ***************
	glm::mat4 depth_vp;
	renderToDepthTexture(depth_vp);

	//**********Done Rendering to Depth Texture ********************

	//Set up for lighting manager to use depth texture for shadows

	//Multiply shadow map matrix by bias matrix
	//	This is to map the shadow texture coordinates to screen coordinates
	glm::mat4x4 shadow_map_space_matrix = BIAS_MATRIX * depth_vp;

	//Set the shadow map and space matrix
	LightingManager::setShadowMap(g_depth_texture.getTexture());
	LightingManager::setShadowMapSpaceMatrix(shadow_map_space_matrix);

	const Vector3& camera_position = active_camera->getPosition();

	//************Render to the screen with shadows and lighting*************
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, g_win_width, g_win_height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Positional light at player position
	LightingManager::setLightPositional(1, player.coordinate_system.getPosition());

	// Get the current camera view matrix
	glm::mat4 view_matrix = active_camera->getViewMatrix();


	//Draw the skybox
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(camera_position));
	glm::mat4 mvp_matrix = g_projection_matrix * view_matrix * model_matrix;

	glDepthMask(GL_FALSE);
	skybox_model.draw(model_matrix, view_matrix, mvp_matrix, camera_position);
	glDepthMask(GL_TRUE);

	//Draw the world
	world.drawOptimized(view_matrix, g_projection_matrix, camera_position);
	pickup_manager.drawOptimized(view_matrix, g_projection_matrix, camera_position);
	//world.draw(view_matrix, projection_matrix, active_camera->getPosition());


	const std::string text = "Score: " + std::to_string(pickup_manager.getScore());
	const float text_width = g_text_renderer.getWidth(text, 0.75f);
	g_text_renderer.draw(text, float(g_win_width - text_width - 10), float(g_win_height - 40), 0.75f, glm::vec3(1, 1, 1));


	player.draw(view_matrix, g_projection_matrix, camera_position);

	displayMovementGraph(view_matrix, g_projection_matrix);

	g_text_renderer.draw("Update Rate: " + std::to_string(long(g_update_fps + 0.5f)), 2, float(g_win_height - 24), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Display Rate: " + std::to_string(long(g_display_fps + 0.5f)), 2, float(g_win_height - 44), 0.4f, glm::vec3(0, 1, 0));

	int node_links = 0;
	int nodes = world_graph.node_list.size();
	for (auto node_list : world_graph.node_list)
		node_links += node_list.node_links.size();


	g_text_renderer.draw("Nodes: " + std::to_string(nodes), 2, float(g_win_height - 64), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Node Links: " + std::to_string(node_links), 2, float(g_win_height - 84), 0.4f, glm::vec3(0, 1, 0));


	static unsigned count = 0;
	static unsigned count2 = 0;
	static unsigned mem_used = 0;
	static bool mem_done = false;
	count++;
	if (count > world_graph.node_list.size() - 1){
		count = 0;
		count2++;
	}
	if(count2 > world_graph.node_list.size() - 1){
		count2 = 0;
		mem_done = true;
	}
	std::deque<unsigned> d = world_graph.dijkstraSearch(count2, count);
	if(!mem_done)
	{
		mem_used += d.size() * 4;
	}
	g_text_renderer.draw("Paths Completed : " + std::to_string(world_graph.node_list.size()*count2 + count) + " / " + std::to_string(world_graph.node_list.size()*world_graph.node_list.size()), 2, float(g_win_height - 104), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Mem Used for path: " + std::to_string(mem_used) + " bytes", 2, float(g_win_height - 124), 0.4f, glm::vec3(0, 1, 0));

	glm::vec4 viewport = glm::vec4(0, 0, g_win_width, g_win_height);
	for (auto& node : world_graph.node_list)
	{
		//Dont draw numbers behind camera
		if (active_camera->getForward().dotProduct(active_camera->getPosition() - node.position) <= 0)
		{
			glm::mat4 model = glm::mat4();
			glm::translate(model, glm::vec3(node.position));
			model *= view_matrix;
			Vector3 a = node.position + ((world.disks[node.disk_id]->position - node.position).getNormalized() * 2);
			glm::vec3 coord = glm::project(glm::vec3(a), model, g_projection_matrix, viewport);

			g_text_renderer.draw(std::to_string(node.node_id), coord.x, coord.y, 0.3f, glm::vec3(0.5, 1, 1));
		}
		//for (auto i : d)
		//{
		//	if (i == node.node_id)
		//	{
		//		glm::mat4 model = glm::mat4();
		//		glm::translate(model, glm::vec3(node.position));
		//		model *= view_matrix;

		//		glm::vec3 coord = glm::project(glm::vec3(node.position), model, g_projection_matrix, viewport);

		//		g_text_renderer.draw("*", coord.x, coord.y, 0.5f, glm::vec3(0.1, 1, 0));
		//	}
		//}
	}

	Vector3 offset(0, 3.0, 0);
	glm::mat4 vp = g_projection_matrix * view_matrix;
	if (!d.empty())
		for (unsigned i = 0; i < d.size() - 1; i++)
		{
			g_line_renderer.draw(world_graph.node_list[d[i]].position + offset, world_graph.node_list[d[i + 1]].position + offset, glm::vec4(1, 1, 1, 1), vp);
		}

	//Render depth texture to screen - **Changes required to shader and Depth Texture to work
	//depth_texture.renderDepthTextureToQuad(0, 0, 512, 512);


	// send the current image to the screen - any drawing after here will not display
}

void Game::destroyIntoNextWorld()
{
#ifdef  _WIN32

	world.destroy();
	world_graph.destroy();
	world.init(WORLD_FOLDER + levels[level++]);
	world_graph.init(world);
	if (level >= levels.size()) level = 0;
	pickup_manager.destroy();
	pickup_manager.init(&world, &rod_model, &ring_model);
#endif
}


void Game::renderToDepthTexture(glm::mat4& depth_vp)
{

	g_depth_texture.startRenderToDepthTexture();
	glDisable(GL_CULL_FACE);

	const Vector3& player_forward = player.coordinate_system.getForward();
	const Vector3&player_position = player.coordinate_system.getPosition();

	//Update the shadow box to re orient the location of the shadow view
	shadow_box.update();

	//Create an orthogonal projection matrix for the depth
	glm::mat4x4 depthProjectionMatrix;
	depthProjectionMatrix[0][0] = 2.0f / shadow_box.getWidth();
	depthProjectionMatrix[1][1] = 2.0f / shadow_box.getHeight();
	depthProjectionMatrix[2][2] = -2.0f / shadow_box.getLength();
	depthProjectionMatrix[3][3] = 1;


	const Vector3 center = -shadow_box.getCenter();

	//Calculate the light view matrix which is what the light views
	const Vector3 light_inverse_dir = -SUN_DIR.getNormalized();

	//Create the light_view_matrix. It is the view in the direction of the light
	light_view_matrix = glm::mat4();
	const float pitch = float(acos(Vector2(light_inverse_dir.x, light_inverse_dir.z).getNorm()));
	light_view_matrix = glm::rotate(light_view_matrix, pitch, glm::vec3(1, 0, 0));
	float yaw = float(atan2(light_inverse_dir.x, light_inverse_dir.z));

	yaw = light_inverse_dir.z > 0 ? yaw - float(MathHelper::M_PI) : yaw;
	light_view_matrix = glm::rotate(light_view_matrix, -yaw + float(MathHelper::M_PI), glm::vec3(0, 1, 0));
	light_view_matrix = glm::translate(light_view_matrix, glm::vec3(center.x, center.y, center.z));

	depth_vp = depthProjectionMatrix * light_view_matrix;

	player.drawToDepth(depth_vp);

	//Draw the world to the depth texture
	world.drawDepth(depth_vp);
	pickup_manager.drawDepth(depth_vp);

}

void Game::playerAccelerateForward(float delta_time)
{
	const Vector3& pos = player.coordinate_system.getPosition();
	const Vector3& forward = player.coordinate_system.getForward();
	Vector3 accel = forward * PLAYER_ACCEL_FORWARD * world.getAccelFactorAtPosition(float(pos.x), float(pos.z));
	accel *= delta_time;
	player.addAcceleration(accel);

}

void Game::playerAccelerateBackward(float delta_time)
{
	const Vector3& pos = player.coordinate_system.getPosition();
	const Vector3& backward = -player.coordinate_system.getForward();
	Vector3 accel = backward * PLAYER_ACCEL * world.getSpeedFactorAtPosition(float(pos.x), float(pos.z));
	accel *= delta_time;
	player.addAcceleration(accel);
}

void Game::playerAccelerateLeft(float delta_time)
{
	const Vector3& pos = player.coordinate_system.getPosition();
	const Vector3& left = -player.coordinate_system.getRight();
	Vector3 accel = left * PLAYER_ACCEL * world.getSpeedFactorAtPosition(float(pos.x), float(pos.z));
	accel *= delta_time;
	player.addAcceleration(accel);
}

void Game::playerAccelerateRight(float delta_time)
{
	const Vector3& pos = player.coordinate_system.getPosition();
	const Vector3& right = player.coordinate_system.getRight();
	Vector3 accel = right * PLAYER_ACCEL * world.getSpeedFactorAtPosition(float(pos.x), float(pos.z));
	accel *= delta_time;
	player.addAcceleration(accel);
}

void Game::playerTurnLeft(float delta_time)
{
	const float player_turn = float(TURNING_DEGREES * delta_time);
	player.coordinate_system.rotateAroundUp(player_turn);
}


void Game::playerTurnRight(float delta_time)
{
	const float player_turn = float(TURNING_DEGREES * delta_time);
	player.coordinate_system.rotateAroundUp(-player_turn);
}




