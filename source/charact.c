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

// <ZZ> This file has stuff for moving characters and stuff.  Stuff.
#define CLIMB_HEIGHT 1.75f
#define WALL_COLLISION_DELAY 20
#define BOUNCE_SCALE -0.35f

//-----------------------------------------------------------------------------------------------
unsigned char action_can_turn[ACTION_MAX];
void character_action_setup()
{
    // <ZZ> Sets up an array...
    unsigned short i;

    repeat(i, ACTION_MAX)
    {
        action_can_turn[i] = TRUE;
    }
    action_can_turn[ACTION_STUN_BEGIN] = FALSE;
    action_can_turn[ACTION_STUN] = FALSE;
    action_can_turn[ACTION_STUN_END] = FALSE;
    action_can_turn[ACTION_KNOCK_OUT_BEGIN] = FALSE;
    action_can_turn[ACTION_KNOCK_OUT] = FALSE;
    action_can_turn[ACTION_KNOCK_OUT_STUN] = FALSE;
    action_can_turn[ACTION_KNOCK_OUT_END] = FALSE;
    action_can_turn[ACTION_BLOCK_BEGIN] = FALSE;
    action_can_turn[ACTION_BLOCK] = FALSE;
    action_can_turn[ACTION_BLOCK_END] = FALSE;
    action_can_turn[ACTION_SPECIAL_4] = FALSE;

    action_can_turn[ACTION_SPECIAL_10] = FALSE;
    action_can_turn[ACTION_SPECIAL_11] = FALSE;
    action_can_turn[ACTION_SPECIAL_12] = FALSE;
}

