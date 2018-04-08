#include "Game.h"
#include "lib/ObjLibrary/LightingManager.h"
#include "LineRenderer.h"
#include "TextRenderer.h"
#include "WindowsHelperFunctions.h"
#include "Globals.h"
#include "MathHelper.h"
#include "Ring.h"
#include "ParticleEmitter.h"

//Matrix to help with calculating depth texture
const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


void Game::initWorldGraphPointLine()
{
	const glm::vec3 offset(0, 0.2, 0);
	world_graph_point_line.clear();
	world_graph_point_line.reserve(world_graph.getNodeLinkCount() * 2);
	for (auto &node : world_graph.getNodeList())
	{
		for (auto &link : node.node_links)
		{
			glm::vec4 color(0.25 + link.weight / 50.0f, 1.0f - link.weight / 150.0f, 0.0f, 1.0f);
			world_graph_point_line.emplace_back(node.position + offset, color);
			world_graph_point_line.emplace_back(world_graph.getNodeList()[link.dest_node_id].position + offset, color);

		}
	}
}

void Game::initBats()
{
	for (auto && disk : world.disks)
	{
		Vector3 pos = world.getRandomXZPosition();
		pos.y = 15.0f;
		bats.emplace_back(bat_model, player, world, pos);
	}
}


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
	model.load(BAT_FILENAME);
	bat_model = model.getModelWithShader();

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
	world_graph.init(world.disks);


	initWorldGraphPointLine();

	player_camera.setPosition(PLAYER_CAMERA_INIT_POS);
	player_camera.setOrientation(CAMERA_INIT_FORWARD);

	overview_camera.setPosition(OVERVIEW_CAMERA_INIT_POS);
	//overview_camera.setOrientation(Vector3(0.0f, -1.0f, 0.0f));

	active_camera = &player_camera;

	pickup_manager.init(world, world_graph, rod_model, ring_model);
	player.init(world);

	initBats();

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


	const Vector3 forward_vector = active_camera->getForward();
	const Vector3 up_vector = active_camera->getUp();
	const Vector3 to_far = forward_vector * shadow_box.SHADOW_DISTANCE;
	const Vector3 to_near = forward_vector * (shadow_box.CLIP_NEAR);

	const Vector3 center_near = to_near + active_camera->getPosition();
	const Vector3 center_far = to_far + active_camera->getPosition();

	glm::vec4* points = shadow_box.calculateFrustumVertices(up_vector, forward_vector, center_near,
		center_far);
	shadow_box_points = shadow_box.calculateFrustumVertices(up_vector, forward_vector, center_near, center_far);


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

	//Update the player
	player.update(world, delta_time_seconds);

	for (auto && bat : bats)
	{
		bat.update(delta_time_seconds);
	}

	batPlayerCollisions();

	//Reset player position and orientation to defaults
	if (g_key_pressed['R'])
	{
		player.reset(world);
	}


	//Update the position of the rings
	pickup_manager.update(fixed_delta_time);

	//Do collision detection for player and rings/rods
	pickup_manager.checkForPickups(player);

	g_particle_emitter.update(fixed_delta_time, active_camera->getPosition());

	//If player is moving/turning then lock the camera behind the player by turning camera_float off
	if (player.coordinate_system.getPosition() != last_player_pos)
	{
		camera_float = false;
	}

	if (player.coordinate_system.getForward() != last_player_forward)
		camera_float = false;

	//Update the animation states for the player
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
	//skybox_model.draw(model_matrix, view_matrix, mvp_matrix, camera_position);
	glDepthMask(GL_TRUE);


	//Draw the world
	world.drawOptimized(view_matrix, g_projection_matrix, camera_position);
	pickup_manager.drawOptimized(view_matrix, g_projection_matrix, camera_position);


	displayBats(view_matrix, g_projection_matrix, camera_position);

	player.draw(view_matrix, g_projection_matrix, camera_position);

	g_particle_emitter.draw(view_matrix,g_projection_matrix);

	const std::string text = "Score: " + std::to_string(pickup_manager.getScore());
	const float text_width = g_text_renderer.getWidth(text, 0.75f);
	//g_text_renderer.draw(text, float(g_win_width - text_width - 10), float(g_win_height - 40), 0.75f, glm::vec3(1, 1, 1));

	displayMovementGraph(view_matrix, g_projection_matrix);

	//Draw the node ids
	//displayNodeNameplates(view_matrix, g_projection_matrix);

	//Draw the Search path for overview camera
	displaySearchPathSpheres(view_matrix, g_projection_matrix);

	//Draw the line of Ring0 from current position to end node
	displayRingZeroPath(view_matrix, g_projection_matrix);


	//Draw useful text on the screen
	const unsigned a_visits = world_graph.getMemorizedAStarVisits(),
		d_visits = world_graph.getMemorizedDijsktraVisits(),
		mm_visits = world_graph.getMemorizedmmVisits();

	g_text_renderer.draw("Update Rate: " + std::to_string(long(round(g_update_fps))), 2, float(g_win_height - 24), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Display Rate: " + std::to_string(long(round(g_display_fps))), 2, float(g_win_height - 44), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Time Scale: " + realToString(g_time_scale, 2) + 'x', 2, float(g_win_height - 64), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Nodes: " + std::to_string(world_graph.getNodeCount()), 2, float(g_win_height - 84), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Node Links: " + std::to_string(world_graph.getNodeLinkCount()), 2, float(g_win_height - 104), 0.4f, glm::vec3(0, 1, 0));


	//Number of node visits for the ring0 path seach per algorithm
	g_text_renderer.draw("Dijkstra Visits: " + std::to_string(d_visits) + " 100%", 2,
		float(g_win_height - 124), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("A* Visits: " + std::to_string(world_graph.getMemorizedAStarVisits()) + " " + realToString(100 * float(a_visits) / float(d_visits), 2) + "%",
		2, float(g_win_height - 144), 0.4f, glm::vec3(0, 1, 0));
	g_text_renderer.draw("Meet in the middle Visits: " + std::to_string(mm_visits) + " " + realToString(100 * float(mm_visits) / float(d_visits), 2) + "%",
		2, float(g_win_height - 164), 0.4f, glm::vec3(0, 1, 0));

	//Render depth texture to screen - **Changes required to shader and Depth Texture to work
	//g_depth_texture.renderDepthTextureToQuad(0, 0, 128, 128);

}

