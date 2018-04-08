#pragma once

#define global_extern extern
#include <lib/glm/mat4x4.hpp>
#include "SphereRenderer.h"

//Forward declarations
namespace ObjLibrary
{
	class Vector3;
}

class TextRenderer;
class DepthTexture;
class LineRenderer;
class PerformanceCounter;
class ParticleEmitter;

using ObjLibrary::Vector3;

//Global External constants available to all files
//***************************************
//ShadowBox.h uses these globals
global_extern const int DEFAULT_WIN_WIDTH;
global_extern const int DEFAULT_WIN_HEIGHT;
global_extern const double FOV;
//Clip planes for view matrix
global_extern const double CLIP_NEAR;
global_extern const double CLIP_FAR;

//Key codes
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
global_extern const double FPS_UPDATE;
global_extern const double FPS_DISPLAY;
global_extern const double FRAME_TIME_UPDATE;
global_extern const double FRAME_TIME_DISPLAY;
global_extern const double FIXED_TIME_STEP;
global_extern const double TIME_SCALE_FACTOR;


//Initial Camera/Player vectors
//Sun Dir is the direction the sun is on the skybox
global_extern const Vector3 SUN_DIR;
global_extern const Vector3 CAMERA_INIT_FORWARD;

global_extern const Vector3 PLAYER_CAMERA_OFFSET;
global_extern const Vector3 PLAYER_CAMERA_INIT_POS;
global_extern const Vector3 OVERVIEW_CAMERA_INIT_POS;

//Players turns 3 degrees per second
global_extern const float TURNING_DEGREES;
//Player moves 10 metres per second
global_extern const float PLAYER_ACCEL_FORWARD;
global_extern const float PLAYER_ACCEL;

global_extern const double TIME_SCALES[];
global_extern const int TIME_SCALE_COUNT;

//Globals Externals available to all files
//***************************************

global_extern bool g_full_screen;
global_extern bool g_fullscreen_toggle_allowed;

//Windows width and height
global_extern int g_win_width;
global_extern int g_win_height;

//Renderers for rendering text and lines
global_extern TextRenderer g_text_renderer;
global_extern LineRenderer g_line_renderer;
global_extern SphereRenderer g_sphere_renderer;

global_extern ParticleEmitter g_particle_emitter;

//Texture used for shadow mapping
global_extern DepthTexture g_depth_texture;

//High precision timer for calculated delta time
global_extern PerformanceCounter g_time_counter;

//Current time scale to allow for slow motion or speed up
global_extern int g_time_scale_id;
global_extern double g_time_scale;

//Time elapsed since last frame
global_extern double g_delta_time;

//Accumulated delta build up that needs to be processed in update
global_extern double g_update_lag;

//Number of times update has be called
global_extern long long g_update_count;

//Number of times display has been called
global_extern long long g_display_count;

//Total time elapsed since start
global_extern long long g_elapsed_time_nanoseconds;

global_extern double g_update_fps;
global_extern double g_display_fps;

//Array indicating which keys are currently pressed
global_extern bool g_key_pressed[];

//Current mouse location
global_extern int g_mouse_x, g_mouse_y;

//The change in mouse since the last time the mouse data was processed
global_extern int g_mouse_dx, g_mouse_dy;

//The location of the mouse when the mouse was pressed. 
//This is used to snap the mouse back to its position so it doesn't leave the window
global_extern int g_mouse_locked_x, g_mouse_locked_y;

//Screen space projection matrix
global_extern glm::mat4 g_projection_matrix;

global_extern float g_polygon_offset_factor, g_polygon_offset_units;
