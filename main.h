#pragma once

#define V3 Vector3

//Global within main.cpp
#define global_local static

//Global throughout all files in project
#define global_extern

//Helper function to get all the files in a folder on windows
#ifdef _WIN32
#include <windows.h>
std::vector<std::string> getFileNamesInFolder(const std::string& folder);
#endif

//Main functions
void init();
void update(float delta_time);
void reshape(int w, int h);
void display();
void timer(int);

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

//Global local Constants
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
global_local const double FPS = 60.0;
global_local const double MIN_FRAME_TIME = 1000.0 / FPS;


//Players turns 3 degrees per second
global_local const float TURNING_DEGREES = 3.0f / 1000.0f;
//Player moves 10 metres per second
global_local const float PLAYER_SPEED = 10.0f / 1000.0f;;

//Player speed multiplier constants
global_local const float PLAYER_SPEED_BOOST_MULT = 4.0f;
global_local const float PLAYER_TURN_BOOST_MULT = 1.5f;


//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
global_local const V3 SUN_DIR(0.34, 0.83, 0.44);
global_local const V3 CAMERA_INIT_FORWARD = V3(-1, 0, -1).getNormalized();
global_local const V3 PLAYER_OFFSET = V3(0.0f, 0.8f, 0.0f);
global_local const V3 PLAYER_INIT_POS = V3(0.0f, 0.0f, 0.0f) + PLAYER_OFFSET;
global_local const V3 PLAYER_INIT_FORWARD = V3(SUN_DIR.x, 0, SUN_DIR.z).getNormalized();
global_local const V3 PLAYER_CAMERA_OFFSET = V3(0, 0.75f, 0);
global_local const V3 PLAYER_CAMERA_INIT_POS = PLAYER_CAMERA_OFFSET;
global_local const V3 OVERVIEW_CAMERA_INIT_POS = V3(100.f, 200.0f, 10.0f);

global_local const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

//Globals Externals available to all files
//***************************************

//Windows width and height
global_extern int win_width = 1280;
global_extern int win_height = 960;


//Renderers for rendering text and lines
global_extern TextRenderer text_renderer;
global_extern LineRenderer line_renderer;
global_extern DepthTexture depth_texture;

//Global Locals available in main.cpp
//***************************************
global_local double delta_time;
global_local bool full_screen = false;
global_local bool fullscreen_toggle_allowed = true;
global_local bool key_pressed[KEY_COUNT] = { false };

//Current mouse location
global_local int mouse_x = 0, mouse_y = 0;

//The change in mouse since the last time the mouse data was processed
global_local int mouse_dx = 0, mouse_dy = 0;

//The location of the mouse when the mouse was click. 
//This is used to snap the mouse back to its position
global_local int mouse_locked_x = 0, mouse_locked_y;

//Set this to true to float the camera when the game starts
global_local bool camera_float = false;

//Current level
global_local unsigned int level = 0;
//list of level file names to load
global_local std::vector<std::string> levels;

global_local std::string world_folder;

global_local PerformanceCounter time_counter;
global_local float time_scale = 1.0f;

//Global Game Objects
global_local ModelWithShader player_model;
global_local ModelWithShader skybox_model;

global_local glm::mat4 projection_matrix;
global_local CoordinateSystem player_camera(PLAYER_CAMERA_INIT_POS, CAMERA_INIT_FORWARD);
global_local CoordinateSystem overview_camera(OVERVIEW_CAMERA_INIT_POS, V3(0.0f, -1.0f, 0.0f));
global_local CoordinateSystem* active_camera = &player_camera;

global_local World world;
global_local CoordinateSystem player(PLAYER_INIT_POS, PLAYER_INIT_FORWARD);
global_local ShadowBox shadow_box;



global_local glm::mat4x4 light_view_matrix;

