/*
 *
 *	Main.cpp
 *	Author: Chris Boyd, 200225231
 *
 *	Description:
 *		An OpenGL game using ObjLibrary and a modified ObjShader for shadows and lighting.
 *		Opens a freeglut window, loads inthe Obj shader models. Reads a world file and loads a world.
 *		Draws to a depth texture. Renders to the screen with lighting and shadows.
 *		Generates Height maps for 5 different disk types. Creates models using triangle lists in vbos
 *
 *
 *		KEY BINDINGS:
 *		[Tab]: Loads the next file in the world folder.
 *		WASD: Move/Strafe
 *		Arrow keys: Move
 *		Left mouse button: Hold down to look around
 *		Right mouse button: Hold down to turn.
 *		Shift: Hold to move and turn faster.
 *		Left/Right mouse buttons: Hold down to move forward.
 *	    O: Hold to change to overview camera.
 *		H: to toggle between full and half speed time scale.
 *		L: Toggles Lighting
 *
 *
 */

#include <cstdlib>
#include <string>
#include <iostream>
#include <ctime>

 //GLM inclues
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"

//GLUT / OBJ inclues
#include "lib/GetGlutWithShaders.h"
#include "lib/ObjLibrary/ObjModel.h"
#include "lib/ObjLibrary/ObjShader.h"
#include "lib/ObjLibrary/ModelWithShader.h"
#include "lib/ObjLibrary/LightingManager.h"
#include "lib/ObjLibrary/SpriteFont.h"

//My includes
#include "CoordinateSystem.h"
#include "World.h"
#include "ShadowBox.h"
#include "MathHelper.h"
#include "PerformanceCounter.h"
#include "Random.h"
#include "DepthTexture.h"
#include "TextRenderer.h"
#include "main.h"


using namespace std;
using namespace ObjLibrary;
using MathHelper::M_PI;
using MathHelper::M_PI_2;


int main(int argc, char* argv[])
{
	srand(unsigned(time(nullptr)));
	Random::init();

	glutInitWindowSize(win_width, win_height);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB | GLUT_MULTISAMPLE);
	glutCreateWindow("Disk Game");

	// add shader initialization here
	const int gl3w_err = gl3wInit();
	if (gl3w_err != 0)
	{
		// Problem: gl3wInit failed, something is seriously wrong.
		cerr << "gl3wInit failed, aborting." << endl;
		exit(1);
	}
	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	//Keyboard input
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);

	//Mouse input
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);

	//Update
	glutIdleFunc(update);
	//Reshape
	glutReshapeFunc(reshape);
	//Render
	glutDisplayFunc(display);

	//Load models and set up lighting
	init();

	//Frame rate limiter
	glutTimerFunc(unsigned(1000.0 / FPS), timer, 0);

	glutMainLoop();

	return 1;
}



void init()
{
	glClearColor(0.2f, 0.4f, 0.6f, 0.0f);
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	textRenderer.init();

	//Load up all the models and grab the models with shader from them
	ObjShader::load();
	ObjModel temp;
	temp.load("assets/Models/cbabe_stand_n.obj");
	player_model = temp.getModelWithShader();
	temp.load("assets/Models/Skybox.obj");
	skybox_model = temp.getModelWithShader();

	//Folder location of worlds
	world_folder = "assets/Worlds/";

#ifdef  _WIN32
	//Load the file names of all of the worlds
	levels = getFileNamesInFolder(world_folder);
	for (auto const& file : levels)
		cout << "File name:" << file << endl;
	cout << "Press [Tab] to cycle worlds" << endl;
#endif

	//The world that is loaded first
	world.init(world_folder + "Basic.txt");

	//Enabled lighting and shadows
	LightingManager::setEnabled(true);
	LightingManager::setShadowEnabled(true);
	LightingManager::setShadowDistance(float(SHADOW_DISTANCE));

	//Set the shadow light to be a direction light where the sun is
	LightingManager::setLightEnabled(0, true);
	LightingManager::setLightDirectional(0, SUN_DIR);
	LightingManager::setLightColour(0, V3(1.0, 1.0, 1.0));


	LightingManager::setLightEnabled(2, true);
	LightingManager::setLightColour(2, V3(1.0, 1.0, 1.0));


	//Set a position light at the players location
	LightingManager::setLightEnabled(1, false);
	LightingManager::setLightPositional(1, PLAYER_INIT_POS);
	LightingManager::setLightColour(1, V3(0.5, 0.5, 0.5));
	LightingManager::setLightAttenuation(1, 1.5f, 0.09f, 0.032f);

	//Pass the light view matrix to the shadow box
	light_view_matrix = glm::mat4();
	shadow_box.init(&light_view_matrix, &active_camera, float(SHADOW_DISTANCE));

	depth_texture.init(SHADOW_MAP_SIZE);

	last_time = glutGet(GLUT_ELAPSED_TIME);


}


