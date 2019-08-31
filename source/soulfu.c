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

// Stuff for SDL
#include <SDL/SDL.h>
#include <SDL/SDL_opengl.h>
#include <SDL/SDL_net.h>

#define EXECUTABLE_VERSION   "1.5 NICEWARE"

unsigned short onscreen_draw_count;

#include "soulfu.h"

#include "logfile.c"
#include "gameseed.c"
#include "common.c"
#include "datafile.c"
#include "random.c"
#include "object.c"
#include "dcodejpg.c"
#include "dcodepcx.c"
#include "dcodeogg.c"
#include "dcodeddd.c"
#include "display.c"
#include "sound.c"
#include "input.c"
#include "tool.c"
#include "dcodesrc.c"
#include "page.c"
#include "message.c"
#include "render.c"
#include "volume.c"
#include "room.c"
#include "map.c"
#include "water.c"
#include "damage.c"
#include "experi.c"
#include "item.c"
#include "particle.c"
#include "charact.c"
#include "network.c"
#include "runsrc.c"

#ifndef _LIB
//-----------------------------------------------------------------------------------------------
void main_loop(void)
{
  int i, j;
  float x, y, brx, bry;
  char* alt_text;
  unsigned char pause_active = FALSE;


  log_message("INFO:   Starting main loop");
  display_zbuffer_on();
  display_cull_on();


  // Loop as long as escape isn't pressed
  main_game_frame = 0;
  main_video_frame = 0;
  main_video_frame_skipped = 0;
  quitgame=FALSE;
  while(quitgame == FALSE)
  {
    main_timer_start();     // Game timer...


    // Draw the world, if we're actually playing the game...
    color_temp[0] = 0;  color_temp[1] = 0;  color_temp[2] = 0;
    if(play_game_active)
    {
      if(map_current_room < MAX_MAP_ROOM)
      {
        if(map_room_data[map_current_room][13] & MAP_ROOM_FLAG_OUTSIDE)
        {
          color_temp[0] = 64;  color_temp[1] = 64;  color_temp[2] = 255;
        }
      }
      display_clear_buffers();



      if(key_pressed[SDLK_F9])
      {
        pause_active=!pause_active;
        DEBUG_STRING[0] = 0;
      }
      // !!!BAD!!!
      // !!!BAD!!!
      // !!!BAD!!!
      //            #ifdef DEVTOOL
      //                if(key_pressed[SDLK_SPACE])
      //                {
      //                    if(network_on)
      //                    {
      //                        network_send_room_update();
      //                    }
      //                }
      //                if(key_pressed[SDLK_F8])
      //                {
      //                    log_message("INFO:   Dumping all particle information...");
      //                    repeat(i, MAX_PARTICLE)
      //                    {
      //                        if(main_particle_on[i])
      //                        {
      //                            obj_get_script_name(main_particle_script_start[i], DEBUG_STRING);
      //                            log_message("INFO:     Particle %d is of type %s.RUN", i, DEBUG_STRING);
      //                            DEBUG_STRING[0] = 0;
      //                        }
      //                        else
      //                        {
      //                            log_message("INFO:     Particle %d is off", i);
      //                        }
      //                    }
      //                }
      //            #endif
      // !!!BAD!!!
      // !!!BAD!!!
      // !!!BAD!!!

      // Move characters around...
      main_frame_skip = 0;
      if(main_timer_length >= 16)
      {
        // Only do up to 4 updates per drawn frame to ensure that the game doesn' get totally
        // hosed from being too slow to update (and constantly has more and more frames
        // to catch up on...)
        while(main_timer_length >= 16 && main_frame_skip < 4)
        {
          character_local_player_control();
          if(!pause_active)
          {
            if(global_luck_timer > 0)
            {
              global_luck_timer--;
            }
            character_update_all();
            character_collide_all();
            particle_update_all();
            character_bone_frame_all();
            character_refresh_items_all();
          }
          else
          {
            character_bone_frame_all();
          }


          main_timer_length-=16;
          main_game_frame++;
          main_frame_skip++;
        }
      }
      else
      {
        character_local_player_control();
        character_bone_frame_all();
      }



      onscreen_draw_count = 0;


      // Figure out the camera position...
      display_camera_position(main_frame_skip, 0.95f, 0.99f);


      // Rotate the camera...
      display_look_at(camera_xyz, target_xyz);
      // Remember camera facing for automap...  Use side, because fore can look down and
      // that screws up the scaling...
      map_side_xy[X] = camera_side_xyz[X];
      map_side_xy[Y] = camera_side_xyz[Y];



      // Draw the world scene...
      drawing_world = TRUE;
      room_draw(roombuffer);
      if(volumetric_shadows_on)
      {
        volume_shadow_draw_all();
      }
      else
      {
        character_shadow_draw_all();
      }
      character_draw_all_prime();
      character_draw_all(FALSE, FALSE);
      if(room_water_type == WATER_TYPE_WATER)
      {
        particle_draw_below_water();
      }
      water_draw_room(roombuffer);
      character_draw_all(TRUE, FALSE);
      if(room_water_type == WATER_TYPE_WATER)
      {
        particle_draw_above_water();
      }
      else
      {
        particle_draw_below_water();
        particle_draw_above_water();
      }
      character_draw_all(FALSE, TRUE);






      //sprintf(DEBUG_STRING, "%d chars drawn", onscreen_draw_count);


      // Draw axis arrows...
      //            #ifdef DEVTOOL
      //                if(debug_active)
      //                {
      //                    display_blend_off();
      //                    display_texture_off();
      //                    render_axis();
      //                    display_marker(blue, 0.0f, 0.0f, 6.0f, 1.0f);  // 6ft height marker...
      //                    display_marker(green, 0.0f, 6.0f, 0.1f, 1.0f);  // Y axis marker
      //                    display_marker(red, 6.0f, 0.0f, 0.1f, 1.0f);  // X axis marker
      //                }
      //            #endif
    }
    else
    {
      // Game isn't active, but still run our timers so the window scripts
      // work right...
      pause_active = FALSE;
      display_clear_buffers();
      character_bone_frame_clear();
      main_frame_skip = 0;
      while(main_timer_length >= 16)
      {
        main_frame_skip++;
        main_timer_length-=16;
      }
    }
    main_video_frame_skipped+=main_frame_skip;



    // Draw window stuff...
    drawing_world = FALSE;
    glLoadMatrixf(window_camera_matrix);
    display_zbuffer_off();
    display_texture_on();
    display_blend_trans();
    display_cull_on();


    promotion_count = 0;
    repeat(i, main_used_window_count)
    {
#ifdef DEVTOOL
      if(i < 0)
      {
        log_message("ERROR:  ");
        log_message("ERROR:  ");
        log_message("ERROR:  Window Poof Count, i == %d", i);
        log_message("ERROR:  ");
        log_message("ERROR:  ");
        exit(0);
      }
#endif
      j = main_window_order[i];
      main_window_poof_count = 0;
      window3d_order = MAX_WINDOW - ((unsigned char) i);
      fast_run_script(main_window_script_start[j], FAST_FUNCTION_REFRESH, main_window_data[j]);
#ifdef DEVTOOL
      if(current_object_data != main_window_data[j])
      {
        sprintf(DEBUG_STRING, "Window current object corrupted...");
        log_message("ERROR:  Window current object corrupted...");
      }
#endif
      i-=main_window_poof_count;
    }
    repeat(i, promotion_count)
    {
      promote_window(promotion_buffer[i]);
    }






    // Handle funky enchantment targeting clicks...
    if(enchant_cursor_active)
    {
      // Constantly check for dead character...
      enchant_cursor_active = FALSE;
      if(enchant_cursor_character < MAX_CHARACTER)
      {
        if(main_character_on[enchant_cursor_character])
        {
          if(main_character_data[enchant_cursor_character][82] > 0)
          {
            enchant_cursor_active = TRUE;
          }
        }
      }
    }
    if(mouse_draw && enchant_cursor_active)
    {
      // Is player still alive?
      if(mouse_pressed[BUTTON0])
      {
        // Was it clicked on a character?
        if(mouse_current_object==NULL && mouse_current_item < MAX_CHARACTER)
        {
          // Figger out who the target is...
          enchant_cursor_target = mouse_current_item;
          enchant_cursor_target_item = 0;  // Means character itself...
          call_enchantment_function();
          if(return_int)
          {
            // Revert to normal cursor if EnchantUsage() returns TRUE
            enchant_cursor_active = FALSE;
          }
        }
      }
    }



    // Handle input changes...
    input_update();


    // Draw the selection box
#ifdef DEVTOOL
    selection_close_type = 0;
    if(select_pick_on && mouse_down[BUTTON0] == 0)
    {
      select_pick_on = FALSE;
    }
    if(select_move_on && mouse_down[BUTTON0] == 0)
    {
      select_move_on = FALSE;
      selection_close_type = BORDER_MOVE;
    }
    if(selection_box_on)
    {
      // It's on...
      render_box();

      // Close if button released
      if(mouse_down[BUTTON0]==0)
      {
        selection_box_on = FALSE;
        selection_close_type = BORDER_SELECT;
        // Ensure that top left is less than bottom right...
        if(selection_box_tl[X] > selection_box_br[X])
        {
          x = selection_box_tl[X];
          selection_box_tl[X] = selection_box_br[X];
          selection_box_br[X] = x;
        }
        if(selection_box_tl[Y] > selection_box_br[Y])
        {
          y = selection_box_tl[Y];
          selection_box_tl[Y] = selection_box_br[Y];
          selection_box_br[Y] = y;
        }

      }
    }
#endif





    // Draw the cursor...
    display_pick_texture(texture_ascii);
    if(mouse_idle_timer > 1800 && mouse_alpha > 0)
    {
      mouse_alpha--;
    }
    if(mouse_idle_timer < 30 && mouse_alpha < 255)
    {
      mouse_alpha++;
      mouse_alpha = (mouse_alpha + mouse_alpha + mouse_alpha + 255)>>2;
    }
    color_temp[0] = 255;  color_temp[1] = 255;  color_temp[2] = 255;  color_temp[3] = mouse_alpha;
    display_color_alpha(color_temp);
    if(mouse_draw)
    {
      x = mouse_x;
      y = mouse_y;

      // Draw the cursor
      if(enchant_cursor_active)
      {
        display_string(cursor_target, x-7.5f, y-7.5f, 15);
      }
      else
      {
        if(mouse_down[BUTTON0])
        {
          // Draw a click cursor...
          display_string(cursor_click, x, y, 15);
        }
        else
        {
          // Draw a normal cursor...
          display_string(cursor_normal, x, y, 15);
        }
      }


      // Draw the alternate text...
      if(mouse_text[0])
      {
        x+=12.0f;
        y+=12.0f;
        alt_text = mouse_text;
        brx = x+script_window_scale*(strlen(alt_text)+1);
        bry = y+(script_window_scale*1.2f);
        if(brx > 400.0f)
        {
          x = 400.0f - (brx-x);
          brx = 400.0f;
        }
        display_color_alpha(whiter);
        display_mouse_text_box(x, y, brx, bry, texture_winalt);
        display_color(white);
        display_pick_texture(texture_ascii);
        display_string(alt_text, x+(script_window_scale*.5f), y+(script_window_scale*0.1f), script_window_scale);
        if(mouse_text_timer == 0)
        {
          mouse_text[0] = 0;
        }
      }
    }


    // Draw the debug string
    display_color(white);
    if(compiler_error)
    {
      //            display_string("Compiler error...  See logfile.txt", 0, 290.0f, 10.0f);
    }
    else
    {
#ifdef DEVTOOL
      if(key_down[SDLK_F11])
      {
        sprintf(DEBUG_STRING, "Obj == %u, Item == %u", mouse_last_object, mouse_last_item);
      }
      if(!pause_active)
      {
        display_string(DEBUG_STRING, 0, 290.0f, 10.0f);
      }
#endif
    }
    //#ifndef DEVTOOL
    //        display_string("DEVTOOL OFF", 290.0f, 290.0f, 10.0f);
    //#endif

    if(pause_active)
    {
      if(play_game_active)
      {
        display_string("-Paused-", 140.0, 15.0f, 15.0f);
      }
    }

#ifdef DEVTOOL
    //        if(!pause_active)
    //        {
    //            if(play_game_active)
    //            {
    //                display_string("Play", 0.0, 0.0f, 10.0f);
    //            }
    //            if(main_game_active)
    //            {
    //                display_string("Main", 50.0, 0.0f, 10.0f);
    //            }
    //        }
#endif



    // Do fade out effect...
    display_texture_off();
    display_blend_off();
#ifdef DEVTOOL
#ifdef SHOW_JOYSTICK_POSITIONS
    repeat(i, MAX_LOCAL_PLAYER)
    {
      x = (400.0f/(MAX_LOCAL_PLAYER+1)) * (i+1);
      y = 275.0f;

      display_color(white);
      display_start_line();
      display_vertex_xy(x-2.0f, y);
      display_vertex_xy(x+2.0f, y);
      display_end();
      display_start_line();
      display_vertex_xy(x, y-2.0f);
      display_vertex_xy(x, y+2.0f);
      display_end();

      display_color(instrument_color[i]);
      display_start_line();
      display_vertex_xy(x, y);
      x+=player_device_xy[i][X]*25.0f;
      y+=player_device_xy[i][Y]*25.0f;
      display_vertex_xy(x, y);
      display_end();
    }
#endif
#endif
    display_shade_on();
    display_fade();
    display_shade_off();
    display_zbuffer_on();



    // Display everything once it's drawn...
    display_swap();


    // Reset our input...  Do once per display
    input_reset_window_key_pressed();
    input_read();
    fill_music();
    fill_sound_buffer();
    // !!!BAD!!!
    // !!!BAD!!!
    // !!!BAD!!!
    //        network_listen();
    // !!!BAD!!!
    // !!!BAD!!!
    // !!!BAD!!!


    // Handle camera controls...
    input_camera_controls();



    main_video_frame++;
#ifdef DEVTOOL
    if(key_pressed[SDLK_F10])
    {
      debug_active = (debug_active+1)&1;
    }
    //            if(key_pressed[SDLK_F4])
    //            {
    //                display_start_fade(FADE_TYPE_FULL, FADE_OUT, 0, 0, black);
    //            }
    //            if(key_pressed[SDLK_F3])
    //            {
    //                display_start_fade(FADE_TYPE_WARNING, FADE_OUT, 0, 0, cyan);
    //            }
    //            if(key_pressed[SDLK_F2])
    //            {
    //                display_start_fade(FADE_TYPE_CIRCLE, FADE_OUT, 200.0f, 150.0f, black);
    //            }
#endif
    main_timer_end();
  }
}

