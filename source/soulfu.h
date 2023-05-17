//    SoulFu - 3D Rogue-like dungeon crawler
//    Copyright (C) 2007 Aaron Bishop
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    web:   http://www.aaronbishopgames.com
//    email: aaron@aaronbishopgames.com

// <ZZ> This file contains lots of global variables and defines and other fun stuff.

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// Don't need sqrt when just tryin' to find closest...  Can do with squared...
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!

// !!!BAD!!!
// !!!BAD!!! Get rid of all of those extra libraries!!!
// !!!BAD!!!

//-Includes--------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <setjmp.h>
#include <time.h>

#include <vorbis/codec.h>
#include "jpeglib.h"

# define SF_EXTERN extern

//-Defines---------------------------------------------------------------------------------------
#define MEG (1048576)

#ifndef FALSE
#    define FALSE 0
#endif

#ifndef TRUE
#    define TRUE 1
#endif

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#    define LIL_ENDIAN
#endif

#define X 0
#define Y 1
#define Z 2
#define A 0
#define B 1
#define C 2


#define MAX_LOCAL_PLAYER 4              // Total players on local machine...
SF_EXTERN unsigned char update_performed; // =FALSE;   // Do any files need decompressing?



// Stops player input from keyboard...
SF_EXTERN unsigned short global_block_keyboard_timer ; // = 0;



// For Window3D
SF_EXTERN float script_matrix[12];
#define WIN_3D_AXIS     0
#define WIN_3D_SHADOW   16
#define WIN_3D_VERTEX   32
#define WIN_3D_BONE     48
#define WIN_3D_MODEL    64
#define WIN_3D_BONE_UPDATE 80
#define WIN_3D_TEXVERTEX 96
#define WIN_3D_SKIN_FROM_CAMERA  112
#define WIN_3D_MODEL_EDIT 128


// For Window3DRoom
#define ROOM_MODE_MINIMAP       0
#define ROOM_MODE_GRID          16
#define ROOM_MODE_VERTEX        32
#define ROOM_MODE_TRIANGLE      48
#define ROOM_MODE_EXTERIOR_WALL 64
#define ROOM_MODE_WAYPOINT      80
#define ROOM_MODE_BRIDGE        96
#define ROOM_MODE_OBJECT_GROUP  112


#define MAX_PLAYER_DEVICE_BUTTON 9              // 9 needed for keyboard...
SF_EXTERN unsigned char player_device_type[MAX_LOCAL_PLAYER];  // None, Keyboard, Joystick 1, Joystick 2, Joystick 3, Joystick 4...
#define PLAYER_DEVICE_NONE 0
#define PLAYER_DEVICE_KEYBOARD 1
#define PLAYER_DEVICE_JOYSTICK_1 2
#define PLAYER_DEVICE_JOYSTICK_2 3
#define PLAYER_DEVICE_JOYSTICK_3 4
#define PLAYER_DEVICE_JOYSTICK_4 5
SF_EXTERN unsigned short player_device_button[MAX_LOCAL_PLAYER][MAX_PLAYER_DEVICE_BUTTON];  // Either keyboard scan codes or joystick button numbers...
#define PLAYER_DEVICE_TYPE -1
#define PLAYER_DEVICE_BUTTON_LEFT 0
#define PLAYER_DEVICE_BUTTON_RIGHT 1
#define PLAYER_DEVICE_BUTTON_SPECIAL1 2
#define PLAYER_DEVICE_BUTTON_SPECIAL2 3
#define PLAYER_DEVICE_BUTTON_ITEMS 4
#define PLAYER_DEVICE_BUTTON_MOVE_UP 5
#define PLAYER_DEVICE_BUTTON_MOVE_DOWN 6
#define PLAYER_DEVICE_BUTTON_MOVE_LEFT 7
#define PLAYER_DEVICE_BUTTON_MOVE_RIGHT 8
#define PLAYER_DEVICE_BUTTON_ITEMS_DOWN 9
SF_EXTERN unsigned char player_device_button_pressed[MAX_LOCAL_PLAYER][4];  // One if button was just pressed, zero if not...
SF_EXTERN unsigned char player_device_button_unpressed[MAX_LOCAL_PLAYER][4];  // One if button was just let up, zero if not...
SF_EXTERN unsigned char player_device_button_pressed_copy[MAX_LOCAL_PLAYER][4];
SF_EXTERN unsigned char player_device_inventory_toggle[MAX_LOCAL_PLAYER];  // TRUE if player pressed the inventory button (and script should toggle the gem)...
SF_EXTERN unsigned char player_device_inventory_down[MAX_LOCAL_PLAYER];   // TRUE if player is holding the inventory button...
SF_EXTERN unsigned char player_device_controls_active[MAX_LOCAL_PLAYER];  // FALSE if player controls don't actually move character...  But script can still read 'em...  Used to access inventory...
SF_EXTERN float player_device_xy[MAX_LOCAL_PLAYER][2];