void Game::displayBats(const glm::mat4x4& view_matrix,
	const glm::mat4x4& projection_matrix,
	const glm::vec3& camera_position)
{
	const ObjLibrary::ObjShader::ShaderUniforms& uniforms = ObjLibrary::ObjShader::activateShader();
	bat_model.getMaterial(0).activate(uniforms);;

	const  glm::mat4 vp_matrix = projection_matrix * view_matrix;

	glUniformMatrix4fv(uniforms.m_view_matrix, 1, false, &(view_matrix[0][0]));
	glUniform3fv(uniforms.m_camera_pos, 1, &(camera_position.x));


	for (auto const& bat : bats)
	{
		glm::mat4 model_matrix = glm::mat4();
		model_matrix = translate(model_matrix, glm::vec3(bat.coordinate_system.position));
		model_matrix = rotate(model_matrix, (float(atan2(bat.coordinate_system.forward.x, bat.coordinate_system.forward.z)) - float(MathHelper::M_PI_2)),
			glm::vec3(bat.coordinate_system.getUp()));
		model_matrix = glm::scale(model_matrix, glm::vec3(bat.model_scalar));
		const glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

		glUniformMatrix4fv(uniforms.m_model_matrix, 1, false, &(model_matrix[0][0]));
		glUniformMatrix4fv(uniforms.m_model_view_projection_matrix, 1, false, &(mvp_matrix[0][0]));
		bat_model.drawCurrentMaterial(0);
	}

}

void Game::displayBatsToDepthTexture(const glm::mat4& depth_view_projection_matrix)
{
	MeshWithShader mesh = bat_model.getMesh(0, 0);

	for (auto const& bat : bats)
	{
		glm::mat4 model_matrix = glm::mat4();
		model_matrix = translate(model_matrix, glm::vec3(bat.coordinate_system.position));
		model_matrix = rotate(model_matrix, (float(atan2(bat.coordinate_system.forward.x, bat.coordinate_system.forward.z)) - float(MathHelper::M_PI_2)),
			glm::vec3(bat.coordinate_system.getUp()));
		model_matrix = glm::scale(model_matrix, glm::vec3(bat.model_scalar));

		glm::mat4x4 depth_mvp = depth_view_projection_matrix * model_matrix;
		g_depth_texture.setDepthMVP(depth_mvp);
		mesh.draw();
	}
}