// Ugly hack to test if we can open a file using STDIO.
char file_can_open(const char *filename, const char *mode)
{
  FILE *f = fopen(filename, mode);

  if(!f) return FALSE;
  fclose(f);
  return TRUE;
}

//-----------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
  int i;
  unsigned char* script;
  unsigned char* config;
  unsigned char bit_depth;
  unsigned char z_depth;    
  unsigned char full_screen;
  unsigned char screen_size;
  unsigned char* data;
  char *home, configfile[1024];
  char config_loaded = FALSE;


  open_logfile();
  log_message("INFO:   ------------------------------------------");
  if(!get_mainbuffer()) { log_message("ERROR:  get_mainbuffer() failed");  exit(1); }
  if(!sdf_load("datafile.sdf")) {
    log_message("ERROR:  sdf_load() failed");
    exit(1);
  }


  // !!!BAD!!!
  // !!!BAD!!!
  // !!!BAD!!!
  //    network_setup();
  // !!!BAD!!!
  // !!!BAD!!!
  // !!!BAD!!!
  display_kanji_setup();
  data = sdf_find_filetype("VERSION", SDF_FILE_IS_DAT);
  if(data)
  {
    data = (unsigned char*) sdf_read_unsigned_int(data);
  }
  log_message("INFO:   SoulFu version %s", EXECUTABLE_VERSION);
  generate_game_seed();
  log_message("INFO:   ------------------------------------------");


  // Make sure the game is turned off...
  play_game_active = FALSE;
  main_game_active = FALSE;


  // Try to not break it too bad...
  // XXX: We need a standard place to save output sdf files from the DevTool.