// Stuff for the enchantment targeting cursor thing...
SF_EXTERN unsigned char enchant_cursor_active ; // = FALSE;
SF_EXTERN unsigned short enchant_cursor_character;
SF_EXTERN unsigned short enchant_cursor_itemindex;
SF_EXTERN unsigned short enchant_cursor_target;
SF_EXTERN unsigned char enchant_cursor_target_item;


// Stuff for SYS_ENCHANT_CURSOR...
#define ENCHANT_CURSOR_ACTIVE       0
#define ENCHANT_CURSOR_TARGET       1
#define ENCHANT_CURSOR_TARGET_ITEM  2


// RDY flags...
#define RENDER_LIGHT_FLAG        1
#define RENDER_COLOR_FLAG        2
#define RENDER_NOCULL_FLAG       4
#define RENDER_ENVIRO_FLAG       8
#define RENDER_CARTOON_FLAG     16
#define RENDER_EYE_FLAG         32
#define RENDER_NO_LINE_FLAG     64
#define RENDER_PAPER_FLAG      128


SF_EXTERN unsigned char global_render_light_color_rgb[3];
SF_EXTERN float global_render_light_offset_xy[2];


// For makin' TIL generation easier...
//unsigned char* global_ddd_file_start;
//unsigned char* global_rdy_file_start;


// Ex. Player 2 is character 1023...    MAX_CHARACTER means invalid...
SF_EXTERN unsigned short local_player_character[MAX_LOCAL_PLAYER];


// Teams
#define TEAM_NEUTRAL   0	// Default team
#define TEAM_MONSTER   1
#define TEAM_GOOD      2
#define TEAM_EVIL      3


// Bones...
#define LEFT_BONE   1
#define RIGHT_BONE  2
#define LEFT2_BONE  3
#define RIGHT2_BONE 4
#define SADDLE_BONE 5



#define PI 3.1415929f
//#define DEVTOOL                     // Compile time option for running in developer mode
//#define SHOW_JOYSTICK_POSITIONS     // Compile time option to show joystick movement...
#define KEEP_COMPRESSED_FILES       // Compile time option to not delete compressed data...  DO NOT REMOVE!!!
//#define CAN_BLOW_SELF_UP            // Compile time option for being able to hurt self...
#define NAME_STRING (run_string[MAX_STRING-2])  // Second to last string used for GetName() and random_name...
#define DEBUG_STRING (run_string[MAX_STRING-1])  // Last string used for debug...
#define CHARACTER_Z_FLOAT   0.15f   // For ground collisions...
#define BACKFACE_CARTOON_LINES      // Seem to perform slightly better, and look a lot nicer...

 
#define ROOM_LINE_SIZE  0.0025f         // For room outlines
#define CHAR_LINE_SIZE  0.0015f         // For character outlines


// For making input and emacs type-ins not interfere with other things...
SF_EXTERN unsigned char input_active ; // = FALSE;


// Language stuff...
SF_EXTERN unsigned char user_language ; // = 0;
SF_EXTERN int user_language_phrases ; // = 0;
SF_EXTERN unsigned char bad_string[] ; // = "Bad";        // Used as return on language failures...
#define LANGUAGE_MAX 16
SF_EXTERN unsigned char* language_file[LANGUAGE_MAX];
SF_EXTERN unsigned char* kanji_data;



// Camera stuff..
#define CAMERA_ROTATION_RATE  90.0f
#define CAMERA_ZOOM_RATE 0.75f
#define MIN_CAMERA_DISTANCE 10.0f
//#define MAX_CAMERA_DISTANCE 50.0f
#define MAX_CAMERA_DISTANCE 75.0f
#define MIN_CAMERA_Y  4096  // Make smaller to view more of horizon...
#define MAX_CAMERA_Y  15360
#define MAX_TERRAIN_DISTANCE 500.0f


