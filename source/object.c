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

// <ZZ> This file contains functions related to characters, particles, and windows...
//      obj_setup           - Gets ready to spawn stuff
//      obj_spawn           - Creates a new object of the desired type
//      obj_reset_property  - Gets ready to use properties
//      obj_add_property    - Registers a property name...  for window.x type stuff in script
//      obj_get_property    - Searches for a property name...  Returns index or -1 if not found
//      obj_get_script_name - Helper for the recompile functions
//      obj_recompile_start - Figures out which script each object is attached to
//      obj_recompile_end   - Gets the new script address for each object


#define CHARACTER 0         // Object types...
#define PARTICLE 1          //
#define WINDOW 2            //

#define CHARACTER_SIZE 616  // The number of bytes for a character's data
#define PARTICLE_SIZE 88    // The number of bytes for a particle's data
#define MAX_WINDOW 16       // The maximum number of windows a machine can have open
#define WINDOW_SIZE 616     // The number of bytes for a window's data

// Spawn stuff...
unsigned short global_spawn_owner = MAX_CHARACTER;
unsigned short global_spawn_target = MAX_CHARACTER;
unsigned char global_spawn_team = 0;
unsigned char global_spawn_subtype = 0;
unsigned char global_spawn_class = 255;


// Room stuff...
float room_min_xyz[3];
float room_size_xyz[3];


// The main data blocks for characters, particles, and windows...
unsigned char main_character_data[MAX_CHARACTER][CHARACTER_SIZE];
unsigned char main_particle_data[MAX_PARTICLE][PARTICLE_SIZE];
unsigned char main_window_data[MAX_WINDOW][WINDOW_SIZE];


// The script pointers for each object
unsigned char* main_character_script_start[MAX_CHARACTER];
unsigned char* main_particle_script_start[MAX_PARTICLE];
unsigned char* main_window_script_start[MAX_WINDOW];



// The script file names for each object
char main_character_script_name[MAX_CHARACTER][8];
char main_particle_script_name[MAX_PARTICLE][8];
char main_window_script_name[MAX_WINDOW][8];


// Property stuff...
#define MAX_PROPERTY 256  // Must be 256...  Or else script props need to have 2 byte extensions...
char property_token[MAX_PROPERTY][8];         // Tags for the properties...  the x of window.x...
char property_type[MAX_PROPERTY];             // F or I or others...
unsigned short property_offset[MAX_PROPERTY]; // Offset of data for this property
int obj_num_property;                         // The number of registered properties


// List of object numbers...  No particular order to contents
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Unused lists should be sorted...  Otherwise network numbers might get messed up...
// !!!BAD!!!  May be better to not keep unused lists for chars & parts, and just search backwards for highest character who's not on...
// !!!BAD!!!
// !!!BAD!!!
unsigned char  main_character_on[MAX_CHARACTER];
unsigned char  main_character_reserve_on[MAX_CHARACTER];
unsigned char  main_particle_on[MAX_PARTICLE];
unsigned short main_unused_window[MAX_WINDOW];
unsigned short main_window_order[MAX_WINDOW];

// How many items in the lists...
unsigned short main_unused_character_count;
unsigned short main_unused_particle_count;
unsigned short main_unused_window_count;
unsigned short main_used_window_count;
unsigned short main_window_poof_count;

//-----------------------------------------------------------------------------------------------
void obj_setup(void)
{
    // <ZZ> This function resets all of the lists for spawning objects
    unsigned short i;


    // Character list...
    repeat(i, MAX_CHARACTER)
    {
        main_character_on[i] = FALSE;
        main_character_reserve_on[i] = FALSE;
    }
    main_unused_character_count = MAX_CHARACTER;


    // Particle list...
    repeat(i, MAX_PARTICLE)
    {
        main_particle_on[i] = FALSE;
    }
    main_unused_particle_count = MAX_PARTICLE;


    // Window list...
    repeat(i, MAX_WINDOW)
    {
        main_unused_window[i] = i;
    }
    main_unused_window_count = MAX_WINDOW;
    main_used_window_count = 0;
}