#ifdef DEVTOOL
  sdf_save("backup.sdf");
#endif


  // Load the config file from disk, if there is one...
  // XXX: Better fallback to the builtin if any of the loaded configs
  //      are corrupt.
  home = getenv("HOME");
  if(home) {
    snprintf(configfile, 1023, "%s/.soulfu.dat", home);
    if(file_can_open(configfile, "r")) {
      if(sdf_add_file("CONFIG.DAT", configfile)) config_loaded = TRUE;
    }
  } else strcpy(configfile, "CONFIG.DAT");

  if(!config_loaded && file_can_open("CONFIG.DAT", "r"))
    sdf_add_file("CONFIG.DAT", "CONFIG.DAT");
   

  // Read the display settings from the config file...
  config = sdf_find_filetype("CONFIG", SDF_FILE_IS_DAT);
  if(config)
  {
    config = (unsigned char*) sdf_read_unsigned_int(config);
    screen_size = (*(config+68)) & (MAX_SCREEN_SIZES-1);
    bit_depth = *(config+69);
    z_depth = *(config+70);
    volumetric_shadows_on = (*(config+71))&1;
    full_screen = *(config+72);
    mip_map_active = (*(config+98));
    fast_and_ugly_active = (*(config+101));
    log_message("INFO:   Config file read okay...");
    if(!display_setup(screen_sizes_xy[screen_size][X], screen_sizes_xy[screen_size][Y], bit_depth, z_depth, full_screen)) { log_message("ERROR:  display_setup() failed");  exit(1); }
  }
  else
  {
    log_message("ERROR:  No config file present");
    exit(1);
  }


  // Draw a please wait type of thing onscreen...
  display_loadin(0.0f);


  //    sdf_add_file("DEFINE.TXT", NULL);  // !!!BAD!!!  Allow me to constantly update the global defines

  sine_table_setup();
  if(!random_setup(game_seed)) { log_message("ERROR:  random_setup() failed");  exit(1); }
  if(!src_define_setup()) { log_message("ERROR:  src_define_setup() failed"); exit(1); }