// For direct code->script calls...
#  define MAX_FAST_FUNCTION          16  // For predefined script functions like Spawn
#  define FAST_FUNCTION_SPAWN        0   // Offset for the Spawn() function
#  define FAST_FUNCTION_REFRESH      2   // Offset for the Refresh() function
#  define FAST_FUNCTION_EVENT        4   // Offset for the Event() function
#  define FAST_FUNCTION_AISCRIPT     6   // Offset for the AIScript() function
#  define FAST_FUNCTION_BUTTONEVENT  8   // Offset for the ButtonEvent() function
#  define FAST_FUNCTION_GETNAME      10  // Offset for the GetName() function
#  define FAST_FUNCTION_UNPRESSED    12  // Offset for the Unpressed() function
#  define FAST_FUNCTION_FRAMEEVENT   14  // Offset for the FrameEvent() function
#  define FAST_FUNCTION_MODELSETUP   16  // Offset for the ModelSetup() function...
#  define FAST_FUNCTION_DEFENSERATING 18 // Offset for the DefenseRating() function...
#  define FAST_FUNCTION_SETUP        20  // Offset for the Setup() function...
#  define FAST_FUNCTION_DIRECTUSAGE  22  // Offset for the DirectUsage() function...
#  define FAST_FUNCTION_ENCHANTUSAGE 24  // Offset for the EnchantUsage() function...


// Level of detail stuff
#ifdef DEVTOOL
#    define DETAIL_LEVEL_MAX 16      // Number of detail levels...  1 for DEVTOOL mode...
#else
#    define DETAIL_LEVEL_MAX 16     // Number of detail levels...  16 for non-DEVTOOL...
//#    define DETAIL_LEVEL_MAX 1      // Detail levels didn't work very well...
#endif
SF_EXTERN unsigned char drawing_world ; // = FALSE;


#define NO_BONE 255
#define NO_ITEM 65535
#define DRAG_ITEM 65534





#define ACTION_MAX 128              // Number of action types...
#define MAX_DDD_TEXTURE 4           // Number of textures in an object file...
#define MAX_DDD_SHADOW_TEXTURE 4    // Number of Shadow textures...
#define DDD_SCALE_WEIGHT 20000.0f   // DDD coordinates should range from - to + this number...
#define DDD_EXTERNAL_BONE_FRAMES 16384  // Flag for external linkage...
#define JOINT_COLLISION_SCALE 0.015f// For converting joint sizes from byte to float...


// Action names...
#define ACTION_BONING            0
#define ACTION_STAND             1
#define ACTION_WALK              2
#define ACTION_STUN_BEGIN        3
#define ACTION_STUN              4
#define ACTION_STUN_END          5
#define ACTION_KNOCK_OUT_BEGIN   6
#define ACTION_KNOCK_OUT         7
#define ACTION_KNOCK_OUT_STUN    8
#define ACTION_KNOCK_OUT_END     9
#define ACTION_BASH_LEFT         10
#define ACTION_BASH_RIGHT        11
#define ACTION_THRUST_LEFT       12
#define ACTION_THRUST_RIGHT      13
#define ACTION_SLASH_LEFT        14
#define ACTION_SLASH_RIGHT       15
#define ACTION_ATTACK_FAIL       16
#define ACTION_BLOCK_BEGIN       17
#define ACTION_BLOCK             18
#define ACTION_BLOCK_END         19
#define ACTION_JUMP_BEGIN        20
#define ACTION_JUMP              21
#define ACTION_JUMP_END          22
#define ACTION_RIDE              23
#define ACTION_SWIM              24
#define ACTION_SWIM_FORWARD      25
#define ACTION_MAGIC             26
#define ACTION_FIRE_BEGIN        27
#define ACTION_FIRE_READY        28
#define ACTION_FIRE              29
#define ACTION_FIRE_END          30
#define ACTION_EXTRA             31
#define ACTION_SPECIAL_0         32
#define ACTION_SPECIAL_1         33
#define ACTION_SPECIAL_2         34
#define ACTION_SPECIAL_3         35
#define ACTION_SPECIAL_4         36
#define ACTION_SPECIAL_5         37
#define ACTION_SPECIAL_6         38
#define ACTION_SPECIAL_7         39
#define ACTION_SPECIAL_8         40
#define ACTION_SPECIAL_9         41
#define ACTION_SPECIAL_10        42
#define ACTION_SPECIAL_11        43
#define ACTION_SPECIAL_12        44
#define ACTION_DOUBLE_BEGIN      45
#define ACTION_DOUBLE            46
#define ACTION_DOUBLE_END        47


// Weird & Silly stuff for figurin' out where a character's joints are onscreen...
// Helps figure out whether we need to actually draw the character or not, as well
// as telling us if the mouse cursor is over the character...
#define ONSCREEN_CLICK_SCALE  300.0f  // Magic number...
SF_EXTERN unsigned short num_onscreen_joint;
SF_EXTERN unsigned char onscreen_joint_active ; // = FALSE;
SF_EXTERN unsigned short onscreen_joint_character;
SF_EXTERN unsigned char* onscreen_joint_character_data;


