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

// <ZZ> This file contains functions to convert DDD files to RDY
//      decode_ddd              - The main function to do a DDD conversion

#define NO_LINE_FLAG  64

//-----------------------------------------------------------------------------------------------
// Variables for seeing how well/poorly the fanning/stripping is working/not working
unsigned char* global_base_old_vertex_start;
unsigned char* global_base_old_tex_vertex_start;
unsigned char* global_base_old_texture_start;
unsigned char* global_base_old_joint_start;
unsigned short global_base_num_vertex; 
unsigned short global_base_num_tex_vertex;
unsigned short global_base_num_triangle;
unsigned int global_base_data_size;

//-----------------------------------------------------------------------------------------------
void ddd_generate_model_action_list(unsigned char* data)
{
    // <ZZ> This function generates an action list for a given RDY model.  Action list is a
    //      list of the first frame for each defined action, or FFFF if action unnavailable.
    unsigned char* frame_data;
    unsigned char** frame_data_start;
    unsigned short* action_list;
    unsigned short num_bone_frame;
    unsigned short i, j;
    unsigned char num_base_model;


    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data);  data+=2;
    action_list = (unsigned short*) data;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data_start =  (unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX));


    // Go through each action...
    repeat(i, ACTION_MAX)
    {
        // Go through each frame looking for first instance...
        action_list[i] = 65535;
        repeat(j, num_bone_frame)
        {
            frame_data = frame_data_start[j];
            if(*frame_data == i)
            {
                action_list[i] = j;
                j = num_bone_frame;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void ddd_remove_data(unsigned char* data_block_start, unsigned int data_size, unsigned char* data_to_remove, unsigned short bytes_to_remove)
{
    // <ZZ> This function removes several bytes from a block of data, and scooches everything after
    //      back to fit...
    signed int bytes_to_move;
    bytes_to_move = ((unsigned int) data_block_start)+data_size-((unsigned int) data_to_remove)-bytes_to_remove;
    if(bytes_to_move > 0)
    {
        memmove(data_to_remove, data_to_remove+bytes_to_remove, bytes_to_move);
    }
    memset(data_block_start+data_size-bytes_to_remove, 0, bytes_to_remove);
}

//-----------------------------------------------------------------------------------------------
void ddd_simplify_two_vertices(unsigned short* num_vertex_spot, unsigned short* num_tex_vertex_spot, unsigned short vertex_to_keep, unsigned short vertex_to_remove, unsigned short tex_vertex_to_keep, unsigned short tex_vertex_to_remove, unsigned char* data, unsigned int data_size, unsigned char num_texture)
{
    // <ZZ> This combines two vertices into one, and deletes any associated triangle.
    unsigned short temp;
    unsigned char* triangle_data;
    unsigned char* strip_data_start;
    float* first_xyz;
    float* second_xyz;
    unsigned short i, j, k;
    unsigned short num_triangle;
    unsigned short matches;



    // Figure out which one to keep...
    if(vertex_to_keep > vertex_to_remove)
    {
        // Swap 'em, so the lower indexed vertex is kept...
        temp = vertex_to_keep;
        vertex_to_keep = vertex_to_remove;
        vertex_to_remove = temp;
    }


    // Do the same for tex vertices...
    if(tex_vertex_to_keep > tex_vertex_to_remove)
    {
        // Swap 'em, so the lower indexed tex_vertex is kept...
        temp = tex_vertex_to_keep;
        tex_vertex_to_keep = tex_vertex_to_remove;
        tex_vertex_to_remove = temp;
    }


    // Go through triangles...  Delete any that contains both of the vertices...  Renumber any
    // instance of the removed vertex...  Handle tex vertices too.
    triangle_data = data + (num_vertex_spot[0]<<5) + (num_tex_vertex_spot[0]<<3);
    repeat(i, num_texture)
    {
        triangle_data++;
        if(*(triangle_data-1) != 0)   // Mode
        {
            strip_data_start = triangle_data;
            num_triangle = *((unsigned short*) triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                triangle_data+=2;  // Skip number of points in strip
                matches = 0;
                repeat(k, 3)
                {
                    if(*((unsigned short*) triangle_data) == vertex_to_remove)
                        *((unsigned short*) triangle_data) = vertex_to_keep;
                    if(*((unsigned short*) triangle_data) == vertex_to_keep)
                        matches++;
                    if(*((unsigned short*) triangle_data) > vertex_to_remove)
                        *((unsigned short*) triangle_data) -= 1;
                    triangle_data+=2;
                    if(tex_vertex_to_keep != tex_vertex_to_remove)
                    {
                        if(*((unsigned short*) triangle_data) == tex_vertex_to_remove)
                            *((unsigned short*) triangle_data) = tex_vertex_to_keep;
                        if(*((unsigned short*) triangle_data) > tex_vertex_to_remove)
                            *((unsigned short*) triangle_data) -= 1;
                    }
                    triangle_data+=2;
                }
                // Delete the triangle if need be...
                if(matches > 1)
                {
                    if(num_triangle > 1)
                    {
                        // Just remove the triangle...
                        ddd_remove_data(data, data_size, triangle_data-14, 14);
                        num_triangle--;
                        *((unsigned short*) strip_data_start) = num_triangle;
                        triangle_data-=14;
                        j--;
                    }
                    else
                    {
                        // Remove the entire texture...
                        ddd_remove_data(data, data_size, strip_data_start-1, 18);
                        *(strip_data_start-1) = 0;
                        triangle_data = strip_data_start-2;
                    }
                }
            }
            triangle_data+=2;  // Skip number of fans
        }
    }


    // Average the positions of the points...
    first_xyz = (float*) (data+(vertex_to_keep<<5));
    second_xyz = (float*) (data+(vertex_to_remove<<5));
    first_xyz[X] = (first_xyz[X] + second_xyz[X]) * 0.5f;
    first_xyz[Y] = (first_xyz[Y] + second_xyz[Y]) * 0.5f;
    first_xyz[Z] = (first_xyz[Z] + second_xyz[Z]) * 0.5f;
    // Increase the weight, making it harder to remove in the future...
    first_xyz[6] += 0.5f;
    if(tex_vertex_to_keep != tex_vertex_to_remove)
    {
        first_xyz = (float*) (data+(num_vertex_spot[0]<<5)+(tex_vertex_to_keep<<3));
        second_xyz = (float*) (data+(num_vertex_spot[0]<<5)+(tex_vertex_to_remove<<3));
        first_xyz[X] = (first_xyz[X] + second_xyz[X]) * 0.5f;
        first_xyz[Y] = (first_xyz[Y] + second_xyz[Y]) * 0.5f;
    }


    // Now delete the vertex and the texture vertex...  Higher numbered references should have
    // been renumbered...
    num_vertex_spot[0]--;
    ddd_remove_data(data, data_size, data+(vertex_to_remove<<5), 32);
    if(tex_vertex_to_keep != tex_vertex_to_remove)
    {
        num_tex_vertex_spot[0]--;
        ddd_remove_data(data, data_size, data+(num_vertex_spot[0]<<5)+(tex_vertex_to_remove<<3), 8);
    }
}

//-----------------------------------------------------------------------------------------------
unsigned char ddd_simplify_once(unsigned short* num_vertex_spot, unsigned short* num_tex_vertex_spot, unsigned char* data, unsigned int data_size, unsigned char num_texture)
{
    // <ZZ> This function determines the best pair of vertices to get rid of, then gets rid of
    //      'em.
    unsigned short i, j, k;
    unsigned short best_vertex[2];
    unsigned short best_tex_vertex[2];
    float best_size;
    float size;
    unsigned char* triangle_data;
    unsigned short num_triangle;
    unsigned short triangle_vertex[3];
    unsigned short triangle_tex_vertex[3];
    float* vertexa_xyz;
    float* vertexb_xyz;
    float* vertexc_xyz;
    float side_xyz[3];

    best_size = 999999.0f;
    triangle_data = data + (num_vertex_spot[0]<<5) + (num_tex_vertex_spot[0]<<3);
    repeat(i, num_texture)
    {
        triangle_data++;
        if(*(triangle_data-1) != 0)   // Mode
        {
            num_triangle = *((unsigned short*) triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                triangle_data+=2;  // Skip number of points in strip...  Assumed to be 3...
                repeat(k, 3)
                {
                    triangle_vertex[k] = *((unsigned short*) triangle_data);  triangle_data+=2;
                    triangle_tex_vertex[k] = *((unsigned short*) triangle_data);  triangle_data+=2;
                }
                vertexa_xyz = (float*) (data + (triangle_vertex[0]<<5));
                vertexb_xyz = (float*) (data + (triangle_vertex[1]<<5));
                vertexc_xyz = (float*) (data + (triangle_vertex[2]<<5));
                // Find the shortest edge of triangle...  Side AB
                side_xyz[X] = vertexb_xyz[X] - vertexa_xyz[X];
                side_xyz[Y] = vertexb_xyz[Y] - vertexa_xyz[Y];
                side_xyz[Z] = vertexb_xyz[Z] - vertexa_xyz[Z];
                size = (side_xyz[X]*side_xyz[X])+(side_xyz[Y]*side_xyz[Y])+(side_xyz[Z]*side_xyz[Z]);
                size = size * (vertexb_xyz[6] + vertexa_xyz[6]);  // Factor vertex weight into it...
                if(size < best_size)
                {
                    best_vertex[0] = triangle_vertex[0];
                    best_vertex[1] = triangle_vertex[1];
                    best_tex_vertex[0] = triangle_tex_vertex[0];
                    best_tex_vertex[1] = triangle_tex_vertex[1];
                    best_size = size;
                }
                // Find the shortest edge of triangle...  Side BC
                side_xyz[X] = vertexc_xyz[X] - vertexb_xyz[X];
                side_xyz[Y] = vertexc_xyz[Y] - vertexb_xyz[Y];
                side_xyz[Z] = vertexc_xyz[Z] - vertexb_xyz[Z];
                size = (side_xyz[X]*side_xyz[X])+(side_xyz[Y]*side_xyz[Y])+(side_xyz[Z]*side_xyz[Z]);
                size = size * (vertexc_xyz[6] + vertexb_xyz[6]);  // Factor vertex weight into it...
                if(size < best_size)
                {
                    best_vertex[0] = triangle_vertex[1];
                    best_vertex[1] = triangle_vertex[2];
                    best_tex_vertex[0] = triangle_tex_vertex[1];
                    best_tex_vertex[1] = triangle_tex_vertex[2];
                    best_size = size;
                }
                // Find the shortest edge of triangle...  Side CA
                side_xyz[X] = vertexa_xyz[X] - vertexc_xyz[X];
                side_xyz[Y] = vertexa_xyz[Y] - vertexc_xyz[Y];
                side_xyz[Z] = vertexa_xyz[Z] - vertexc_xyz[Z];
                size = (side_xyz[X]*side_xyz[X])+(side_xyz[Y]*side_xyz[Y])+(side_xyz[Z]*side_xyz[Z]);
                size = size * (vertexa_xyz[6] + vertexc_xyz[6]);  // Factor vertex weight into it...
                if(size < best_size)
                {
                    best_vertex[0] = triangle_vertex[2];
                    best_vertex[1] = triangle_vertex[0];
                    best_tex_vertex[0] = triangle_tex_vertex[2];
                    best_tex_vertex[1] = triangle_tex_vertex[0];
                    best_size = size;
                }
            }
            triangle_data+=2;  // Skip number of fans
        }
    }


    // Did we find a triangle to axe?
    if(best_size < 999990.0f)
    {
        ddd_simplify_two_vertices(num_vertex_spot, num_tex_vertex_spot, best_vertex[0], best_vertex[1], best_tex_vertex[0], best_tex_vertex[1], data, data_size, num_texture);
        return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
void ddd_simplify_geometry(unsigned short* num_vertex_spot, unsigned short* num_tex_vertex_spot, unsigned char* data, unsigned char percent, unsigned int data_size, unsigned char num_texture)
{
    // <ZZ> Given a block of vertex/texture vertex/texture data, this function spits out a simpler
    //      version of that data (depending on detail percent).  The values at num_vertex_spot and
    //      num_tex_vertex_spot are changed by the function to show the new number of vertices and
    //      texture vertices.
    unsigned short num_vertex_wanted;
    unsigned char can_remove_more;
    unsigned char* vertex_data;
    unsigned char* triangle_data;
    unsigned short num_triangle;
    unsigned short triangle_vertex[3];
    float start_xyz[3];
    float end_xyz[3];
    float side_xyz[3];
    float front_xyz[3];
    float* normal_xyz;
    float length;
    unsigned short i, j, k;



    // Clear out the extra vertex data we've been given...
    vertex_data = data;
    repeat(i, num_vertex_spot[0])
    {
        *((float*) (vertex_data+12)) = 0;           // Normal X
        *((float*) (vertex_data+16)) = 0;           // Normal Y
        *((float*) (vertex_data+20)) = 0;           // Normal Z
        *((float*) (vertex_data+24)) = 0;           // Weight
        *((unsigned short*) (vertex_data+28)) = 0;  // Neighbor count
        *((unsigned short*) (vertex_data+30)) = i;  // Original vertex
        vertex_data+=32;
    }


    // Figure out normal and neighbor count for each vertex...
    vertex_data = data;
    triangle_data = data+(num_vertex_spot[0]<<5)+(num_tex_vertex_spot[0]<<3);
    repeat(i, num_texture)
    {
        triangle_data++;
        if(*(triangle_data-1) != 0)   // Mode
        {
            num_triangle = *((unsigned short*) triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                triangle_data+=2;  // Skip number of points in strip, assume to be 3
                repeat(k, 3)
                {
                    triangle_vertex[k] = *((unsigned short*) triangle_data);  triangle_data+=4;
                    // Increase the neighbor count for each vertex...
                    *((unsigned short*) (vertex_data+28+(triangle_vertex[k]<<5)))+=1;
                }
                // Figure out the normal of the triangle
                start_xyz[X] = *((float*) (vertex_data+(triangle_vertex[0]<<6)));
                start_xyz[Y] = *((float*) (vertex_data+4+(triangle_vertex[0]<<6)));
                start_xyz[Z] = *((float*) (vertex_data+8+(triangle_vertex[0]<<6)));
                end_xyz[X] = *((float*) (vertex_data+(triangle_vertex[1]<<6)));
                end_xyz[Y] = *((float*) (vertex_data+4+(triangle_vertex[1]<<6)));
                end_xyz[Z] = *((float*) (vertex_data+8+(triangle_vertex[1]<<6)));
                end_xyz[X]-=start_xyz[X];
                end_xyz[Y]-=start_xyz[Y];
                end_xyz[Z]-=start_xyz[Z];
                side_xyz[X]= *((float*) (vertex_data+(triangle_vertex[2]<<6)));
                side_xyz[Y]= *((float*) (vertex_data+4+(triangle_vertex[2]<<6)));
                side_xyz[Z]= *((float*) (vertex_data+8+(triangle_vertex[2]<<6)));
                side_xyz[X]-=start_xyz[X];
                side_xyz[Y]-=start_xyz[Y];
                side_xyz[Z]-=start_xyz[Z];
                cross_product(end_xyz, side_xyz, front_xyz);
                // Normalize the normal
                length = vector_length(front_xyz);
                if(length > 0.01f)
                {
                    front_xyz[X]/=length;
                    front_xyz[Y]/=length;
                    front_xyz[Z]/=length;
                }
                // Add the normal to each of the vertices
                repeat(k, 3)
                {
                    *((float*) (vertex_data+12+(triangle_vertex[k]<<5)))+=front_xyz[X];
                    *((float*) (vertex_data+16+(triangle_vertex[k]<<5)))+=front_xyz[Y];
                    *((float*) (vertex_data+20+(triangle_vertex[k]<<5)))+=front_xyz[Z];
                }
            }
            triangle_data+=2;  // Skip number of fans
        }
    }


    // Calculate vertex weights...
    repeat(i, num_vertex_spot[0])
    {
        normal_xyz = ((float*) (vertex_data+12+(i<<5)));
        length = vector_length(normal_xyz);
        if(*((unsigned short*) (vertex_data+28+(i<<5))) > 0)
        {
            // Weight is the similarity of normals...
            *((float*) (vertex_data+24+(i<<5))) = 1.0f - (0.5f*(length/(*((unsigned short*) (vertex_data+28+(i<<5))))));
        }
    }


    // Start removing vertices...
    num_vertex_wanted = (num_vertex_spot[0] * percent) >> 8;
    if(num_vertex_wanted < 16)
    {
        // So eyes and other little details come through okay...
        num_vertex_wanted = 16;
    }
    can_remove_more = TRUE;
    while(can_remove_more && num_vertex_spot[0] > num_vertex_wanted)
    {
        can_remove_more = ddd_simplify_once(num_vertex_spot, num_tex_vertex_spot, data, data_size, num_texture);
    }
}

//-----------------------------------------------------------------------------------------------
void ddd_continue_simplify(unsigned short* num_vertex_spot, unsigned short* num_tex_vertex_spot, unsigned char* data, unsigned char percent, unsigned int data_size, unsigned char num_texture, unsigned short num_vertex)
{
    // <ZZ> Optimization (read hack) to ddd_simplify_geometry that allows a lower detail level's
    //      simplification process to start with the last detail level's data...
    unsigned short num_vertex_wanted;
    unsigned char can_remove_more;


    // Start removing vertices...
    num_vertex_wanted = (num_vertex * percent) >> 8;
#ifdef VERBOSE_COMPILE
    log_message("INFO:   Continue simplify");
    log_message("INFO:   Have %d vertices, Want %d", num_vertex_spot[0], num_vertex_wanted);
#endif
    if(num_vertex_wanted < 8)
    {
        // So eyes and other little details come through okay...
        num_vertex_wanted = 8;
    }
    can_remove_more = TRUE;
    while(can_remove_more && num_vertex_spot[0] > num_vertex_wanted)
    {
        can_remove_more = ddd_simplify_once(num_vertex_spot, num_tex_vertex_spot, data, data_size, num_texture);
#ifdef VERBOSE_COMPILE
    log_message("INFO:   Have %d vertices, Want %d", num_vertex_spot[0], num_vertex_wanted);
#endif
    }
}

//-----------------------------------------------------------------------------------------------
void add_cartoon_line(unsigned short vertex_a, unsigned short vertex_b, unsigned short check, unsigned char force_line, unsigned short* cartoon_data)
{
    // <ZZ> This function adds a line between two vertices (if there isn't already one between 'em).
    //      The check points are points on nearby triangles that are used to determine if the line
    //      is visible or not (visible if both check points are on same side of line, when seen
    //      from camera position).
    //      If called once for a given vertex pair, the line is assumed to be always on (if force_line is TRUE)...
    //      If called twice, the two check points are used to figure if the line is on or not...
    int num_cartoon_line;
    unsigned short* data;
    unsigned char found;
    unsigned short i;

    num_cartoon_line = *cartoon_data;
    data = cartoon_data+1;
    found = FALSE;
    repeat(i, num_cartoon_line)
    {
        if(*data == vertex_a || *data == vertex_b)
        {
            if(*(data+1) == vertex_a || *(data+1) == vertex_b)
            {
                if(*(data+2) != check)
                {
                    found = TRUE;
                    i = num_cartoon_line;
                    // Fill in the second check point...
                    *(data+3) = check;
                }
            }
        }
        data+=4;
    }
    if(found == FALSE)
    {
        // Line doesn't exist yet, so add it...
        *data = vertex_a;  data++;
        *data = vertex_b;  data++;
        *data = check;  data++;
        if(force_line)
        {
            *data = check;
        }
        else
        {
            *data = 65535;  // Go back and remove these entries later...
        }
        num_cartoon_line++;
        *cartoon_data = num_cartoon_line;
    }
}

//-----------------------------------------------------------------------------------------------
void remove_cartoon_lines(unsigned short* cartoon_data)
{
    // <ZZ> This function deletes any cartoon line with 65535 as its second check point...
    //      Bare edges of culled textures...
    int num_cartoon_line;
    unsigned short* data;
    unsigned short* copy_data;
    unsigned short i, j;

    num_cartoon_line = *cartoon_data;
    data = cartoon_data+1;
    repeat(i, num_cartoon_line)
    {
        if(*(data+3) == 65535)
        {
            // We should delete this one...  Bump all others back...
            copy_data = data;
            j = i+1;
            while(j < num_cartoon_line)
            {
                *(copy_data)   = *(copy_data+4);
                *(copy_data+1) = *(copy_data+5);
                *(copy_data+2) = *(copy_data+6);
                *(copy_data+3) = *(copy_data+7);
                copy_data+=4;
                j++;
            }

            // Remember that it's been deleted...
            num_cartoon_line--;
            *cartoon_data = num_cartoon_line;
            i--;
        }
        else
        {
            data+=4;
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define USED_TEMPORARY 2
#define USED_PERMANENT 1

//-----------------------------------------------------------------------------------------------
unsigned char* ddd_create_strip(unsigned char* helper_data, unsigned char* new_data, unsigned short main_triangle, unsigned short num_triangle)
{
    // <ZZ> This function adds a strip for the strip_fan_geometry function...  Returns next primitive
    //      position...  Checks all 3 directions for largest strip possible, and uses that one...
    unsigned char texture;
    unsigned short look_triangle;
    unsigned short test_triangle;
    unsigned short last_triangle[3];
    unsigned short num_found_triangle[3];
    unsigned short vertex[3];
    unsigned short tex_vertex[3];
    unsigned short starting_vertex[3];
    unsigned short vertex_edge[2];
    unsigned short tex_vertex_edge[2];
    unsigned short i, j, k;
    unsigned char* new_data_start;
    unsigned char* temp_data;
    unsigned short set_count;
    unsigned char keep_looking;
    unsigned char looking_for_neighbor;
    unsigned short num_found_vertex;
    unsigned short order, second;

    texture = *(helper_data+30+(main_triangle<<5));
    new_data_start = new_data;



#ifdef VERBOSE_COMPILE
    log_message("INFO:   Making triangle strip that includes triangle %d", main_triangle);
#endif
    // Check all 3 directions for strip size...  Direction depends on the starting vertex...
    repeat(i, 3)
    {
#ifdef VERBOSE_COMPILE
    log_message("INFO:     Stripping attempt %d", i);
#endif


        // Clear out all temporary used flags
        temp_data = helper_data;
        repeat(j, num_triangle)
        {
            *(temp_data+15) &= USED_PERMANENT;
            temp_data+=16;
        }


        // Figure out which vertices are in the triangle...
        repeat(j, 3)
        {
            k = (i+j)%3;
            vertex[j] = *((unsigned short*) (helper_data+(main_triangle<<5)+(k<<1)+18));
            tex_vertex[j] = *((unsigned short*) (helper_data+(main_triangle<<5)+(k<<1)+24));
        }


        // Mark main triangle as being used...
#ifdef VERBOSE_COMPILE
    log_message("INFO:       Added triangle %d (main)", main_triangle);
#endif
        *(helper_data+(main_triangle<<5)+31) |= USED_TEMPORARY;
        num_found_triangle[i] = 1;


        // Go forwards...  Connected triangle should have points 1 and 2...
        starting_vertex[i] = vertex[0];
        look_triangle = main_triangle;
        vertex_edge[0] = vertex[1];
        vertex_edge[1] = vertex[2];
        tex_vertex_edge[0] = tex_vertex[1];
        tex_vertex_edge[1] = tex_vertex[2];
        keep_looking = TRUE;
        while(keep_looking)
        {
            // Find a neighbor who uses both vertices in the edge, and make it our current triangle...
            looking_for_neighbor = TRUE;
            j = 0;
            while(looking_for_neighbor && j < *((unsigned short*) (helper_data+(look_triangle<<5))) && j < 8)
            {
                test_triangle = *((unsigned short*) (helper_data + (look_triangle<<5) + 2 + (j<<1)));
                if(*(helper_data + (test_triangle<<5) + 31) == 0 && *(helper_data + (test_triangle<<5) + 30) == texture)
                {
                    // Hasn't been used yet and is of the proper texture...  Does it contain both of the vertices?
                    if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[0]))
                    {
                        // Contains the first vertex...  How bout the second?
                        if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[1]))
                        {
                            // Contains both vertices...  How bout tex vertices?
                            if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[0]))
                            {
                                // First okay...  Second?
                                if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[1]))
                                {
                                    // Use it as our next triangle...
                                    look_triangle = test_triangle;
                                    looking_for_neighbor = FALSE;
                                }
                                else
                                {
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Get these errors a lot...  Think I need to check if texture mode is on...
// !!!BAD!!!
// !!!BAD!!!
#ifdef VERBOSE_COMPILE
    log_message("ERROR:  Fore Tex Vertex 1 %d", tex_vertex_edge[1]);
#endif
                                }
                            }
                            else
                            {
#ifdef VERBOSE_COMPILE
    log_message("ERROR:  Fore Tex Vertex 0 %d", tex_vertex_edge[0]);
#endif
                            }
                        }
                    }
                }
                j++;
            }
            if(looking_for_neighbor)
            {
                // Didn't find a valid neighbor...  Must be all done in this direction...
                keep_looking = FALSE;
            }
            else
            {
#ifdef VERBOSE_COMPILE
    log_message("INFO:       Added triangle %d (forward)", look_triangle);
#endif
                num_found_triangle[i]++;
                *(helper_data+(look_triangle<<5)+31) = USED_TEMPORARY;


                // Figure out next pair of vertices...
                repeat(j, 3)
                {
                    if(*((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[0] && *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[1])
                    {
                        vertex_edge[0] = vertex_edge[1];
                        vertex_edge[1] = *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5)));
                        tex_vertex_edge[0] = tex_vertex_edge[1];
                        tex_vertex_edge[1] = *((unsigned short*) (helper_data+24+(j<<1)+(look_triangle<<5)));
                        j = 3;
                    }
                }
            }
        }




        // Go backwards...  Count in sets of two...  Must have points 0 and 1....
        look_triangle = main_triangle;
        vertex_edge[0] = vertex[1];
        vertex_edge[1] = vertex[0];
        tex_vertex_edge[0] = tex_vertex[1];
        tex_vertex_edge[1] = tex_vertex[0];
        set_count = 0;
        keep_looking = TRUE;
        while(keep_looking)
        {
            // Find a neighbor who uses both vertices in the edge, and make it our current triangle...
            looking_for_neighbor = TRUE;
            j = 0;
            while(looking_for_neighbor && j < *((unsigned short*) (helper_data+(look_triangle<<5))) && j < 8)
            {
                test_triangle = *((unsigned short*) (helper_data + (look_triangle<<5) + 2 + (j<<1)));
                if(*(helper_data + (test_triangle<<5) + 31) == 0 && *(helper_data + (test_triangle<<5) + 30) == texture)
                {
                    // Hasn't been used yet and is of the proper texture...  Does it contain both of the vertices?
                    if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[0]))
                    {
                        // Contains the first vertex...  How bout the second?
                        if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[1]))
                        {
                            // Contains both vertices...  How bout tex vertices?
                            if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[0]))
                            {
                                // First okay...  Second?
                                if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[1]))
                                {
                                  // If this one ends up being our last triangle, we'll need to remember a starting vertex...
//log_message("INFO:       Should we set start vertex???");
                                    if(set_count == 1 && look_triangle != main_triangle)
                                    {
//log_message("INFO:       Setting the start vertex for direction %d", i);
                                        if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) != vertex_edge[0]) && (*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) != vertex_edge[1]))
                                        {
                                            starting_vertex[i] = *((unsigned short*) (helper_data + (test_triangle<<5) + 18));
//log_message("INFO:         Start vertex is %d (first)", starting_vertex[i]);
                                        }
                                        else if((*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) != vertex_edge[0]) && (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) != vertex_edge[1]))
                                        {
                                            starting_vertex[i] = *((unsigned short*) (helper_data + (test_triangle<<5) + 20));
//log_message("INFO:         Start vertex is %d (second)", starting_vertex[i]);
                                        }
                                        else if((*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) != vertex_edge[0]) && (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) != vertex_edge[1]))
                                        {
                                            starting_vertex[i] = *((unsigned short*) (helper_data + (test_triangle<<5) + 22));
//log_message("INFO:         Start vertex is %d (third)", starting_vertex[i]);
                                        }
                                    }


                                    // Contains both vertices...  Use it as our next triangle...
                                    last_triangle[i] = look_triangle;
                                    look_triangle = test_triangle;
                                    looking_for_neighbor = FALSE;
                                    set_count=(set_count+1)&1;
                                }
                                else
                                {
#ifdef VERBOSE_COMPILE
    log_message("ERROR:  Back Tex Vertex 1 %d", tex_vertex_edge[1]);
#endif
                                }
                            }
                            else
                            {
#ifdef VERBOSE_COMPILE
    log_message("ERROR:  Back Tex Vertex 0 %d", tex_vertex_edge[0]);
#endif
                            }
                        }
                    }
                }
                j++;
            }
            if(looking_for_neighbor)
            {
                // Didn't find a valid neighbor...  Must be all done in this direction...
                keep_looking = FALSE;
            }
            else
            {
#ifdef VERBOSE_COMPILE
    log_message("INFO:       Added triangle %d (backward)", look_triangle);
#endif
                num_found_triangle[i]++;
                *(helper_data+(look_triangle<<5)+31) = USED_TEMPORARY;


                // Figure out next pair of vertices...
                repeat(j, 3)
                {
                    if(*((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[0] && *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[1])
                    {
                        vertex_edge[0] = vertex_edge[1];
                        vertex_edge[1] = *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5)));
                        tex_vertex_edge[0] = tex_vertex_edge[1];
                        tex_vertex_edge[1] = *((unsigned short*) (helper_data+24+(j<<1)+(look_triangle<<5)));
                        j = 3;
                    }
                }
            }
        }
        // Going backwards 2 at a time...
        if(set_count == 0)
        {
            last_triangle[i] = look_triangle;
        }
        num_found_triangle[i] -= set_count;
