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

// <ZZ> This file has all the stuff for drawing volumetric shadows...

//-----------------------------------------------------------------------------------------------
// The shadow vector tells us how much to offset the shadows for every foot dropped (x,y,z)
// (value at [Z] should always be -1.0f) and also tells us the z value ([3]) at which to
// cutoff the shadows...
//float global_volumetric_shadow_vector_xyzz[4] = {0.0f, 1.0f, -1.0f, -20.0f};
float global_volumetric_shadow_vector_xyzz[4] = {0.490f, -0.513f, -1.0f, -20.0f};
// !!!BAD!!!
// !!!BAD!!!  Should macroize this...
// !!!BAD!!!
void volume_shadow_edge(float* start_xyz, float* end_xyz)
{
    // <ZZ> This function draws a volumetric shadow quad for a triangle's edge...
    //      Should be called twice, once for front face and once for back face...
    float start_projected_xyz[3];
    float end_projected_xyz[3];
    float distance;

    // Calculate the projected xyz locations...
    distance = start_xyz[Z]-global_volumetric_shadow_vector_xyzz[3];
    start_projected_xyz[X] = start_xyz[X] + global_volumetric_shadow_vector_xyzz[X]*distance;
    start_projected_xyz[Y] = start_xyz[Y] + global_volumetric_shadow_vector_xyzz[Y]*distance;
    start_projected_xyz[Z] = global_volumetric_shadow_vector_xyzz[3];

    distance = end_xyz[Z]-global_volumetric_shadow_vector_xyzz[3];
    end_projected_xyz[X] = end_xyz[X] + global_volumetric_shadow_vector_xyzz[X]*distance;
    end_projected_xyz[Y] = end_xyz[Y] + global_volumetric_shadow_vector_xyzz[Y]*distance;
    end_projected_xyz[Z] = global_volumetric_shadow_vector_xyzz[3];

    // Now draw the quad...  Assume that display stuff has already been set...
    display_start_fan();
        display_vertex(start_xyz);
        display_vertex(end_xyz);
        display_vertex(end_projected_xyz);
        display_vertex(start_projected_xyz);
    display_end();
}

//-----------------------------------------------------------------------------------------------
void volume_rdy_find_temp_stuff(unsigned char* data, unsigned short frame)
{
    // <ZZ> Sets up some stuff that's needed for attaching weapons...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* bone_data;
    unsigned char base_model, detail_level;
    unsigned short i;


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) frame = 0;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    detail_level = global_rdy_detail_level;


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);
    num_bone = *((unsigned short*) (base_model_data+6));



    repeat(i, 8)
    {
        temp_character_bone_number[i] = 255;
    }
    repeat(i, num_bone)
    {
        temp_character_bone_number[(*(bone_data+(i*9))) & 7] = (unsigned char) i;
    }
    temp_character_frame_event = frame_data[1];
}