// Turn pretty shadows off by default...  Later read from config file...
SF_EXTERN unsigned char volumetric_shadows_on ; // = FALSE;


// Stuff for character button controls...
SF_EXTERN unsigned char global_button_handled ; // = FALSE;


#define GRAVITY -0.05f

// Event callback codes...
#define EVENT_HIT_WALL         1
#define EVENT_HIT_WATER        2
#define EVENT_HIT_CHARACTER    3
#define EVENT_HIT_FLOOR        4  // For jumping characters or any type of particle...
#define EVENT_TIMER            5
#define EVENT_FELL_IN_PIT      6
#define EVENT_DAMAGED          7
#define EVENT_LEVEL_UP         8
#define EVENT_SECONDARY_TIMER  9
#define EVENT_THIRDIARY_TIMER  10
#define EVENT_MOUNTED          11
#define EVENT_DISMOUNTED       12
#define EVENT_BLOCKED          13
#define EVENT_DEFLECTED        14 // Only particles get this one...
#define EVENT_PETRIFIED        15
#define EVENT_UNPETRIFIED      16
#define EVENT_JUMPED_ON        17 // For when a platform is jumped on...
#define EVENT_DROWN            18 // Spawn some bubbles...
#define EVENT_RIPPLE           19
#define EVENT_SPAWNED_IN_WATER 20
#define EVENT_HIT_WAYPOINT     21
#define EVENT_HUNGER_DAMAGED   22
#define EVENT_NETWORK_UPDATE   23



// For AcquireTarget()
#define ACQUIRE_SEE_NEUTRAL   1
#define ACQUIRE_SEE_FRIENDLY  2
#define ACQUIRE_SEE_ENEMY     4
#define ACQUIRE_SEE_INVISIBLE 8
#define ACQUIRE_SEE_BEHIND    16
#define ACQUIRE_OPEN_MOUNT_ONLY 32
#define ACQUIRE_STANDING_ONLY 64
#define ACQUIRE_IGNORE_COLLISION 128


// Particle flags...
#define PART_CHAR_COLLISION_ON 1    // Part collides with characters...
#define PART_WALL_COLLISION_ON 2    // Part collides with heightmap/terrain...
#define PART_IGNORE_INTIMER 4       // Part collides with characters even if they are invincible (part should check in script)...
#define PART_FIT_LENGTH_ON 8        // 2 point particle is scaled to a set length...  So arrows don't stretch...
#define PART_SPIN_ON       16       // Part is a 1 point spinny particle
#define PART_FLAT_ON       32       // Part is drawn flat on xy plane (like ripples on water) 
#define PART_STUCK_ON      64       // Part is stuck to a character...
#define PART_GRAVITY_ON    128      // Part is pulled by gravity
#define PART_LIGHT_ON      256      // Part is drawn with additive transparency
#define PART_NUMBER_ON     512      // Part image is a number from 0-99
#define PART_IN_WATER      1024     // Part is under water
#define PART_AFTER_WATER   2048     // Part is drawn after water (so bubbles are more visible)
#define PART_SLOW_ON       4096     // Part is moving slow, so character collisions can be done less often...
#define PART_FAST_ON       8192     // Part is moving extra fast...  Update 10 times per frame (instead of just once)...
#define PART_HIT_OWNER_ON  16384    // Part can damage its owner...  Only if friendly fire is active...
#define PART_ATTACK_SPIN_ON 32768   // Attack spin is set from particle location instead of owner's direction...


// Virtue flags...  (for characters...  Because I ran out of character flags again...)
#define VIRTUE_FLAG_HITS         1  // Prayer whammy
#define VIRTUE_FLAG_EATS_TRIPE   2  // Character can be healed/charmed with tripe 
#define VIRTUE_FLAG_AFTER_WATER  4  // Character is always drawn after water...
#define VIRTUE_FLAG_NO_DROWN     8  // Character doesn't take drown damage...
#define VIRTUE_FLAG_STILL_HIDE  16  // Character is hidden when action == ACTION_STAND (enemies have trouble acquiring character as a target)
#define VIRTUE_FLAG_RUN_ME_OVER 32  // Can be run over by tankes (without being knocked down first)
#define VIRTUE_FLAG_BUMP_IMMUNE 64  // Immune to damage from bumps (unless bumper is also immune)
#define VIRTUE_FLAG_IMMUNE     128  // Immune to damage from own shots...