#ifdef _WIN32
std::vector<std::string> getFileNamesInFolder(const std::string& folder)
{
	std::vector<std::string> names;
	std::string search_path = folder + "/*.*";
	WIN32_FIND_DATAA fd;
	HANDLE hFind = FindFirstFileA(search_path.c_str(), &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			// read all (real) files in current folder
			// , delete '!' read other 2 default folder . and ..
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				names.emplace_back(fd.cFileName);
			}
		} while (::FindNextFileA(hFind, &fd));
		::FindClose(hFind);
	}
	return names;
}
#endif

//Read keyboard input
void keyboard(unsigned char key, int x, int y)
{
	//Convert lower case to uppercase
	if (key >= 'a' && key <= 'z')
		key = key - 'a' + 'A';

	//These should only happen on key down not hold
	switch (key)
	{
	case 'H':
		//Time Scale to half speed
		if (!key_pressed['H'])
		{
			if (time_scale == 1.0f)
				time_scale = 0.5f;
			else
				time_scale = 1.0f;
		}
		break;

		//Tab cycle between worlds on win32
#ifdef _WIN32
	case '\t':
		if (!key_pressed['\t'])
		{
			world.destroy();
			world.init(world_folder + levels[level++]);
			if (level >= levels.size()) level = 0;
		}
		break;
#endif
	case 'L':
		LightingManager::setEnabled(!LightingManager::isEnabled());
		break;
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	default:
		break;
	}

	key_pressed[key] = true;

	key_pressed[KEY_SHIFT] = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
	key_pressed[KEY_LEFT_ALT] = (glutGetModifiers() == GLUT_ACTIVE_ALT);

	//Alt-Enter will change to and from fullscreen
	if (key_pressed[KEY_LEFT_ALT] && key_pressed[13] && fullscreen_toggle_allowed)
	{
		full_screen = !full_screen;
		fullscreen_toggle_allowed = false;

		if (full_screen)
			glutFullScreen();
		else
		{
			glutReshapeWindow(win_width, win_height);
			glutPositionWindow(0, 0);
		}
	}



}

//Read keyboard input
void keyboardUp(unsigned char key, int x, int y)
{
	if (key >= 'a' && key <= 'z')
		key = key - 'a' + 'A';

	key_pressed[key] = false;

	//Fix sticky key with shift
	if (key == '?' || key == '/')
	{
		key_pressed['?'] = false;
		key_pressed['/'] = false;
	}

	switch (key)
	{
	case 13:
		fullscreen_toggle_allowed = true;
		break;
	}

	key_pressed[KEY_SHIFT] = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
	key_pressed[KEY_LEFT_ALT] = (glutGetModifiers() == GLUT_ACTIVE_ALT);

}

//Read keyboard input
void special(const int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_SHIFT_L:
		key_pressed[KEY_SHIFT] = true;
		break;
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_ALT_L:
		key_pressed[KEY_LEFT_ALT] = false;
		break;
	}

}

