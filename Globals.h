#pragma once
#include "lib/ObjLibrary/Vector3.h"
#include "lib/glm/glm.hpp"

using ObjLibrary::Vector3;

#define global extern

class TextRenderer;
class DepthTexture;
class LineRenderer;
class PerformanceCounter;

//Global External constants available to all files
//***************************************
//ShadowBox.h uses these globals
global const double FOV;
global const double CLIP_NEAR;
global const double CLIP_FAR;

global const unsigned int KEY_UP_ARROW;
global const unsigned int KEY_DOWN_ARROW;
global const unsigned int KEY_LEFT_ARROW;
global const unsigned int KEY_RIGHT_ARROW;
global const unsigned int KEY_END;
global const unsigned int KEY_SHIFT;
global const unsigned int KEY_LEFT_ALT;
global const unsigned int MOUSE_LEFT;
global const unsigned int MOUSE_RIGHT;
global const unsigned int KEY_COUNT;

//Framerate and Frame time
global const double FPS_UPDATE;
global const double FPS_DISPLAY;
global const double FRAME_TIME_UPDATE;
global const double FRAME_TIME_DISPLAY;
global const double FIXED_TIME_STEP;


//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
global const Vector3 SUN_DIR;
global const Vector3 CAMERA_INIT_FORWARD;

global const Vector3 PLAYER_CAMERA_OFFSET;
global const Vector3 PLAYER_CAMERA_INIT_POS ;
global const Vector3 OVERVIEW_CAMERA_INIT_POS;

//Players turns 3 degrees per second
global const float TURNING_DEGREES;
//Player moves 10 metres per second
global const float PLAYER_SPEED;

//Player speed multiplier constants
global const float PLAYER_SPEED_BOOST_MULT;
global const float PLAYER_TURN_BOOST_MULT;


//Globals Externals available to all files
//***************************************

global bool full_screen;
global bool fullscreen_toggle_allowed;

//Windows width and height
global int win_width;
global int win_height;

//Renderers for rendering text and lines
global TextRenderer text_renderer;
global LineRenderer line_renderer;

//Texture used for shadow mapping
global DepthTexture depth_texture;

//High precision timer for calculated delta time
global PerformanceCounter time_counter;

//Current time scale to allow for slow motion or speed up
global double time_scale;

//Time elapsed since last frame
global double delta_time;

//Accumulated delta build up that needs to be processed in update
global double update_lag;

//Number of times update has be called
global long long update_count;

//Number of times display has been called
global long long display_count;

//Total time elapsed since start
global long long elapsed_time_nanoseconds;

global double update_fps;
global double display_fps;

//Array indicating which keys are currently pressed
global bool key_pressed[];

//Current mouse location
global int mouse_x, mouse_y;

//The change in mouse since the last time the mouse data was processed
global int mouse_dx, mouse_dy;

//The location of the mouse when the mouse was pressed. 
//This is used to snap the mouse back to its position so it doesn't leave the window
global int mouse_locked_x, mouse_locked_y;

//Screen space projection matrix
global glm::mat4 projection_matrix;