#ifdef VERBOSE_COMPILE
    log_message("INFO:       Found %d triangles, strip should start on %d, ignoring %d triangle", num_found_triangle[i], last_triangle[i], set_count);
#endif
    }


    // Figure out which triangle to start from (depending on whichever direction had the longest strip)
    i = 0;
    if(num_found_triangle[1] > num_found_triangle[i])  i = 1;
    if(num_found_triangle[2] > num_found_triangle[i])  i = 2;
    main_triangle = last_triangle[i];
#ifdef VERBOSE_COMPILE
    log_message("INFO:     Best strip was from direction %d", i);
    log_message("INFO:     Starting at triangle %d, Vertex %d", main_triangle, starting_vertex[i]);
    log_message("INFO:     First three vertices are %d, %d, %d...", *((unsigned short*) (helper_data+18+(main_triangle<<5))), *((unsigned short*) (helper_data+20+(main_triangle<<5))), *((unsigned short*) (helper_data+22+(main_triangle<<5))));
#endif

    // Find the starting edge...
    repeat(j, 3)
    {
        if(*((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+18)) == starting_vertex[i])
        {
            tex_vertex[0] = *((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+24));
            j = (j+1)%3;
            vertex_edge[0] = *((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+18));
            tex_vertex[1] = *((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+24));
            j = (j+1)%3;
            vertex_edge[1] = *((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+18));
            tex_vertex[2] = *((unsigned short*) (helper_data+(main_triangle<<5)+(j<<1)+24));
            j = 3;
        }
    }


    // Clear out all temporary used flags
    temp_data = helper_data;
    repeat(j, num_triangle)
    {
        *(temp_data+31) &= USED_PERMANENT;
        temp_data+=32;
    }
    *(helper_data+(main_triangle<<5)+31) = USED_PERMANENT;


    // Start writing our output data...