//-----------------------------------------------------------------------------------------------
void volume_rdy_character_shadow(unsigned char* data, unsigned short frame, unsigned char* bone_frame_data)
{
    // <ZZ> This function draws a volumetric shadow for an RDY file (character)...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* vertex_data;
    unsigned char* bone_data;
    unsigned char* frame_data;
    unsigned char* edge_line_data;
    unsigned char* temp_data;
    unsigned short num_edge_line;
    unsigned short i;
    unsigned char base_model, detail_level;
    unsigned int value;
    float a_xyz[3];
    float b_xyz[3];
    float c_xyz[3];
    float d_xyz[3];
    float* temp_xyz;
    float edge_xyz[3];
    float vector_xyz[3];
    float plane_xyz[3];
    float front_dot;
    float back_dot;
    float cartoon_line_size;


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) frame = 0;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);



    cartoon_line_size = 0.0875f;
    detail_level = global_rdy_detail_level;



    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);
    vertex_data = base_model_data;
    num_vertex = *((unsigned short*) vertex_data); vertex_data+=6;
    num_bone = *((unsigned short*) vertex_data); vertex_data+=2;


    // Generate vertex coordinates based on bone locations...
    if(bone_frame_data)
    {
        render_bone_frame(base_model_data, bone_data, bone_frame_data);
    }



    // Edge line data start...
    edge_line_data = bone_data + (num_bone*9);
    num_edge_line = *((unsigned short*) edge_line_data);  edge_line_data+=2;


    // Clear out visibility count for each vertex...
    // Get onscreen coordinates of each vertex...  Stored in fourthbuffer...
    if(num_edge_line > 0)
    {
        // Determine visibility of each edge line...
        temp_data = edge_line_data;
        repeat(i, num_edge_line)
        {
            // 4 vertices of cartoon line define two adjacent triangles with a shared edge...
            value = (*((unsigned short*) temp_data));  temp_data++;  temp_data++;  temp_xyz = (float*) (vertex_data + (value<<6));  a_xyz[X] = temp_xyz[X];  a_xyz[Y] = temp_xyz[Y];  a_xyz[Z] = temp_xyz[Z];
                    temp_xyz = (float*) (vertex_data + (value<<6) + 15);  a_xyz[X] += temp_xyz[X]*cartoon_line_size;  a_xyz[Y] += temp_xyz[Y]*cartoon_line_size;  a_xyz[Z] += temp_xyz[Z]*cartoon_line_size;
            value = (*((unsigned short*) temp_data));  temp_data++;  temp_data++;  temp_xyz = (float*) (vertex_data + (value<<6));  b_xyz[X] = temp_xyz[X];  b_xyz[Y] = temp_xyz[Y];  b_xyz[Z] = temp_xyz[Z];
                    temp_xyz = (float*) (vertex_data + (value<<6) + 15);  b_xyz[X] += temp_xyz[X]*cartoon_line_size;  b_xyz[Y] += temp_xyz[Y]*cartoon_line_size;  b_xyz[Z] += temp_xyz[Z]*cartoon_line_size;
            value = (*((unsigned short*) temp_data));  temp_data++;  temp_data++;  temp_xyz = (float*) (vertex_data + (value<<6));  c_xyz[X] = temp_xyz[X];  c_xyz[Y] = temp_xyz[Y];  c_xyz[Z] = temp_xyz[Z];
                    temp_xyz = (float*) (vertex_data + (value<<6) + 15);  c_xyz[X] += temp_xyz[X]*cartoon_line_size;  c_xyz[Y] += temp_xyz[Y]*cartoon_line_size;  c_xyz[Z] += temp_xyz[Z]*cartoon_line_size;
            value = (*((unsigned short*) temp_data));  temp_data++;  temp_data++;  temp_xyz = (float*) (vertex_data + (value<<6));  d_xyz[X] = temp_xyz[X];  d_xyz[Y] = temp_xyz[Y];  d_xyz[Z] = temp_xyz[Z];
                    temp_xyz = (float*) (vertex_data + (value<<6) + 15);  d_xyz[X] += temp_xyz[X]*cartoon_line_size;  d_xyz[Y] += temp_xyz[Y]*cartoon_line_size;  d_xyz[Z] += temp_xyz[Z]*cartoon_line_size;


            // Shared edge and light direction give a plane that splits space in half...  Are check points on same side of plane?
            edge_xyz[X] = b_xyz[X] - a_xyz[X];  edge_xyz[Y] = b_xyz[Y] - a_xyz[Y];  edge_xyz[Z] = b_xyz[Z] - a_xyz[Z];
            cross_product(edge_xyz, global_volumetric_shadow_vector_xyzz, plane_xyz);
            vector_xyz[X] = d_xyz[X] - a_xyz[X];  vector_xyz[Y] = d_xyz[Y] - a_xyz[Y];  vector_xyz[Z] = d_xyz[Z] - a_xyz[Z];
            back_dot = dot_product(vector_xyz, plane_xyz);
            vector_xyz[X] = c_xyz[X] - a_xyz[X];  vector_xyz[Y] = c_xyz[Y] - a_xyz[Y];  vector_xyz[Z] = c_xyz[Z] - a_xyz[Z];
            front_dot = dot_product(vector_xyz, plane_xyz);


            // If both are on the same side, we've got an edge...
            if((front_dot > 0.0f) == (back_dot > 0.0f))
            {
                // Now find surface normal of triangle ABC...  Stored in plane_xyz...
                cross_product(edge_xyz, vector_xyz, plane_xyz);


                // Now dot surface normal with light direction...
                front_dot = dot_product(plane_xyz, global_volumetric_shadow_vector_xyzz);



                if(front_dot > 0.0f)
                {
                    volume_shadow_edge(a_xyz, b_xyz);
                }
                else
                {
                    volume_shadow_edge(b_xyz, a_xyz);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void volume_draw_room_shadows(unsigned char* data)
{
    // <ZZ> This function draws all of the terrain shadows.  Should be called
    //      twice...
    unsigned short i;
    unsigned short num_edge_line;
    unsigned short value, c, d;
    unsigned char* edge_line_data;
    unsigned char* vertex_data;
    float a_xyz[3];
    float b_xyz[3];
    float c_xyz[3];
    float d_xyz[3];
    float edge_xyz[3];
    float vector_xyz[3];
    float plane_xyz[3];
    float* temp_xyz;
    float front_dot;
    float back_dot;



//    float vertex_xyz[4][3];


//    vertex_xyz[0][X] = 0.0f;    vertex_xyz[0][Y] = 30.0f;    vertex_xyz[0][Z] = 20.0f;
//    vertex_xyz[1][X] = 20.0f;   vertex_xyz[1][Y] = 30.0f;    vertex_xyz[1][Z] = 20.0f;
//    vertex_xyz[2][X] = 20.0f;   vertex_xyz[2][Y] = 30.0f;    vertex_xyz[2][Z] = 0.0f;
//    vertex_xyz[3][X] = 0.0f;    vertex_xyz[3][Y] = 30.0f;    vertex_xyz[3][Z] = 0.0f;


//    volume_shadow_edge(vertex_xyz[0], vertex_xyz[1]);
//    volume_shadow_edge(vertex_xyz[1], vertex_xyz[2]);
//    volume_shadow_edge(vertex_xyz[2], vertex_xyz[3]);
//    volume_shadow_edge(vertex_xyz[3], vertex_xyz[0]);


//    display_start_fan();
//        display_vertex(vertex_xyz[0]);
//        display_vertex(vertex_xyz[3]);
//        display_vertex(vertex_xyz[2]);
//        display_vertex(vertex_xyz[1]);
//    display_end();





    // Edge line data start...
    vertex_data = data + (*((unsigned int*) (data+SRF_VERTEX_OFFSET)));
    edge_line_data = data + (*((unsigned int*) (data+SRF_EDGE_LINE_OFFSET)));
    num_edge_line = *((unsigned short*) edge_line_data);  edge_line_data+=2;
    vertex_data+=2;


    // Determine visibility of each edge line...
    repeat(i, num_edge_line)
    {
        // 4 vertices of cartoon line define two adjacent triangles with a shared edge...
        value = (*((unsigned short*) edge_line_data));  edge_line_data+=2;  temp_xyz = (float*) (vertex_data + (value*26));  a_xyz[X] = temp_xyz[X];  a_xyz[Y] = temp_xyz[Y];  a_xyz[Z] = temp_xyz[Z];
        value = (*((unsigned short*) edge_line_data));  edge_line_data+=2;  temp_xyz = (float*) (vertex_data + (value*26));  b_xyz[X] = temp_xyz[X];  b_xyz[Y] = temp_xyz[Y];  b_xyz[Z] = temp_xyz[Z];
        value = (*((unsigned short*) edge_line_data));  edge_line_data+=2;  temp_xyz = (float*) (vertex_data + (value*26));  c_xyz[X] = temp_xyz[X];  c_xyz[Y] = temp_xyz[Y];  c_xyz[Z] = temp_xyz[Z];  c = value;
        value = (*((unsigned short*) edge_line_data));  edge_line_data+=2;  temp_xyz = (float*) (vertex_data + (value*26));  d_xyz[X] = temp_xyz[X];  d_xyz[Y] = temp_xyz[Y];  d_xyz[Z] = temp_xyz[Z];  d = value;


        if(c != d)
        {
            // Shared edge and light direction give a plane that splits space in half...  Are check points on same side of plane?
            edge_xyz[X] = b_xyz[X] - a_xyz[X];  edge_xyz[Y] = b_xyz[Y] - a_xyz[Y];  edge_xyz[Z] = b_xyz[Z] - a_xyz[Z];
            cross_product(edge_xyz, global_volumetric_shadow_vector_xyzz, plane_xyz);
            vector_xyz[X] = d_xyz[X] - a_xyz[X];  vector_xyz[Y] = d_xyz[Y] - a_xyz[Y];  vector_xyz[Z] = d_xyz[Z] - a_xyz[Z];
            back_dot = dot_product(vector_xyz, plane_xyz);
            vector_xyz[X] = c_xyz[X] - a_xyz[X];  vector_xyz[Y] = c_xyz[Y] - a_xyz[Y];  vector_xyz[Z] = c_xyz[Z] - a_xyz[Z];
            front_dot = dot_product(vector_xyz, plane_xyz);


            // If both are on the same side, we've got an edge...
            if((front_dot > 0.0f) == (back_dot > 0.0f))
            {
                // Now find surface normal of triangle ABC...  Stored in plane_xyz...
                cross_product(edge_xyz, vector_xyz, plane_xyz);


                // Now dot surface normal with light direction...
                front_dot = dot_product(plane_xyz, global_volumetric_shadow_vector_xyzz);



                if(front_dot > 0.0f)
                {
                    volume_shadow_edge(a_xyz, b_xyz);
                }
                else
                {
                    volume_shadow_edge(b_xyz, a_xyz);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
// Stuff for finding screen corners
#define CORNER_FORE_SCALE (-ZNEAR)
#define CORNER_LEFT_SCALE ((virtual_x / virtual_y)*1.26f*ZNEAR)
#define CORNER_UP_SCALE (1.26f*ZNEAR)
void volume_shadow_draw_all()
{
    // <ZZ> This function draws all of the character's shadows (volumetric)...
    unsigned short i, j, corner, frame;
    unsigned char* model_data;
    float screen_corner_xyz[4][3];
    float old_camera_xyz[3];




    if(fast_and_ugly_active)
    {
        // No shadows in fast and ugly mode...
        return;
    }



    // Setup some basic display stuff...
    display_paperdoll_off();
    display_shade_off();
    display_texture_off();
    display_zbuffer_on();
    display_zbuffer_write_off();
    display_draw_off();
    display_blend_off();
    display_stencilbuffer_on();




    // Back faces should be invisible and increment stencil buffer values...
    display_stencilbuffer_back();
    display_cull_frontface();
    // For each character...
    repeat(i, MAX_CHARACTER)
    {
        // Is character on?
        if(main_character_on[i])
        {
            // Check character's alpha
            if((main_character_data[i][79] > 128) && !(main_character_data[i][104] & MORE_FLAG_NOSHADOW))
            {
                // Draw this character's shadow
                frame = *((unsigned short*) (main_character_data[i]+178));
                model_data = (main_character_data[i]+256);
                volume_rdy_find_temp_stuff(*((unsigned char**) model_data), frame);

                // Main model
                volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);

                // Non-animated overlapped parts...  Arms, Head, etc...
                model_data+=24;
                frame = 0;
                repeat(j, 7)
                {
                    if(*((unsigned char**) model_data) != NULL)
                    {
                        volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                    }
                    model_data+=24;
                }


                // Draw the animated eyes...
                frame = *((unsigned short*) (main_character_data[i] + 182));
                if(*((unsigned char**) model_data) != NULL)
                {
                    volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                }
                model_data+=24;


                // Draw the animated mouth...
                frame = *((unsigned short*) (main_character_data[i] + 184));
                if(*((unsigned char**) model_data) != NULL)
                {
                    volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                }
                model_data+=24;


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
                        switch(j&1)
                        {
                            case 0:
                                frame = (temp_character_frame_event & FRAME_EVENT_LEFT)?2:(temp_character_frame_event >> 7);
                                break;
                            case 1:
                                frame = (temp_character_frame_event & FRAME_EVENT_RIGHT)?2:(temp_character_frame_event >> 7);
                                break;
                        }
                        script_matrix_good_bone(temp_character_bone_number[j+1], temp_character_bone_frame[i], main_character_data[i]);
                        render_generate_model_world_data(*((unsigned char**) model_data), frame, script_matrix, fourthbuffer);  // Generate new bone frame in fourthbuffer
                        volume_rdy_character_shadow(*((unsigned char**) model_data), frame, fourthbuffer);
                    }
                    model_data+=24;
                }
            }
        }
    }




    // Now draw the backfaces of the room shadows...
    volume_draw_room_shadows(roombuffer);



    // Front faces should be invisible and decrement the stencil buffer values...
    display_stencilbuffer_front();
    display_cull_on();
    // For each character...
    repeat(i, MAX_CHARACTER)
    {
        // Is character on?
        if(main_character_on[i])
        {
            // Check character's alpha
            if((main_character_data[i][79] > 128) && !(main_character_data[i][104] & MORE_FLAG_NOSHADOW))
            {
                // Draw this character's shadow
                frame = *((unsigned short*) (main_character_data[i]+178));
                model_data = (main_character_data[i]+256);
                volume_rdy_find_temp_stuff(*((unsigned char**) model_data), frame);

                // Main model
                volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);

                // Non-animated overlapped parts...  Arms, Head, etc...
                model_data+=24;
                frame = 0;
                repeat(j, 7)
                {
                    if(*((unsigned char**) model_data) != NULL)
                    {
                        volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                    }
                    model_data+=24;
                }


                // Draw the animated eyes...
                frame = *((unsigned short*) (main_character_data[i] + 182));
                if(*((unsigned char**) model_data) != NULL)
                {
                    volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                }
                model_data+=24;


                // Draw the animated mouth...
                frame = *((unsigned short*) (main_character_data[i] + 184));
                if(*((unsigned char**) model_data) != NULL)
                {
                    volume_rdy_character_shadow(*((unsigned char**) model_data), frame, temp_character_bone_frame[i]);
                }
                model_data+=24;


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
                        switch(j&1)
                        {
                            case 0:
                                frame = (temp_character_frame_event & FRAME_EVENT_LEFT)?2:(temp_character_frame_event >> 7);
                                break;
                            case 1:
                                frame = (temp_character_frame_event & FRAME_EVENT_RIGHT)?2:(temp_character_frame_event >> 7);
                                break;
                        }
                        script_matrix_good_bone(temp_character_bone_number[j+1], temp_character_bone_frame[i], main_character_data[i]);
                        render_generate_model_world_data(*((unsigned char**) model_data), frame, script_matrix, fourthbuffer);  // Generate new bone frame in fourthbuffer
                        volume_rdy_character_shadow(*((unsigned char**) model_data), frame, fourthbuffer);
                    }
                    model_data+=24;
                }
            }
        }
    }




    // Now draw the frontfaces of the room shadows...
    volume_draw_room_shadows(roombuffer);



    // Find screen corners...  Scooch the camera back a bit, so I can find the corners by goin' forward...
    old_camera_xyz[X] = camera_xyz[X];  camera_xyz[X]-=rotate_camera_matrix[1] * CORNER_FORE_SCALE;
    old_camera_xyz[Y] = camera_xyz[Y];  camera_xyz[Y]-=rotate_camera_matrix[5] * CORNER_FORE_SCALE;
    old_camera_xyz[Z] = camera_xyz[Z];  camera_xyz[Z]-=rotate_camera_matrix[9] * CORNER_FORE_SCALE;


    // Top corners...
    corner = 0;
    repeat(i, 2)
    {
        screen_corner_xyz[corner][X] = camera_xyz[X]
                                  + rotate_camera_matrix[0] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[1] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[2] * CORNER_UP_SCALE;
        screen_corner_xyz[corner][Y] = camera_xyz[Y]
                                  + rotate_camera_matrix[4] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[5] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[6] * CORNER_UP_SCALE;
        screen_corner_xyz[corner][Z] = camera_xyz[Z]
                                  + rotate_camera_matrix[8] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[9] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[10] * CORNER_UP_SCALE;
        corner++;
    }
    // Bottom corners...
    corner = 3;
    repeat(i, 2)
    {
        screen_corner_xyz[corner][X] = camera_xyz[X]
                                  + rotate_camera_matrix[0] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[1] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[2] * -CORNER_UP_SCALE;
        screen_corner_xyz[corner][Y] = camera_xyz[Y]
                                  + rotate_camera_matrix[4] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[5] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[6] * -CORNER_UP_SCALE;
        screen_corner_xyz[corner][Z] = camera_xyz[Z]
                                  + rotate_camera_matrix[8] * CORNER_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[9] * CORNER_FORE_SCALE
                                  + rotate_camera_matrix[10] * -CORNER_UP_SCALE;
        corner--;
    }





    // Switch our settings again...
    display_draw_on();
    display_zbuffer_off();
    display_blend_trans();





    // Now draw the shadow as one big rectangle across screen...  Only affect areas where stencil buffer is 0 (normally, sometimes more if camera is inside a shadow)...
    display_stencilbuffer_shadow(8);
    color_temp[0] = 0;    color_temp[1] = 0;    color_temp[2] = 0;    color_temp[3] = 128; //82;
    display_color_alpha(color_temp);
    display_start_fan();
        display_vertex(screen_corner_xyz[0]);
        display_vertex(screen_corner_xyz[1]);
        display_vertex(screen_corner_xyz[2]);
        display_vertex(screen_corner_xyz[3]);
    display_end();



    // Restore the camera to where it was...
    camera_xyz[X] = old_camera_xyz[X];
    camera_xyz[Y] = old_camera_xyz[Y];
    camera_xyz[Z] = old_camera_xyz[Z];


    // Undo display settings...
    display_texture_on();
    display_zbuffer_on();
    display_stencilbuffer_off();



    display_color(red);
    display_start_line_loop();
        display_vertex(screen_corner_xyz[0]);
        display_vertex(screen_corner_xyz[1]);
        display_vertex(screen_corner_xyz[2]);
        display_vertex(screen_corner_xyz[3]);
    display_end();
}

//-----------------------------------------------------------------------------------------------

//unsigned char screen_image[640*480*4];
//unsigned int i;
//if(key_pressed[SDLK_F5])
//{
//    glReadPixels(0, 0, 640, 480, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, screen_image);
//    repeat(i, (640*480))
//    {
//        screen_image[i] = (screen_image[i]&3)*80;
//    }
//    display_export_tga("THIRD.TGA", screen_image, 640, 480, 1);
//}
//float value;
//if(key_pressed[SDLK_F5])
//{
//    glReadPixels(0, 0, 640, 480, GL_DEPTH_COMPONENT, GL_FLOAT, screen_image);
//    repeat(i, (640*480))
//    {
//        value = ((float*) screen_image)[i];
//        value = (value-0.5f)*2.0f;
//        value*=value;
//        value*=value;
//        value*=value;
//        value*=value;
//        value*=value;
//        value*=value;
//        screen_image[i] = (unsigned char) (255.0f*value);
//    }
//    display_export_tga("FIRST.TGA", screen_image, 640, 480, 1);
//}
//if(key_pressed[SDLK_F5])
//{
//    glReadPixels(0, 0, 640, 480, GL_RGB, GL_UNSIGNED_BYTE, screen_image);
//    display_export_tga("SECOND.TGA", screen_image, 640, 480, 3);
//    sprintf(DEBUG_STRING, "Export %d", main_video_frame);
//}
//if(key_pressed[SDLK_F5])
//{
//    glReadPixels(0, 0, 640, 480, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, screen_image);
//    repeat(i, (640*480))
//    {
//        screen_image[i] = (screen_image[i]&3)*80;
//    }
//    display_export_tga("THIRD.TGA", screen_image, 640, 480, 1);
//}


//-----------------------------------------------------------------------------------------------