//Read keyboard input
void specialUp(const int special_key, int x, int y)
{
	switch (special_key)
	{
	case GLUT_KEY_UP:
		key_pressed[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_DOWN_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_END:
		key_pressed[KEY_END] = false;
		break;
	case GLUT_KEY_ALT_L:
		key_pressed[KEY_LEFT_ALT] = false;
		break;
	}
	key_pressed[KEY_SHIFT] = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
	key_pressed[KEY_LEFT_ALT] = (glutGetModifiers() == GLUT_ACTIVE_ALT);
}

//Read mouse move input when a button is held
void mouseMove(const int x, const int y)
{
	//Add up mouse movements until they are processed.
	mouse_dx += x - mouse_x;
	mouse_dy += y - mouse_y;
	mouse_x = x;
	mouse_y = y;

	//Warp the pointer back to its original spot
	if (mouse_x != mouse_locked_x || mouse_y != mouse_locked_y)
	{
		glutWarpPointer(mouse_locked_x, mouse_locked_y);
		mouse_x = mouse_locked_x;
		mouse_y = mouse_locked_y;
	}

}

//Read mouse click input
void mouseButton(const int button, const int state, const int x, const int y)
{

	switch (button)
	{
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			key_pressed[MOUSE_RIGHT] = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		} else
		{
			key_pressed[MOUSE_RIGHT] = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			key_pressed[MOUSE_LEFT] = true;
			glutSetCursor(GLUT_CURSOR_NONE);
		} else
		{
			key_pressed[MOUSE_LEFT] = false;
			glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;
	default:
		break;
	}

	//Reset mouse coords on mouse click
	mouse_x = x;
	mouse_y = y;
	mouse_locked_x = x;
	mouse_locked_y = y;

}



void update()
{
	//CALCULATE TIME ELAPSED
	const int this_time = glutGet(GLUT_ELAPSED_TIME);
	const int delta_time = this_time - last_time;
	last_time = this_time;

	//Multiple delta time by time_scale to slow or speed up game
	const float scaled_time = delta_time * time_scale;

	//Multiply player speed/turn by scaled time.
	float player_distance = PLAYER_SPEED * scaled_time;
	//Shift moves faster
	if (key_pressed[KEY_SHIFT])
		player_distance *= PLAYER_SPEED_BOOST_MULT;

	float player_turn = TURNING_DEGREES * scaled_time;
	//Shift turns faster
	if (key_pressed[KEY_SHIFT])
		player_turn *= PLAYER_TURN_BOOST_MULT;

	//old player values for calculations
	const V3 last_player_pos = player.getPosition();
	const V3 last_player_forward = player.getForward();

	Vector2 player_velocity(0.0, 0.0);

	//vector for diagonal movement
	if (key_pressed['D'])
		player_velocity.x += 1.0;
	if (key_pressed['A'])
		player_velocity.x -= 1.0;
	if (key_pressed['W'])
		player_velocity.y += 1.0;
	if (key_pressed['S'])
		player_velocity.y -= 1.0;

	if (player_velocity.getNorm() > 1.0)
		player_distance *= float(MathHelper::M_SQRT2_2);

	//move player
	if (key_pressed['D'])
		player.moveRight(player_distance);
	if (key_pressed['A'])
		player.moveRight(-player_distance);
	if (key_pressed['W'])
		player.moveForward(player_distance);
	if (key_pressed['S'])
		player.moveForward(-player_distance);

	//Reset player position and orientation to defaults
	if (key_pressed['R'])
	{
		player.setPosition(PLAYER_INIT_POS);
		player.setOrientation(PLAYER_INIT_FORWARD);
	}

	//Change to overview camera when 'O' held
	if (key_pressed['O'])
	{
		active_camera = &overview_camera;
	} else
	{
		active_camera = &player_camera;
	}

	//Move player
	if (key_pressed[KEY_UP_ARROW])
		player.moveForward(player_distance);
	if (key_pressed[KEY_DOWN_ARROW])
		player.moveForward(-player_distance);
	if (key_pressed[KEY_LEFT_ARROW])
		player.rotateAroundUp(player_turn);
	if (key_pressed[KEY_RIGHT_ARROW])
		player.rotateAroundUp(-player_turn);


	//On mouse left/right down, move forward
	if (key_pressed[MOUSE_LEFT] && key_pressed[MOUSE_RIGHT])
	{
		player.moveForward(player_distance);
	}


	const Vector3 player_position = player.getPosition();
	float y = world.getHeightAtCirclePosition(float(player_position.x), float(player_position.z), 0.25f);
	player.setPosition(Vector3(player_position.x, y, player_position.z) + PLAYER_OFFSET);

	world.update(scaled_time);
	world.pickupManager.checkForPickups(player.getPosition());
	


	//CAMERA STUFF
	bool player_moved = false;

	//If player is moving then lock the camera behind the player by turning camera_float off
	if (player.getPosition().getDistanceSquared(last_player_pos) > 0.0 ||
		player.getForward().getDistanceSquared(last_player_forward) > 0.0)
	{
		camera_float = false;
		player_moved = true;
	}


	//Arcball rotate around the player on left click
	if (key_pressed[MOUSE_LEFT] && !key_pressed[MOUSE_RIGHT] && !player_moved)
	{
		const double angle_y = player_camera.getForward().getAngleNormal(player_camera.getUp());
		if (angle_y > M_PI * 0.92)
		{
			if (mouse_dy > 0) mouse_dy = 0;
		}
		if (angle_y < M_PI * 0.08)
		{
			if (mouse_dy < 0) mouse_dy = 0;
		}
		const V3 target = player.getPosition() + PLAYER_CAMERA_OFFSET;
		player_camera.rotateAroundTarget(target, glm::radians(double(-mouse_dx)), glm::radians(double(-mouse_dy)));
		player_camera.setOrientation(player_camera.getForward().getNormalized(), V3(0, 1, 0));
		camera_float = true;
	}


	//Glides the camera back behind the player when left click is released.
	if (!key_pressed[MOUSE_LEFT])
	{
		V3 new_cam_position = player.getPosition();
		const V3& player_forward = player.getForward();

		new_cam_position.x -= 4.0f * player_forward.x;
		new_cam_position.z -= 4.0f * player_forward.z;
		new_cam_position += PLAYER_CAMERA_OFFSET;

		V3 diff = player_camera.getPosition() - new_cam_position;
		const double norm = diff.getNorm();

		//We want the camera to move faster if farther but to not too fast when far
		//or too slow when near
		if (norm >= 0.005f && camera_float)
		{
			double log_factor = (log(norm + 1) + 1) * (scaled_time / 10.0);
			//Caps the speed
			if (log_factor > 10.0) log_factor = 10.0;

			const V3 target = player.getPosition() + PLAYER_CAMERA_OFFSET;
			player_camera.rotateAroundTargetToPosition(target, new_cam_position, glm::radians(log_factor*0.05*scaled_time));
		} else
		{
			//If camera is near the origin point we can snap it back.
			camera_float = false;
		}
	}

	//Rotate the player left/right when mouse right is down
	if (key_pressed[MOUSE_RIGHT])
	{
		player.rotateAroundUp(-glm::radians(float(mouse_dx)) * time_scale);
		camera_float = false;
	}

	//Once mouse input has been used we need to set the dx and dy back to zero
	mouse_dx = 0;
	mouse_dy = 0;

	//If camera is not in free float around player, snap it back behind the player
	if (!camera_float)
	{
		V3 new_cam_position = player.getPosition();
		const V3& player_forward = player.getForward();
		player_camera.setOrientation(player_forward);
		new_cam_position.x -= 4.0f * player_forward.x;
		new_cam_position.z -= 4.0f * player_forward.z;
		new_cam_position += PLAYER_CAMERA_OFFSET;
		player_camera.setPosition(new_cam_position);
		player_camera.setOrientation(player.getForward());
	}

	//Overview camera follows player position
	overview_camera.lookAt(player.getPosition());





}

//Resizes window and adjusts the projection matrix
void reshape(const int w, const int h)
{
	glViewport(0, 0, w, h);
	win_width = w;
	win_height = h;

	const float aspect_ratio = float(w) / float(h);

	// calculate the projection matrix
	projection_matrix = glm::perspective(glm::radians(float(FOV)),
		aspect_ratio,
		float(CLIP_NEAR),
		float(CLIP_FAR));

	glutPostRedisplay();
}

//Frame limiter
void timer(int)
{
	glutPostRedisplay();
	glutTimerFunc(unsigned(1000.0 / FPS), timer, 0);
}


void renderToDepthTexture(glm::mat4& depth_vp)
{
	depth_texture.startRenderToDepthTexture();

	glDisable(GL_CULL_FACE);

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const V3& player_forward = player.getForward();
	const V3&player_position = player.getPosition();

	//Update the shadow box to re orient the location of the shadow view
	shadow_box.update();

	//Create an orthogonal view matrix for the depth view
	glm::mat4x4 depthProjectionMatrix;
	depthProjectionMatrix[0][0] = 2.0f / shadow_box.getWidth();
	depthProjectionMatrix[1][1] = 2.0f / shadow_box.getHeight();
	depthProjectionMatrix[2][2] = -2.0f / shadow_box.getLength();
	depthProjectionMatrix[3][3] = 1;

	const Vector3 center = -shadow_box.getCenter();

	//Calculate the light view matrix which is what the light views
	const Vector3 light_inverse_dir = -SUN_DIR.getNormalized();

	light_view_matrix = glm::mat4();
	const float pitch = float(acos(Vector2(light_inverse_dir.x, light_inverse_dir.z).getNorm()));
	light_view_matrix = glm::rotate(light_view_matrix, pitch, glm::vec3(1, 0, 0));
	float yaw = float(atan2(light_inverse_dir.x, light_inverse_dir.z));

	yaw = light_inverse_dir.z > 0 ? yaw - float(M_PI) : yaw;
	light_view_matrix = glm::rotate(light_view_matrix, -yaw + float(M_PI), glm::vec3(0, 1, 0));
	light_view_matrix = glm::translate(light_view_matrix, glm::vec3(center.x, center.y, center.z));

	depth_vp = depthProjectionMatrix * light_view_matrix;


	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(player_position));
	model_matrix = glm::rotate(model_matrix, (float(atan2(player_forward.x, player_forward.z)) - float(M_PI_2)), glm::vec3(player.getUp()));;
	glm::mat4 depth_mvp = depth_vp * model_matrix;

	const unsigned int depth_matrix_id = depth_texture.getDepthMatrixUniformLocation();

	glUniformMatrix4fv(depth_matrix_id, 1, GL_FALSE, &depth_mvp[0][0]);

	//Draw the players meshes only to the depth texture
	const unsigned int mat_count = player_model.getMaterialCount();
	for (unsigned int i = 0; i < mat_count; i++)
	{
		const unsigned int mesh_count = player_model.getMeshCount(i);
		for (unsigned int j = 0; j < mesh_count; j++)
		{
			const MeshWithShader& m = player_model.getMesh(i, j);
			m.draw();
		}

	}

	//Draw the world to the depth texture
	//TODO RINGS?
	world.drawDepth(depth_matrix_id, depth_vp);




}