#ifdef VERBOSE_COMPILE
    log_message("INFO:     Output start...");
    log_message("INFO:       (char)  'S'");
    log_message("INFO:       (char)  %d", texture);
    log_message("INFO:       (word)  %d (may change)", num_found_triangle[i]+2);
#endif
    *new_data = 'S';  new_data++;
    *new_data = texture;  new_data+=3;
    num_found_vertex = 3;
#ifdef VERBOSE_COMPILE
    log_message("INFO:       (word)  %d", starting_vertex[i]);
    log_message("INFO:       (word)  %d (tex)", tex_vertex[0]);
#endif
    *((unsigned short*) new_data) = starting_vertex[i];  new_data+=2;
    *((unsigned short*) new_data) = tex_vertex[0];  new_data+=2;
#ifdef VERBOSE_COMPILE
    log_message("INFO:       (word)  %d", vertex_edge[0]);
    log_message("INFO:       (word)  %d (tex)", tex_vertex[1]);
#endif
    *((unsigned short*) new_data) = vertex_edge[0];  new_data+=2;
    *((unsigned short*) new_data) = tex_vertex[1];  new_data+=2;
#ifdef VERBOSE_COMPILE
    log_message("INFO:       (word)  %d", vertex_edge[1]);
    log_message("INFO:       (word)  %d (tex)", tex_vertex[2]);
