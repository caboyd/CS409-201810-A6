#include "Game.h"
#include "lib/ObjLibrary/LightingManager.h"
#include "LineRenderer.h"
#include "TextRenderer.h"
#include "WindowsHelperFunctions.h"
#include "Globals.h"

//Matrix to help with calculating depth texture
const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


 void Game::init()
{
	player_camera.setPosition(PLAYER_CAMERA_INIT_POS);
	player_camera.setOrientation(CAMERA_INIT_FORWARD);

	overview_camera.setPosition(OVERVIEW_CAMERA_INIT_POS);
	overview_camera.setOrientation(Vector3(0.0f, -1.0f, 0.0f));

	active_camera = &player_camera;

	player.init();

	ObjModel temp;
	temp.load("assets/Models/Skybox.obj");
	skybox_model = temp.getModelWithShader();

	//Folder location of worlds
	world_folder = "assets/Worlds/";

#ifdef  _WIN32
	//Load the file names of all of the worlds
	levels = getFileNamesInFolder(world_folder);
	for (auto const& file : levels)
		std::cout << "File name:" << file << std::endl;
	std::cout << "Press [Tab] to cycle worlds" << std::endl;
#endif

	//The world that is loaded first
	world.init(world_folder + "Basic.txt");


	//Initialize for shadows
	//Pass the light view matrix to the shadow box
	light_view_matrix = glm::mat4();
	shadow_box.init(&light_view_matrix, &active_camera);
}

   void Game::update(double fixed_delta_time)
{
	//Multiply player speed/turn by scaled time.
	float player_distance = float(PLAYER_SPEED * fixed_delta_time);
	float player_turn = float(TURNING_DEGREES * fixed_delta_time);

	//Shift moves/turns faster
	if (key_pressed[KEY_SHIFT])
	{
		player_distance *= PLAYER_SPEED_BOOST_MULT;
		player_turn *= PLAYER_TURN_BOOST_MULT;
	}

	//save old player values for calculations
	const Vector3 last_player_pos = player.coordinate_system.getPosition();
	const Vector3 last_player_forward = player.coordinate_system.getForward();

	//Use vectors to calculate correct diagonal movement
	Vector2 player_direction(0.0, 0.0);

	if (key_pressed['D'])
		player_direction.x += 1.0;
	if (key_pressed['A'])
		player_direction.x -= 1.0;
	if (key_pressed['W'] || (key_pressed[MOUSE_LEFT] && key_pressed[MOUSE_RIGHT]) || key_pressed[KEY_UP_ARROW])
		player_direction.y += 1.0;
	if (key_pressed['S'] || key_pressed[KEY_DOWN_ARROW])
		player_direction.y -= 1.0;

	//If player is moving diagonally then multiply by sqrt(2)/2
	if (player_direction.getNorm() > 1.0)
		player_distance *= float(MathHelper::M_SQRT2_2);

	//move player
	if (key_pressed['D'])
		player.coordinate_system.moveRight(player_distance);
	if (key_pressed['A'])
		player.coordinate_system.moveRight(-player_distance);
	if (key_pressed['W'] || (key_pressed[MOUSE_LEFT] && key_pressed[MOUSE_RIGHT]) || key_pressed[KEY_UP_ARROW])
		player.coordinate_system.moveForward(player_distance);
	if (key_pressed['S'] || key_pressed[KEY_DOWN_ARROW])
		player.coordinate_system.moveForward(-player_distance);

	//Rotate player
	if (key_pressed[KEY_LEFT_ARROW])
		player.coordinate_system.rotateAroundUp(player_turn);
	if (key_pressed[KEY_RIGHT_ARROW])
		player.coordinate_system.rotateAroundUp(-player_turn);

	//Reset player position and orientation to defaults
	if (key_pressed['R'])
	{
		player.resetPosition();
	}

	//Change to overview camera when 'O' held
	if (key_pressed['O'])
		active_camera = &overview_camera;
	else
		active_camera = &player_camera;




	//Set the players height to height of the world
	const Vector3 player_position = player.coordinate_system.getPosition();
	const float y = world.getHeightAtCirclePosition(float(player_position.x), float(player_position.z), 0.25f);
	player.setPosition(Vector3(player_position.x, y, player_position.z));

	//Updates the rings in the world
	world.update(fixed_delta_time);

	//Do collision detection for player and rings/rods
	world.pickupManager.checkForPickups(player.coordinate_system.getPosition());


	//CAMERA STUFF
	//*********************

	bool player_moved = false;


	//If player is moving/turning then lock the camera behind the player by turning camera_float off
	if (player.coordinate_system.getPosition() != last_player_pos)
	{
		player_moved = true;
		camera_float = false;
	}

	if (player.coordinate_system.getForward() != last_player_forward)
		camera_float = false;


	//Arcball rotate around the player on left click
	if (key_pressed[MOUSE_LEFT] && !key_pressed[MOUSE_RIGHT] && !player_moved)
	{
		const double angle_y = player_camera.getForward().getAngleNormal(player_camera.getUp());

		if (angle_y > MathHelper::M_PI * 0.92)
			if (mouse_dy > 0) mouse_dy = 0;
		if (angle_y < MathHelper::M_PI * 0.08)
			if (mouse_dy < 0) mouse_dy = 0;

		const Vector3 target = player.coordinate_system.getPosition() + PLAYER_CAMERA_OFFSET;
		player_camera.rotateAroundTarget(target, glm::radians(double(-mouse_dx)), glm::radians(double(-mouse_dy)));
		player_camera.setOrientation(player_camera.getForward().getNormalized(), Vector3(0, 1, 0));
		camera_float = true;
	}


	//Glides the camera back behind the player when left click is released.
	if (!key_pressed[MOUSE_LEFT])
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
	if (key_pressed[MOUSE_RIGHT])
	{
		player.coordinate_system.rotateAroundUp(-glm::radians(float(mouse_dx)) * time_scale);
		camera_float = false;
	}

	//Once mouse input has been used we need to set the dx and dy back to zero
	mouse_dx = 0;
	mouse_dy = 0;

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

		if (!player_moved)
	{
		player.transitionAnimationTo(Player_State::Standing);
	} else if (player_direction.y > 0 || player_direction.x != 0)
	{
		player.transitionAnimationTo(Player_State::Running);
	} else if (player_direction.y < 0)
	{
		player.transitionAnimationTo(Player_State::Reversing);
	}
	if (key_pressed[32])
	{
		player.transitionAnimationTo(Player_State::Jumping);
	}

}

 void Game::updateAnimations(double delta_time)
{
	player.updateAnimation(delta_time);
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
	LightingManager::setShadowMap(depth_texture.getTexture());
	LightingManager::setShadowMapSpaceMatrix(shadow_map_space_matrix);


	const Vector3& player_forward = player.coordinate_system.getForward();
	const Vector3& player_position = player.coordinate_system.getPosition();
	const Vector3& camera_position = active_camera->getPosition();


	//************Render to the screen with shadows and lighting*************
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, win_width, win_height);

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
	glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

	glDepthMask(GL_FALSE);
	skybox_model.draw(model_matrix, view_matrix, mvp_matrix, camera_position);
	glDepthMask(GL_TRUE);

	//Draw the world
	world.drawOptimized(view_matrix, projection_matrix, camera_position);
	//world.draw(view_matrix, projection_matrix, active_camera->getPosition());


	const std::string text = "Score: " + std::to_string(world.pickupManager.score);
	const float text_width = text_renderer.getWidth(text, 0.75f);
	text_renderer.draw(text, float(win_width - text_width - 10), float(win_height - 40), 0.75f, glm::vec3(1, 1, 1));


	player.draw(view_matrix, projection_matrix, camera_position);

	text_renderer.draw("Update Rate: " + std::to_string(long(update_fps)), 2, float(win_height - 24), 0.4f, glm::vec3(0, 1, 0));
	text_renderer.draw("Display Rate: " + std::to_string(long(display_fps)), 2, float(win_height - 44), 0.4f, glm::vec3(0, 1, 0));

	//Render depth texture to screen - **Changes required to shader and Depth Texture to work
	//depth_texture.renderDepthTextureToQuad(0, 0, 512, 512);

	// send the current image to the screen - any drawing after here will not display
}

 void Game::destroyIntoNextWorld()
{
	world.destroy();
	world.init(world_folder + levels[level++]);
	if (level >= levels.size()) level = 0;
}


void Game::renderToDepthTexture(glm::mat4& depth_vp)
{

	depth_texture.startRenderToDepthTexture();
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

	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(player_position));

	model_matrix = glm::rotate(model_matrix, (float(atan2(player_forward.x, player_forward.z)) - float(MathHelper::M_PI_2)), glm::vec3(player.coordinate_system.getUp()));;
	glm::mat4 depth_mvp = depth_vp * model_matrix;

	depth_texture.setDepthMVP(depth_mvp);


	player.drawToDepth();



	//Draw the world to the depth texture
	world.drawDepth(depth_vp);





}