// More flags...  (for characters....  Because I ran out of character flags...)
#define MORE_FLAG_FAST_TURN   1   // Makes character not slow down when turning...
#define MORE_FLAG_PETRIFY_ON  2   // Don't mess with this in script...  Used internally to give petrify events...
#define MORE_FLAG_HIT_HEAD    4   // Hitting in head does double damage...
#define MORE_FLAG_PUSHABLE    8   // Character is pushable...
#define MORE_FLAG_PLATFALL    16  // Character falls through platforms
#define MORE_FLAG_NOSHADOW    64  // Character's volumetric shadow is never drawn (if pretty shadows are turned on)
#define MORE_FLAG_DOOR        128 // Weird collision thing...


// Other flags...  (for characters...  Ran out...)
#define OTHER_FLAG_ENCHANT_CENTER     1 // Character is using an ENCHANT.SRC spell and targeting circle has already been center'd...
#define OTHER_FLAG_NO_MORPH           2 // Character can't be Morph'd (for bosses & chests & stuff)
#define OTHER_FLAG_NO_HEARTS          4 // Character can't grab Hearts (for tankes & stuff)
#define OTHER_FLAG_EASY_ENTANGLE      8 // Character can be Entangle'd easily
#define OTHER_FLAG_NO_WALL_COLLIDE   16 // Character doesn't bump into walls...
#define OTHER_FLAG_ENCHANT_MOVED     32 // Character has moved the enchantment target cursor thing off of his own character (so we don't accidentally DChant ourselves)
// 64 is unused
// 128 is unused


// Enchant flags...  (for characters...  Guess why...)
#define ENCHANT_FLAG_SUMMON_1         1 // Character is summon'd (character is instantly destroy'd if disenchanted)
#define ENCHANT_FLAG_SUMMON_2         2 //   Lowest 2 bits used to specify level of summoning (& mana to be refunded on DChant)
#define ENCHANT_FLAG_SUMMON_3         3 //
#define ENCHANT_FLAG_LEVITATE         4 // Jump action allows levitation if held down (should fall if disenchanted)
#define ENCHANT_FLAG_INVISIBLE        8 // Character is invisible (turns visible again if disenchanted)
#define ENCHANT_FLAG_MORPH           16 // Character is morph'd into an alternate form (revert to normal form if disenchanted)
#define ENCHANT_FLAG_DEFLECT         32 // Arrows and other particles deflect off character (glowie sphere goes away if disenchanted) (glow particle shatters if it absorbs too many particles too quickly - should flash with each one it deflects)
#define ENCHANT_FLAG_HASTE           64 // Character moves as if they had double Dex (goes away if disenchanted)
#define ENCHANT_FLAG_MIRROR_DEFLECT 128 // Character is using a mirror shield to deflect stuff...


// Character flags...
#define CHAR_FALL_ON         1      // Character is falling if TRUE...
#define CHAR_FUZZY_ON        2      // Character is drawn in second pass (along with transparent chars...)
#define CHAR_ON_GROUND       4      // Character is standing on the ground...
#define CHAR_GRAVITY_ON      8      // Character is affected by gravity...
#define CHAR_IN_WATER        16     // Character is in/under water
#define CHAR_NO_COLLISION    32     // Character doesn't collide with other characters
#define CHAR_FINISH_ACTION   64     // Character shouldn't switch between walk & stand in mid stride...
#define CHAR_NO_STUCK_PARTS  128    // Character can't have arrows attached to 'em...  Must check manually in script...
#define CHAR_ON_PLATFORM     256    // Character is standing on a platform (used internally)
#define CHAR_CAN_BE_MOUNTED  512    // Character can be mounted
#define CHAR_CAN_RIDE_MOUNT  1024   // Character can ride mounts
#define CHAR_LITTLE_BOX      2048   // Character has a boxsize of 1.0 for room geometry collisions (but not for character-particle or character-character collisions)
#define CHAR_FULL_NETWORK    4096   // All of character's equipment data is sent over network (all armor & actions, not just 2 weapons)
#define CHAR_PLATFORM_ON     8192   // Character can be stood on...
#define CHAR_HOVER_ON        16384  // Character moves with sluggish velz and floats towards a given z position...
#define CHAR_NO_BACKSTAB     32768  // Character is immune to backstabs...


#define PART_FLAGS (*((unsigned short*) (particle_data+60)))
#define CHAR_FLAGS (*((unsigned short*) (character_data+60)))
#define MOUNT_FLAGS (*((unsigned short*) (mount_data+60)))
#define CHECK_FLAGS (*((unsigned short*) (check_data+60)))
#define TARGET_FLAGS (*((unsigned short*) (target_data+60)))


// Simple directions...
#define DIRECTION_FRONT  0
#define DIRECTION_LEFT   1
#define DIRECTION_RIGHT  2
#define DIRECTION_BACK   3


