#pragma once

#define V3 Vector3

#define global static
#define global_extern

//Helper function to get all the files in a folder on windows
#ifdef _WIN32
#include <windows.h>
std::vector<std::string> getFileNamesInFolder(const std::string& folder);
#endif

void init();
void update();
void reshape(int w, int h);
void display();
void timer(int);

void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void mouseMove(int x, int y);
void mouseButton(int button, int state, int x, int y);

//Consts
const unsigned int KEY_UP_ARROW = 256;
const unsigned int KEY_DOWN_ARROW = 257;
const unsigned int KEY_LEFT_ARROW = 258;
const unsigned int KEY_RIGHT_ARROW = 259;
const unsigned int KEY_END = 260;
const unsigned int KEY_SHIFT = 261;
const unsigned int KEY_LEFT_ALT = 262;
const unsigned int MOUSE_LEFT = 263;
const unsigned int MOUSE_RIGHT = 264;
const unsigned int KEY_COUNT = 265;

const double FPS = 144.0;

//ShadowBox.h uses these globals
global_extern const double FOV = 60.0;
global_extern const double CLIP_NEAR = 0.1;
global_extern const double CLIP_FAR = 1600.0;
const double SHADOW_DISTANCE = 35.0;
const unsigned int SHADOW_MAP_SIZE = 4096;

const double MIN_FRAME_TIME = 1000.0 / FPS;
//3 degrees per second
const float TURNING_DEGREES = 3.0f / 1000.0f;
//10 metres per second
const float PLAYER_SPEED = 10.0f / 1000.0f;;
//Boost constants
const float PLAYER_SPEED_BOOST_MULT = 4.0f;
const float PLAYER_TURN_BOOST_MULT = 1.5f;

//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
const V3 SUN_DIR(0.34, 0.83, 0.44);
const V3 CAMERA_INIT_FORWARD = V3(-1, 0, -1).getNormalized();
const V3 PLAYER_OFFSET = V3(0.0f, 0.8f, 0.0f);
const V3 PLAYER_INIT_POS = V3(0.0f, 0.0f, 0.0f) + PLAYER_OFFSET;
const V3 PLAYER_INIT_FORWARD = V3(SUN_DIR.x, 0, SUN_DIR.z).getNormalized();
const V3 PLAYER_CAMERA_OFFSET = V3(0, 0.75f, 0);
const V3 PLAYER_CAMERA_INIT_POS = PLAYER_CAMERA_OFFSET;
const V3 OVERVIEW_CAMERA_INIT_POS = V3(100.f, 200.0f, 10.0f);

const glm::mat4 BIAS_MATRIX(
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 0.5, 0.0,
	0.5, 0.5, 0.5, 1.0
);

//Globals
global_extern int win_width = 1280;
global_extern int win_height = 960;
global bool full_screen = false;
global bool fullscreen_toggle_allowed = true;
global bool key_pressed[KEY_COUNT] = { false };

//Current mouse location
global int mouse_x = 0, mouse_y = 0;

//The change in mouse since the last time the mouse data was processed
global int mouse_dx = 0, mouse_dy = 0;

//The location of the mouse when the mouse was click. 
//This is used to snap the mouse back to its position
global int mouse_locked_x = 0, mouse_locked_y;

//Set this to true to float the camera when the game starts
global bool camera_float = false;

//Current level
global unsigned int level = 0;
//list of level file names to load
global std::vector<std::string> levels;

global std::string world_folder;

global int last_time;
global float time_scale = 1.0f;

//Global Game Objects
global ModelWithShader player_model;
global ModelWithShader skybox_model;

global glm::mat4 projection_matrix;
global CoordinateSystem player_camera(PLAYER_CAMERA_INIT_POS, CAMERA_INIT_FORWARD);
global CoordinateSystem overview_camera(OVERVIEW_CAMERA_INIT_POS, V3(0.0f, -1.0f, 0.0f));
global CoordinateSystem* active_camera = &player_camera;

global World world;
global CoordinateSystem player(PLAYER_INIT_POS, PLAYER_INIT_FORWARD);
global ShadowBox shadow_box;

global DepthTexture depth_texture;

global glm::mat4x4 light_view_matrix;

global TextRenderer textRenderer;