void display()
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


	const V3& player_forward = player.getForward();
	const V3& player_position = player.getPosition();
	const V3& camera_position = active_camera->getPosition();


	//************Render to the screen with shadows and lighting*************
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, win_width, win_height);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Positional light at player position
	LightingManager::setLightPositional(1, player.getPosition());

	// Get the current camera view matrix
	glm::mat4 view_matrix = active_camera->getViewMatrix();


	//Draw the skybox
	glm::mat4 model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(camera_position));
	glm::mat4 mvp_matrix = projection_matrix * view_matrix * model_matrix;

	glDepthMask(GL_FALSE);
	skybox_model.draw(model_matrix, view_matrix, mvp_matrix, active_camera->getPosition());
	glDepthMask(GL_TRUE);

	//Draw the world
	//world.drawOptimized(view_matrix, projection_matrix, active_camera->getPosition());
	world.draw(view_matrix, projection_matrix, active_camera->getPosition());

	//Draw the player
	model_matrix = glm::mat4();
	model_matrix = glm::translate(model_matrix, glm::vec3(player_position));
	model_matrix = glm::rotate(model_matrix, (float(atan2(player_forward.x, player_forward.z)) - float(M_PI_2)), glm::vec3(player.getUp()));;
	mvp_matrix = projection_matrix * view_matrix * model_matrix;
	player_model.draw(model_matrix, view_matrix, mvp_matrix, active_camera->getPosition());

	const std::string text = "Score: " + std::to_string(world.pickupManager.score);
	textRenderer.draw(text, 10.0f, float(win_height - 40), 0.75f, glm::vec3(1, 1, 1));

	//Render depth texture to screen - **Changes required to shader and Depth Texture to work
	//depth_texture.renderDepthTextureToQuad(0, 0, 512, 512);

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();

}