//-Globals---------------------------------------------------------------------------------------
#ifdef DEVTOOL
    SF_EXTERN unsigned char debug_active ; // = FALSE;
#endif


SF_EXTERN unsigned char color_temp[4];
SF_EXTERN unsigned char black[4] ; // = {0, 0, 0, 128};
SF_EXTERN unsigned char red[4] ; // = {255, 0, 0, 128};
SF_EXTERN unsigned char line_color[4] ; // = {0, 0, 0, 128};
SF_EXTERN unsigned char dark_red[4] ; // = {150, 0, 0, 128};
SF_EXTERN unsigned char green[4] ; // = {0, 255, 0, 128};
SF_EXTERN unsigned char dark_green[4] ; // = {0, 70, 0, 200};
SF_EXTERN unsigned char light_green[4] ; // = {160, 255, 160, 128};
SF_EXTERN unsigned char blue[4] ; // = {0, 0, 255, 128};
SF_EXTERN unsigned char cyan[4] ; // = {0, 255, 255, 128};
SF_EXTERN unsigned char magenta[4] ; // = {255, 0, 255, 128};
SF_EXTERN unsigned char grey[4] ; // = {128, 128, 128, 128};
SF_EXTERN unsigned char white[4] ; // = {255, 255, 255, 80};
SF_EXTERN unsigned char shadow_color[4] ; // = {255, 255, 255, 255};
SF_EXTERN unsigned char gold[4] ; // = {230, 198, 25, 128};
SF_EXTERN unsigned char brown[4] ; // = {84, 30, 9, 200};
SF_EXTERN unsigned char bronze[4] ; // = {244, 180, 119, 128};
SF_EXTERN unsigned char yellow[4] ; // = {255, 255, 0, 128};
SF_EXTERN unsigned char med_yellow[4] ; // = {210, 200, 5, 128};
SF_EXTERN unsigned char dark_yellow[4] ; // = {170, 143, 5, 128};
SF_EXTERN unsigned char faded_yellow[4] ; // = {255, 255, 0, 64};
SF_EXTERN unsigned char whiter[4] ; // = {255, 255, 255, 180};
SF_EXTERN unsigned char whitest[4] ; // = {255, 255, 255, 255};
SF_EXTERN unsigned char instrument_color[8][4] ; /* = {{255,   0,   0, 128},
                                        {  0, 255,   0, 128},
                                        {  0,   0, 255, 128},
                                        {255, 128, 255, 128},
                                        {128, 255, 255, 128},
                                        {255, 255, 128, 128},
                                        {255, 192, 192, 128},
                                        {255, 255, 255, 128}}; */



SF_EXTERN unsigned short map_room_door_spin ; // = 0;  // For telling players their new spin on room change...



SF_EXTERN unsigned char* current_object_data;  // Should be filled before run_script...
SF_EXTERN unsigned short current_object_item;  // Ditto
SF_EXTERN unsigned char quitgame;
SF_EXTERN unsigned int main_game_frame;        // Should update at a rate of GAME_FRAMES_PER_SECOND (but only when game is running...)
SF_EXTERN unsigned int main_video_frame;       // Number of times the screen has been drawn...
SF_EXTERN unsigned int main_video_frame_skipped;  // Should update at a rate of GAME_FRAMES_PER_SECOND (even when game isn't active...)
SF_EXTERN unsigned short main_frame_skip ; // = 0;  // Number of game frames in last update...
SF_EXTERN unsigned char* random_table ; // = NULL;  // The random number table


// Network and game system stuff...
SF_EXTERN unsigned char main_game_active;         // TRUE if the local machine has joined or started a game
SF_EXTERN unsigned char play_game_active;         // TRUE if the local players are actually playing (ie characters are spawned and runnin' around)
SF_EXTERN unsigned char paying_customer ; // = FALSE;  // Is the local machine a paying customer?
SF_EXTERN unsigned char update_active ; // = FALSE;    // For updating files in the datafile...


// Heartbeat stuff...
SF_EXTERN unsigned int time_since_i_got_heartbeat;// Frames since I got a heartbeat from somebody...
SF_EXTERN unsigned int time_since_i_sent_heartbeat;// Frames since I sent a heartbeat from somebody...


SF_EXTERN unsigned char mip_map_active ; // = FALSE;
SF_EXTERN unsigned char fast_and_ugly_active ; // = FALSE;

// need to be power of 2 for bit masking
#define MAX_SCREEN_SIZES 16
SF_EXTERN unsigned short screen_sizes_xy[MAX_SCREEN_SIZES][2] ;