#endif
    *((unsigned short*) new_data) = vertex_edge[1];  new_data+=2;
    *((unsigned short*) new_data) = tex_vertex[2];  new_data+=2;
    tex_vertex_edge[0] = tex_vertex[1];
    tex_vertex_edge[1] = tex_vertex[2];


    // Go through triangles again, this time with a known starting triangle and vertex...
    // Write output vertices as we go...
    starting_vertex[i] = vertex[0];
    look_triangle = main_triangle;
    set_count = 0;
    keep_looking = TRUE;
    while(keep_looking)
    {
        // Find a neighbor who uses both vertices in the edge, and make it our current triangle...
//log_message("INFO:       Triangle %d has been added, now looking for a neighbor...", look_triangle);
//log_message("INFO:       Looking on edge %d to %d", vertex_edge[0], vertex_edge[1]);

        looking_for_neighbor = TRUE;
        j = 0;
        while(looking_for_neighbor && j < *((unsigned short*) (helper_data+(look_triangle<<5))) && j < 8)
        {
            test_triangle = *((unsigned short*) (helper_data + (look_triangle<<5) + 2 + (j<<1)));
//log_message("INFO:         Testing neighbor %d", test_triangle);
            if(*(helper_data + (test_triangle<<5) + 31) == 0 && *(helper_data + (test_triangle<<5) + 30) == texture)
            {
//log_message("INFO:           Hasn't been used and is of proper texture");
                // Hasn't been used yet and is of the proper texture...  Does it contain both of the vertices?




                // !!!BAD!!!
                // !!!BAD!!!  Order test...
                // !!!BAD!!!
                order = 5;
                if(*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[0])  { order = 0; }
                else if(*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[0])  { order = 1; }
                else if(*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[0])  { order = 2; }
                if(order < 3)
                {
                    if(set_count) second = (order+1)%3;
                    else          second = (order+2)%3;
                    if(*((unsigned short*) (helper_data + (test_triangle<<5) + 18 + (second<<1))) == vertex_edge[1])
                    {
                        if(*((unsigned short*) (helper_data + (test_triangle<<5) + 24 + (order<<1))) == tex_vertex_edge[0])
                        {
                            if(*((unsigned short*) (helper_data + (test_triangle<<5) + 24 + (second<<1))) == tex_vertex_edge[1])
                            {







//                if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[0]))
//                {
//log_message("INFO:           Contains first vertex (%d)", vertex_edge[0]);
//                    // Contains the first vertex...  How bout the second?
//                    if((*((unsigned short*) (helper_data + (test_triangle<<5) + 18)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 20)) == vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 22)) == vertex_edge[1]))
//                    {
//                        // Contains both vertices...  How bout tex vertices?
//                        if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[0]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[0]))
//                        {
//                            // First okay...  Second?
//                            if((*((unsigned short*) (helper_data + (test_triangle<<5) + 24)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 26)) == tex_vertex_edge[1]) || (*((unsigned short*) (helper_data + (test_triangle<<5) + 28)) == tex_vertex_edge[1]))
//                            {
//log_message("INFO:           Contains second vertex (%d)", vertex_edge[1]);
//log_message("INFO:           Looks like a good neighbor");
                                // Contains all vertices...  Use it as our next triangle...
                                look_triangle = test_triangle;
                                looking_for_neighbor = FALSE;
                                set_count=(set_count+1)&1;
                            }
                        }
                    }
                }
            }
            j++;
        }
        if(looking_for_neighbor)
        {
            // Didn't find a valid neighbor...  Must be all done in this direction...
            keep_looking = FALSE;
        }
        else
        {
            *(helper_data+(look_triangle<<5)+31) = USED_PERMANENT;


            // Figure out next pair of vertices...  And which one to add to output
            repeat(j, 3)
            {
                if(*((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[0] && *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5))) != vertex_edge[1])
                {
                    vertex_edge[0] = vertex_edge[1];
                    vertex_edge[1] = *((unsigned short*) (helper_data+18+(j<<1)+(look_triangle<<5)));
                    *((unsigned short*) new_data) = vertex_edge[1];  new_data+=2;
                    tex_vertex_edge[0] = tex_vertex_edge[1];
                    tex_vertex_edge[1] = *((unsigned short*) (helper_data+24+(j<<1)+(look_triangle<<5)));
                    *((unsigned short*) new_data) = tex_vertex_edge[1];  new_data+=2;
                    num_found_vertex++;
#ifdef VERBOSE_COMPILE
    log_message("INFO:       (word)  %d", vertex_edge[1]);
    log_message("INFO:       (word)  %d (tex)", tex_vertex_edge[1]);
#endif
                    j = 3;
                }
            }
        }
    }
#ifdef VERBOSE_COMPILE
    log_message("INFO:     Final vertex count was %d (written back in up above...)", num_found_vertex);
