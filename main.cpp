/*
 *
 *	Main.cpp
 *	Assignment 3
 *	Author: Chris Boyd, 200225231
 *
 *	Description:
 *		An OpenGL game using ObjLibrary and a modified ObjShader for shadows and lighting.
 *		Opens a freeglut window, loads in the Obj shader models. Reads a world file and loads a world.
 *		Draws to a depth texture. Renders to the screen with lighting and shadows.
 *		Generates Height maps for 5 different disk types. Creates models using triangle lists in vbos.
 *		Loads in a rod and ring at the position of each disk center. Rings move around the map to a new
 *		disk center when they reach one they choose a new disk target to move towards.
 *
 *		Rings and rods are worth points. The player can collect them for points.
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

#include "Sleep.h"
#include "MathHelper.h"

#include "Random.h"
#include "Game.h"
#include "main.h"


global_local Game game;

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

	glutMainLoop();
	return 1;
}



void init()
{
	glClearColor(0.2f, 0.4f, 0.6f, 0.0f);
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	text_renderer.init();
	line_renderer.init();
	//Load up all the models and grab the models with shader from them
	ObjShader::load();

	game.init();
	depth_texture.init();

	//Enabled lighting and shadows
	LightingManager::setEnabled(true);
	LightingManager::setShadowEnabled(true);
	LightingManager::setShadowDistance(game.shadow_box.getShadowDistance());

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

	time_counter.start();
}




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
				time_scale = 0.25f;
			else
				time_scale = 1.0f;
		}
		break;

		//Tab cycle between worlds on win32
#ifdef _WIN32
	case '\t':
		if (!key_pressed['\t'])
			game.destroyIntoNextWorld();
		break;
#endif
	case 'L':
		LightingManager::setEnabled(!LightingManager::isEnabled());
		break;
	case 27: // on [ESC]
		exit(0); // normal exit
	default:;
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
			win_width = 1280;
			win_height = 960;
			glutReshapeWindow(win_width, win_height);
			glutPositionWindow(0, 0);
		}
	}
}

//Read keyboard input on key up
void keyboardUp(unsigned char key, int x, int y)
{
	//Convert all keys to uppercase
	if (key >= 'a' && key <= 'z')
		key = key - 'a' + 'A';

	//Unpress the key
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
	default:;
	}

	//Set special keys
	key_pressed[KEY_SHIFT] = (glutGetModifiers() == GLUT_ACTIVE_SHIFT);
	key_pressed[KEY_LEFT_ALT] = (glutGetModifiers() == GLUT_ACTIVE_ALT);

}

//Read keyboard input on key down
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
	default:;
	}

}

//Read keyboard input on key up
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
	default:;
	}

	//Set special keys
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

	//Warp the pointer back to its original spot if it moved
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

	//Hides mouse when held
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
	default:;
	}

	//Reset mouse coords on mouse click
	mouse_x = x;
	mouse_y = y;
	mouse_locked_x = x;
	mouse_locked_y = y;

}



void update()
{
	delta_time = time_counter.getAndReset();

	//Multiply delta time by time_scale to slow or speed up game
	const double scaled_time = delta_time * time_scale;
	elapsed_time_milliseconds += long long(scaled_time + 0.5);
	update_lag += scaled_time;

	if (update_lag > FRAME_TIME_UPDATE * time_scale)
	{
		while (update_lag > FRAME_TIME_UPDATE * time_scale)
		{
			game.update(FRAME_TIME_UPDATE * time_scale);
			update_lag -= FRAME_TIME_UPDATE * time_scale;
			update_count++;
		}
	} else if (update_lag < MIN_FRAME_TIME_DISPLAY * time_scale)
	{
		sleepms(MIN_FRAME_TIME_DISPLAY * time_scale - update_lag);
	}
	game.updateAnimations(scaled_time);

	display_count++;
	glutPostRedisplay();

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

void display()
{
	game.display();
	glutSwapBuffers();
}