SF_EXTERN int screen_x; // =400;                       // The x screen size
SF_EXTERN int screen_y; // =300;                       // The y screen size
SF_EXTERN float script_window_x;
SF_EXTERN float script_window_y;
SF_EXTERN float script_window_scale ; // = 6.0f;



// For door collisions...
SF_EXTERN unsigned char global_room_changed;


// For water...
#define WATER_TYPE_WATER 0
#define WATER_TYPE_LAVA  1
#define WATER_TYPE_SAND  2
SF_EXTERN float room_water_level ; // = -2.0f;
SF_EXTERN unsigned char room_water_type ; // = WATER_TYPE_WATER;
SF_EXTERN unsigned char room_metal_box_item ; // = 0;


// For Window3D
SF_EXTERN float window3d_x;
SF_EXTERN float window3d_y;
SF_EXTERN float window3d_w;
SF_EXTERN float window3d_h;
SF_EXTERN float window3d_camera_xyz[3];
SF_EXTERN float window3d_target_xyz[3];
SF_EXTERN unsigned char window3d_order;


#define MAX_STRING 16                           // Global string variables for string functions...
#define MAX_STRING_SIZE 256                     //
SF_EXTERN char run_string[MAX_STRING][MAX_STRING_SIZE];   //
SF_EXTERN char number_string[MAX_STRING_SIZE];            //



#define MAX_CHARACTER 128 // 1024  // The maximum number of characters in a single room
#define MAX_CHARACTER_SHIFT 7 // 10 // 1<<10 == MAX_CHARACTER
#define MAX_PARTICLE 4096 // 1024   // The maximum number of particles in a single room
#define MAX_PARTICLE_SHIFT 12 // 10 // 1<<10 == MAX_PARTICLE


// Bone frame pointers...  Should point into mainbuffer...
SF_EXTERN unsigned char* temp_character_bone_frame[MAX_CHARACTER];


// Table that tells us which bone in a model has a given name/id...  Have to either call render_rdy
// or special render_fill_temp____ function...
SF_EXTERN unsigned char temp_character_bone_number[8];


// Frame event flags for last render'd character...
SF_EXTERN unsigned char temp_character_frame_event;


// Frame events...
#define FRAME_EVENT_LEFT    16
#define FRAME_EVENT_RIGHT   32
#define FRAME_EVENT_BLOCK   64
#define FRAME_EVENT_COMBAT  128



// Bump event callback stuff...
SF_EXTERN float global_bump_normal_xyz[3];
SF_EXTERN unsigned short global_bump_flags;
SF_EXTERN unsigned short global_bump_char;
SF_EXTERN unsigned char global_bump_abort;


// Strings for clicked and unclicked cursor...
SF_EXTERN unsigned char cursor_click[2] ; // = { 10, 0 };
SF_EXTERN unsigned char cursor_normal[2] ; // = { 9, 0 };
SF_EXTERN unsigned char cursor_target[2] ; // = {178, 0};


// Stuff for character collisions...
#define COLLISION_ALLOW_X_MOVEMENT 1
#define COLLISION_ALLOW_Y_MOVEMENT 2
#define COLLISION_ALLOW_Z_MOVEMENT 4
#define COLLISION_DISALLOW_X_MOVEMENT 6
#define COLLISION_DISALLOW_Y_MOVEMENT 5
#define COLLISION_DISALLOW_Z_MOVEMENT 3
SF_EXTERN unsigned short character_collision_list[MAX_CHARACTER];
SF_EXTERN unsigned short num_character_collision_list ; // = 0;
SF_EXTERN unsigned short character_door_collision_list[MAX_CHARACTER];
SF_EXTERN unsigned short num_character_door_collision_list ; // = 0;



// Stuff for room format...
#define SRF_VERTEX_OFFSET           40
#define SRF_TEX_VERTEX_OFFSET       44
#define SRF_TRIANGLE_OFFSET         48
#define SRF_EXTERIOR_WALL_OFFSET    52
#define SRF_TEXTURE_OFFSET          56
#define SRF_HEIGHTMAP_OFFSET        60
#define SRF_WAYPOINT_OFFSET         64
#define SRF_BRIDGE_OFFSET           68
#define SRF_MINIMAP_OFFSET          72
#define SRF_OBJECT_GROUP_OFFSET     76
#define SRF_EDGE_LINE_OFFSET        80


// Uncomment this for debug info...
// #define VERBOSE_COMPILE



//-Macros----------------------------------------------------------------------------------------