#ifdef DEVTOOL
  //    sdf_delete_all_files(SDF_FILE_IS_RUN, NULL);
  sdf_list_all_files(SDF_FILE_IS_SRC, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_SRF, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_DDD, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_JPG, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_PCX, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_OGG, NULL, FALSE);
  sdf_list_all_files(SDF_FILE_IS_MUS, NULL, FALSE);
  //    sdf_list_all_files(SDF_FILE_IS_SRC, NULL, TRUE);
  //    sdf_list_all_files(SDF_FILE_IS_OGG, NULL, TRUE);
#endif

  // Decode all of the files...
  sdf_decode(SDF_ALL, TRUE);


  ddd_magic_update_thing(SDF_ALL);
  display_loadin(0.50f);
  render_crunch_all(SDF_ALL);
  display_loadin(0.60f);
  sdf_flag_clear(SDF_FLAG_WAS_UPDATED);

  // set-up the messaging system
  message_reset();
  message_setup();


  // go through all compilation stages for the script files in the archive
  compiler_error = FALSE;
  for(i=SRC_HEADERIZE; i<=SRC_FUNCTIONIZE; i++)
  {
    src_compile_archive(i);
    display_loadin(0.60f + 0.1f*i);
  };


  damage_setup();
  input_setup();
  sound_setup();
  page_setup();
  render_shadow_setup();
  item_type_setup();
  water_setup();
  water_drown_delay_setup();
  character_action_setup();
  display_load_all_textures();
  display_loadin(1.00f);



  // Flag certain files to not be updated...  (This is outdated sorta stuff...  No longer do FTP stuff...)
  sdf_flag_set("RANDOM.DAT", SDF_FLAG_NO_UPDATE);
  sdf_flag_set("CONFIG.DAT", SDF_FLAG_NO_UPDATE);





  // Start by spawning the first window object...  Main menu/Title...
  obj_setup();
  promotion_count = 0;
  script = sdf_find_filetype("WSTART", SDF_FILE_IS_RUN);
  if(script)
  {
    script = (unsigned char*) sdf_read_unsigned_int(script);
    obj_spawn(WINDOW, 200.0f, 150.0f, 0, script, MAX_WINDOW);
    repeat(i, promotion_count)
    {
      promote_window(promotion_buffer[i]);
    }
  }


  // !!!BAD!!!
  // !!!BAD!!!  SUPER BAD!!!  Used to get rid of all script texts for final release...
  // !!!BAD!!!
  //    sdf_delete_all_files(SDF_FILE_IS_SRC, NULL);
  // !!!BAD!!!
  // !!!BAD!!!
  // !!!BAD!!!



  // Do the main loop...
  main_loop();


  // Export the config file to disk, so we don't need to save the whole datafile
  sdf_export_file("CONFIG.DAT", configfile);



  return 0;
}