//-----------------------------------------------------------------------------------------------
void character_collide_all()
{
    // <ZZ> Makes a list of all the characters that need to be checked for collisions...
    //      Also does character-character collisions...
    unsigned short i, j;
    unsigned short check;
    unsigned char* check_data;
    float* check_xyz;
    float* check_vel_xyz;
    float* check_front_xyz;
    unsigned short character;
    unsigned char* character_data;
    float* character_xyz;
    float* character_vel_xyz;
    float distance;
    float collide_distance;
    float disx, disy, disz;
    float clearance;
    unsigned char* mount_rdy_file;


    // Start by building our collision lists...
    num_character_collision_list = 0;
    num_character_door_collision_list = 0;
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            // Check no collision flag...
            if(!(*((unsigned short*) (main_character_data[i]+60)) & CHAR_NO_COLLISION))
            {
                // Check mounted character...
                if(*((unsigned short*) (main_character_data[i]+164)) >= MAX_CHARACTER)
                {
                    // Character should collide with doors...
                    character_door_collision_list[num_character_door_collision_list] = i;
                    num_character_door_collision_list++;


                    // Check no collision timer...
                    if(*((unsigned short*) (main_character_data[i]+166)) == 0)
                    {
                        character_collision_list[num_character_collision_list] = i;
                        num_character_collision_list++;
                    }
                }
            }
        }
    }


    // Check each door against each normal character...  (Doors shouldn't be in collision list...  Should be flag'd as CHAR_NO_COLLISION)
    global_room_changed = FALSE;
    repeat(check, MAX_CHARACTER)
    {
        check_data = main_character_data[check];
        if(main_character_on[check] && (check_data[104] & MORE_FLAG_DOOR))
        {
            // The check character is an open/closed door, and we need to make sure no character passes it...
            // Open doors stop characters from going through 'em if the character is too high up (like a
            // dragon flying through the wall above the door)...
            check_xyz = (float*) check_data;
            repeat(j, num_character_door_collision_list)
            {
                character = character_door_collision_list[j];
                character_data = main_character_data[character];
                character_xyz = (float*) character_data;
                collide_distance = *((float*) (character_data+160)) + *((float*) (check_data+160));
                check_vel_xyz = (float*) (check_data+24);
                character_vel_xyz = (float*) (character_data+24);


                // Do a circular collision check...
                disx = character_xyz[X] - check_xyz[X];
                disy = character_xyz[Y] - check_xyz[Y];
                distance = (disx*disx) + (disy*disy);
                collide_distance*=collide_distance;
                if(distance < collide_distance)
                {
                    // Then check the dot to divide the door in two (front and back)...
                    check_front_xyz = (float*) (check_data+120);
                    distance = (disx * check_front_xyz[X]) + (disy * check_front_xyz[Y]);
                    distance = (*((float*) (character_data+160))) + 0.5f - distance;
                    if(distance > 0.0f)
                    {
                        // Character is on the other side of the door...  Might be okay if the
                        // door is open (Knock Out)...  But character can bump an open door if
                        // far to the back (leaving room) or if too high up...
                        clearance = ((check_xyz[Z]+check_data[175])-(character_xyz[Z]+character_data[175])); // Distance between head and top of door passage (in feet)
                        if(check_data[66] != ACTION_KNOCK_OUT || (clearance < 0.0f) || distance > 6.0f)
                        {
                            // We have a collision...
                            // Run the bump event for the door...
                            global_bump_char = character;
                            check_data[67] = EVENT_HIT_CHARACTER;
                            fast_run_script(main_character_script_start[check], FAST_FUNCTION_EVENT, check_data);
                            if(global_room_changed)
                            {
                                // We changed rooms...  Skip collisions for right now...
                                j = MAX_CHARACTER;
                                check = MAX_CHARACTER;
                            }

                            // Run the bump event for the character...  Pretend the door is a wall...
                            if(check_data[66] != ACTION_KNOCK_OUT)
                            {
                                // Character only gets bump wall event if door is closed...
                                global_bump_char = check;
                                character_data[67] = EVENT_HIT_WALL;
                                fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                            }

                            if(check_data[66] != ACTION_KNOCK_OUT || (clearance < 0.0f))
                            {
                                // Bump head if well behind the door...
                                if(distance > 1.0f && clearance < 0.0f)
                                {
                                    character_xyz[Z] += clearance;
                                    // But don't push 'em through floor...
                                    if(character_xyz[Z] < *((float*) (character_data + 44)))
                                    {
                                        character_xyz[Z] = *((float*) (character_data + 44));
                                    }
                                }

                                // Stop character from moving towards door...  Don't restrict motion away from door...
                                disz = 0.000001f - ((character_vel_xyz[X] * check_front_xyz[X]) + (character_vel_xyz[Y] * check_front_xyz[Y]));
                                if(disz > 0.0f)
                                {
                                    character_vel_xyz[X] += check_front_xyz[X] * disz;
                                    character_vel_xyz[Y] += check_front_xyz[Y] * disz;
                                }


                                // Pushable characters are thrown back super hard so they don't get pushed through door...
                                if(character_data[104] & MORE_FLAG_PUSHABLE)
                                {
                                    character_xyz[X] = character_xyz[X] + check_front_xyz[X]*distance;
                                    character_xyz[Y] = character_xyz[Y] + check_front_xyz[Y]*distance;
                                }
                                else
                                {
                                    distance = (CHAR_FLAGS & CHAR_HOVER_ON) ? distance*0.002f : distance*0.20f;
                                    character_vel_xyz[X] += check_front_xyz[X]*distance;
                                    character_vel_xyz[Y] += check_front_xyz[Y]*distance;
                                }
                            }
                        }
                    }
                }
            }
        }
    }










    // Now check each character in the list against every other one...
    if(global_room_changed == FALSE)
    {
        repeat(i, num_character_collision_list)
        {
            check = character_collision_list[i];
            check_data = main_character_data[check];
            check_xyz = (float*) check_data;
            j = i+1;

            // Normal collision checks...
            while(j < num_character_collision_list)
            {
                character = character_collision_list[j];
                character_data = main_character_data[character];
                character_xyz = (float*) character_data;
                collide_distance = *((float*) (character_data+160)) + *((float*) (check_data+160));
                check_vel_xyz = (float*) (check_data+24);
                character_vel_xyz = (float*) (character_data+24);

                // Do a circular collision check...
                disx = character_xyz[X] - check_xyz[X];
                disy = character_xyz[Y] - check_xyz[Y];
                distance = (disx*disx) + (disy*disy);
                collide_distance*=collide_distance;
                if(distance < collide_distance)
                {
                    collide_distance = (character_xyz[Z] < check_xyz[Z]) ? ((float) character_data[175] - check_vel_xyz[Z]) : ((float) check_data[175] - character_vel_xyz[Z]);
                    disz = character_xyz[Z] - check_xyz[Z];
                    ABS(disz);
                    if(disz < collide_distance)
                    {
                        // We have a collision...
                        // Run the bump event...
                        global_bump_abort = FALSE;
                        global_bump_char = check;
                        character_data[67] = EVENT_HIT_CHARACTER;
                        fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);

                        // Run the bump event for the other character...
                        global_bump_char = character;
                        check_data[67] = EVENT_HIT_CHARACTER;
                        fast_run_script(main_character_script_start[check], FAST_FUNCTION_EVENT, check_data);


                        // Does the script say to keep goin'?
                        if(!global_bump_abort)
                        {
                            // Disallow XY movement if overlapping, but not if one char is standin' on another...
                            collide_distance = (collide_distance > 6.0f) ? (collide_distance-3.0f) : (collide_distance*0.5f);
                            if(disz < collide_distance)
                            {
                                // One character is well below the other...  Do bump collisions...
                                disz = disx;
                                disx = disy;
                                disy = -disz;
                                distance = (float) sqrt(distance);
                                distance += 0.01f;
                                disx /= distance;
                                disy /= distance;
                                distance = -disy*check_vel_xyz[X] + disx*check_vel_xyz[Y];
                                if(distance > 0.0f)
                                {
                                    distance = disx*check_vel_xyz[X] + disy*check_vel_xyz[Y];
                                    check_vel_xyz[X] = distance*disx;
                                    check_vel_xyz[Y] = distance*disy;
                                }

                                distance = -disy*character_vel_xyz[X] + disx*character_vel_xyz[Y];
                                if(distance < 0.0f)
                                {
                                    distance = disx*character_vel_xyz[X] + disy*character_vel_xyz[Y];
                                    character_vel_xyz[X] = distance*disx;
                                    character_vel_xyz[Y] = distance*disy;
                                }
                            }
                            else
                            {
                                // One char is standing on the other...  Check for platforms and mounting...
                                if(check_xyz[Z] < character_xyz[Z])
                                {
                                    // Check for mounting...
                                    if(character_vel_xyz[Z] < 0.0f)
                                    {
                                        if((CHECK_FLAGS & CHAR_CAN_BE_MOUNTED) && (*(unsigned short*) (check_data+106)) >= MAX_CHARACTER)
                                        {
                                            if((CHAR_FLAGS & CHAR_CAN_RIDE_MOUNT) && !(CHAR_FLAGS & CHAR_HOVER_ON) && (*(unsigned short*) (character_data+164)) >= MAX_CHARACTER)
                                            {
                                                // Looks like character is trying to mount check...
                                                if((check_data[78] == character_data[78] || check_data[78] == TEAM_NEUTRAL) && *((unsigned short*) (check_data+42)) == 0 && character_data[82] > 0 && check_data[82] > 0)
                                                {
                                                    // Teams match...  Attach 'em and run their callbacks...
                                                    mount_rdy_file = *((unsigned char**) (check_data+256));
                                                    if(mount_rdy_file && (check_data[65] < ACTION_KNOCK_OUT_BEGIN || check_data[65] > ACTION_KNOCK_OUT_END))
                                                    {
                                                        render_fill_temp_character_bone_number(mount_rdy_file);
                                                        if(temp_character_bone_number[SADDLE_BONE] < 255)
                                                        {
                                                            // Saddle bone is valid...
                                                            (*(unsigned short*) (check_data+106)) = character;
                                                            (*(unsigned short*) (character_data+164)) = check;
                                                            character_data[64] = temp_character_bone_number[SADDLE_BONE];
                                                            character_data[65] = ACTION_RIDE;
                                                            character_data[66] = ACTION_RIDE;

                                                            // Give each character a script event...
                                                            character_data[67] = EVENT_MOUNTED;
                                                            fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                                                            check_data[67] = EVENT_MOUNTED;
                                                            fast_run_script(main_character_script_start[check], FAST_FUNCTION_EVENT, check_data);

                                                            // Unpress buttons...
                                                            character_data[198] = 0;
                                                            check_data[198] = 0;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if((CHECK_FLAGS & CHAR_PLATFORM_ON) && !(character_data[104] & MORE_FLAG_PLATFALL))
                                    {
                                        // Mount takes precedence over platform...
                                        if(!(CHECK_FLAGS & CHAR_CAN_BE_MOUNTED) || *((unsigned short*) (check_data+106)) < MAX_CHARACTER)
                                        {
                                            // Smaller collision test for platforms stacked on platforms...
                                            collide_distance = (*((float*) (character_data+160)) + *((float*) (check_data+160)) ) - 0.5f;
                                            collide_distance *= collide_distance;
                                            if(distance < collide_distance || !(CHAR_FLAGS & CHAR_PLATFORM_ON))
                                            {
                                                // Make the character stand on the platform
                                                character_xyz[Z] = check_xyz[Z] + check_data[175] - 0.1f;
                                                if(character_vel_xyz[Z] < 0.01f)
                                                {
                                                    CHAR_FLAGS |= CHAR_ON_PLATFORM;
                                                    character_vel_xyz[Z] = -GRAVITY;
                                                    character_vel_xyz[X] += check_vel_xyz[X];
                                                    character_vel_xyz[Y] += check_vel_xyz[Y];


                                                    // Press event...
                                                    global_bump_char = character;
                                                    check_data[67] = EVENT_JUMPED_ON;
                                                    fast_run_script(main_character_script_start[check], FAST_FUNCTION_EVENT, check_data);
                                                }
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    // Check for mounting...
                                    if(check_vel_xyz[Z] < 0.0f)
                                    {
                                        if((CHAR_FLAGS & CHAR_CAN_BE_MOUNTED) && (*(unsigned short*) (character_data+106)) >= MAX_CHARACTER)
                                        {
                                            if((CHECK_FLAGS & CHAR_CAN_RIDE_MOUNT) && !(CHECK_FLAGS & CHAR_HOVER_ON) && (*(unsigned short*) (check_data+164)) >= MAX_CHARACTER)
                                            {
                                                // Looks like check is trying to mount character...
                                                if((check_data[78] == character_data[78] || character_data[78] == TEAM_NEUTRAL) && *((unsigned short*) (character_data+42)) == 0 && character_data[82] > 0 && check_data[82] > 0)
                                                {
                                                    // Teams match...  Attach 'em and run their callbacks...
                                                    mount_rdy_file = *((unsigned char**) (character_data+256));
                                                    if(mount_rdy_file && (character_data[65] < ACTION_KNOCK_OUT_BEGIN || character_data[65] > ACTION_KNOCK_OUT_END))
                                                    {
                                                        render_fill_temp_character_bone_number(mount_rdy_file);
                                                        if(temp_character_bone_number[SADDLE_BONE] < 255)
                                                        {
                                                            // Saddle bone is valid...
                                                            (*(unsigned short*) (character_data+106)) = check;
                                                            (*(unsigned short*) (check_data+164)) = character;
                                                            check_data[64] = temp_character_bone_number[SADDLE_BONE];
                                                            check_data[65] = ACTION_RIDE;
                                                            check_data[66] = ACTION_RIDE;

                                                            // Give each character a script event...
                                                            character_data[67] = EVENT_MOUNTED;
                                                            fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                                                            check_data[67] = EVENT_MOUNTED;
                                                            fast_run_script(main_character_script_start[check], FAST_FUNCTION_EVENT, check_data);

                                                            // Unpress buttons...
                                                            character_data[198] = 0;
                                                            check_data[198] = 0;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    if((CHAR_FLAGS & CHAR_PLATFORM_ON) && !(check_data[104] & MORE_FLAG_PLATFALL))
                                    {
                                        // Mount takes precedence over platform...
                                        if(!(CHAR_FLAGS & CHAR_CAN_BE_MOUNTED) || *((unsigned short*) (character_data+106)) < MAX_CHARACTER)
                                        {
                                            // Smaller collision test for platforms stacked on platforms...
                                            collide_distance = (*((float*) (character_data+160)) + *((float*) (check_data+160)) ) * 0.5f;
                                            collide_distance *= collide_distance;
                                            if(distance < collide_distance || !(CHECK_FLAGS & CHAR_PLATFORM_ON))
                                            {
                                                // Make the character stand on the platform
                                                check_xyz[Z] = character_xyz[Z] + character_data[175] - 0.1f;
                                                if(check_vel_xyz[Z] < 0.01f)
                                                {
                                                    CHECK_FLAGS |= CHAR_ON_PLATFORM;
                                                    check_vel_xyz[Z] = -GRAVITY;
                                                    check_vel_xyz[X] += character_vel_xyz[X];
                                                    check_vel_xyz[Y] += character_vel_xyz[Y];


                                                    // Press event...
                                                    global_bump_char = check;
                                                    character_data[67] = EVENT_JUMPED_ON;
                                                    fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                j++;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define MAX_SHADOW_DISTANCE 20.0f
void character_shadow_draw_all()
{
    // <ZZ> This function draws all of the character's shadows
    unsigned short i;
    unsigned char* character_data;
    unsigned char* model_data;
    float z, distance, scale;
    unsigned char alpha;


    if(fast_and_ugly_active)
    {
        // No shadows in fast and ugly mode...
        return;
    }


    // !!!BAD!!!
    // !!!BAD!!!  Only do what needs to be done...
    // !!!BAD!!!
    display_color(white);
    display_shade_off();
    display_texture_on();
    display_blend_trans();
    display_zbuffer_on();
    display_zbuffer_write_off();
    display_cull_off();
    // !!!BAD!!!
    // !!!BAD!!!
    // !!!BAD!!!



    // Render shadows...
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            character_data = main_character_data[i];
            model_data = character_data+256;
            z = *((float*) (character_data + 44));
            if(z < room_water_level)
            {
                // Character is over/under water...  Shadow should be on surface
                z = room_water_level;
                distance = (*((float*) (character_data + 8))) - z;
                distance = (distance < 0.0f) ? (distance*-5.0f) : distance;
            }
            else
            {
                // Normal shadow...
                distance = (*((float*) (character_data + 8))) - z;
            }
            if(distance < MAX_SHADOW_DISTANCE)
            {
                if(distance < 0.0f)
                {
                    alpha = 255;
                    scale = 1.0f;
                }
                else
                {
                    scale = (MAX_SHADOW_DISTANCE-distance)/MAX_SHADOW_DISTANCE;
                    alpha = (unsigned char) (scale*255.0f);
                }
                render_rdy_character_shadow(*((unsigned char**) model_data), character_data, alpha, scale, z);
            }
        }
    }


    display_zbuffer_write_on();
    display_cull_on();
}

//-----------------------------------------------------------------------------------------------
//#define SHOW_INVINCIBILITY   // Uncomment to show invincibility timer...
unsigned char character_draw_on_pass[MAX_CHARACTER];
void character_draw_all_prime()
{
    // <ZZ> This function sets up the array that tells us the order in which to draw our characters...
    unsigned short i;
    unsigned char* character_data;
    repeat(i, MAX_CHARACTER)
    {
        character_draw_on_pass[i] = 255;
        if(main_character_on[i])
        {
            // Draw solid chars on first pass and fuzzy characters on second and transparent characters on third...
            // Water comes next...
            // Then draw chars that are flagged as "after water", then fuzzy chars that are flagged as "after water", then transparent characters flagged as "after water"...
            character_data = main_character_data[i];
            character_draw_on_pass[i] = (CHAR_FLAGS & CHAR_FUZZY_ON) ? 1 : 0;
            character_draw_on_pass[i] = (character_data[79] < 255) ? 2 : character_draw_on_pass[i];
            character_draw_on_pass[i] = (character_data[105] & VIRTUE_FLAG_AFTER_WATER) ? (character_draw_on_pass[i]+3) : character_draw_on_pass[i];
            character_draw_on_pass[i] = (character_data[104] & MORE_FLAG_DOOR) ? 7 : character_draw_on_pass[i];
        }
    }
}
void character_draw_all(unsigned char after_water, unsigned char draw_only_doors)
{
    // <ZZ> This function draws all of the characters...  After_water should be TRUE if the
    //      function is called after drawing water (called twice now)...
    unsigned short i, j, k, num_character;
    unsigned short frame;
    unsigned char alpha;
    unsigned char* character_data;
    unsigned char* model_data;
    unsigned char pass, pass_end;
    unsigned short timer;
    unsigned char petrify;
    unsigned char eye_frame;
    unsigned char brightness;
    unsigned short num_door_list;
    unsigned short door_list[MAX_CHARACTER];
    unsigned short door_list_order[MAX_CHARACTER];
    unsigned short door_list_final[MAX_CHARACTER];
    float door_list_distance[MAX_CHARACTER];
    float x, y, z;
    #ifdef DEVTOOL
        float top_left_xyz[3];
        float bottom_right_xyz[3];
        float boxsize;
        unsigned char height;
    #endif


    // Draw goto point
    #ifdef DEVTOOL
        if(debug_active && after_water)
        {
            display_color(red);
            display_texture_off();
            display_blend_off();
            repeat(i, MAX_CHARACTER)
            {
                if(main_character_on[i])
                {
                    character_data = main_character_data[i];
                    display_start_line();
                        display_vertex_xyz(*((float*) (character_data+12)), *((float*) (character_data+16)), 0.0f);
                        display_vertex_xyz(*((float*) (character_data+12)), *((float*) (character_data+16)), 1000.0f);
                    display_end();
                }
            }
            display_texture_on();
        }
    #endif




    if(draw_only_doors)
    {
        num_door_list = 0;
        repeat(i, MAX_CHARACTER)
        {
            if(character_draw_on_pass[i] == 7)
            {
                // It's a door...
                character_data = main_character_data[i];
                x = camera_xyz[X] - ((float*) character_data)[X];
                y = camera_xyz[Y] - ((float*) character_data)[Y];
                z = camera_xyz[Z] - ((float*) character_data)[Z];
                door_list_distance[num_door_list] = (camera_fore_xyz[X] * x) + (camera_fore_xyz[Y] * y) + (camera_fore_xyz[Z] * z);
                door_list[num_door_list] = i;
                door_list_order[num_door_list] = 0;
                repeat(j, num_door_list)
                {
                    if(door_list_distance[j] < door_list_distance[num_door_list])
                    {
                        // Current door character should be drawn after the one we're checking...
                        door_list_order[num_door_list]++;
                    }
                    else
                    {
                        // Other way around...
                        door_list_order[j]++;
                    }
                }
                num_door_list++;
            }
        }

        // Now figger the final door list...
        repeat(i, num_door_list)
        {
            repeat(j, num_door_list)
            {
                if(door_list_order[j] == i)
                {
                    door_list_final[i] = door_list[j];
                }
            }
        }
    }



    #ifdef DEVTOOL
    if(!key_down[SDLK_F12])
    {
    #endif
        // Solid, Fuzzy, Transparent
        num_character = MAX_CHARACTER;
        pass = 0;
        pass_end = 3;
        if(after_water)
        {
            // Solid after water, Fuzzy after water, Transparent after water
            pass = 3;
            pass_end = 6;
        }
        if(draw_only_doors)
        {
            num_character = num_door_list;
            pass = 7;
            pass_end = 8;
        }
        while(pass < pass_end)
        {
            repeat(k, num_character)
            {
                i = k;
                if(draw_only_doors)
                {
                    i = door_list_final[k];
                }
                if(character_draw_on_pass[i] == pass)
                {
                    character_data = main_character_data[i];
                    alpha = character_data[79];
                    model_data = character_data+256;
                    eye_frame = (unsigned char) (*((unsigned short*) (character_data + 182)));
                    frame = *((unsigned short*) (character_data + 178));


                    // Draw the base model...  (May just be bones)
                    if(*((unsigned char**) model_data) != NULL)
                    {
                        onscreen_joint_active = TRUE;  // Figure out if all joints are onscreen before drawing...
                        onscreen_joint_character = i;
                        onscreen_joint_character_data = character_data;

                        // Setup the lighting information...
                        global_render_light_color_rgb[0] = 255;
                        global_render_light_color_rgb[1] = 255;
                        global_render_light_color_rgb[2] = 255;
                        global_render_light_offset_xy[X] = 0.75f;  // 0.25
                        global_render_light_offset_xy[Y] = 0.60f;  // 0.38


                        // Modify lighting for poison...
                        timer = (*((unsigned short*) (character_data+40)));
                        if(timer > 0)
                        {
                            if(timer > 63)
                            {
                                global_render_light_color_rgb[0] = 0;
                                global_render_light_color_rgb[2] = 0;
                            }
                            else
                            {
                                timer = 255 - (timer<<2);
                                global_render_light_color_rgb[0] = (unsigned char) timer;
                                global_render_light_color_rgb[2] = (unsigned char) timer;
                            }
                        }



                        // Modify lighting for damage...
                        timer = (*((unsigned short*) (character_data+168)));
                        if(timer > 0)
                        {
                            global_render_light_color_rgb[0] = 255;
                            if(timer > 15)
                            {
                                global_render_light_color_rgb[1] = 0;
                                global_render_light_color_rgb[2] = 0;
                            }
                            else
                            {
                                timer = 255 - (timer<<4);
                                global_render_light_color_rgb[1] = (global_render_light_color_rgb[1] * ((unsigned char) timer)) >> 8;
                                global_render_light_color_rgb[2] = (global_render_light_color_rgb[2] * ((unsigned char) timer)) >> 8;
                            }
                        }




// Modify lighting for invincibility
#ifdef DEVTOOL
  #ifdef SHOW_INVINCIBILITY
    timer = (*((unsigned short*) (character_data+170)));
    if(timer > 0)
    {
        global_render_light_color_rgb[0] = 0;
        global_render_light_color_rgb[1] = 255;
        global_render_light_color_rgb[2] = 255;
    }
  #endif
#endif


                        // Modify lighting for character brightness (script value and falling in pits)...
                        brightness = character_data[206];
                        if(((float*) character_data)[Z] < ROOM_PIT_HIGH_LEVEL)
                        {
                            if(((float*) character_data)[Z] < ROOM_PIT_LOW_LEVEL)
                            {
                                brightness = 0;
                            }
                            else
                            {
                                brightness -= (unsigned char) ((0.0625f * (ROOM_PIT_HIGH_LEVEL - ((float*) character_data)[Z])) * brightness);
                            }
                        }
                        global_render_light_color_rgb[0] = (global_render_light_color_rgb[0] * brightness) >> 8;
                        global_render_light_color_rgb[1] = (global_render_light_color_rgb[1] * brightness) >> 8;
                        global_render_light_color_rgb[2] = (global_render_light_color_rgb[2] * brightness) >> 8;




                        // Petrify stuff...
                        petrify = ((*((unsigned short*) (character_data+42))) > 0);




                        render_rdy(*((unsigned char**) model_data), frame, WIN_3D_MODEL, (unsigned char**) (model_data+4), alpha, temp_character_bone_frame[i], petrify, eye_frame);
                        if(onscreen_joint_active)
                        {
                            // Character base model (bones) was onscreen...  Go ahead and actually draw the
                            // character's parts now...
                            onscreen_joint_active = FALSE;



                            // Draw all of the non-animated overlapped parts...  Arms, Head, etc...
                            model_data+=24;
                            frame = 0;
                            repeat(j, 7)
                            {
                                if(*((unsigned char**) model_data) != NULL)
                                {
                                    render_rdy(*((unsigned char**) model_data), frame, WIN_3D_MODEL, (unsigned char**) (model_data+4), alpha, temp_character_bone_frame[i], petrify, eye_frame);
                                }
                                model_data+=24;
                            }


                            // Draw the animated mouth...
                            frame = *((unsigned short*) (character_data + 184));
                            model_data+=24;
                            if(*((unsigned char**) model_data) != NULL)
                            {
                                render_rdy(*((unsigned char**) model_data), frame, WIN_3D_MODEL, (unsigned char**) (model_data+4), alpha, temp_character_bone_frame[i], petrify, eye_frame);
                            }
                            model_data-=24;



                            // Draw the animated eyes...
                            frame = *((unsigned short*) (character_data + 182));
                            if(*((unsigned char**) model_data) != NULL)
                            {
                                render_rdy(*((unsigned char**) model_data), frame, WIN_3D_MODEL, (unsigned char**) (model_data+4), alpha, temp_character_bone_frame[i], petrify, eye_frame);
                            }
                            model_data+=48;



                            // Draw the left and right and left2 and right2 weapons...  Only drawn if setup in character's model
                            // data, and if character's model has the required bone...
                            model_data+=24;
                            repeat(j, 4)
                            {
                                if((*((unsigned char**) model_data)) != NULL && temp_character_bone_number[j+1] < 255)
                                {
                                    // Hand held weapons have 3 frames...  (should be 3 independent base models...)
                                    //    Frame 0 is when holstered.
                                    //    Frame 1 is when taken out (reversal for shields) or attacking (swipe for weapons)
                                    //    Frame 2 is when attacking (swipe thing) (shields don't need this...)
                                    if(*((unsigned short*) (character_data+42)) > 0)
                                    {
                                        // Petrified characters don't have swipe thing...  But do have weapon on back thing...
                                        temp_character_frame_event&=128;
                                    }
                                    switch(j&1)
                                    {
                                        case 0:
                                            frame = (temp_character_frame_event & FRAME_EVENT_LEFT)?2:(temp_character_frame_event >> 7);
                                            break;
                                        case 1:
                                            frame = (temp_character_frame_event & FRAME_EVENT_RIGHT)?2:(temp_character_frame_event >> 7);
                                            break;
                                    }


                                    script_matrix_good_bone(temp_character_bone_number[j+1], temp_character_bone_frame[i], character_data);
                                    render_generate_model_world_data(*((unsigned char**) model_data), frame, script_matrix, fourthbuffer);  // Generate new bone frame in fourthbuffer
                                    render_rdy(*((unsigned char**) model_data), frame, WIN_3D_MODEL, (unsigned char**) (model_data+4), alpha, fourthbuffer, petrify, 0);
                                }
                                model_data+=24;
                            }



                            // Reset line color (may've changed if highlighted...)
                            line_color[0] = 0;
                            line_color[1] = 0;
                            line_color[2] = 0;
                        }
                    }
                }
            }
            pass++;
        }
        onscreen_joint_active = FALSE;
    #ifdef DEVTOOL
    }
    else
    {
        if(after_water)
        {
            repeat(i, MAX_CHARACTER)
            {
                if(main_character_on[i])
                {
                    character_data = main_character_data[i];
                    if(!(CHAR_FLAGS & CHAR_NO_COLLISION))
                    {
                        top_left_xyz[X] = *((float*) (character_data));
                        top_left_xyz[Y] = *((float*) (character_data+4));
                        top_left_xyz[Z] = *((float*) (character_data+8));
                        bottom_right_xyz[X] = *((float*) (character_data));
                        bottom_right_xyz[Y] = *((float*) (character_data+4));
                        bottom_right_xyz[Z] = *((float*) (character_data+8));
                        boxsize = *((float*) (character_data+160));
                        height = *(character_data+175);
                        top_left_xyz[X]-=boxsize;
                        top_left_xyz[Y]-=boxsize;
                        top_left_xyz[Z]+=height;
                        bottom_right_xyz[X]+=boxsize;
                        bottom_right_xyz[Y]+=boxsize;
                        render_solid_box(red, top_left_xyz[X], top_left_xyz[Y], top_left_xyz[Z], bottom_right_xyz[X], bottom_right_xyz[Y], bottom_right_xyz[Z]);
                    }
                }
            }
        }
    }
    #endif


    // Reset the depth buffer stuff (funky thing for making layer'd textures not blip...)
    display_depth_scene();
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This is helper stuff for doing character-wall collision detection, and modifying velocity
//      to slide off wall...
#define NUMBER_OF_COLLISION_ITERATIONS 8
float global_collision_vx;
float global_collision_vy;
float global_collision_z;
unsigned char global_collision_fail_count;
unsigned char global_collision_hit_wall;
void char_collision_point(float* position_xyz, float* velocity_xyz, float x_offset, float y_offset)
{
    float x, y, z;
    x = position_xyz[X]+x_offset+velocity_xyz[X];
    y = position_xyz[Y]+y_offset+velocity_xyz[Y];
    z = room_heightmap_height(roombuffer, x, y);
    if(z > global_collision_z)
    {
        // Remember our highest floor level...
        global_collision_z = z;
    }
    if(position_xyz[Z] < (z-CLIMB_HEIGHT))
    {
        // We hit a wall...
        global_collision_fail_count++;
        global_collision_hit_wall=TRUE;
        global_collision_vx-=x_offset;
        global_collision_vy-=y_offset;
    }
}
void char_floor_point(float* position_xyz)
{
    float x, y, z;
    x = position_xyz[X];
    y = position_xyz[Y];
    z = room_heightmap_height(roombuffer, x, y);
    if(z > global_collision_z)
    {
        // Remember our highest floor level...
        global_collision_z = z;
    }
}

//-----------------------------------------------------------------------------------------------
void character_update_all()
{
    // <ZZ> This function moves the characters around and stuff.
    unsigned short i, j, k;
    unsigned char* character_data;
    unsigned char on_ground, turning;
    float cosine, sine;
    float* velocity_xyz;
    float* position_xyz;
    float velx, boxsize;
    float vely;
    float x, y;
    unsigned short rider;
    unsigned char can_swim;
    unsigned char* data;
    unsigned char* next_frame_data;
    unsigned short num_base_model;
    unsigned short num_bone_frame;
    unsigned short frame;
    unsigned char action;
    unsigned char next_action;
    unsigned short* action_first_frame;
    unsigned short current_facing, desired_facing, spin_rate;
    signed short spin_difference;
    float height;
    unsigned char skip;
    unsigned char frame_event_flags;
    unsigned char in_water;
    unsigned char* mount_data;
    unsigned char* child_data;
    unsigned short mount;
    unsigned char eye_model_valid;
    unsigned char dexterity;


    // Update all of the characters...
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            // Make it more readable...
            character_data = main_character_data[i];
            velocity_xyz = (float*) (character_data+24);
            position_xyz = (float*) character_data;
            height = (float) character_data[175];
            dexterity = character_data[87];
            if(character_data[216] & ENCHANT_FLAG_HASTE)
            {
                // Character is hasten'd...  Counts as +25 dexterity...
                dexterity += 25;
            }



            // Model eye frame stuff...
            if(character_data[186] != 0)
            {
                // Do we have a valid eye model?  Do simple eye quad style animation (standard type)
                // if the eye model only has 1 frame, or if there isn't a model...
                frame = *((unsigned short*) (character_data + 182));
                data = (*((unsigned char**) (character_data+448)));
                eye_model_valid = FALSE;
                if(data)
                {
                    // Have an eye model...  Start reading the header...
                    data+=3;
                    num_base_model = *data;  data++;
                    num_bone_frame = *((unsigned short*) data); data+=2;
                    if(num_bone_frame > 1)
                    {
                        // We have enough frames to do model style animation...
                        eye_model_valid = TRUE;
                    }
                }
                if(eye_model_valid)
                {
                    // Continue reading the eye model header...
                    action_first_frame = (unsigned short*) data;
                    data+=(ACTION_MAX<<1);
                    data+=(MAX_DDD_SHADOW_TEXTURE);
                    data+=(num_base_model*20*DETAIL_LEVEL_MAX);
                    if(frame < (num_bone_frame-1))
                    {
                        next_frame_data = *((unsigned char**) (data+((frame+1)<<2)));
                        data =  *((unsigned char**) (data+(frame<<2)));
                        action = *data;
                        next_action = *next_frame_data;
                        // Eye action animation
                        if(action != character_data[186])
                        {
                            // Start a new action...
                            *((unsigned short*) (character_data+182)) = action_first_frame[character_data[186]];
                            if((*((unsigned short*) (character_data+182))) == 65535)
                            {
                                // Default to boning action if action not found...
                                character_data[186] = 0;
                                *((unsigned short*) (character_data+182)) = action_first_frame[0];
                            }
                        }
                        else
                        {
                            // Increment the frame...
                            (*((unsigned short*) (character_data+182)))++;
                            if(next_action != action)
                            {
                                // Return to the default action...
                                character_data[186] = 0;
                                *((unsigned short*) (character_data+182)) = action_first_frame[0];
                            }
                        }
                    }
                    else
                    {
                        // Return to the default action...
                        character_data[186] = 0;
                        *((unsigned short*) (character_data+182)) = action_first_frame[0];
                    }
                }
                else
                {
                    // No eye model, or not enough frames to be valid...
                    // Do special cased frame increment...  32 frames...
                    // Model probably has an eye texture that has a magic little flag on it
                    // to change tex vertex positions based on eye frame...
                    if(character_data[65] < ACTION_KNOCK_OUT_BEGIN || character_data[65] > ACTION_KNOCK_OUT_STUN)
                    {
                        // Character is awake...
                        if(frame < 31)
                        {
                            // Blinking...
                            (*((unsigned short*) (character_data+182)))++;
                        }
                        else
                        {
                            // Return to the default action...
                            character_data[186] = 0;
                            *((unsigned short*) (character_data+182)) = 0;
                        }
                    }
                    else
                    {
                        // Character is knock'd out...  Close eyes...
                        (*((unsigned short*) (character_data+182))) = 7;
                    }
                }
            }



            // Model mouth frame stuff...
            if(character_data[187] != 0)
            {
                frame = *((unsigned short*) (character_data + 184));
                data = (*((unsigned char**) (character_data+472)));
                if(data)
                {
                    data+=3;
                    num_base_model = *data;  data++;
                    num_bone_frame = *((unsigned short*) data); data+=2;
                    action_first_frame = (unsigned short*) data;
                    data+=(ACTION_MAX<<1);
                    data+=(MAX_DDD_SHADOW_TEXTURE);
                    data+=(num_base_model*20*DETAIL_LEVEL_MAX);
                    if(frame < (num_bone_frame-1))
                    {
                        next_frame_data = *((unsigned char**) (data+((frame+1)<<2)));
                        data =  *((unsigned char**) (data+(frame<<2)));
                        action = *data;
                        next_action = *next_frame_data;
                        // Mouth action animation
                        if(action != character_data[187])
                        {
                            // Start a new action...
                            *((unsigned short*) (character_data+184)) = action_first_frame[character_data[187]];
                            if((*((unsigned short*) (character_data+184))) == 65535)
                            {
                                // Default to boning action if action not found...
                                character_data[187] = 0;
                                *((unsigned short*) (character_data+184)) = action_first_frame[0];
                            }
                        }
                        else
                        {
                            // Increment the frame...
                            (*((unsigned short*) (character_data+184)))++;
                            if(next_action != action)
                            {
                                // Return to the default action...
                                character_data[187] = 0;
                                *((unsigned short*) (character_data+184)) = action_first_frame[0];
                            }
                        }
                    }
                    else
                    {
                        // Return to the default action...
                        character_data[187] = 0;
                        *((unsigned short*) (character_data+184)) = action_first_frame[0];
                    }
                }
            }



            // Model frame stuff...
            frame_event_flags = 0;
            skip = 1;
            if(character_data[65] != ACTION_BONING && *((unsigned short*) (character_data+42)) == 0)
            {
                // Character is unpetrified and doing a normal action...
                if(dexterity > 19)
                {
                    // Characters with high dexterity skip frames in animation to make it appear faster...
                    if(dexterity > 39)
                    {
                        if(dexterity > 60)
                        {
                            // 2.0x normal rate...
                            skip++;
                        }
                        else
                        {
                            // 1.5x normal rate...
                            skip += (main_game_frame&1);
                        }
                    }
                    else
                    {
                        // 1.25x normal rate...
                        skip += ((main_game_frame&3)==3);
                    }
                }
            }
            while(skip > 0)
            {
                frame = *((unsigned short*) (character_data + 178));
                data = (*((unsigned char**) (character_data+256)))+3;
                num_base_model = *data;  data++;
                num_bone_frame = *((unsigned short*) data); data+=2;
                action_first_frame = (unsigned short*) data;
                data+=(ACTION_MAX<<1);
                data+=(MAX_DDD_SHADOW_TEXTURE);
                data+=(num_base_model*20*DETAIL_LEVEL_MAX);
                if(frame < (num_bone_frame-1))
                {
                    next_frame_data = *((unsigned char**) (data+((frame+1)<<2)));
                    data =  *((unsigned char**) (data+(frame<<2)));
                }
                else
                {
                    next_frame_data = *((unsigned char**) (data));
                    if(frame < num_bone_frame)
                    {
                        data = *((unsigned char**) (data+(frame<<2)));
                    }
                    else
                    {
                        data = next_frame_data;
                    }
                }
                action = *data;
                next_action = *next_frame_data;

                // Get the frame events flag...
                frame_event_flags |= *(data+1);

                skip--;
                if(skip > 0)
                {
                    if(frame < (num_bone_frame-1))
                    {
                        if(next_action == action)
                        {
                            (*((unsigned short*) (character_data+178)))++;
                        }
                    }
                }
            }
            if(*((unsigned short*) (character_data+42)) > 0)
            {
                // Petrified characters don't get frame events...
                frame_event_flags = 0;
            }
 



            // Make sure self.rider is either valid or MAX_CHARACTER...
            rider = *((unsigned short*) (character_data+106));
            if(rider < MAX_CHARACTER)
            {
                if(main_character_on[rider] == FALSE || (*((unsigned short*) (main_character_data[rider]+164))) != i)
                {
                    // Character we think is our rider doesn't recognize us as its mount...  Or rider has been poof'd...
                    (*(unsigned short*) (character_data+106)) = MAX_CHARACTER;
                    character_data[67] = EVENT_DISMOUNTED;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }


            // Check if the character is mounted...
            mount_data = NULL;
            mount = *((unsigned short*) (character_data+164));
            if(mount < MAX_CHARACTER)
            {
                if(main_character_on[mount])
                {
                    mount_data = main_character_data[mount];
                    if(!(MOUNT_FLAGS & CHAR_CAN_BE_MOUNTED) || *((unsigned short*) (mount_data+42)) > 0)
                    {
                        // Uh oh...  Invalid mount (or mount is petrified)...  Knock 'em off...
                        *((unsigned short*) (character_data+164)) = 65535;
                        character_data[65] = ACTION_STAND;
                        character_data[66] = ACTION_STAND;
                        mount_data = NULL;

                        // Give the rider a dismount event...
                        character_data[67] = EVENT_DISMOUNTED;
                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                    }
                }
                else
                {
                    // Invalid mount...  Mount poof'd or somethin'...
                    *((unsigned short*) (character_data+164)) = 65535;
                    character_data[65] = ACTION_STAND;
                    character_data[66] = ACTION_STAND;

                    // Give the rider a dismount event...
                    character_data[67] = EVENT_DISMOUNTED;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }



            // Only do motion updates on characters who're not mounted...
            if(mount_data)
            {
                // Copy the character's controls over to its mount...
                if(mount != local_player_character[0] && mount != local_player_character[1] && mount != local_player_character[2] && mount != local_player_character[3])
                {
                    // But only if the mount is not a player...
                    if(*((unsigned short*) (character_data+42)) == 0)
                    {
                        // Non-petrified characters only...
                        mount_data[36] = (character_data[36]>mount_data[36])?character_data[36]:mount_data[36];  character_data[36] = (unsigned char) 0;
                        mount_data[37] = (character_data[37]>mount_data[37])?character_data[37]:mount_data[37];  character_data[37] = (unsigned char) 0;
                        mount_data[38] = (character_data[38]>mount_data[38])?character_data[38]:mount_data[38];  character_data[38] = (unsigned char) 0;
                        mount_data[39] = (character_data[39]>mount_data[39])?character_data[39]:mount_data[39];  character_data[39] = (unsigned char) 0;
                        mount_data[198] |= character_data[198];  character_data[198] = (unsigned char) 0;
                        mount_data[199] = character_data[199];  character_data[199] = (unsigned char) 0;


                        // Goto xy also...
                        (*((float*) (mount_data+12))) = ((*((float*) (character_data+12))) - (*((float*) (character_data+0))) + (*((float*) (mount_data+0))) + (*((float*) (mount_data+12)))) * 0.5f;
                        (*((float*) (mount_data+16))) = ((*((float*) (character_data+16))) - (*((float*) (character_data+4))) + (*((float*) (mount_data+4))) + (*((float*) (mount_data+16)))) * 0.5f;
                    }
                    else
                    {
                        // Petrified characters don't make very good drivers...
                        character_data[36] = (unsigned char) 0;
                        character_data[37] = (unsigned char) 0;
                        character_data[38] = (unsigned char) 0;
                        character_data[39] = (unsigned char) 0;
                        character_data[198] = (unsigned char) 0;
                        character_data[199] = (unsigned char) 0;
                        mount_data[36] = character_data[36];
                        mount_data[37] = character_data[37];
                        mount_data[38] = character_data[38];
                        mount_data[39] = character_data[39];
                        mount_data[198] = character_data[198];
                        mount_data[199] = character_data[199];
                        (*((float*) (mount_data+12))) = (*((float*) (mount_data+0)));
                        (*((float*) (mount_data+16))) = (*((float*) (mount_data+4)));
                    }
                }


                // Drown timer for rider...
                if(((position_xyz[Z]+height) < room_water_level && room_water_type != WATER_TYPE_LAVA) || (position_xyz[Z] < room_water_level && room_water_type == WATER_TYPE_LAVA && ((signed char*)character_data)[100] < 4))  // DefFire of 4 prevents lava damage...
                {
                    if((character_data[82] > 0) && (character_data[80] < 255) && (!(character_data[105]&VIRTUE_FLAG_NO_DROWN) || room_water_type == WATER_TYPE_LAVA))
                    {
                        character_data[49]++;
                        if(character_data[49] >= drown_delay[character_data[100]])  // Amount of time before damage based on DefFire for lava...
                        {
                            // Uh, oh...  We flipped the drown timer...  That means we gotta do some damage...
                            // Decrement Hits...  Increment Taps...
                            if(character_data[82] > 0)
                            {
                                character_data[82]--;
                                character_data[81]++;
                            }


                            // Spawn a damage particle...
                            if(room_water_type != WATER_TYPE_SAND)
                            {
                                child_data = obj_spawn(PARTICLE, position_xyz[X], position_xyz[Y], position_xyz[Z]+(character_data[175] - 1), pnumber_file, MAX_PARTICLE);
                                if(child_data)
                                {
                                    // Tint the number...
                                    if(room_water_type == WATER_TYPE_WATER)
                                    {
                                        child_data[48] = 255;
                                        child_data[49] = 255;
                                        child_data[50] = 255;
                                    }
                                    else
                                    {
                                        child_data[48] = 255;
                                        child_data[49] = 50;
                                        child_data[50] = 0;
                                        (*((unsigned short*) (character_data+168))) = character_data[49] + 15;
                                    }
                                    child_data[51] = 1;
                                }
                            }

                            // Reset drown timer...
                            character_data[49] = 0;


                            // Call the event function if the drowning killed the character...
                            if(character_data[82] == 0)
                            {
                                character_data[67] = EVENT_DAMAGED;
                                global_attacker = i;
                                global_attack_spin = (*((unsigned short*) (character_data + 56))) + 32768;
                                fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                            }
                            else
                            {
                                // Otherwise call the drown event, so we know to spawn some bubbles...
                                character_data[67] = EVENT_DROWN;
                                fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                            }
                        }
                    }
                }
                else
                {
                    // Reset drown timer...
                    character_data[49] = 0;
                }







                // Save our floor height for later...
                global_collision_z = -9999.0f;
                char_floor_point(position_xyz);
                *((float*) (character_data + 44)) = global_collision_z;               
            }
            else
            {
                // Do the button press/unpress/timers script calls...  Only if not petrified...
                if(*((unsigned short*) (character_data+42)) == 0)
                {
                    repeat(j, 4)
                    {
                        // Button hold timer...  Unpress'd when it hits 0...
                        if(character_data[36+j] > 0)
                        {
                            character_data[36+j]--;
                            if(character_data[36+j] == 0)
                            {
                                character_data[198] |= (16<<j);
                            }
                        }


                        // Button press scripts...
                        global_button_handled = FALSE;
                        if(character_data[198] & (1<<j))
                        {
                            character_data[67] = (unsigned char) j;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_BUTTONEVENT, character_data);
                            if(global_button_handled)
                            {
                                character_data[198] &= 255 - (1<<j);
                            }
                        }


                        // Button release scripts...
                        global_button_handled = FALSE;
                        if(character_data[198] & (16<<j))
                        {
                            // Turn off any presses to same button......
                            character_data[198] &= 255 - (1<<j);


                            character_data[67] = ((unsigned char) j)|8;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_BUTTONEVENT, character_data);
                            if(global_button_handled)
                            {
                                character_data[198] &= 255 - (16<<j);
                            }
                        }
                    }
                }




                // Rotate to face the desired x, y coordinates
                turning = FALSE;
                if(action_can_turn[character_data[65] & (ACTION_MAX-1)] && (*((unsigned short*) (character_data+42)) == 0))
                {
                    // Character is able to turn...
                    x = (*((float*) (character_data+12)));
                    y = (*((float*) (character_data+16)));
                    x-=position_xyz[X];
                    y-=position_xyz[Y];
                    if((x*x + y*y) > 1.00f)
                    {
                        // Gotoxy is at least a little bit away from our character...  Start doin' a
                        // walk action, if we're standing still...
                        if(character_data[65] == ACTION_STAND)
                        {
                            if(!(CHAR_FLAGS & CHAR_FINISH_ACTION))
                            {
                                // Some characters wait until current action is done before switchin'...
                                character_data[65] = ACTION_WALK;
                            }
                            character_data[66] = ACTION_WALK;
                        }                
                        if(character_data[65] == ACTION_SWIM)
                        {
                            character_data[65] = ACTION_SWIM_FORWARD;
                            character_data[66] = ACTION_SWIM;
                        }


                        // Gradually spin around to face the desired direction
// !!!BAD!!!
// !!!BAD!!!  Get rid of atan()...  Maybe do every 16 frames or so (save desired facing in character data)...  Maybe some lookup thing...
// !!!BAD!!!
                        desired_facing = (unsigned short) (((float) atan2(y, x)) * (65535.0f / (2.0f*PI)));
                        current_facing = *((unsigned short*) (character_data+56));
                        spin_rate = *((unsigned short*) (character_data+58));
                        spin_difference = desired_facing - current_facing;
                        if(spin_difference > 0)
                        {
                            if(spin_difference < spin_rate)
                            {
                                (*((unsigned short*) (character_data+56))) = desired_facing;
                            }
                            else
                            {
                                (*((unsigned short*) (character_data+56))) += spin_rate;
                                turning = TRUE;
                            }
                        }
                        else
                        {
                            if(-spin_difference < spin_rate)
                            {
                                (*((unsigned short*) (character_data+56))) = desired_facing;
                            }
                            else
                            {
                                (*((unsigned short*) (character_data+56))) -= spin_rate;
                                turning = TRUE;
                            }
                        }
                    }
                    else
                    {
                        // Very close to goto point...  Should stop...
                        if(character_data[65] == ACTION_WALK)
                        {
                            if(!(CHAR_FLAGS & CHAR_FINISH_ACTION))
                            {
                                // Some characters wait until current action is done before switchin'...
                                character_data[65] = ACTION_STAND;
                            }
                            character_data[66] = ACTION_STAND;

                            // Give us a hit waypoint event for the script to chew on...
                            rider = *((unsigned short*) (character_data+106));
                            if(rider >= MAX_CHARACTER)
                            {
                                // Normal character...
                                character_data[67] = EVENT_HIT_WAYPOINT;
                                fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                            }
                            else
                            {
                                // A mount has reached its waypoint...  Give its rider the hit waypoint event...
                                if(main_character_on[rider])
                                {
                                    main_character_data[rider][67] = EVENT_HIT_WAYPOINT;
                                    fast_run_script(main_character_script_start[rider], FAST_FUNCTION_EVENT, main_character_data[rider]);
                                }
                                else
                                {
                                    // Rider seems to be invalid...
                                    (*(unsigned short*) (character_data+106)) = MAX_CHARACTER;
                                    character_data[67] = EVENT_HIT_WAYPOINT;
                                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                                }
                            }
                        }
                    }
                }


                // Update motion...  Gravity...
                if(CHAR_FLAGS & CHAR_GRAVITY_ON)
                {
                    velocity_xyz[Z] += GRAVITY;
                }


                // Get the size of the bounding box...
                boxsize = *((float*) (character_data+160));
                if(CHAR_FLAGS & CHAR_LITTLE_BOX)
                {
                    boxsize = 1.0f;
                }


                // Setup stuff...
                on_ground = (CHAR_FLAGS & CHAR_ON_PLATFORM)>>8;  // Usually FALSE, unless we stood on a platform last collision update...
                CHAR_FLAGS &= ~(CHAR_ON_PLATFORM);


                // Do the character-terrain collisions...
                global_collision_hit_wall = FALSE;
                if(character_data[96]&OTHER_FLAG_NO_WALL_COLLIDE)
                {
                    // Oops...  Looks like we can pass through terrain...  Never mind...
                    global_collision_z = -999.0f;
                }
                else
                {
                    // Check the heightmap a bunch of times (several samples in concentric circles)...
                    repeat(j, NUMBER_OF_COLLISION_ITERATIONS)
                    {
                        global_collision_vx = 0.0f;
                        global_collision_vy = 0.0f;
                        global_collision_z = -9999.0f;
                        global_collision_fail_count = 0;

                        k = 0;
                        while(k < NUM_SINE_ENTRIES)
                        {
                            sine = sine_table[k]*boxsize;
                            cosine = cosine_table[k]*boxsize;
                            char_collision_point(position_xyz, velocity_xyz, sine, cosine);
                            k+=256;
                        }


                        // Did we hit a wall?
                        if(global_collision_fail_count)
                        {
                            // Yup we did...  Now we'll have to modify our velocity and do the
                            // checks again until it works...
                            x = (velocity_xyz[X]*velocity_xyz[X] + velocity_xyz[Y]*velocity_xyz[Y]);
                            y = (global_collision_vx*global_collision_vx + global_collision_vy*global_collision_vy);
                            // Scale the velocity correction amount (global_collision_v's) to match
                            // the old in magnitude (but not direction)...
                            x = (float) sqrt(x/y);
                            global_collision_vx*=x;
                            global_collision_vy*=x;
                            // Then do a weighted average of 'em for our new velocity...
                            velocity_xyz[X] = (velocity_xyz[X]*0.80f)+(global_collision_vx*0.40f);
                            velocity_xyz[Y] = (velocity_xyz[Y]*0.80f)+(global_collision_vy*0.40f);
                        }
                        else
                        {
                            // Nope, we're okay to move this character...  Stop iterating through
                            // these collision checks...
                            j = NUMBER_OF_COLLISION_ITERATIONS;
                        }
                    }
                    if(global_collision_fail_count)
                    {
                        // Even our last check failed...  Looks like we'll just have to stop movement...
                        velocity_xyz[X] = 0.0f;
                        velocity_xyz[Y] = 0.0f;
                        global_collision_z = *((float*) (character_data + 44));  // Use floor height from last position...
                    }
                    // Save our floor height for later...
                    char_floor_point(position_xyz);
                }
                *((float*) (character_data + 44)) = global_collision_z;


                // Actually move the character...  At this point our velocity has been approved...  Wherever we end up should be safe for our character to fit in (not bumping any walls)...  At least that's how it's supposed to work...
                position_xyz[X]+=velocity_xyz[X];
                position_xyz[Y]+=velocity_xyz[Y];
                position_xyz[Z]+=velocity_xyz[Z];


                // Stop character from passing through ceiling...
                position_xyz[Z] = (position_xyz[Z] > ROOM_CEILING_BUMP_Z) ? ROOM_CEILING_BUMP_Z : position_xyz[Z];


                // Now check to see if the character hit the floor below 'em...
//                if(position_xyz[Z] < global_collision_z && velocity_xyz[Z] < 0.0f)
//                if(position_xyz[Z] < (global_collision_z-velocity_xyz[Z]) && velocity_xyz[Z] < 0.0f)
                if(position_xyz[Z] < (global_collision_z-velocity_xyz[Z]+0.25f) && velocity_xyz[Z] < 0.0f)
                {
                    // Character is below the ground and sinking...  Raise 'em up...
                    position_xyz[Z] = ((position_xyz[Z]-velocity_xyz[Z]+0.25f) > global_collision_z) ? global_collision_z : (position_xyz[Z]-velocity_xyz[Z]+0.25f);
                    on_ground = TRUE;
                }



//                // Now check to see if the character hit the floor below 'em...
//                if(position_xyz[Z] < (global_collision_z+0.25f) && velocity_xyz[Z] < 0.0f)
//                {
//                    // Character is below the ground and sinking...  Raise 'em up...
//                    position_xyz[Z] = ((position_xyz[Z]+0.25f) > global_collision_z) ? global_collision_z : (position_xyz[Z]+0.25f);
//                    on_ground = TRUE;
//                }


                // Now check if the character is hovering...
                if(CHAR_FLAGS & CHAR_HOVER_ON)
                {
                    y = (room_water_level > global_collision_z) ? room_water_level : global_collision_z;
                    y+=1.0f;
                    (*((float*) (character_data + 20))) = (*((float*) (character_data + 20)) < y) ? (((*((float*) (character_data + 20)))*0.80f) + (y*0.20f)) : (*((float*) (character_data + 20)));

                    y = (*((unsigned short*) (character_data+42)) == 0) ? 0.5f*sine_table[((main_game_frame+(i<<3))&63)<<6] : 0.0f;  // Waver amount, or 0.0 if petrified...
                    y += *((float*) (character_data + 20));  // plus hoverz...
                    velocity_xyz[Z] = ((y - position_xyz[Z]) * 0.01f) + (velocity_xyz[Z]*0.90f);
                    velocity_xyz[Z] = (on_ground && velocity_xyz[Z] < 0.0f) ? (0.0f) : (velocity_xyz[Z]);  // Don't allow movement below ground...
//                    (*((float*) (character_data + 20))) = (position_xyz[Z] < (global_collision_z+1.25f)) ? ((*((float*) (character_data + 20)))+0.025f) : (*((float*) (character_data + 20)));  // Slowly correct hoverz if we're bumping the ground...
                    on_ground = FALSE;
                }



                // Give the character a hit wall event...
                if(global_collision_hit_wall)
                {
                    // Check the wall collision delay timer (only give an event if this is 0, to prevent getting lots of events)
                    if(character_data[191] == 0)
                    {
                        character_data[191] = WALL_COLLISION_DELAY;
                        character_data[67] = EVENT_HIT_WALL;
                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                    }
                }




                // Do water checks...
                in_water = (position_xyz[Z] < room_water_level);
                if(in_water)
                {
                    // Character is in water/lava/sand area...
                    if(!(CHAR_FLAGS & CHAR_IN_WATER))
                    {
                        CHAR_FLAGS |= CHAR_IN_WATER;

                        // Call the water event for making splashes...
                        character_data[67] = EVENT_HIT_WATER;
                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                        if(room_water_type == WATER_TYPE_LAVA)
                        {
                            if(((signed char*)character_data)[100] < 4)
                            {
                                (*((unsigned short*) (character_data+168))) = drown_delay[character_data[100]] + 15;
                            }
                        }
                    }




                    // Drown timer...
                    if(((position_xyz[Z]+height-1.0f) < room_water_level && room_water_type != WATER_TYPE_LAVA) || (position_xyz[Z] < room_water_level && room_water_type == WATER_TYPE_LAVA && ((signed char*)character_data)[100] < 4))  // DefFire of 4 prevents lava damage...
                    {
                        if((character_data[82] > 0) && (character_data[80] < 255) && (!(character_data[105]&VIRTUE_FLAG_NO_DROWN) || room_water_type == WATER_TYPE_LAVA))
                        {
                            character_data[49]++;
                            if(character_data[49] >= drown_delay[character_data[100]])  // Amount of time before damage based on DefFire for lava...
                            {
                                // Uh, oh...  We flipped the drown timer...  That means we gotta do some damage...
                                // Decrement Hits...  Increment Taps...
                                if(character_data[82] > 0)
                                {
                                    character_data[82]--;
                                    character_data[81]++;
                                }


                                // Spawn a damage particle...
                                if(room_water_type != WATER_TYPE_SAND)
                                {
                                    child_data = obj_spawn(PARTICLE, position_xyz[X], position_xyz[Y], position_xyz[Z]+(character_data[175] - 1), pnumber_file, MAX_PARTICLE);
                                    if(child_data)
                                    {
                                        // Tint the number...
                                        if(room_water_type == WATER_TYPE_WATER)
                                        {
                                            child_data[48] = 255;
                                            child_data[49] = 255;
                                            child_data[50] = 255;
                                        }
                                        else
                                        {
                                            child_data[48] = 255;
                                            child_data[49] = 50;
                                            child_data[50] = 0;
                                            (*((unsigned short*) (character_data+168))) = character_data[49] + 15;
                                        }
                                        child_data[51] = 1;
                                    }
                                }


                                // Reset drown timer...
                                character_data[49] = 0;


                                // Call the event function if the drowning killed the character...
                                if(character_data[82] == 0)
                                {
                                    character_data[67] = EVENT_DAMAGED;
                                    global_attacker = i;
                                    global_attack_spin = (*((unsigned short*) (character_data + 56))) + 32768;
                                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                                }
                                else
                                {
                                    // Otherwise call the drown event, so we know to spawn some bubbles...
                                    character_data[67] = EVENT_DROWN;
                                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                                }
                            }
                        }
                    }
                    else
                    {
                        // Reset drown timer...
                        character_data[49] = 0;


                        // Give us some ripples...
                        if(((main_game_frame+i)&7) < 1)
                        {
                            character_data[67] = EVENT_RIPPLE;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                        }
                    }



                    // Make the character sink or swim depending on flag...
                    if(room_water_type == WATER_TYPE_LAVA)
                    {
                        velocity_xyz[Z] *= 0.60f;
                        on_ground = TRUE;
                    }
                    else if(room_water_type == WATER_TYPE_SAND)
                    {
                        if(velocity_xyz[Z] < (GRAVITY*-0.5f))
                        {
                            velocity_xyz[Z] = GRAVITY*-0.5f;
                        }
                        on_ground = TRUE;
                    }
                    else
                    {
                        can_swim = (action_first_frame[ACTION_SWIM] != 65535) && (character_data[82] > 0) && (*((unsigned short*) (character_data+42)) == 0);
                        if(can_swim)
                        {
                            // Swim action...
                            if(!on_ground && (position_xyz[Z]+height*0.50f) < room_water_level)
                            {
                                character_data[66] = ACTION_SWIM;
                            }

                            // Float on surface...  Bouy'd up by percentage of height under water...
                            if(height < 2.1f)
                            {
                                y = room_water_level - position_xyz[Z] + 4.0f;
                            }
                            else
                            {
                                y = room_water_level - position_xyz[Z] + 2.20f;
                            }
                            y += 0.25f*sine_table[((main_game_frame+(i<<3))&63)<<6];  // waver amount...
                            velocity_xyz[Z] -= (GRAVITY*y*0.25f);
                            if(velocity_xyz[Z] > 2.0f)
                            {
                                // Limit rise rate so characters don't go shooting out of the water...
                                velocity_xyz[Z] = 2.0f;
                            }
                        }
                        velocity_xyz[Z] *= 0.90f;
                    }
                }
                else
                {
                    CHAR_FLAGS &= ~(CHAR_IN_WATER);
                    if(character_data[66] == ACTION_SWIM)
                    {
                        character_data[65] = ACTION_STAND;
                        character_data[66] = ACTION_STAND;
                    }


                    // Reset drown timer...
                    character_data[49] = 0;


                    // Pit hurt...
                    if((position_xyz[Z] < ROOM_PIT_HURT_LEVEL) && !(CHAR_FLAGS & CHAR_HOVER_ON))
                    {
                        // Call the pit event function...  Should poof character and play scream sound...
                        character_data[67] = EVENT_FELL_IN_PIT;
                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                    }
                }


                if(on_ground)
                {
                    if(CHAR_FLAGS & CHAR_FALL_ON)
                    {
                        CHAR_FLAGS &= ~(CHAR_FALL_ON);
                        // Call the event function
                        character_data[67] = EVENT_HIT_FLOOR;
                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                    }
                    CHAR_FLAGS |= CHAR_ON_GROUND;
                }
                else
                {
                    CHAR_FLAGS &= ~(CHAR_ON_GROUND);
                }





                // Generate the character's matrix...
                x = 1.0f;
                sine = sine_table[(*((unsigned short*) (character_data+56)))>>4];
                cosine = cosine_table[(*((unsigned short*) (character_data+56)))>>4];


                // Flatten'd characters...
                if(character_data[48] < 255)
                {
                    x = ((255 - character_data[48]) * ONE_OVER_512) + 1.0f;
                    sine*=x;
                    cosine*=x;
                    x = character_data[48] * ONE_OVER_256;
                    j = character_data[48] + (character_data[48]>>4) + 1;
                    if(j > 255) { j = 255; }
                    character_data[48] = (unsigned char) j;
                }


                // Side
                *((float*) (character_data+108)) = sine;
                *((float*) (character_data+112)) = -cosine;
                *((float*) (character_data+116)) = 0.0f;


                // Front
                *((float*) (character_data+120)) = cosine;
                *((float*) (character_data+124)) = sine;
                *((float*) (character_data+128)) = 0.0f;

    
                // Up
                *((float*) (character_data+132)) = 0.0f;
                *((float*) (character_data+136)) = 0.0f;
                *((float*) (character_data+140)) = x;







                // Give character velocity for next time, based on frame...  If character isn't
                // on the ground, velocity is kept from last frame...
                if(on_ground)
                {
                    // Do we need to stop swimming?
                    if(character_data[66] == ACTION_SWIM)
                    {
                        character_data[66] = ACTION_STAND;
                    }


                    // Get the movement offset from the current frame
                    velx = (*((float*) (data+3)));
                    vely = (*((float*) (data+7)));

                    velx = (velx + (velx*dexterity*0.02f));
                    vely = (vely + (vely*dexterity*0.02f));

                    velocity_xyz[X] = (*((float*) (character_data+108)))*velx + (*((float*) (character_data+120)))*vely;
                    velocity_xyz[Y] = (*((float*) (character_data+112)))*velx + (*((float*) (character_data+124)))*vely;


                    if((turning && !(character_data[104] & MORE_FLAG_FAST_TURN)) || in_water)
                    {
                        velocity_xyz[X] *= 0.5f;
                        velocity_xyz[Y] *= 0.5f;

                        if(in_water && room_water_type != WATER_TYPE_WATER)
                        {
                            // Sand and lava slow us down more than water...
                            velocity_xyz[X] *= 0.75f;
                            velocity_xyz[Y] *= 0.75f;
                        }

                    }

                    // Bounce off ground just a little bit...
                    if(velocity_xyz[Z] < 0.0f && !in_water)
                    {
                        velocity_xyz[Z] *= BOUNCE_SCALE;
                    }
                }
                else
                {
                    if((action >= ACTION_JUMP_BEGIN && action <= ACTION_JUMP_END) || (action >= ACTION_DOUBLE_BEGIN && action <= ACTION_DOUBLE_END))
                    {
                        // Jumping creature...  Allow direct control with goto point...
                        vely = (*((float*) (data+7))) * 3.75f;
                        vely = (vely + (vely*dexterity*0.02f));

                        x = (*((float*) (character_data+12)));
                        y = (*((float*) (character_data+16)));
                        x-=position_xyz[X];
                        y-=position_xyz[Y];
                        velx = (x*x + y*y);
                        velx = (float) sqrt(velx);
                        velx += 1.0f;  // Prevents divide by 0 and shakiness associated with small goto offset...
                        vely = vely/velx;
                        x*=vely;
                        y*=vely;
                        if(CHAR_FLAGS & CHAR_HOVER_ON)
                        {
                            // Flying characters have custom friction and poor acceleration...
                            velocity_xyz[X] += x*0.1f;
                            velocity_xyz[Y] += y*0.1f;
                            velocity_xyz[X] *= (*((float*) (character_data+212)));  //0.95f;
                            velocity_xyz[Y] *= (*((float*) (character_data+212)));  //0.95f;
                        }
                        else
                        {
                            // Jumping characters have high friction for better control...
                            velocity_xyz[X] += x;
                            velocity_xyz[Y] += y;
                            velocity_xyz[X] *= 0.75f;
                            velocity_xyz[Y] *= 0.75f;
                        }
                    }
                    else
                    {
                        // Swimming or Hovering creature...
                        velx = (*((float*) (data+3)))*0.025f;
                        vely = (*((float*) (data+7)))*0.025f;
                        velx = (velx + (velx*dexterity*0.02f));
                        vely = (vely + (vely*dexterity*0.02f));
                        velocity_xyz[X] += (*((float*) (character_data+108)))*velx + (*((float*) (character_data+120)))*vely;
                        velocity_xyz[Y] += (*((float*) (character_data+112)))*velx + (*((float*) (character_data+124)))*vely;
                        velocity_xyz[X] *= 0.975f;
                        velocity_xyz[Y] *= 0.975f;
                    }
                }
            }




            // Do frame events and callback...
            if(frame_event_flags&15)
            {
                character_data[67] = frame_event_flags;
                fast_run_script(main_character_script_start[i], FAST_FUNCTION_FRAMEEVENT, character_data);
            }



            // Action animations...
            if(character_data[65] != ACTION_BONING)
            {
                // Start a new action if .action isn't what it was before...  Only allow if not petrified...
                if(action != character_data[65] && (*((unsigned short*) (character_data+42)) == 0))
                {
                    // Start a new action...
                    *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                    if((*((unsigned short*) (character_data+178))) == 65535)
                    {
                        // Action wasn't found...
                        if(character_data[65] >= ACTION_BASH_LEFT && character_data[65] <= ACTION_SLASH_RIGHT)
                        {
                            // Tried to do an attack action...  Try a different attack instead...
                            j = character_data[65] & 1;
                            if(action_first_frame[ACTION_BASH_LEFT | j] != 65535)
                            {
                                // Bash action is valid...
                                character_data[65] = ACTION_BASH_LEFT | j;
                                *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            }
                            else if(action_first_frame[ACTION_THRUST_LEFT | j] != 65535)
                            {
                                // Thrust action is valid...
                                character_data[65] = ACTION_THRUST_LEFT | j;
                                *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            }
                            else if(action_first_frame[ACTION_SLASH_LEFT | j] != 65535)
                            {
                                // Slash action is valid...
                                character_data[65] = ACTION_SLASH_LEFT | j;
                                *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            }
                            else
                            {
                                // Default to stand or jump action...
                                if(action_first_frame[ACTION_STAND] != 65535)
                                {
                                    character_data[65] = ACTION_STAND;
                                }
                                else
                                {
                                    character_data[65] = ACTION_JUMP;
                                }
                                *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            }
                        }
                        else
                        {
                            // Default to stand or jump action...
                            if(action_first_frame[ACTION_STAND] != 65535)
                            {
                                character_data[65] = ACTION_STAND;
                            }
                            else
                            {
                                character_data[65] = ACTION_JUMP;
                            }
                            *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                        }
                    }
                }
                else
                {
                    // Increment the frame...  Only if not petrified...
                    if(*((unsigned short*) (character_data+42)) == 0)
                    {
                        (*((unsigned short*) (character_data+178)))++;
                        if(next_action != character_data[65])
                        {
                            // Done with this action...  Return to the default action...
                            character_data[65] = character_data[66];
                            *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            if((*((unsigned short*) (character_data+178))) == 65535)
                            {
                                // Default to stand or jump action
                                if(action_first_frame[ACTION_STAND] != 65535)
                                {
                                    character_data[65] = ACTION_STAND;
                                }
                                else
                                {
                                    character_data[65] = ACTION_JUMP;
                                }
                                character_data[66] = character_data[65];
                                *((unsigned short*) (character_data+178)) = action_first_frame[character_data[65]];
                            }
                        }
                    }
                }
            }
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



            // Wall collision delay timer
            if(character_data[191])
            {
                character_data[191]--;
            }


            // Refresh timer
            if(*((unsigned short*) (character_data+52)) > 0)
            {
                (*((unsigned short*) (character_data+52)))--;
                if(*((unsigned short*) (character_data+52)) == 0)
                {
                    // Call the refresh function
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_REFRESH, character_data);
                }
            }



            // Event timer
            if(*((unsigned short*) (character_data+54)) > 0)
            {
                (*((unsigned short*) (character_data+54)))--;
                if(*((unsigned short*) (character_data+54)) == 0)
                {
                    // Call the event handler...
                    character_data[67] = EVENT_TIMER;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }


            // Secondary event timer
            if(*((unsigned short*) (character_data+200)) > 0)
            {
                (*((unsigned short*) (character_data+200)))--;
                if(*((unsigned short*) (character_data+200)) == 0)
                {
                    // Call the event handler...
                    character_data[67] = EVENT_SECONDARY_TIMER;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }


            // Thirdiary event timer
            if(*((unsigned short*) (character_data+202)) > 0)
            {
                (*((unsigned short*) (character_data+202)))--;
                if(*((unsigned short*) (character_data+202)) == 0)
                {
                    // Call the event handler...
                    character_data[67] = EVENT_THIRDIARY_TIMER;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }


            // No collision timer
            if(*((unsigned short*) (character_data+166)) > 0)
            {
                (*((unsigned short*) (character_data+166)))--;
            }


            // Food timer (food of 0 means don't bother with hunger for this character...)
            if(*((unsigned short*) (character_data+180)) > 0)
            {
                (*((unsigned short*) (character_data+180)))--;

                // Do hunger damage...
                if(*((unsigned short*) (character_data+180)) == 0)
                {
                    *((unsigned short*) (character_data+180)) = 3600;  // Give 'em a little while 'til we hurt 'em again...


                    if(character_data[82] > 0)
                    {
                        // Decrement Hits...  Increment Taps...
                        character_data[82]--;
                        character_data[81]++;


                        // Spawn a damage particle...
                        child_data = obj_spawn(PARTICLE, position_xyz[X], position_xyz[Y], position_xyz[Z]+(character_data[175] - 1), pnumber_file, MAX_PARTICLE);
                        if(child_data)
                        {
                            // Tint the number...
                            child_data[48] = 255;
                            child_data[49] = 0;
                            child_data[50] = 0;
                            child_data[51] = 1;
                        }


                        // Call the event function if the hunger killed the character...
                        if(character_data[82] == 0)
                        {
                            character_data[67] = EVENT_DAMAGED;
                            global_attacker = i;
                            global_attack_spin = (*((unsigned short*) (character_data + 56))) + 32768;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                        }
                        else
                        {
                            // Otherwise they get a hunger damaged event...
                            character_data[67] = EVENT_HUNGER_DAMAGED;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                        }
                    }
                }
            }





            // Poison (color) timer
            if(*((unsigned short*) (character_data+40)) > 0)
            {
                (*((unsigned short*) (character_data+40)))--;


                // Do poison damage...  Every 256 frames...  About once every 4 seconds...
                if(((*((unsigned short*) (character_data+40))) & 255)== 0)
                {
                    if(character_data[82] > 0 && (*((unsigned short*) (character_data+40))) > 64)
                    {
                        // Decrement Hits...
                        character_data[82]--;


                        // Spawn a damage particle...
                        child_data = obj_spawn(PARTICLE, position_xyz[X], position_xyz[Y], position_xyz[Z]+(character_data[175] - 1), pnumber_file, MAX_PARTICLE);
                        if(child_data)
                        {
                            // Tint the number according to the damage type...
                            child_data[48] = damage_color_rgb[DAMAGE_ACID][0];
                            child_data[49] = damage_color_rgb[DAMAGE_ACID][1];
                            child_data[50] = damage_color_rgb[DAMAGE_ACID][2];
                            child_data[51] = 1;
                        }


                        // Call the event function if the poison killed the character...
                        if(character_data[82] == 0)
                        {
                            character_data[67] = EVENT_DAMAGED;
                            global_attacker = (*((unsigned short*) (character_data + 208)));  // self.lasthit
                            global_attack_spin = (*((unsigned short*) (character_data + 56))) + 32768;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                        }
                    }
                }
            }


            // Petrify (texture) timer
            if(*((unsigned short*) (character_data+42)) > 0)
            {
                // Character is petrified...
                (*((unsigned short*) (character_data+42)))--;

                // Stop movement...
                velocity_xyz[X] = 0.0f;
                velocity_xyz[Y] = 0.0f;
            }
            else
            {
                // Character is not petrified...
                // Increase the combo timer...
                character_data[72]++;
                // Decrease the combo counter (number of times character has been hit) if the combo timer is high enough...
                if(character_data[72] > 120)
                {
                    if(character_data[75] > 0)
                    {
                        character_data[75]--;
                    }
                    character_data[72] = 60;
                }
            }


            // Petrify events...
            if(character_data[104] & MORE_FLAG_PETRIFY_ON)
            {
                // Character was petrified last frame...
                if(*((unsigned short*) (character_data+42)) == 0)
                {
                    // Character is no longer petrified...
                    character_data[67] = EVENT_UNPETRIFIED;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }
            else
            {
                // Character wasn't petrified last frame...
                if(*((unsigned short*) (character_data+42)) > 0)
                {
                    // Character is now petrified...
                    character_data[67] = EVENT_PETRIFIED;
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                }
            }


            // Remember whether we're petrified or not for next frame...
            character_data[104] = (*((unsigned short*) (character_data+42)) > 0) ? (character_data[104] | MORE_FLAG_PETRIFY_ON) : (character_data[104] & (255-MORE_FLAG_PETRIFY_ON));


            // Damage (color) timer
            if(*((unsigned short*) (character_data+168)) > 0)
            {
                (*((unsigned short*) (character_data+168)))--;
            }


            // Invincibility timer
            if(*((unsigned short*) (character_data+170)) > 0)
            {
                (*((unsigned short*) (character_data+170)))--;
            }


            // Reload left timer (doubles as a reload timer for special 1)
            if(*((unsigned short*) (character_data+194)) > 0)
            {
                (*((unsigned short*) (character_data+194)))--;
            }


            // Reload right timer (doubles as a reload timer for special 2)
            if(*((unsigned short*) (character_data+196)) > 0)
            {
                (*((unsigned short*) (character_data+196)))--;
            }


            // AI timer
            if(*((unsigned short*) (character_data+176)) > 0)
            {
                (*((unsigned short*) (character_data+176)))--;
                if(*((unsigned short*) (character_data+176)) == 0)
                {
                    // Call the player function...
                    fast_run_script(main_character_script_start[i], FAST_FUNCTION_AISCRIPT, character_data);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void character_bone_frame_clear()
{
    // <ZZ> This function resets the bone frame list for each character to NULL...
    unsigned short i;
    repeat(i, MAX_CHARACTER)
    {
        temp_character_bone_frame[i] = NULL;
    }
}

//-----------------------------------------------------------------------------------------------
void character_bone_frame_all()
{
    // <ZZ> This function generates world bone frame data for all the characters in the room.
    //      The bone frame data is stored in mainbuffer, and is used to generate vertex
    //      coordinates for object models.  Bone frames should be generated every game cycle...
    unsigned short i;
    unsigned char* character_data;
    unsigned char* model_data;
    unsigned short frame;
    unsigned char* place_to_stick_bone_frame;
    unsigned char* mount_data;
    unsigned short mount;
    unsigned char bone;


    place_to_stick_bone_frame = mainbuffer;
    // Handle unmounted characters in first pass...
    repeat(i, MAX_CHARACTER)
    {
        temp_character_bone_frame[i] = NULL;
        if(main_character_on[i])
        {
            character_data = main_character_data[i];
            model_data=character_data+256;
            if(*((unsigned char**) model_data) != NULL)
            {
                mount_data = NULL;
                mount = *((unsigned short*) (character_data+164));
                if(mount < MAX_CHARACTER)
                {
                    if(main_character_on[mount])
                    {
                        mount_data = main_character_data[mount];
                    }
                }
                if(mount_data == NULL)
                {
                    // We're a normal, unmounted character...
                    // Copy the position matrix from the character's data...
                    script_matrix[0]  = *((float*) (character_data+108));
                    script_matrix[1]  = *((float*) (character_data+112));
                    script_matrix[2]  = *((float*) (character_data+116));
                    script_matrix[3]  = *((float*) (character_data+120));
                    script_matrix[4]  = *((float*) (character_data+124));
                    script_matrix[5]  = *((float*) (character_data+128));
                    script_matrix[6]  = *((float*) (character_data+132));
                    script_matrix[7]  = *((float*) (character_data+136));
                    script_matrix[8]  = *((float*) (character_data+140));
                    script_matrix[9]  = *((float*) (character_data));
                    script_matrix[10] = *((float*) (character_data+4));
                    script_matrix[11] = *((float*) (character_data+8)) + CHARACTER_Z_FLOAT;

                    // Generate the bone frame data...
                    frame = *((unsigned short*) (character_data + 178));
                    temp_character_bone_frame[i] = place_to_stick_bone_frame;
                    place_to_stick_bone_frame = render_generate_model_world_data(*((unsigned char**) model_data), frame, script_matrix, place_to_stick_bone_frame);
                }
            }
        }
    }
    // Handle mounted characters in second pass...
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            character_data = main_character_data[i];
            model_data=character_data+256;
            if(*((unsigned char**) model_data) != NULL)
            {
                mount_data = NULL;
                mount = *((unsigned short*) (character_data+164));
                if(mount < MAX_CHARACTER)
                {
                    if(main_character_on[mount])
                    {
                        mount_data = main_character_data[mount];
                    }
                    bone = character_data[64];  // Should've been set when we first mounted other character...
                }
                if(mount_data)
                {
                    // We're a mounted character...
                    // Generate the position matrix from the mount's saddle bone...
                    script_matrix_good_bone(bone, temp_character_bone_frame[mount], mount_data);


                    // Set the character's position based on the saddle matrix...
                    *((float*) (character_data)) = script_matrix[9];
                    *((float*) (character_data+4)) = script_matrix[10];
                    *((float*) (character_data+8)) = script_matrix[11];
                    *((unsigned short*) (character_data+56)) = *((unsigned short*) (mount_data+56));



                    // Update the character's matrix...
                    *((float*) (character_data+108)) = script_matrix[0];
                    *((float*) (character_data+112)) = script_matrix[1];
                    *((float*) (character_data+116)) = script_matrix[2];
                    *((float*) (character_data+120)) = script_matrix[3];
                    *((float*) (character_data+124)) = script_matrix[4];
                    *((float*) (character_data+128)) = script_matrix[5];
                    *((float*) (character_data+132)) = script_matrix[6];
                    *((float*) (character_data+136)) = script_matrix[7];
                    *((float*) (character_data+140)) = script_matrix[8];


                    // Generate the bone frame data...
                    frame = *((unsigned short*) (character_data + 178));
                    temp_character_bone_frame[i] = place_to_stick_bone_frame;
                    place_to_stick_bone_frame = render_generate_model_world_data(*((unsigned char**) model_data), frame, script_matrix, place_to_stick_bone_frame);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define BUTTON_PRESS            0
#define BUTTON_UNPRESS          1
#define BUTTON_PRESS_AND_HOLD   2
#define BUTTON_HANDLED          3
void character_button_function(unsigned short character, unsigned char code, unsigned char button, unsigned char axis)
{
    // <ZZ> This function stores button press information (for players or AI's) in a character's
    //      data block...  Character's ButtonEvent() script should be called at a later point to
    //      actually use items and set actions and stuff...
    unsigned char* character_data;

    if(character < MAX_CHARACTER)
    {
        character_data = main_character_data[character];
        button&=3;
        code&=3;
        switch(code)
        {
            case BUTTON_PRESS:
                character_data[198] |= (1<<button);
                character_data[199] = axis | (16<<button) | (character_data[199] & 240);
                character_data[36+button] = 1;  // Release right away...
                break;
            case BUTTON_UNPRESS:
                character_data[198] |= (16<<button);
                character_data[199] = axis | (character_data[199] & (255-(16<<button)));
                break;
            case BUTTON_PRESS_AND_HOLD:
                // Axis parameter is really time to hold...  0 means until unpressed...
                character_data[198] |= (1<<button);
                character_data[199] = (16<<button) | (character_data[199] & 240);
                character_data[36+button] = axis;  // Really duration...
                break;
            case BUTTON_HANDLED:
                global_button_handled = TRUE;
                break;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void character_local_player_control(void)
{
    // <ZZ> This function updates the characters controlled by the local players, so that
    //      their gotoxy's & stuff are set correctly...
    unsigned short i;
    unsigned char button;
    unsigned short character;
    unsigned char* character_data;
    unsigned char axis;
    float x, y;


    repeat(i, MAX_LOCAL_PLAYER)
    {
        character = local_player_character[i];
        if(character < MAX_CHARACTER)
        {
            if(main_character_on[character])
            {
                character_data = main_character_data[character];
                if(player_device_controls_active[i])
                {
                    // Generate gotoxy, based on camera rotation...  Map_side_xy shouldn't have gotten corrupted
                    // by window cameras...
                    x = *((float*) (character_data));
                    y = *((float*) (character_data+4));
                    x+= (player_device_xy[i][X]*map_side_xy[X] - player_device_xy[i][Y]*map_side_xy[Y])*10.0f;
                    y+= (player_device_xy[i][X]*map_side_xy[Y] + player_device_xy[i][Y]*map_side_xy[X])*10.0f;
                    *((float*) (character_data+12)) = x;
                    *((float*) (character_data+16)) = y;


                    // Turn off the AI timer...
                    (*((unsigned short*) (character_data+176))) = 0;


                    // Transfer player button presses into character data...
                    axis = ((player_device_xy[i][Y]<-0.10f) << 3);  // 8 = Up
                    axis|= ((player_device_xy[i][Y]>0.10f)  << 2);  // 4 = Down
                    axis|= ((player_device_xy[i][X]<-0.10f) << 1);  // 2 = Left
                    axis|= ((player_device_xy[i][X]>0.10f)      );  // 1 = Right
                    repeat(button, 4)
                    {
                        if(player_device_button_pressed[i][button])
                        {
                            character_button_function(character, BUTTON_PRESS, button, axis);
                            player_device_button_pressed[i][button] = FALSE;
                            character_data[36+button] = 0;  // Make it not release right away...
                        }    
                        if(player_device_button_unpressed[i][button])
                        {
                            character_button_function(character, BUTTON_UNPRESS, button, axis);
                            player_device_button_unpressed[i][button] = FALSE;
                            player_device_button_pressed[i][button] = FALSE;
                        }    
                    }
                    character_data[199] = (character_data[199] & 240) | (axis&15);  // Make sure baxis is always set for players...
                }
                else
                {
                    // Stop the character from moving...
                    x = *((float*) (character_data));
                    y = *((float*) (character_data+4));
                    *((float*) (character_data+12)) = x;
                    *((float*) (character_data+16)) = y;

                    // Turn off the AI timer...
                    (*((unsigned short*) (character_data+176))) = 0;

                    // Don't allow button presses just yet...
                    repeat(button, 4)
                    {
                        player_device_button_unpressed[i][button] = FALSE;
                        player_device_button_pressed[i][button] = FALSE;
                    }
                }
            }
            else
            {
                // Character is defeated...
                repeat(button, 4)
                {
                    player_device_button_unpressed[i][button] = FALSE;
                    player_device_button_pressed[i][button] = FALSE;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void character_refresh_items_all()
{
    // <ZZ> Calls the Refresh() script for each of a character's 4 items (weapons & actions)...
    unsigned short i;
    unsigned char slot;
    unsigned char* character_data;
    unsigned short item_type;
    unsigned char* item_script;


    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            // Call the refresh function for each of the character's four weapons/specials...
            character_data = main_character_data[i];

            // Only allow if not petrified and not mounted and not in deep water...
            if((*((unsigned short*) (character_data+42)) == 0) && (*((unsigned short*) (character_data+164)) >= MAX_CHARACTER) && ((((float*) character_data)[Z] > room_water_level)   || (room_water_type != WATER_TYPE_WATER)   ||   ((((float*) character_data)[Z] + (character_data[175] * 0.50f)) > room_water_level && (CHAR_FLAGS & CHAR_ON_GROUND))))
            {
                repeat(slot, 4)
                {
                    item_type = *(character_data + slot + 242);
                    item_script = item_type_script[item_type];
                    if(item_script)
                    {
                        // Call the refresh function...  Used to make flame effects & stuff on weapons...
                        global_item_index = item_type;
                        global_item_bone_name = slot+1;
                        fast_run_script(item_script, FAST_FUNCTION_REFRESH, character_data);
                    }
                }
            }
        }
    }
}


//-----------------------------------------------------------------------------------------------
