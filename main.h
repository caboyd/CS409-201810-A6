#pragma once
#include "PerformanceCounter.h"
#include "DepthTexture.h"
#include "TextRenderer.h"
#include "LineRenderer.h"

//Global within main.cpp
#define global_local static

//Global throughout all files in project
#define global_extern


//Main functions
void init();
void update();
void reshape(int w, int h);
void display();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);



//Global External constants available to all files
//***************************************
//ShadowBox.h uses these globals
global_extern const double FOV = 60.0;
global_extern const double CLIP_NEAR = 0.1;
global_extern const double CLIP_FAR = 1600.0;



//Global local Constants available in main.cpp and game class
//***************************************
global_local const unsigned int KEY_UP_ARROW = 256;
global_local const unsigned int KEY_DOWN_ARROW = 257;
global_local const unsigned int KEY_LEFT_ARROW = 258;
global_local const unsigned int KEY_RIGHT_ARROW = 259;
global_local const unsigned int KEY_END = 260;
global_local const unsigned int KEY_SHIFT = 261;
global_local const unsigned int KEY_LEFT_ALT = 262;
global_local const unsigned int MOUSE_LEFT = 263;
global_local const unsigned int MOUSE_RIGHT = 264;
global_local const unsigned int KEY_COUNT = 265;

//Framerate and Frame time
global_local const double FPS_UPDATE = 60.0;
global_local const double FPS_DISPLAY = 300.0;
global_local const double FRAME_TIME_UPDATE = 1000.0 / FPS_UPDATE;
global_local const double MIN_FRAME_TIME_DISPLAY = 1000.0 / FPS_DISPLAY;
global_local const double FIXED_TIME_STEP = 1.0/FPS_UPDATE;

//Matrix to help with calculating depth texture p
global_local const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);


//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
global_local const Vector3 SUN_DIR(0.34, 0.83, 0.44);
global_local const Vector3 CAMERA_INIT_FORWARD = Vector3(-1, 0, -1).getNormalized();

global_local const Vector3 PLAYER_CAMERA_OFFSET = Vector3(0, 0.75f, 0);
global_local const Vector3 PLAYER_CAMERA_INIT_POS = PLAYER_CAMERA_OFFSET;
global_local const Vector3 OVERVIEW_CAMERA_INIT_POS = Vector3(100.f, 200.0f, 10.0f);


//Players turns 3 degrees per second
global_local const float TURNING_DEGREES = 3.0f / 1000.0f;
//Player moves 10 metres per second
global_local const float PLAYER_SPEED = 10.0f / 1000.0f;;

//Player speed multiplier constants
global_local const float PLAYER_SPEED_BOOST_MULT = 4.0f;
global_local const float PLAYER_TURN_BOOST_MULT = 1.5f;


//Globals Externals available to all files
//***************************************

//Windows width and height
global_extern int win_width = 1280;
global_extern int win_height = 960;

//Renderers for rendering text and lines
global_extern TextRenderer text_renderer;
global_extern LineRenderer line_renderer;
global_extern DepthTexture depth_texture;

//High precision timer for calculated delta time
global_extern PerformanceCounter time_counter;

//Global Locals available in main.cpp and game class
//***************************************
global_local double delta_time;
global_local double update_lag = 0;
global_local long long update_count = 0;
global_local long long display_count = 0;
global_local long long elapsed_time_milliseconds = 0;
global_local bool full_screen = false;
global_local bool fullscreen_toggle_allowed = true;
global_local bool key_pressed[KEY_COUNT] = { false };

//Current mouse location
global_local int mouse_x = 0, mouse_y = 0;

//The change in mouse since the last time the mouse data was processed
global_local int mouse_dx = 0, mouse_dy = 0;

//The location of the mouse when the mouse was pressed. 
//This is used to snap the mouse back to its position so it doesn't leave the window
global_local int mouse_locked_x = 0, mouse_locked_y;



//Current time scale to allow for slow motion or speed up
global_local double time_scale = 1.0f;

//Screen space projection matrix
global_local glm::mat4 projection_matrix;




//Helper function to get all the files in a folder on windows
#ifdef _WIN32
#include <windows.h>
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