#endif
    *((unsigned short*) (new_data_start+2)) = num_found_vertex;


    *new_data = 0;  // To prevent a random 'S' from bein' there...  Can cause trouble...
    return new_data;
}

//-----------------------------------------------------------------------------------------------
void ddd_strip_fan_geometry(unsigned short num_vertex, unsigned short num_tex_vertex, unsigned char* old_data, unsigned char* helper_data, unsigned char* new_data, unsigned char num_texture)
{
    // <ZZ> This function converts a list of triangles into a list of strips and fans...  Also
    //      generates connection info, which is useful for other things (cartoon lines)...  No longer
    //      handles fan generation, as it generally wasn't as efficient as just having strips...
    unsigned short num_triangle, num_primitive, triangle, first_triangle;
    unsigned short i, j, k, m;
    unsigned char mode;
    unsigned char* data;
    unsigned short vertex[3];
    unsigned char match;


#ifdef VERBOSE_COMPILE
    log_message("INFO:   Stripping, Fanning");
#endif


    // Skip over useless stuff...
    old_data += (num_vertex<<5) + (num_tex_vertex<<3);


    // Count the number of triangles
    num_triangle = 0;
    data = old_data;
    repeat(i, num_texture)
    {
        mode = *(data);  data++;
        if(mode)
        {
            num_primitive = *((unsigned short*) data);  data+=2;
            num_triangle+=num_primitive;
            data+=(num_primitive*14);
            data+=2;  // Skip number of fans (should be 0)
        }
    }


    // Start building triangle connection table...
    data = helper_data;
    repeat(i, num_triangle)
    {
        *((unsigned short*) data) = 0;  data+=2;        //  0  Number of neighboring triangles
        *((unsigned short*) data) = 65535;  data+=2;    //  2  Neighbor 0
        *((unsigned short*) data) = 65535;  data+=2;    //  4  Neighbor 1
        *((unsigned short*) data) = 65535;  data+=2;    //  6  Neighbor 2
        *((unsigned short*) data) = 65535;  data+=2;    //  8  Neighbor 3
        *((unsigned short*) data) = 65535;  data+=2;    // 10  Neighbor 4
        *((unsigned short*) data) = 65535;  data+=2;    // 12  Neighbor 5
        *((unsigned short*) data) = 65535;  data+=2;    // 14  Neighbor 6
        *((unsigned short*) data) = 65535;  data+=2;    // 16  Neighbor 7
        *((unsigned short*) data) = 0;  data+=2;        // 18  Vertex 0
        *((unsigned short*) data) = 0;  data+=2;        // 20  Vertex 1
        *((unsigned short*) data) = 0;  data+=2;        // 22  Vertex 2
        *((unsigned short*) data) = 0;  data+=2;        // 24  Tex Vertex 0
        *((unsigned short*) data) = 0;  data+=2;        // 26  Tex Vertex 1
        *((unsigned short*) data) = 0;  data+=2;        // 28  Tex Vertex 2
        *data = 0;  data+=1;                            // 30  Texture triangle is in
        *data = 0;  data+=1;                            // 31  Triangle used somewhere
    }
    global_base_num_triangle = num_triangle;
    // Fill in the table with triangle data...
    data = old_data;
    triangle = 0;
    repeat(i, num_texture)
    {
        mode = *(data);  data++;
        if(mode)
        {
            num_primitive = *((unsigned short*) data);  data+=2;
            first_triangle = triangle;
            repeat(j, num_primitive)
            {
                data+=2;  // Skip number of vertices (should be 3)
                repeat(k, 3)
                {
                    vertex[k] = *((unsigned short*) data);  data+=2;
                    *((unsigned short*) (helper_data+(triangle<<5)+18+(k<<1))) = vertex[k];  // Vertex
                    *((unsigned short*) (helper_data+(triangle<<5)+24+(k<<1))) = *((unsigned short*) data);  data+=2;  // Tex vertex
                }
                // Remember which texture it falls under...
                *(helper_data+(triangle<<5)+30) = (unsigned char) i;
                // Look for any triangle that shares 2 vertices...  Count as a neighbor...
                k = first_triangle;
                while(k < triangle)
                {
                    match = 0;
                    repeat(m, 3)
                    {
                        if(*((unsigned short*) (helper_data+(k<<5)+18+(m<<1))) == vertex[0] || *((unsigned short*) (helper_data+(k<<5)+18+(m<<1))) == vertex[1] || *((unsigned short*) (helper_data+(k<<5)+18+(m<<1))) == vertex[2])
                        {
                            match++;
                        }
                    }
                    if(match > 1)
                    {
                        // Triangles are neighbors...
                        // Record in the lower indexed, if possible...
                        if(*((unsigned short*) (helper_data+(k<<5))) < 8)
                        {
                            m = *((unsigned short*) (helper_data+(k<<5)));
                            *((unsigned short*) (helper_data+(k<<5)+2+(m<<1))) = triangle;
                            *((unsigned short*) (helper_data+(k<<5))) += 1;
                        }
                        // Record in the current, if possible...
                        if(*((unsigned short*) (helper_data+(triangle<<5))) < 8)
                        {
                            m = *((unsigned short*) (helper_data+(triangle<<5)));
                            *((unsigned short*) (helper_data+(triangle<<5)+2+(m<<1))) = k;
                            *((unsigned short*) (helper_data+(triangle<<5))) += 1;
                        }
                    }
                    k++;
                }
                triangle++;
            }
            data+=2;  // Skip number of fans (should be 0)
        }
    }


    // Should now have full connection information...
#ifdef VERBOSE_COMPILE
    data = helper_data;
    repeat(i, num_triangle)
    {
        log_message("INFO:     Triangle %d", i);
        log_message("INFO:       %d connections", *((unsigned short*) data));
        log_message("INFO:       Connected to %d, %d, %d, %d", *((unsigned short*) (data+2)), *((unsigned short*) (data+4)), *((unsigned short*) (data+6)), *((unsigned short*) (data+8)));
        log_message("INFO:       Connected to %d, %d, %d, %d", *((unsigned short*) (data+10)), *((unsigned short*) (data+12)), *((unsigned short*) (data+14)), *((unsigned short*) (data+16)));
        log_message("INFO:       Vertices %d, %d, %d", *((unsigned short*) (data+18)), *((unsigned short*) (data+20)), *((unsigned short*) (data+22)));
        data+=32;
    }
#endif


    // Show texture vertices...
#ifdef VERBOSE_COMPILE
    log_message("INFO:     Texture Shoow...");
    data = helper_data;
    repeat(i, num_triangle)
    {
        log_message("INFO:       Triangle %d...  Tex vertices %d, %d, %d", i, *((unsigned short*) (data+24)), *((unsigned short*) (data+26)), *((unsigned short*) (data+28)));
        data+=32;
    }
#endif



    #ifdef DEVTOOL
        // Turn the triangles into single triangle strips...  Round and about way of doing things...
        data = helper_data;
        repeat(i, num_triangle)
        {
            *new_data = 'S';  new_data++;
            *new_data = *(data+30);  new_data++;
            *((unsigned short*) new_data) = 3;  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+18));  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+24));  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+20));  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+26));  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+22));  new_data+=2;
            *((unsigned short*) new_data) = *((unsigned short*) (data+28));  new_data+=2;
            data+=32;
        }
    #else
        // Turn the triangles into strips...  Used to do fans too, but took out...
        data = helper_data;
        repeat(i, num_triangle)
        {
            while((*(data+31) & USED_PERMANENT) == 0)
            {
                new_data = ddd_create_strip(helper_data, new_data, i, num_triangle);
            }
            data+=32;
        }
    #endif




    *new_data = 0;
    // The new_data should look something like this now...
    // 'F'      (unsigned char)     // Start of primitive (F for Fan, S for Strip) (Fans no longer supported)
    // 0        (unsigned char)     // Texture
    // 6        (unsigned short)    // Number of vertices in primitive
    //   15       (unsigned short)    // Vertex
    //   0        (unsigned short)    // Tex vertex
    //   16       (unsigned short)    // Vertex
    //   1        (unsigned short)    // Tex vertex
    //   17       (unsigned short)    // Vertex
    //   2        (unsigned short)    // Tex vertex
    //   18       (unsigned short)    // Vertex
    //   3        (unsigned short)    // Tex vertex
    //   19       (unsigned short)    // Vertex
    //   4        (unsigned short)    // Tex vertex
    //   20       (unsigned short)    // Vertex
    //   5        (unsigned short)    // Tex vertex
    // 'S'      (unsigned char)     // Start of primitive...
    // 0        (unsigned char)     // Texture...
    // 5    ...
    // ...
    // ...
    // ...
    // 0        (unsigned char)     // Start of primitive (Null terminator)
}