//-----------------------------------------------------------------------------------------------
void obj_get_script_name(unsigned char* file_start, unsigned char* file_name)
{
    // <ZZ> This function searches the index for a RUN file that starts at file_start, and fills
    //      in the file_name if it finds one.  Fills in 0 if not.
    int i;
    unsigned char *data, *index;
    unsigned char file_type;

    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        file_type = *(index+4) & 15;
        if(file_type == SDF_FILE_IS_RUN)
        {
            data = (unsigned char*) sdf_read_unsigned_int(index);
            if(data == file_start)
            {
                if(strcmp(index+8, "GENERIC") == 0) return;
                memcpy(file_name, index+8, 8);
                return;
            }
        }
    }
    file_name[0] = 0;
}

//-----------------------------------------------------------------------------------------------
unsigned char* obj_spawn(unsigned char type, float x, float y, float z, unsigned char* script_file_start, unsigned short force_index)
{
    // <ZZ> This function spawns an object (CHARACTER, PARTICLE, or WINDOW) in the current_room.
    //      If we've run out of that type of object, the function returns NULL and doesn't spawn.
    //      Otherwise, it should return a pointer to the object's data (within the main arrays).
    //      If force_index is less than MAX_(whatever), that object number should be used instead
    //      of automagically assigning one...
    unsigned short i, j;
    unsigned char new_team, new_subtype, new_class;
    unsigned short new_owner, new_target;
    float floor_z;


    // Stuff for setting team & owner & target on spawn...
    new_team = global_spawn_team;
    new_owner = global_spawn_owner;
    new_target = global_spawn_target;
    new_subtype = global_spawn_subtype;
    new_class = global_spawn_class;
    global_spawn_owner = MAX_CHARACTER;
    global_spawn_target = MAX_CHARACTER;
    global_spawn_team = 0;
    global_spawn_subtype = 0;
    global_spawn_class = 255;



    if(script_file_start != NULL)
    {
        switch(type)
        {
            case CHARACTER:
                if(force_index < MAX_CHARACTER)
                {
                    // Use a specific character slot...
                    i = force_index;
                    if(!main_character_on[i])
                    {
                        main_unused_character_count--;
                    }
                }
                else if(main_unused_character_count > 0)
                {
                    // Search for an empty character slot...
                    i = MAX_CHARACTER-1;
                    while(main_character_on[i] || main_character_reserve_on[i])
                    {
                        if(i == 0)
                        {
                            // This shouldn't happen...
                            return NULL;
                        }
                        i--;
                    }
                    main_unused_character_count--;
                }
                else
                {
                    // No character slots left...
                    return NULL;
                }


                // At this point, i should be the correct index to use for this object...
                temp_character_bone_frame[i] = NULL;
                floor_z = room_heightmap_height_slow(roombuffer, x, y);
                if(z < floor_z)
                {
                    z = floor_z;
                }
                main_character_on[i] = TRUE;
                *((float*) (main_character_data[i])) = x;
                *((float*) (main_character_data[i]+4)) = y;
                *((float*) (main_character_data[i]+8)) = z;
                // Load default data...
                *((float*) (main_character_data[i]+12)) = x;
                *((float*) (main_character_data[i]+16)) = y;
                *((float*) (main_character_data[i]+20)) = z;
                *((float*) (main_character_data[i]+24)) = 0.0f;
                *((float*) (main_character_data[i]+28)) = 0.0f;
                *((float*) (main_character_data[i]+32)) = 0.0f;
                *((unsigned int*) (main_character_data[i]+36)) = 0;
                *((unsigned int*) (main_character_data[i]+40)) = 0;
                *((float*) (main_character_data[i]+44)) = floor_z;
                *((unsigned int*) (main_character_data[i]+48)) = 0;
                main_character_data[i][48] = 255;  // Flatness
                main_character_data[i][49] = 0;    // Drown Timer
                main_character_data[i][50] = 255;  // Last waypoint
                main_character_data[i][51] = 255;  // Next waypoint
                *((unsigned int*) (main_character_data[i]+52)) = 0;
                *((unsigned int*) (main_character_data[i]+56)) = 0;
                *((unsigned int*) (main_character_data[i]+60)) = 0;
                *((unsigned short*) (main_character_data[i]+62)) = i;  // Target = self...
                main_character_data[i][64] = NO_BONE;
                main_character_data[i][65] = ACTION_STAND;
                main_character_data[i][66] = ACTION_STAND;
                main_character_data[i][67] = 0;
                *((unsigned int*) (main_character_data[i]+68)) = 0;
                *((unsigned int*) (main_character_data[i]+72)) = 0;
                *((unsigned int*) (main_character_data[i]+76)) = 0;
                *((unsigned int*) (main_character_data[i]+80)) = 0;
                *((unsigned int*) (main_character_data[i]+84)) = 0;
                *((unsigned int*) (main_character_data[i]+88)) = 0;
                *((unsigned int*) (main_character_data[i]+92)) = 0;
                *((unsigned int*) (main_character_data[i]+96)) = 0;
                *((unsigned int*) (main_character_data[i]+100)) = 0;
                *((unsigned int*) (main_character_data[i]+104)) = 0;

                *((float*) (main_character_data[i]+108)) = 1.0f;
                *((float*) (main_character_data[i]+112)) = 0.0f;
                *((float*) (main_character_data[i]+116)) = 0.0f;
                *((float*) (main_character_data[i]+120)) = 0.0f;
                *((float*) (main_character_data[i]+124)) = 1.0f;
                *((float*) (main_character_data[i]+128)) = 0.0f;
                *((float*) (main_character_data[i]+132)) = 0.0f;
                *((float*) (main_character_data[i]+136)) = 0.0f;
                *((float*) (main_character_data[i]+140)) = 1.0f;

                *((unsigned int*) (main_character_data[i]+144)) = 0;
                *((unsigned int*) (main_character_data[i]+148)) = 0;
                *((unsigned int*) (main_character_data[i]+152)) = 0;
                *((unsigned int*) (main_character_data[i]+156)) = 0;

                *((float*) (main_character_data[i]+160)) = 1.0f;
                *((unsigned int*) (main_character_data[i]+164)) = 0;
                *((float*) (main_character_data[i]+168)) = 0.0f;
                *((unsigned int*) (main_character_data[i]+172)) = 0;
                *((unsigned int*) (main_character_data[i]+176)) = 0;
                *((unsigned int*) (main_character_data[i]+180)) = 0;
                *((unsigned int*) (main_character_data[i]+184)) = 0;
                *((unsigned int*) (main_character_data[i]+188)) = 0;
                *((unsigned int*) (main_character_data[i]+192)) = 0;
                *((unsigned int*) (main_character_data[i]+196)) = 0;
                *((unsigned int*) (main_character_data[i]+200)) = 0;
                *((unsigned int*) (main_character_data[i]+204)) = 0;
                *((unsigned int*) (main_character_data[i]+208)) = 0;
                *((float*) (main_character_data[i]+212)) = 0.95f;  // Default flying friction...
                main_character_data[i][216] = 0;  // Enchant flags...


                main_character_data[i][204] = new_class;  // Character class
                main_character_data[i][206] = 255;  // Brightness
                main_character_data[i][79] = 255;  // Alpha
                main_character_data[i][175] = 5;  // Height


                // Fill in owner, target and team...
                (*((unsigned short*)(main_character_data[i]+76))) = new_owner;
                (*((unsigned short*)(main_character_data[i]+62))) = new_target;
                main_character_data[i][78] = new_team;


                // Clear out inventory & equipment...
                j = 224;
                while(j < 252)
                {
                    *((unsigned int*)(main_character_data[i]+j)) = 0;
                    j+=4;
                }

                // Default to no rider/mount...
                (*((unsigned short*)(main_character_data[i]+106))) = 65535;  // self.rider
                (*((unsigned short*)(main_character_data[i]+164))) = 65535;  // self.mount


                // Fill in subtype (equipment colors 0 & 1...)
                main_character_data[i][240] = new_subtype;


                // Fill in mana cost (so players without mana can buy it with stat points...)
                main_character_data[i][217] = 5;


                // Clear out model assigns...
                j = 256;
                while(j < 616)
                {
                    *((unsigned char**)(main_character_data[i]+j)) = NULL;
                    j+=24;
                }


                // Clear the object spawn number...
                main_character_data[i][249] = 255;


                // Clear the remote index & IP Address...
                main_character_data[i][250] = 0;  // Remote index
                main_character_data[i][252] = 0;
                main_character_data[i][253] = 0;
                main_character_data[i][254] = 0;
                main_character_data[i][255] = 0;


                main_character_script_start[i] = script_file_start;
                obj_get_script_name(main_character_script_start[i], main_character_script_name[i]);
                main_character_data[i][251] = network_find_script_index(main_character_script_name[i]);
                fast_run_script(script_file_start, FAST_FUNCTION_SPAWN, main_character_data[i]);
                return main_character_data[i];
                break;
            case PARTICLE:
                if(force_index < MAX_PARTICLE)
                {
                    // Use a specific particle slot...
                    i = force_index;
                    if(!main_particle_on[i])
                    {
                        main_unused_particle_count--;
                    }
                }
                else if(main_unused_particle_count > 0)
                {
                    // Search for an empty particle slot...
                    i = MAX_PARTICLE-1;
                    while(main_particle_on[i])
                    {
                        if(i == 0)
                        {
                            // This shouldn't happen...
                            return NULL;
                        }
                        i--;
                    }
                    main_unused_particle_count--;
                }
                else
                {
                    // No particle slots left...
                    return NULL;
                }



                // At this point, i should be the correct index to use for this object...
                main_particle_on[i] = TRUE;
                *((float*) (main_particle_data[i])) = x;
                *((float*) (main_particle_data[i]+4)) = y;
                *((float*) (main_particle_data[i]+8)) = z;
                // Load default data...
                *((float*) (main_particle_data[i]+12)) = x;
                *((float*) (main_particle_data[i]+16)) = y+0.0001f;
                *((float*) (main_particle_data[i]+20)) = z;
                *((float*) (main_particle_data[i]+24)) = 0.0f;
                *((float*) (main_particle_data[i]+28)) = 0.0f;
                *((float*) (main_particle_data[i]+32)) = 0.0f;
                *((float*) (main_particle_data[i]+36)) = 1.0f;
                *((float*) (main_particle_data[i]+40)) = 1.0f;
                *((unsigned int*) (main_particle_data[i]+44)) = 0;
                *((unsigned int*) (main_particle_data[i]+48)) = 0;
                *((unsigned int*) (main_particle_data[i]+52)) = 0;
                *((unsigned int*) (main_particle_data[i]+56)) = 0;
                *((unsigned int*) (main_particle_data[i]+60)) = 0;
                *((unsigned int*) (main_particle_data[i]+64)) = 0;
                *((unsigned int*) (main_particle_data[i]+68)) = 0;
                *((unsigned int*) (main_particle_data[i]+72)) = 0;
                *((float*) (main_particle_data[i]+80)) = 0.0f;
                *((unsigned int*) (main_particle_data[i]+84)) = 0;
                main_particle_data[i][84] = TRUE;  // Just spawn'd...
                *((unsigned short*) (main_particle_data[i]+86)) = MAX_CHARACTER;  // Who it's stuck to...


                // Default tint color to white
                main_particle_data[i][48] = 255;  // Red
                main_particle_data[i][49] = 255;  // Green
                main_particle_data[i][50] = 255;  // Blue
                main_particle_data[i][79] = 255;  // Alpha


                // Default damage type...
                main_particle_data[i][66] = 15;  // No flags, type 15


                // Fill in owner, target and team...
                (*((unsigned short*)(main_particle_data[i]+76))) = new_owner;
                (*((unsigned short*)(main_particle_data[i]+62))) = new_target;
                main_particle_data[i][78] = new_team;


                main_particle_script_start[i] = script_file_start;
                fast_run_script(script_file_start, FAST_FUNCTION_SPAWN, main_particle_data[i]);


                return main_particle_data[i];
                break;
            case WINDOW:
                if(main_unused_window_count > 0)
                {
                    main_unused_window_count--;
                    i = main_unused_window[main_unused_window_count];
                    main_window_order[main_used_window_count] = i;
                    main_used_window_count++;
                    *((float*) (main_window_data[i])) = x;
                    *((float*) (main_window_data[i]+4)) = y;
                    *((float*) (main_window_data[i]+8)) = z;
                    *((unsigned short*) (main_window_data[i]+220)) = MAX_CHARACTER;


                    // Clear out inventory & equipment...
                    j = 224;
                    while(j < 252)
                    {
                        *((unsigned int*)(main_character_data[i]+j)) = 0;
                        j+=4;
                    }


                    // Clear out model assigns...
                    j = 256;
                    while(j < 616)
                    {
                        *((unsigned char**)(main_character_data[i]+j)) = NULL;
                        j+=24;
                    }


                    main_window_script_start[i] = script_file_start;
                    j = main_unused_window_count;
                    fast_run_script(script_file_start, FAST_FUNCTION_SPAWN, main_window_data[i]);
                    if(main_unused_window_count > j)
                    {
                        // Window poofed in its Spawn() script...
                        main_window_poof_count--;
                    }
                    return main_window_data[i];
                }
                break;
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------
void obj_destroy(unsigned char* object_data)
{
    // <ZZ> This function destroys an object that had been previously spawned, and shuffles
    //      around the data accordingly...
    int i, j;
    unsigned char refresh_script_already_ran;


    if(object_data >= main_particle_data[0] && object_data <= main_particle_data[MAX_PARTICLE-1])
    {
        // It's a particle, so destroy it...
        i = (object_data-main_particle_data[0])/PARTICLE_SIZE;
        i = i & (MAX_PARTICLE-1);
        if(main_particle_on[i])
        {
            main_unused_particle_count++;
            main_particle_on[i] = FALSE;
        }
    }
    else if(object_data >= main_character_data[0] && object_data <= main_character_data[MAX_CHARACTER-1])
    {
        // It's a character, so destroy it...
        i = (object_data-main_character_data[0])/CHARACTER_SIZE;
        i = i & (MAX_CHARACTER-1);
        if(main_character_on[i])
        {
//log_message("INFO:   Destroying character %d", i);
            main_character_on[i] = FALSE;
            if(!main_character_reserve_on[i])
            {
                main_unused_character_count++;
            }


            // Destroy all of the particles that are stuck to this character...
            repeat(j, MAX_PARTICLE)
            {
                if(main_particle_on[j])
                {
                    if((*((unsigned short*) (main_particle_data[j]+60))) & PART_STUCK_ON)
                    {
                        if((*((unsigned short*) (main_particle_data[j]+86))) == i)
                        {
                            main_unused_particle_count++;
                            main_particle_on[j] = FALSE;
                        }
                    }
                }
            }


//            // Unassign local player if it was a local player...
//            repeat(j, MAX_LOCAL_PLAYER)
//            {
//                // Is this player active?
//                if(local_player_character[j] == i)
//                {
//                    local_player_character[j] = MAX_CHARACTER;
//                }
//            }
        }
    }
    else if(object_data >= main_window_data[0] && object_data <= main_window_data[MAX_WINDOW-1])
    {
        // It's a window, so destroy it...
        i = (object_data-main_window_data[0])/WINDOW_SIZE;
        main_unused_window[main_unused_window_count] = i;
        main_unused_window_count++;
        // Remove the window from the display order
        refresh_script_already_ran = TRUE;
        j = 0;
        while(j < main_used_window_count && main_window_order[j] != i)
        {
            if(main_window_data[main_window_order[j]] == current_object_data)
            {
                // The poof'd window comes after the current window...  That means we
                // didn't run its refresh() script yet, and don't need to count it
                // as being poof'd...
                refresh_script_already_ran = FALSE;
            }
            j++;
        }
        if(refresh_script_already_ran)
        {
            main_window_poof_count++;
        }
        main_used_window_count--;
        while(j < main_used_window_count)
        {
            main_window_order[j] = main_window_order[j+1];
            j++;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void obj_poof_all(unsigned char type)
{
    // <ZZ> This function poofs all of the objects of a given type...
    unsigned short i;
    if(type == CHARACTER)
    {
        repeat(i, MAX_CHARACTER)
        {
            main_character_on[i] = FALSE;
            main_character_reserve_on[i] = FALSE;
        }
        main_unused_character_count = MAX_CHARACTER;


        // Turn off all local player controls...
        repeat(i, MAX_LOCAL_PLAYER)
        {
            local_player_character[i] = MAX_CHARACTER;
        }
    }
    if(type == PARTICLE)
    {
        repeat(i, MAX_PARTICLE)
        {
            main_particle_on[i] = FALSE;
        }
        main_unused_particle_count = MAX_PARTICLE;
    }
    // Don't bother with windows...
}

//-----------------------------------------------------------------------------------------------
unsigned short promotion_buffer[MAX_WINDOW];
unsigned short promotion_count;
void promote_window(int window)
{
    // <ZZ> This function puts a given window at the very top of the window_order.
    int i;

    i = 0;
    while(i < main_used_window_count && main_window_order[i] != window)
    {
        i++;
    }
    if(i < main_used_window_count)
    {
        // We found the given window in the current list, so remove it by moving everything
        // else back a slot...
        main_used_window_count--;
        while(i < main_used_window_count)
        {
            main_window_order[i] = main_window_order[i+1];
            i++;
        }
        main_window_order[main_used_window_count] = window;
        main_used_window_count++;
    }
}

//-----------------------------------------------------------------------------------------------
void obj_reset_property(void)
{
    // <ZZ> This function clears out our properties
    obj_num_property = 0;
}

//-----------------------------------------------------------------------------------------------
void obj_add_property(char* tag, char type, char* offset)
{
    // <ZZ> This function registers a new property index...  The x in window.x for example.
    //      Tag is the name of the property, type is F or I or something, offset is the
    //      location of the data for this property relative to the start of the object's
    //      data (and is stored in ASCII text... "202" not the number 202).
    int i;

    if(obj_num_property < MAX_PROPERTY)
    {
        // Copy the name
        repeat(i, 8) { property_token[obj_num_property][i] = tag[i]; }
        property_token[obj_num_property][7] = 0;

        // Copy the type
        property_type[obj_num_property] = type;

        // Copy the offset
        sscanf(offset, "%d", &i);
        property_offset[obj_num_property] = i;

        #ifdef VERBOSE_COMPILE
            log_message("INFO:   Added property %s as number %d...  Type %c, Offset %d", property_token[obj_num_property], obj_num_property, property_type[obj_num_property], property_offset[obj_num_property]);
        #endif
        obj_num_property++;
    }
}

//-----------------------------------------------------------------------------------------------
int obj_get_property(char* token)
{
    // <ZZ> This function returns the index of the first property that matches token.  If there
    //      aren't any matches, it returns -1.
    int i;

    // Check each define...
    repeat(i, obj_num_property)
    {
        if(strcmp(token, property_token[i]) == 0) return i;
    }
    return -1;
}

//-----------------------------------------------------------------------------------------------
void obj_recompile_start(void)
{
    // <ZZ> This function gets an 8 character filename for each object's script.
    int i;
    int object;
    


    // Do all windows
    repeat(i, main_used_window_count)
    {
        object = main_window_order[i];
        obj_get_script_name(main_window_script_start[object], main_window_script_name[object]);
    }


    // Do all particles
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            obj_get_script_name(main_particle_script_start[i], main_particle_script_name[i]);
        }
    }


    // Do all characters
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i] || main_character_reserve_on[i])
        {
            obj_get_script_name(main_character_script_start[i], main_character_script_name[i]);
        }
    }
}

//-----------------------------------------------------------------------------------------------
void obj_recompile_end(void)
{
    // <ZZ> This function fills in the script_start for each object, based on the filename we
    //      found earlier.
    int i;
    int object;
    unsigned char* index;


    // Do all windows
    repeat(i, main_used_window_count)
    {
        object =  main_window_order[i];
        index = sdf_find_filetype(main_window_script_name[object], SDF_FILE_IS_RUN);
        if(index)
        {
            main_window_script_start[object] = (unsigned char*) sdf_read_unsigned_int(index);
        }
        else
        {
            // Try to use a generic script, so we don't have to respawn... 
            // Generic is handled in the _start function, so the script doesn't get lost...
            index = sdf_find_filetype("GENERIC", SDF_FILE_IS_RUN);
            if(index)
            {
                main_window_script_start[object] = (unsigned char*) sdf_read_unsigned_int(index);
            }
            else
            {
                obj_destroy(main_window_data[object]);
                i--;
            }
        }
    }




    // Do all particles
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            index = sdf_find_filetype(main_particle_script_name[i], SDF_FILE_IS_RUN);
            if(index)
            {
                main_particle_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
            }
            else
            {
                // Try to use a generic script, so we don't have to respawn... 
                // Generic is handled in the _start function, so the script doesn't get lost...
                index = sdf_find_filetype("GENERIC", SDF_FILE_IS_RUN);
                if(index)
                {
                    main_particle_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
                }
                else
                {
                    obj_destroy(main_particle_data[i]);
                    i--;
                }
            }
        }
    }



    // Do all characters
    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            index = sdf_find_filetype(main_character_script_name[i], SDF_FILE_IS_RUN);
            if(index)
            {
                main_character_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
            }
            else
            {
                // Try to use a generic script, so we don't have to respawn... 
                // Generic is handled in the _start function, so the script doesn't get lost...
                index = sdf_find_filetype("GENERIC", SDF_FILE_IS_RUN);
                if(index)
                {
                    main_character_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
                }
                else
                {
                    obj_destroy(main_character_data[i]);
                    i--;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
