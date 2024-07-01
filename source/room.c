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

//-----------------------------------------------------------------------------------------------
// <ZZ> This file contains functions for drawing & editing & otherwise working with the new room
//      format...
//-----------------------------------------------------------------------------------------------
#define EXTERIOR_VERTEX             0
#define EXTERIOR_TEX_VERTEX         1
#define EXTERIOR_TRIANGLE           2
unsigned short global_num_vertex = 0;
unsigned short global_num_tex_vertex = 0;
unsigned short global_num_triangle = 0;
unsigned short global_room_active_group = 0;

#define ROOM_CEILING_Z  20.0f
#define ROOM_CEILING_BUMP_Z 18.0f
#define ROOM_HEIGHTMAP_Z  40.0f
#define ROOM_HEIGHTMAP_SCALE 0.00244140625f // ROOM_HEIGHTMAP_Z / 16384...  for the heightmap return function...
#define ROOM_HEIGHTMAP_SIZE 1024            // Heightmap is 2048x2048, 16 values per square foot
#define ROOM_HEIGHTMAP_PRECISION 4.0f       // Each foot of room is sampled this many times (square for a square foot)
#define ROOM_HEIGHTMAP_BIAS      0.125f     // Should be 0.5/precision...  Used to help out with rounding...
#define ROOM_HEIGHTMAP_BORDER 64            // Size of border on any side...
#define ROOM_PIT_HIGH_LEVEL -14.0f
#define ROOM_PIT_HURT_LEVEL -28.0f
#define ROOM_PIT_LOW_LEVEL -30.0f

#define ONE_OVER_512 0.001953125f
#define ONE_OVER_256 0.00390625f
#define MAX_ROOM_TEXTURE 32

#define WALL_TEXTURE_SCALE   0.050f   // Wall texture repeats every 20ft...
//#define WALL_TEXTURE_SCALE   0.0625f   // Wall texture repeats every 16ft...
#define MINIMUM_WALL_LENGTH  0.2500f    // Don't allow short walls...

// Exterior wall flags...
#define ROOM_WALL_FLAG_LOW_NORMAL_WALL   0
#define ROOM_WALL_FLAG_LOW_NO_TRIM_WALL  1
#define ROOM_WALL_FLAG_LOW_NO_WALL       2
#define ROOM_WALL_FLAG_LOW_DOOR          3
#define ROOM_WALL_FLAG_LOW_SHUTTER       4
#define ROOM_WALL_FLAG_LOW_PASSAGE       5
#define ROOM_WALL_FLAG_LOW_BOSS_DOOR     6
#define ROOM_WALL_FLAG_LOW_CRACKED_DOOR  7
#define ROOM_WALL_FLAG_LOW_SECRET_DOOR   8


// Room texture flags...
#define ROOM_TEXTURE_FLAG_NOLINE     1
#define ROOM_TEXTURE_FLAG_NOHEIGHT   2
#define ROOM_TEXTURE_FLAG_FLOORTEX   4
#define ROOM_TEXTURE_FLAG_NODRAW     8
#define ROOM_TEXTURE_FLAG_ALPHATRANS 16
#define ROOM_TEXTURE_FLAG_NOSHADOW   32
#define ROOM_TEXTURE_FLAG_PAPERDOLL  64
#define ROOM_TEXTURE_FLAG_SMOOTH     128