//-----------------------------------------------------------------------------------------------
void ddd_decode_base_model(unsigned char** olddata_spot, unsigned char** newdata_spot, unsigned char** newindex_spot, unsigned char* newdata_start, unsigned char detail_level, float scale)
{
    // <ZZ> This function helps out decode_ddd...
    unsigned char* olddata;
    unsigned char* newdata;
    unsigned char* tempdata;
    unsigned char* strip_fan_data;
    unsigned char* strip_fan_data_start;
    unsigned char* triangle_data_start;
    unsigned short num_vertex, num_joint, num_bone;
    unsigned short num_triangle, num_points_in_prim;
    unsigned short i, j, k, b;
    unsigned char mode, alpha, flags;
    unsigned short vertex, tex_vertex;
    float x, y, z;
    unsigned short original_vertex;
    unsigned short triangle_vertex[3];
    unsigned char silhouette_flag;
    unsigned short num_prim;

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  DETAIL LEVEL 0 SHOULD BE FULL QUALITY!!!!  ALWAYS!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



    // Get the file positions from the main function...
    olddata = *olddata_spot;
    newdata = *newdata_spot;


    // Read the base model header...
    num_vertex = sdf_read_unsigned_short(olddata);  olddata+=4;
    num_joint = sdf_read_unsigned_short(olddata);  olddata+=2;
    num_bone = sdf_read_unsigned_short(olddata);  olddata+=2;


    if(detail_level == 0)
    {
        global_base_num_vertex = sdf_read_unsigned_short(olddata-8);
        global_base_num_tex_vertex = sdf_read_unsigned_short(olddata-6);


        // Copy the vertex coordinates, tex_vertex coordinates, and triangle lists into a big chunk of data...
        // This is for the simplify routine...
        global_base_old_vertex_start = olddata;
        tempdata = subbuffer;
        repeat(i, global_base_num_vertex)
        {
            x = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
            z = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
            *((float*) tempdata) = x;  tempdata+=4;
            *((float*) tempdata) = y;  tempdata+=4;
            *((float*) tempdata) = z;  tempdata+=4;
            olddata+=3;  // Skip extra junk for now
            tempdata+=18;  // Extra room for normal (12), weight (4), and neighbor count (2)
            // Fill in original vertex (2)
            *((unsigned short*) tempdata) = i;
            tempdata+=2;
        }
        global_base_old_tex_vertex_start = olddata;
        repeat(i, global_base_num_tex_vertex)
        {
            x = ((float) ((signed short) sdf_read_unsigned_short(olddata))) / 256.0f;  olddata+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(olddata))) / 256.0f;  olddata+=2;
            *((float*) tempdata) = x;  tempdata+=4;
            *((float*) tempdata) = y;  tempdata+=4;
        }
        global_base_old_texture_start = olddata;
        repeat(i, MAX_DDD_TEXTURE)
        {
            *tempdata = *olddata; tempdata++;
            if(*olddata != 0)   // Mode
            {
                olddata+=3;  // Skip mode, flags, alpha
                num_triangle = sdf_read_unsigned_short(olddata);  olddata+=2;
                *((unsigned short*) tempdata) = num_triangle;  tempdata+=2;  // Number of strips
                repeat(j, num_triangle)
                {
                    *((unsigned short*) tempdata) = 3;  tempdata+=2;  // Number of points in strip
                    repeat(k, 3)
                    {
                        vertex = sdf_read_unsigned_short(olddata);  olddata+=2;
                        tex_vertex = sdf_read_unsigned_short(olddata);  olddata+=2;
                        *((unsigned short*) tempdata) = vertex;  tempdata+=2;
                        *((unsigned short*) tempdata) = tex_vertex;  tempdata+=2;
                    }
                }
                *((unsigned short*) tempdata) = 0;  tempdata+=2;  // Number of fans
            }
            else
            {
                olddata++;
            }
        }
        global_base_old_joint_start = olddata;
        global_base_data_size = ((unsigned int) tempdata)-((unsigned int) subbuffer);
    }







    // Simplify the geometry, according to the detail level...
    // Number of vertices and tex_vertices may change in simplification...  Number of triangles too...
    if(detail_level == 1)
    {
        // Prime the simplifier...
        ddd_simplify_geometry(&global_base_num_vertex, &global_base_num_tex_vertex, subbuffer, (unsigned char) (255-(detail_level*6)), global_base_data_size, MAX_DDD_TEXTURE);
    }
    if(detail_level > 1)
    {
        // Continue simplification, reusing data from last run... 
        ddd_continue_simplify(&global_base_num_vertex, &global_base_num_tex_vertex, subbuffer, (unsigned char) (255-(detail_level*6)), global_base_data_size, MAX_DDD_TEXTURE, num_vertex);
    }





    // Optimize triangle mesh into strips and fans...  Fans are no longer supported...
    ddd_strip_fan_geometry(global_base_num_vertex, global_base_num_tex_vertex, subbuffer, thirdbuffer, fourthbuffer, MAX_DDD_TEXTURE);
    // Third buffer should now hold connection information for triangles...  32 bytes each...
    // Fourth buffer should now hold final triangle data



    // Write the header for the new, simplified base model...
    *newindex_spot = newdata - ((unsigned int) newdata_start);  newindex_spot++;
    *((unsigned short*) newdata) = global_base_num_vertex;  newdata+=2;
    *((unsigned short*) newdata) = global_base_num_tex_vertex;  newdata+=2;
    *((unsigned short*) newdata) = num_joint;  newdata+=2;
    *((unsigned short*) newdata) = num_bone;  newdata+=2;


    // Write the new, simplified vertex data
    tempdata = subbuffer;
    repeat(i, global_base_num_vertex)
    {
        x = *((float*) tempdata);  tempdata+=4;
        y = *((float*) tempdata);  tempdata+=4;
        z = *((float*) tempdata);  tempdata+=4;
        *((float*) newdata) = x;  newdata+=4;
        *((float*) newdata) = y;  newdata+=4;
        *((float*) newdata) = z;  newdata+=4;
        // Ignore normal, weight, and neighbor count
        tempdata+=18;
        // Use the original vertex to get bone bindings and weight...
        original_vertex = *((unsigned short*) tempdata);
        tempdata+=2;
        olddata = global_base_old_vertex_start + (original_vertex*9) + 6;
        *newdata = *olddata;  newdata++;  olddata++;  // Bone binding 0
        *newdata = *olddata;  newdata++;  olddata++;  // Bone binding 1
//log_message("INFO:   DDD Weight %d == %d", original_vertex, (*olddata));
        *newdata = *olddata;  newdata++;              // Bone Weighting


        // Bone and normal scalars calculated later...
        repeat(j, 12)
        {
            *((float*) newdata) = 0.0f;  newdata+=4;
        }


        // Pad vertex data, so each is 64 bytes long...  Also used for hide flag in modeler...
        *newdata = 0;  newdata++;
    }


    // Write the new, simplified texture vertex data
    *newindex_spot = newdata - ((unsigned int) newdata_start);  newindex_spot++;
    repeat(i, global_base_num_tex_vertex)
    {
        x = *((float*) tempdata);  tempdata+=4;
        y = *((float*) tempdata);  tempdata+=4;
        *((float*) newdata) = x;  newdata+=4;
        *((float*) newdata) = y;  newdata+=4;
    }



    // Write the new, simplified texture/strip/fan data
    *newindex_spot = newdata - ((unsigned int) newdata_start);  newindex_spot++;
    olddata = global_base_old_texture_start;
    triangle_data_start = newdata;
    repeat(i, MAX_DDD_TEXTURE)
    {
        // Was this texture on or off?
        if(*olddata != 0)
        {
            // Texture used to be on...  Get it's info...
            // On...  Write texture header...
            olddata++;
            flags = *olddata;  olddata++;
            alpha = *olddata;  olddata++;
            olddata+= (sdf_read_unsigned_short(olddata) * 12);  olddata+=2;
        }
        else
        {
            // Was off...  Should still be...
            olddata++;
        }



        // Is it currently on?
        mode = *tempdata;  tempdata++;
        *newdata = mode;  newdata++;    // Mode
        if(mode != 0)
        {
            // On...  Write texture header...
            *newdata = flags;  newdata++;   // Flags
            *newdata = alpha;  newdata++;   // Alpha

            // Skip ignored stuff in tempdata...  Just so we don't mess up...
            num_triangle = *((unsigned short*) tempdata);  tempdata+=2;
            repeat(j, num_triangle)
            {
                num_points_in_prim = *((unsigned short*) tempdata);  tempdata+=2;  // Number of points in strip
                repeat(k, num_points_in_prim)
                {
                    tempdata+=4;
                }
            }
            tempdata+=2;  // Skip fans...


            // Fill in striperized data...  Fans taken out...
            // First skip through data until we get to our desired texture...
            strip_fan_data = fourthbuffer;
            if(i > 0)
            {
                while(strip_fan_data[0] != 0 && strip_fan_data[1] < i)
                {
                    strip_fan_data+=2;
                    b = *((unsigned short*) strip_fan_data);  // vertex_count
                    strip_fan_data+=2;
                    strip_fan_data+=(b<<2);
                }
            }


            // Now count all of the strips in the texture...
            num_triangle = 0;
            strip_fan_data_start = strip_fan_data;
            while(strip_fan_data[0] != 0 && strip_fan_data[1] == i)
            {
                num_triangle++;
                strip_fan_data+=2;
                b = *((unsigned short*) strip_fan_data);  // vertex_count
                strip_fan_data+=2;
                strip_fan_data+=(b<<2);
            }


            // Now go back through and copy the data...
            *((unsigned short*) newdata) = num_triangle;  newdata+=2;
            strip_fan_data = strip_fan_data_start;
            repeat(j, num_triangle)
            {
                strip_fan_data+=2;
                b = *((unsigned short*) strip_fan_data);  // vertex_count
#ifdef VERBOSE_COMPILE
    log_message("INFO:   Copied strip %d, Contains %d triangles...", j, b-2);
#endif
                memcpy(newdata, strip_fan_data, (b<<2)+2);
                newdata+=(b<<2)+2;
                strip_fan_data+=(b<<2)+2;
            }


            // Put fans down as 0, since they've been removed...
            *((unsigned short*) newdata) = 0;  newdata+=2;
        }
    }



    // Copy the old joint data
    *newindex_spot = newdata - ((unsigned int) newdata_start);  newindex_spot++;
    olddata = global_base_old_joint_start;
    repeat(i, num_joint)
    {
        *((float*) newdata) = (*olddata)*JOINT_COLLISION_SCALE;  newdata+=4;  olddata++;
    }




    // Copy the old bone data
    *newindex_spot = newdata - ((unsigned int) newdata_start);  newindex_spot++;
    repeat(i, num_bone)
    {
        *newdata = *olddata;  newdata++;  olddata++;
        k = sdf_read_unsigned_short(olddata);  olddata+=2;
        b = sdf_read_unsigned_short(olddata);  olddata+=2;
        *((unsigned short*) newdata) = k;  newdata+=2;
        *((unsigned short*) newdata) = b;  newdata+=2;
        newdata+=4;  // Reserve room for the bone length
    }



    // Generate the cartoon line data...  Start by assuming there aren't any...
    *((unsigned short*) newdata) = 0;



    // Old cartoon line code that finds triangle edges...  Now used for volumetric shadows...
    // For each texture...
