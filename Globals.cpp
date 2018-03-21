#include "Globals.h"

#include "TextRenderer.h"
#include "LineRenderer.h"
#include "DepthTexture.h"
#include "PerformanceCounter.h"


//Global External constants available to all files
//***************************************
//ShadowBox.h uses these globals
const int DEFAULT_WIN_WIDTH = 1280;
const int DEFAULT_WIN_HEIGHT = 960;
const double FOV = 60.0;
const double CLIP_NEAR = 0.1;
const double CLIP_FAR = 1600.0;

//Global local Constants available in main.cpp and game class
//***************************************
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

//Framerate and Frame time
const double FPS_UPDATE = 60.0;
const double FPS_DISPLAY = 1000.0;
const double FRAME_TIME_UPDATE = 1000.0 / FPS_UPDATE;
const double FRAME_TIME_DISPLAY = 1000.0 / FPS_DISPLAY;
const double FIXED_TIME_STEP = 1.0 / FPS_UPDATE;


//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
const Vector3 SUN_DIR(0.34, 0.83, 0.44);
const Vector3 CAMERA_INIT_FORWARD = Vector3(-1, 0, -1).getNormalized();

const Vector3 PLAYER_CAMERA_OFFSET = Vector3(0, 1.55f, 0);
const Vector3 PLAYER_CAMERA_INIT_POS = PLAYER_CAMERA_OFFSET;
const Vector3 OVERVIEW_CAMERA_INIT_POS = Vector3(100.f, 200.0f, 10.0f);

//Players turns 3 degrees per second
const float TURNING_DEGREES = 3.0f;

const float PLAYER_ACCEL_FORWARD = 25.0f;
const float PLAYER_ACCEL = 10.0f;


//Globals Externals available to all files
//***************************************

//Windows width and height
int g_win_width = 1280;
int g_win_height = 960;

//Renderers for rendering text and lines
TextRenderer g_text_renderer;
LineRenderer g_line_renderer;
DepthTexture g_depth_texture;

//High precision timer for calculated delta time
PerformanceCounter g_time_counter;

//Global Locals available in main.cpp and game class
//***************************************
double g_delta_time;
double g_update_lag = 0;
long long g_update_count = 0;
long long g_display_count = 0;
long long g_elapsed_time_nanoseconds = 0;
double g_update_fps = FPS_UPDATE;
double g_display_fps = FPS_DISPLAY;

bool g_full_screen = false;
bool g_fullscreen_toggle_allowed = true;
bool g_key_pressed[KEY_COUNT] = { false };

//Current mouse location
int g_mouse_x = 0, g_mouse_y = 0;

//The change in mouse since the last time the mouse data was processed
int g_mouse_dx = 0, g_mouse_dy = 0;

//The location of the mouse when the mouse was pressed. 
//This is used to snap the mouse back to its position so it doesn't leave the window
int g_mouse_locked_x = 0, g_mouse_locked_y = 0;



//Current time scale to allow for slow motion or speed up
double g_time_scale = 1.0f;

glm::mat4 g_projection_matrix = glm::mat4();