#define ABS(A) { A = (A < 0) ? -A : A; }
#define repeat(A, B) for(A=0;  A<B;  A++)
#define clip(LO, A, HI)                                                             \
{                                                                                   \
    A = (A < LO) ? LO : A;                                                          \
    A = (A > HI) ? HI : A;                                                          \
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro is the main script entry point...
SF_EXTERN int fast_run_offset;

// Macro with corruption check
SF_EXTERN unsigned char looking_for_fast_function ; // = FALSE;
SF_EXTERN unsigned char fast_function_found ; // = FALSE;
#ifdef DEVTOOL
#define fast_run_script(file_start, fast_function, object_data)                         \
{                                                                                       \
    fast_run_offset = sdf_read_unsigned_short(file_start+fast_function);                \
    if(fast_run_offset != 0)                                                            \
    {                                                                                   \
        if(looking_for_fast_function) {fast_function_found = TRUE;}                     \
        looking_for_fast_function = FALSE;                                              \
        current_object_data = object_data;                                              \
        current_object_item = 0;                                                        \
        run_script(file_start+fast_run_offset, file_start, 0, NULL, 0, NULL);           \
        if(current_object_data != object_data)                                          \
        {                                                                               \
            sprintf(DEBUG_STRING, "CURRENT_OBJECT WAS CORRUPTED!!!");                   \
        }                                                                               \
    }                                                                                   \
}
#else
// Macro without corruption check
#define fast_run_script(file_start, fast_function, object_data)                         \
{                                                                                       \
    fast_run_offset = sdf_read_unsigned_short(file_start+fast_function);                \
    if(fast_run_offset != 0)                                                            \
    {                                                                                   \
        if(looking_for_fast_function) {fast_function_found = TRUE;}                     \
        looking_for_fast_function = FALSE;                                              \
        current_object_data = object_data;                                              \
        current_object_item = 0;                                                        \
        run_script(file_start+fast_run_offset, file_start, 0, NULL, 0, NULL);           \
    }                                                                                   \
}
#endif



//-Things I Dislike Doing------------------------------------------------------------------------
signed char decode_jpg(unsigned char* index, unsigned char* filename);
signed char decode_pcx(unsigned char* index, unsigned char* filename);
signed char decode_ogg(unsigned char* index, unsigned char* filename);
signed char decode_ddd(unsigned char* index, unsigned char* filename);
signed char run_script(unsigned char* address, unsigned char* file_start, unsigned char num_int_args, signed int* int_arg_stack, unsigned char num_float_args, float* float_arg_stack);
void language_message_add(unsigned short message_index);
void message_add(char* message_text, char* speaker_name, unsigned char sanitize);
void setup_shadow(void);
unsigned char* render_get_set_external_linkage(unsigned char* data, unsigned char* link_data);
unsigned char render_pregenerate_normals(unsigned char* data, unsigned short frame, unsigned char detail_level);
unsigned char particle_attach_to_character(unsigned short particle, unsigned short character, unsigned char bone_type);
void network_send(unsigned char remotes_to_send_to);
void display_loadin(float amount_done);
void render_rdy(unsigned char* data, unsigned short frame, unsigned char mode, unsigned char** texture_file_data_block, unsigned char main_alpha, unsigned char* bone_frame_data, unsigned char petrify, unsigned char eye_frame);
unsigned char* render_generate_model_world_data(unsigned char* data, unsigned short frame, float* matrix, unsigned char* write);
void render_rdy_shadow(unsigned char* data, unsigned short frame, float x, float y, float z, unsigned char mode);
void character_button_function(unsigned short character, unsigned char code, unsigned char button, unsigned char axis);
float room_heightmap_height_slow(unsigned char* data, float x, float y);
void script_matrix_good_bone(unsigned char bone, unsigned char* data_start, unsigned char* data);
void particle_draw(unsigned char* particle_data);
void room_srf_build_waypoint_table(unsigned char* srf_file);
void water_drown_delay_setup();
void character_bone_frame_clear();
void character_update_all();
unsigned char network_find_script_index(unsigned char* filename);

//-----------------------------------------------------------------------------------------------

extern int  sdf_num_files;
extern char sdf_extension[16][4];      // The extension strings that the program recognizes

signed char sdf_export_file(char* filename, char* filename_path);
signed char sdf_add_file(char* sdf_filename, char* input_filename);
signed char sdf_save(char* filename);
signed char sdf_allocate();

signed char sdf_get_filename(int index, char* filename, unsigned char* filetype);
signed char sdf_load(const char *datafile);
void sdf_unload(void);

signed char src_compile_archive(unsigned char stage);

#define SRC_HEADERIZE   1   // The first stage of compilation
#define SRC_COMPILERIZE 2   // The second stage of compilation
#define SRC_FUNCTIONIZE 3   // The third stage of compilation