//-----------------------------------------------------------------------------------------------
#define MAX_ROOM_WELDABLE_VERTEX 128
#define ROOM_WELDABLE_DISTANCE_TOLERANCE 100.0f  // Square of actual distance tolerance...
unsigned short room_weldable_vertex_list[MAX_ROOM_WELDABLE_VERTEX];
unsigned short room_weldable_vertex_segment[MAX_ROOM_WELDABLE_VERTEX];
float          room_weldable_intersection_info[MAX_ROOM_WELDABLE_VERTEX][7];
unsigned short num_room_weldable_vertex;
#define MAX_DDD_WELDABLE_VERTEX 128
#define MAX_DDD_WELDABLE_BASE_MODEL 16
#define DDD_NO_WELD    0
#define DDD_LEFT_WELD  1
#define DDD_RIGHT_WELD 2
unsigned short ddd_vertex_translation[MAX_DDD_WELDABLE_VERTEX];
float          ddd_vertex_percent[MAX_DDD_WELDABLE_VERTEX];


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
#define MAX_ROOM_SELECT 8192
unsigned char plopping_bridge = FALSE;
unsigned short room_select_num = 0;
unsigned short room_select_list[MAX_ROOM_SELECT];
unsigned short room_select_index = 0;
float room_select_xyz[MAX_ROOM_SELECT][3];
unsigned char* room_select_data[MAX_ROOM_SELECT];
unsigned short room_select_flag[MAX_ROOM_SELECT];
unsigned char room_select_axes[MAX_ROOM_SELECT];
unsigned short room_autotrim_select_count[3];
#define room_select_clear() { room_select_num = 0; }
void room_select_add(unsigned short item, unsigned char* item_xyz, unsigned char axes)
{
    // <ZZ> Adds an item to the selection list...
    if(room_select_num < MAX_ROOM_SELECT)
    {
        room_select_list[room_select_num] = item;
        if(axes > 0)
        {
            room_select_xyz[room_select_num][X] = ((signed short) sdf_read_unsigned_short(item_xyz)) * ONE_OVER_256;
        }
        if(axes > 1)
        {
            room_select_xyz[room_select_num][Y] = ((signed short) sdf_read_unsigned_short(item_xyz+2)) * ONE_OVER_256;
        }
        if(axes > 2)
        {
            room_select_xyz[room_select_num][Z] = ((signed short) sdf_read_unsigned_short(item_xyz+4)) * ONE_OVER_256;
        }
        room_select_data[room_select_num] = item_xyz;
        room_select_axes[room_select_num] = axes;
        room_select_flag[room_select_num] = FALSE;
        room_select_num++;
    }
}
unsigned char room_select_inlist(unsigned short item)
{
    // <ZZ> Returns TRUE if the given item has already been added to the selection list.
    //      Also sets room_select_index to the index of that item.
    unsigned short i;

    repeat(i, room_select_num)
    {
        if(room_select_list[i] == item)
        {
            room_select_index = i;
            return TRUE;
        }
    }
    room_select_index = 0;
    return FALSE;
}
void room_select_remove(unsigned short item)
{
    // <ZZ> Removes an item from the selection list
    if(item < room_select_num)
    {
        while(item < (room_select_num-1))
        {
            room_select_list[item] = room_select_list[item+1];
            room_select_xyz[item][X] = room_select_xyz[item+1][X];
            room_select_xyz[item][Y] = room_select_xyz[item+1][Y];
            room_select_xyz[item][Z] = room_select_xyz[item+1][Z];
            room_select_data[item] = room_select_data[item+1];
            room_select_axes[item] = room_select_axes[item+1];
            room_select_flag[item] = room_select_flag[item+1];
            item++;
        }
        room_select_num--;
    }
}
void room_select_update_xyz(void)
{
    // <ZZ> Reloads xyz values from actual data
    unsigned short i;
    unsigned char axes;
    repeat(i, room_select_num)
    {
        axes = room_select_axes[i];
        if(axes > 0)
        {
            room_select_xyz[i][X] = ((signed short) sdf_read_unsigned_short(room_select_data[i])) * ONE_OVER_256;
        }
        if(axes > 1)
        {
            room_select_xyz[i][Y] = ((signed short) sdf_read_unsigned_short(room_select_data[i]+2)) * ONE_OVER_256;
        }
        if(axes > 2)
        {
            room_select_xyz[i][Z] = ((signed short) sdf_read_unsigned_short(room_select_data[i]+4)) * ONE_OVER_256;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
void room_weldable_vertex_clear(void)
{
    // <ZZ> This function clears the list of yet-to-be-welded vertices in the room...
//log_message("INFO:   Cleared room weldable list");
    num_room_weldable_vertex = 0;
}

//-----------------------------------------------------------------------------------------------
void room_weldable_vertex_add(unsigned short vertex, unsigned short segment, float* intersection_info)
{
    // <ZZ> This function adds a vertex to the list of yet-to-be-welded vertices in the room...
    //      Segment tells us which wall segment it's in...
    if(num_room_weldable_vertex < MAX_ROOM_WELDABLE_VERTEX)
    {
//log_message("INFO:   Assigned vertex %d (segment %d) as weldable", vertex, segment);
        room_weldable_vertex_list[num_room_weldable_vertex] = vertex;
        room_weldable_vertex_segment[num_room_weldable_vertex] = segment;
        room_weldable_intersection_info[num_room_weldable_vertex][0] = intersection_info[0];  // Start x
        room_weldable_intersection_info[num_room_weldable_vertex][1] = intersection_info[1];  // Start y
        room_weldable_intersection_info[num_room_weldable_vertex][2] = intersection_info[2];  // Vector x
        room_weldable_intersection_info[num_room_weldable_vertex][3] = intersection_info[3];  // Vector y
        room_weldable_intersection_info[num_room_weldable_vertex][4] = intersection_info[4];  // Vertex x in model
        room_weldable_intersection_info[num_room_weldable_vertex][5] = intersection_info[5];  // Vertex y in model
        room_weldable_intersection_info[num_room_weldable_vertex][6] = intersection_info[6];  // Vertex z in model
        num_room_weldable_vertex++;
    }
}

//-----------------------------------------------------------------------------------------------
void room_weldable_vertex_remove(unsigned short vertex)
{
    // <ZZ> This function removes a vertex from the list of yet-to-be-welded vertices in the room...
    unsigned short i;

    repeat(i, num_room_weldable_vertex)
    {
        if(room_weldable_vertex_list[i] == vertex)
        {
//log_message("INFO:   Unassigned vertex %d as weldable", vertex);
            // We found the one we're lookin' for...
            num_room_weldable_vertex--;
            while(i < num_room_weldable_vertex)
            {
                room_weldable_vertex_list[i] = room_weldable_vertex_list[i+1];
                room_weldable_vertex_segment[i] = room_weldable_vertex_segment[i+1];
                room_weldable_intersection_info[i][0] = room_weldable_intersection_info[i+1][0];
                room_weldable_intersection_info[i][1] = room_weldable_intersection_info[i+1][1];
                room_weldable_intersection_info[i][2] = room_weldable_intersection_info[i+1][2];
                room_weldable_intersection_info[i][3] = room_weldable_intersection_info[i+1][3];
                room_weldable_intersection_info[i][4] = room_weldable_intersection_info[i+1][4];
                room_weldable_intersection_info[i][5] = room_weldable_intersection_info[i+1][5];
                room_weldable_intersection_info[i][6] = room_weldable_intersection_info[i+1][6];
                i++;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void intersect_xy_lines(float* info_a, float* info_b, float* write_xy)
{
    // <ZZ> This function figgers out the xy coordinates of where two xy lines intersect...
    //      The info blocks should be 4 floats long...  Start x, start y, vector x, vector y...
    //      Sets 2 values of write_xy if it worked...
    float denominator;
    float percent;

//log_message("INFO:   Trying to intersect two lines...");
//log_message("INFO:     Line A from   (%f, %f)", info_a[X], info_a[Y]);
//log_message("INFO:     Line A vector (%f, %f)", info_a[2], info_a[3]);
//log_message("INFO:     Line B from   (%f, %f)", info_b[X], info_b[Y]);
//log_message("INFO:     Line B vector (%f, %f)", info_b[2], info_b[3]);


//    normal_xy[X] = (-info_a[3]);
//    normal_xy[Y] = (info_a[2]);
//    offset_xy[X] = (info_a[X] - (info_b[X] + (info_b[2]*percent)));
//    offset_xy[Y] = (info_a[Y] - (info_b[Y] + (info_b[3]*percent)));
//    find percent where....
//        (offset_xy[X] * normal_xy[X]) + (offset_xy[Y] * normal_xy[Y]) == 0
//    percent = (-info_a[X]*info_a[3]+info_b[X]*info_a[3]+info_a[Y]*info_a[2]-info_b[Y]*info_a[2]) / (info_b[3]*info_a[2] - info_b[2]*info_a[3])



    // Find intersection point by finding point where dot product == 0...
    denominator = (info_b[3]*info_a[2] - info_b[2]*info_a[3]);
    if(denominator != 0.0f)
    {
        // Lines are not parallel...
        percent = (-info_a[X]*info_a[3]+info_b[X]*info_a[3]+info_a[Y]*info_a[2]-info_b[Y]*info_a[2]) / denominator; 
        write_xy[X] = info_b[X] + (info_b[2]*percent);
        write_xy[Y] = info_b[Y] + (info_b[3]*percent);
//log_message("INFO:     Intersection found at (%f, %f)", write_xy[X], write_xy[Y]);
    }
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro rotates a point...
#define room_rotate_macro(POSX, POSY, SINE, COSINE, TEMP) \
{                                               \
    TEMP = (COSINE * POSX) - (SINE * POSY);     \
    POSY = (SINE * POSX) + (COSINE * POSY);     \
    POSX = TEMP;                                \
}

//-----------------------------------------------------------------------------------------------
unsigned char* room_ddd_plop_function(unsigned char function, unsigned char* write, unsigned char* ddd_file_start, unsigned char base_model, unsigned char* texture_list, float* position_xyz, float* fore_xy, unsigned char pillar_stretch)
{
    // <ZZ> This function is a helper that handles a ddd plop into the room geometry...
    //      For pillars and stuff like that...
    //      Can add vertices, tex vertices, or triangles...
    //      Texture_list is a 4 value array telling which room texture to use for each model texture...
    //      Fore_xy is used to rotate the object...
    //
    //      Function returns the next position for data to be written...
    unsigned short i, j, k;
    unsigned short num_vertex, num_tex_vertex, num_primitive;
    float x, y, z;
    unsigned char lighting_multiplier;
    unsigned char* ddd_file_data;
    unsigned char num_base_model;
    unsigned char mode;
    unsigned short new_vertex;
    unsigned short new_tex_vertex;
    unsigned short num_joint;
    unsigned short num_bone;
    unsigned short flags;
    float scale, temp;


//log_message("");
//log_message("");
//log_message("INFO:   Handling function %d for ddd plop", function);
//if(function == EXTERIOR_VERTEX)      log_message("INFO:     Function is EXTERIOR_VERTEX function");
//if(function == EXTERIOR_TEX_VERTEX)  log_message("INFO:     Function is EXTERIOR_TEX_VERTEX function");
//if(function == EXTERIOR_TRIANGLE)    log_message("INFO:     Function is EXTERIOR_TRIANGLE function");



    // Read the DDD file to find our pointers and stuff...
    ddd_file_data = ddd_file_start;
    scale = sdf_read_unsigned_short(ddd_file_data) / DDD_SCALE_WEIGHT;  ddd_file_data+=2;
    flags = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data += 3;
    num_base_model = *ddd_file_data;  ddd_file_data += 3+MAX_DDD_SHADOW_TEXTURE;
    if(base_model >= num_base_model)
    {
//log_message("ERROR:    room_ddd_plop_function():  Base model incorrect");
        return write;
    }
    if(flags & DDD_EXTERNAL_BONE_FRAMES)
    {
        // Skip weird stuff...
        ddd_file_data+=8;
    }
    // Skip over base models that we're not using...
    repeat(i, base_model)
    {
        num_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_tex_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_joint = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_bone = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        ddd_file_data+=(9*num_vertex);
        ddd_file_data+=(4*num_tex_vertex);
        // Skip over the triangle lists...
        repeat(j, MAX_DDD_TEXTURE)
        {
            mode = *ddd_file_data;  ddd_file_data++;
            if(mode)
            {
                ddd_file_data+=2;
                num_primitive = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
                ddd_file_data+=num_primitive*12;
            }
        }
        ddd_file_data += num_joint;
        ddd_file_data += 5*num_bone;
    }
    num_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
    num_tex_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=6;
    // Important stuff should now be ready (ddd_file_data, num_vertex, num_tex_vertex)...


    // Handle the vertex add function...
    if(function == EXTERIOR_VERTEX)
    {
        // For each vertex of base model...
        repeat(i, num_vertex)
        {
            // Find location of vertex in room (translation & scaling and rotation & stuff)
            x = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=2;
            z = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=5;
            lighting_multiplier = 255;


            // Then do the rotation...
            room_rotate_macro(x, y, (-fore_xy[X]), fore_xy[Y], temp);


            // Offset translation...
            x += position_xyz[X];
            y += position_xyz[Y];
            if(pillar_stretch && z > 8.0f)
            {
                // Do thing for pillars to make 'em reach ceiling...
                z = z + ROOM_CEILING_Z - 16.0f;
            }
            else
            {
                // Normal z offset...
                z += position_xyz[Z];
            }



            // Add the new vertex
            (*((float*) write)) = x;  write+=4;
            (*((float*) write)) = y;  write+=4;
            (*((float*) write)) = z;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            *(write) = lighting_multiplier;  write++;  // Lighting multiplier (for pits and random variety)
            *(write) = lighting_multiplier;  write++;  // Final lighting value (generated every draw cycle)
//log_message("INFO:   Added DDD vertex %d as Room vertex %d (%f, %f, %f)", i, global_num_vertex, x, y, z);
            global_num_vertex++;
        }
    }
    else
    {
        // Skip the vertices of the DDD file...
        ddd_file_data+=(9*num_vertex);
    }






    // Handle the tex vertex add function...
    if(function == EXTERIOR_TEX_VERTEX)
    {
        // For each tex vertex of base model
        repeat(i, num_tex_vertex)
        {
            // Figger the tex vertex position...  Position may be modified later by triangle adds...
            x = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * ONE_OVER_256;  ddd_file_data+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * ONE_OVER_256;  ddd_file_data+=2;
//log_message("INFO:   Added DDD tex vertex %d as room tex vertex %d", i, global_num_tex_vertex);


            // Add a new tex vertex...
            (*((float*) write)) = x;  write+=4;
            (*((float*) write)) = y;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            *write = 255;  write++;   // Lighting multiplier (for pits and random variety)
            *write = 0;  write++;     // Final lighting value (generated every draw cycle)
            global_num_tex_vertex++;
        }
    }
    else
    {
        // Skip over the tex vertices in the DDD file...
        ddd_file_data += (4*num_tex_vertex);
    }













    // Handle the triangle add function...
    if(function == EXTERIOR_TRIANGLE)
    {
        // For each texture of base model
        repeat(i, MAX_DDD_TEXTURE)
        {
            // Is texture on?
            mode = *ddd_file_data;  ddd_file_data++;
            if(mode)
            {
                // Skip flags, skip alpha...
                ddd_file_data+=2;


                // For each triangle of texture...
                num_primitive = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
                repeat(j, num_primitive)
                {
                    // Setup some of the room triangle data (old DDD stripper format)
                    *((unsigned short*) write) = 0;  write+=2;              //  0  Number of neighboring triangles (never more than 8)
                    *((unsigned short*) write) = 65535;  write+=2;          //  2  Neighbor 0
                    *((unsigned short*) write) = 65535;  write+=2;          //  4  Neighbor 1
                    *((unsigned short*) write) = 65535;  write+=2;          //  6  Neighbor 2
                    *((unsigned short*) write) = 65535;  write+=2;          //  8  Neighbor 3
                    *((unsigned short*) write) = 65535;  write+=2;          // 10  Neighbor 4
                    *((unsigned short*) write) = 65535;  write+=2;          // 12  Neighbor 5
                    *((unsigned short*) write) = 65535;  write+=2;          // 14  Neighbor 6
                    *((unsigned short*) write) = 65535;  write+=2;          // 16  Neighbor 7

                    // For each point in triangle...
//log_message("INFO:   Added DDD tex triangle %d-%d as room tex-triangle %d-%d", i, j, texture_list[i], global_num_triangle);
                    repeat(k, 3)
                    {
                        // Read the vertex and the tex vertex from the ddd file...
                        new_vertex = sdf_read_unsigned_short(ddd_file_data) + global_num_vertex;  ddd_file_data+=2;
                        new_tex_vertex = sdf_read_unsigned_short(ddd_file_data) + global_num_tex_vertex;  ddd_file_data+=2;
//log_message("INFO:     Room vertex %d, Room tex vertex %d", new_vertex, new_tex_vertex);


                        // Add info to room triangle data (not in strips at this point)
                        *((unsigned short*) write) = new_vertex;            // 18, 20, 22  Vertex 0, 1, 2
                        *((unsigned short*) (write+6)) = new_tex_vertex;    // 24, 26, 28  Tex Vertex 0, 1, 2
                        write+=2;
                    }


                    // Finish writing this triangle...
                    write+=6;
                    *write = texture_list[i];  write+=1;                    // 30  Texture to use for triangle
                    *write = 0;  write+=1;                                  // 31  Triangle used somewhere
                    global_num_triangle++;
                }
            }
        }
        // Accumulate our number of vertices/tex vertices (triangle adds need to simulate vertex/tex vertex adds too...)
        global_num_vertex += num_vertex;
        global_num_tex_vertex += num_tex_vertex;
    }
    return write;
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro finds the xyz values of the next object in an srf file...  Does all the rotation
//      and stuff...  Assumes that object_group_data (should point to name of current object in
//      object group data), sine, and cosine are all setup correctly...
#define room_draw_srf_object_helper()                   \
{                                                       \
    temp_xyz[X] = (((signed short) sdf_read_unsigned_short(object_group_data+16)) * ONE_OVER_256);  \
    temp_xyz[Y] = (((signed short) sdf_read_unsigned_short(object_group_data+18)) * ONE_OVER_256);  \
    temp_xyz[Z] = (((signed short) sdf_read_unsigned_short(object_group_data+20)) * ONE_OVER_256);  \
    vertex_xyz[X] = (cosine*temp_xyz[X]) - (sine*temp_xyz[Y]) + x; \
    vertex_xyz[Y] = (sine*temp_xyz[X]) + (cosine*temp_xyz[Y]) + y; \
    vertex_xyz[Z] = temp_xyz[Z] + z; \
}

//-----------------------------------------------------------------------------------------------
unsigned char* room_plop_all_function(unsigned char function, unsigned char* write, unsigned char* data, unsigned char* srf_file, unsigned char* ddd_wall_door_file, unsigned short object_group, unsigned short rotation)
{
    // <ZZ> This is a wrapper for plopping all of the room's DDD models into it's geometry...
    //      Handles pillars at wall intersection points and DDD's in the object group info...
    //      Data is the start of the uncompressed room data...
    unsigned char* vertex_data;
    unsigned short num_vertex;
    unsigned char* exterior_wall_data;
    unsigned short num_exterior_wall;
    unsigned char* object_group_data;
    unsigned short num_group, num_object;
    unsigned short i;
    float vertex_xyz[3];
    float distance_xyz[3];
    unsigned char texture_list[MAX_DDD_TEXTURE];
    unsigned short length;
    unsigned char* model_data;
    float temp_xyz[3];
    float x, y, z, angle, sine, cosine;



//    unsigned short j, vertex;
//    float last_vertex_xyz[3];
//    float next_vertex_xyz[3];
//    float distance;
//    float last_wall_xy[2];
//    float next_wall_xy[2];








    // Find our pointers...
    vertex_data = data + (*((unsigned int*) (data+SRF_VERTEX_OFFSET)));  // Read from uncompressed data...
    num_vertex = *((unsigned short*) vertex_data);  vertex_data+=2;
    exterior_wall_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_EXTERIOR_WALL_OFFSET);  // Read from srf file data...
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;


/*
    // Build the texture list...
    repeat(i, MAX_DDD_TEXTURE)
    {
        texture_list[i] = (unsigned char) 4;
    }

log_message("INFO:   Room_plop_all...  Have %d wall pillars...", num_exterior_wall);


    // Do all the wall pillars...
    repeat(i, num_exterior_wall)
    {
        vertex = i;
        vertex = sdf_read_unsigned_short(exterior_wall_data + (vertex*3));
        vertex_xyz[X] = *((float*) (vertex_data + (vertex*26)));
        vertex_xyz[Y] = *((float*) (vertex_data + (vertex*26) + 4));
        vertex_xyz[Z] = *((float*) (vertex_data + (vertex*26) + 8));

        vertex = (i + 1) % num_exterior_wall;
        vertex = sdf_read_unsigned_short(exterior_wall_data + (vertex*3));
        next_vertex_xyz[X] = *((float*) (vertex_data + (vertex*26)));
        next_vertex_xyz[Y] = *((float*) (vertex_data + (vertex*26) + 4));
        next_vertex_xyz[Z] = *((float*) (vertex_data + (vertex*26) + 8));

        vertex = (i + num_exterior_wall - 1) % num_exterior_wall;
        vertex = sdf_read_unsigned_short(exterior_wall_data + (vertex*3));
        last_vertex_xyz[X] = *((float*) (vertex_data + (vertex*26)));
        last_vertex_xyz[Y] = *((float*) (vertex_data + (vertex*26) + 4));
        last_vertex_xyz[Z] = *((float*) (vertex_data + (vertex*26) + 8));

        // Check distance from current vertex to last vertex...  Don't plop if close together...
        distance_xyz[X] = vertex_xyz[X] - last_vertex_xyz[X];
        distance_xyz[Y] = vertex_xyz[Y] - last_vertex_xyz[Y];
        distance_xyz[Z] = 0.0f;
        distance = vector_length(distance_xyz);
        if(distance > MINIMUM_WALL_LENGTH)
        {
            // Check distance from current vertex to next vertex...  Use lowest z if close together...
            distance_xyz[X] = vertex_xyz[X] - next_vertex_xyz[X];
            distance_xyz[Y] = vertex_xyz[Y] - next_vertex_xyz[Y];
            distance = vector_length(distance_xyz);
            if(distance < MINIMUM_WALL_LENGTH)
            {
                if(next_vertex_xyz[Z] < vertex_xyz[Z])
                {
                    vertex_xyz[Z] = next_vertex_xyz[Z];
                }
            }


            // Find the forward normal of the pillar, by looking at normals of adjacent walls...
            last_wall_xy[X] = -(vertex_xyz[Y] - last_vertex_xyz[Y]);
            last_wall_xy[Y] = vertex_xyz[X] - last_vertex_xyz[X];
            if(distance > MINIMUM_WALL_LENGTH)
            {
                next_wall_xy[X] = -(next_vertex_xyz[Y] - vertex_xyz[Y]);
                next_wall_xy[Y] = next_vertex_xyz[X] - vertex_xyz[X];
            }
            else
            {
                next_wall_xy[X] = last_wall_xy[X];
                next_wall_xy[Y] = last_wall_xy[Y];
            }
            distance_xyz[X] = last_wall_xy[X] + next_wall_xy[X];
            distance_xyz[Y] = last_wall_xy[Y] + next_wall_xy[Y];
            distance = vector_length(distance_xyz);
            if(distance > 0.001f)
            {
                distance_xyz[X] /= distance;
                distance_xyz[Y] /= distance;
            }
            else
            {
                distance_xyz[X] = 0.0f;
                distance_xyz[Y] = 1.0f;
            }



            // Add the pillar...
log_message("INFO:     Plopping pillar %d", i);
            write = room_ddd_plop_function(function, write, ddd_wall_door_file, 2, texture_list, vertex_xyz, distance_xyz, TRUE);
        }
    }
*/







    // Now plop all of the DDD objects in the object group...
    // Find the rotation info...
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);


    // Read the object group data...
    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(num_group > 0)
    {
        // Make sure the object group is valid...
        object_group = object_group % num_group;


        // Skip through data to get to the current group...
        repeat(i, object_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }



        // Go through each object lookin' for .DDD's...
        num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
        repeat(i, num_object)
        {
            // Make sure we've got a null terminated string for the filename...
            object_group_data[12] = 0;
            length = strlen(object_group_data);
            if(length > 4)
            {
                // Is it a .DDD file?
                if(object_group_data[length-1] == 'D' && object_group_data[length-2] == 'D' && object_group_data[length-3] == 'D' && object_group_data[length-4] == '.')
                {
                    // Find the spawn coordinates...
                    room_draw_srf_object_helper();


                    // Find the file...
                    object_group_data[length-4] = 0;
                    model_data = sdf_find_filetype(object_group_data, SDF_FILE_IS_DDD);
                    object_group_data[length-4] = '.';
                    if(model_data)
                    {
                        model_data = (unsigned char*) sdf_read_unsigned_int(model_data);
//log_message("INFO:     Plopping DDD model %d", i);

                        // Build the texture list...
                        texture_list[0] = (unsigned char) (16 + (object_group_data[13]>>4));
                        texture_list[1] = (unsigned char) (16 + (object_group_data[13]&15));
                        texture_list[2] = (unsigned char) (16 + (object_group_data[14]>>4));
                        texture_list[3] = (unsigned char) (16 + (object_group_data[14]&15));
//log_message("INFO:       Using textures %d, %d, %d, %d", texture_list[0], texture_list[1], texture_list[2], texture_list[3]);


                        angle = ((unsigned short)(rotation + (object_group_data[15]<<8))) * (2.0f * PI / 65536.0f);
                        distance_xyz[X] = -((float) sin(angle));
                        distance_xyz[Y] = (float) cos(angle);
                        write = room_ddd_plop_function(function, write, model_data, 0, texture_list, vertex_xyz, distance_xyz, FALSE);
                    }
                }
            }
            object_group_data+=22;
        }
    }



    // Get back to the main decompression stuff...
    return write;
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
unsigned char* room_wall_segment_function(unsigned char function, unsigned char* srf_file, unsigned char* write, unsigned char* vertex_data, unsigned char* tex_vertex_data, unsigned short segment, unsigned short num_segment, unsigned short vertex, unsigned short last_vertex, unsigned char* ddd_wall_door_file, unsigned char base_model, float wall_texture_offset, float wall_texture_scale, unsigned short rotation)
{
    // <ZZ> This function is a helper that handles a wall segment between two specific vertices...
    //      Can add vertices, tex vertices, or triangles...  Must have setup weldable data
    //      (read through DDD file) before calling...
    //
    //      Function returns the next position for data to be written...
    unsigned char num_base_model;
    unsigned short num_vertex;
    unsigned short num_tex_vertex;
    unsigned short num_joint;
    unsigned short num_bone;
    unsigned char* ddd_file_data;
    float scale;
    unsigned char texture_to_use;
    unsigned char texture_mode;
    unsigned short num_primitive;
    unsigned short i, j, k;
    float x, y, z;
    float tx, ty;
    float temp;
    float vertex_xyz[3];
    float last_vertex_xyz[3];
    float vector_xyz[3];
    float normal_xyz[3];
    float segment_length;
    float distance_xyz[3];
    float distance, best_distance;
    unsigned char lighting_multiplier;
    unsigned char mode;
    unsigned char* read;
    float* weld_xyz;
    unsigned char welded_vertex;
    unsigned short best_vertex, best_index;
    unsigned short old_vertex;
    unsigned short new_vertex;
    unsigned short new_tex_vertex;
    float percent, inverse;
    float intersection_info[7];
    unsigned short flags;
    float angle, sine, cosine;


    angle = rotation * (-2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);


//log_message("");
//log_message("");
//log_message("INFO:   Handling function %d for wall segment from %d to %d", function, last_vertex, vertex);
//if(function == EXTERIOR_VERTEX)      log_message("INFO:     Function is EXTERIOR_VERTEX function");
//if(function == EXTERIOR_TEX_VERTEX)  log_message("INFO:     Function is EXTERIOR_TEX_VERTEX function");
//if(function == EXTERIOR_TRIANGLE)    log_message("INFO:     Function is EXTERIOR_TRIANGLE function");


    // Do some error checks...
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(read);  read+=2;
    if(vertex >= num_vertex || last_vertex >= num_vertex)
    {
        log_message("ERROR:    Room_wall_segment_function():  Invalid vertex or last vertex (%d or %d) (num vertex is %d)", vertex, last_vertex, num_vertex);
        return write;
    }


    // Find the positions of the two vertices that the segment lies between...
    vertex_xyz[X] = ((float*) (vertex_data + (26*vertex)))[X];
    vertex_xyz[Y] = ((float*) (vertex_data + (26*vertex)))[Y];
    vertex_xyz[Z] = ((float*) (vertex_data + (26*vertex)))[Z];
    last_vertex_xyz[X] = ((float*) (vertex_data + (26*last_vertex)))[X];
    last_vertex_xyz[Y] = ((float*) (vertex_data + (26*last_vertex)))[Y];
    last_vertex_xyz[Z] = ((float*) (vertex_data + (26*last_vertex)))[Z];
//log_message("INFO:     Last vertex at %f, %f, %f", last_vertex_xyz[X], last_vertex_xyz[Y], last_vertex_xyz[Z]);
//log_message("INFO:     Vertex at %f, %f, %f", vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z]);


    // Find the vector from the last vertex to the current one...
    vector_xyz[X] = vertex_xyz[X] - last_vertex_xyz[X];
    vector_xyz[Y] = vertex_xyz[Y] - last_vertex_xyz[Y];
    vector_xyz[Z] = 0.0f;
    segment_length = vector_length(vector_xyz);
    if(segment_length < MINIMUM_WALL_LENGTH)
    {
//        log_message("INFO:     Room_wall_segment_function():  XY Segment length was %f...  Too short to draw", segment_length);
        return write;
    }
    vector_xyz[Z] = vertex_xyz[Z] - last_vertex_xyz[Z];
//    segment_length = vector_length(vector_xyz);
//log_message("INFO:     Segment Length is %f", segment_length);


    // Find the normal to that vector (wall segment)...
    distance_xyz[X] = 0.0f;
    distance_xyz[Y] = 0.0f;
    distance_xyz[Z] = 1.0f;
    cross_product(vector_xyz, distance_xyz, normal_xyz);
    distance = vector_length(normal_xyz);
    if(distance > 0.0001f)
    {
        normal_xyz[X]/=distance;
        normal_xyz[Y]/=distance;
    }


    // And scale the vector by 1/32...  (because wall DDD's are 32ft long...)
    vector_xyz[X]*=0.03125f;  //0.0625f;
    vector_xyz[Y]*=0.03125f;  //0.0625f;
    vector_xyz[Z]*=0.03125f;  //0.0625f;
// LOLO








    // Read the DDD File header, looking for the desired base model...
    ddd_file_data = ddd_wall_door_file;
    scale = sdf_read_unsigned_short(ddd_file_data) / DDD_SCALE_WEIGHT;  ddd_file_data+=2;
    flags = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data += 3;
    num_base_model = *ddd_file_data;  ddd_file_data += 3+MAX_DDD_SHADOW_TEXTURE;
    if(base_model >= num_base_model)
    {
        log_message("ERROR:    Room_wall_segment_function():  Invalid base model (%d)", base_model);
        return write;
    }
    if(flags & DDD_EXTERNAL_BONE_FRAMES)
    {
        // Skip weird stuff...
        ddd_file_data+=8;
    }
    // Skip over all the base models we don't care about...
    repeat(i, base_model)
    {
        num_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_tex_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_joint = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        num_bone = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
        // Skip over the vertices
        ddd_file_data+=num_vertex*9;
        // Skip over the tex vertices...
        ddd_file_data += 4*num_tex_vertex;
        // Skip over the triangle lists...
        repeat(j, MAX_DDD_TEXTURE)
        {
            texture_mode = *ddd_file_data;  ddd_file_data++;
            if(texture_mode)
            {
                ddd_file_data+=2;
                num_primitive = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
                ddd_file_data+=num_primitive*12;
            }
        }
        // Skip over the joints...
        ddd_file_data += num_joint;
        // Skip over the bones...
        ddd_file_data += 5*num_bone;
    }
    // Figger the pointers & counts for our desired base model...
    // ddd_file_data should be at the start of our desired base model...
    num_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
    num_tex_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=6;
    // ddd_file_data should now be at the start of the ddd vertices...
    if(num_vertex >= MAX_DDD_WELDABLE_VERTEX)
    {
        log_message("ERROR:    Room_wall_segment_function():  Too many vertices (%d) in base model %d", num_vertex, base_model);
        return write;
    }


 




    // Handle the vertex add function...  (Also do for triangle add, to generate correct vertex translation lists...  For DDD vertex # to room vertex #...)
    if(function == EXTERIOR_VERTEX || function == EXTERIOR_TRIANGLE)
    {
        // For each vertex of base model...
        repeat(i, num_vertex)
        {
            // Find location of vertex in room (translation & scaling and rotation & stuff)
            x = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=2;
            z = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * scale;  ddd_file_data+=5;
            if(x < -15.90f) { x = -16.0f; }
            if(x > 15.90f)  { x = 16.0f; }
            intersection_info[4] = x;
            intersection_info[5] = y;
            intersection_info[6] = z;
            lighting_multiplier = 255;
            // Fade out room exits...
            if(y < -3.0f)
            {
                temp = 255.0f + ((y+3.0f)*32.0f);
                if(temp > 0.0f)
                {
                    lighting_multiplier = (unsigned char) temp;
                }
                else
                {
                    lighting_multiplier = 0;
                }
            }


            // Do special thing to keep the door size consistant despite length of wall segment...
            // Is the vertex in the middle area of the model (area used for door)?
// LOLO
            if(x > -8.10f && x < 8.10f)
            {
                // Yup it is...  Scale it so that it remains same size when scaled later on...
                x*=32.0f/segment_length;  //16.0f/segment_length;
            }




// LOLO
            x+=16.0f;  // Should range from 0.0 to 32.0...     x+=8.0f;  // Should range from 0.0 to 16.0...
// LOLO
            percent = x * 0.03125f;  //0.0625f;  // Should range from 0.0 to 1.0...
            inverse = 1.0f-percent;


            // Then do z skew and ceiling...
            if(z > 15.90f)
            {
                // High vertices in wall become our ceiling vertices...
                z = ROOM_CEILING_Z;
            }
            else
            {
                // Other vertices are skewed according to the start and stop
                // positions of the wall segment...
                z += (vertex_xyz[Z]*percent) + (last_vertex_xyz[Z]*inverse);
            }



            // Then use vector_xyz and normal_xyz to do the rotation & scaling...
            distance_xyz[X] = x*vector_xyz[X] - y*normal_xyz[X];
            distance_xyz[Y] = x*vector_xyz[Y] - y*normal_xyz[Y];
            x = distance_xyz[X] + last_vertex_xyz[X];
            y = distance_xyz[Y] + last_vertex_xyz[Y];


            // Write our intersection info while we're at it...  (Better than welding...  I hope...)
            intersection_info[0] = x;
            intersection_info[1] = y;
            intersection_info[2] = vector_xyz[X];
            intersection_info[3] = vector_xyz[Y];
//log_message("INFO:   Generated intersection info for vertex %d", i);
//log_message("INFO:     Info[0] == %f", intersection_info[0]);
//log_message("INFO:     Info[1] == %f", intersection_info[1]);
//log_message("INFO:     Info[2] == %f", intersection_info[2]);
//log_message("INFO:     Info[3] == %f", intersection_info[3]);



            // Save the percent for this vertex...
            welded_vertex = FALSE;
            if(i < MAX_DDD_WELDABLE_VERTEX)
            {
                ddd_vertex_percent[i] = percent;
//log_message("INFO:     VERTEX %d (%f, %f, %f) PERCENT == %f", i, x, y, z, percent);


                // If DDD vertex is weldable
// LOLO
                if(intersection_info[4] < -15.90f || intersection_info[4] > 15.90f)
                {
//log_message("INFO:     Trying to weld DDD vertex %d (segment %d) (position %f, %f, %f)", i, segment, intersection_info[4], intersection_info[5], intersection_info[6]);


                    // Check every distance to every vertex in room weldable list for smallest distance
                    best_distance = ROOM_WELDABLE_DISTANCE_TOLERANCE;
                    best_vertex = 65535;
                    best_index = 65535;
                    repeat(j, num_room_weldable_vertex)
                    {
//log_message("INFO:       Possibility %d (segment %d) (position %f, %f, %f)...", j, room_weldable_vertex_segment[j], room_weldable_intersection_info[j][4], room_weldable_intersection_info[j][5], room_weldable_intersection_info[j][6]);


                        // Match adjacent wall segments...
                        if(room_weldable_vertex_segment[j] == ((segment+1)%num_segment) || ((room_weldable_vertex_segment[j]+1)%num_segment) == segment)
                        {
//log_message("INFO:         Matched adjacent segments");
                            // Check model tolerance distance...
                            distance_xyz[X] = intersection_info[4] + room_weldable_intersection_info[j][4];
                            distance_xyz[Y] = intersection_info[5] - room_weldable_intersection_info[j][5];
                            distance_xyz[Z] = intersection_info[6] - room_weldable_intersection_info[j][6];
                            distance = (distance_xyz[X]*distance_xyz[X]) + (distance_xyz[Y]*distance_xyz[Y]) + (distance_xyz[Z]*distance_xyz[Z]);
                            if(distance < 0.01f)
                            {
//log_message("INFO:         Beat model tolerance distance");
                                // Check actual in-world distance...
                                weld_xyz = (float*) (vertex_data + room_weldable_vertex_list[j]*26);
                                distance_xyz[X] = x - weld_xyz[X];
                                distance_xyz[Y] = y - weld_xyz[Y];
                                distance_xyz[Z] = z - weld_xyz[Z];
                                distance = (distance_xyz[X]*distance_xyz[X]) + (distance_xyz[Y]*distance_xyz[Y]) + (distance_xyz[Z]*distance_xyz[Z]);
                                if(distance < best_distance)
                                {
//log_message("INFO:         Beat best distance");
                                    best_distance = distance;
                                    best_vertex = room_weldable_vertex_list[j];
                                    best_index = j;
                                }
                            }
                        }
                    }
                    // If we found a match...
                    if(best_vertex < 65535)
                    {
                        // Weld the vertex to the vertex we found
                        welded_vertex = TRUE;

                        // Modify the position of the old vertex...
                        if(function == EXTERIOR_VERTEX)
                        {
                            // Only if we're actually addin' vertices...
                            weld_xyz = (float*) (vertex_data + best_vertex*26);
//log_message("INFO:   Trying to weld vertex at (%f, %f, %f) to one at (%f, %f, %f)", weld_xyz[X], weld_xyz[Y], weld_xyz[Z], x, y, z);
                            weld_xyz[X] = (weld_xyz[X] + x)*0.5f;
                            weld_xyz[Y] = (weld_xyz[Y] + y)*0.5f;
                            weld_xyz[Z] = (weld_xyz[Z] + z)*0.5f;


                            // Weld_xyz[X] and Weld_xyz[Y] are only used if intersection routine fails...
// !!!BAD!!!
// !!!BAD!!! This only kinda worked...  Seems like it gets outta whack when room is rotated though, so I got rid of it...
// !!!BAD!!!
//                            intersect_xy_lines(intersection_info, room_weldable_intersection_info[best_index], weld_xyz);
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!


//log_message("INFO:   Welded DDD vertex %d to Room vertex %d (%f, %f, %f)", i, best_vertex, weld_xyz[X], weld_xyz[Y], weld_xyz[Z]);
                        }

                        // Remove room vertex from weldable list
                        room_weldable_vertex_remove(best_vertex);

                        // Vertex Translation Number is number of vertex we welded to...
                        // So triangles can say, "Oh, DDD vertex 0 is room vertex 23"...
                        ddd_vertex_translation[i] = best_vertex;
                    }
                }
            }
            // If we didn't weld vertex...
            if(!welded_vertex)
            {
                // Remember the vertex translation number...
                // So triangles can say, "Oh, DDD vertex 0 is room vertex 23"...
                new_vertex = global_num_vertex;
                ddd_vertex_translation[i] = new_vertex;


                // Add the new vertex
                if(function == EXTERIOR_VERTEX)
                {
                    // Only if we're actually addin' vertices...
                    // Position...
                    (*((float*) write)) = x;  write+=4;
                    (*((float*) write)) = y;  write+=4;
                    (*((float*) write)) = z;  write+=4;

                    // Normal...
                    (*((float*) write)) = 0.0f;  write+=4;
                    (*((float*) write)) = 0.0f;  write+=4;
                    (*((float*) write)) = 0.0f;  write+=4;

                    // Lighting info...
                    *(write) = lighting_multiplier;  write++;  // Lighting multiplier (for pits and random variety)
                    *(write) = lighting_multiplier;  write++;  // Final lighting value (generated every draw cycle)
//log_message("INFO:   Added DDD vertex %d as Room vertex %d (%f, %f, %f)", i, new_vertex, x, y, z);
                }
                global_num_vertex++;


                // If DDD vertex is weldable
// LOLO
                if(intersection_info[4] < -15.90f || intersection_info[4] > 15.90f)
                {
                    // Add the new vertex to the room weldable list
                    room_weldable_vertex_add(new_vertex, segment, intersection_info);
                }
            }
        }
    }
    else
    {
        // Skip the vertices of the DDD file...
        ddd_file_data+=(9*num_vertex);
    }













    // Handle the tex vertex add function...
    if(function == EXTERIOR_TEX_VERTEX)
    {
        // For each tex vertex of base model
        repeat(i, num_tex_vertex)
        {
            // Figger the tex vertex position...  Position may be modified later by triangle adds...
            x = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * ONE_OVER_256;  ddd_file_data+=2;
            y = ((float) ((signed short) sdf_read_unsigned_short(ddd_file_data))) * ONE_OVER_256;  ddd_file_data+=2;
//log_message("INFO:   Added DDD tex vertex %d as room tex vertex %d", i, global_num_tex_vertex);


            // Add a new tex vertex...
            // Position...
            (*((float*) write)) = x;  write+=4;
            (*((float*) write)) = y;  write+=4;

            // Normal...
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;
            (*((float*) write)) = 0.0f;  write+=4;

            // Lighting info...
            *write = 255;  write++;   // Lighting multiplier (for pits and random variety)
            *write = 0;  write++;     // Final lighting value (generated every draw cycle)


            global_num_tex_vertex++;
        }
    }
    else
    {
        // Skip over the tex vertices in the DDD file...
        ddd_file_data += (4*num_tex_vertex);
    }













    // Handle the triangle add function...  Also assigns tex vertex locations based on DDD texture flags...
    if(function == EXTERIOR_TRIANGLE)
    {
        // For each texture of base model
        repeat(i, MAX_DDD_TEXTURE)
        {
            // Is texture on?
            mode = *ddd_file_data;  ddd_file_data++;
            if(mode)
            {
                // Read flags, skip alpha...
                flags = *ddd_file_data;  ddd_file_data+=2;


                // For each triangle of texture...
                num_primitive = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
                repeat(j, num_primitive)
                {
                    // Setup some of the room triangle data (old DDD stripper format)
                    *((unsigned short*) write) = 0;  write+=2;              //  0  Number of neighboring triangles (never more than 8)
                    *((unsigned short*) write) = 65535;  write+=2;          //  2  Neighbor 0
                    *((unsigned short*) write) = 65535;  write+=2;          //  4  Neighbor 1
                    *((unsigned short*) write) = 65535;  write+=2;          //  6  Neighbor 2
                    *((unsigned short*) write) = 65535;  write+=2;          //  8  Neighbor 3
                    *((unsigned short*) write) = 65535;  write+=2;          // 10  Neighbor 4
                    *((unsigned short*) write) = 65535;  write+=2;          // 12  Neighbor 5
                    *((unsigned short*) write) = 65535;  write+=2;          // 14  Neighbor 6
                    *((unsigned short*) write) = 65535;  write+=2;          // 16  Neighbor 7

                    // For each point in triangle...
//log_message("INFO:   Added DDD tex triangle %d-%d as room triangle %d", i, j, global_num_triangle);
                    repeat(k, 3)
                    {
                        // Read the vertex and the tex vertex from the ddd file...
                        old_vertex = sdf_read_unsigned_short(ddd_file_data);  ddd_file_data+=2;
                        new_vertex = ddd_vertex_translation[old_vertex];
                        new_tex_vertex = sdf_read_unsigned_short(ddd_file_data) + global_num_tex_vertex;  ddd_file_data+=2;
//log_message("INFO:     Room vertex %d, Room tex vertex %d", new_vertex, new_tex_vertex);




                        // Add info to room triangle data (not in strips at this point)
                        *((unsigned short*) write) = new_vertex;            // 18, 20, 22  Vertex 0, 1, 2
                        *((unsigned short*) (write+6)) = new_tex_vertex;    // 24, 26, 28  Tex Vertex 0, 1, 2
                        write+=2;


                        // Set tex vertex position from actual in-room vertex position...
                        if(flags & RENDER_LIGHT_FLAG)
                        {
                            // X tex vertex location should come from vertex's distance along wall...
                            (*((float*) (tex_vertex_data + (22*new_tex_vertex)))) = (segment_length * ddd_vertex_percent[old_vertex&(MAX_DDD_WELDABLE_VERTEX-1)] * wall_texture_scale) + wall_texture_offset;
                        }
                        if(flags & RENDER_COLOR_FLAG)
                        {
                            // Y tex vertex location should come from z height of vertex...
                            (*((float*) (tex_vertex_data + (22*new_tex_vertex) + 4))) = (*((float*) (vertex_data + (26*new_vertex) + 8))) * -WALL_TEXTURE_SCALE;
                        }
                        if(flags & RENDER_NOCULL_FLAG)
                        {
                            // X and Y tex vertex location should come from xy location of vertex...  Like floor...
//                            (*((float*) (tex_vertex_data + (22*new_tex_vertex))))     = (*((float*) (vertex_data + (26*new_vertex)))) * WALL_TEXTURE_SCALE;
//                            (*((float*) (tex_vertex_data + (22*new_tex_vertex) + 4))) = (*((float*) (vertex_data + (26*new_vertex) + 4))) * WALL_TEXTURE_SCALE;


                            // X and Y tex vertex location should come from xy location of vertex...  Like floor...
                            tx = (*((float*) (vertex_data + (26*new_vertex)))) * WALL_TEXTURE_SCALE;
                            ty = (*((float*) (vertex_data + (26*new_vertex) + 4))) * WALL_TEXTURE_SCALE;
                            (*((float*) (tex_vertex_data + (22*new_tex_vertex)))) = (cosine*tx) - (sine*ty);
                            (*((float*) (tex_vertex_data + (22*new_tex_vertex) + 4))) = (sine*tx) + (cosine*ty);
                        }
//log_message("INFO:     Tex vertex %d is now at %f, %f", new_tex_vertex, (*((float*) (tex_vertex_data + (22*new_tex_vertex)))), (*((float*) (tex_vertex_data + (22*new_tex_vertex) + 4))));
                    }


                    // Finish writing this triangle...
                    write+=6;
                    texture_to_use = (unsigned char) i;
                    // Funky thing for making lower level doors use Floor2...
                    if(texture_to_use == 0)
                    {
                        if(vertex_xyz[Z] < -3.0f && last_vertex_xyz[Z] < -3.0f)
                        {
                            texture_to_use = 7;
                        }
                    }
                    if(flags & RENDER_PAPER_FLAG)
                    {
                        texture_to_use = 28;
                    }
                    *write = texture_to_use;  write+=1;                     // 30  Texture to use for triangle
                    *write = 0;  write+=1;                                  // 31  Triangle used somewhere
                    global_num_triangle++;
                }
            }
        }
        // Accumulate our number of tex vertices (triangle adds need to simulate tex vertex adds too...)
        global_num_tex_vertex += num_tex_vertex;
    }
    return write;
}

//-----------------------------------------------------------------------------------------------
unsigned char* room_exterior_wall_function(unsigned char function, unsigned char* srf_file, unsigned char* write, unsigned char* vertex_data, unsigned char* tex_vertex_data, unsigned char* ddd_wall_door_file, unsigned char* door_wall_list, unsigned short rotation)
{
    // <ZZ> This function goes around all of the exterior walls of a room, doing a specific
    //      operation as it goes.  Adds vertices, tex_vertices, or triangles at the given
    //      write position, and also increments the global numbers accordingly...
    //      vertex_data is the start of the uncompressed vertex locations (not num)
    //
    //      Door_wall_list should be 5 bytes telling us which walls are used by doors (or
    //      255 if not used)...  If door_wall_list is NULL, all doors are made active...
    //
    //      Function returns the next position for data to be written...
    unsigned char* read;
    unsigned char door_flags;
    unsigned short i, j, num_exterior_wall, vertex, last_vertex;
    unsigned char found_door;
    float distance_xyz[3];
    float distance;
    float perimeter;
    unsigned short walls_we_can_fit;
    float wall_texture_scale;
    float wall_texture_offset;

//log_message("INFO:   Doing room wall function %d...", function);


    // Clear the list of weldable vertices...
    room_weldable_vertex_clear();



    // Find the total perimeter length of the room...
    perimeter = 0.1f;
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(read);  read+=2;
    vertex = sdf_read_unsigned_short((read+(3*num_exterior_wall)-3));
    repeat(i, num_exterior_wall)
    {
        last_vertex = vertex;
        vertex = sdf_read_unsigned_short(read);  read+=3;
        distance_xyz[X] = ((float*) (vertex_data + (26*vertex)))[X];
        distance_xyz[Y] = ((float*) (vertex_data + (26*vertex)))[Y];
        distance_xyz[X] -= ((float*) (vertex_data + (26*last_vertex)))[X];
        distance_xyz[Y] -= ((float*) (vertex_data + (26*last_vertex)))[Y];
        distance_xyz[Z] = 0.0f;
        distance = vector_length(distance_xyz);
        perimeter+=distance;
    }
    walls_we_can_fit = (unsigned short) (perimeter * WALL_TEXTURE_SCALE);
    walls_we_can_fit++;
    wall_texture_scale = walls_we_can_fit / perimeter;





    // Go around each exterior wall segment, doing whatever we were told to do...
    distance = 0.0f;
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(read);  read+=2;
    vertex = sdf_read_unsigned_short((read+(3*num_exterior_wall)-3));
    repeat(i, num_exterior_wall)
    {
        last_vertex = vertex;
        vertex = sdf_read_unsigned_short(read);  read+=2;
        door_flags = *read;  read++;



        wall_texture_offset = walls_we_can_fit * (distance / perimeter);
        wall_texture_offset = wall_texture_offset - ((unsigned short) wall_texture_offset);
        distance_xyz[X] = ((float*) (vertex_data + (26*vertex)))[X];
        distance_xyz[Y] = ((float*) (vertex_data + (26*vertex)))[Y];
        distance_xyz[X] -= ((float*) (vertex_data + (26*last_vertex)))[X];
        distance_xyz[Y] -= ((float*) (vertex_data + (26*last_vertex)))[Y];
        distance_xyz[Z] = 0.0f;
        distance += vector_length(distance_xyz);



        // Check if this segment might be a door...
        if(door_wall_list)
        {
            found_door = FALSE;
            repeat(j, 5)
            {
                if(((unsigned char) door_wall_list[j]) == i)
                {
                    found_door = TRUE;
                }
            }
        }
        else
        {
            found_door = TRUE;
        }


        // Do the function for each segment...
//log_message("INFO:   Setting wall (%d), Found door == %d, Door flags == %d", i, found_door, door_flags&15);
        if(((door_flags&15) > ROOM_WALL_FLAG_LOW_NO_WALL) && found_door)
        {
            // Make a door hole...
            if((door_flags & 15) == ROOM_WALL_FLAG_LOW_SECRET_DOOR)
            {
                // Secret door wallset type...  Model 4...
//log_message("INFO:     Setting secret door");
                write = room_wall_segment_function(function, srf_file, write, vertex_data, tex_vertex_data, i, num_exterior_wall, vertex, last_vertex, ddd_wall_door_file, 4, wall_texture_offset, wall_texture_scale, rotation);
            }
            else
            {
                // Normal door wallset type...  Model 3...
//log_message("INFO:     Setting normal door");
                write = room_wall_segment_function(function, srf_file, write, vertex_data, tex_vertex_data, i, num_exterior_wall, vertex, last_vertex, ddd_wall_door_file, 3, wall_texture_offset, wall_texture_scale, rotation);
            }
        }
        else
        {
            if((door_flags & 15) == ROOM_WALL_FLAG_LOW_NO_WALL)
            {
                // No wall wallset type...  Model 2...
//log_message("INFO:     Setting no wall");
                write = room_wall_segment_function(function, srf_file, write, vertex_data, tex_vertex_data, i, num_exterior_wall, vertex, last_vertex, ddd_wall_door_file, 2, wall_texture_offset, wall_texture_scale, rotation);
            }
            else
            {

                if((door_flags & 15) == ROOM_WALL_FLAG_LOW_NO_TRIM_WALL)
                {
                    // No trim wall wallset type...  Model 1...
//log_message("INFO:     Setting no trim wall");
                    write = room_wall_segment_function(function, srf_file, write, vertex_data, tex_vertex_data, i, num_exterior_wall, vertex, last_vertex, ddd_wall_door_file, 1, wall_texture_offset, wall_texture_scale, rotation);
                }
                else
                {
                    // Normal wall wallset type...  Model 0...
//log_message("INFO:     Setting normal wall");
                    write = room_wall_segment_function(function, srf_file, write, vertex_data, tex_vertex_data, i, num_exterior_wall, vertex, last_vertex, ddd_wall_door_file, 0, wall_texture_offset, wall_texture_scale, rotation);
                }
            }
        }
    }
    return write;
}

//-----------------------------------------------------------------------------------------------
// <ZZ> Macroized version of the function below...  This is straight forward enough...
#define room_heightmap_height(DATA, FX, FY)   ((*(((signed short*) (DATA + (*((unsigned int*) (DATA+SRF_HEIGHTMAP_OFFSET)))))+((((signed short) (((FY+ROOM_HEIGHTMAP_BIAS)*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1))) & (ROOM_HEIGHTMAP_SIZE-1))*ROOM_HEIGHTMAP_SIZE)+(((signed short) (((FX+ROOM_HEIGHTMAP_BIAS)*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1))) & (ROOM_HEIGHTMAP_SIZE-1))))*ROOM_HEIGHTMAP_SCALE)

//-----------------------------------------------------------------------------------------------
//float room_heightmap_height(unsigned char* data, float fx, float fy)
//{
//    // <ZZ> This function returns the height of the room at the desired xy position...
//    //      data is the start of the uncompressed room data...
//    signed short x, y, z;
//    signed short* heightmap_data;
//
//    // Read the room header to find where our heightmap data is...
//    heightmap_data = ((signed short*) (data + (*((unsigned int*) (data+SRF_HEIGHTMAP_OFFSET)))));
//
//    // Offset into the heightmap...
//    x = (((signed short) ((fx*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1))) & (ROOM_HEIGHTMAP_SIZE-1));
//    y = (((signed short) ((fy*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1))) & (ROOM_HEIGHTMAP_SIZE-1));
//
//    // Read the value...
//    z = (*(heightmap_data+(y*ROOM_HEIGHTMAP_SIZE)+x));
//
//    // Convert the value (signed short) into our final height value (float)
//    return (z*ROOM_HEIGHTMAP_SCALE);
//}

// Wrapper for macro above...  So I can call in weird places...
float room_heightmap_height_slow(unsigned char* data, float x, float y)
{
    float z;
    z = room_heightmap_height(data, x, y);
    return z;
}


//-----------------------------------------------------------------------------------------------
void room_heightmap_clear(signed short* heightmap_data)
{
    // <ZZ> This function clears out the heightmap to all 32767 (very high)...
    unsigned short x, y;

    repeat(y, ROOM_HEIGHTMAP_SIZE)
    {
        repeat(x, ROOM_HEIGHTMAP_SIZE)
        {
            *heightmap_data = 32767;
            heightmap_data++;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void room_heightmap_triangle(signed short* heightmap_data, float* fa_xyz, float* fb_xyz, float* fc_xyz)
{
    // <ZZ> This function draws a triangle into the heightmap, given three in-room vertex coordinates...
    //      Coordinates are converted into heightmap coordinates, then drawn with a z-buffer type
    //      thing (so only the highest value is in the final heightmap)...
    signed short vertex_xyz[3][3];
    unsigned char a_order, b_order, c_order;
    signed short x, y, z, left_x, right_x, left_z, right_z;
    signed short full_size, section_size, x_size, z_size, x_count;
    signed short* read;


    // Sort the vertices by increasing y value...
    a_order = 0;
    b_order = 0;
    c_order = 0;
    if(fb_xyz[Y] < fa_xyz[Y]) { a_order++; } else { b_order++; }
    if(fc_xyz[Y] < fa_xyz[Y]) { a_order++; } else { c_order++; }
    if(fc_xyz[Y] < fb_xyz[Y]) { b_order++; } else { c_order++; }


    // Convert room coordinates into heightmap coordinates...
    vertex_xyz[a_order][X] = (signed short) ((fa_xyz[X]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[a_order][Y] = (signed short) ((fa_xyz[Y]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[a_order][Z] = (signed short) (fa_xyz[Z]*16384/ROOM_HEIGHTMAP_Z);
    vertex_xyz[b_order][X] = (signed short) ((fb_xyz[X]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[b_order][Y] = (signed short) ((fb_xyz[Y]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[b_order][Z] = (signed short) (fb_xyz[Z]*16384/ROOM_HEIGHTMAP_Z);
    vertex_xyz[c_order][X] = (signed short) ((fc_xyz[X]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[c_order][Y] = (signed short) ((fc_xyz[Y]*ROOM_HEIGHTMAP_PRECISION) + (ROOM_HEIGHTMAP_SIZE>>1));
    vertex_xyz[c_order][Z] = (signed short) (fc_xyz[Z]*16384/ROOM_HEIGHTMAP_Z);



    // Draw the triangle in two sections...
    y = vertex_xyz[0][Y];
    full_size = vertex_xyz[2][Y] - vertex_xyz[0][Y];


    // First section...
    section_size = vertex_xyz[1][Y] - vertex_xyz[0][Y];
//log_message("INFO:   room_heightmap_triangle():  Drawing triangle from (%d, %d, %d) to (%d, %d, %d) to (%d, %d, %d)", vertex_xyz[0][X], vertex_xyz[0][Y], vertex_xyz[0][Z], vertex_xyz[1][X], vertex_xyz[1][Y], vertex_xyz[1][Z], vertex_xyz[2][X], vertex_xyz[2][Y], vertex_xyz[2][Z]);


//log_message("INFO:   room_heightmap_triangle():  Drawing first section from (%d, %d, %d) to (%d, %d, %d)", vertex_xyz[0][X], vertex_xyz[0][Y], vertex_xyz[0][Z], vertex_xyz[1][X], vertex_xyz[1][Y], vertex_xyz[1][Z]);
    while(y < vertex_xyz[1][Y])
    {
        if(y > ROOM_HEIGHTMAP_BORDER && y < (ROOM_HEIGHTMAP_SIZE-ROOM_HEIGHTMAP_BORDER-1))
        {
            left_x =  ((y-vertex_xyz[0][Y])*(vertex_xyz[1][X] - vertex_xyz[0][X])/section_size) + vertex_xyz[0][X];
            right_x = ((y-vertex_xyz[0][Y])*(vertex_xyz[2][X] - vertex_xyz[0][X])/full_size) + vertex_xyz[0][X];
            left_z =  ((y-vertex_xyz[0][Y])*(vertex_xyz[1][Z] - vertex_xyz[0][Z])/section_size) + vertex_xyz[0][Z];
            right_z = ((y-vertex_xyz[0][Y])*(vertex_xyz[2][Z] - vertex_xyz[0][Z])/full_size) + vertex_xyz[0][Z];
            if(right_x < left_x) { x = left_x;  left_x = right_x;  right_x = x;  z = left_z;  left_z = right_z;  right_z = z; }
//log_message("INFO:     Y line is at %d, left is %d (%d high), right is %d (%d high)", y, left_x, left_z, right_x, right_z);
            x = left_x;
            x_size = right_x-left_x;
            x_count = 0;
            z_size = right_z-left_z;
            read = heightmap_data+(y*ROOM_HEIGHTMAP_SIZE)+x;
            while(x < right_x)
            {
                if(x > ROOM_HEIGHTMAP_BORDER && x < (ROOM_HEIGHTMAP_SIZE-ROOM_HEIGHTMAP_BORDER-1))
                {
                    z = ((x_count)*(z_size)/(x_size)) + left_z;
                    *(read) = (z > (*read) || (*read) == 32767) ? z : (*read);
                }
                read++;
                x++;
                x_count++;
            }
            // Draw one extra to help fill in gaps...
            *(read) = (right_z > (*read) || (*read) == 32767) ? right_z : (*read);
        }
        y++;
    }


    // Second section...
//log_message("INFO:   room_heightmap_triangle():  Drawing second section from (%d, %d, %d) to (%d, %d, %d)", vertex_xyz[1][X], vertex_xyz[1][Y], vertex_xyz[1][Z], vertex_xyz[2][X], vertex_xyz[2][Y], vertex_xyz[2][Z]);
    section_size = vertex_xyz[2][Y] - vertex_xyz[1][Y];
    while(y < vertex_xyz[2][Y])
    {
        if(y > ROOM_HEIGHTMAP_BORDER && y < (ROOM_HEIGHTMAP_SIZE-ROOM_HEIGHTMAP_BORDER-1))
        {
            left_x =  ((y-vertex_xyz[1][Y])*(vertex_xyz[2][X] - vertex_xyz[1][X])/section_size) + vertex_xyz[1][X];
            right_x = ((y-vertex_xyz[0][Y])*(vertex_xyz[2][X] - vertex_xyz[0][X])/full_size) + vertex_xyz[0][X];
            left_z =  ((y-vertex_xyz[1][Y])*(vertex_xyz[2][Z] - vertex_xyz[1][Z])/section_size) + vertex_xyz[1][Z];
            right_z = ((y-vertex_xyz[0][Y])*(vertex_xyz[2][Z] - vertex_xyz[0][Z])/full_size) + vertex_xyz[0][Z];
            if(right_x < left_x) { x = left_x;  left_x = right_x;  right_x = x;  z = left_z;  left_z = right_z;  right_z = z; }
//log_message("INFO:     Y line is at %d, left is %d (%d high), right is %d (%d high)", y, left_x, left_z, right_x, right_z);
            x = left_x;
            x_size = right_x-left_x;
            x_count = 0;
            z_size = right_z-left_z;
            read = heightmap_data+(y*ROOM_HEIGHTMAP_SIZE)+x;
            while(x < right_x)
            {
                if(x > ROOM_HEIGHTMAP_BORDER && x < (ROOM_HEIGHTMAP_SIZE-ROOM_HEIGHTMAP_BORDER-1))
                {
                    z = ((x_count)*(z_size)/(x_size)) + left_z;
                    *(read) = (z > (*read) || (*read) == 32767) ? z : (*read);
                }
                read++;
                x++;
                x_count++;
            }
            // Draw one extra to help fill in gaps...
            *(read) = (right_z > (*read) || (*read) == 32767) ? right_z : (*read);
        }
        y++;
    }
}

//-----------------------------------------------------------------------------------------------
float room_srf_waypoint_distance(unsigned char* waypoint_data, unsigned char from_waypoint, unsigned char to_waypoint)
{
    // <ZZ> This function returns the distance from one waypoint to another (straight line)...
    //      Waypoint_data is a pointer to the start of the COMPRESSED waypoint data...
    unsigned char num_waypoint;
    float x, y;
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    if(from_waypoint < num_waypoint && to_waypoint < num_waypoint)
    {
        x = (((signed short) sdf_read_unsigned_short(waypoint_data+(from_waypoint*4))) * ONE_OVER_256);
        y = (((signed short) sdf_read_unsigned_short(waypoint_data+(from_waypoint*4)+2)) * ONE_OVER_256);
        x -= (((signed short) sdf_read_unsigned_short(waypoint_data+(to_waypoint*4))) * ONE_OVER_256);
        y -= (((signed short) sdf_read_unsigned_short(waypoint_data+(to_waypoint*4)+2)) * ONE_OVER_256);
        return ((float) sqrt(x*x + y*y));
    }
    return 99999.0f;
}

//-----------------------------------------------------------------------------------------------
#define MAX_ROOM_WAYPOINT 128
#define WAYPOINT_NOT_TRAVERSED  0
#define WAYPOINT_BECOME_ACTIVE  1
#define WAYPOINT_ACTIVE         2
#define WAYPOINT_TRAVERSED      3
unsigned char room_waypoint_traversed[MAX_ROOM_WAYPOINT];
unsigned char room_waypoint_initial[MAX_ROOM_WAYPOINT];
float room_waypoint_distance[MAX_ROOM_WAYPOINT];
void room_srf_find_goto_waypoint(unsigned char* waypoint_data, unsigned char* waypoint_line_data, unsigned char initial_waypoint)
{
    // <ZZ> This function builds a slice of the waypoint lookup table (number of entries filled is equal to
    //      number of waypoints).  The waypoint lookup table is located after the waypoint_data, but before
    //      the waypoint_line_data (this is all COMPRESSED waypoint data, by the way)...  Initial_waypoint is
    //      the waypoint we're going from, and for every waypoint in the room we find the waypoint which should
    //      be visited next...  If room has 9 waypoints, then our table should be 81 entries long...
    unsigned short num_waypoint_line, number_of_changes, line;
    unsigned char  i, j, num_waypoint;
    unsigned char* read;
    unsigned char* write;
    float distance;



    // How many waypoints and how many lines do we have?
    num_waypoint = *(waypoint_data);
    num_waypoint_line = sdf_read_unsigned_short(waypoint_line_data);
    if(num_waypoint > MAX_ROOM_WAYPOINT)
    {
        // Shouldn't happen...
        log_message("ERROR:  Waypoint table not built...  Too many waypoints...");
        return;
    }
    if(initial_waypoint >= num_waypoint)
    {
        // Shouldn't happen...
        log_message("ERROR:  Invalid initial waypoint");
        return;
    }


    // Clear out our traversal array...  This function works sorta like a flood fill,
    // walking out from our initial waypoint until we've found all there is to find...
    repeat(i, num_waypoint)
    {
        room_waypoint_traversed[i] = WAYPOINT_NOT_TRAVERSED;
        room_waypoint_initial[i] = initial_waypoint;
        room_waypoint_distance[i] = 99999.0f;
    }

//log_message("INFO:   Searching from initial waypoint %d (%d total, %d lines)", initial_waypoint, num_waypoint, num_waypoint_line);


    // Set our initial waypoint...
    room_waypoint_traversed[initial_waypoint] = WAYPOINT_ACTIVE;
    room_waypoint_distance[initial_waypoint] = 0.0f;



    // Start traversing the waypoints...  Keep going until nothing changes...
    number_of_changes = 1;
    while(number_of_changes > 0)
    {
        number_of_changes = 0;


        // Handle each active waypoint...
        repeat(i, num_waypoint)
        {
            if(room_waypoint_traversed[i] == WAYPOINT_ACTIVE)
            {
                // Search waypoint line listing for any neighbors to the active waypoint...
                read = waypoint_line_data+2;
                repeat(line, num_waypoint_line)
                {
                    // Is our active waypoint in this line?
                    if(read[0] == i || read[1] == i)
                    {
                        // Yup, but which one is the neighbor...
                        j = (read[0] == i) ? read[1] : read[0];

                        // Error check...
                        if(j < num_waypoint)
                        {
                            // Now find the distance from our active waypoint to it's neighbor...
                            distance = room_srf_waypoint_distance(waypoint_data, i, j);
                            distance += room_waypoint_distance[i];


                            // Is our total distance better than the neighbor's total distance?
                            if(distance < room_waypoint_distance[j])
                            {
                                // Oooooh....  It is....  That means we should walk to our neighbor next...
                                room_waypoint_traversed[j] = WAYPOINT_BECOME_ACTIVE;
                                room_waypoint_distance[j] = distance;
                                room_waypoint_initial[j] = (i == initial_waypoint) ? j : room_waypoint_initial[i];  // Remember which chain we're on...
                                number_of_changes++;
//log_message("INFO:     Found %d on chain %d (distance %f)", j, room_waypoint_initial[j], distance);
                            }
                        }
                    }
                    read+=2;
                }


                // Active waypoints become traversed (inactive) for next time...
                room_waypoint_traversed[i] = WAYPOINT_TRAVERSED;
            }
        }


        // Waypoints flagged to become active should now do so...
        repeat(i, num_waypoint)
        {
            if(room_waypoint_traversed[i] == WAYPOINT_BECOME_ACTIVE)
            {
                room_waypoint_traversed[i] = WAYPOINT_ACTIVE;
            }
        }
    }


    // Write our output data...
    write = waypoint_data+2+((4+initial_waypoint)*num_waypoint);
    repeat(i, num_waypoint)
    {
//log_message("INFO:   From waypoint %d to %d, visit %d next", initial_waypoint, i, room_waypoint_initial[i]);
        (*(write)) = room_waypoint_initial[i];
        write++;
    }
}

//-----------------------------------------------------------------------------------------------
void room_setup_lighting(unsigned char* data, float* light_xyz, float ambient)
{
    // <ZZ> This function figgers out the final lighting values for an uncompressed room...
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned short num_vertex, num_tex_vertex;
    unsigned short i;
    float final_light_xyz[3];
    float temp;
    float* normal_xyz;


    // Read the header...
    vertex_data = data + (*((unsigned int*) (data+SRF_VERTEX_OFFSET)));
    num_vertex = *((unsigned short*) vertex_data);  vertex_data+=2;
    tex_vertex_data = data + (*((unsigned int*) (data+SRF_TEX_VERTEX_OFFSET)));
    num_tex_vertex = *((unsigned short*) tex_vertex_data);  tex_vertex_data+=2;


    // Normalize the lighting vector...
    temp = vector_length(light_xyz);
    if(temp < 0.01f)
    {
        temp = 0.01f;
    }
    ambient = ambient * 255.0f;
    final_light_xyz[X] = (light_xyz[X] * (255.0f - ambient)) / -temp;
    final_light_xyz[Y] = (light_xyz[Y] * (255.0f - ambient)) / -temp;
    final_light_xyz[Z] = (light_xyz[Z] * (255.0f - ambient)) / -temp;


    // Figger out the vertex & tex vertex lighting based on normals...
    repeat(i, num_vertex)
    {
        normal_xyz = (float*) (vertex_data+12);
        temp = ((255.0f-ambient) + dot_product(final_light_xyz, normal_xyz))*0.5f + ambient;
        clip(0.0f, temp, 255.0f);
        vertex_data[25] = (unsigned char) ((temp + 255.0f) * 0.5f);
        vertex_data[25] = (vertex_data[25]*vertex_data[24]) >> 8;
        vertex_data+=26;
    }
    repeat(i, num_tex_vertex)
    {
        normal_xyz = (float*) (tex_vertex_data+8);
        temp = ((255.0f-ambient) + dot_product(final_light_xyz, normal_xyz))*0.5f + ambient;
        clip(0.0f, temp, 255.0f);
        tex_vertex_data[21] = (unsigned char) temp;
        tex_vertex_data[21] = (tex_vertex_data[21]*tex_vertex_data[20]) >> 8;
        tex_vertex_data[21] = (tex_vertex_data[21]+255)>>1;
        tex_vertex_data+=22;
    }
}

//-----------------------------------------------------------------------------------------------
void room_set_texture_data(unsigned char* texture_data, unsigned char texture, unsigned int hardware_texture, unsigned char flags)
{
    // <ZZ> Simple little helper for writing room texture data...
    texture_data += (texture<<3);
    *((unsigned int*) texture_data) = hardware_texture;
    texture_data[4] = flags;
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro finds the xyz values of a given vertex in an srf file...  Does all the rotation
//      and stuff...  Assumes that vertex_data, sine, and cosine are all setup correctly...  Assumes
//      that the vertex number is valid...
#define room_draw_srf_vertex_helper(VERTEX)             \
{                                                       \
    temp_xyz[X] = (((signed short) sdf_read_unsigned_short(vertex_data+(VERTEX*6))) * ONE_OVER_256);  \
    temp_xyz[Y] = (((signed short) sdf_read_unsigned_short(vertex_data+(VERTEX*6)+2)) * ONE_OVER_256);  \
    temp_xyz[Z] = (((signed short) sdf_read_unsigned_short(vertex_data+(VERTEX*6)+4)) * ONE_OVER_256);  \
    vertex_xyz[X] = (cosine*temp_xyz[X]) - (sine*temp_xyz[Y]) + x; \
    vertex_xyz[Y] = (sine*temp_xyz[X]) + (cosine*temp_xyz[Y]) + y; \
    vertex_xyz[Z] = temp_xyz[Z] + z; \
}

//-----------------------------------------------------------------------------------------------
void room_spawn_all(unsigned char* srf_file, unsigned short rotation, unsigned char* door_wall_list, unsigned char* object_defeated_list, unsigned char difficulty, unsigned short seed, unsigned short object_group)
{
    // <ZZ> This function spawns all of the characters & particles in the room...
    //      Doors too...
    unsigned char* object_group_data;
    unsigned short num_group, num_object, i, j, num_exterior_wall, vertex, last_vertex;
    unsigned short length;
    unsigned char found_door, door_wall;
    unsigned char* script;
    unsigned char* child_data;
    unsigned char* model_data;
    unsigned char chance, type;
    float temp_xyz[3];
    float last_vertex_xyz[3];
    float vertex_xyz[3];
    float center_xyz[3];
    float perp_xyz[3];
    float perp_distance;
    float x, y, z, angle, sine, cosine;
    unsigned char* old_object_data;
    unsigned short old_object_item;
    unsigned char* read;
    unsigned char* vertex_data;
    unsigned short num_vertex;
    unsigned char door_flags;
    unsigned char object_number;
    unsigned char do_spawn;




    // Make sure we don't break things...
    old_object_data = current_object_data;
    old_object_item = current_object_item;


    // Find the rotation info...
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);



    // Read the object group data...
    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(num_group > 0)
    {
        // Make sure the object group is valid...
        object_group = object_group % num_group;


        // Skip through data to get to the current group...
        repeat(i, object_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }


        // Spawn each object...
        object_number = 0;
        num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
        repeat(i, num_object)
        {
            // Seed the random number generator...
            next_random = seed + i;


            // Check the object's percent chance of spawning...
            chance = random_number;
            if(chance <= ((unsigned char) (((object_group_data[13]&240)*255)/240)))
            {
                // Check this object's name, and make sure it's null terminated...
                object_group_data[12] = 0;
                length = strlen(object_group_data);
                if(length > 4)
                {
                    // Is it a .RUN or .SRC file?
                    if(((object_group_data[length-1] == 'N' && object_group_data[length-2] == 'U' && object_group_data[length-3] == 'R') || (object_group_data[length-1] == 'C' && object_group_data[length-2] == 'R' && object_group_data[length-3] == 'S')) && object_group_data[length-4] == '.')
                    {
                        // Yup, but is it a particle or a character?  Check prefix...
                        type = CHARACTER;
                        if(object_group_data[0] == 'P')
                        {
                            type = PARTICLE;
                        }


                        // Find the spawn coordinates...
                        room_draw_srf_object_helper();


                        // Find the file...
                        object_group_data[length-4] = 0;
                        script = sdf_find_filetype(object_group_data, SDF_FILE_IS_RUN);
                        object_group_data[length-4] = '.';
                        if(script)
                        {
                            script = (unsigned char*) sdf_read_unsigned_int(script);
                            global_spawn_team = object_group_data[14];


                            if(type == CHARACTER)
                            {
                                // Determine subtype of character (particles don't have subtypes...  but misc7 is set after spawning for 'em...)
                                global_spawn_subtype = object_group_data[13]&15;
                                switch(global_spawn_subtype)
                                {
                                    case 0:
                                        // Default...
                                        global_spawn_subtype = random_number;
                                        global_spawn_subtype = (global_spawn_subtype & 63);
                                        break;
                                    case 1:
                                        // Captain...
                                        global_spawn_subtype = random_number;
                                        global_spawn_subtype = (global_spawn_subtype & 63) | 64;
                                        break;
                                    case 2:
                                        // Super Captain... 
                                        global_spawn_subtype = random_number;
                                        global_spawn_subtype = (global_spawn_subtype & 63) | 128;
                                        break;
                                    case 3:
                                        // Random...
                                        global_spawn_subtype = random_number;
                                        break;
                                    case 4:
                                        // Subtype 0...
                                        global_spawn_subtype = 0;
                                        break;
                                    case 5:
                                        // Subtype 1...
                                        global_spawn_subtype = 64;
                                        break;
                                    case 6:
                                        // Subtype 2...
                                        global_spawn_subtype = 128;
                                        break;
                                    case 7:
                                        // Subtype 3...
                                        global_spawn_subtype = 192;
                                        break;
                                    default:
                                        // Cases 8-15...  For specifying eqcol01 more precisely...
                                        global_spawn_subtype = (global_spawn_subtype-8)<<5;
                                        break;
                                }
                            }


                            // Stuff for remembering which objects to spawn on room re-entry...
                            do_spawn = TRUE;
                            if(object_defeated_list)
                            {
                                if(type == CHARACTER && object_number < 64)
                                {
                                    do_spawn = object_defeated_list[object_number>>3];
                                    do_spawn = ((do_spawn>>(object_number&7))+1) & 1;
                                }
                            }

                            if(do_spawn)
                            {
                                child_data = obj_spawn(type, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], script, 65535);
                                if(child_data)
                                {
                                    // Set the current frame to be the first frame of the stand action...
                                    if(type == CHARACTER)
                                    {
                                        model_data = *((unsigned char**) (child_data+256));
                                        if(model_data)
                                        {
                                            model_data+=6+(ACTION_STAND<<1);
                                            (*((unsigned short*) (child_data+178))) = (*((unsigned short*) model_data));
                                        }
                                        // Set the aitimer
                                        (*((unsigned short*) (child_data+176))) = 1;

                                        // Set the object number...
                                        child_data[249] = object_number;
                                    }
                                    else
                                    {
                                        // Set misc7 for particles...
                                        child_data[75] = object_group_data[13]&15;
                                    }


                                    // Set the facing too...
                                    (*((unsigned short*) (child_data+56))) = (object_group_data[15] << 8) + rotation + 16384;
                                }
                            }
                            object_number++;
                        }
                    }
                }
            }
            object_group_data+=22;
        }
    }





    // Spawn all of the doors...
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(read);  read+=2;
    vertex = sdf_read_unsigned_short((read+(3*num_exterior_wall)-3));
    vertex_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    script = sdf_find_filetype("CDOOR", SDF_FILE_IS_RUN);
    if(script)
    {
        script = (unsigned char*) sdf_read_unsigned_int(script);
        repeat(i, num_exterior_wall)
        {
            // Seed the random number generator...
            next_random = seed + i;


            last_vertex = vertex;
            vertex = sdf_read_unsigned_short(read);  read+=2;
            door_flags = (*read)&15;  read++;


            // Check the wall list to see if there's a door in this segment...
            door_wall = 15;
            if(door_wall_list)
            {
                found_door = FALSE;
                repeat(j, 5)
                {
                    if(((unsigned char) door_wall_list[j]) == i)
                    {
                        found_door = TRUE;
                        door_wall = (unsigned char) j;
                    }
                }
            }
            else
            {
                found_door = TRUE;
            }


            // Does this wall segment have a door hole cut in it?
            if(door_flags >= ROOM_WALL_FLAG_LOW_DOOR && found_door)
            {
                // Yup, so find the midpoint of the wall segment...
                if(vertex < num_vertex && last_vertex < num_vertex)
                {
                    room_draw_srf_vertex_helper(last_vertex);
                    last_vertex_xyz[X] = vertex_xyz[X];
                    last_vertex_xyz[Y] = vertex_xyz[Y];
                    last_vertex_xyz[Z] = vertex_xyz[Z];
                    room_draw_srf_vertex_helper(vertex);
                    center_xyz[X]=(last_vertex_xyz[X] + vertex_xyz[X])*0.5f;
                    center_xyz[Y]=(last_vertex_xyz[Y] + vertex_xyz[Y])*0.5f;
                    center_xyz[Z]=(last_vertex_xyz[Z] + vertex_xyz[Z])*0.5f;
                    temp_xyz[X] = vertex_xyz[X]-last_vertex_xyz[X];
                    temp_xyz[Y] = vertex_xyz[Y]-last_vertex_xyz[Y];
                    perp_xyz[X] = -temp_xyz[Y];
                    perp_xyz[Y] = temp_xyz[X];
                    perp_distance = ((float) sqrt(perp_xyz[X]*perp_xyz[X] + perp_xyz[Y]*perp_xyz[Y])) + 0.00000001f;
                    perp_xyz[X]/=perp_distance;
                    perp_xyz[Y]/=perp_distance;
//                    perp_xyz[X]*=-2.85f;
//                    perp_xyz[Y]*=-2.85f;
                    perp_xyz[X]*=-0.9f;
                    perp_xyz[Y]*=-0.9f;
                    center_xyz[X]+=perp_xyz[X];
                    center_xyz[Y]+=perp_xyz[Y];



                    global_spawn_team = TEAM_NEUTRAL;
                    global_spawn_subtype = 0;
                    if(door_flags == ROOM_WALL_FLAG_LOW_DOOR)
                    {
                        global_spawn_subtype = 64;
                    }
                    if(door_flags == ROOM_WALL_FLAG_LOW_SHUTTER)
                    {
                        global_spawn_subtype = 128;
                    }
                    global_spawn_subtype |= door_wall;
                    child_data = obj_spawn(CHARACTER, center_xyz[X], center_xyz[Y], center_xyz[Z], script, 65535);
                    if(child_data)
                    {
                        // Set the current frame to be the first frame of the stand action...
                        model_data = *((unsigned char**) (child_data+256));
                        if(model_data)
                        {
                            model_data+=6+(ACTION_STAND<<1);
                            (*((unsigned short*) (child_data+178))) = (*((unsigned short*) model_data));
                        }
                        // Set the aitimer
                        (*((unsigned short*) (child_data+176))) = 1;


                        // Set the facing too...
                        (*((unsigned short*) (child_data+56))) = ((unsigned short) (atan2(temp_xyz[Y], temp_xyz[X])*10430.37835047f)) + 16384;
                    }
                }
            }
        }
    }


    // Make sure we don't break things...
    current_object_data = old_object_data;
    current_object_item = old_object_item;
}

//-----------------------------------------------------------------------------------------------
void room_edge_line_add(unsigned char* edge_line_data, unsigned short vertex_a, unsigned short vertex_b, unsigned short check_vertex)
{
    // <ZZ> This function is a helper for figgering edge lines for volumetric room shadows...
    //      Should be called twice for each line in the room geometry - but triangle edges
    //      without neighbors should only be called once...
    unsigned short i, num_line, a, b;
    unsigned char* write;
    write = edge_line_data;
    num_line = *((unsigned short*) write);
    write+=2;
    repeat(i, num_line)
    {
        a = *((unsigned short*) (write));
        b = *((unsigned short*) (write+2));
        if((vertex_a == a && vertex_b == b) || (vertex_a == b && vertex_b == a))
        {
            // This is the second add for this line...
            *((unsigned short*) (write+6)) = check_vertex;
            return;
        }
        write+=8;
    }
    // We didn't find the line we're trying to add, so let's add it already...
    *((unsigned short*) (write)) = vertex_a;
    *((unsigned short*) (write+2)) = vertex_b;
    *((unsigned short*) (write+4)) = check_vertex;
    *((unsigned short*) (write+6)) = check_vertex;
    num_line++;
    *((unsigned short*) edge_line_data) = num_line;
}

//-----------------------------------------------------------------------------------------------
void room_restock_monsters(unsigned char* srf_file, unsigned char* object_defeated_list, unsigned char difficulty, unsigned short seed, unsigned short object_group)
{
    // <ZZ> This function modifies the object defeated list to randomly restock one fourth of
    //      the monsters in the room...  Does not restock neutral and good characters...
    unsigned char* object_group_data;
    unsigned short num_group, num_object, i;
    unsigned char* script;
    unsigned char chance, type, restock_chance;
    unsigned short length;
    unsigned char object_number;
    unsigned char temp;
    unsigned short old_random;


    old_random = next_random;


    // Read the object group data...
    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(num_group > 0)
    {
        // Make sure the object group is valid...
        object_group = object_group % num_group;


        // Skip through data to get to the current group...
        repeat(i, object_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }


        // Search for the object to restock...
        object_number = 0;
        num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
        repeat(i, num_object)
        {
            // Seed the random number generator...
            // Check the object's percent chance of spawning...
            next_random = old_random + i;
            restock_chance = random_number;
            next_random = seed + i;
            chance = random_number;
            if(chance <= ((unsigned char) (((object_group_data[13]&240)*255)/240)))
            {
                // Check this object's name, and make sure it's null terminated...
                object_group_data[12] = 0;
                length = strlen(object_group_data);
                if(length > 4)
                {
                    // Is it a .RUN or .SRC file?
                    if(((object_group_data[length-1] == 'N' && object_group_data[length-2] == 'U' && object_group_data[length-3] == 'R') || (object_group_data[length-1] == 'C' && object_group_data[length-2] == 'R' && object_group_data[length-3] == 'S')) && object_group_data[length-4] == '.')
                    {
                        // Yup, but is it a particle or a character?  Check prefix...
                        type = CHARACTER;
                        if(object_group_data[0] == 'P')
                        {
                            type = PARTICLE;
                        }


                        // Find the file...
                        object_group_data[length-4] = 0;
                        script = sdf_find_filetype(object_group_data, SDF_FILE_IS_RUN);
                        object_group_data[length-4] = '.';
                        if(script)
                        {
                            script = (unsigned char*) sdf_read_unsigned_int(script);
                            if(type == CHARACTER && object_group_data[14] != TEAM_NEUTRAL && object_group_data[14] != TEAM_GOOD)
                            {
                                if(object_defeated_list)
                                {
                                    if(object_number < 64 && (restock_chance < 64))
                                    {
                                        // Clear out the object defeated bit for this character...
                                        temp = ~(1 << (object_number&7));
                                        object_defeated_list[object_number>>3] &= temp;
                                    }
                                }
                            }
                            object_number++;
                        }
                    }
                }
            }
            object_group_data+=22;
        }
    }


    next_random = old_random+1;
}

//-----------------------------------------------------------------------------------------------
unsigned char room_uncompress(unsigned char* srf_file, unsigned char* destination_buffer, unsigned char* ddd_wall_door_file, unsigned short rotation, unsigned char* door_wall_list, unsigned char* object_defeated_list, unsigned char difficulty, unsigned short seed, unsigned short object_group)
{
    // <ZZ> This function converts a .SRF room file into something useable in the game...  Converts units
    //      to system endian format & floats...  Puts in the walls and doors (which can be of different types
    //      or in different locations even if the srf file is the same)...  Strips up the triangle lists...
    //      Initializes all of the Bridges...  Optimizes the waypoint search web...  Creates the heightmap...
    //      And figures out all of the vertex normals...
    //
    //      Returns TRUE if it all worked...  FALSE if it all broked...
    unsigned short i, j, m, match, texture;
    signed short x, y, z;
    float fx, fy, fz;
    unsigned short vertex[3], tex_vertex[3];
    unsigned short num_vertex, num_tex_vertex, num_triangle, old_seed;
    unsigned char num_waypoint;
    unsigned char* num_write;
    unsigned char* read;
    unsigned char* write;
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    signed short* heightmap_data;
    unsigned char* texture_data;
    float* vertex_xyz[3];
    float ab_xyz[3];
    float bc_xyz[3];
    float normal_xyz[3];
    float length;
    float angle, sine, cosine;


//timer_start();
    // Remember the random number seed...
    old_seed = next_random;


    // First copy the name...
    memcpy(destination_buffer, srf_file, 32);
    destination_buffer[31] = 0;  // Force null termination...
//log_message("INFO:   Uncompressing Room - %s", destination_buffer);


    // Then some flags, flags, and water level...
    destination_buffer[32] = srf_file[32];  // Room Flags
    destination_buffer[33] = srf_file[33];  // Water Type
    destination_buffer[34] = srf_file[34];  // Water Level
    destination_buffer[35] = srf_file[35];  // Metal Box Item
    room_water_level = (srf_file[34] * 100.0f / 255.0f) - 50.0f;
    room_water_type = srf_file[33];
    room_metal_box_item = srf_file[35];
//log_message("INFO:     Room water info...  Type = %d, Level = %f", room_water_type, room_water_level);
    water_drown_delay_setup();


    // Then CPORTAL/CRANDOM monster types...  0 means Default which is a per room random monster...
    destination_buffer[36] = srf_file[36];  // Monster for CPORTAL subtype 0
    destination_buffer[37] = srf_file[37];  // Monster for CPORTAL subtype 1
    destination_buffer[38] = srf_file[38];  // Monster for CPORTAL subtype 2
    destination_buffer[39] = srf_file[39];  // Monster for CPORTAL subtype 3
    if(destination_buffer[36] == 0) { destination_buffer[36] = random_number; if(destination_buffer[36] < 1) { destination_buffer[36]=1; } } else { destination_buffer[36]--; }
    if(destination_buffer[37] == 0) { destination_buffer[37] = random_number; if(destination_buffer[37] < 1) { destination_buffer[37]=1; } } else { destination_buffer[37]--; }
    if(destination_buffer[38] == 0) { destination_buffer[38] = random_number; if(destination_buffer[38] < 1) { destination_buffer[38]=1; } } else { destination_buffer[38]--; }
    if(destination_buffer[39] == 0) { destination_buffer[39] = random_number; if(destination_buffer[39] < 1) { destination_buffer[39]=1; } } else { destination_buffer[39]--; }


    // Figger some rotation stuff...
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);


    // Get number of vertices in .SRF
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(read);


    // Get number of tex vertices in .SRF
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(read);


    // Copy all of the vertices that are in the .SRF file...
//log_message("INFO:   Copying SRF vertices");
    write = destination_buffer+96;
    *((unsigned int*) (destination_buffer+SRF_VERTEX_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_VERTEX_OFFSET);  read+=2;  // Already know how many...
    num_write = write;  write+=2;     // Need to come back and write final number of vertices
    vertex_data = write;
    repeat(i, num_vertex)
    {
//log_message("INFO:     Vertex %d of %d", i, num_vertex);
        x = (signed short) sdf_read_unsigned_short(read);  read+=2;
        y = (signed short) sdf_read_unsigned_short(read);  read+=2;
        z = (signed short) sdf_read_unsigned_short(read);  read+=2;
        fx = x * ONE_OVER_256;
        fy = y * ONE_OVER_256;
        fz = z * ONE_OVER_256;
        room_rotate_macro(fx, fy, sine, cosine, angle);


        // Position xyz
        *((float*) write) = fx;  write+=4;
        *((float*) write) = fy;  write+=4;
        *((float*) write) = fz;  write+=4;
//log_message("INFO:       Vertex is at (%f, %f, %f)", fx, fy, fz);


        // Normal xyz
        *((float*) write) = 0.0f; write+=4;
        *((float*) write) = 0.0f; write+=4;
        *((float*) write) = 0.0f; write+=4;

        // Lighting info
        *write = 255;  write++;
        *write = 255;  write++;
    }
    // Now add in all the new vertices using our magic little function...
    global_num_vertex = num_vertex;
    write = room_exterior_wall_function(EXTERIOR_VERTEX, srf_file, write, vertex_data, NULL, ddd_wall_door_file, door_wall_list, rotation);
    write = room_plop_all_function(EXTERIOR_VERTEX, write, destination_buffer, srf_file, ddd_wall_door_file, object_group, rotation);
    *((unsigned short*) num_write) = global_num_vertex;





    // Copy all of the tex vertices that are in the .SRF file...
//log_message("INFO:   Copying SRF tex vertices");
    *((unsigned int*) (destination_buffer+SRF_TEX_VERTEX_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_TEX_VERTEX_OFFSET);  read+=2;  // Already know how many...
    num_write = write;  write+=2;     // Need to come back and write final number of tex vertices
    tex_vertex_data = write;
    repeat(i, num_tex_vertex)
    {
//log_message("INFO:     Tex Vertex %d of %d", i, num_tex_vertex);
        x = (signed short) sdf_read_unsigned_short(read);  read+=2;
        y = (signed short) sdf_read_unsigned_short(read);  read+=2;
        fx = x * ONE_OVER_256;
        fy = y * ONE_OVER_256;

        // Position...
        *((float*) write) = fx;  write+=4;
        *((float*) write) = fy;  write+=4;
//log_message("INFO:       Tex vertex is at (%f, %f)", fx, fy);

        // Normal...
        (*((float*) write)) = 0.0f;  write+=4;
        (*((float*) write)) = 0.0f;  write+=4;
        (*((float*) write)) = 0.0f;  write+=4;

        // Lighting info...
        *write = 255;  write++;   // Lighting multiplier (for pits and random variety)
        *write = 0;  write++;     // Final lighting value (generated every draw cycle)
    }
    // Now add in all the new tex vertices using our magic little function...
    global_num_tex_vertex = num_tex_vertex;
    write = room_exterior_wall_function(EXTERIOR_TEX_VERTEX, srf_file, write, vertex_data, tex_vertex_data, ddd_wall_door_file, door_wall_list, rotation);
    write = room_plop_all_function(EXTERIOR_TEX_VERTEX, write, destination_buffer, srf_file, ddd_wall_door_file, object_group, rotation);
    *((unsigned short*) num_write) = global_num_tex_vertex;









    // Copy all of the triangles from the .SRF file...  32 bytes per triangle (so we can use same stripper from old DDD code)
    // THIS IS NOT FINAL TRIANGLE STRIP DATA, AND DOESN'T MATCH FORMAT!!!
//log_message("INFO:   Copying SRF triangles");
    *((unsigned int*) (destination_buffer+SRF_TRIANGLE_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_TRIANGLE_OFFSET);
    num_write = write;  write+=2;     // Need to come back and write final number of triangles
    triangle_data = write;
    global_num_triangle = 0;
    repeat(texture, MAX_ROOM_TEXTURE)
    {
//log_message("INFO:     Texture %d", texture);
        num_triangle = sdf_read_unsigned_short(read);  read+=2;
        repeat(i, num_triangle)
        {
//log_message("INFO:       Triangle %d of %d", i, num_triangle);
            repeat(j, 3)
            {
                vertex[j] = sdf_read_unsigned_short(read);  read+=2;
                tex_vertex[j] = sdf_read_unsigned_short(read);  read+=2;
            }


            // The new triangle data is written in the format that the old DDD stripper uses...
            *((unsigned short*) write) = 0;  write+=2;              //  0  Number of neighboring triangles (never more than 8)
            *((unsigned short*) write) = 65535;  write+=2;          //  2  Neighbor 0
            *((unsigned short*) write) = 65535;  write+=2;          //  4  Neighbor 1
            *((unsigned short*) write) = 65535;  write+=2;          //  6  Neighbor 2
            *((unsigned short*) write) = 65535;  write+=2;          //  8  Neighbor 3
            *((unsigned short*) write) = 65535;  write+=2;          // 10  Neighbor 4
            *((unsigned short*) write) = 65535;  write+=2;          // 12  Neighbor 5
            *((unsigned short*) write) = 65535;  write+=2;          // 14  Neighbor 6
            *((unsigned short*) write) = 65535;  write+=2;          // 16  Neighbor 7
            *((unsigned short*) write) = vertex[0];  write+=2;      // 18  Vertex 0
            *((unsigned short*) write) = vertex[1];  write+=2;      // 20  Vertex 1
            *((unsigned short*) write) = vertex[2];  write+=2;      // 22  Vertex 2
            *((unsigned short*) write) = tex_vertex[0];  write+=2;  // 24  Tex Vertex 0
            *((unsigned short*) write) = tex_vertex[1];  write+=2;  // 26  Tex Vertex 1
            *((unsigned short*) write) = tex_vertex[2];  write+=2;  // 28  Tex Vertex 2
            *write = (unsigned char) texture;  write+=1;            // 30  Texture to use for triangle
            *write = 0;  write+=1;                                  // 31  Triangle used somewhere
            global_num_triangle++;
        }
    }
    // Now add in all the new triangles using our magic little function...
    global_num_vertex = num_vertex;  // Need this because we simulate vertex adds...
    global_num_tex_vertex = num_tex_vertex;  // Need this because we need to know where tex vertices were first added...
    write = room_exterior_wall_function(EXTERIOR_TRIANGLE, srf_file, write, vertex_data, tex_vertex_data, ddd_wall_door_file, door_wall_list, rotation);
    write = room_plop_all_function(EXTERIOR_TRIANGLE, write, destination_buffer, srf_file, ddd_wall_door_file, object_group, rotation);
    *((unsigned short*) num_write) = global_num_triangle;





    // Fill in the neighbor information for each triangle...
    read = triangle_data;
    repeat(i, global_num_triangle)
    {
        vertex[0] = *((unsigned short*) (read+18));
        vertex[1] = *((unsigned short*) (read+20));
        vertex[2] = *((unsigned short*) (read+22));
        repeat(j, i)
        {
            match = 0;
            repeat(m, 3)
            {
                if(*((unsigned short*) (triangle_data+(j<<5)+18+(m<<1))) == vertex[0] || *((unsigned short*) (triangle_data+(j<<5)+18+(m<<1))) == vertex[1] || *((unsigned short*) (triangle_data+(j<<5)+18+(m<<1))) == vertex[2])
                {
                    match++;
                }
            }
            if(match > 1)
            {
                // Triangles are neighbors...
                // Record in the lower indexed, if possible...
                if(*((unsigned short*) (triangle_data+(j<<5))) < 8)
                {
                    m = *((unsigned short*) (triangle_data+(j<<5)));
                    *((unsigned short*) (triangle_data+(j<<5)+2+(m<<1))) = i;
                    *((unsigned short*) (triangle_data+(j<<5))) += 1;
                }
                // Record in the current, if possible...
                if(*((unsigned short*) (triangle_data+(i<<5))) < 8)
                {
                    m = *((unsigned short*) (triangle_data+(i<<5)));
                    *((unsigned short*) (triangle_data+(i<<5)+2+(m<<1))) = j;
                    *((unsigned short*) (triangle_data+(i<<5))) += 1;
                }
            }
        }
        read+=32;
    }



    // Now let's find all of the vertex (and tex vertex) normals, by accumulating the normals
    // of their triangles...
    read = triangle_data;
    repeat(i, global_num_triangle)
    {
        // Find the points of this triangle...
        vertex[0] = *((unsigned short*) (read+18));
        vertex[1] = *((unsigned short*) (read+20));
        vertex[2] = *((unsigned short*) (read+22));
        tex_vertex[0] = *((unsigned short*) (read+24));
        tex_vertex[1] = *((unsigned short*) (read+26));
        tex_vertex[2] = *((unsigned short*) (read+28));

        // Find the normal of the triangle by crossing two of its sidelines...
        vertex_xyz[0] = (float*) (vertex_data + (vertex[0]*26));
        vertex_xyz[1] = (float*) (vertex_data + (vertex[1]*26));
        vertex_xyz[2] = (float*) (vertex_data + (vertex[2]*26));
        ab_xyz[X] = vertex_xyz[1][X] - vertex_xyz[0][X];
        ab_xyz[Y] = vertex_xyz[1][Y] - vertex_xyz[0][Y];
        ab_xyz[Z] = vertex_xyz[1][Z] - vertex_xyz[0][Z];
        bc_xyz[X] = vertex_xyz[2][X] - vertex_xyz[1][X];
        bc_xyz[Y] = vertex_xyz[2][Y] - vertex_xyz[1][Y];
        bc_xyz[Z] = vertex_xyz[2][Z] - vertex_xyz[1][Z];
        cross_product(ab_xyz, bc_xyz, normal_xyz);
        length = vector_length(normal_xyz);
        if(length > 0.0001f)
        {
            normal_xyz[X]/=length;
            normal_xyz[Y]/=length;
            normal_xyz[Z]/=length;
        }


        // Set tex vertex lighting multiplier (lowest value of any vertex values)...
        if((vertex_data + (vertex[0]*26))[24] < (tex_vertex_data + (tex_vertex[0]*22))[20]) { (tex_vertex_data + (tex_vertex[0]*22))[20] = (vertex_data + (vertex[0]*26))[24]; }
        if((vertex_data + (vertex[1]*26))[24] < (tex_vertex_data + (tex_vertex[1]*22))[20]) { (tex_vertex_data + (tex_vertex[1]*22))[20] = (vertex_data + (vertex[1]*26))[24]; }
        if((vertex_data + (vertex[2]*26))[24] < (tex_vertex_data + (tex_vertex[2]*22))[20]) { (tex_vertex_data + (tex_vertex[2]*22))[20] = (vertex_data + (vertex[2]*26))[24]; }


//log_message("INFO:   Triangle Normal...");
//log_message("INFO:     Vertex 0 is at (%f, %f, %f)", vertex_xyz[0][X], vertex_xyz[0][Y], vertex_xyz[0][Z]);
//log_message("INFO:     Vertex 1 is at (%f, %f, %f)", vertex_xyz[1][X], vertex_xyz[1][Y], vertex_xyz[1][Z]);
//log_message("INFO:     Vertex 2 is at (%f, %f, %f)", vertex_xyz[2][X], vertex_xyz[2][Y], vertex_xyz[2][Z]);
//log_message("INFO:     Normal is (%f, %f, %f)", normal_xyz[X], normal_xyz[Y], normal_xyz[Z]);



        // Accumulate the normal into the vertices and tex vertices...
        vertex_xyz[0]+=3;  vertex_xyz[1]+=3;  vertex_xyz[2]+=3;  // Skip over position info
        vertex_xyz[0][X] += normal_xyz[X];  vertex_xyz[0][Y] += normal_xyz[Y];  vertex_xyz[0][Z] += normal_xyz[Z];
        vertex_xyz[1][X] += normal_xyz[X];  vertex_xyz[1][Y] += normal_xyz[Y];  vertex_xyz[1][Z] += normal_xyz[Z];
        vertex_xyz[2][X] += normal_xyz[X];  vertex_xyz[2][Y] += normal_xyz[Y];  vertex_xyz[2][Z] += normal_xyz[Z];
        vertex_xyz[0] = (float*) (tex_vertex_data + (tex_vertex[0]*22) + 8);
        vertex_xyz[1] = (float*) (tex_vertex_data + (tex_vertex[1]*22) + 8);
        vertex_xyz[2] = (float*) (tex_vertex_data + (tex_vertex[2]*22) + 8);
        vertex_xyz[0][X] += normal_xyz[X];  vertex_xyz[0][Y] += normal_xyz[Y];  vertex_xyz[0][Z] += normal_xyz[Z];
        vertex_xyz[1][X] += normal_xyz[X];  vertex_xyz[1][Y] += normal_xyz[Y];  vertex_xyz[1][Z] += normal_xyz[Z];
        vertex_xyz[2][X] += normal_xyz[X];  vertex_xyz[2][Y] += normal_xyz[Y];  vertex_xyz[2][Z] += normal_xyz[Z];
        read+=32;
    }
    // Now go through all vertices, normalizing normals...
    write = vertex_data;
    repeat(i, global_num_vertex)
    {
        normal_xyz[X] = *((float*) (write+12));
        normal_xyz[Y] = *((float*) (write+16));
        normal_xyz[Z] = *((float*) (write+20));
        length = vector_length(normal_xyz);
        if(length > 0.0001f)
        {
            normal_xyz[X]/=length;
            normal_xyz[Y]/=length;
            normal_xyz[Z]/=length;
        }
        *((float*) (write+12)) = normal_xyz[X];
        *((float*) (write+16)) = normal_xyz[Y];
        *((float*) (write+20)) = normal_xyz[Z];

        // If vertex is below pit level, paint it black...
        if(((float*) write)[Z] < ROOM_PIT_HIGH_LEVEL)
        {
            if(((float*) write)[Z] < ROOM_PIT_LOW_LEVEL)
            {
                write[24] = 0;
            }
            else
            {
                write[24] -= (unsigned char) ((0.0625f * (ROOM_PIT_HIGH_LEVEL - ((float*) write)[Z])) * write[24]);
            }
        }
        write+=26;
    }
    // Now go through all tex vertices, normalizing normals...
    write = tex_vertex_data;
    repeat(i, global_num_tex_vertex)
    {
        normal_xyz[X] = *((float*) (write+8));
        normal_xyz[Y] = *((float*) (write+12));
        normal_xyz[Z] = *((float*) (write+16));
        length = vector_length(normal_xyz);
        if(length > 0.0001f)
        {
            normal_xyz[X]/=length;
            normal_xyz[Y]/=length;
            normal_xyz[Z]/=length;
        }
        *((float*) (write+8)) = normal_xyz[X];
        *((float*) (write+12)) = normal_xyz[Y];
        *((float*) (write+16)) = normal_xyz[Z];
        write+=22;
    }



    // Do the lighting...
    normal_xyz[X] = 1.0f;  // Light direction
    normal_xyz[Y] = 0.5f;
    normal_xyz[Z] = -1.0f;
    room_setup_lighting(destination_buffer, normal_xyz, 0.25f);




    // Strip up the triangles using the old DDD stripper...  Strip info written into a
    // temporary buffer...
//log_message("INFO:   Stripping up triangles using DDD Stripper...");
    read = triangle_data;
    write = thirdbuffer;
    repeat(i, global_num_triangle)
    {
        while((*(read+31) & USED_PERMANENT) == 0)
        {
//log_message("INFO:     Stripping triangle %d (vert %d, %d, %d) (tex %d, %d, %d)", i, *((unsigned short*) (read+18)), *((unsigned short*) (read+20)), *((unsigned short*) (read+22)), *((unsigned short*) (read+24)), *((unsigned short*) (read+26)), *((unsigned short*) (read+28)));
            write = ddd_create_strip(triangle_data, write, i, global_num_triangle);
        }
        read+=32;
    }


    // The thirdbuffer *should* look something like this now...
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




//log_message("INFO:   Dumping strip info from ddd_create_strip...");
//read = thirdbuffer;
//while(read[0] == 'S')
//{
//    // How many vertices are in the strip?
//    num_vertex = *((unsigned short*) (read+2));
//log_message("INFO:   '%c'  (unsigned char) (Strip start)", read[0]);
//log_message("INFO:   %d   (unsigned char) (Texture)", read[1]);
//log_message("INFO:   %d   (unsigned short) (Num Vertex)", num_vertex);
//    read+=4;
//    repeat(i, num_vertex)
//    {
//        j = *((unsigned short*) read);  read+=2;  // Vertex
//        m = *((unsigned short*) read);  read+=2;  // Tex Vertex
//log_message("INFO:   %d   (unsigned short) (Vertex)", j);
//log_message("INFO:   %d   (unsigned short) (Tex Vertex)", m);
//    }
//}







    // Read through the temporary strip information, and write the final strip data into the
    // triangle data block...
    // For each texture...
//log_message("INFO:   Copying Stripped Triangles into Final Room Data...");
    triangle_data-=2;  // No longer need count...
    write = triangle_data;
    repeat(texture, MAX_ROOM_TEXTURE)
    {
        // Reset the counter for how many triangles are in this texture, and leave a spot
        // to come back and write it later...
        num_triangle = 0;
        num_write = write;
        write+=2;
//log_message("INFO:     Texture %d", texture);


        // For each triangle strip (in temporary data)
        read = thirdbuffer;
        while(read[0] == 'S')
        {
            // How many vertices are in the strip?
            num_vertex = *((unsigned short*) (read+2));

            // If triangle strip uses current texture...
            if(read[1] == texture)
            {
                // Copy this strip and remember that we found one...
                read+=4;
                num_triangle++;

                *((unsigned short*) write) = num_vertex;  write+=2;
//log_message("INFO:       %d vertices in strip", num_vertex);
                repeat(i, num_vertex)
                {
                    j = *((unsigned short*) read);  read+=2;  // Vertex
                    m = *((unsigned short*) read);  read+=2;  // Tex Vertex
                    *((unsigned short*) write) = j; write+=2; // Vertex
                    *((unsigned short*) write) = m; write+=2; // Tex Vertex
//log_message("INFO:         Vertex %d, Tex Vertex %d", j, m);
                }
            }
            else
            {
                // Skip over this strip...
                read+=4;
                read+=num_vertex*4;
            }
        }


        // Go back and write the final num_triangle for this texture...
        *((unsigned short*) num_write) = num_triangle;
//log_message("INFO:     %d strips found for texture %d", num_triangle, texture);
    }










    // Fill in the texture data...
    *((unsigned int*) (destination_buffer+SRF_TEXTURE_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_TEXTURE_OFFSET);
    texture_data = write;
    repeat(i, MAX_ROOM_TEXTURE)
    {
        // Clear it out...
        *((unsigned int*) write) = 0;
        *((unsigned int*) (write+4)) = 0;
        *(write+4) = *read;  // Texture flags...
        read++;
        write+=8;
    }



    // Now write the heightmap data...
//log_message("INFO:   Building the heightmap...");
    heightmap_data = (signed short*) write;
    *((unsigned int*) (destination_buffer+SRF_HEIGHTMAP_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    room_heightmap_clear(heightmap_data);
    write += (ROOM_HEIGHTMAP_SIZE*ROOM_HEIGHTMAP_SIZE*2);



    // Go through each of the textures in the room...  Last two textures are reserved for water, and are never written into heightmap...
    read = triangle_data;
    repeat(texture, (MAX_ROOM_TEXTURE-2))
    {
        // Go through each triangle strip...
        num_triangle = *((unsigned short*) read);  read+=2;
//log_message("INFO:     %d strips for texture %d", num_triangle, texture);
        repeat(i, num_triangle)
        {
            // Go through each vertex in strip...
            num_vertex = *((unsigned short*) read);  read+=2;
//log_message("INFO:       %d vertices for strip %d", num_vertex, i);


            repeat(j, num_vertex)
            {
                // Find coordinates for each vertex in the strip...
                m = (*((unsigned short*) read));  read+=4;
                vertex_xyz[j%3] = (float*) (vertex_data+(m*26));
//log_message("INFO:         Vertex %d (%f, %f, %f)", m, vertex_xyz[j%3][X], vertex_xyz[j%3][Y], vertex_xyz[j%3][Z]);

                // After 3 vertices are found, start adding to the heightmap...
                if(j > 1)
                {
                    // Does this texture get added to the heightmap?
                    if(!((texture_data + (texture<<3))[4] & ROOM_TEXTURE_FLAG_NOHEIGHT))
                    {
                        room_heightmap_triangle(heightmap_data, vertex_xyz[0], vertex_xyz[1], vertex_xyz[2]);
                    }
                }
            }
        }
    }


// !!!BAD!!!
// !!!BAD!!!  Dump the heightmap...
// !!!BAD!!!
//timer_end();
//timer_start();
//    display_export_tga("HEIGHT.TGA", ((unsigned char*) heightmap_data), ROOM_HEIGHTMAP_SIZE, ROOM_HEIGHTMAP_SIZE, -2);
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!




    // Copy waypoint information...
    *((unsigned int*) (destination_buffer+SRF_WAYPOINT_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    read = srf_file + sdf_read_unsigned_int(srf_file+SRF_WAYPOINT_OFFSET);
    num_waypoint = *read;  read+=2;
    *(write) = num_waypoint;  write++;
    *(write) = 0;  write++;  // Padding
    repeat(i, num_waypoint)
    {
        x = (signed short) sdf_read_unsigned_short(read);  read+=2;
        y = (signed short) sdf_read_unsigned_short(read);  read+=2;
        fx = x * ONE_OVER_256;
        fy = y * ONE_OVER_256;
        room_rotate_macro(fx, fy, sine, cosine, angle);

        // Position xyz
        *((float*) write) = fx;  write+=4;
        *((float*) write) = fy;  write+=4;
//log_message("INFO:     Waypoint %d of %d is at (%f, %f)", i, num_waypoint, fx, fy);
    }
    repeat(i, num_waypoint)
    {
        repeat(j, num_waypoint)
        {
            *(write) = *(read);  read++;  write++;
        }
    }
//log_message("INFO:     Copied (%d*%d) bytes of waypoint table", num_waypoint, num_waypoint);



    // Now spawn all of the objects...
    room_spawn_all(srf_file, rotation, door_wall_list, object_defeated_list, difficulty, seed, object_group);



    // Now generate all the volumetric edge line stuff (if we're doing pretty shadows)
    *((unsigned int*) (destination_buffer+SRF_EDGE_LINE_OFFSET)) = ((unsigned int) write)-((unsigned int)destination_buffer);
    *((unsigned short*) write) = 0;
    if(volumetric_shadows_on)
    {
        read = triangle_data;
        repeat(texture, (MAX_ROOM_TEXTURE-2))
        {
            // Go through each triangle strip...
            num_triangle = *((unsigned short*) read);  read+=2;
            repeat(i, num_triangle)
            {
                // Go through each vertex in strip...
                num_vertex = *((unsigned short*) read);  read+=2;
                repeat(j, num_vertex)
                {
                    // Make list of vertices in each strip...
                    vertex[j%3] = (*((unsigned short*) read));  read+=4;


                    // After 3 vertices are found, start adding to the heightmap...
                    if(j > 1)
                    {
                        // Does this texture get volumetric shadows?
                        if(!((texture_data + (texture<<3))[4] & (ROOM_TEXTURE_FLAG_NODRAW | ROOM_TEXTURE_FLAG_NOSHADOW)))
                        {
                            room_edge_line_add(write, vertex[0], vertex[1], vertex[2]);
                            room_edge_line_add(write, vertex[1], vertex[2], vertex[0]);
                            room_edge_line_add(write, vertex[2], vertex[0], vertex[1]);
                        }
                    }
                }
            }
        }
    }
    write+= ((*((unsigned short*) write)) * 8) + 2;  // Skip over the edge stuff...






    // Figger the lowest z value in the room...
    if(volumetric_shadows_on)
    {
        read = vertex_data;
        global_volumetric_shadow_vector_xyzz[3] = 0.0f;
        repeat(i, global_num_vertex)
        {
            fz = *((float*) (read+8));
            global_volumetric_shadow_vector_xyzz[3] = (fz < global_volumetric_shadow_vector_xyzz[3]) ? fz : global_volumetric_shadow_vector_xyzz[3];
            read+=26;
        }
        global_volumetric_shadow_vector_xyzz[3] -= 0.1f;
    }




    // Unseed the random number generator...
    next_random = old_seed;


//timer_end();
    return TRUE;
}


//-----------------------------------------------------------------------------------------------
// <ZZ> This macro finds the xy values of a given waypoint in an srf file...  Does all the rotation
//      and stuff...  Assumes that vertex_data, num_vertex, sine, and cosine are all setup correctly...
#define room_draw_srf_waypoint_helper(WAYPOINT)         \
{                                                       \
    temp_xyz[X] = (((signed short) sdf_read_unsigned_short(waypoint_data+(WAYPOINT*4))) * ONE_OVER_256);  \
    temp_xyz[Y] = (((signed short) sdf_read_unsigned_short(waypoint_data+(WAYPOINT*4)+2)) * ONE_OVER_256);  \
    vertex_xyz[X] = (cosine*temp_xyz[X]) - (sine*temp_xyz[Y]) + x; \
    vertex_xyz[Y] = (sine*temp_xyz[X]) + (cosine*temp_xyz[Y]) + y; \
    vertex_xyz[Z] = 0.0f; \
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro finds the xy values of a given tex vertex in an srf file...
#define room_draw_srf_tex_vertex_helper(TEX_VERTEX)         \
{                                                       \
    vertex_xyz[X] = (((signed short) sdf_read_unsigned_short(tex_vertex_data+(TEX_VERTEX*4))) * ONE_OVER_256);  \
    vertex_xyz[Y] = (((signed short) sdf_read_unsigned_short(tex_vertex_data+(TEX_VERTEX*4)+2)) * ONE_OVER_256);  \
    vertex_xyz[Z] = 0.0f; \
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro finds the xy values of a given bridge point in an srf file...  Does all the rotation
//      and stuff...  Assumes that vertex_data, num_vertex, sine, and cosine are all setup correctly...
#define room_draw_srf_bridge_helper(BRIDGE, POINT)         \
{                                                       \
    temp_xyz[X] = (((signed short) sdf_read_unsigned_short(bridge_data+(BRIDGE*8)+(POINT*4))) * ONE_OVER_256);  \
    temp_xyz[Y] = (((signed short) sdf_read_unsigned_short(bridge_data+(BRIDGE*8)+(POINT*4)+2)) * ONE_OVER_256);  \
    vertex_xyz[X] = (cosine*temp_xyz[X]) - (sine*temp_xyz[Y]) + x; \
    vertex_xyz[Y] = (sine*temp_xyz[X]) + (cosine*temp_xyz[Y]) + y; \
    vertex_xyz[Z] = 0.0f; \
}

//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// Stuff for finding screen corners
#define SCREEN_FORE_SCALE (-ZNEAR)
#define SCREEN_LEFT_SCALE (screen_frustum_x)
#define SCREEN_UP_SCALE (screen_frustum_y)
void room_get_point_xyz(float percent_x, float percent_y, float* final_x, float* final_y, float* final_z, float scale, unsigned char z_finder_mode, unsigned char snap_mode)
{
    // <ZZ> This function projects an xy screen point onto the 0 z plane of the room editor,
    //      and returns that position...
    unsigned short i, corner;
    float screen_corner_xyz[4][3];
    float screen_point_xyz[3];
    float vector_xyz[3];
    float normal_xyz[3];
    float distance;


    // Scooch the camera back a bit, so I can find the corners by goin' forward...
    window3d_camera_xyz[X] -= rotate_camera_matrix[1] * SCREEN_FORE_SCALE;
    window3d_camera_xyz[Y] -= rotate_camera_matrix[5] * SCREEN_FORE_SCALE;
    window3d_camera_xyz[Z] -= rotate_camera_matrix[9] * SCREEN_FORE_SCALE;

    // Find the screen corners
    // Top corners...
    corner = 0;
    repeat(i, 2)
    {
        screen_corner_xyz[corner][X] = window3d_camera_xyz[X]
                                  + rotate_camera_matrix[0] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[1] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[2] * SCREEN_UP_SCALE;
        screen_corner_xyz[corner][Y] = window3d_camera_xyz[Y]
                                  + rotate_camera_matrix[4] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[5] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[6] * SCREEN_UP_SCALE;
        screen_corner_xyz[corner][Z] = window3d_camera_xyz[Z]
                                  + rotate_camera_matrix[8] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[9] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[10] * SCREEN_UP_SCALE;
        corner++;
    }
    // Bottom corners...
    corner = 3;
    repeat(i, 2)
    {
        screen_corner_xyz[corner][X] = window3d_camera_xyz[X]
                                  + rotate_camera_matrix[0] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[1] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[2] * -SCREEN_UP_SCALE;
        screen_corner_xyz[corner][Y] = window3d_camera_xyz[Y]
                                  + rotate_camera_matrix[4] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[5] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[6] * -SCREEN_UP_SCALE;
        screen_corner_xyz[corner][Z] = window3d_camera_xyz[Z]
                                  + rotate_camera_matrix[8] * SCREEN_LEFT_SCALE * ((i<<1)-1)
                                  + rotate_camera_matrix[9] * SCREEN_FORE_SCALE
                                  + rotate_camera_matrix[10] * -SCREEN_UP_SCALE;
        corner--;
    }


    // Use the screen corner positions to find where our screen point is between 'em...
    screen_point_xyz[X] = (screen_corner_xyz[2][X]*percent_x) + (screen_corner_xyz[3][X]*(1.0f-percent_x));  // Point along bottom of screen...
    screen_point_xyz[Y] = (screen_corner_xyz[2][Y]*percent_x) + (screen_corner_xyz[3][Y]*(1.0f-percent_x));
    screen_point_xyz[Z] = (screen_corner_xyz[2][Z]*percent_x) + (screen_corner_xyz[3][Z]*(1.0f-percent_x));
    vector_xyz[X] = (screen_corner_xyz[1][X]*percent_x) + (screen_corner_xyz[0][X]*(1.0f-percent_x));  // Point along top of screen...
    vector_xyz[Y] = (screen_corner_xyz[1][Y]*percent_x) + (screen_corner_xyz[0][Y]*(1.0f-percent_x));
    vector_xyz[Z] = (screen_corner_xyz[1][Z]*percent_x) + (screen_corner_xyz[0][Z]*(1.0f-percent_x));
    screen_point_xyz[X] = (screen_point_xyz[X]*percent_y) + (vector_xyz[X]*(1.0f-percent_y));  // Final point projected between 3D onscreen corner locations
    screen_point_xyz[Y] = (screen_point_xyz[Y]*percent_y) + (vector_xyz[Y]*(1.0f-percent_y));
    screen_point_xyz[Z] = (screen_point_xyz[Z]*percent_y) + (vector_xyz[Z]*(1.0f-percent_y));


    // Use that screen point position to calculate the screen point ray...
    vector_xyz[X] = screen_point_xyz[X] - window3d_camera_xyz[X];
    vector_xyz[Y] = screen_point_xyz[Y] - window3d_camera_xyz[Y];
    vector_xyz[Z] = screen_point_xyz[Z] - window3d_camera_xyz[Z];


    // Two possible operation modes...
    if(z_finder_mode)
    {
        // We want to find the z at the given xy location (we're holding shift for a riser line)...
        // To do that, we collide our ray with the plane that runs through our xy location and
        // whose normal is opposite our camera fore xyz...  Find point where dot product is 0...
        normal_xyz[X] = rotate_camera_matrix[1];
        normal_xyz[Y] = rotate_camera_matrix[5];
        normal_xyz[Z] = 0.0f; //rotate_camera_matrix[9];
        distance = vector_length(normal_xyz);
        if(distance > 0.0f)
        {
            normal_xyz[X]/=distance;  normal_xyz[Y]/=distance;
            distance = (normal_xyz[X]*(*final_x) - normal_xyz[X]*screen_point_xyz[X] + normal_xyz[Y]*(*final_y) - normal_xyz[Y]*screen_point_xyz[Y]) / (normal_xyz[Y]*vector_xyz[Y] + normal_xyz[X]*vector_xyz[X]);
            *final_z = screen_point_xyz[Z] + vector_xyz[Z]*distance;
        }
    }
    else
    {
        // Collide the screen point ray with our 0-z plane...
//        distance = window3d_camera_xyz[Z] /  -vector_xyz[Z];

        // Collide with our current z position's plane...
        distance = (window3d_camera_xyz[Z]-(*final_z)) /  -vector_xyz[Z];
        *final_x = window3d_camera_xyz[X] + (distance*vector_xyz[X]);
        *final_y = window3d_camera_xyz[Y] + (distance*vector_xyz[Y]);
    }


    // Snap to nearest foot...
    if(snap_mode)
    {
        *final_x = (float) ((int) (*final_x));
        *final_y = (float) ((int) (*final_y));
        *final_z = (float) ((int) (*final_z));
    }


    // Limit x and y locations by scale...
    clip(-scale, (*final_x), scale);
    clip(-scale, (*final_y), scale);


    // Limit z by room pit level...
    clip(ROOM_PIT_LOW_LEVEL, (*final_z), -(ROOM_PIT_LOW_LEVEL));



    // Restore the camera to where it was...
    window3d_camera_xyz[X] += rotate_camera_matrix[1] * SCREEN_FORE_SCALE;
    window3d_camera_xyz[Y] += rotate_camera_matrix[5] * SCREEN_FORE_SCALE;
    window3d_camera_xyz[Z] += rotate_camera_matrix[9] * SCREEN_FORE_SCALE;
}

//-----------------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// Stuff for editing SRF files...  Also do some magic stuff in sdf_insert_data to keep offsets good...
#ifdef DEVTOOL
void room_srf_move(void)
{
    // <ZZ> This function moves vertices and stuff around in an SRF file
    unsigned short index;
    float x, y, z;
    float tx, ty, tz;
    float distance;
    float rotate_matrix[9];


    // Move, Rotate, or Scale selected vertices...
    if(selection_move == MOVE_MODE_ROTATE)
    {
        // Build the rotation matrix...
        x = select_offset_xyz[X] - select_center_xyz[X];
        y = select_offset_xyz[Y] - select_center_xyz[Y];
        z = 0;
        distance = (float) sqrt(x*x + y*y + z*z);
        if(distance > 0.20f)
        {
            // Start from identity
            rotate_matrix[0] = 1;  rotate_matrix[3] = 0; rotate_matrix[6] = 0;
            rotate_matrix[1] = 0;  rotate_matrix[4] = 1; rotate_matrix[7] = 0;
            rotate_matrix[2] = 0;  rotate_matrix[5] = 0; rotate_matrix[8] = 1;
            x = x/distance;  y = y/distance;  z = z/distance;

            // Rotation in XY plane (top)
            rotate_matrix[0] = y;  rotate_matrix[3] = x;
            rotate_matrix[1] = -x;  rotate_matrix[4] = y;
        }
        else
        {
            return;
        }
    }


    repeat(index, room_select_num)
    {
        x = 0.0f;  y = 0.0f;  z = 0.0f;
        if(room_select_axes[index] > 0)  { x = room_select_xyz[index][X]; }
        if(room_select_axes[index] > 1)  { y = room_select_xyz[index][Y]; }
        if(room_select_axes[index] > 2)  { z = room_select_xyz[index][Z]; }


        if(selection_move == MOVE_MODE_MOVE)
        {
            // Translation
            x += select_offset_xyz[X] - select_center_xyz[X];
            y += select_offset_xyz[Y] - select_center_xyz[Y];
            z += select_offset_xyz[Z] - select_center_xyz[Z];
        }
        else if(selection_move == MOVE_MODE_SCALE)
        {
            // Scaling
            x = ((x-select_center_xyz[X])*(1.0f + 0.05f*(select_offset_xyz[X] - select_center_xyz[X]))) + select_center_xyz[X];
            y = ((y-select_center_xyz[Y])*(1.0f + 0.05f*(select_offset_xyz[Y] - select_center_xyz[Y]))) + select_center_xyz[Y];
            z = ((z-select_center_xyz[Z])*(1.0f + 0.05f*(select_offset_xyz[Z] - select_center_xyz[Z]))) + select_center_xyz[Z];
        }
        else if(selection_move == MOVE_MODE_ROTATE)
        {
            // Rotation
            x -= select_center_xyz[X];
            y -= select_center_xyz[Y];
            z -= select_center_xyz[Z];
            tx = x*rotate_matrix[0] + y*rotate_matrix[3] + z*rotate_matrix[6];
            ty = x*rotate_matrix[1] + y*rotate_matrix[4] + z*rotate_matrix[7];
            tz = x*rotate_matrix[2] + y*rotate_matrix[5] + z*rotate_matrix[8];
            x = tx + select_center_xyz[X];
            y = ty + select_center_xyz[Y];
            z = tz + select_center_xyz[Z];
        }

        // Bounds
        clip(-100.0f, x, 100.0f);
        clip(-100.0f, y, 100.0f);
        clip(ROOM_PIT_LOW_LEVEL, z, -(ROOM_PIT_LOW_LEVEL));

        // Save
        if(room_select_axes[index] > 0)
        {
            sdf_write_unsigned_short(room_select_data[index], (unsigned short) ((signed short) (x*256.0f)) );
        }
        if(room_select_axes[index] > 1)
        {
            sdf_write_unsigned_short(room_select_data[index]+2, (unsigned short) ((signed short) (y*256.0f)) );
        }
        if(room_select_axes[index] > 2)
        {
            sdf_write_unsigned_short(room_select_data[index]+4, (unsigned short) ((signed short) (z*256.0f)) );
        }
    }
}
void room_srf_triangle_vertex_replace(unsigned char* data, unsigned short vertex_to_replace, unsigned short vertex_to_use_instead, unsigned char vertex_was_deleted)
{
    // <ZZ> This function goes through all of the triangles in an SRF file, replacing any reference to the given vertex
    //      with another...  if vertex_was_deleted, then we need to shuffle back our references instead...
    unsigned char* triangle_data;
    unsigned short num_triangle, texture, i, j, vertex;
    unsigned char skip_triangle;
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
//log_message("INFO:   Replacing all references to room vertex %d with %d", vertex_to_replace, vertex_to_use_instead);
    repeat(texture, MAX_ROOM_TEXTURE)
    {
        num_triangle =  sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        repeat(i, num_triangle)
        {
            // Make sure we don't get two of the same vertex...  Can't have a triangle from vertex 1->2->1...
            skip_triangle = FALSE;
            if(!vertex_was_deleted)
            {
                repeat(j, 3)
                {
                    vertex = sdf_read_unsigned_short(triangle_data);
                    if(vertex == vertex_to_use_instead) { skip_triangle = TRUE; }
                    triangle_data+=4;
                }
                triangle_data-=12;



if(skip_triangle)
{
//log_message("INFO:     Skipping tex-triangle %d-%d", texture, i);
    repeat(j, 3)
    {
        vertex = sdf_read_unsigned_short(triangle_data);
//log_message("INFO:       Vertex %d", vertex);
        triangle_data+=4;
    }
    triangle_data-=12;
}

            }


            if(skip_triangle)
            {
                // Should only happen if !vertex_was_deleted...
                triangle_data+=12;
            }
            else
            {
                repeat(j, 3)
                {
                    vertex = sdf_read_unsigned_short(triangle_data);
                    if(vertex_was_deleted)
                    {
                        // Shuffle back any higher value...
                        if(vertex > vertex_to_replace)
                        {
                            sdf_write_unsigned_short(triangle_data, (unsigned short) (vertex-1));
                        }
                    }
                    else if(vertex == vertex_to_replace)
                    {
                        // Exchange the old value for the new...
                        sdf_write_unsigned_short(triangle_data, vertex_to_use_instead);
                    }
                    triangle_data+=4;
                }
            }
        }
    }
}
void room_srf_triangle_tex_vertex_replace(unsigned char* data, unsigned short tex_vertex_to_replace)
{
    // <ZZ> This function goes through all of the triangles in an SRF file, shufflin' back our
    //      references...
    unsigned char* triangle_data;
    unsigned short num_triangle, texture, i, j, tex_vertex;
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    repeat(texture, MAX_ROOM_TEXTURE)
    {
        num_triangle =  sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        repeat(i, num_triangle)
        {
            repeat(j, 3)
            {
                triangle_data+=2;
                tex_vertex = sdf_read_unsigned_short(triangle_data);
                // Shuffle back any higher value...
                if(tex_vertex > tex_vertex_to_replace)
                {
                    sdf_write_unsigned_short(triangle_data, (unsigned short) (tex_vertex-1));
                }
                triangle_data+=2;
            }
        }
    }
}
void room_srf_minimap_clear(unsigned char* data)
{
    // <ZZ> This function clears out the minimap...
    unsigned char* minimap_data;
    unsigned short num_minimap_triangle;

    minimap_data = data + sdf_read_unsigned_int(data+SRF_MINIMAP_OFFSET);
    num_minimap_triangle = sdf_read_unsigned_short(minimap_data);  minimap_data+=2;
    if(sdf_insert_data(minimap_data, NULL, -6*num_minimap_triangle))
    {
        // Deleted all minimap triangles...
        minimap_data-=2;
        sdf_write_unsigned_short(minimap_data, 0);
    }
}
void room_srf_clear_exterior(unsigned char* data, unsigned short vertex, unsigned char force_clear)
{
    // <ZZ> This function clears out the entire exterior wall list if vertex is present, or if
    //      force_clear is set...  If any vertex in the chain is > vertex, the chain's vertex numbers
    //      are decremented...
    unsigned char* exterior_wall_data;
    unsigned short num_exterior_wall, i, chain_vertex;


    // Check if the given vertex is in the exterior wall list...
    if(force_clear)
    {
        vertex = 65535;
    }
    else
    {
        exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
        num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
        repeat(i, num_exterior_wall)
        {
            chain_vertex = sdf_read_unsigned_short(exterior_wall_data);
            if(chain_vertex == vertex)
            {
                // The vertex is present in the exterior wall chain...
                force_clear = TRUE;
            }
            else
            {
                // Vertex is marked for deletion, so later references gotta get scooched back...
                if(chain_vertex > vertex)
                {
                    chain_vertex--;
                    sdf_write_unsigned_short(exterior_wall_data, chain_vertex);
                }
            }
            exterior_wall_data+=3;
        }
    }


    // Clear out the exterior wall...
    if(force_clear)
    {
        // Clear the minimap too...
        room_srf_minimap_clear(data);


        exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
        num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
        if(sdf_insert_data(exterior_wall_data, NULL, -3*num_exterior_wall))
        {
            // Deleted all of the walls okay...
            exterior_wall_data-=2;
            sdf_write_unsigned_short(exterior_wall_data, 0);
        }
    }
}
unsigned char room_srf_add_next_exterior_wall(unsigned char* data, unsigned short vertex)
{
    // <ZZ> This function adds the next vertex in the exterior wall chain...  Returns TRUE if it worked...
    unsigned char* exterior_wall_data;
    unsigned short num_exterior_wall;
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;

    exterior_wall_data = exterior_wall_data+(num_exterior_wall*3);
    if(sdf_insert_data(exterior_wall_data, NULL, 3))
    {
        // Add the next vertex, and flags for the next segment...
        sdf_write_unsigned_short(exterior_wall_data, vertex);
        *(exterior_wall_data+2) = 0;


        // Modify the number of vertices in the chain...
        exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
        num_exterior_wall++;
        sdf_write_unsigned_short(exterior_wall_data, num_exterior_wall);
        return TRUE;
    }
    return FALSE;
}
void room_srf_exterior_wall_vertex_replace(unsigned char* data, unsigned short vertex_to_replace, unsigned short vertex_to_use_instead)
{
    // <ZZ> This function goes through all of the exterior walls in an SRF file, replacing any reference to the given vertex
    //      with another...
    unsigned char* exterior_wall_data;
    unsigned short num_exterior_wall, i, vertex;
    unsigned char found_to_replace, found_to_use;


    // First check to see if both the vertex_to_replace and the vertex_to_use_instead are part of the
    // exterior wall chain...  If they are, we'd better just clear the chain (or it'll be complicated)
    // (and we don't want that)...
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    found_to_replace = FALSE;
    found_to_use = FALSE;
    repeat(i, num_exterior_wall)
    {
        vertex = sdf_read_unsigned_short(exterior_wall_data);
        if(vertex == vertex_to_replace)
        {
            found_to_replace = TRUE;
        }
        if(vertex == vertex_to_use_instead)
        {
            found_to_use = TRUE;
        }
        exterior_wall_data+=3;
    }
    if(found_to_use && found_to_replace)
    {
        room_srf_clear_exterior(data, 0, TRUE);
        return;
    }



    // Otherwise go through and do replacement...
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    repeat(i, num_exterior_wall)
    {
        vertex = sdf_read_unsigned_short(exterior_wall_data);
        if(vertex == vertex_to_replace)
        {
            sdf_write_unsigned_short(exterior_wall_data, vertex_to_use_instead);
        }
        exterior_wall_data+=3;
    }
}
unsigned char room_srf_exterior_wall_flags(unsigned char* data, unsigned short vertex_a, unsigned short vertex_b, unsigned char overwrite_flags, unsigned char new_flags)
{
    // <ZZ> This function returns the exterior wall flags for a given segment of the exterior wall...
    //      Can also be used to overwrite the existing flags (if overwrite_flags is TRUE)...
    unsigned char* exterior_wall_data;
    unsigned char flags;
    unsigned short num_exterior_wall, i, j, k;


    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;


    j = 0;
    repeat(i, (num_exterior_wall+1))
    {
        k = j;  // Last vertex
        j = sdf_read_unsigned_short(exterior_wall_data + 3*(i%num_exterior_wall));  // Current vertex
        flags = *(exterior_wall_data + 3*(i%num_exterior_wall) + 2);  // Flags for current vertex

        if(i > 0)
        {
            if(j == vertex_a || j == vertex_b)
            {
                if(k == vertex_a || k == vertex_b)
                {
                    // We found our segment...
                    if(overwrite_flags)
                    {
                        *(exterior_wall_data + 3*(i%num_exterior_wall) + 2) = new_flags;
                        flags = new_flags;
                    }
                    return flags;
                }
            }
        }
    }
    return -1;
}
unsigned char room_srf_delete_triangle(unsigned char* data, unsigned short vertex_a, unsigned short vertex_b, unsigned short vertex_c)
{
    // <ZZ> This function deletes a given triangle...  If all of the vertices are the same number it
    //      deletes any triangle connected to that vertex...  Returns TRUE if it deleted anything...
    unsigned char* triangle_data;
    unsigned char* num_write;
    unsigned short num_triangle, texture, i, j, vertex;
    unsigned char delete_connected;
    unsigned char match_count;
    unsigned char delete_okay;


//log_message("INFO:   Deleting triangle %d, %d, %d", vertex_a, vertex_b, vertex_c);
    delete_okay = FALSE;
    delete_connected = (vertex_a == vertex_b) && (vertex_b == vertex_c);
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    repeat(texture, MAX_ROOM_TEXTURE)
    {
        num_write = triangle_data;
        num_triangle =  sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        i = 0;
        while(i < num_triangle)
        {
            // Look for matching vertices in this triangle...
//log_message("INFO:     Checking tex-triangle %d-%d, at offset %d", texture, i, ((int) triangle_data) - ((int) num_write));
//repeat(j, 3)
//{
//    vertex = sdf_read_unsigned_short(triangle_data);
//log_message("INFO:       Vertex %d", vertex);
//    triangle_data+=4;
//}
//triangle_data-=12;


            match_count = 0;
            repeat(j, 3)
            {
                vertex = sdf_read_unsigned_short(triangle_data);
                if(vertex == vertex_a || vertex == vertex_b || vertex == vertex_c)
                {
                    match_count++;
                }
                triangle_data+=4;
            }

            if(match_count == 3 || (match_count > 0 && delete_connected))
            {
                // This triangle should be deleted...
//log_message("INFO:     Trying to delete triangle");
                triangle_data-=12;
                if(sdf_insert_data(triangle_data, NULL, -12))
                {
                    // We deleted the triangle okay...
//log_message("INFO:     Deleted triangle okay");
                    num_triangle--;
                    sdf_write_unsigned_short(num_write, num_triangle);
                    delete_okay = TRUE;
                    i--;
                }
                else
                {
//log_message("ERROR:    Had trouble with delete");
                    // Had trouble with the delete...
                    triangle_data+=12;
                }
            }
            i++;
        }
    }
    return delete_okay;
}
unsigned char room_srf_add_waypoint(unsigned char* data, float x, float y)
{
    // <ZZ> This function adds a new waypoint to the srf file, returning TRUE if it worked...
    //      Also screws up the waypoint optimization table (to be rebuilt on save)...
    unsigned char* waypoint_data;
    unsigned char num_waypoint;
    unsigned short table_entries_to_add;
    signed short value;

    waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    if(num_waypoint < MAX_ROOM_WAYPOINT)
    {
        waypoint_data += num_waypoint*4;
        table_entries_to_add = ((num_waypoint+1)*(num_waypoint+1)) - (num_waypoint*num_waypoint);
        if(sdf_insert_data(waypoint_data, NULL, 4+table_entries_to_add))
        {
            // New waypoint added successfully...  Now set the positions...
            // Don't worry about the table data...
            value = (signed short) (x*256.0f);  sdf_write_unsigned_short(waypoint_data, (unsigned short) value);
            value = (signed short) (y*256.0f);  sdf_write_unsigned_short(waypoint_data+2, (unsigned short) value);
            num_waypoint++;
            waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
            *(waypoint_data) = num_waypoint;
            return TRUE;
        }
    }
    return FALSE;
}
void room_srf_waypoint_replace(unsigned char* data, unsigned char waypoint_to_replace)
{
    // <ZZ> This function goes through all of the waypoint lines in an SRF file, scooching
    //      back all references after a given waypoint...
    unsigned char* waypoint_data;
    unsigned char  num_waypoint, waypoint;
    unsigned short num_waypoint_line, i;


    waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    waypoint_data += num_waypoint*4 + (num_waypoint*num_waypoint);
    num_waypoint_line = sdf_read_unsigned_short(waypoint_data);  waypoint_data+=2;
    num_waypoint_line*=2;  // Two waypoints per line...
    repeat(i, num_waypoint_line)
    {
        waypoint = *waypoint_data;
        if(waypoint > waypoint_to_replace)
        {
            *(waypoint_data) = (waypoint-1);
        }
        waypoint_data++;
    }
}
unsigned char room_srf_link_waypoint(unsigned char* data, unsigned char first, unsigned char second)
{
    // <ZZ> This function links two waypoints in an SRF file, returning TRUE if it worked...
    unsigned char* waypoint_data;
    unsigned char num_waypoint, j, k;
    unsigned short num_waypoint_line, i;

    // Error check...
    if(first == second)
    {
        return FALSE;
    }


    waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    waypoint_data += num_waypoint*4 + (num_waypoint*num_waypoint);
    num_waypoint_line = sdf_read_unsigned_short(waypoint_data);  waypoint_data+=2;
    if(num_waypoint_line < 65535)
    {
        // Check for existing link...
        repeat(i, num_waypoint_line)
        {
            j = *waypoint_data;  waypoint_data++;
            k = *waypoint_data;  waypoint_data++;
            if(j == first || j == second)
            {
                if(k == first || k == second)
                {
                    // These two waypoints are already linked...
                    return FALSE;
                }
            }
        }

        // Should now be ready to add a new link...
        if(sdf_insert_data(waypoint_data, NULL, 2))
        {
            num_waypoint_line++;
            *waypoint_data = first;  waypoint_data++;
            *waypoint_data = second;  waypoint_data++;
            waypoint_data-=(num_waypoint_line*2);
            waypoint_data-=2;
            sdf_write_unsigned_short(waypoint_data, num_waypoint_line);
            return TRUE;
        }
    }
    return FALSE;
}
unsigned char room_srf_unlink_waypoint(unsigned char* data, unsigned char first, unsigned char second)
{
    // <ZZ> This function unlinks two waypoints in an SRF file, or unlinks all connected to a single waypoint
    //      (if first == second)...  Returns TRUE if any were unlinked...
    unsigned char* waypoint_data;
    unsigned char num_waypoint, j, k;
    unsigned short num_waypoint_line, i;
    unsigned char worked;


    worked = FALSE;
    waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    waypoint_data += num_waypoint*4 + (num_waypoint*num_waypoint);
    num_waypoint_line = sdf_read_unsigned_short(waypoint_data);  waypoint_data+=2;

    // Check for existing link...
    i = 0;
    while(i < num_waypoint_line)
    {
        j = *waypoint_data;  waypoint_data++;
        k = *waypoint_data;  waypoint_data++;
        if(j == first || k == first)
        {
            if(j == second || k == second || first == second)
            {
                // Found a link...
                waypoint_data-=2;
                if(sdf_insert_data(waypoint_data, NULL, -2))
                {
                    num_waypoint_line--;
                    waypoint_data-=(i*2);
                    waypoint_data-=2;
                    sdf_write_unsigned_short(waypoint_data, num_waypoint_line);
                    worked = TRUE;
                    waypoint_data+=2;
                    waypoint_data+=(i*2);
                    i--;  // Undo next increment...
                }
                else
                {
                    // Couldn't delete for some reason...
                    waypoint_data+=2;
                }
            }
        }
        i++;
    }
    return worked;
}
unsigned char room_srf_delete_waypoint(unsigned char* data, unsigned char waypoint)
{
    // <ZZ> This function deletes a waypoint from an SRF file, and returns TRUE if it worked...
    unsigned char* waypoint_data;
    unsigned char num_waypoint;
    unsigned short table_entries_to_delete;


    // Delete any waypoint links...
    room_srf_unlink_waypoint(data, waypoint, waypoint);


    // Now try to delete the waypoint...
    waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    if(waypoint < num_waypoint)
    {
        // Delete table data first...
        table_entries_to_delete = (num_waypoint*num_waypoint) - ((num_waypoint-1)*(num_waypoint-1));
        if(sdf_insert_data(waypoint_data+(num_waypoint*4), NULL, -table_entries_to_delete))
        {
            waypoint_data += waypoint*4;
            if(sdf_insert_data(waypoint_data, NULL, -4))
            {
                // Waypoint deleted successfully...
                num_waypoint--;
                waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
                *(waypoint_data) = num_waypoint;

                // Need to scooch back any waypoint link references...
                room_srf_waypoint_replace(data, waypoint);
                return TRUE;
            }
            else
            {
                // Uh, oh...  We deleted the waypoint's table data, but then had trouble with the waypoint itself...
                // Our data is gonna be all messed up...  This shouldn't happen (hopefully)...
                sprintf(DEBUG_STRING, "CRITICAL DELETE WAYPOINT ERROR");
                log_message("ERROR:  CRITICAL DELETE WAYPOINT ERROR");
            }
        }
    }
    return FALSE;
}
void room_srf_build_waypoint_table(unsigned char* srf_file)
{
    // <ZZ> This function builds the waypoint to waypoint lookup table for an SRF file (COMPRESSED...)
    unsigned char* waypoint_data;
    unsigned char* waypoint_line_data;
    unsigned char num_waypoint, i;



    // Decode waypoint information...
    waypoint_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_WAYPOINT_OFFSET);
    num_waypoint = *waypoint_data;
    waypoint_line_data = waypoint_data + 2 + (num_waypoint*4) + (num_waypoint*num_waypoint);


//timer_start();
//    log_message("INFO:   Building the waypoint table...");
    repeat(i, num_waypoint)
    {
        room_srf_find_goto_waypoint(waypoint_data, waypoint_line_data, i);
    }
//timer_end();
}
unsigned char room_srf_add_bridge(unsigned char* data, float ax, float ay, float bx, float by)
{
    // <ZZ> This function adds a new bridge to the srf file, returning TRUE if it worked...
    unsigned char* bridge_data;
    unsigned char num_bridge;
    signed short value;


    bridge_data = data + sdf_read_unsigned_int(data+SRF_BRIDGE_OFFSET);
    num_bridge = *bridge_data;  bridge_data+=2;

    if(num_bridge < 255)
    {
        bridge_data += num_bridge*8;
        if(sdf_insert_data(bridge_data, NULL, 8))
        {
            // New bridge added successfully...  Now set the positions...
            value = (signed short) (ax*256.0f);  sdf_write_unsigned_short(bridge_data, (unsigned short) value);
            value = (signed short) (ay*256.0f);  sdf_write_unsigned_short(bridge_data+2, (unsigned short) value);
            value = (signed short) (bx*256.0f);  sdf_write_unsigned_short(bridge_data+4, (unsigned short) value);
            value = (signed short) (by*256.0f);  sdf_write_unsigned_short(bridge_data+6, (unsigned short) value);
            num_bridge++;
            bridge_data = data + sdf_read_unsigned_int(data+SRF_BRIDGE_OFFSET);
            *bridge_data = num_bridge;
            return TRUE;
        }
    }
    return FALSE;
}
unsigned char room_srf_delete_bridge(unsigned char* data, unsigned char bridge_to_delete)
{
    // <ZZ> This function deletes a bridge from the srf file, returning TRUE if it worked...
    unsigned char* bridge_data;
    unsigned char num_bridge;


    bridge_data = data + sdf_read_unsigned_int(data+SRF_BRIDGE_OFFSET);
    num_bridge = *bridge_data;  bridge_data+=2;
    if(bridge_to_delete < num_bridge)
    {
        bridge_data += bridge_to_delete*8;
        if(sdf_insert_data(bridge_data, NULL, -8))
        {
            // Bridge deleted successfully...
            num_bridge--;
            bridge_data = data + sdf_read_unsigned_int(data+SRF_BRIDGE_OFFSET);
            *bridge_data = num_bridge;
            return TRUE;
        }
    }
    return FALSE;
}
#define room_srf_minimap_vertex_helper(VERTEX, PLACE_XYZ)   \
{                                                       \
    PLACE_XYZ[X] = (((signed short) sdf_read_unsigned_short(vertex_data+(VERTEX*6))) * ONE_OVER_256);  \
    PLACE_XYZ[Y] = (((signed short) sdf_read_unsigned_short(vertex_data+(VERTEX*6)+2)) * ONE_OVER_256);  \
    PLACE_XYZ[Z] = 0.0f;  \
}
unsigned char room_srf_intersection_helper(float* a_xy, float* b_xy, float* c_xy, float* d_xy)
{
    // <ZZ> This function does a 2D line-line intersection test...  Line from a to b, line from c to d...
    //      Returns TRUE if there's a collision...
    float normal_xy[2], temp_xy[2], dota, dotb, dotc, dotd;

//log_message("INFO:   Testing line collisions");
//log_message("INFO:     Line ab == %f, %f to %f, %f", a_xy[X], a_xy[Y], b_xy[X], b_xy[Y]);
//log_message("INFO:     Line cd == %f, %f to %f, %f", c_xy[X], c_xy[Y], d_xy[X], d_xy[Y]);


    // Check points a and b against line cd...
    normal_xy[X] = c_xy[Y] - d_xy[Y];
    normal_xy[Y] = d_xy[X] - c_xy[X];
    temp_xy[X] = a_xy[X] - c_xy[X];
    temp_xy[Y] = a_xy[Y] - c_xy[Y];
    dota = normal_xy[X]*temp_xy[X] + normal_xy[Y]*temp_xy[Y];
    temp_xy[X] = b_xy[X] - c_xy[X];
    temp_xy[Y] = b_xy[Y] - c_xy[Y];
    dotb = normal_xy[X]*temp_xy[X] + normal_xy[Y]*temp_xy[Y];
    if((dota >= 0.0f && dotb >= 0.0f) || (dota <= 0.0f && dotb <= 0.0f))
    {
        // Both points (a & b) are on the same side of line cd...
        return FALSE;
    }


    // Check points c and d against line ab...
    normal_xy[X] = a_xy[Y] - b_xy[Y];
    normal_xy[Y] = b_xy[X] - a_xy[X];
    temp_xy[X] = c_xy[X] - a_xy[X];
    temp_xy[Y] = c_xy[Y] - a_xy[Y];
    dotc = normal_xy[X]*temp_xy[X] + normal_xy[Y]*temp_xy[Y];
    temp_xy[X] = d_xy[X] - a_xy[X];
    temp_xy[Y] = d_xy[Y] - a_xy[Y];
    dotd = normal_xy[X]*temp_xy[X] + normal_xy[Y]*temp_xy[Y];
    if((dotc >= 0.0f && dotd >= 0.0f) || (dotc <= 0.0f && dotd <= 0.0f))
    {
        // Both points (c & d) are on the same side of line ab...
        return FALSE;
    }
    return TRUE;
}
unsigned char room_srf_minimap_add_triangle(unsigned char* data, unsigned short va, unsigned short vb, unsigned short vc)
{
    // <ZZ> This function attempts to add a triangle to the minimap...  It fails if it intersects
    //      with any existing triangle, or if it's backwards...  returns TRUE if it worked...
    unsigned char* exterior_wall_data;
    unsigned char* vertex_data;
    unsigned char* minimap_data;
    unsigned char* read;
    unsigned short num_exterior_wall, num_vertex, num_minimap_triangle;
    unsigned short ra, rb;
    unsigned short i;
    float va_xyz[3], vb_xyz[3], vc_xyz[3], ab_xyz[3], bc_xyz[3], normal_xyz[3];
    float ra_xyz[3], rb_xyz[3];


    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    minimap_data = data + sdf_read_unsigned_int(data+SRF_MINIMAP_OFFSET);
    num_minimap_triangle = sdf_read_unsigned_short(minimap_data);  minimap_data+=2;

    // Error check...
    if(num_minimap_triangle < 65535)
    {
        // Check for inversion...  Vertices must be clockwise to add...
        room_srf_minimap_vertex_helper(va, va_xyz);
        room_srf_minimap_vertex_helper(vb, vb_xyz);
        room_srf_minimap_vertex_helper(vc, vc_xyz);
        ab_xyz[X] = vb_xyz[X]-va_xyz[X];  ab_xyz[Y] = vb_xyz[Y]-va_xyz[Y];  ab_xyz[Z] = vb_xyz[Z]-va_xyz[Z];
        bc_xyz[X] = vc_xyz[X]-vb_xyz[X];  bc_xyz[Y] = vc_xyz[Y]-vb_xyz[Y];  bc_xyz[Z] = vc_xyz[Z]-vb_xyz[Z];
        cross_product(ab_xyz, bc_xyz, normal_xyz);
        if(normal_xyz[Z] < 0.0f)
        {
            // Inverted...
            return FALSE;
        }


        // Check for intersections with exterior wall list...
        read = exterior_wall_data;
        repeat(i, num_exterior_wall)
        {
            ra = sdf_read_unsigned_short(read+(i*3));
            rb = sdf_read_unsigned_short(read+(((i+1)%num_exterior_wall)*3));
            room_srf_minimap_vertex_helper(ra, ra_xyz);
            room_srf_minimap_vertex_helper(rb, rb_xyz);
            if(room_srf_intersection_helper(vc_xyz, va_xyz, ra_xyz, rb_xyz))
            {
                // Have an intersection...
                return FALSE;
            }
        }


        // Safe to add a new triangle...
        if(sdf_insert_data(minimap_data, NULL, 6))
        {
            num_minimap_triangle++;
            sdf_write_unsigned_short(minimap_data, va);
            sdf_write_unsigned_short(minimap_data+2, vb);
            sdf_write_unsigned_short(minimap_data+4, vc);
            minimap_data-=2;
            sdf_write_unsigned_short(minimap_data, num_minimap_triangle);
            return TRUE;
        }
    }
    return FALSE;
}
void room_srf_minimap_build(unsigned char* data)
{
    // <ZZ> This function builds the minimap from the exterior wall list...
    unsigned short build_list[5000];
    unsigned short build_list_size;
    unsigned char* exterior_wall_data;
    unsigned short num_exterior_wall;
    unsigned short i, j, k, va, vb, vc;
    unsigned short trial_count;


    // Clear out the minimap...
    room_srf_minimap_clear(data);


    // Read some info...
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    if(num_exterior_wall >= 5000)
    {
        // Error check...
        return;
    }


    // Fill in the build list with all the vertices of the exterior wall chain...
    repeat(i, num_exterior_wall)
    {
        build_list[i] = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=3;
    }
    build_list_size = num_exterior_wall;


    // Walk through the build list adding triangles...
    i = 0;
    trial_count = 0;
    while(build_list_size > 2 && trial_count < 100)
    {
        // Try to add a new triangle...
        j = (i+1)%build_list_size;
        k = (i+2)%build_list_size;
        va = build_list[i];
        vb = build_list[j];
        vc = build_list[k];
        if(room_srf_minimap_add_triangle(data, va, vb, vc))
        {
            // We added the triangle, so remove the second vertex...
            if(i > j)
            {
                // Need to change i if second vertex was lesser...  Wrapped around build list...
                i--;
            }
            // Now remove j from the build list..
            build_list_size--;
            while(j < build_list_size)
            {
                build_list[j] = build_list[j+1];
                j++;
            }
            trial_count = 0;
        }
        else
        {
            // We were unable to add the triangle, so proceed to the next...
            i = (i+1)%build_list_size;
        }
        trial_count++;
    }
}
unsigned short global_new_tex_vertex;
unsigned char room_srf_add_tex_vertex(unsigned char* data, float x, float y)
{
    // <ZZ> This function adds a new tex vertex to the srf file, returning TRUE if it worked...
    //      Also sets a global variable telling us the number of the new tex vertex...
    unsigned char* tex_vertex_data;
    unsigned short num_tex_vertex;
    signed short value;

    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;

    if(num_tex_vertex < 65535)
    {
        tex_vertex_data += num_tex_vertex*4;
        if(sdf_insert_data(tex_vertex_data, NULL, 4))
        {
            // New vertex added successfully...  Now set the positions...
            value = (signed short) (x*256.0f);  sdf_write_unsigned_short(tex_vertex_data, (unsigned short) value);
            value = (signed short) (y*256.0f);  sdf_write_unsigned_short(tex_vertex_data+2, (unsigned short) value);
            global_new_tex_vertex = num_tex_vertex;
            num_tex_vertex++;
            tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
            sdf_write_unsigned_short(tex_vertex_data, num_tex_vertex);
//log_message("INFO:   Added new tex vertex %d at %f, %f", global_new_tex_vertex, x, y);
            return TRUE;
        }
    }
    return FALSE;
}
unsigned char room_srf_delete_tex_vertex(unsigned char* data, unsigned short tex_vertex)
{
    // <ZZ> This function deletes a tex vertex from an SRF file, and returns TRUE if it worked...
    //      TEX VERTEX MUST NOT BE USED BY ANY TRIANGLE!!!  Should only ever use this
    //      function as a helper to the clean-up function...
    unsigned char* tex_vertex_data;
    unsigned short num_tex_vertex;

    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    if(tex_vertex < num_tex_vertex)
    {
        tex_vertex_data += tex_vertex*4;
        if(sdf_insert_data(tex_vertex_data, NULL, -4))
        {
            // Tex vertex deleted successfully...
//log_message("INFO:   Deleted tex vertex %d", tex_vertex);
            num_tex_vertex--;
            tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
            sdf_write_unsigned_short(tex_vertex_data, num_tex_vertex);

            // Need to scooch back any triangle references...
            room_srf_triangle_tex_vertex_replace(data, tex_vertex);
            return TRUE;
        }
    }
    return FALSE;
}
unsigned char room_srf_add_texture_triangle(unsigned char* data, unsigned char texture, unsigned short va, unsigned short vb, unsigned short vc, float* tvaxy, float* tvbxy, float* tvcxy)
{
    // <ZZ> This function adds a new triangle to a room, under a given texture (MAX_ROOM_TEXTURE), and connected to three
    //      vertices (va, vb, vc)...  It also adds tex vertices and sets their positions according to tvaxy, tvbxy, tvcxy...
    //      0.0, 0.0 if tv?xy is NULL...  Always tries to add 3 tex vertices...  Vertices should get autowelded later...
    //      Returns TRUE if it worked...
    float x, y;
    unsigned short tva, tvb, tvc;
    unsigned short i;
    unsigned char* triangle_data;
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned short num_triangle;
    unsigned short num_vertex;
    unsigned short num_tex_vertex;

//log_message("INFO:   Attempting to add triangle to texture %d, vertices %d, %d, %d", texture, va, vb, vc);

    // Error check...
    if(texture >= MAX_ROOM_TEXTURE)
    {
//log_message("ERROR:    Bad texture number for triangle add");
        return FALSE;
    }


    // Find our data...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    repeat(i, texture)
    {
        // Skip this texture-triangle set...
        num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        triangle_data+=(num_triangle*12);
    }
    num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;


    // Make sure all of our vertices are valid...
    if(va >= num_vertex || vb >= num_vertex || vc >= num_vertex)
    {
//log_message("ERROR:    Bad vertex number (num_vertex == %d)", num_vertex);
        return FALSE;
    }





    // Add tex vertices...
    x = 0.0f;
    y = 0.0f;
    if(tvaxy)
    {
        x = tvaxy[X];
        y = tvaxy[Y];
    }
    if(room_srf_add_tex_vertex(data, x, y))
    {
        tva = global_new_tex_vertex;
        triangle_data+=4;
    }
    else
    {
        return FALSE;
    }
    // Add tex vertices...
    x = 0.0f;
    y = 0.0f;
    if(tvbxy)
    {
        x = tvbxy[X];
        y = tvbxy[Y];
    }
    if(room_srf_add_tex_vertex(data, x, y))
    {
        tvb = global_new_tex_vertex;
        triangle_data+=4;
    }
    else
    {
        return FALSE;
    }
    // Add tex vertices...
    x = 0.0f;
    y = 0.0f;
    if(tvcxy)
    {
        x = tvcxy[X];
        y = tvcxy[Y];
    }
    if(room_srf_add_tex_vertex(data, x, y))
    {
        tvc = global_new_tex_vertex;
        triangle_data+=4;
    }
    else
    {
        return FALSE;
    }



    // Add the triangle to the texture-triangle data...
    if(num_triangle < 65535)
    {
        if(sdf_insert_data(triangle_data, NULL, 12))
        {
//log_message("INFO:     Added triangle data...  %d, %d, %d...  %d, %d, %d", va, vb, vc, tva, tvb, tvc);
            // New triangle added successfully...  Now set the positions...
            sdf_write_unsigned_short(triangle_data, va);
            sdf_write_unsigned_short(triangle_data+2, tva);
            sdf_write_unsigned_short(triangle_data+4, vb);
            sdf_write_unsigned_short(triangle_data+6, tvb);
            sdf_write_unsigned_short(triangle_data+8, vc);
            sdf_write_unsigned_short(triangle_data+10, tvc);
            num_triangle++;
            triangle_data-=2;
            sdf_write_unsigned_short(triangle_data, num_triangle);
            return TRUE;
        }
    }
    return FALSE;
}
void room_srf_autoweld_tex_vertices(unsigned char* data)
{
    // <ZZ> This function replaces any non-floor triangle's tex vertex if it can find a lower number'd one that's in
    //      pretty much the same location...
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned char* texture_data;
    unsigned char* triangle_data_start;
    unsigned char* other_triangle_data;
    unsigned short num_tex_vertex;
    unsigned short num_triangle;
    unsigned int num_triangle_vertex;
    unsigned short vertex, tex_vertex, other_vertex, other_tex_vertex;
    unsigned short i;
    unsigned int j, k;
    signed short tex_vertex_xy[2];
    signed short other_tex_vertex_xy[2];
    signed short disx, disy;

//log_message("INFO:   Autowelding tex vertices");

    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    texture_data = data + sdf_read_unsigned_int(data+SRF_TEXTURE_OFFSET);
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);


    // Go through each texture of the room...
    repeat(i, MAX_ROOM_TEXTURE)
    {
        num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        triangle_data_start = triangle_data;
        if(texture_data[i]&ROOM_TEXTURE_FLAG_FLOORTEX)
        {
            // Skip this texture, since floor autotexture function works pretty well...
            triangle_data+=(12*num_triangle);
        }
        else
        {
            // Read through all of the triangle vertices in this texture...
            num_triangle_vertex=num_triangle*3;
            repeat(j, num_triangle_vertex)
            {
                vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                tex_vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                tex_vertex_xy[X] = (signed short) sdf_read_unsigned_short((tex_vertex_data + (tex_vertex<<2)));
                tex_vertex_xy[Y] = (signed short) sdf_read_unsigned_short((tex_vertex_data + (tex_vertex<<2) + 2));

                // Look for any triangles that share this vertex...
                other_triangle_data = triangle_data_start;
                repeat(k, j)
                {
                    other_vertex = sdf_read_unsigned_short(other_triangle_data);  other_triangle_data+=2;
                    if(vertex == other_vertex)
                    {
                        // Found one...  Are tex vertices near one another?
                        other_tex_vertex = sdf_read_unsigned_short(other_triangle_data);
                        other_tex_vertex_xy[X] = (signed short) sdf_read_unsigned_short((tex_vertex_data + (other_tex_vertex<<2)));
                        other_tex_vertex_xy[Y] = (signed short) sdf_read_unsigned_short((tex_vertex_data + (other_tex_vertex<<2) + 2));
                        disx = other_tex_vertex_xy[X] - tex_vertex_xy[X];
                        disy = other_tex_vertex_xy[Y] - tex_vertex_xy[Y];
                        ABS(disx);
                        ABS(disy);
                        disx+=disy;
                        if(disx < 20)
                        {
                            // We found one that's close enough to use...
//if(tex_vertex != other_tex_vertex)
//{
//    log_message("INFO:     Found better tex vertex (%d -> %d)", other_tex_vertex, tex_vertex);
//}
                            sdf_write_unsigned_short(other_triangle_data, other_tex_vertex);
                        }
                    }
                    other_triangle_data+=2;
                }
            }
        }

    }
}
void room_srf_clean_up_tex_vertices(unsigned char* data)
{
    // <ZZ> This function deletes any unused tex vertex in an srf file...
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned short num_tex_vertex;
    unsigned short num_triangle;
    unsigned short i, j, k;
    unsigned short tex_vertex;

//log_message("INFO:   Cleaning up tex vertices");

    // Select all tex vertices...
    room_select_clear();
    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    repeat(i, num_tex_vertex)
    {
//log_message("INFO:     Adding tex vertex %d", i);
        room_select_add(i, tex_vertex_data, 2);
        tex_vertex_data+=4;
    }


    // Unselect any tex vertex that's used by any triangle...
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    repeat(i, MAX_ROOM_TEXTURE)
    {
        // Read through this texture-triangle set...
        num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
        repeat(j, num_triangle)
        {
            // For each tex vertex of the triangle...
            repeat(k, 3)
            {
                triangle_data+=2;
                tex_vertex = sdf_read_unsigned_short(triangle_data);
                triangle_data+=2;
                // Unselect this tex vertex...
                if(room_select_inlist(tex_vertex))
                {
//log_message("INFO:     Removing tex vertex %d", tex_vertex);
                    room_select_remove(room_select_index);
                }
            }
        }
    }



    // Delete all tex vertices that are still selected (in reverse order)...
    i = room_select_num;
    while(i > 0)
    {
        i--;
//log_message("INFO:     Deleting tex vertex %d", room_select_list[i]);
        room_srf_delete_tex_vertex(data, room_select_list[i]);
    }
    room_select_clear();
}
unsigned char room_srf_textureflags(unsigned char* data, unsigned char texture, unsigned char set_flags, unsigned char new_flags)
{
    // <ZZ> This function returns the texture flags value for a given texture of an srf file...
    //      Can also be used to set the texture flag...
    unsigned char* texture_data;

    texture = texture&(MAX_ROOM_TEXTURE-1);
    texture_data = data + sdf_read_unsigned_int(data+SRF_TEXTURE_OFFSET);
    if(set_flags)
    {
        texture_data[texture] = new_flags;
    }
    return texture_data[texture];
}
void room_srf_autotexture(unsigned char* data)
{
    // <ZZ> This function assigns all tex vertex locations in an srf file...  Well for
    //      texture layers that're flagged to autotexture anyway...  For texturing floor...
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned char* texture_data;
    unsigned short num_vertex;
    unsigned short num_tex_vertex;
    unsigned short num_triangle;
    unsigned short i, j, k;
    unsigned short value;
    unsigned short vertex, tex_vertex;

//log_message("INFO:   Autotexturing...");


    // First do our clean up...
    room_srf_autoweld_tex_vertices(data);
    room_srf_clean_up_tex_vertices(data);


    // Find the data blocks...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    texture_data = data + sdf_read_unsigned_int(data+SRF_TEXTURE_OFFSET);


    // Go through each triangle...
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    repeat(i, MAX_ROOM_TEXTURE)
    {
        // Is this texture-triangle set flag'd as being floor?  Only autotexture floor...
        if(texture_data[i]&ROOM_TEXTURE_FLAG_FLOORTEX)
        {
            // Read through this texture-triangle set...
            num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                // For each vertex/tex vertex of the triangle...
                repeat(k, 3)
                {
                    vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                    tex_vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;


                    // Floor mode...
                    value = ((signed short) sdf_read_unsigned_short(vertex_data+(vertex*6))) / 20;
                    sdf_write_unsigned_short(tex_vertex_data+(tex_vertex*4), (unsigned short) value);
                    value = ((signed short) sdf_read_unsigned_short(vertex_data+(vertex*6)+2)) / 20;
                    sdf_write_unsigned_short(tex_vertex_data+(tex_vertex*4)+2, (unsigned short) value);
                }
            }
        }
        else
        {
            // Skip over non-floor textures...
            num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
            triangle_data+=(12*num_triangle);
        }
    }
}
float autotrim_length = 0.0f;
float autotrim_offset = 0.0f;
float autotrim_scaling= WALL_TEXTURE_SCALE;
void room_srf_autotrim(unsigned char* data)
{
    // <ZZ> This function assigns tex vertex locations in an srf file for the selected vertices...
    //      Selected vertices must be divided into three types (wall top, floor edge, wall other)...
    //      For texturing walls and transition area of floor...  Autotrim length should also be set
    //      prior to calling...
    float segment_length;
    float segment_scaling;
    float segment_start_xyz[3];
    float segment_end_xyz[3];
    float segment_normal_xy[2];
    float x, y, z, distance;
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned char* temp_data;
    unsigned char* texture_data;
    unsigned short num_tex_vertex;
    unsigned short num_triangle;
    unsigned short i, j, k;
    unsigned short vertex, tex_vertex;
    unsigned char  triangle_selected, handle_as_floor_edge;
    float*         triangle_vertex_xyz[3];
    unsigned char  triangle_vertex_group[3];
    unsigned short value;


//log_message("INFO:   Autotrimming");
//log_message("INFO:     Autotrim select[0] == %d", room_autotrim_select_count[0]);
//log_message("INFO:     Autotrim select[1] == %d", room_autotrim_select_count[1]);
//log_message("INFO:     Autotrim select[2] == %d", room_autotrim_select_count[2]);

    // Make sure we don't have too few vertices...
    if(room_autotrim_select_count[0] < 2 || room_select_num < 3)
    {
        return;
    }


    // Find the data blocks...
    tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
    num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
    triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
    texture_data = data + sdf_read_unsigned_int(data+SRF_TEXTURE_OFFSET);


    // Find the length of the segment we're currently working on...
    segment_start_xyz[X] = room_select_xyz[0][X];
    segment_start_xyz[Y] = room_select_xyz[0][Y];
    segment_start_xyz[Z] = room_select_xyz[0][Z];
    segment_end_xyz[X] = room_select_xyz[1][X];
    segment_end_xyz[Y] = room_select_xyz[1][Y];
    segment_end_xyz[Z] = room_select_xyz[1][Z];
    x = segment_end_xyz[X] - segment_start_xyz[X];
    y = segment_end_xyz[Y] - segment_start_xyz[Y];
    segment_length = (float) sqrt(x*x + y*y);
    segment_scaling = segment_length * autotrim_scaling;
    segment_normal_xy[X] = 0.0f;
    segment_normal_xy[Y] = 0.0f;
    if(segment_length > 0.0f)
    {
        segment_normal_xy[X] = x/segment_length;
        segment_normal_xy[Y] = y/segment_length;
    }



    // Go through each triangle...
    repeat(i, MAX_ROOM_TEXTURE)
    {
        // Read through this texture-triangle set...
        if(texture_data[i]&ROOM_TEXTURE_FLAG_FLOORTEX)
        {
            // Don't autotrim floor textures...
            num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
            triangle_data+=(12*num_triangle);
        }
        else
        {
            num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                // Determine if all 3 vertices of triangle are selected...
                temp_data = triangle_data;
                triangle_selected = TRUE;
                handle_as_floor_edge = FALSE;
                repeat(k, 3)
                {
                    vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=4;
                    if(room_select_inlist(vertex))
                    {
                        triangle_vertex_xyz[k] = room_select_xyz[room_select_index];
                        // Remember which group each vertex is in...  0 wall top, 1 floor edge, 2 wall other...
                        triangle_vertex_group[k] = 0;
                        if(room_select_index >= room_autotrim_select_count[0]) { triangle_vertex_group[k]++; }
                        if(room_select_index >= room_autotrim_select_count[1]) { triangle_vertex_group[k]++; }
                        if(triangle_vertex_group[k] == 1)
                        {
                            handle_as_floor_edge = TRUE;
                        }
                    }
                    else
                    {
                        triangle_selected = FALSE;
                    }
                }
                if(triangle_selected)
                {
                    // All 3 vertices were selected, so now figger out XY coordinates for each vertex's
                    // corresponding tex vertex...
                    triangle_data = temp_data;
                    repeat(k, 3)
                    {
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// old way of doing without adding tex vertices...
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
//                        triangle_data+=2;
//                        tex_vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!

                        // Add a new tex vertex for each point of the triangle...
                        triangle_data+=2;
                        if(room_srf_add_tex_vertex(data, 0.0f, 0.0f))
                        {
                            // Remember the new tex vertex...
                            tex_vertex = global_new_tex_vertex;
                            // Change pointer to reflect data insertion (tex_vertex_data should be < triangle_data)...
                            triangle_data+=4;
                            sdf_write_unsigned_short(triangle_data, tex_vertex);
                        }
                        else
                        {
                            // Couldn't add for some reason...
                            tex_vertex = sdf_read_unsigned_short(triangle_data);
                        }
                        triangle_data+=2;


                        // Now figger out the X position based on distance over segment (plus offset)...
                        x = triangle_vertex_xyz[k][X] - segment_start_xyz[X];
                        y = triangle_vertex_xyz[k][Y] - segment_start_xyz[Y];
                        z = (x*segment_normal_xy[X] + y*segment_normal_xy[Y]);
                        distance = (z*autotrim_scaling) + autotrim_offset;
                        z = z*((segment_end_xyz[Z]-segment_start_xyz[Z])/segment_length);
                        z+=segment_start_xyz[Z];
                        value = (signed short) (distance * 256.0f);
//    log_message("INFO:       Writing value %d for Tex X", value);
                        sdf_write_unsigned_short(tex_vertex_data+(tex_vertex*4), (unsigned short) value);


                        // And figger out the Y position...
                        if(handle_as_floor_edge)
                        {
                            //  ...based on vertex group...
                            value = 246;
                            if(triangle_vertex_group[k] == 1)
                            {
                                value = 10;
                            }
                        }
                        else
                        {
                            //  ...based on distance from top of wall...
                            value = (signed short) ((z-triangle_vertex_xyz[k][Z]) * WALL_TEXTURE_SCALE * 256.0f);
                        }
//    log_message("INFO:       Writing value %d for Tex Y", value);
                        sdf_write_unsigned_short(tex_vertex_data+(tex_vertex*4)+2, (unsigned short) value);
                    }
                }
            }
        }
    }
    autotrim_offset += segment_scaling;
}
unsigned char room_srf_add_vertex(unsigned char* data, float x, float y, float z, unsigned char select_add)
{
    // <ZZ> This function adds a new vertex to the srf file, returning TRUE if it worked...
    unsigned char* vertex_data;
    unsigned short num_vertex;
    signed short value;

    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;

    if(num_vertex < 65535)
    {
        vertex_data += num_vertex*6;
        if(sdf_insert_data(vertex_data, NULL, 6))
        {
            // New vertex added successfully...  Now set the positions...
            value = (signed short) (x*256.0f);  sdf_write_unsigned_short(vertex_data, (unsigned short) value);
            value = (signed short) (y*256.0f);  sdf_write_unsigned_short(vertex_data+2, (unsigned short) value);
            value = (signed short) (z*256.0f);  sdf_write_unsigned_short(vertex_data+4, (unsigned short) value);
            if(select_add)
            {
                room_select_add(num_vertex, vertex_data, 3);
            }
            num_vertex++;
            vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
            sdf_write_unsigned_short(vertex_data, num_vertex);
            return TRUE;
        }
    }
    return FALSE;
}
unsigned char room_srf_delete_vertex(unsigned char* data, unsigned short vertex)
{
    // <ZZ> This function deletes a vertex from an SRF file, and returns TRUE if it worked...
    unsigned char* vertex_data;
    unsigned short num_vertex;


    // Delete any connected triangle...
    room_srf_delete_triangle(data, vertex, vertex, vertex);


    // Clear the wall chain if the given vertex is in it...
    room_srf_clear_exterior(data, vertex, FALSE);


    // Now try to delete the vertex...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    if(vertex < num_vertex)
    {
        vertex_data += vertex*6;
        if(sdf_insert_data(vertex_data, NULL, -6))
        {
            // Vertex deleted successfully...
            num_vertex--;
            vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
            sdf_write_unsigned_short(vertex_data, num_vertex);

            // Need to scooch back any triangle references...
            room_srf_triangle_vertex_replace(data, vertex, 0, TRUE);
            return TRUE;
        }
    }
    return FALSE;
}
void room_srf_weld_selected_vertices(unsigned char* data)
{
    // <ZZ> This function finds the best pairs of vertices within the selected group, and turns
    //      each pair into a single vertex.  Renumbers triangle associations to match.
    unsigned short i, j;
    float distance_xyz[3];
    float distance;
    float best_distance;
    unsigned short best_partner;
    unsigned char axes;


    // Need an even number of 'em
    if(room_select_num & 1)
    {
        message_add("ERROR:  Need an even number to weld", NULL, FALSE);
        return;
    }


    // Flag each of our selected vertices as having no partner...
    repeat(i, room_select_num)
    {
        room_select_flag[i] = MAX_VERTEX;
    }


    // Find a partner for each vertex
    repeat(i, room_select_num)
    {
        // Does it have a partner yet?
        if(room_select_flag[i] == MAX_VERTEX)
        {
            // Nope, so find one...
            axes = room_select_axes[i];
            best_partner = MAX_VERTEX;
            best_distance = 9999.9f;
            j = i+1;
            while(j < room_select_num)
            {
                // Can we pair it with this one?
                if(room_select_flag[j] == MAX_VERTEX)
                {
                    // Yup, so check how close they are...
                    distance_xyz[X] = 0.0f;  if(axes > 0) { distance_xyz[X] = room_select_xyz[j][X]-room_select_xyz[i][X]; }
                    distance_xyz[Y] = 0.0f;  if(axes > 1) { distance_xyz[Y] = room_select_xyz[j][Y]-room_select_xyz[i][Y]; }
                    distance_xyz[Z] = 0.0f;  if(axes > 2) { distance_xyz[Z] = room_select_xyz[j][Z]-room_select_xyz[i][Z]; }
                    distance = vector_length(distance_xyz);
                    if(distance < best_distance)
                    {
                        best_partner = j;
                        best_distance = distance;
                    }
                }
                j++;
            }
            // Shoulda found a partner, so exit if it didn't
            if(best_partner == MAX_VERTEX)
            {
                log_message("ERROR:  Partnering error...  Shouldn't happen...");
                return;
            }
            // Remember our partners...
            room_select_flag[i] = best_partner;
            room_select_flag[best_partner] = i;
            // Find the center of the points, and move both to that location
            room_select_xyz[i][X] = (room_select_xyz[i][X] + room_select_xyz[best_partner][X]) * 0.5f;
            room_select_xyz[i][Y] = (room_select_xyz[i][Y] + room_select_xyz[best_partner][Y]) * 0.5f;
            room_select_xyz[i][Z] = (room_select_xyz[i][Z] + room_select_xyz[best_partner][Z]) * 0.5f;
            room_select_xyz[best_partner][X] = room_select_xyz[i][X];
            room_select_xyz[best_partner][Y] = room_select_xyz[i][Y];
            room_select_xyz[best_partner][Z] = room_select_xyz[i][Z];
            if(axes > 0)
            {
                sdf_write_unsigned_short(room_select_data[i], (unsigned short) ((signed short) (room_select_xyz[i][X]*256.0f)) );
                sdf_write_unsigned_short(room_select_data[best_partner], (unsigned short) ((signed short) (room_select_xyz[best_partner][X]*256.0f)) );
            }
            if(axes > 1)
            {
                sdf_write_unsigned_short(room_select_data[i]+2, (unsigned short) ((signed short) (room_select_xyz[i][Y]*256.0f)) );
                sdf_write_unsigned_short(room_select_data[best_partner]+2, (unsigned short) ((signed short) (room_select_xyz[best_partner][Y]*256.0f)) );
            }
            if(axes > 2)
            {
                sdf_write_unsigned_short(room_select_data[i]+4, (unsigned short) ((signed short) (room_select_xyz[i][Z]*256.0f)) );
                sdf_write_unsigned_short(room_select_data[best_partner]+4, (unsigned short) ((signed short) (room_select_xyz[best_partner][Z]*256.0f)) );
            }
        }
    }


    // Delete the higher vertex of each pair...  Do in weird order so it doesn't mess up...
    // Do with hacked in replacement thing...
    i = 50000;
    while(i > 0)
    {
        i--;
        if(room_select_inlist(i))
        {
            if(room_select_flag[room_select_index] != MAX_VERTEX)
            {
                // Delete the vertex, and replace any reference with one to its partner...
                // Flag its partner to not be deleted...
                best_partner = room_select_flag[room_select_index];
                room_select_flag[best_partner] = MAX_VERTEX;
                best_partner = room_select_list[best_partner];
    			room_srf_triangle_vertex_replace(data, i, best_partner, FALSE);
                room_srf_exterior_wall_vertex_replace(data, i, best_partner);
                room_srf_delete_vertex(data, i);
            }
        }
    }
    room_select_clear();
}
unsigned short global_room_active_object = 0;
unsigned char* global_room_active_object_data = NULL;
void room_group_set(unsigned char* srf_file, unsigned short new_group_number)
{
    // <ZZ> This function tries to set the global group number (for editing object-groups)
    unsigned char* object_group_data;
    unsigned short num_group;


    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(new_group_number < num_group)
    {
        global_room_active_group = new_group_number;
        global_room_active_object = 0;
        global_room_active_object_data = NULL;
    }
}
void room_group_add(unsigned char* srf_file, unsigned char delete_instead)
{
    // <ZZ> This function adds a new object group at the current active group...
    unsigned char* object_group_data;
    unsigned char* num_write;
    unsigned short num_group, num_object, i;


    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_write = object_group_data;
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(global_room_active_group <= num_group)
    {
        // Skip through data to get to the current group...
        repeat(i, global_room_active_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }


        // Add the new group...
        if(delete_instead)
        {
            // Err...  I mean delete the current group...
            if(num_group > 1)
            {
                num_object = sdf_read_unsigned_short(object_group_data);
                if(sdf_insert_data(object_group_data, NULL, -(2+(num_object*22))))
                {
                    num_group--;
                    sdf_write_unsigned_short(num_write, num_group);
                    if(global_room_active_group > 0) { global_room_active_group--; }
                    global_room_active_object = 0;
                    global_room_active_object_data = NULL;
                }
            }
            else if(num_group == 1)
            {
                // We only have this one group left...  Delete all objects, but not group itself...
                num_write = object_group_data;
                num_object = sdf_read_unsigned_short(object_group_data); object_group_data+=2;
                if(sdf_insert_data(object_group_data, NULL, -(num_object*22)))
                {
                    sdf_write_unsigned_short(num_write, 0);
                    global_room_active_group = 0;
                    global_room_active_object = 0;
                    global_room_active_object_data = NULL;
                }
            }
        }
        else
        {
            if(num_group < 65535)
            {
                if(sdf_insert_data(object_group_data, NULL, 2))
                {
                    num_group++;
                    sdf_write_unsigned_short(num_write, num_group);
                    global_room_active_object = 0;
                    global_room_active_object_data = NULL;
                }
            }
        }
    }
}
void room_object_set(unsigned char* srf_file, unsigned short new_object_number)
{
    // <ZZ> This function tries to set the global object number (for editing object-groups)...
    unsigned char* object_group_data;
    unsigned short num_group;
    unsigned short num_object;
    unsigned short i;


    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;

    // Is our current group valid?
    if(global_room_active_group < num_group)
    {
        // Skip through data to get to the current group...
        repeat(i, global_room_active_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }

        // Now find the selected object...
        num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
        if(new_object_number < num_object)
        {
            global_room_active_object = new_object_number;
            global_room_active_object_data = object_group_data + (22*new_object_number);
        }
    }
    else
    {
        // Reset everything...
        global_room_active_group = 0;
        global_room_active_object = 0;
        global_room_active_object_data = NULL;
    }
}
void room_object_add(unsigned char* srf_file, unsigned char delete_instead)
{
    // <ZZ> This function adds a new object at the current active group-object...
    unsigned char* object_group_data;
    unsigned char* num_write;
    unsigned short num_group, num_object, i;


    object_group_data = srf_file + sdf_read_unsigned_int(srf_file+SRF_OBJECT_GROUP_OFFSET);
    num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
    if(global_room_active_group < num_group)
    {
        // Skip through data to get to the current group...
        repeat(i, global_room_active_group)
        {
            num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
            object_group_data += 22*num_object;
        }


        // Skip to the current object...
        num_write = object_group_data;
        num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
        if(global_room_active_object <= num_object)
        {
            object_group_data += 22*global_room_active_object;

            // Add the new object...
            if(delete_instead)
            {
                // Err...  I mean delete the current object...
                if(num_object > 0)
                {
                    if(sdf_insert_data(object_group_data, NULL, -22))
                    {
                        num_object--;
                        sdf_write_unsigned_short(num_write, num_object);
                        if(global_room_active_object > 0) { global_room_active_object--;  global_room_active_object_data-=22; }
                        else { global_room_active_object_data = NULL; }
                    }
                }
            }
            else
            {
                if(num_object < 65535)
                {
                    if(sdf_insert_data(object_group_data, NULL, 22))
                    {
                        num_object++;
                        sdf_write_unsigned_short(num_write, num_object);
                        object_group_data[0]  = 'C';
                        object_group_data[1]  = 'R';
                        object_group_data[2]  = 'A';
                        object_group_data[3]  = 'N';
                        object_group_data[4]  = 'D';
                        object_group_data[5]  = 'O';
                        object_group_data[6]  = 'M';
                        object_group_data[7]  = '.';
                        object_group_data[8]  = 'R';
                        object_group_data[9]  = 'U';
                        object_group_data[10] = 'N';
                        object_group_data[11] = 0;
                        object_group_data[13] = 240;
                        object_group_data[14] = TEAM_MONSTER;
                        object_group_data[15] = random_number;
                    }
                }
            }
        }
    }
}
#define HARDPLOPPER_FENCE        0
#define HARDPLOPPER_STAIRS       1
#define HARDPLOPPER_LEDGE        2
#define HARDPLOPPER_LEDGE_LOOP   3
#define HARDPLOPPER_DECAL        4
#define HARDPLOPPER_PAVEMENT     5
#define HARDPLOPPER_BORDER       6
#define HARDPLOPPER_MINE         7
#define RANDOMIZE_TEMP_XYZ() { temp_xyz[X] = 0.0f;  temp_xyz[Y] = 0.0f;  temp_xyz[Z] = 0.0f;  if(randomize_ledge) { temp = random_number;  temp*=ONE_OVER_256*0.5f;  temp_xyz[X]=temp*final_vector_xyz[X];  temp_xyz[Y]=temp*final_vector_xyz[Y];  temp = random_number;  temp*=ONE_OVER_256;  temp_xyz[Z]=(temp-0.5f);} }
void room_srf_hardplopper(unsigned char* srf_file, unsigned char width, unsigned char normalize, unsigned char type, unsigned char base_vertex_ledge, unsigned char randomize_ledge, unsigned char ledge_height)
{
    // <ZZ> This function plops a hard coded model type o' thing into an SRF file...  Used for making
    //      stairs and fences in the room editor.  A list of control vertices should be selected to
    //      show what path the stair/fence should take.  Control vertices get scooted around to provide
    //      uniform spacing if normalize is set.  Width only works for stairs (64 is default).  Type
    //      0 means fence, 1 means stairs...
    unsigned short i, j, k, skip, skipstt, skiplow, num_segment, num_original;
    unsigned char worked, texture;
    float vector_xyz[3];
    float last_vector_xyz[3];
    float final_vector_xyz[3];
    float temp_xyz[3];
    float tex_vertex_xy[5][2];
    float left_x, right_x, top_y, bottom_y;
    float left_tex_x, right_tex_x;
    float length, scaling, temp;

    if(room_select_num > 1 || type == HARDPLOPPER_DECAL)
    {
        num_original = room_select_num;
        num_segment = room_select_num-1;


        // Space control vertices well...
        if(normalize)
        {
            repeat(i, num_segment)
            {
                vector_xyz[X] = room_select_xyz[i+1][X] - room_select_xyz[i][X];
                vector_xyz[Y] = room_select_xyz[i+1][Y] - room_select_xyz[i][Y];
                vector_xyz[Z] = 0.0f;
                length = vector_length(vector_xyz)*0.5f;
                if(length > 0.0f)
                {
                    vector_xyz[X] = (vector_xyz[X]/length) - vector_xyz[X];
                    vector_xyz[Y] = (vector_xyz[Y]/length) - vector_xyz[Y];
                    j = i;
                    while(j < num_segment)
                    {
                        room_select_xyz[j+1][X] += vector_xyz[X];
                        room_select_xyz[j+1][Y] += vector_xyz[Y];
                        j++;
                    }
                }
            }
        }


        // Adjust the Z values for stairs...
        if(type==HARDPLOPPER_STAIRS)
        {
            repeat(i, num_segment)
            {
                room_select_xyz[i+1][Z] = room_select_xyz[0][Z] + ((i+1)*1.0f);
            }
        }
        repeat(i, num_original)
        {
            room_select_xyz[i][Z] += 0.30f;  // 0.30f is to offset above floor for cartoon lines...
        }



        // Add new vertices to make up the desired geometry...  Original control vertices should not be modified (should get deleted later)...
        worked = TRUE;
        scaling = width*0.01953125f;
        repeat(i, num_segment)
        {
            vector_xyz[X] = room_select_xyz[i+1][X] - room_select_xyz[i][X];
            vector_xyz[Y] = room_select_xyz[i+1][Y] - room_select_xyz[i][Y];
            vector_xyz[Z] = 0.0f;
            length = vector_length(vector_xyz);
            if(length > 0.0f)
            {
                vector_xyz[X]/=length;
                vector_xyz[Y]/=length;
                if(i == 0)
                {
                    last_vector_xyz[X] = vector_xyz[X];
                    last_vector_xyz[Y] = vector_xyz[Y];
                    last_vector_xyz[Z] = vector_xyz[Z];
                    if(type == HARDPLOPPER_LEDGE_LOOP)
                    {
                        last_vector_xyz[X] = room_select_xyz[i][X] - room_select_xyz[num_segment][X];
                        last_vector_xyz[Y] = room_select_xyz[i][Y] - room_select_xyz[num_segment][Y];
                        last_vector_xyz[Z] = 0.0f;
                        length = vector_length(last_vector_xyz);
                        if(length > 0.0f)
                        {
                            last_vector_xyz[X]/=length;
                            last_vector_xyz[Y]/=length;
                        }
                    }
                }
                final_vector_xyz[X] = (last_vector_xyz[Y] + vector_xyz[Y])*-scaling;
                final_vector_xyz[Y] = (last_vector_xyz[X] + vector_xyz[X])*scaling;
                final_vector_xyz[Z] = 0.0f;
                last_vector_xyz[X] = vector_xyz[X];
                last_vector_xyz[Y] = vector_xyz[Y];
                last_vector_xyz[Z] = vector_xyz[Z];




                // Add extra vertices for fence...
                if(type==HARDPLOPPER_FENCE)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Setup vectors...
                        temp_xyz[X] = final_vector_xyz[X] - final_vector_xyz[Y];
                        temp_xyz[Y] = final_vector_xyz[Y] + final_vector_xyz[X];
                        temp_xyz[Z] = 0.0f;
                        length = vector_length(temp_xyz);
                        if(length > 0.0f)
                        {
                            temp_xyz[X]/=length;
                            temp_xyz[Y]/=length;
                        }
                        temp_xyz[X]*=scaling;
                        temp_xyz[Y]*=scaling;



                        if(i > 0)
                        {
                            // Three vertices on left side of post for bottom railing
                            temp_xyz[Z] = room_select_xyz[i][Z]+1.25f;
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[X]-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+temp_xyz[Y]-(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[Y]-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+temp_xyz[X]-(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+((temp_xyz[X]-temp_xyz[Y])*0.5f)-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+((temp_xyz[X]+temp_xyz[Y])*0.5f)-(vector_xyz[Y]*0.22f), temp_xyz[Z]-0.75f, TRUE);

                            // Three vertices on left side of post for top railing
                            temp_xyz[Z] = room_select_xyz[i][Z]+3.5f;
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[X]-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+temp_xyz[Y]-(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[Y]-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+temp_xyz[X]-(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+((temp_xyz[X]-temp_xyz[Y])*0.5f)-(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+((temp_xyz[X]+temp_xyz[Y])*0.5f)-(vector_xyz[Y]*0.22f), temp_xyz[Z]-0.75f, TRUE);
                        }


                        // Four vertices at bottom of post...
                        temp_xyz[Z] = room_select_xyz[i][Z];
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[X], room_select_xyz[i][Y]+temp_xyz[Y], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[Y], room_select_xyz[i][Y]+temp_xyz[X], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X], room_select_xyz[i][Y]-temp_xyz[Y], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[Y], room_select_xyz[i][Y]-temp_xyz[X], temp_xyz[Z], TRUE);


                        // Four vertices at top of post...
                        temp_xyz[Z] = room_select_xyz[i][Z]+4.0f;
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[X], room_select_xyz[i][Y]+temp_xyz[Y], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[Y], room_select_xyz[i][Y]+temp_xyz[X], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X], room_select_xyz[i][Y]-temp_xyz[Y], temp_xyz[Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[Y], room_select_xyz[i][Y]-temp_xyz[X], temp_xyz[Z], TRUE);


                        if(i < num_segment)
                        {
                            // Three vertices on right side of post for bottom railing
                            temp_xyz[Z] = room_select_xyz[i][Z]+1.25f;
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X]+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]-temp_xyz[Y]+(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[Y]+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]-temp_xyz[X]+(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+((temp_xyz[Y]-temp_xyz[X])*0.5f)+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+((-temp_xyz[X]-temp_xyz[Y])*0.5f)+(vector_xyz[Y]*0.22f), temp_xyz[Z]-0.75f, TRUE);


                            // Three vertices on right side of post for top railing
                            temp_xyz[Z] = room_select_xyz[i][Z]+3.5f;
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X]+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]-temp_xyz[Y]+(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+temp_xyz[Y]+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]-temp_xyz[X]+(vector_xyz[Y]*0.22f), temp_xyz[Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+((temp_xyz[Y]-temp_xyz[X])*0.5f)+(vector_xyz[X]*0.22f), room_select_xyz[i][Y]+((-temp_xyz[X]-temp_xyz[Y])*0.5f)+(vector_xyz[Y]*0.22f), temp_xyz[Z]-0.75f, TRUE);
                        }


                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                            final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            i++;
                        }
                    }
                }




                // Add extra vertices for stairs...
                if(type==HARDPLOPPER_STAIRS)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Two vertices for low end of step...  To sides of control vertices...
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X], room_select_xyz[i][Y]-final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);


                        // Two vertices for top of step...  Don't do for last step...
                        if(i < num_segment)
                        {
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X], room_select_xyz[i][Y]-final_vector_xyz[Y], room_select_xyz[i][Z]+1.0f, TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y], room_select_xyz[i][Z]+1.0f, TRUE);
                        }


                        // Also put two vertices at floor level...  Don't do for first step...
                        if(i > 0)
                        {
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X], room_select_xyz[i][Y]-final_vector_xyz[Y], room_select_xyz[0][Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y], room_select_xyz[0][Z], TRUE);
                        }


                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                            final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            i++;
                        }
                    }
                }




                // Add extra vertices for pavement...
                if(type==HARDPLOPPER_PAVEMENT)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Expand each vertex into two vertices...
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X], room_select_xyz[i][Y]-final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);


                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                            final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            i++;
                        }
                    }
                }





                // Add extra vertices for mine track...
                if(type==HARDPLOPPER_MINE)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Expand each vertex into 10 vertices...
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*-1.25f, room_select_xyz[i][Y]+final_vector_xyz[Y]*-1.25f, room_select_xyz[i][Z]+0.3f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*-1.25f, room_select_xyz[i][Y]+final_vector_xyz[Y]*-1.25f, room_select_xyz[i][Z]+0.8f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*-1.0f,  room_select_xyz[i][Y]+final_vector_xyz[Y]*-1.0f,  room_select_xyz[i][Z]+0.8f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*-1.0f,  room_select_xyz[i][Y]+final_vector_xyz[Y]*-1.0f,  room_select_xyz[i][Z]+0.3f, TRUE);

                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*1.0f,  room_select_xyz[i][Y]+final_vector_xyz[Y]*1.0f,  room_select_xyz[i][Z]+0.3f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*1.0f,  room_select_xyz[i][Y]+final_vector_xyz[Y]*1.0f,  room_select_xyz[i][Z]+0.8f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*1.25f, room_select_xyz[i][Y]+final_vector_xyz[Y]*1.25f, room_select_xyz[i][Z]+0.8f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*1.25f, room_select_xyz[i][Y]+final_vector_xyz[Y]*1.25f, room_select_xyz[i][Z]+0.3f, TRUE);

                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*-1.75f, room_select_xyz[i][Y]+final_vector_xyz[Y]*-1.75f, room_select_xyz[i][Z]+0.0f, TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*1.75f,  room_select_xyz[i][Y]+final_vector_xyz[Y]*1.75f,  room_select_xyz[i][Z]+0.0f, TRUE);



                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                            final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            i++;
                        }
                    }
                }





                // Add extra vertices for border...
                if(type==HARDPLOPPER_BORDER)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Expand each vertex into two vertices...
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X], room_select_xyz[i][Y]-final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);
                        worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y], room_select_xyz[i][Z], TRUE);


                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                            final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            i++;
                        }
                    }
                }



                // Add extra vertices for ledges...
                if(type==HARDPLOPPER_LEDGE || type==HARDPLOPPER_LEDGE_LOOP)
                {
                    k = 1;
                    if((i+1) == num_segment)
                    {
                        // Do an extra time to handle final vertex of chain...
                        k++;
                    }
                    repeat(j, k)
                    {
                        // Vertices...
                        if(base_vertex_ledge)
                        {
                            // Five vertices...
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X], room_select_xyz[i][Y], room_select_xyz[i][Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-final_vector_xyz[X]*0.5f, room_select_xyz[i][Y]-final_vector_xyz[Y]*0.5f, room_select_xyz[i][Z], TRUE);
                            RANDOMIZE_TEMP_XYZ();
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X], room_select_xyz[i][Y], room_select_xyz[i][Z]+1.0f+temp_xyz[Z]*0.5f, TRUE);
                            RANDOMIZE_TEMP_XYZ();
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X], room_select_xyz[i][Y]-temp_xyz[Y], room_select_xyz[i][Z]+ledge_height+temp_xyz[Z], TRUE);
                            RANDOMIZE_TEMP_XYZ();
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]-temp_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y]-temp_xyz[Y], room_select_xyz[i][Z]+ledge_height+temp_xyz[Z], TRUE);
                        }
                        else
                        {
                            // Four vertices...
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]*0.5f, room_select_xyz[i][Y]+final_vector_xyz[Y]*0.5f, room_select_xyz[i][Z], TRUE);
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X], room_select_xyz[i][Y], room_select_xyz[i][Z], TRUE);
                            RANDOMIZE_TEMP_XYZ();
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-temp_xyz[X], room_select_xyz[i][Y]-temp_xyz[Y], room_select_xyz[i][Z]+ledge_height+temp_xyz[Z]-1.0f, TRUE);
                            RANDOMIZE_TEMP_XYZ();
                            worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+final_vector_xyz[X]-temp_xyz[X], room_select_xyz[i][Y]+final_vector_xyz[Y]-temp_xyz[Y], room_select_xyz[i][Z]+ledge_height+temp_xyz[Z]-1.0f, TRUE);
                        }


                        // Just in case we're on the last segment...
                        if(k > 1)
                        {
                            if(type == HARDPLOPPER_LEDGE_LOOP)
                            {
                                vector_xyz[X] = room_select_xyz[0][X] - room_select_xyz[i+1][X];
                                vector_xyz[Y] = room_select_xyz[0][Y] - room_select_xyz[i+1][Y];
                                vector_xyz[Z] = 0.0f;
                                length = vector_length(vector_xyz);
                                if(length > 0.0f)
                                {
                                    vector_xyz[X]/=length;
                                    vector_xyz[Y]/=length;
                                }
                                final_vector_xyz[X] = (last_vector_xyz[Y] + vector_xyz[Y])*-scaling;
                                final_vector_xyz[Y] = (last_vector_xyz[X] + vector_xyz[X])*scaling;
                            }
                            else
                            {
                                final_vector_xyz[X] = (vector_xyz[Y] + vector_xyz[Y])*-scaling;
                                final_vector_xyz[Y] = (vector_xyz[X] + vector_xyz[X])*scaling;
                            }
                            i++;
                        }
                    }
                }
            }
        }



        // Add extra vertices for decal...  Need 4 of 'em...
        if(type==HARDPLOPPER_DECAL)
        {
            repeat(i, num_original)
            {
                worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-2.0f, room_select_xyz[i][Y]+2.0f, room_select_xyz[i][Z], TRUE);
                worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]-2.0f, room_select_xyz[i][Y]-2.0f, room_select_xyz[i][Z], TRUE);
                worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+2.0f, room_select_xyz[i][Y]-2.0f, room_select_xyz[i][Z], TRUE);
                worked &= room_srf_add_vertex(srf_file, room_select_xyz[i][X]+2.0f, room_select_xyz[i][Y]+2.0f, room_select_xyz[i][Z], TRUE);
            }
        }



        // Make sure all vertex locations are up to date...
        repeat(i, room_select_num)
        {
//log_message("INFO:   Stair vertex %d at %f, %f, %f", i, room_select_xyz[i][X], room_select_xyz[i][Y], room_select_xyz[i][Z]);
            sdf_write_unsigned_short(room_select_data[i], (unsigned short) ((signed short) (room_select_xyz[i][X]*256.0f)) );
            sdf_write_unsigned_short(room_select_data[i]+2, (unsigned short) ((signed short) (room_select_xyz[i][Y]*256.0f)) );
            sdf_write_unsigned_short(room_select_data[i]+4, (unsigned short) ((signed short) (room_select_xyz[i][Z]*256.0f)) );
        }


        if(worked)
        {
            // Add all triangles...
            if(type==HARDPLOPPER_FENCE)
            {
//log_message("INFO:   Texturing fence");
                j = num_original;  // First vertex (index into selected vertex array) of current segment...


                repeat(i, (num_segment+1))
                {
//
//  s+4    s+5     s+12    s+11                s+17    s+18
//
//  s+7    s+6         s+13                        s+19
//
//                 s+9     s+8                 s+14    s+15
//  s+0    s+1
//                     s+10                        s+16
//  s+3    s+2
//

                    // Post top & bottom triangles...
                    left_x = 0.8125f;  right_x = 0.96875f;  top_y = 0.53125f;  bottom_y = 0.9375f;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+4], room_select_list[j+5], room_select_list[j+6], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+4], room_select_list[j+6], room_select_list[j+7], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+2], room_select_list[j+1], room_select_list[j+0], tex_vertex_xy[2], tex_vertex_xy[1], tex_vertex_xy[0]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+3], room_select_list[j+2], room_select_list[j+0], tex_vertex_xy[3], tex_vertex_xy[2], tex_vertex_xy[0]);


                    // Post side triangles
                    left_x = 0.0625f;  right_x = 0.8125f;  top_y = 0.53125f;  bottom_y = 0.9375f;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+7], room_select_list[j+6], room_select_list[j+2], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+7], room_select_list[j+2], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+4], room_select_list[j+7], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+4], room_select_list[j+3], room_select_list[j+0], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+5], room_select_list[j+4], room_select_list[j+0], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+5], room_select_list[j+0], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+6], room_select_list[j+5], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+6], room_select_list[j+1], room_select_list[j+2], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Rail triangles...
                    if(i < num_segment)
                    {
                        // Do bottom railing first, then top railing...
                        vector_xyz[X] = room_select_xyz[j+8][X] - room_select_xyz[j+14][X];
                        vector_xyz[Y] = room_select_xyz[j+8][Y] - room_select_xyz[j+14][Y];
                        vector_xyz[Z] = 0.0f;
                        length = vector_length(vector_xyz);
                        left_x = 0.0f;  right_x = (length*0.15f);  top_y = 0.0625f;  bottom_y = 0.4375f;
                        tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                        tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                        tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                        tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;


                        k = 0;
                        while(k < 4)
                        {
                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+8+k], room_select_list[j+9+k], room_select_list[j+10+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+14+k], room_select_list[j+15+k], room_select_list[j+16+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);

                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+8+k], room_select_list[j+15+k], room_select_list[j+14+k], tex_vertex_xy[1], tex_vertex_xy[2], tex_vertex_xy[3]);
                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+8+k], room_select_list[j+14+k], room_select_list[j+9+k], tex_vertex_xy[1], tex_vertex_xy[3], tex_vertex_xy[0]);

                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+15+k], room_select_list[j+8+k], room_select_list[j+10+k], tex_vertex_xy[1], tex_vertex_xy[2], tex_vertex_xy[3]);
                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+15+k], room_select_list[j+10+k], room_select_list[j+16+k], tex_vertex_xy[1], tex_vertex_xy[3], tex_vertex_xy[0]);

                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+9+k], room_select_list[j+14+k], room_select_list[j+16+k], tex_vertex_xy[1], tex_vertex_xy[2], tex_vertex_xy[3]);
                            room_srf_add_texture_triangle(srf_file, 16, room_select_list[j+9+k], room_select_list[j+16+k], room_select_list[j+10+k], tex_vertex_xy[1], tex_vertex_xy[3], tex_vertex_xy[0]);
                            k+=3;
                        }
                    }


                    j+=20;
                }
            }
            if(type==HARDPLOPPER_STAIRS)
            {
//log_message("INFO:   Texturing stairs");
                j = num_original;  // First vertex (index into selected vertex array) of current segment...
                left_tex_x = 4.0f;
                right_tex_x = 0.5f;
                repeat(i, num_segment)
                {
//
//              s+2     s+3
// 2    3       s+0     s+1
// 0    1
// 4?   5?      s+s+0   s+s+5
//

                    skip = (i>0) ? 6 : 4;
                    skipstt = (i>0) ? 4 : 0;
                    skiplow = (i<(num_segment-1)) ? 4 : 2;

                    // Riser
                    left_x = ((rand()&255)*ONE_OVER_256);  right_x = left_x + (scaling*WALL_TEXTURE_SCALE*4.0f);  top_y = 0.0f;  bottom_y = WALL_TEXTURE_SCALE;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+0], room_select_list[j+2], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+2], room_select_list[j+3], room_select_list[j+1], tex_vertex_xy[1], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Walkable area of step
                    tex_vertex_xy[0][Y] = 0.95f;
                    tex_vertex_xy[1][Y] = 0.05f;
                    tex_vertex_xy[2][Y] = 0.05f;
                    tex_vertex_xy[3][Y] = 0.95f;
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[j+2], room_select_list[j+skip+0], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[3]);
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[j+skip+0], room_select_list[j+skip+1], room_select_list[j+3], tex_vertex_xy[1], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Right wall side
                    vector_xyz[X] = room_select_xyz[j+skip+1][X] - room_select_xyz[j+3][X];
                    vector_xyz[Y] = room_select_xyz[j+skip+1][Y] - room_select_xyz[j+3][Y];
                    vector_xyz[Z] = 0.0f;
                    length = vector_length(vector_xyz);
                    left_x = (right_tex_x*WALL_TEXTURE_SCALE);  right_x = ((right_tex_x+length)*WALL_TEXTURE_SCALE);  top_y = 1.0f-room_select_xyz[j+3][Z]*WALL_TEXTURE_SCALE;  bottom_y = 1.0f-room_select_xyz[j+skip+skiplow+1][Z]*WALL_TEXTURE_SCALE;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    tex_vertex_xy[4][X] = left_x;  tex_vertex_xy[4][Y] = 1.0f-room_select_xyz[j+1][Z]*WALL_TEXTURE_SCALE;
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+1], room_select_list[j+3], room_select_list[j+skip+1], tex_vertex_xy[4], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+1], room_select_list[j+skip+1], room_select_list[j+skip+skiplow+1], tex_vertex_xy[4], tex_vertex_xy[2], tex_vertex_xy[3]);
                    if(i>0)
                    {
                        room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+1], room_select_list[j+skip+skiplow+1], room_select_list[j+5], tex_vertex_xy[4], tex_vertex_xy[3], tex_vertex_xy[0]);
                    }
                    right_tex_x += length;


                    // Left wall side
                    vector_xyz[X] = room_select_xyz[j+skip+0][X] - room_select_xyz[j+2][X];
                    vector_xyz[Y] = room_select_xyz[j+skip+0][Y] - room_select_xyz[j+2][Y];
                    vector_xyz[Z] = 0.0f;
                    length = vector_length(vector_xyz);
                    left_x = ((left_tex_x-length)*WALL_TEXTURE_SCALE);  right_x = (left_tex_x*WALL_TEXTURE_SCALE);  top_y = 1.0f-room_select_xyz[j+2][Z]*WALL_TEXTURE_SCALE;  bottom_y = 1.0f-room_select_xyz[j+skip+skiplow+0][Z]*WALL_TEXTURE_SCALE;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    tex_vertex_xy[4][X] = right_x; tex_vertex_xy[4][Y] = 1.0f-room_select_xyz[j+0][Z]*WALL_TEXTURE_SCALE;
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+0], room_select_list[j+skip+0], room_select_list[j+2], tex_vertex_xy[4], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+0], room_select_list[j+skip+skiplow], room_select_list[j+skip+0], tex_vertex_xy[4], tex_vertex_xy[0], tex_vertex_xy[1]);
                    if(i>0)
                    {
                        room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+0], room_select_list[j+4], room_select_list[j+skip+skiplow], tex_vertex_xy[4], tex_vertex_xy[3], tex_vertex_xy[0]);
                    }
                    left_tex_x -= length;


                    // Unseen bottom (for lines)
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+skipstt], room_select_list[j+skipstt+1], room_select_list[j+skip+skiplow+1], NULL, NULL, NULL);
                    room_srf_add_texture_triangle(srf_file, 6, room_select_list[j+skipstt], room_select_list[j+skip+skiplow+1], room_select_list[j+skip+skiplow], NULL, NULL, NULL);
                    j+=skip;
                }
            }
            if(type==HARDPLOPPER_LEDGE || type==HARDPLOPPER_LEDGE_LOOP)
            {
//log_message("INFO:   Texturing fence");
                skiplow=(base_vertex_ledge)?1:0;
                scaling = WALL_TEXTURE_SCALE;
                if(type == HARDPLOPPER_LEDGE_LOOP)
                {
                    // Find the total length of the ledge...
                    j = num_original;  // First vertex (index into selected vertex array) of current segment...
                    right_tex_x = 0.0f;
                    repeat(i, (num_segment+1))
                    {
                        skip=j+skiplow+4;  if(i >= num_segment) { skip = num_original; }
                        vector_xyz[X] = room_select_xyz[skip+skiplow+2][X] - room_select_xyz[j+skiplow+2][X];
                        vector_xyz[Y] = room_select_xyz[skip+skiplow+2][Y] - room_select_xyz[j+skiplow+2][Y];
                        vector_xyz[Z] = 0.0f;
                        length = vector_length(vector_xyz);
                        right_tex_x += length;
                        j+=skiplow+4;
                    }
//log_message("INFO:   Doing ledge loop...");
//log_message("INFO:     Length = %f", right_tex_x);


                    // Recompute scaling to ensure a complete wrap of texture coordinates...
                    if(right_tex_x > 0.0f)
                    {
                        scaling = (float) ((int) (right_tex_x*WALL_TEXTURE_SCALE));
//log_message("INFO:     Scaling = %f", scaling);
                        scaling = scaling/right_tex_x;
//log_message("INFO:     Scaling = %f", scaling);
//log_message("INFO:     Tex length = %f", scaling * right_tex_x);
                    }
                }
                j = num_original;  // First vertex (index into selected vertex array) of current segment...
                right_tex_x = 0.0f;
                repeat(i, (num_segment+(type==HARDPLOPPER_LEDGE_LOOP)))
                {
                    // Skip takes the place of j (for next set of vertices)...
                    skip=j+skiplow+4;  if(i >= num_segment) { skip = num_original; }


                    // Ledge cap triangles...
                    vector_xyz[X] = room_select_xyz[skip+skiplow+2][X] - room_select_xyz[j+skiplow+2][X];
                    vector_xyz[Y] = room_select_xyz[skip+skiplow+2][Y] - room_select_xyz[j+skiplow+2][Y];
                    vector_xyz[Z] = 0.0f;
                    length = vector_length(vector_xyz);
                    right_x = (right_tex_x*scaling);  left_x = ((right_tex_x+length)*scaling);  top_y = 0.0390625f;  bottom_y = 0.9609375f;
                    right_tex_x += length;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+skiplow+2], room_select_list[skip+skiplow+3], room_select_list[j+skiplow+3], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+skiplow+2], room_select_list[j+skiplow+3], room_select_list[j+skiplow+2], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Ledge base triangles...
                    if(base_vertex_ledge)
                    {
                        room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+1], room_select_list[skip+2], room_select_list[j+2], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                        room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+1], room_select_list[j+2], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    }


                    // Unseen bottom (for lines)
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+0], room_select_list[skip+1], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 5, room_select_list[skip+0], room_select_list[j+1], room_select_list[j+0], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Ledge wall triangles...
                    top_y = 0.0f;  bottom_y = (room_select_xyz[skip+skiplow+2][Z]-room_select_xyz[skip+skiplow+1][Z])*WALL_TEXTURE_SCALE;;
                    tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                    tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                    tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                    tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;
                    room_srf_add_texture_triangle(srf_file, 4, room_select_list[skip+skiplow+1], room_select_list[skip+skiplow+2], room_select_list[j+skiplow+2], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 4, room_select_list[skip+skiplow+1], room_select_list[j+skiplow+2], room_select_list[j+skiplow+1], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                    // Skip to the next set of vertices...
                    j+=skiplow+4;
                }
            }
            if(type==HARDPLOPPER_DECAL)
            {
                width &= 15;  // Really decal number...
                temp = (width & 3) * 0.25f;
                left_x = temp + 0.0078125f;  right_x = temp + 0.2421875f;
                temp = (width>>2) * 0.25f;
                top_y = temp + 0.0078125f;  bottom_y = temp + 0.2421875f;
                tex_vertex_xy[0][X] = left_x;  tex_vertex_xy[0][Y] = bottom_y;
                tex_vertex_xy[1][X] = left_x;  tex_vertex_xy[1][Y] = top_y;
                tex_vertex_xy[2][X] = right_x; tex_vertex_xy[2][Y] = top_y;
                tex_vertex_xy[3][X] = right_x; tex_vertex_xy[3][Y] = bottom_y;


                j = num_original;
                repeat(i, num_original)
                {
                    room_srf_add_texture_triangle(srf_file, 29, room_select_list[j+0], room_select_list[j+1], room_select_list[j+2], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 29, room_select_list[j+0], room_select_list[j+2], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    j+=4;
                }
            }
            if(type==HARDPLOPPER_PAVEMENT)
            {
//log_message("INFO:   Texturing pavement");
                tex_vertex_xy[0][X] = 0.0f;
                tex_vertex_xy[1][X] = 0.0f;
                tex_vertex_xy[2][X] = 1.0f;
                tex_vertex_xy[3][X] = 1.0f;
                j = num_original;  // First vertex (index into selected vertex array) of current segment...
                repeat(i, num_segment)
                {
                    // Reset bottom edges...
                    tex_vertex_xy[0][Y] = 0.0f;
                    tex_vertex_xy[3][Y] = 0.0f;


                    // Left side of road...
                    vector_xyz[X] = room_select_xyz[j+0][X] - room_select_xyz[j+2][X];
                    vector_xyz[Y] = room_select_xyz[j+0][Y] - room_select_xyz[j+2][Y];
                    vector_xyz[Z] = room_select_xyz[j+0][Z] - room_select_xyz[j+2][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[1][Y] = length * WALL_TEXTURE_SCALE * 4.0f + 0.25f;
                    tex_vertex_xy[1][Y] = ((int) (tex_vertex_xy[1][Y] * 4.0f)) * 0.25f;


                    // Right side of road...
                    vector_xyz[X] = room_select_xyz[j+1][X] - room_select_xyz[j+3][X];
                    vector_xyz[Y] = room_select_xyz[j+1][Y] - room_select_xyz[j+3][Y];
                    vector_xyz[Z] = room_select_xyz[j+1][Z] - room_select_xyz[j+3][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[2][Y] = length * WALL_TEXTURE_SCALE * 4.0f + 0.25f;
                    tex_vertex_xy[2][Y] = ((int) (tex_vertex_xy[2][Y] * 4.0f)) * 0.25f;


                    // Plop the triangles...
                    room_srf_add_texture_triangle(srf_file, 17, room_select_list[j+0], room_select_list[j+2], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 17, room_select_list[j+0], room_select_list[j+3], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    j+=2;
                }
            }
            if(type==HARDPLOPPER_BORDER)
            {
//log_message("INFO:   Texturing border");
                texture = (unsigned char) ((ledge_height-2)&31);
                tex_vertex_xy[0][X] = 0.0f;
                tex_vertex_xy[1][X] = 0.0f;
                tex_vertex_xy[2][X] = 0.0f;
                tex_vertex_xy[3][X] = 0.0f;
                tex_vertex_xy[0][Y] = 0.0625f;
                tex_vertex_xy[1][Y] = 0.0625f;
                tex_vertex_xy[2][Y] = 0.9375f;
                tex_vertex_xy[3][Y] = 0.9375f;
                j = num_original;  // First vertex (index into selected vertex array) of current segment...
                repeat(i, num_segment)
                {
                    // Reset edges...
                    tex_vertex_xy[0][X] = tex_vertex_xy[1][X];
                    tex_vertex_xy[3][X] = tex_vertex_xy[2][X];


                    // Left side of border...
                    vector_xyz[X] = room_select_xyz[j+0][X] - room_select_xyz[j+2][X];
                    vector_xyz[Y] = room_select_xyz[j+0][Y] - room_select_xyz[j+2][Y];
                    vector_xyz[Z] = room_select_xyz[j+0][Z] - room_select_xyz[j+2][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[1][X] = tex_vertex_xy[0][X] + (length * WALL_TEXTURE_SCALE * 2.0f);


                    // Right side of border...
                    vector_xyz[X] = room_select_xyz[j+1][X] - room_select_xyz[j+3][X];
                    vector_xyz[Y] = room_select_xyz[j+1][Y] - room_select_xyz[j+3][Y];
                    vector_xyz[Z] = room_select_xyz[j+1][Z] - room_select_xyz[j+3][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[2][X] = tex_vertex_xy[3][X] + (length * WALL_TEXTURE_SCALE * 2.0f);


                    // Average 'em together...
                    tex_vertex_xy[1][X] = (tex_vertex_xy[1][X] + tex_vertex_xy[2][X]) * 0.5f;
                    tex_vertex_xy[2][X] = tex_vertex_xy[1][X];


                    // Plop the triangles...
                    room_srf_add_texture_triangle(srf_file, texture, room_select_list[j+0], room_select_list[j+2], room_select_list[j+3], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, texture, room_select_list[j+0], room_select_list[j+3], room_select_list[j+1], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                    j+=2;
                }
            }
            if(type==HARDPLOPPER_MINE)
            {
//log_message("INFO:   Texturing mine track");

                tex_vertex_xy[0][X] = 0.0f;
                tex_vertex_xy[1][X] = 0.0f;
                tex_vertex_xy[2][X] = 0.0f;
                tex_vertex_xy[3][X] = 0.0f;
                j = num_original;  // First vertex (index into selected vertex array) of current segment...
                repeat(i, num_segment)
                {
                    // Reset edges...
                    tex_vertex_xy[0][X] = tex_vertex_xy[1][X];
                    tex_vertex_xy[3][X] = tex_vertex_xy[2][X];


                    // Left side of border...
                    vector_xyz[X] = room_select_xyz[j+8][X] - room_select_xyz[j+18][X];
                    vector_xyz[Y] = room_select_xyz[j+8][Y] - room_select_xyz[j+18][Y];
                    vector_xyz[Z] = room_select_xyz[j+8][Z] - room_select_xyz[j+18][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[1][X] = tex_vertex_xy[0][X] + (length * WALL_TEXTURE_SCALE * 2.0f);


                    // Right side of border...
                    vector_xyz[X] = room_select_xyz[j+9][X] - room_select_xyz[j+19][X];
                    vector_xyz[Y] = room_select_xyz[j+9][Y] - room_select_xyz[j+19][Y];
                    vector_xyz[Z] = room_select_xyz[j+9][Z] - room_select_xyz[j+19][Z];
                    length = vector_length(vector_xyz);
                    tex_vertex_xy[2][X] = tex_vertex_xy[3][X] + (length * WALL_TEXTURE_SCALE * 2.0f);


                    // Average 'em together...
                    tex_vertex_xy[1][X] = (tex_vertex_xy[1][X] + tex_vertex_xy[2][X]) * 0.5f;
                    tex_vertex_xy[2][X] = tex_vertex_xy[1][X];



                    // Plop rail triangles...
                    tex_vertex_xy[0][X] *= 0.25f;
                    tex_vertex_xy[1][X] *= 0.25f;
                    tex_vertex_xy[2][X] *= 0.25f;
                    tex_vertex_xy[3][X] *= 0.25f;
                    k = 0;
                    while(k < 5)
                    {
                        // Left side
                        tex_vertex_xy[0][Y] = 0.00f;
                        tex_vertex_xy[1][Y] = 0.00f;
                        tex_vertex_xy[2][Y] = 0.25f;
                        tex_vertex_xy[3][Y] = 0.25f;
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+0+k], room_select_list[j+10+k], room_select_list[j+11+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+0+k], room_select_list[j+11+k], room_select_list[j+1+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                        // Top side
                        tex_vertex_xy[0][Y] = 0.25f;
                        tex_vertex_xy[1][Y] = 0.25f;
                        tex_vertex_xy[2][Y] = 0.50f;
                        tex_vertex_xy[3][Y] = 0.50f;
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+1+k], room_select_list[j+11+k], room_select_list[j+12+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+1+k], room_select_list[j+12+k], room_select_list[j+2+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                        // Right side
                        tex_vertex_xy[0][Y] = 0.50f;
                        tex_vertex_xy[1][Y] = 0.50f;
                        tex_vertex_xy[2][Y] = 0.75f;
                        tex_vertex_xy[3][Y] = 0.75f;
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+2+k], room_select_list[j+12+k], room_select_list[j+13+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+2+k], room_select_list[j+13+k], room_select_list[j+3+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);


                        // Under side
                        tex_vertex_xy[0][Y] = 0.75f;
                        tex_vertex_xy[1][Y] = 0.75f;
                        tex_vertex_xy[2][Y] = 1.00f;
                        tex_vertex_xy[3][Y] = 1.00f;
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+3+k], room_select_list[j+13+k], room_select_list[j+10+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                        room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+3+k], room_select_list[j+10+k], room_select_list[j+0+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);



                        // Plop starting segment endcaps...
                        if(i == 0)
                        {
                            room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+0+k], room_select_list[j+1+k], room_select_list[j+2+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                            room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+0+k], room_select_list[j+2+k], room_select_list[j+3+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                        }


                        // Plop ending segment endcaps...
                        if(i == (num_segment-1))
                        {
                            room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+10+k], room_select_list[j+13+k], room_select_list[j+12+k], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                            room_srf_add_texture_triangle(srf_file, 14, room_select_list[j+10+k], room_select_list[j+12+k], room_select_list[j+11+k], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);
                        }

                        k+=4;
                    }
                    tex_vertex_xy[0][X] *= 4.00f;
                    tex_vertex_xy[1][X] *= 4.00f;
                    tex_vertex_xy[2][X] *= 4.00f;
                    tex_vertex_xy[3][X] *= 4.00f;



                    // Plop wooden tie triangles
                    tex_vertex_xy[0][Y] = 0.00f;
                    tex_vertex_xy[1][Y] = 0.00f;
                    tex_vertex_xy[2][Y] = 1.00f;
                    tex_vertex_xy[3][Y] = 1.00f;
                    room_srf_add_texture_triangle(srf_file, 15, room_select_list[j+8], room_select_list[j+18], room_select_list[j+19], tex_vertex_xy[0], tex_vertex_xy[1], tex_vertex_xy[2]);
                    room_srf_add_texture_triangle(srf_file, 15, room_select_list[j+8], room_select_list[j+19], room_select_list[j+9], tex_vertex_xy[0], tex_vertex_xy[2], tex_vertex_xy[3]);




                    j+=10;
                }
            }






        }



        // Delete all control vertices...
        room_select_num = num_original;
        i = 50000;
        while(i > 0)
        {
            i--;
            if(room_select_inlist(i))
            {
                room_srf_delete_vertex(srf_file, i);
            }
        }
        room_select_num = 0;
        room_srf_autotexture(srf_file);
    }
}
#define MAX_ROOM_COPY_VERTEX 5000
#define MAX_ROOM_COPY_TRIANGLE 5000
unsigned int num_room_copy_vertex = 0;
unsigned int num_room_copy_triangle = 0;
unsigned short room_copy_vertex_original[MAX_ROOM_COPY_VERTEX];
float room_copy_vertex_xyz[MAX_ROOM_COPY_VERTEX][3];
unsigned short room_copy_triangle_texture[MAX_ROOM_COPY_TRIANGLE];
unsigned short room_copy_triangle_vertex[MAX_ROOM_COPY_TRIANGLE][3];
float room_copy_triangle_tex_vertex_xy[MAX_ROOM_COPY_TRIANGLE][3][2];
void room_srf_copy(unsigned char* data)
{
    // <ZZ> This function copies the selected vertices and any attached triangles into an internal
    //      copy buffer...
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned short num_tex_vertex;
    unsigned short i, j, k;
    unsigned short vertex, tex_vertex, num_triangle;
    unsigned char match_count;
    float vertex_xyz[3];

//log_message("INFO:   Copying vertices & stuff...");

    if(room_select_num > 0 && room_select_num < MAX_ROOM_COPY_VERTEX)
    {
        tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
        num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
        triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
//log_message("INFO:     num_tex_vertex = %d", num_tex_vertex);


        // Copy selected vertices into buffer...
        repeat(i, room_select_num)
        {
            room_copy_vertex_original[i] = room_select_list[i];
            room_copy_vertex_xyz[i][X] = room_select_xyz[i][X];
            room_copy_vertex_xyz[i][Y] = room_select_xyz[i][Y];
            room_copy_vertex_xyz[i][Z] = room_select_xyz[i][Z];
        }
        num_room_copy_vertex = room_select_num;
//log_message("INFO:     Copied all %d vertices", num_room_copy_vertex);


        // Copy selected tex triangles and related stuff into buffer...
        num_room_copy_triangle = 0;
        repeat(i, MAX_ROOM_TEXTURE)
        {
            num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
            repeat(j, num_triangle)
            {
                match_count = 0;
                repeat(k, 3)
                {
                    vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=4;
                    if(room_select_inlist(vertex))
                    {
                        match_count++;
                    }
                }
                if(match_count == 3 && num_room_copy_triangle < MAX_ROOM_COPY_TRIANGLE)
                {
//log_message("INFO:     Tex-Triangle %d-%d is fully selected", i, j);
                    // All 3 of this triangle's vertices are selected, so let's add it to our list...
                    triangle_data-=12;
                    repeat(k, 3)
                    {
                        vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                        tex_vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                        room_draw_srf_tex_vertex_helper(tex_vertex);
                        room_copy_triangle_vertex[num_room_copy_triangle][k] = vertex;
                        room_copy_triangle_tex_vertex_xy[num_room_copy_triangle][k][X] = vertex_xyz[X];
                        room_copy_triangle_tex_vertex_xy[num_room_copy_triangle][k][Y] = vertex_xyz[Y];
                    }
                    room_copy_triangle_texture[num_room_copy_triangle] = i;
                    num_room_copy_triangle++;
                }
            }
        }
    }
}
void room_srf_paste(unsigned char* data)
{
    // <ZZ> This function pastes the stuff in the internal copy buffer back into the room...
    unsigned int temp;
    unsigned short i, j, k;
    unsigned char worked;
    unsigned char* vertex_data;
    unsigned short num_vertex_original;
    unsigned short vertex[3];


    // Error Check...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex_original = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    temp = num_vertex_original;
    temp += num_room_copy_vertex;
    if(temp > 65535)
    {
        return;
    }


    // Add all copied vertices back into room...
    room_select_clear();
    worked = FALSE;
    repeat(i, num_room_copy_vertex)
    {
        worked |= room_srf_add_vertex(data, room_copy_vertex_xyz[i][X], room_copy_vertex_xyz[i][Y], room_copy_vertex_xyz[i][Z], TRUE);
    }



    if(worked)
    {
        // Add all of the copied triangles back into room...
        repeat(i, num_room_copy_triangle)
        {
            worked = 0;
            repeat(j, 3)
            {
                vertex[j] = 0;
                k = 0;
                while(k < num_room_copy_vertex)
                {
                    if(room_copy_triangle_vertex[i][j] == room_copy_vertex_original[k])
                    {
                        vertex[j] = (k+num_vertex_original);
                        k = num_room_copy_vertex;
                        worked++;
                    }
                    k++;
                }
            }
            if(worked == 3)
            {
                room_srf_add_texture_triangle(data, (unsigned char) room_copy_triangle_texture[i], vertex[0], vertex[1], vertex[2], room_copy_triangle_tex_vertex_xy[i][0], room_copy_triangle_tex_vertex_xy[i][1], room_copy_triangle_tex_vertex_xy[i][2]);
            }
        }
    }
}
#endif
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------







//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
unsigned char room_find_wall_center(unsigned char* data, unsigned short rotation, unsigned short wall, float* center_xyz, float* offset_xyz, float normal_add)
{
    // <ZZ> This function finds the center xyz position of a given wall...  Returns TRUE if it worked
    //      and fills in the values of center_xyz[]...  Returns FALSE if it failed...
    float angle, sine, cosine;
    float temp_xyz[3];
    float vertex_xyz[3];
    float side_xy[2];
    float distance;
    unsigned short num_vertex, num_exterior_wall, vertex, last_vertex;
    unsigned char* vertex_data;
    unsigned char* exterior_wall_data;
    float x, y, z;

    // Generate the rotation information...
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);
    x = offset_xyz[X];
    y = offset_xyz[Y];
    z = offset_xyz[Z];


    // Start to read the SRF file...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    if(wall < num_exterior_wall)
    {
        vertex = sdf_read_unsigned_short(exterior_wall_data+(3*wall));
        if(wall == 0)
        {
            last_vertex = sdf_read_unsigned_short((exterior_wall_data+(3*num_exterior_wall)-3));
        }
        else
        {
            wall--;
            last_vertex = sdf_read_unsigned_short(exterior_wall_data+(3*wall));
        }
        room_draw_srf_vertex_helper(last_vertex);
        center_xyz[X] = vertex_xyz[X];
        center_xyz[Y] = vertex_xyz[Y];
        center_xyz[Z] = vertex_xyz[Z];
        room_draw_srf_vertex_helper(vertex);
        if(normal_add > 0.001f)
        {
            side_xy[X] = vertex_xyz[Y] - center_xyz[Y];
            side_xy[Y] = -(vertex_xyz[X] - center_xyz[X]);
            distance = ((float) sqrt(side_xy[X]*side_xy[X] + side_xy[Y]*side_xy[Y])) + 0.0000001f;
            normal_add = normal_add / distance;
            center_xyz[X] += vertex_xyz[X];
            center_xyz[Y] += vertex_xyz[Y];
            center_xyz[Z] += vertex_xyz[Z];
            center_xyz[X] *= 0.5f;
            center_xyz[Y] *= 0.5f;
            center_xyz[Z] *= 0.5f;
            center_xyz[X]+=(side_xy[X]*normal_add);
            center_xyz[Y]+=(side_xy[Y]*normal_add);
            map_room_door_spin = ((unsigned short) (atan2(side_xy[Y], side_xy[X])*10430.37835047f)) + 32768;
            return TRUE;
        }
        center_xyz[X] += vertex_xyz[X];
        center_xyz[Y] += vertex_xyz[Y];
        center_xyz[Z] += vertex_xyz[Z];
        center_xyz[X] *= 0.5f;
        center_xyz[Y] *= 0.5f;
        center_xyz[Z] *= 0.5f;
        return TRUE;
    }
    center_xyz[X] = x;
    center_xyz[Y] = y;
    center_xyz[Z] = z;
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
unsigned char room_find_best_wall(unsigned char* data, unsigned char allow_bottom_doors, unsigned short rotation, float* vector_xy, unsigned char* walls_to_not_use)
{
    // <ZZ> This function returns the best wall for sticking a door in for a given SRF file and
    //      a direction out of the room...  Walls_to_not_use is a list of 5 walls that already
    //      have doors in 'em...   Returns 255 if there weren't any good walls...  vector_xy should
    //      be of unit length...
    float angle, sine, cosine;
    float dot, best_dot;
    float temp_xyz[3];
    float vertex_xyz[3];
    float last_vertex_xyz[3];
    float wall_xy[2];
    float side_xy[2];
    float distance;
    unsigned char keep_going;
    unsigned short i, j, num_vertex, num_exterior_wall, best_wall, vertex, last_vertex;
    unsigned char* vertex_data;
    unsigned char* exterior_wall_data;
    unsigned char* read;
    unsigned char door_flags;
    float x, y, z;


    // Generate the rotation information...
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);
    x = 0.0f;
    y = 0.0f;
    z = 0.0f;


    // Start to read the SRF file...
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;
    exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
    num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
    read = exterior_wall_data;


    // Go through each wall segment...
    best_wall = 255;
    best_dot = 0.707f;
    vertex = sdf_read_unsigned_short((read+(3*num_exterior_wall)-3));
    repeat(i, num_exterior_wall)
    {
        last_vertex = vertex;
        vertex = sdf_read_unsigned_short(read);  read+=2;
        door_flags = *read;  read++;

        if((door_flags & 15) >= ROOM_WALL_FLAG_LOW_DOOR)
        {
            // Flagged as possible door...  Make sure it's not in our do-not-use list...
            keep_going = TRUE;
            repeat(j, 5)
            {
                if(i == ((unsigned short) walls_to_not_use[j]))
                {
                    keep_going = FALSE;
                }
            }
            if(keep_going)
            {
                // Check the dot product, of the wall side normal & the supplied vector to
                // see if this is the best wall...
                room_draw_srf_vertex_helper(last_vertex);
                last_vertex_xyz[X] = vertex_xyz[X];
                last_vertex_xyz[Y] = vertex_xyz[Y];
                last_vertex_xyz[Z] = vertex_xyz[Z];
                room_draw_srf_vertex_helper(vertex);
                wall_xy[X] = vertex_xyz[X] - last_vertex_xyz[X];
                wall_xy[Y] = vertex_xyz[Y] - last_vertex_xyz[Y];
                side_xy[X] = wall_xy[Y];
                side_xy[Y] = -wall_xy[X];
                distance = ((float) sqrt(side_xy[X]*side_xy[X] + side_xy[Y]*side_xy[Y])) + 0.000001f;
                side_xy[X]/=distance;
                side_xy[Y]/=distance;
                dot = side_xy[X]*vector_xy[X] + side_xy[Y]*vector_xy[Y];
                if(dot > best_dot)
                {
                    if((vertex_xyz[Z] > -1.0f && last_vertex_xyz[Z] > -1.0f) || allow_bottom_doors)
                    {
                        best_wall = i;
                        best_dot = dot;
                    }
                }
            }
        }
    }
    if(best_wall < 255)
    {
        return ((unsigned char) best_wall);
    }
    return 255;
}


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
    unsigned int room_editor_texture[MAX_ROOM_TEXTURE];
#endif
#define ROOM_WORKMODE_VERTEX   0
#define ROOM_WORKMODE_WAYPOINT 1
#define ROOM_WORKMODE_BRIDGE   2
#define ROOM_WORKMODE_OBJECT   3
void room_draw_srf(float x, float y, float z, unsigned char* data, unsigned char* color_rgb, unsigned short rotation, unsigned char mode)
{
    // <ZZ> This function draws a compressed (.SRF) room...  Used for the minimap, and also for
    //      the room editor tool.
    unsigned char submode, workmode;
    unsigned char* minimap_data;
    unsigned char* vertex_data;
    unsigned short num_minimap_triangle, num_vertex, i, j;
    float temp_xyz[3];
    float vertex_xyz[3];
    float angle, sine, cosine;
    #ifdef DEVTOOL
        unsigned char length;
        float center_xyz[3];
        float perp_xyz[3];
        unsigned char* tex_vertex_data;
        unsigned char* triangle_data;
        unsigned char* exterior_wall_data;
        unsigned char* waypoint_data;
        unsigned char* waypoint_line_data;
        unsigned char found_connection;
        unsigned short num_group, num_object;
        unsigned short num_tex_vertex, num_triangle, num_exterior_wall, num_waypoint_line, num_bridge;
        unsigned char num_waypoint;
        unsigned short k, m, vertex, tex_vertex;
        unsigned short best_vertex;
        float best_depth;
        unsigned char* best_data;
        unsigned char* search_data;
        float gx, gy;
        unsigned char* bridge_data;
        unsigned char* object_group_data;
        float new_x, new_y, new_d;
        unsigned char flags;
        unsigned char inlist, team;
        float size;
    #endif


    // Figger our mode stuff...
    submode = mode&15;   // 0 = Normal, 1 = Crosshairs, 2 = Dual crosshairs
    workmode = submode;  // 0 = Vertex, 1 = Waypoint, 2 = Bridge, 3 = Object group
    mode = mode&240;
    if(mode == ROOM_MODE_GRID || mode == ROOM_MODE_OBJECT_GROUP)
    {
        submode = 0;  // Low 15 bits interpretted as workmode when we're in grid/object mode...
    }


    // Generate the rotation information...
    angle = rotation * (2.0f * PI / 65536.0f);
    sine = (float) sin(angle);
    cosine = (float) cos(angle);


    // Pick our color...
    display_color(color_rgb);



    // Start to read the SRF file...
    minimap_data = data + sdf_read_unsigned_int(data+SRF_MINIMAP_OFFSET);
    num_minimap_triangle = sdf_read_unsigned_short(minimap_data);  minimap_data+=2;
    vertex_data = data + sdf_read_unsigned_int(data+SRF_VERTEX_OFFSET);
    num_vertex = sdf_read_unsigned_short(vertex_data);  vertex_data+=2;



    // Draw the room for the minimap...
    if(mode == ROOM_MODE_MINIMAP)
    {
        // Draw the minimap geometry...
        repeat(i, num_minimap_triangle)
        {
            display_start_fan();
                j = sdf_read_unsigned_short(minimap_data+(i*6));    room_draw_srf_vertex_helper(j);  display_vertex(vertex_xyz);
                j = sdf_read_unsigned_short(minimap_data+(i*6)+2);  room_draw_srf_vertex_helper(j);  display_vertex(vertex_xyz);
                j = sdf_read_unsigned_short(minimap_data+(i*6)+4);  room_draw_srf_vertex_helper(j);  display_vertex(vertex_xyz);
            display_end();
        }
    }


    #ifdef DEVTOOL
        exterior_wall_data = data + sdf_read_unsigned_int(data+SRF_EXTERIOR_WALL_OFFSET);
        num_exterior_wall = sdf_read_unsigned_short(exterior_wall_data);  exterior_wall_data+=2;
        tex_vertex_data = data + sdf_read_unsigned_int(data+SRF_TEX_VERTEX_OFFSET);
        num_tex_vertex = sdf_read_unsigned_short(tex_vertex_data);  tex_vertex_data+=2;
        triangle_data = data + sdf_read_unsigned_int(data+SRF_TRIANGLE_OFFSET);
        waypoint_data = data + sdf_read_unsigned_int(data+SRF_WAYPOINT_OFFSET);
        num_waypoint = *waypoint_data;  waypoint_data+=2;
        waypoint_line_data = waypoint_data+(num_waypoint*4) + (num_waypoint*num_waypoint);
        num_waypoint_line = sdf_read_unsigned_short(waypoint_line_data);  waypoint_line_data+=2;
        bridge_data = data + sdf_read_unsigned_int(data+SRF_BRIDGE_OFFSET);
        num_bridge = *bridge_data;  bridge_data+=2;
        object_group_data = data + sdf_read_unsigned_int(data+SRF_OBJECT_GROUP_OFFSET);
        num_group = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;



        // Draw the xy grid...
        if(mode == ROOM_MODE_GRID)
        {
            // Horizontal lines...
            display_texture_off();
            vertex_xyz[Z] = 0.0f;
            gy = -100.0f;
            while(gy < 101.0f)
            {
                display_start_line();
                    vertex_xyz[Y] = gy;
                    vertex_xyz[X] = -100.0f;  display_vertex(vertex_xyz);
                    vertex_xyz[X] =  100.0f;  display_vertex(vertex_xyz);
                display_end();
                gy+=10.0f;
            }
            // Vertical lines...
            gx = -100.0f;
            while(gx < 101.0f)
            {
                display_start_line();
                    vertex_xyz[X] = gx;
                    vertex_xyz[Y] = -100.0f;  display_vertex(vertex_xyz);
                    vertex_xyz[Y] =  100.0f;  display_vertex(vertex_xyz);
                display_end();
                gx+=10.0f;
            }
            display_texture_on();








            // Handle box selection of vertices...  Other stuff too...
            if(plopping_bridge && !mouse_down[BUTTON0])
            {
                room_srf_add_bridge(data, select_center_xyz[X], select_center_xyz[Y], select_offset_xyz[X], select_offset_xyz[Y]);
                plopping_bridge = FALSE;
            }
            if(selection_close_type == BORDER_SELECT)
            {
                if(workmode == ROOM_WORKMODE_VERTEX)
                {
                    repeat(i, num_vertex)
                    {
                        if(!room_select_inlist(i))
                        {
                            room_draw_srf_vertex_helper(i);
                            render_get_point_xy(vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], &new_x, &new_y);
                            if(new_x > selection_box_tl[X] &&  new_x < selection_box_br[X])
                            {
                                if(new_y > selection_box_tl[Y] &&  new_y < selection_box_br[Y])
                                {
                                    room_select_add(i, vertex_data+(i*6), 3);
                                }
                            }
                        }
                    }
                }
            }
            else if(selection_close_type == BORDER_CROSS_HAIRS)
            {
                room_get_point_xyz(((mouse_x - script_window_x)/script_window_w), ((mouse_y - script_window_y)/script_window_h), &select_center_xyz[X], &select_center_xyz[Y], &select_center_xyz[Z], 100.0f,(unsigned char) (key_down[SDL_SCANCODE_LCTRL] || key_down[SDL_SCANCODE_RCTRL]), (unsigned char) (key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT]));
            }
            else if(selection_close_type == BORDER_MOVE)
            {
                if(select_move_on)
                {
                    // Working on movement...  Center xyz should've been magically set by now...
                    room_get_point_xyz(((mouse_x - script_window_x)/script_window_w), ((mouse_y - script_window_y)/script_window_h), &select_offset_xyz[X], &select_offset_xyz[Y], &select_offset_xyz[Z], 100.0f,(unsigned char) (key_down[SDL_SCANCODE_LCTRL] || key_down[SDL_SCANCODE_RCTRL]), (unsigned char) (key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT]));
                    room_srf_move();
                    if(selection_move == MOVE_MODE_BRIDGE_PLOP)
                    {
                        plopping_bridge = TRUE;
                    }
                }
                else
                {
                    // Done with movement
                    room_get_point_xyz(((mouse_x - script_window_x)/script_window_w), ((mouse_y - script_window_y)/script_window_h), &select_center_xyz[X], &select_center_xyz[Y], &select_center_xyz[Z], 100.0f,(unsigned char) (key_down[SDL_SCANCODE_LCTRL] || key_down[SDL_SCANCODE_RCTRL]), (unsigned char) (key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT]));
                    select_center_xyz[Z] = 0.0f;
                    select_offset_xyz[X] = select_center_xyz[X];
                    select_offset_xyz[Y] = select_center_xyz[Y];
                    select_offset_xyz[Z] = select_center_xyz[Z];
                    room_select_update_xyz();
                }
            }
            else if(selection_close_type == BORDER_POINT_PICK)
            {
                best_vertex = 65535;
                best_depth = 99999.0f;
                k = 0;

                if(workmode == ROOM_WORKMODE_VERTEX)
                {
                    // Selecting vertices
                    search_data = vertex_data;  // Start of item coordinates
                    j = 6;                      // Size of each item
                    k = num_vertex;             // Number of items to check
                    m = 3;                      // Number of axes for each item...  (2 or 3)
                }
                if(workmode == ROOM_WORKMODE_WAYPOINT)
                {
                    search_data = waypoint_data;// Start of item coordinates
                    j = 4;                      // Size of each item
                    k = num_waypoint;           // Number of items to check
                    m = 2;                      // Number of axes for each item...  (2 or 3)
                }
                if(workmode == ROOM_WORKMODE_BRIDGE)
                {
                    search_data = bridge_data;  // Start of item coordinates
                    j = 4;                      // Size of each item
                    k = num_bridge*2;           // Number of items to check
                    m = 2;                      // Number of axes for each item...  (2 or 3)
                }
                if(workmode == ROOM_WORKMODE_OBJECT)
                {
                    search_data = object_group_data;    // Start of item coordinates
                    j = 22;                             // Size of each item
                    k = 0;                              // Number of items to check
                    m = 3;                              // Number of axes for each item...  (2 or 3)
                    if(global_room_active_group < num_group)
                    {
                        // Skip through data to get to the current group...
                        repeat(i, global_room_active_group)
                        {
                            num_object = sdf_read_unsigned_short(search_data);  search_data+=2;
                            search_data += 22*num_object;
                        }
                        k = sdf_read_unsigned_short(search_data);  search_data+=2;
                        search_data += 16;  // Skip name & other data...
                    }
                }


                // Look through the coordinates
                repeat(i, k)
                {
                    if(!room_select_inlist(i))
                    {
                        temp_xyz[X] = ((signed short)sdf_read_unsigned_short(search_data)) * ONE_OVER_256;
                        temp_xyz[Y] = ((signed short)sdf_read_unsigned_short(search_data+2)) * ONE_OVER_256;
                        temp_xyz[Z] = 0.0f;
                        if(m == 3)
                        {
                            temp_xyz[Z] = ((signed short)sdf_read_unsigned_short(search_data+4)) * ONE_OVER_256;
                        }
                        render_get_point_xyd(temp_xyz[X], temp_xyz[Y], temp_xyz[Z], &new_x, &new_y, &new_d);
                        new_x = new_x-mouse_x;
                        new_y = new_y-mouse_y;
                        new_x = (float) sqrt(new_x*new_x + new_y*new_y);
                        if(new_x < 5.0f && new_d < best_depth)
                        {
                            best_depth = new_d;
                            best_vertex = i;
                            best_data = search_data;
                        }
                    }
                    search_data = search_data+j;
                }
                if(best_vertex != 65535)
                {
                    room_select_add(best_vertex, best_data, (unsigned char) m);
                }
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_ALL)
            {
                if(workmode == ROOM_WORKMODE_VERTEX)
                {
                    repeat(i, num_vertex)
                    {
                        if(!room_select_inlist(i))
                        {
                            room_select_add(i, vertex_data+(i*6), 3);
                        }
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_INVERT)
            {
                if(workmode == ROOM_WORKMODE_VERTEX)
                {
                    repeat(i, num_vertex)
                    {
                        if(!room_select_inlist(i))
                        {
                            room_select_add(i, vertex_data+(i*6), 3);
                        }
                        else
                        {
                            // Items with a NULL data get removed later
                            room_select_data[room_select_index] = NULL;
                        }
                    }
                    repeat(i, room_select_num)
                    {
                        if(room_select_data[i] == NULL)
                        {
                            room_select_remove(i);
                            i--;
                        }
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_CONNECTED)
            {
                if(workmode == ROOM_WORKMODE_VERTEX)
                {
                    // Flag all of our original vertices
                    repeat(i, room_select_num)
                    {
                        room_select_flag[i] = TRUE;
                    }
                    // Go through each triangle, looking for vertices that are selected...
                    search_data = triangle_data;
                    repeat(j, MAX_ROOM_TEXTURE)
                    {
                        num_triangle = sdf_read_unsigned_short(search_data);  search_data+=2;
                        repeat(k, num_triangle)
                        {
                            found_connection = FALSE;
                            repeat(m, 3)
                            {
                                vertex = sdf_read_unsigned_short(search_data);  search_data+=4;
                                if(room_select_inlist(vertex))
                                {
                                    if(room_select_flag[room_select_index])
                                    {
                                        found_connection = TRUE;
                                    }
                                }
                            }
                            if(found_connection)
                            {
                                search_data-=12;
                                repeat(m, 3)
                                {
                                    vertex = sdf_read_unsigned_short(search_data);  search_data+=4;
                                    if(!room_select_inlist(vertex))
                                    {
                                        room_select_add(vertex, vertex_data+(vertex*6), 3);
                                    }
                                }
                            }
                        }
                    }
                }
                selection_close_type = 0;
            }




        }
        // Draw the room vertices...
        if(mode == ROOM_MODE_VERTEX)
        {
            display_texture_off();
            repeat(i, num_vertex)
            {
                room_draw_srf_vertex_helper(i);
                inlist = FALSE;
                if(room_select_inlist(i))
                {
                    if(room_select_data[room_select_index] == (vertex_data+(i*6)))
                    {
                        inlist = TRUE;
                    }
                }
                if(inlist)
                {
                    display_riser_marker(white, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
                else
                {
                    display_riser_marker(color_rgb, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
            }
            display_texture_on();
        }
        // Draw the room triangles...
        if(mode == ROOM_MODE_TRIANGLE)
        {
            display_texture_on();
            repeat(i, MAX_ROOM_TEXTURE)
            {
                if(i == 30)
                {
                    display_blend_trans();
                    color_temp[0] = 0;
                    color_temp[1] = 0;
                    color_temp[2] = 250;
                    color_temp[3] = (unsigned char) (80.0f + (((float) sin((main_video_frame & 63)*0.0490859375f)) * 64.0f));
                    display_color_alpha(color_temp);
                    display_zbuffer_write_off();
                }
                display_pick_texture(room_editor_texture[i]);
                num_triangle = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                repeat(j, num_triangle)
                {
                    display_start_fan();
                        repeat(k, 3)
                        {
                            vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                            tex_vertex = sdf_read_unsigned_short(triangle_data);  triangle_data+=2;
                            room_draw_srf_tex_vertex_helper(tex_vertex);
                            display_texpos(vertex_xyz);
                            room_draw_srf_vertex_helper(vertex);
                            display_vertex(vertex_xyz);
                        }
                    display_end();
                }
            }
            display_blend_off();
            display_color(color_rgb);
            display_zbuffer_write_on();
        }
        // Draw the room exterior wall chain...
        if(mode == ROOM_MODE_EXTERIOR_WALL && num_exterior_wall)
        {
            display_zbuffer_off();
            display_texture_off();
            j = 0;
            repeat(i, (num_exterior_wall+1))
            {
                k = j;  // Last vertex
                j = sdf_read_unsigned_short(exterior_wall_data + 3*(i%num_exterior_wall));  // Current vertex
                flags = *(exterior_wall_data + 3*(i%num_exterior_wall) + 2);  // Flags for current vertex

                if(i > 0)
                {
                    // Draw the wall line...
                    display_color(color_rgb);
                    display_start_line();
                        room_draw_srf_vertex_helper(k);
                        display_vertex(vertex_xyz);
                        room_draw_srf_vertex_helper(j);
                        display_vertex(vertex_xyz);
                    display_end();


                    // Check the low 4 bits of flags for the door type...
                    if((flags & 15) != ROOM_WALL_FLAG_LOW_NORMAL_WALL)
                    {
                        // We've got a door (or a slanted wall), so draw it as a color'd line perpendicular to wall line...
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_NO_TRIM_WALL)
                        {
                            display_color(bronze);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_NO_WALL)
                        {
                            display_color(black);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_DOOR)
                        {
                            display_color(white);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_SHUTTER)
                        {
                            display_color(green);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_PASSAGE)
                        {
                            display_color(magenta);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_BOSS_DOOR)
                        {
                            display_color(cyan);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_CRACKED_DOOR)
                        {
                            display_color(yellow);
                        }
                        if((flags & 15) == ROOM_WALL_FLAG_LOW_SECRET_DOOR)
                        {
                            display_color(light_green);
                        }



                        room_draw_srf_vertex_helper(k);
                        center_xyz[X] = vertex_xyz[X];
                        center_xyz[Y] = vertex_xyz[Y];
                        center_xyz[Z] = vertex_xyz[Z];
                        room_draw_srf_vertex_helper(j);
                        perp_xyz[X] = -(vertex_xyz[Y] - center_xyz[Y]);
                        perp_xyz[Y] = vertex_xyz[X] - center_xyz[X];
                        new_d = ((float) sqrt(perp_xyz[X]*perp_xyz[X] + perp_xyz[Y]*perp_xyz[Y])) + 0.0000001f;
                        perp_xyz[X]/=new_d;
                        perp_xyz[Y]/=new_d;
                        center_xyz[X] = (center_xyz[X] + vertex_xyz[X]) * 0.5f;
                        center_xyz[Y] = (center_xyz[Y] + vertex_xyz[Y]) * 0.5f;
                        center_xyz[Z] = (center_xyz[Z] + vertex_xyz[Z]) * 0.5f;
                        vertex_xyz[X] = center_xyz[X] + perp_xyz[X];
                        vertex_xyz[Y] = center_xyz[Y] + perp_xyz[Y];
                        vertex_xyz[Z] = center_xyz[Z];
                        center_xyz[X] -= perp_xyz[X];
                        center_xyz[Y] -= perp_xyz[Y];
                        display_start_line();
                            display_vertex(center_xyz);
                            display_vertex(vertex_xyz);
                        display_end();
                    }
                }
            }
            display_texture_on();
            display_zbuffer_on();
        }
        // Draw the room waypoints...
        if(mode == ROOM_MODE_WAYPOINT)
        {
            display_zbuffer_off();
            display_texture_off();
            repeat(i, num_waypoint)
            {
                room_draw_srf_waypoint_helper(i);
                inlist = FALSE;
                if(room_select_inlist(i))
                {
                    if(room_select_data[room_select_index] == (waypoint_data+(i*4)))
                    {
                        inlist = TRUE;
                    }
                }
                if(inlist)
                {
                    display_solid_marker(white, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
                else
                {
                    display_solid_marker(color_rgb, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
            }
            display_color(color_rgb)
            repeat(i, num_waypoint_line)
            {
                display_start_line();
                    j = *(waypoint_line_data+(i*2));
                    k = *(waypoint_line_data+(i*2)+1);
                    room_draw_srf_waypoint_helper(j);
                    display_vertex(vertex_xyz);
                    room_draw_srf_waypoint_helper(k);
                    display_vertex(vertex_xyz);
                display_end();
            }
            display_texture_on();
            display_zbuffer_on();
        }
        // Draw all of the room bridges...
        if(mode == ROOM_MODE_BRIDGE)
        {
            display_zbuffer_off();
            display_texture_off();
            repeat(i, num_bridge)
            {
                display_start_line();
                    room_draw_srf_bridge_helper(i, 0);
                    display_vertex(vertex_xyz);
                    room_draw_srf_bridge_helper(i, 1);
                    display_vertex(vertex_xyz);
                display_end();

                room_draw_srf_bridge_helper(i, 0);
                inlist = FALSE;
                if(room_select_inlist((unsigned short) (i*2)))
                {
                    if(room_select_data[room_select_index] == (bridge_data+(i*8)))
                    {
                        inlist = TRUE;
                    }
                }
                if(inlist)
                {
                    display_solid_marker(white, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
                else
                {
                    display_solid_marker(color_rgb, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
                room_draw_srf_bridge_helper(i, 1);
                inlist = FALSE;
                if(room_select_inlist((unsigned short) (i*2+1)))
                {
                    if(room_select_data[room_select_index] == (bridge_data+(i*8)+4))
                    {
                        inlist = TRUE;
                    }
                }
                if(inlist)
                {
                    display_solid_marker(white, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
                else
                {
                    display_solid_marker(color_rgb, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], 1.0f);
                }
            }
            display_texture_on();
            display_zbuffer_on();
        }
        if(mode == ROOM_MODE_OBJECT_GROUP)
        {
            // Clear out the mouse text...
            if(workmode == ROOM_WORKMODE_OBJECT)
            {
                mouse_text[0] = 0;
            }

            // Is our current group valid?
            if(global_room_active_group < num_group)
            {
                // Skip through data to get to the current group...
                repeat(i, global_room_active_group)
                {
                    num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
                    object_group_data += 22*num_object;
                }

                // Now draw all the objects in the current group...
                display_zbuffer_off();
                display_texture_off();
                num_object = sdf_read_unsigned_short(object_group_data);  object_group_data+=2;
                repeat(i, num_object)
                {
                    room_draw_srf_object_helper();
                    team = object_group_data[14];
                    if(team == TEAM_NEUTRAL) { color_temp[0] = 150;  color_temp[1] = 150;  color_temp[2] = 150; }
                    if(team == TEAM_MONSTER) { color_temp[0] = 255;  color_temp[1] = 255;  color_temp[2] =   0; }
                    if(team == TEAM_GOOD)    { color_temp[0] =   0;  color_temp[1] = 255;  color_temp[2] =   0; }
                    if(team == TEAM_EVIL)    { color_temp[0] = 255;  color_temp[1] =   0;  color_temp[2] =   0; }


                    // Draw .DDD's as a different color...
                    object_group_data[12] = 0;
                    length = strlen(object_group_data);
                    if(length > 4)
                    {
                        // Is it a .DDD file?
                        if(object_group_data[length-1] == 'D' && object_group_data[length-2] == 'D' && object_group_data[length-3] == 'D' && object_group_data[length-4] == '.')
                        {
                            { color_temp[0] = 255;  color_temp[1] = 255;  color_temp[2] = 255; }
                        }
                    }


                    size = 1.0f;
                    if(workmode == ROOM_WORKMODE_OBJECT)
                    {
                        if(room_select_inlist(i))
                        {
                            if(room_select_data[room_select_index] == (object_group_data+16))
                            {
                                color_temp[0] = 255;  color_temp[1] = 255;  color_temp[2] = 255;
                                memcpy(mouse_text, object_group_data, 13);
                                mouse_text[12] = 0;

                                global_room_active_object = i;
                                global_room_active_object_data = object_group_data;
                            }
                        }
                        if(i == global_room_active_object && object_group_data == global_room_active_object_data)
                        {
                            size = 2.0f;
                        }
                    }
                    display_solid_marker(color_temp, vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z], size);

                    // Riser line
                    display_start_line();
                        display_vertex(vertex_xyz);
                        temp_xyz[X] = vertex_xyz[X];
                        temp_xyz[Y] = vertex_xyz[Y];
                        temp_xyz[Z] = 0.0f;
                        display_vertex(temp_xyz);
                    display_end();


                    // Facing line
                    display_start_line();
                        display_vertex(vertex_xyz);
                        angle = ((unsigned short) (rotation + (object_group_data[15]<<8))) * (2.0f * PI / 65536.0f);
                        temp_xyz[X] = - (((float) sin(angle))*5.0f) + vertex_xyz[X];
                        temp_xyz[Y] = (((float) cos(angle))*5.0f) + vertex_xyz[Y];
                        temp_xyz[Z] = vertex_xyz[Z];
                        display_vertex(temp_xyz);
                    display_end();
                    object_group_data+=22;
                }
                display_texture_on();
                display_zbuffer_on();
            }
        }
        // Do all of the submodes...
        // Submode 1 for selection crosshairs...
        if(submode == 1)
        {
            display_zbuffer_off();
            display_texture_off();
            display_crosshairs(white, select_center_xyz[X], select_center_xyz[Y], 100.00f);
            display_riser_marker(white, select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z], 2.0f);
            display_texture_on();
            display_zbuffer_on();
        }
        // Submode 2 for selection & offset crosshairs
        if(submode == 2)
        {
            display_zbuffer_off();
            display_texture_off();

            display_crosshairs(red, select_center_xyz[X], select_center_xyz[Y], 100.00f);
            display_riser_marker(red, select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z], 2.0f);

            display_crosshairs(white, select_offset_xyz[X], select_offset_xyz[Y], 100.00f);
            display_riser_marker(white, select_offset_xyz[X], select_offset_xyz[Y], select_offset_xyz[Z], 2.0f);

            display_start_line();
                display_vertex(select_center_xyz);
                display_vertex(select_offset_xyz);
            display_end();

            display_texture_on();
            display_zbuffer_on();
        }
    #endif
}

//-----------------------------------------------------------------------------------------------
void room_draw(unsigned char* data)
{
    // <ZZ> This function draws an uncompressed room...
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned char* triangle_data;
    unsigned char* texture_data;
    unsigned char texture;
    unsigned int hardware_texture;
    unsigned char flags;
    unsigned short num_vertex, num_tex_vertex, num_strip;
    unsigned short i, j, vertex, tex_vertex;
    unsigned char* current_vertex_data;
    unsigned char* current_tex_vertex_data;
    unsigned char light;
    float vertex_xyz[3];
    float cartoon_line_size;


    // Read the header...
    vertex_data = data + (*((unsigned int*) (data+SRF_VERTEX_OFFSET)));
    num_vertex = *((unsigned short*) vertex_data);  vertex_data+=2;
    tex_vertex_data = data + (*((unsigned int*) (data+SRF_TEX_VERTEX_OFFSET)));
    num_tex_vertex = *((unsigned short*) tex_vertex_data);  tex_vertex_data+=2;
    triangle_data = data + (*((unsigned int*) (data+SRF_TRIANGLE_OFFSET)));
    texture_data = data + (*((unsigned int*) (data+SRF_TEXTURE_OFFSET)));


    // Setup some basic display stuff...
    display_blend_off();
    display_zbuffer_on();
    display_paperdoll_off();
    display_shade_on();
    display_texture_on();


    // Draw the triangle strips...  Last two textures are used for water...  Drawn later...
    // First pass is for non-transparent textures...
    repeat(texture, (MAX_ROOM_TEXTURE-2))
    {
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;
        if(num_strip > 0)
        {
            flags = (texture_data + (texture<<3))[4];
            if((flags & ROOM_TEXTURE_FLAG_NODRAW) || (flags & ROOM_TEXTURE_FLAG_ALPHATRANS))
            {
                // Skip over this texture...
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    triangle_data+=(num_vertex<<2);
                }
            }
            else
            {
                // Draw the triangle strips...
                hardware_texture = *((unsigned int*) (texture_data + (texture<<3)));
                display_pick_texture(hardware_texture);


                if(flags & ROOM_TEXTURE_FLAG_PAPERDOLL) { display_paperdoll_on(); }
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    display_start_strip();
                        repeat(j, num_vertex)
                        {
                            // Read the vertex and tex vertex
                            vertex = (*((unsigned short*) triangle_data));  triangle_data+=2;
                            tex_vertex = (*((unsigned short*) (triangle_data)));  triangle_data+=2;
                            current_vertex_data = (vertex_data+(vertex*26));
                            current_tex_vertex_data = (tex_vertex_data+(tex_vertex*22));

                            // Determine final light intensity...
                            light = (flags & ROOM_TEXTURE_FLAG_SMOOTH) ? (current_vertex_data[25]) : ((current_vertex_data[25] * current_tex_vertex_data[21]) >> 8);
                            light = (flags & ROOM_TEXTURE_FLAG_PAPERDOLL) ? ((light + 255)>>1) : (light);
                            color_temp[0] = light;
                            color_temp[1] = light;
                            color_temp[2] = light;
                            display_color(color_temp);


                            // Add to draw list...
                            display_texpos(((float*) current_tex_vertex_data));
                            display_vertex(((float*) current_vertex_data));
                        }
                    display_end();
                }
                if(flags & ROOM_TEXTURE_FLAG_PAPERDOLL) { display_paperdoll_off(); }
            }
        }
    }




    // Draw the triangle strips...  Last two textures are used for water...  Drawn later...
    // Second pass is for transparent textures...
    triangle_data = data + (*((unsigned int*) (data+SRF_TRIANGLE_OFFSET)));
    repeat(texture, (MAX_ROOM_TEXTURE-2))
    {
        num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;
        if(num_strip > 0)
        {
            flags = (texture_data + (texture<<3))[4];
            if((flags & ROOM_TEXTURE_FLAG_NODRAW) || !(flags & ROOM_TEXTURE_FLAG_ALPHATRANS))
            {
                // Skip over this texture...
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    triangle_data+=(num_vertex<<2);
                }
            }
            else
            {
                // Draw the triangle strips...
                hardware_texture = *((unsigned int*) (texture_data + (texture<<3)));
                display_pick_texture(hardware_texture);
                if(flags & ROOM_TEXTURE_FLAG_ALPHATRANS) { display_blend_trans(); display_zbuffer_write_off(); }
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    display_start_strip();
                        repeat(j, num_vertex)
                        {
                            // Read the vertex and tex vertex
                            vertex = (*((unsigned short*) triangle_data));  triangle_data+=2;
                            tex_vertex = (*((unsigned short*) (triangle_data)));  triangle_data+=2;
                            current_vertex_data = (vertex_data+(vertex*26));
                            current_tex_vertex_data = (tex_vertex_data+(tex_vertex*22));

                            // Determine final light intensity...
                            light = (flags & ROOM_TEXTURE_FLAG_SMOOTH) ? (current_vertex_data[25]) : ((current_vertex_data[25] * current_tex_vertex_data[21]) >> 8);
                            color_temp[0] = light;
                            color_temp[1] = light;
                            color_temp[2] = light;
                            display_color(color_temp);


                            // Add to draw list...
                            display_texpos(((float*) current_tex_vertex_data));
                            display_vertex(((float*) current_vertex_data));
                        }
                    display_end();
                }
                if(flags & ROOM_TEXTURE_FLAG_ALPHATRANS) { display_blend_off(); display_zbuffer_write_on(); }
            }
        }
    }




    // Check user flag for whether or not we should draw room outline...
    if(line_mode & 2)
    {
        // Now draw the backfaces...
        triangle_data = data + (*((unsigned int*) (data+SRF_TRIANGLE_OFFSET)));
        cartoon_line_size = (camera_distance+60.0f)*ROOM_LINE_SIZE;


        // Setup some basic display stuff...
        display_blend_off();
        display_shade_off();
        display_texture_off();
        display_cull_frontface();
        display_color(black);


        // Draw the triangle strips...
        repeat(texture, (MAX_ROOM_TEXTURE-2))
        {
            num_strip = *((unsigned short*) triangle_data);  triangle_data+=2;
            flags = (texture_data + (texture<<3))[4];
            if((flags & ROOM_TEXTURE_FLAG_NODRAW) || (flags & ROOM_TEXTURE_FLAG_NOLINE))
            {
                // Skip over the line for this texture...
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    triangle_data+=(num_vertex<<2);
                }
            }
            else
            {
                // Draw the line for this texture...  Backfaces...
                repeat(i, num_strip)
                {
                    num_vertex = *((unsigned short*) triangle_data);  triangle_data+=2;
                    display_start_strip();
                        repeat(j, num_vertex)
                        {
                            // Read the vertex
                            vertex = (*((unsigned short*) triangle_data));  triangle_data+=4;
                            current_vertex_data = (vertex_data+(vertex*26));
                            vertex_xyz[X] = (*((float*) current_vertex_data)) + (*((float*) (current_vertex_data+12)))*cartoon_line_size;
                            vertex_xyz[Y] = (*((float*) (current_vertex_data+4))) + (*((float*) (current_vertex_data+16)))*cartoon_line_size;
                            vertex_xyz[Z] = (*((float*) (current_vertex_data+8))) + (*((float*) (current_vertex_data+20)))*cartoon_line_size;


                            // Add to draw list...
                            display_vertex(vertex_xyz);
                        }
                    display_end();
                }
            }
        }
        display_cull_on();
    }
    display_shade_off();
}

//-----------------------------------------------------------------------------------------------
unsigned char room_findpath(unsigned char* data, float* from_xy, float* to_xy, float* write_xy, unsigned char* character_data)
{
    // <ZZ> This function searches the waypoints of an uncompress'd room (data) for the
    //      best path from the from_xy location to the to_xy location...  The next
    //      waypoint's location (or to_xy if that's better) is written into write_xy...
    //      Returns TRUE if a path was found, FALSE if not...
    unsigned char from_waypoint;
    unsigned char to_waypoint;
    unsigned char i;
    float best_distance;
    float distance, disx, disy;
    float* waypoint_xy_start;
    float* waypoint_xy;
    unsigned char* waypoint_data;
    unsigned char num_waypoint;
    unsigned char return_code;


    return_code = TRUE;
    waypoint_data = data + (*((unsigned int*) (data+SRF_WAYPOINT_OFFSET)));
    num_waypoint = *waypoint_data;  waypoint_data+=2;
    if(num_waypoint > 0)
    {
        waypoint_xy_start = (float*) waypoint_data;
        waypoint_data+=num_waypoint<<3;


        // Find our to_waypoint...
        best_distance = 9999999.0f;
        to_waypoint = 0;
        waypoint_xy = waypoint_xy_start;
        repeat(i, num_waypoint)
        {
            disx = waypoint_xy[X] - to_xy[X];
            disy = waypoint_xy[Y] - to_xy[Y];
            distance = disx*disx + disy*disy;
            if(distance < best_distance)
            {
                best_distance = distance;
                to_waypoint = i;
            }
            waypoint_xy+=2;
        }


        // Is our from spot closer to the target than our final waypoint?
        disx = to_xy[X] - from_xy[X];
        disy = to_xy[Y] - from_xy[Y];
        distance = disx*disx + disy*disy;
        if(distance < best_distance)
        {
            // Looks better just to go straight to our target...
            write_xy[X] = to_xy[X];
            write_xy[Y] = to_xy[Y];
        }
        else
        {
            // Find our from_waypoint...
            best_distance = 9999999.0f;
            from_waypoint = 0;
            waypoint_xy = waypoint_xy_start;
            repeat(i, num_waypoint)
            {
                disx = waypoint_xy[X] - from_xy[X];
                disy = waypoint_xy[Y] - from_xy[Y];
                distance = disx*disx + disy*disy;
                if(distance < best_distance)
                {
                    best_distance = distance;
                    from_waypoint = i;
                }
                waypoint_xy+=2;
            }


            // Check old character data...
            if(from_waypoint == character_data[51] && best_distance > 25.0f) // nextway
            {
                // Hasn't gotten close enough to next waypoint yet, so retain last waypoint as our starting point...
                from_waypoint = character_data[50]; // lastway
            }



            if(from_waypoint == to_waypoint)
            {
                // Looks like we're really close...  Just go 'fer the target...
                write_xy[X] = to_xy[X];
                write_xy[Y] = to_xy[Y];
            }
            else
            {
                // Use the look up table we computed (when the SRF file was created) to find the next waypoint,
                // given the from and to waypoint...
                waypoint_data += ((from_waypoint*num_waypoint) + to_waypoint);
                to_waypoint = *waypoint_data;  // Our new waypoint...
                if(from_waypoint == to_waypoint)
                {
                    // Hmm...  That shouldn't happen, should it?
                    // Means that we're trying to get to a waypoint on a seperate chain...
                    return_code = FALSE;
                }
                waypoint_xy = waypoint_xy_start + (to_waypoint<<1);
                write_xy[X] = waypoint_xy[X];
                write_xy[Y] = waypoint_xy[Y];


                // Record new character data...
                character_data[50] = (unsigned char) from_waypoint;
                character_data[51] = (unsigned char) to_waypoint;
            }
        }
    }
    else
    {
        // No waypoint data...  We'll just have to give 'em the to_xy...
        write_xy[X] = to_xy[X];
        write_xy[Y] = to_xy[Y];
    }
    return return_code;
}

//-----------------------------------------------------------------------------------------------
/*
void room_shadow_test(float x, float y, float z, float radius)
{
    // <ZZ> Volumetric shadow (without stencil buffer) test...
    unsigned short i;
    float angle, angleadd;
    float vertex_xyz[3];

    // Setup some basic display stuff...
    display_paperdoll_off();
    display_shade_off();
    display_texture_off();
    display_zbuffer_write_off();

    // Radial stuff...
    angleadd = -PI/16.0f;

    // Front faces should halve the existing color...
    if(!key_down[SDLK_F5])
    {
        display_blend_trans();
        color_temp[0] = 0;    color_temp[1] = 0;    color_temp[2] = 0;    color_temp[3] = 128;
        display_color_alpha(color_temp);
        display_cull_on();
        display_start_fan();
        display_vertex_xyz(x, y, z);
        angle = 0.0f;
        repeat(i, 33)
        {
            vertex_xyz[X] = x+(((float) sin(angle))*radius);
            vertex_xyz[Y] = y+(((float) cos(angle))*radius);
            vertex_xyz[Z] = 0.0f;
            display_vertex(vertex_xyz);
            angle+=angleadd;
        }
        display_end();
    }


    // Back faces should double the existing color...
    if(!key_down[SDLK_F6])
    {
        display_blend_double();
        color_temp[0] = 255;    color_temp[1] = 255;    color_temp[2] = 255;    color_temp[3] = 128;
        display_color_alpha(color_temp);
        display_cull_frontface();
        display_start_fan();
        display_vertex_xyz(x, y, z);
        angle = 0.0f;
        repeat(i, 33)
        {
            vertex_xyz[X] = x+(((float) sin(angle))*radius);
            vertex_xyz[Y] = y+(((float) cos(angle))*radius);
            vertex_xyz[Z] = 0.0f;
            display_vertex(vertex_xyz);
            angle+=angleadd;
        }
        display_end();
    }


    display_texture_on();
    display_cull_on();
    display_zbuffer_write_on();
}

//-----------------------------------------------------------------------------------------------
void room_light_test(float x, float y, float z, float radius)
{
    // <ZZ> Volumetric light (without stencil buffer) test...
    unsigned short i;
    float angle, angleadd;
    float vertex_xyz[3];

    // Setup some basic display stuff...
    display_paperdoll_off();
    display_shade_off();
    display_texture_off();
    display_zbuffer_write_off();

    // Radial stuff...
    angleadd = -PI/16.0f;

    // Back faces should halve the existing color...
    if(!key_down[SDLK_F5])
    {
        display_blend_trans();
        color_temp[0] = 0;    color_temp[1] = 0;    color_temp[2] = 0;    color_temp[3] = 128;
        display_color_alpha(color_temp);
        display_cull_frontface();
        display_start_fan();
        display_vertex_xyz(x, y, z);
        angle = 0.0f;
        repeat(i, 33)
        {
            vertex_xyz[X] = x+(((float) sin(angle))*radius);
            vertex_xyz[Y] = y+(((float) cos(angle))*radius);
            vertex_xyz[Z] = 0.0f;
            display_vertex(vertex_xyz);
            angle+=angleadd;
        }
        display_end();
    }


    // Front faces should double the existing color...
    if(!key_down[SDLK_F6])
    {
        display_blend_double();
        color_temp[0] = 255;    color_temp[1] = 255;    color_temp[2] = 255;    color_temp[3] = 128;
        display_color_alpha(color_temp);
        display_cull_on();
        display_start_fan();
        display_vertex_xyz(x, y, z);
        angle = 0.0f;
        repeat(i, 33)
        {
            vertex_xyz[X] = x+(((float) sin(angle))*radius);
            vertex_xyz[Y] = y+(((float) cos(angle))*radius);
            vertex_xyz[Z] = 0.0f;
            display_vertex(vertex_xyz);
            angle+=angleadd;
        }
        display_end();
    }


    display_texture_on();
    display_zbuffer_write_on();
}
*/

//-----------------------------------------------------------------------------------------------