void Game::displayMovementGraph(const glm::mat4x4& view_matrix,
	const glm::mat4x4& projection_matrix) const
{

	if (active_camera == &overview_camera)
		glDisable(GL_DEPTH_TEST);

	glm::mat4 vp = projection_matrix * view_matrix;

	g_line_renderer.drawPointList(world_graph_point_line, vp);
	glEnable(GL_DEPTH_TEST);
}


void Game::displayRingZeroPath(const glm::mat4& view_matrix,
	const glm::mat4x4& projection_matrix) const
{
	//Draw the Path found in search
	std::deque<unsigned> path = pickup_manager.rings[0].path;
	const Ring& ring = pickup_manager.rings[0];
	const glm::vec3 offset(0, 1.0, 0);
	g_line_renderer.preAllocateLine(path.size() * 2 + 6);
	g_line_renderer.addLine(glm::vec3(ring.coordinate_system.position), glm::vec3(ring.coordinate_system.position + offset), glm::vec4(1, 1, 1, 1));
	g_line_renderer.addLine(glm::vec3(ring.coordinate_system.position + offset), glm::vec3(ring.targetPosition + offset), glm::vec4(1, 1, 1, 1));
	if (!path.empty())
	{
		g_line_renderer.addLine(
			glm::vec3(world_graph.getNodeList()[ring.target_node_id].position + offset),
			glm::vec3(world_graph.getNodeList()[path[0]].position + offset),
			glm::vec4(1, 1, 1, 1));
		for (unsigned i = 0; i < path.size() - 1; i++)
		{
			g_line_renderer.addLine(
				glm::vec3(world_graph.getNodeList()[path[i]].position + offset),
				glm::vec3(world_graph.getNodeList()[path[i + 1]].position + offset),
				glm::vec4(1, 1, 1, 1));
		}

	}
	glDisable(GL_DEPTH_TEST);
	const glm::mat4 vp = g_projection_matrix * view_matrix;
	g_line_renderer.drawLinesAndClear(vp);
	glEnable(GL_DEPTH_TEST);
}

void Game::displayNodeNameplates(const glm::mat4& view_matrix,
	const glm::mat4x4& projection_matrix) const
{
	const glm::vec4 viewport = glm::vec4(0, 0, g_win_width, g_win_height);
	for (auto& node : world_graph.getNodeList())
	{
		//Dont draw numbers behind camera
		if (active_camera->getForward().dotProduct(active_camera->getPosition() - node.position) <= 0)
		{
			glm::mat4 model = glm::mat4();
			glm::translate(model, glm::vec3(node.position));
			model *= view_matrix;
			Vector3 a = node.position + ((world.disks[node.disk_id]->position - node.position).getNormalized() * 2);
			const glm::vec3 coord = glm::project(glm::vec3(a), model, g_projection_matrix, viewport);

			g_text_renderer.draw(std::to_string(node.node_id), coord.x, coord.y, 0.3f, glm::vec3(0.5, 1, 1));
		}

	}
}