//-----------------------------------------------------------------------------------------------
#endif

unsigned char update_performed=FALSE;   // Do any files need decompressing?
unsigned short global_block_keyboard_timer = 0;
float script_matrix[12];
unsigned char player_device_type[MAX_LOCAL_PLAYER];  // None, Keyboard, Joystick 1, Joystick 2, Joystick 3, Joystick 4...
unsigned short player_device_button[MAX_LOCAL_PLAYER][MAX_PLAYER_DEVICE_BUTTON];  // Either keyboard scan codes or joystick button numbers...
unsigned char player_device_button_pressed[MAX_LOCAL_PLAYER][4];  // One if button was just pressed, zero if not...
unsigned char player_device_button_unpressed[MAX_LOCAL_PLAYER][4];  // One if button was just let up, zero if not...
unsigned char player_device_button_pressed_copy[MAX_LOCAL_PLAYER][4];
unsigned char player_device_inventory_toggle[MAX_LOCAL_PLAYER];  // TRUE if player pressed the inventory button (and script should toggle the gem)...
unsigned char player_device_inventory_down[MAX_LOCAL_PLAYER];   // TRUE if player is holding the inventory button...
unsigned char player_device_controls_active[MAX_LOCAL_PLAYER];  // FALSE if player controls don't actually move character...  But script can still read 'em...  Used to access inventory...
float player_device_xy[MAX_LOCAL_PLAYER][2];
unsigned char enchant_cursor_active = FALSE;
unsigned short enchant_cursor_character;
unsigned short enchant_cursor_itemindex;
unsigned short enchant_cursor_target;
unsigned char enchant_cursor_target_item;
unsigned char global_render_light_color_rgb[3];
float global_render_light_offset_xy[2];
unsigned short local_player_character[MAX_LOCAL_PLAYER];
unsigned char input_active = FALSE;
unsigned char user_language = 0;
int user_language_phrases = 0;
unsigned char bad_string[] = "Bad";        // Used as return on language failures...
unsigned char* language_file[LANGUAGE_MAX];
unsigned char* kanji_data;
unsigned char drawing_world = FALSE;
unsigned short num_onscreen_joint;
unsigned char onscreen_joint_active = FALSE;
unsigned short onscreen_joint_character;
unsigned char* onscreen_joint_character_data;
unsigned char volumetric_shadows_on = FALSE;
unsigned char global_button_handled = FALSE;
unsigned char debug_active = FALSE;
unsigned char color_temp[4];
unsigned char black[4] = {0, 0, 0, 128};
unsigned char red[4] = {255, 0, 0, 128};
unsigned char line_color[4] = {0, 0, 0, 128};
unsigned char dark_red[4] = {150, 0, 0, 128};
unsigned char green[4] = {0, 255, 0, 128};
unsigned char dark_green[4] = {0, 70, 0, 200};
unsigned char light_green[4] = {160, 255, 160, 128};
unsigned char blue[4] = {0, 0, 255, 128};
unsigned char cyan[4] = {0, 255, 255, 128};
unsigned char magenta[4] = {255, 0, 255, 128};
unsigned char grey[4] = {128, 128, 128, 128};
unsigned char white[4] = {255, 255, 255, 80};
unsigned char shadow_color[4] = {255, 255, 255, 255};
unsigned char gold[4] = {230, 198, 25, 128};
unsigned char brown[4] = {84, 30, 9, 200};
unsigned char bronze[4] = {244, 180, 119, 128};
unsigned char yellow[4] = {255, 255, 0, 128};
unsigned char med_yellow[4] = {210, 200, 5, 128};
unsigned char dark_yellow[4] = {170, 143, 5, 128};
unsigned char faded_yellow[4] = {255, 255, 0, 64};
unsigned char whiter[4] = {255, 255, 255, 180};
unsigned char whitest[4] = {255, 255, 255, 255};
unsigned char instrument_color[8][4] = 
{
  {255,   0,   0, 128},
  {  0, 255,   0, 128},
  {  0,   0, 255, 128},
  {255, 128, 255, 128},
  {128, 255, 255, 128},
  {255, 255, 128, 128},
  {255, 192, 192, 128},
  {255, 255, 255, 128}
};