//#ifndef BACKFACE_CARTOON_LINES
    tempdata = triangle_data_start;
    repeat(i, MAX_DDD_TEXTURE)
    {
        // If texture is on...
        mode = *tempdata;  tempdata++;
        if(mode)
        {
            // Does this texture have a silhouette?
            silhouette_flag = !((*tempdata) & NO_LINE_FLAG);  tempdata++;
            tempdata++;


            // For strips and fans...
            repeat(b, 2)
            {
                // For each triangle strip or fan...
                num_prim = *((unsigned short*) (tempdata));  tempdata+=2;
                repeat(j, num_prim)
                {
                    num_points_in_prim = *((unsigned short*) (tempdata));  tempdata+=2;
                    repeat(k, num_points_in_prim)
                    {
                        triangle_vertex[k%3] = *((unsigned short*) (tempdata));  tempdata+=4;

                        // Add each edge of triangle
                        if(silhouette_flag && k > 1)
                        {
                            if(k & 1)
                            {
                                add_cartoon_line(triangle_vertex[2], triangle_vertex[1], triangle_vertex[0], FALSE, (unsigned short*) newdata);
                                add_cartoon_line(triangle_vertex[1], triangle_vertex[0], triangle_vertex[2], FALSE, (unsigned short*) newdata);
                                add_cartoon_line(triangle_vertex[0], triangle_vertex[2], triangle_vertex[1], FALSE, (unsigned short*) newdata);
                            }
                            else
                            {
                                add_cartoon_line(triangle_vertex[0], triangle_vertex[1], triangle_vertex[2], FALSE, (unsigned short*) newdata);
                                add_cartoon_line(triangle_vertex[1], triangle_vertex[2], triangle_vertex[0], FALSE, (unsigned short*) newdata);
                                add_cartoon_line(triangle_vertex[2], triangle_vertex[0], triangle_vertex[1], FALSE, (unsigned short*) newdata);
                            }
                        }
                    }
                }
            }
        }
    }
    remove_cartoon_lines((unsigned short*) newdata);
//#endif


    // Skip over cartoon data...
    newdata += (*((unsigned short*) newdata)) << 3;
    newdata += 2;



    // Update the file positions in the main function...
    if(detail_level == (DETAIL_LEVEL_MAX-1))
    {
        // Done with this base model...
        *olddata_spot = olddata;
    }
    *newdata_spot = newdata;
}

//-----------------------------------------------------------------------------------------------
void ddd_decode_bone_frame(unsigned char** olddata_spot, unsigned char** newdata_spot, unsigned char** newindex_spot, unsigned char* newdata_start, float scale)
{
    // <ZZ> This function helps out decode_ddd...
    unsigned char* olddata;
    unsigned char* newdata;
    unsigned char* base_model_start;
    unsigned char* bone_data;
    unsigned char* joint_position_data;
    unsigned char action_name;
    unsigned char action_flags;
    unsigned char base_model;
    unsigned char alpha;
    unsigned short num_vertex, num_bone, num_joint;
    unsigned short i, j;
    unsigned short joint;
    float front_xyz[3];
    float bone_xyz[3];
    float side_xyz[3];
    float x, y, z, distance;
    signed short offset;


    // Get the file positions from the main function...
    olddata = *olddata_spot;
    newdata = *newdata_spot;
    *newindex_spot = newdata - ((unsigned int) newdata_start);


    // Read the bone frame header...
    action_name  = *olddata;  olddata++;
      *newdata = action_name;  newdata++;
    action_flags = *olddata;  olddata++;
      *newdata = action_flags;  newdata++;
    base_model   = *olddata;  olddata++;
      *newdata = base_model;  newdata++;
    offset = (signed short) sdf_read_unsigned_short(olddata);  olddata+=2;
      *((float*) newdata) = offset / 256.0f;  newdata+=4;
    offset = (signed short) sdf_read_unsigned_short(olddata);  olddata+=2;
      *((float*) newdata) = offset / 256.0f;  newdata+=4;


    // Figure out how many bones and joints by looking at the base model...
    base_model_start = newdata_start + 6 + (ACTION_MAX<<1) + (MAX_DDD_SHADOW_TEXTURE) + (base_model*20);
    base_model_start = (*((unsigned int*) base_model_start)) + newdata_start;
    num_vertex = *((unsigned short*) base_model_start);  base_model_start+=4;
    num_joint = *((unsigned short*) base_model_start);  base_model_start+=2; 
    num_bone = *((unsigned short*) base_model_start);  base_model_start+=2;


    bone_data = newdata_start + 6 + (ACTION_MAX<<1) + (MAX_DDD_SHADOW_TEXTURE) + (base_model*20) + 16;
    bone_data = (*((unsigned int*) bone_data)) + newdata_start;




    // Copy the bone info
    joint_position_data = olddata+(num_bone*6);
    repeat(i, num_bone)
    {
        // Forward normal
        x = (float) ((signed short) sdf_read_unsigned_short(olddata));  olddata+=2;
        y = (float) ((signed short) sdf_read_unsigned_short(olddata));  olddata+=2;
        z = (float) ((signed short) sdf_read_unsigned_short(olddata));  olddata+=2;
//        x = ((float) (*(signed char*) olddata));  olddata++;
//        y = ((float) (*(signed char*) olddata));  olddata++;
//        z = ((float) (*(signed char*) olddata));  olddata++;
        distance = (float) sqrt(x*x + y*y + z*z);
        if(distance > 0.001f)
        {
            x = x/distance;  y = y/distance;  z = z/distance;
        }
        else
        {
            x = 1.0f;  y = 0.0f;  z = 0.0f;
        }
        *((float*) newdata) = x;  newdata+=4;
        *((float*) newdata) = y;  newdata+=4;
        *((float*) newdata) = z;  newdata+=4;
        front_xyz[X] = x;
        front_xyz[Y] = y;
        front_xyz[Z] = z;


        // Bone vector
        joint = *((unsigned short*) (bone_data + (i*9) + 3));
        bone_xyz[X] = ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6)))) * scale;
        bone_xyz[Y] = ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6) + 2))) * scale;
        bone_xyz[Z] = ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6) + 4))) * scale;
        joint = *((unsigned short*) (bone_data + (i*9) + 1));
        bone_xyz[X] -= ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6)))) * scale;
        bone_xyz[Y] -= ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6) + 2))) * scale;
        bone_xyz[Z] -= ((float) ((signed short) sdf_read_unsigned_short(joint_position_data + (joint*6) + 4))) * scale;


        // Calculate side normal
        cross_product(front_xyz, bone_xyz, side_xyz);
        distance = vector_length(side_xyz);
        if(distance > 0.001f)
        {
            x = side_xyz[X]/distance;  y = side_xyz[Y]/distance;  z = side_xyz[Z]/distance;
        }
        else
        {
            x = 0.0f;  y = 1.0f;  z = 0.0f;
        }


        *((float*) newdata) = x;  newdata+=4;
        *((float*) newdata) = y;  newdata+=4;
        *((float*) newdata) = z;  newdata+=4;
    }
    // Copy the joint info
    repeat(i, num_joint)
    {
        x = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
        y = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
        z = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
        *((float*) newdata) = x;  newdata+=4;
        *((float*) newdata) = y;  newdata+=4;
        *((float*) newdata) = z;  newdata+=4;
    }
    // Copy the shadow info
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        alpha = *olddata;  olddata++;
            *newdata = alpha;  newdata++;
        if(alpha > 0)
        {
            repeat(j, 4)
            {
                x = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
                y = ((float) ((signed short) sdf_read_unsigned_short(olddata))) * scale;  olddata+=2;
                *((float*) newdata) = x;  newdata+=4;
                *((float*) newdata) = y;  newdata+=4;
            }
        }
    }


    // Update the file positions in the main function...
    *olddata_spot = olddata;
    *newdata_spot = newdata;
}

