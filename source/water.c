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

// <ZZ> This file has stuff for drawing the water.  Done as a multitextured plane with animated
//      textures...
//      water_generate              - Creates a batch of animated tga files for the water...

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void water_phong_texture_color_xy(unsigned char* rgb_file, unsigned char x, unsigned char y)
{
    // <ZZ> This function gets a color from an RGB file...  File must be 32x32...
    unsigned int offset;

    // Adjust the coordinates to account for 32x32 tile...
    y&=31;
    x&=31;
    offset = y;
    offset<<=5;
    offset+=x;
    offset*=3;
    rgb_file+=offset+10;

    // Find the color
    color_temp[0] = rgb_file[0];
    color_temp[1] = rgb_file[1];
    color_temp[2] = rgb_file[2];
}
float water_generator_distance(float* first_xy, float* second_xy)
{
    // <ZZ> Helper to find distance between a generator and a point, with wrap around edges...
    float best_distance;
    float distance;
    float vector_xyz[3];
    float offset_xy[2];

    vector_xyz[Z] = 0.0f;


    vector_xyz[X] = first_xy[X] - second_xy[X];
    vector_xyz[Y] = first_xy[Y] - second_xy[Y];
    best_distance = vector_length(vector_xyz);

    offset_xy[X] = 0.0f;  offset_xy[Y] = 0.0f;
    if(second_xy[X] > first_xy[X]) { offset_xy[X] = -1.0f; } else { offset_xy[X] = 1.0f; }
    vector_xyz[X] = first_xy[X] - (second_xy[X] + offset_xy[X]);
    vector_xyz[Y] = first_xy[Y] - (second_xy[Y] + offset_xy[Y]);
    distance = vector_length(vector_xyz);
    if(distance < best_distance) best_distance = distance;

    offset_xy[X] = 0.0f;  offset_xy[Y] = 0.0f;
    if(second_xy[Y] > first_xy[Y]) { offset_xy[Y] = -1.0f; } else { offset_xy[Y] = 1.0f; }
    vector_xyz[X] = first_xy[X] - (second_xy[X] + offset_xy[X]);
    vector_xyz[Y] = first_xy[Y] - (second_xy[Y] + offset_xy[Y]);
    distance = vector_length(vector_xyz);
    if(distance < best_distance) best_distance = distance;

    offset_xy[X] = 0.0f;  offset_xy[Y] = 0.0f;
    if(second_xy[X] > first_xy[X]) { offset_xy[X] = -1.0f; } else { offset_xy[X] = 1.0f; }
    if(second_xy[Y] > first_xy[Y]) { offset_xy[Y] = -1.0f; } else { offset_xy[Y] = 1.0f; }
    vector_xyz[X] = first_xy[X] - (second_xy[X] + offset_xy[X]);
    vector_xyz[Y] = first_xy[Y] - (second_xy[Y] + offset_xy[Y]);
    distance = vector_length(vector_xyz);
    if(distance < best_distance) best_distance = distance;
    return best_distance;
}
#define WATER_GENERATE_SIZE 64
#define WATER_GENERATORS 256
void water_generate(unsigned short number_of_frames)
{
    // <ZZ> This function exports a bunch of .TGA files for helping with the water animation...
    unsigned char filename[20];
    unsigned short frame, i, x, y;
    unsigned char value;
    unsigned char image_data[WATER_GENERATE_SIZE*WATER_GENERATE_SIZE*3];
    float height_data[WATER_GENERATE_SIZE][WATER_GENERATE_SIZE];
    float generator_xy[WATER_GENERATORS][2];
    float generator_radius[WATER_GENERATORS];
    float generator_amplitude[WATER_GENERATORS];
    float generator_current_amplitude[WATER_GENERATORS];
    unsigned short generator_frame[WATER_GENERATORS];
    float min, max;
    float point_xy[2];
    float height;
    float distance;
    float normal_xy[2];
    unsigned char phong_xy[2];
    unsigned char* write;
    unsigned char* phong_file;

    if(number_of_frames < 4)
    {
        log_message("ERROR:  Water generate may not work well with low number of frames...");
    }
    phong_file = sdf_find_filetype("STEEL", SDF_FILE_IS_RGB);
    if(phong_file)
    {
        phong_file = (unsigned char*) sdf_read_unsigned_int(phong_file);


        // Setup water generators...
        log_message("INFO:   Generating water animation...");
        repeat(i, WATER_GENERATORS)
        {
            generator_xy[i][X] = (rand()&255) / 256.0f;
            generator_xy[i][Y] = (rand()&255) / 256.0f;
            generator_radius[i] = (((rand()&127) / 256.0f) + 0.5f) * 0.10f;
            generator_amplitude[i] = (((rand()&127) / 256.0f) + 0.5f);
            generator_frame[i] = rand() % number_of_frames;
        }

        // Create the animation...
        min = -2.0f;
        max = 2.0f;
        repeat(frame, number_of_frames)
        {
            sprintf(filename, "MPWAWA%02d.TGA", frame);
            log_message("INFO:     Working on %s", filename);

            // Find current amplitudes...
            repeat(i, WATER_GENERATORS)
            {
                generator_current_amplitude[i] = generator_amplitude[i] * ((float) sin(((generator_frame[i]+frame)%number_of_frames)*6.28f/number_of_frames));
            }

            // Generate each height value...
            repeat(y, WATER_GENERATE_SIZE)
            {
                repeat(x, WATER_GENERATE_SIZE)
                {
                    point_xy[X] = ((float) x)/((float) WATER_GENERATE_SIZE);
                    point_xy[Y] = ((float) y)/((float) WATER_GENERATE_SIZE);
                    height = 0.0f;
                    repeat(i, WATER_GENERATORS)
                    {
                        distance = water_generator_distance(point_xy, generator_xy[i]);
                        if(distance < generator_radius[i])
                        {
                            distance = distance * 3.1415f / generator_radius[i];
                            height += ((((float) cos(distance)) + 1.0f) * 0.5f) * generator_current_amplitude[i];
                        }
                    }
                    height_data[y][x] = height;
                    if(height_data[y][x] < min) { height_data[y][x] = min; }
                    if(height_data[y][x] > max) { height_data[y][x] = max; }
                    height_data[y][x] -= min;
                    height_data[y][x] /= (max-min);
                }
            }


            // Convert the height data into image data...
            write = image_data;
            repeat(y, WATER_GENERATE_SIZE)
            {
                repeat(x, WATER_GENERATE_SIZE)
                {
                    value = (unsigned char) (height_data[y][x] * 255.0f);
                    normal_xy[X] = height_data[y][(x+1)&(WATER_GENERATE_SIZE-1)] - height_data[y][x];
                    normal_xy[Y] = height_data[(y+1)&(WATER_GENERATE_SIZE-1)][x] - height_data[y][x];
                    phong_xy[X] = (unsigned char) (16 + (normal_xy[X]*80.0f));
                    phong_xy[Y] = (unsigned char) (16 + (normal_xy[Y]*80.0f));
                    water_phong_texture_color_xy(phong_file, phong_xy[X], phong_xy[Y]);
                    *write = (color_temp[0] + value)>>1;  write++;
                    *write = (color_temp[1] + value)>>1;  write++;
                    *write = (color_temp[2] + value)>>1;  write++;
                }
            }


            // Export the image...
            display_export_tga(filename, image_data, WATER_GENERATE_SIZE, WATER_GENERATE_SIZE, 3);
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#define MAX_WATER_FRAME 32
unsigned int texture_water[MAX_WATER_FRAME];
unsigned int texture_shimmer[MAX_WATER_FRAME];
unsigned int texture_sand;

//-----------------------------------------------------------------------------------------------
void water_setup(void)
{
    // <ZZ> This function clears out the water texture list...
    unsigned short i;
    repeat(i, MAX_WATER_FRAME)
    {
        texture_water[i] = 0;
        texture_shimmer[i] = 0;
    }
    texture_sand = 0;
}

//-----------------------------------------------------------------------------------------------
unsigned char water_layers_active = TRUE;
void water_draw_room(unsigned char* data)
{
    // <ZZ> This function draws the water triangles of an uncompressed room...  Water triangles
    //      are texture layers 30 & 31...
    unsigned char* vertex_data;
    unsigned char* triangle_data;
    unsigned char* water_triangle_data_start;
    unsigned char* tex_vertex_data;
    unsigned char texture;
    unsigned short num_vertex, num_strip;
    unsigned short i, j, vertex;
    unsigned char alpha, temp;
    float* current_vertex_data;
    float* current_tex_vertex_data;
    float* character_xyz;
    float vertex_xyz[3];
    float tex_vertex_xy[2];
    float tex_scale, offx, offy, angle;
    float z;


    // Error check...
    if(room_water_type > WATER_TYPE_SAND)
    {
        return;
    }



    // Read the header...
    vertex_data = data + (*((unsigned int*) (data+SRF_VERTEX_OFFSET)));
    num_vertex = *((unsigned short*) vertex_data);  vertex_data+=2;
    triangle_data = data + (*((unsigned int*) (data+SRF_TRIANGLE_OFFSET)));
    tex_vertex_data = data + (*((unsigned int*) (data+SRF_TEX_VERTEX_OFFSET)));
    tex_vertex_data+=2;


    // Setup some basic display stuff...
    display_texture_on();
    if(room_water_type == WATER_TYPE_WATER)
    {
        // Water is partially transparent...
        display_zbuffer_write_off();

        // If any local player is below the water level, we need to make it more transparent...
        alpha = 170;

        repeat(i, MAX_LOCAL_PLAYER)
        {
            if(player_device_type[i] && local_player_character[i] < MAX_CHARACTER)
            {
                character_xyz = (float*) main_character_data[local_player_character[i]];
                if(character_xyz[Z] < (room_water_level-4.0f))
                {
                    z = room_water_level - 4.0f - character_xyz[Z];
                    if(z > 5.0f)
                    {
                        alpha = 70;
                        i = MAX_LOCAL_PLAYER;
                    }
                    else
                    {
                        temp = (unsigned char) 170 - ((unsigned char) (z*20.0f));
                        alpha = (temp < alpha) ? temp : alpha;
                    }
                }
            }
        }
    }
    else
    {
        // Sand and lava are solid...
        display_zbuffer_write_on();
        display_blend_off();
        if(room_water_type == WATER_TYPE_SAND)
        {
            display_pick_texture(texture_sand);
            display_color(white);
            offx = 0.0f;
            offy = 0.0f;
        }
    }


    // Skip the room triangle strips (for walls and stuff)...
    repeat(texture, (MAX_ROOM_TEXTURE-2))
    {
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;
        repeat(i, num_strip)
        {
            num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
            triangle_data += (num_vertex*4);
        }
    }



    if(water_layers_active)
    {
        // Draw the triangle strips for the water side walls...
        water_triangle_data_start = triangle_data;
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;


        tex_scale = 0.07f;
        if(room_water_type == WATER_TYPE_WATER)
        {
            display_pick_texture(texture_water[((main_game_frame+30)/3)&(MAX_WATER_FRAME-1)]);
            display_blend_trans();
            color_temp[0] = 0;
            color_temp[1] = 0;
            color_temp[2] = 200;
            color_temp[3] = alpha;
            display_color_alpha(color_temp);
            angle = (main_game_frame & 1023) * 6.2832f / 1024.0f;
            offx = ((float) sin(angle)) * 3.0f * tex_scale;
            offy = ((float) cos(angle)) * 3.0f;
        }
        else if(room_water_type == WATER_TYPE_LAVA)
        {
            display_pick_texture(texture_water[((main_game_frame+30)/3)&(MAX_WATER_FRAME-1)]);
            color_temp[0] = 255;
            color_temp[1] = 50;
            color_temp[2] = 0;
            display_color(color_temp);
            angle = (main_game_frame & 1023) * 6.2832f / 1024.0f;
            offx = ((float) sin(angle)) * 3.0f * tex_scale;
            offy = ((float) cos(angle)) * 3.0f;
        }
        else
        {
            tex_scale = 0.11f;
        }



        repeat(i, num_strip)
        {
            num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
            display_start_strip();
                repeat(j, num_vertex)
                {
                    // Read the vertex and tex vertex
                    vertex = (*((unsigned short*) triangle_data));  triangle_data+=2;
                    current_vertex_data = (float*) (vertex_data+(vertex*26));
                    vertex = (*((unsigned short*) triangle_data));  triangle_data+=2;
                    current_tex_vertex_data = (float*) (tex_vertex_data+(vertex*22));
                    vertex_xyz[X] = current_vertex_data[X];
                    vertex_xyz[Y] = current_vertex_data[Y];
                    vertex_xyz[Z] = (current_vertex_data[Z] > -0.25f) ? room_water_level : current_vertex_data[Z];
                    tex_vertex_xy[X] = current_tex_vertex_data[X] + offx;
                    tex_vertex_xy[Y] = (vertex_xyz[Z] + offy) * tex_scale;


                    // Add to draw list...
                    display_texpos(tex_vertex_xy);
                    display_vertex(vertex_xyz);
                }
            display_end();
        }
    }
    else
    {
        // Skip over water side walls...
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;
        repeat(i, num_strip)
        {
            num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
            triangle_data += (num_vertex*4);
        }
    }










    // Draw the triangle strips for the water top surface...
if(!key_down[SDLK_F5])
{
    water_triangle_data_start = triangle_data;
    num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;


    tex_scale = 0.07f;
    vertex_xyz[Z] = room_water_level;
    if(room_water_type == WATER_TYPE_WATER)
    {
        display_pick_texture(texture_water[(main_game_frame>>2)&(MAX_WATER_FRAME-1)]);
        display_blend_trans();
        color_temp[0] = 0;
        color_temp[1] = 0;
        color_temp[2] = 250;
        color_temp[3] = alpha;
        display_color_alpha(color_temp);
        angle = (main_game_frame & 1023) * 6.2832f / 1024.0f;
        offx = ((float) sin(angle)) * 3.0f;
        offy = ((float) cos(angle)) * 3.0f;
    }
    else if(room_water_type == WATER_TYPE_LAVA)
    {
        display_pick_texture(texture_water[(main_game_frame>>2)&(MAX_WATER_FRAME-1)]);
        color_temp[0] = 255;
        color_temp[1] = 50;
        color_temp[2] = 0;
        display_color(color_temp);
        angle = (main_game_frame & 1023) * 6.2832f / 1024.0f;
        offx = ((float) sin(angle)) * 3.0f;
        offy = ((float) cos(angle)) * 3.0f;
    }
    else
    {
        tex_scale = 0.11f;
    }




    repeat(i, num_strip)
    {
        num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
        display_start_strip();
            repeat(j, num_vertex)
            {
                // Read the vertex and tex vertex
                vertex = (*((unsigned short*) triangle_data));  triangle_data+=4;
                current_vertex_data = (float*) (vertex_data+(vertex*26));
                vertex_xyz[X] = current_vertex_data[X];
                vertex_xyz[Y] = current_vertex_data[Y];
                tex_vertex_xy[X] = (vertex_xyz[X] + offx) * tex_scale;
                tex_vertex_xy[Y] = (vertex_xyz[Y] + offy) * tex_scale;


                // Add to draw list...
                display_texpos(tex_vertex_xy);
                display_vertex(vertex_xyz);
            }
        display_end();
    }
}




if(!key_down[SDLK_F6])
{
    // Draw upper layer of the water (shimmer)...
    if(water_layers_active && room_water_type != WATER_TYPE_SAND)
    {
        triangle_data = water_triangle_data_start;
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;




        tex_scale = 0.10f;
        angle = ((main_game_frame & 32767) % 1638) * -6.2832f / 1638.0f;
        offx = ((float) sin(angle)) * 4.0f;
        offy = ((float) cos(angle)) * 4.0f;
        offx += (camera_fore_xyz[X]*0.5f);
        offy += (camera_fore_xyz[Y]*0.5f);
        vertex_xyz[Z] = room_water_level;
        if(room_water_type == WATER_TYPE_WATER)
        {
            display_pick_texture(texture_shimmer[((main_game_frame+30)/3)&(MAX_WATER_FRAME-1)]);
            display_blend_light();
            color_temp[0] = 255;
            color_temp[1] = 255;
            color_temp[2] = 255;
            color_temp[3] = (alpha*3)>>2;
            display_color_alpha(color_temp);
        }
        else
        {
            display_pick_texture(texture_shimmer[((main_game_frame+30)/3)&(MAX_WATER_FRAME-1)]);
            display_zbuffer_write_off();
            display_blend_light();

            color_temp[0] = 255;
            color_temp[1] = 210;
            color_temp[2] = 0;
            color_temp[3] = 255;
            display_color_alpha(color_temp);
        }



        repeat(i, num_strip)
        {
            num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
            display_start_strip();
                repeat(j, num_vertex)
                {
                    // Read the vertex and tex vertex
                    vertex = (*((unsigned short*) triangle_data));  triangle_data+=4;
                    current_vertex_data = (float*) (vertex_data+(vertex*26));
                    vertex_xyz[X] = current_vertex_data[X];
                    vertex_xyz[Y] = current_vertex_data[Y];
                    tex_vertex_xy[X] = (vertex_xyz[Y] + offx) * tex_scale;
                    tex_vertex_xy[Y] = (vertex_xyz[X] + offy) * tex_scale;


                    // Add to draw list...
                    display_texpos(tex_vertex_xy);
                    display_vertex(vertex_xyz);
                }
            display_end();
        }
    }
}




    display_zbuffer_write_on();
}

//-----------------------------------------------------------------------------------------------
unsigned char drown_delay[256];
void water_drown_delay_setup()
{
    // <ZZ> Sets up an array that tells us how long to delay between hurting character when
    //      stepping in lava, based on DefFire, or drowning...  Should be called whenever a
    //      new room is enter'd...
    unsigned short i;
    signed char deffire;

    if(room_water_type == WATER_TYPE_LAVA)
    {
        deffire = 0;
        repeat(i, 256)
        {
            if(deffire < -2)  { drown_delay[((unsigned char) deffire)] = 3;  }
            if(deffire == -2) { drown_delay[((unsigned char) deffire)] = 7; }
            if(deffire == -1) { drown_delay[((unsigned char) deffire)] = 15; }
            if(deffire == 0)  { drown_delay[((unsigned char) deffire)] = 30; }
            if(deffire == 1)  { drown_delay[((unsigned char) deffire)] = 60; }
            if(deffire == 2)  { drown_delay[((unsigned char) deffire)] = 120; }
            if(deffire > 2)   { drown_delay[((unsigned char) deffire)] = 240; }
            deffire++;
        }
    }
    else
    {
        repeat(i, 256)
        {
            drown_delay[i] = 255;
        }
    }
}

//-----------------------------------------------------------------------------------------------
