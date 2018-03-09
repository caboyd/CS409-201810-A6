#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "lib/glm/glm.hpp"

using ObjLibrary::Vector3;

#define global_extern extern
#define global_local static


class TextRenderer;
class DepthTexture;
class LineRenderer;
class PerformanceCounter;

//Global External constants available to all files
//***************************************
//ShadowBox.h uses these globals
global_extern const double FOV;
global_extern const double CLIP_NEAR;
global_extern const double CLIP_FAR;



//Global local Constants available in main.cpp and game class
//***************************************
global_extern const unsigned int KEY_UP_ARROW;
global_extern const unsigned int KEY_DOWN_ARROW;
global_extern const unsigned int KEY_LEFT_ARROW;
global_extern const unsigned int KEY_RIGHT_ARROW;
global_extern const unsigned int KEY_END;
global_extern const unsigned int KEY_SHIFT;
global_extern const unsigned int KEY_LEFT_ALT;
global_extern const unsigned int MOUSE_LEFT;
global_extern const unsigned int MOUSE_RIGHT;
global_extern const unsigned int KEY_COUNT;

//Framerate and Frame time
global_extern const double FPS_UPDAT;
global_extern const double FPS_DISPLAY;
global_extern const double FRAME_TIME_UPDATE;
global_extern const double MIN_FRAME_TIME_DISPLAY;
global_extern const double FIXED_TIME_STEP;




//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
global_extern const Vector3 SUN_DIR;
global_extern const Vector3 CAMERA_INIT_FORWARD;

global_extern const Vector3 PLAYER_CAMERA_OFFSET;
global_extern const Vector3 PLAYER_CAMERA_INIT_POS ;
global_extern const Vector3 OVERVIEW_CAMERA_INIT_POS;


//Players turns 3 degrees per second
global_extern const float TURNING_DEGREES;
//Player moves 10 metres per second
global_extern const float PLAYER_SPEED;

//Player speed multiplier constants
global_extern const float PLAYER_SPEED_BOOST_MULT;
global_extern const float PLAYER_TURN_BOOST_MULT;


//Globals Externals available to all files
//***************************************

//Windows width and height
global_extern int win_width;
global_extern int win_height;

//Renderers for rendering text and lines
global_extern TextRenderer text_renderer;
global_extern LineRenderer line_renderer;
global_extern DepthTexture depth_texture;

//High precision timer for calculated delta time
global_extern PerformanceCounter time_counter;

//Global Locals available in main.cpp and game class
//***************************************
global_extern double delta_time;
global_extern double update_lag;
global_extern long long update_count;
global_extern long long display_count;
global_extern long long elapsed_time_milliseconds;
global_extern bool full_screen;
global_extern bool fullscreen_toggle_allowed;
global_extern bool key_pressed[];

//Current mouse location
global_extern int mouse_x, mouse_y;

//The change in mouse since the last time the mouse data was processed
global_extern int mouse_dx, mouse_dy;

//The location of the mouse when the mouse was pressed. 
//This is used to snap the mouse back to its position so it doesn't leave the window
global_extern int mouse_locked_x, mouse_locked_y;



//Current time scale to allow for slow motion or speed up
global_extern double time_scale;

//Screen space projection matrix
global_extern glm::mat4 projection_matrix;