//-----------------------------------------------------------------------------------------------
signed char decode_ddd(unsigned char* index, unsigned char* filename)
{
    // <ZZ> This function decompresses a ddd file that has been stored in memory.  Index is a
    //      pointer to the start of the file's index in sdf_index, and can be gotten from
    //      sdf_find_index.  If the function works okay, it should create a new RDY file in the
    //      index and return TRUE.  It might also delete the original compressed file to save
    //      space, but that's a compile time option.  If it fails it should return FALSE, or
    //      it might decide to crash.
    unsigned char* old_data_start;
    unsigned char* data;    // Compressed
    unsigned int size;      // Compressed
    unsigned char* newdata; // Decompressed
    unsigned int newsize;   // Decompressed
    unsigned int actualsize;
    int i, j, k;
    float scale;
    unsigned short flags;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char** new_base_model_offset_start;
    unsigned char** new_bone_frame_offset_start;
    unsigned char external_bone_frame;
    unsigned char linkname[16];
    unsigned char* link_data;
    unsigned char temp;


    // Log what we're doing
    #ifdef VERBOSE_COMPILE
        log_message("INFO:     Decoding %s.DDD to %s.RDY", filename, filename);
    #endif


    // Find the location of the file data, and its size...
    data = (unsigned char*) sdf_read_unsigned_int(index);
    size = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;
    old_data_start = data;
//global_ddd_file_start = data;
//global_rdy_file_start = NULL;


    // Make sure we have room in the index for a new file
    if(sdf_extra_files <= 0)
    {
        log_message("ERROR:  No room left to add file, program must be restarted");
        return FALSE;
    }


    // Read the DDD header...  Write the RDY header to the mainbuffer...
    newdata = mainbuffer;
    scale = sdf_read_unsigned_short(data) / DDD_SCALE_WEIGHT;  data+=2;
        // RDY doesn't need scale info...
    flags = sdf_read_unsigned_short(data);  data+=2;
        external_bone_frame = FALSE;
        if(flags & DDD_EXTERNAL_BONE_FRAMES)
        {
            external_bone_frame = TRUE;
        }
        *((unsigned short*) newdata) = flags;  newdata+=2;
    data++;
        *(newdata) = DETAIL_LEVEL_MAX;  newdata++;
    num_base_model = *data;  data++;
        *(newdata) = num_base_model;  newdata++;
    num_bone_frame = sdf_read_unsigned_short(data);  data+=2;
        *((unsigned short*) newdata) = num_bone_frame;  newdata+=2;
    // Starting frames for each action...
    repeat(i, ACTION_MAX)
    {
        // Not stored in file...
        *((unsigned short*) newdata) = 65535;  newdata+=2;
    }
    // Shadow texture indices
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        *newdata = *data;  data++;  newdata++;
    }
    // External bone frame filename (skipped for now)
    if(external_bone_frame)
    {
        // Actually...  Let's figger out how many frames are in our link'd file...
        temp = data[8];
        data[8] = 0;
        sprintf(linkname, "%s", data);
        data[8] = temp;
        link_data = sdf_find_filetype(linkname, SDF_FILE_IS_DDD);
        num_bone_frame = 0;
        if(link_data)
        {
            link_data = (unsigned char*) sdf_read_unsigned_int(link_data);
            link_data+=6;
            num_bone_frame = sdf_read_unsigned_short(link_data);
        }

        // Write the number of bone frames...
        *((unsigned short*) (newdata - MAX_DDD_SHADOW_TEXTURE - (ACTION_MAX * 2) - 2)) = num_bone_frame;


        data+=8;
    }
    // Offsets...  Fill in later...
    new_base_model_offset_start = (unsigned char**) newdata;
    repeat(i, DETAIL_LEVEL_MAX)
    {
        repeat(j, num_base_model)
        {
            *((unsigned int*) newdata) = 0;  newdata+=4;
            *((unsigned int*) newdata) = 0;  newdata+=4;
            *((unsigned int*) newdata) = 0;  newdata+=4;
            *((unsigned int*) newdata) = 0;  newdata+=4;
            *((unsigned int*) newdata) = 0;  newdata+=4;
        }
    }
    new_bone_frame_offset_start = (unsigned char**) newdata;
    repeat(i, num_bone_frame)
    {
        *((unsigned int*) newdata) = 0;  newdata+=4;
    }



    // Done with header, now decode base models...  For each detail level...
    repeat(i, num_base_model)
    {
        repeat(j, DETAIL_LEVEL_MAX)
        {
#ifdef VERBOSE_COMPILE
    log_message("INFO:   Decoding base %d, detail %d", i, j);
#endif
            ddd_decode_base_model(&data, &newdata, new_base_model_offset_start+((5*i)+(5*num_base_model*j)), mainbuffer, (unsigned char) j, scale);
        }
    }



    // Decode bone frames...  Fills in pointer block...  Filled in later for external linking...
    if(!external_bone_frame)
    {
        repeat(i, num_bone_frame)
        {
            ddd_decode_bone_frame(&data, &newdata, new_bone_frame_offset_start, mainbuffer, scale);
            new_bone_frame_offset_start++;
        }
    }





    // Allocate memory for the new file...
    newsize = ((unsigned int) newdata) - ((unsigned int) mainbuffer);
    actualsize = newsize;
    #ifdef DEVTOOL
        // Allocate extra memory for the modeler...
        if(newsize < (unsigned int) sdf_dev_size[SDF_FILE_IS_RDY])
        {
            actualsize = sdf_dev_size[SDF_FILE_IS_RDY];
        }
    #endif
    newdata = malloc(actualsize);
    if(newdata)
    {
        // Do we create a new index?
        index = sdf_get_new_index();

        // Write the index...
        sdf_write_unsigned_int(index, (unsigned int) newdata);
//global_rdy_file_start = newdata;
        sdf_write_unsigned_int(index+4, newsize);
        *(index+4) = SDF_FILE_IS_RDY | SDF_FLAG_WAS_UPDATED;
        repeat(j, 8) { *(index+8+j) = 0; }
        memcpy(index+8, filename, strlen(filename));
    }
    else
    {
        log_message("ERROR:  Not enough memory to decompress");
        return FALSE;
    }


    // Copy the mainbuffer data into the new file...
    memcpy(newdata, mainbuffer, newsize);


    // Go back through and turn offsets into pointers...
    data = newdata;
    data+=6+(ACTION_MAX<<1)+(MAX_DDD_SHADOW_TEXTURE);
    repeat(i, DETAIL_LEVEL_MAX)
    {
        repeat(j, num_base_model)
        {
            repeat(k, 5)
            {
                *((unsigned int*) data) += (unsigned int) newdata;  data+=4;
            }
        }
    }
    if(!external_bone_frame)
    {
        repeat(i, num_bone_frame)
        {
            *((unsigned int*) data) += (unsigned int) newdata;  data+=4;
        }
    }


//    datadump(newdata, newsize, FALSE);
    return TRUE;
}

//-----------------------------------------------------------------------------------------------
void ddd_magic_update_thing(unsigned char mask)
{
    // <ZZ> This function generates action lists for all of the RDY models, and also fills in
    //      frame pointers for externally linked files.  Should be called directly after all of
    //      the DDD files are decoded, and also after any file transfer.  Also does lots of math
    //      to pregenerate normals for each vertex of each base model of each detail level of each
    //      RDY file...
    int i;
    unsigned short frame, num_base_model;
    unsigned char detail_level;
    char filename[16];
    char linkname[16];
    char filetype;
    char temp;
    unsigned short flags;
    unsigned char* data;
    unsigned char* start_data;
    unsigned char* ddd_data;
    unsigned char* link_data;
    unsigned short num_bone_frame;
    unsigned char* index;



    #ifdef VERBOSE_COMPILE
        log_message("INFO:   Magic update called");
    #endif


    // For each file in index...
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        // Let me choose if I want all files or just updated ones...
        if( *(index+4) & mask)
        {
            // If the file is an RDY file...
            sdf_get_filename(i, filename, &filetype);
            if((filetype & 15) == SDF_FILE_IS_RDY)
            {
                // Set up frame pointers if the file is externally linked...
                data = (unsigned char*) sdf_read_unsigned_int(index);
                start_data = data;
                flags = *((unsigned short*) data);
                if(flags & DDD_EXTERNAL_BONE_FRAMES)
                {
                    // Find the name of the referenced file...
                    ddd_data = sdf_find_filetype(filename, SDF_FILE_IS_DDD);
                    if(ddd_data)
                    {
                        ddd_data = (unsigned char*) sdf_read_unsigned_int(ddd_data);
                        temp = ddd_data[20];
                        ddd_data[20] = 0;
                        sprintf(linkname, "%s", ddd_data+12);
                        ddd_data[20] = temp;
                        link_data = sdf_find_filetype(linkname, SDF_FILE_IS_RDY);
                        if(link_data)
                        {
                            // Copy the frame list from one file to the other...
                            link_data = (unsigned char*) sdf_read_unsigned_int(link_data);
                            num_bone_frame = *((unsigned short*) (data+4));
                            data+=data[3]*DETAIL_LEVEL_MAX*20;
                            data+=6;
                            data+=(ACTION_MAX<<1);
                            data+=(MAX_DDD_SHADOW_TEXTURE);
                            link_data+=link_data[3]*DETAIL_LEVEL_MAX*20;
                            link_data+=6;
                            link_data+=(ACTION_MAX<<1);
                            link_data+=(MAX_DDD_SHADOW_TEXTURE);
                            memcpy(data, link_data, num_bone_frame<<2);
                        }
                        else
                        {
                            log_message("ERROR:  Couldn't link %s.RDY to %s.RDY...  File doesn't exist...", filename, linkname);
                        }
                    }
                    else
                    {
                        log_message("ERROR:  Couldn't find %s.DDD...  Weird...", filename);
                    }
                }

                // Figure out the action list...
                ddd_generate_model_action_list(start_data);


                // Figure out the starting normals
                num_base_model = start_data[3];
                repeat(detail_level, DETAIL_LEVEL_MAX)
                {
                    // Assume that the first several frames are used for boning models...
                    repeat(frame, num_base_model)
                    {
                        render_pregenerate_normals(start_data, frame, detail_level);
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