void Game::displaySearchPathSpheres(const glm::mat4& view_matrix,
	const glm::mat4x4& projection_matrix) const
{
	const std::vector<NodeSearchData>& search_data = world_graph.getMemorizedSearchData();
	unsigned start_node_id = 0;
	unsigned end_node_id = 0;
	unsigned meeting_node_id = 0;
	if (!search_data.empty())
		if (active_camera == &overview_camera)
		{
			//Collect data About the search to draw the spheres
			float highest_priority_start = 0;
			float lowest_priority_start = 99999999.0f;
			float highest_priority_end = 0;
			float lowest_priority_end = 99999999.0f;

			for (unsigned i = 0; i < search_data.size(); i++)
			{
				const SearchData& node_start_data = search_data[i].start;
				const SearchData& node_end_data = search_data[i].end;
				if (node_start_data.visited && node_end_data.visited) meeting_node_id = i;
				if (node_end_data.visited)
				{
					if (node_end_data.given_cost == 0) end_node_id = i;
					highest_priority_end = max(node_end_data.priority, highest_priority_end);
					lowest_priority_end = min(node_end_data.priority, lowest_priority_end);
				}
				if (node_start_data.visited)
				{
					if (node_start_data.given_cost == 0) start_node_id = i;
					highest_priority_start = max(node_start_data.priority, highest_priority_start);
					lowest_priority_start = min(node_start_data.priority, lowest_priority_start);
				}
			}
			highest_priority_start = search_data[meeting_node_id].start.priority;
			highest_priority_end = search_data[meeting_node_id].end.priority;
			//Loops through spheres on path and draws them with certaian color based on priority
			for (unsigned i = 0; i < search_data.size(); i++)
			{
				const SearchData& node_data_start = search_data[i].start;
				const SearchData& node_data_end = search_data[i].end;

				//If not visited dont draw them
				if (!node_data_end.visited && !node_data_start.visited)
					continue;

				Vector3 color;
				float scale = 1.5;

				//If on path make them bigger
				if (i == start_node_id || i == end_node_id || i == meeting_node_id)
					scale = 3.0;


				if (node_data_start.visited)
				{
					float color_frac = 1.0f - (node_data_start.priority - lowest_priority_start) / (highest_priority_start - lowest_priority_start) / 2.0f;
					color = Vector3(0, color_frac, 1.0);
				} else if (node_data_end.visited)
				{
					float color_frac = 1.0f - (node_data_end.priority - lowest_priority_end) / (highest_priority_end - lowest_priority_end);
					color = Vector3(1, color_frac, 1);
				}

				//Set thses colors for distinguished nodes
				if (i == start_node_id) color = Vector3(0, 1, 1);
				if (i == end_node_id)color = Vector3(1, 1, 1);
				if (i == meeting_node_id) color = Vector3(0.6, 0, 1);

				//Draw the sphere
				const Vector3& pos = world_graph.getNodeList()[i].position;
				glm::mat4 model = glm::mat4();
				model = glm::translate(model, glm::vec3(pos + Vector3(0, 0.2, 0)));
				model = glm::scale(model, glm::vec3(scale, scale, scale));
				const glm::mat4 mvp_matrix = g_projection_matrix * view_matrix * model;
				g_sphere_renderer.draw(glm::vec3(color), model, view_matrix, mvp_matrix);

			}
		}
}


void Game::renderToDepthTexture(glm::mat4& depth_vp)
{

	g_depth_texture.startRenderToDepthTexture();
	glDisable(GL_CULL_FACE);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(g_polygon_offset_factor, g_polygon_offset_units);


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
	float yaw = float(atan2(light_inverse_dir.z, light_inverse_dir.x));

	yaw = light_inverse_dir.z > 0 ? yaw - float(MathHelper::M_PI) : yaw;

	light_view_matrix = glm::rotate(light_view_matrix, yaw + float(MathHelper::M_PI_2), glm::vec3(0, 1, 0));
	light_view_matrix = glm::translate(light_view_matrix, glm::vec3(center.x, center.y, center.z));

	depth_vp = depthProjectionMatrix * light_view_matrix;

	player.drawToDepth(depth_vp);

	for (auto && bat : bats)
	{
		bat.drawToDepth(depth_vp);
	}

	//Draw the world to the depth texture
	world.drawDepthOptimized(player_position, shadow_box.getShadowFadeDistance(), depth_vp);
	pickup_manager.drawDepthOptimized(player_position, shadow_box.getShadowFadeDistance(), depth_vp);

	glDisable(GL_POLYGON_OFFSET_FILL);
}

void Game::destroyIntoNextWorld()
{
#ifdef  _WIN32

	bats.clear();
	world.destroy();
	world_graph.destroy();
	world.init(WORLD_FOLDER + levels[level++]);
	world_graph.init(world.disks);
	initWorldGraphPointLine();
	if (level >= levels.size()) level = 0;
	pickup_manager.destroy();
	pickup_manager.init(world, world_graph, rod_model, ring_model);
	initBats();
	player.reset(world);
#endif
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

void Game::batPlayerCollisions()
{
	for (auto && bat : bats)
	{
		if (bat.ignore_timer > 0) continue;
		//Collision with player
		if (Collision::cylinderIntersection(bat.coordinate_system.position, bat.radius, bat.half_height,
			player.coordinate_system.position, player.getRadius(), player.getHalfHeight()))
		{
			player.hitByBat(bat.velocity);

			bat.velocity = Vector3(0, bat.S_MAX, 0);

			bat.ignore_timer = 1.0f;

			glm::vec3 pos(player.getPosition().x, player.getPosition().y + player.getHalfHeight(), player.getPosition().z);
			g_particle_emitter.addEffect(500,pos,0.03,glm::vec4(1,1,0.3,1),500,Particle_Pattern::Random,2.0,0.0);

		}
	}

}