unsigned short map_room_door_spin = 0;  // For telling players their new spin on room change...
unsigned char* current_object_data;  // Should be filled before run_script...
unsigned short current_object_item;  // Ditto
unsigned char quitgame;
unsigned int main_game_frame;        // Should update at a rate of GAME_FRAMES_PER_SECOND (but only when game is running...)
unsigned int main_video_frame;       // Number of times the screen has been drawn...
unsigned int main_video_frame_skipped;  // Should update at a rate of GAME_FRAMES_PER_SECOND (even when game isn't active...)
unsigned short main_frame_skip = 0;  // Number of game frames in last update...
unsigned char* random_table = NULL;  // The random number table
unsigned char main_game_active;         // TRUE if the local machine has joined or started a game
unsigned char play_game_active;         // TRUE if the local players are actually playing (ie characters are spawned and runnin' around)
unsigned char paying_customer = FALSE;  // Is the local machine a paying customer?
unsigned char update_active = FALSE;    // For updating files in the datafile...
unsigned int time_since_i_got_heartbeat;// Frames since I got a heartbeat from somebody...
unsigned int time_since_i_sent_heartbeat;// Frames since I sent a heartbeat from somebody...
unsigned char mip_map_active = FALSE;
unsigned char fast_and_ugly_active = FALSE;
unsigned short screen_sizes_xy[MAX_SCREEN_SIZES][2] =
{
  {640, 480},
  {800, 600},
  {1024, 768},
  {1280, 720},
  {1280, 1024},
  {1366, 768},
  {1600, 1200},
  {1920, 1080}
};
int screen_x=400;                       // The x screen size
int screen_y=300;                       // The y screen size
float script_window_x;
float script_window_y;
float script_window_scale = 6.0f;
unsigned char global_room_changed;
float room_water_level = -2.0f;
unsigned char room_water_type = WATER_TYPE_WATER;
unsigned char room_metal_box_item = 0;
float window3d_x;
float window3d_y;
float window3d_w;
float window3d_h;
float window3d_camera_xyz[3];
float window3d_target_xyz[3];
unsigned char window3d_order;
char run_string[MAX_STRING][MAX_STRING_SIZE];   //
char number_string[MAX_STRING_SIZE];            //
unsigned char* temp_character_bone_frame[MAX_CHARACTER];
unsigned char temp_character_bone_number[8];
unsigned char temp_character_frame_event;
float global_bump_normal_xyz[3];
unsigned short global_bump_flags;
unsigned short global_bump_char;
unsigned char global_bump_abort;
unsigned char cursor_click[2] = { 10, 0 };
unsigned char cursor_normal[2] = { 9, 0 };
unsigned char cursor_target[2] = {178, 0};
unsigned short character_collision_list[MAX_CHARACTER];
unsigned short num_character_collision_list = 0;
unsigned short character_door_collision_list[MAX_CHARACTER];
unsigned short num_character_door_collision_list = 0;
int fast_run_offset;
unsigned char looking_for_fast_function = FALSE;
unsigned char fast_function_found = FALSE;
