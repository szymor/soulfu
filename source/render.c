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

// <ZZ> This file has all the stuff for rendering objects...
//      setup_shadow            - Reads shadow names from SHADOW.TXT
//      render_rdy              - Renders a model

#ifdef DEVTOOL
unsigned char global_billboard_active = TRUE;
#endif


// For modeler...
#define MOVE_MODE_MOVE      0
#define MOVE_MODE_SCALE     1
#define MOVE_MODE_ROTATE    2

// For room editor...
#define MOVE_MODE_BRIDGE_PLOP 3


// For modeler/border tools
#define BORDER_SIMPLE       0
#define BORDER_DRAG         1
#define BORDER_SELECT       2
#define BORDER_POINT_PICK   3
#define BORDER_CROSS_HAIRS  4
#define BORDER_MOVE         5
#define BORDER_SPECIAL_SELECT_ALL  20
#define BORDER_SPECIAL_SELECT_INVERT  21
#define BORDER_SPECIAL_SELECT_CONNECTED  22


#define MAX_SHADOW_TEXTURE 256
unsigned int shadow_texture[MAX_SHADOW_TEXTURE];
#define MAX_JOINT 256
#define MAX_BONE 256
#define MAX_VERTEX 16384
#define MAX_TEX_VERTEX 16384
#define MAX_TRIANGLE 16384

#ifdef DEVTOOL
    unsigned char do_anchor_swap = FALSE;
#endif


//-----------------------------------------------------------------------------------------------
//void render_set_light(unsigned char* light_data)
//{
//    // <ZZ> This function sets up the light position and color for a character.  Must be called
//    //      before rendering the character...  light_data is a pointer to 3 bytes of color info,
//    //      1 byte of padding, and 3 floats of light position...
//    global_render_light_color_rgb[0] = light_data[0];
//    global_render_light_color_rgb[1] = light_data[1];
//    global_render_light_color_rgb[2] = light_data[2];
//
//
//    // Figure out the cartoon texture offset, depending on light position...
//    light_data+=4;
//    global_render_light_offset_xy[X] = (camera_rotation_xy[X]/65535.0f) + ((float) atan2(((float*) (light_data))[Y], ((float*) (light_data))[X])/(2.0f*PI)) + 0.25f;
//    global_render_light_offset_xy[Y] = 0.38f;
//}

//-----------------------------------------------------------------------------------------------
unsigned char* render_generate_model_world_data(unsigned char* data, unsigned short frame, float* matrix, unsigned char* write)
{
    // <ZZ> This function generates new bone frame data for a given model (skipping first 11 values).
    //      This info is translated and rotated to a world coordinate system, which can be fed into
    //      render_bone_frame() to calculate vertex positions.  Write tells us where to put the new
    //      stuff.  Matrix is a 12 value array like this...
    //          [RightX][RightY][RightZ]  (entries 0 to 2)
    //          [FrontX][FrontY][FrontZ]  (entries 3 to 5)
    //          [ Up X ][ Up Y ][ Up Z ]  (entries 6 to 8)
    //          [OffstX][OffstY][OffstZ]  (entries 9 to 11)
    //      Function returns next place to write data...
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    float* joint_position_data;
    unsigned char base_model;
    unsigned short i, j;
    unsigned char num_base_model;
    unsigned short num_bone_frame;

//log_message("INFO:   render_generate_blah_blah_blah");


    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data);  data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    if(frame >= num_bone_frame) frame = 0;
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data+2);  frame_data+=11;
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);


    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    joint_position_data = (float*) frame_data;


    // Calculate new bone normals...  Just rotate, no translation...
//log_message("INFO:   Calculating bone normals");
    num_bone = num_bone<<1;
    repeat(i, num_bone)
    {
        *((float*) write) = (joint_position_data[X]*matrix[0]) + (joint_position_data[Y]*matrix[3]) + (joint_position_data[Z]*matrix[6]);  write+=4;
        *((float*) write) = (joint_position_data[X]*matrix[1]) + (joint_position_data[Y]*matrix[4]) + (joint_position_data[Z]*matrix[7]);  write+=4;
        *((float*) write) = (joint_position_data[X]*matrix[2]) + (joint_position_data[Y]*matrix[5]) + (joint_position_data[Z]*matrix[8]);  write+=4;
//if((i & 1) == 0)
//{
//  log_message("INFO:     Front %d == (%f, %f, %f)", i>>1, *((float*)(write-12)), *((float*)(write-8)), *((float*)(write-4)));
//}
//else
//{
//  log_message("INFO:     Side %d == (%f, %f, %f)", i>>1, *((float*)(write-12)), *((float*)(write-8)), *((float*)(write-4)));
//}
        joint_position_data+=3;
    }
//log_message("INFO:   Calculated bone normals");


    // Calculate new joint positions...  Translation and rotation...
//log_message("INFO:   Calculating joint positions");
    repeat(i, num_joint)
    {
        *((float*) write) = (joint_position_data[X]*matrix[0]) + (joint_position_data[Y]*matrix[3]) + (joint_position_data[Z]*matrix[6]) + matrix[9];  write+=4;
        *((float*) write) = (joint_position_data[X]*matrix[1]) + (joint_position_data[Y]*matrix[4]) + (joint_position_data[Z]*matrix[7]) + matrix[10];  write+=4;
        *((float*) write) = (joint_position_data[X]*matrix[2]) + (joint_position_data[Y]*matrix[5]) + (joint_position_data[Z]*matrix[8]) + matrix[11];  write+=4;
//  log_message("INFO:     Joint %d == (%f, %f, %f)", i, *((float*)(write-12)), *((float*)(write-8)), *((float*)(write-4)));
        joint_position_data+=3;
    }
//log_message("INFO:   Calculated joint positions");


    // Calculate shadow information...
//log_message("INFO:   Calculating shadow corners");
    frame_data = (unsigned char*) joint_position_data;
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        base_model = *frame_data;  // really shadow alpha, not base_model
        *write = base_model;  write++;  frame_data++;
        if(base_model)
        {
            // Rotate and translate each shadow coordinate...
            repeat(j, 4)
            {
                *((float*) write) = ((((float*) frame_data)[X])*matrix[0]) + ((((float*) frame_data)[Y])*matrix[3]) + matrix[9];  write+=4;
                *((float*) write) = ((((float*) frame_data)[X])*matrix[1]) + ((((float*) frame_data)[Y])*matrix[4]) + matrix[10];  write+=4;
                frame_data+=8;
            }
        }
    }
//log_message("INFO:   Calculated shadow corners");


    // We should now be able to feed render_bone_frame() our old write value...
    return write;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char triangle_lines = FALSE;
void render_fix_model_to_bone_length(unsigned char* data, unsigned short frame, unsigned short exempt_joint)
{
    // <ZZ> This function moves all of the joints, except exempt_joint, to make the bone lengths
    //      match the initial bone lengths.  My shoddy little inverse kinematics routine.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* bone_data;
    float* joint_size_data;
    float start_xyz[3];
    float end_xyz[3];
    float length;
    unsigned char base_model, detail_level;
    unsigned short joint[2];
    float joint_weight[2];
    unsigned short i, j;
    float joint_movement_xyz[MAX_JOINT][3];
    float desired_length;
    float total_weight;



    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    detail_level = 0;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=12;
    joint_size_data = *((float**) data);  data+=4;
    bone_data = *((unsigned char**) data);


    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(num_joint >= MAX_JOINT) return;


    // Figure out bone movement...  Done interatively...
    repeat(i, 20)
    {
        // Clear joint movement vectors
        repeat(j, num_joint)
        {
            joint_movement_xyz[j][X] = 0;
            joint_movement_xyz[j][Y] = 0;
            joint_movement_xyz[j][Z] = 0;
        }



        // Fill in joint movement vectors
        repeat(j, num_bone)
        {
            // Figure out bone/joint coordinates
            joint[0] = *((unsigned short*) (bone_data+1+(j*9)));
            joint[1] = *((unsigned short*) (bone_data+3+(j*9)));
            desired_length = *((float*) (bone_data+5+(j*9)));


            start_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[0]*12)));
            start_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[0]*12)));
            start_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[0]*12)));


            end_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[1]*12)));
            end_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[1]*12)));
            end_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[1]*12)));


            // Vector from bone start to bone end to find length...
            end_xyz[X]-=start_xyz[X];
            end_xyz[Y]-=start_xyz[Y];
            end_xyz[Z]-=start_xyz[Z];
            length = vector_length(end_xyz);
            if(length < 0.001f) length = 0.001f;

            // Figure out the movement...
            length = (length-desired_length)/length;
            end_xyz[X]*=length;
            end_xyz[Y]*=length;
            end_xyz[Z]*=length;
            if(exempt_joint == joint[0])
            {
                // Only joint[1] should move
                joint_movement_xyz[joint[1]][X] = -end_xyz[X];
                joint_movement_xyz[joint[1]][Y] = -end_xyz[Y];
                joint_movement_xyz[joint[1]][Z] = -end_xyz[Z];
            }
            else if(exempt_joint == joint[1])
            {
                // Only joint[0] should move
                joint_movement_xyz[joint[0]][X] = end_xyz[X];
                joint_movement_xyz[joint[0]][Y] = end_xyz[Y];
                joint_movement_xyz[joint[0]][Z] = end_xyz[Z];
            }
            else
            {
                // Split movement according to weight
                joint_weight[0] = joint_size_data[joint[0]];
                joint_weight[1] = joint_size_data[joint[1]];
                if(joint_weight[0] < JOINT_COLLISION_SCALE) joint_weight[0] = JOINT_COLLISION_SCALE;
                if(joint_weight[1] < JOINT_COLLISION_SCALE) joint_weight[1] = JOINT_COLLISION_SCALE;
                total_weight = joint_weight[0] + joint_weight[1];

                joint_movement_xyz[joint[0]][X] += end_xyz[X]*joint_weight[1]/total_weight;
                joint_movement_xyz[joint[0]][Y] += end_xyz[Y]*joint_weight[1]/total_weight;
                joint_movement_xyz[joint[0]][Z] += end_xyz[Z]*joint_weight[1]/total_weight;

                joint_movement_xyz[joint[1]][X] -= end_xyz[X]*joint_weight[0]/total_weight;
                joint_movement_xyz[joint[1]][Y] -= end_xyz[Y]*joint_weight[0]/total_weight;
                joint_movement_xyz[joint[1]][Z] -= end_xyz[Z]*joint_weight[0]/total_weight;
            }
        }



        // Apply the movement for each joint...
        repeat(j, num_joint)
        {
            *((float*) (frame_data + 11 + (num_bone*24) + (j*12))) += joint_movement_xyz[j][X]*0.50f;
            *((float*) (frame_data + 11 + (num_bone*24) + 4 + (j*12))) += joint_movement_xyz[j][Y]*0.50f;
            *((float*) (frame_data + 11 + (num_bone*24) + 8 + (j*12))) += joint_movement_xyz[j][Z]*0.50f;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
void render_crunch_bone(unsigned char* data, unsigned short frame, unsigned short bone, unsigned char detail_level)
{
    // <ZZ> This function generates the length of a given bone.  Bone length...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* bone_data;
    unsigned char bone_name;
    float start_xyz[3];
    float end_xyz[3];
    float length;
    unsigned char base_model;
    unsigned short joint[2];


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    if(detail_level >= num_detail_level) return;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);


    base_model_data+=6;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(bone >= num_bone) return;



    // Figure out bone/joint coordinates
    bone_name = *(bone_data + (bone*9));
    if(bone_name == 0)
    {
        joint[0] = *((unsigned short*) (bone_data+1+(bone*9)));
        joint[1] = *((unsigned short*) (bone_data+3+(bone*9)));


        start_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[0]*12)));
        start_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[0]*12)));
        start_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[0]*12)));


        end_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[1]*12)));
        end_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[1]*12)));
        end_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[1]*12)));


        // Vector from bone start to bone end to find length...
        end_xyz[X]-=start_xyz[X];
        end_xyz[Y]-=start_xyz[Y];
        end_xyz[Z]-=start_xyz[Z];
        length = vector_length(end_xyz);

//        log_message("INFO:   Bone %d, from joint %d to %d, is %f long", bone, joint[0], joint[1], length);
    }
    else
    {
        length = 1.0f;
//        log_message("INFO:   Bone %d, is a special grip bone...  Length is 1.0", bone);
    }


    // Write length to RDY data
    *((float*) (bone_data+5+(bone*9))) = length;
}

//-----------------------------------------------------------------------------------------------
void render_crunch_vertex(unsigned char* data, unsigned short frame, unsigned short vertex, unsigned char recalc_weight, unsigned char detail_level)
{
    // <ZZ> This function generates the bone-offset coordinates for a given vertex.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex, num_bone;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* vertex_data;
    unsigned char* bone_data;
    unsigned short i;
    float start_xyz[3];
    float end_xyz[3];
    float point_xyz[3];
    float height_xyz[3];
    float front_xyz[3];
    float side_xyz[3];
    float length;
    float height;
    float front;
    float side;
    unsigned char base_model;
    unsigned char bone_binding[2];
    unsigned short joint[2];
    float weight[2];


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    if(detail_level >= num_detail_level) return;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);


    num_vertex = *((unsigned short*) base_model_data); base_model_data+=6;
    if(vertex >= num_vertex) return;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;


    // Skip ahead to the current vertex's data
    vertex_data = base_model_data + (vertex<<6);
    point_xyz[X] = *((float*) (vertex_data));
    point_xyz[Y] = *((float*) (vertex_data+4));
    point_xyz[Z] = *((float*) (vertex_data+8));
    bone_binding[0] = *(vertex_data+12);
    bone_binding[1] = *(vertex_data+13);


//log_message("INFO:   Point %d, Bone %d and %d", vertex, bone_binding[0], bone_binding[1]);
//log_message("INFO:     Actual = (%f, %f, %f)", point_xyz[X], point_xyz[Y], point_xyz[Z]);

    // Crunch for each bone
    repeat(i, 2)
    {
        // Figure out bone/joint coordinates
        joint[0] = *((unsigned short*) (bone_data+1+(bone_binding[i]*9)));
        joint[1] = *((unsigned short*) (bone_data+3+(bone_binding[i]*9)));
//log_message("INFO:     Joint %d to %d", joint[0], joint[1]);
        start_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[0]*12)));
        start_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[0]*12)));
        start_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[0]*12)));

//log_message("INFO:       StartPos = (%f, %f, %f)", start_xyz[X], start_xyz[Y], start_xyz[Z]);


        end_xyz[X] = *((float*) (frame_data + 11 + (num_bone*24) + (joint[1]*12)));
        end_xyz[Y] = *((float*) (frame_data + 11 + 4 + (num_bone*24) + (joint[1]*12)));
        end_xyz[Z] = *((float*) (frame_data + 11 + 8 + (num_bone*24) + (joint[1]*12)));

//log_message("INFO:       EndPos = (%f, %f, %f)", end_xyz[X], end_xyz[Y], end_xyz[Z]);


        front_xyz[X] = *((float*) (frame_data + 11 + (bone_binding[i]*24)));
        front_xyz[Y] = *((float*) (frame_data + 11 + 4 + (bone_binding[i]*24)));
        front_xyz[Z] = *((float*) (frame_data + 11 + 8 + (bone_binding[i]*24)));

        side_xyz[X] = *((float*) (frame_data + 11 + 12 + (bone_binding[i]*24)));
        side_xyz[Y] = *((float*) (frame_data + 11 + 16 + (bone_binding[i]*24)));
        side_xyz[Z] = *((float*) (frame_data + 11 + 20 + (bone_binding[i]*24)));


        // Vector from bone start to bone end
        end_xyz[X]-=start_xyz[X];
        end_xyz[Y]-=start_xyz[Y];
        end_xyz[Z]-=start_xyz[Z];

//log_message("INFO:       AlongBone = (%f, %f, %f)", end_xyz[X], end_xyz[Y], end_xyz[Z]);

        // Vector from bone start to point
        height_xyz[X] = point_xyz[X] - start_xyz[X];
        height_xyz[Y] = point_xyz[Y] - start_xyz[Y];
        height_xyz[Z] = point_xyz[Z] - start_xyz[Z];

//log_message("INFO:       BoneStartToPoint = (%f, %f, %f)", height_xyz[X], height_xyz[Y], height_xyz[Z]);


        // Calculate height value
        length = dot_product(end_xyz, end_xyz);
        height = dot_product(end_xyz, height_xyz);
//log_message("INFO:       Length = %f, Height = %f", length, height);
        if(length > 0) height = height/length;
        else height = 0;
//log_message("INFO:       Height = %f", height);


        // Calculate the weight for this bone
        if(recalc_weight)
        {
            if(height < 1.5f && height > -0.5f)
            {
                weight[i] = 0.5f-height;
                ABS(weight[i]);
                weight[i] = 1.0f - weight[i];
            }
            else
            {
                weight[i] = 0;
            }
        }


        // Calculate the height position
        height_xyz[X] = start_xyz[X] + (end_xyz[X]*height);
        height_xyz[Y] = start_xyz[Y] + (end_xyz[Y]*height);
        height_xyz[Z] = start_xyz[Z] + (end_xyz[Z]*height);
//log_message("INFO:       HeightPos = (%f, %f, %f)", height_xyz[X], height_xyz[Y], height_xyz[Z]);



        // Calculate vector from height position (along bone) to point
        height_xyz[X] = point_xyz[X] - height_xyz[X];
        height_xyz[Y] = point_xyz[Y] - height_xyz[Y];
        height_xyz[Z] = point_xyz[Z] - height_xyz[Z];
//log_message("INFO:       HeightPosToPoint = (%f, %f, %f)", height_xyz[X], height_xyz[Y], height_xyz[Z]);



        // Calculate front and side values
        front = dot_product(front_xyz, height_xyz);
        side = dot_product(side_xyz, height_xyz);
//log_message("INFO:       Front = (%f, %f, %f)", front_xyz[X], front_xyz[Y], front_xyz[Z]);
//log_message("INFO:       Front = %f", front);
//log_message("INFO:       Side = (%f, %f, %f)", side_xyz[X], side_xyz[Y], side_xyz[Z]);
//log_message("INFO:       Side = %f", side);

        // Write the values for the current vertex/bone
        *((float*) (vertex_data+27+(i<<2))) = height;
        *((float*) (vertex_data+35+(i<<2))) = front;
        *((float*) (vertex_data+43+(i<<2))) = side;

//end_xyz[X] = start_xyz[X] + (height*end_xyz[X]) + (front*front_xyz[X]) + (side*side_xyz[X]);
//end_xyz[Y] = start_xyz[Y] + (height*end_xyz[Y]) + (front*front_xyz[Y]) + (side*side_xyz[Y]);
//end_xyz[Z] = start_xyz[Z] + (height*end_xyz[Z]) + (front*front_xyz[Z]) + (side*side_xyz[Z]);
//log_message("INFO:       Generd = (%f, %f, %f)", end_xyz[X], end_xyz[Y], end_xyz[Z]);
    }
    if(recalc_weight)
    {
        if(weight[0] > 0.98f || weight[1] < 0.02f)
        {
            // Single bone
//            *(vertex_data+14) = (unsigned char) 255;
            *(vertex_data+14) = ((*(vertex_data+14)) & 128) | 127;  // Force full weight, but save anchor bit...  !!!ANCHOR!!!
        }
        else if(weight[1] > 0.98f || weight[0] < 0.02f)
        {
            // Single bone...  Copy second into first...
//            *(vertex_data+14) = (unsigned char) 255;
            *(vertex_data+14) = ((*(vertex_data+14)) & 128) | 127;  // Force full weight, but save anchor bit...  !!!ANCHOR!!!
            *(vertex_data+12) = *(vertex_data+13);
            *((float*) (vertex_data+27)) = height;
            *((float*) (vertex_data+35)) = front;
            *((float*) (vertex_data+43)) = side;
        }
        else
        {
//            weight[0] = 255.0f * 0.5f *(weight[0] + (1.0f - weight[1]));
//            *(vertex_data+14) = (unsigned char) weight[0];


            weight[0] = 127.0f * 0.5f *(weight[0] + (1.0f - weight[1]));
            *(vertex_data+14) = ((*(vertex_data+14)) & 128) | (((unsigned char) weight[0])&127);  // Save anchor bit...  !!!ANCHOR!!!
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define MAX_BASE_TO_CRUNCH 256

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
void render_attach_vertex_to_bone(unsigned char* data, unsigned short frame, unsigned short vertex);
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



void render_crunch_rdy(unsigned char* data)
{
    // <ZZ> This function crunches all of the vertices and bones in an RDY file, which needs to be done
    //      before the bone animation system can be put into effect.
    unsigned short frame;
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char* frame_data;
    unsigned short i;
    unsigned char base_model, detail_level;
    unsigned char base_crunched[MAX_BASE_TO_CRUNCH];


    // Keep track of which we've crunched
    repeat(i, MAX_BASE_TO_CRUNCH)
    {
        base_crunched[i] = FALSE;
    }


    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

    repeat(frame, num_bone_frame)
    {
        // Go to the current base model
        frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
        base_model = *(frame_data + 2);
        if(base_crunched[base_model] == FALSE)
        {
            // The bone structure is held in the first frame in which a base model is used
            base_crunched[base_model] = TRUE;
            repeat(detail_level, num_detail_level)
            {
                base_model_data = data + (base_model*20) + (num_base_model*20*detail_level);
                base_model_data = *((unsigned char**) base_model_data);
                num_vertex = *((unsigned short*) base_model_data);  base_model_data+=6;
                num_bone = *((unsigned short*) base_model_data);


                // Do the crunching
                repeat(i, num_vertex)
                {
                    render_crunch_vertex(start_data, frame, i, FALSE, detail_level);
                }
                repeat(i, num_bone)
                {
                    render_crunch_bone(start_data, frame, i, detail_level);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void render_crunch_all(unsigned char mask)
{
    // <ZZ> This function crunches all of the vertex data for all of the RDY files...
    //      Mask lets me say if I don't really want all of the files...  Seems to do
    //      bad things if crunched multiple times (?)...
    int i;
    unsigned char filename[9];
    unsigned char filetype;
    unsigned char* file_start;
    unsigned char* index;


    // Go through each file in the index
    log_message("INFO:   Crunching RDY files...");
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        // Check the type of file to decompress, then hand off to a subroutine...
        if(*(index+4) & mask)
        {
            sdf_get_filename(i, filename, &filetype);
            if(filetype == SDF_FILE_IS_RDY)
            {
                file_start = (unsigned char*) sdf_read_unsigned_int(index);
                render_crunch_rdy(file_start);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
#define MAX_SELECT 8192
unsigned short select_num = 0;
unsigned short select_list[MAX_SELECT];
unsigned short select_index = 0;
float select_xyz[MAX_SELECT][3];
float* select_data[MAX_SELECT];
unsigned short select_flag[MAX_SELECT];
#define select_clear() { select_num = 0; }
void select_add(unsigned short item, float* item_xyz)
{
    // <ZZ> Adds an item to the selection list...
    if(select_num < MAX_SELECT)
    {
        select_list[select_num] = item;
        select_xyz[select_num][X] = item_xyz[X];
        select_xyz[select_num][Y] = item_xyz[Y];
        select_xyz[select_num][Z] = item_xyz[Z];
        select_data[select_num] = item_xyz;
        select_num++;
    }
}
unsigned char select_inlist(unsigned short item)
{
    // <ZZ> Returns TRUE if the given item has already been added to the selection list.
    //      Also sets select_index to the index of that item.
    unsigned short i;

    repeat(i, select_num)
    {
        if(select_list[i] == item)
        {
            select_index = i;
            return TRUE;
        }
    }
    select_index = 0;
    return FALSE;
}
void select_remove(unsigned short item)
{
    // <ZZ> Removes an item from the selection list
    if(item < select_num)
    {
        while(item < (select_num-1))
        {
            select_list[item] = select_list[item+1];
            select_xyz[item][X] = select_xyz[item+1][X];
            select_xyz[item][Y] = select_xyz[item+1][Y];
            select_xyz[item][Z] = select_xyz[item+1][Z];
            select_data[item] = select_data[item+1];
            item++;
        }
        select_num--;
    }
}
void select_update_xyz(void)
{
    // <ZZ> Reloads xyz values from actual data
    unsigned short i;
    repeat(i, select_num)
    {
        select_xyz[i][X] = select_data[i][X];
        select_xyz[i][Y] = select_data[i][Y];
        select_xyz[i][Z] = select_data[i][Z];
    }
}
void select_update_xy(void)
{
    // <ZZ> Reloads xy values from actual data
    unsigned short i;
    repeat(i, select_num)
    {
        select_xyz[i][X] = select_data[i][X];
        select_xyz[i][Y] = select_data[i][Y];
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void hide_vertices(unsigned char* data, unsigned short frame, unsigned char do_hide)
{
    // <ZZ> This function flags the selected vertices as being hidden, or unhides all vertices,
    //      depending on the value of do_hide...  Joints and other data may not be selected, only
    //      vertices...
    unsigned short num_vertex;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char base_model;
    unsigned short i;
    unsigned char num_base_model;


    if(do_hide)
    {
        // Hide 'em...
        repeat(i, select_num)
        {
            *(((unsigned char*) select_data[i]) + 63) = TRUE;
        }
        select_clear();
    }
    else
    {
        // Unhide 'em
        data+=3;
        num_base_model = *data;  data+=3;
        data+=(ACTION_MAX<<1);
        data+=(MAX_DDD_SHADOW_TEXTURE);

        // Go to the current base model
        frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
        base_model = *(frame_data+2);  frame_data+=11;
        data = data + (base_model*20);
        base_model_data = *((unsigned char**) data);
        num_vertex = *((unsigned short*) base_model_data); base_model_data+=8;

        repeat(i, num_vertex)
        {
            *(base_model_data+63) = FALSE;
            base_model_data+=64;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
float old_front_xyz[MAX_BONE][3];
float old_side_xyz[MAX_BONE][3];
void render_rotate_bones(unsigned char* data, unsigned short frame, signed char rotation, unsigned char undo_rotation)
{
    // <ZZ> This function rotates every bone that has both joints selected around its central
    //      axis.  Rotation of 0 means no rotation.  It makes a backup of the old rotation
    //      components, so the function can be undone, if undo_rotation is TRUE.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* bone_data;
    unsigned char base_model, detail_level;
    unsigned short i;
    float frotation;
    float frotcos;
    float frotsin;


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    detail_level = 0;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);


    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    frame_data+=11;

    if(undo_rotation == FALSE)
    {
        // Go through each bone, looking for selected joints...
        frotation = rotation * 2.0f * PI / 256.0f;
        frotsin = (float) sin(frotation);
        frotcos = (float) cos(frotation);
        repeat(i, num_bone)
        {
            // Save the old data for an undo...
            old_front_xyz[i][X] = *((float*) frame_data);  frame_data+=4;
            old_front_xyz[i][Y] = *((float*) frame_data);  frame_data+=4;
            old_front_xyz[i][Z] = *((float*) frame_data);  frame_data+=4;
            old_side_xyz[i][X] = *((float*) frame_data);  frame_data+=4;
            old_side_xyz[i][Y] = *((float*) frame_data);  frame_data+=4;
            old_side_xyz[i][Z] = *((float*) frame_data);  frame_data+=4;

            // Is this a selected bone?
            if(select_inlist(*((unsigned short*) (bone_data+1))) && select_inlist(*((unsigned short*) (bone_data+3))))
            {
                // Generate new bone normals...
                frame_data-=24;
                *((float*) frame_data) = (old_front_xyz[i][X] * frotcos) + (old_side_xyz[i][X] * frotsin);  frame_data+=4;
                *((float*) frame_data) = (old_front_xyz[i][Y] * frotcos) + (old_side_xyz[i][Y] * frotsin);  frame_data+=4;
                *((float*) frame_data) = (old_front_xyz[i][Z] * frotcos) + (old_side_xyz[i][Z] * frotsin);  frame_data+=4;
                *((float*) frame_data) = (-old_front_xyz[i][X] * frotsin) + (old_side_xyz[i][X] * frotcos);  frame_data+=4;
                *((float*) frame_data) = (-old_front_xyz[i][Y] * frotsin) + (old_side_xyz[i][Y] * frotcos);  frame_data+=4;
                *((float*) frame_data) = (-old_front_xyz[i][Z] * frotsin) + (old_side_xyz[i][Z] * frotcos);  frame_data+=4;
            }
            bone_data += 9;
        }
    }
    else
    {
        // Go through each bone, restoring the old data...
        repeat(i, num_bone)
        {
            // Save the old data for an undo...
            *((float*) frame_data) = old_front_xyz[i][X];  frame_data+=4;
            *((float*) frame_data) = old_front_xyz[i][Y];  frame_data+=4;
            *((float*) frame_data) = old_front_xyz[i][Z];  frame_data+=4;
            *((float*) frame_data) = old_side_xyz[i][X];  frame_data+=4;
            *((float*) frame_data) = old_side_xyz[i][Y];  frame_data+=4;
            *((float*) frame_data) = old_side_xyz[i][Z];  frame_data+=4;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char selection_box_on = FALSE;
unsigned char selection_close_type = 0;
float selection_box_tl[2];
float selection_box_br[2];
float selection_box_min[2];
float selection_box_max[2];
float select_center_xyz[3] = {0.0f, 0.0f, 0.0f};
float select_offset_xyz[3];
unsigned char select_move_on = FALSE;
unsigned char select_pick_on = FALSE;
#define VIEW_TOP_XY    0
#define VIEW_SIDE_YZ   1
#define VIEW_FRONT_XZ  2
#define VIEW_3D_XYZ    3
unsigned char selection_view = 0;
unsigned char rotation_view = 0;
unsigned char selection_move = 0;
void render_box()
{
    // <ZZ> This function draws the selection box


    // Check size...
    clip(selection_box_min[X], selection_box_br[X], selection_box_max[X]);
    clip(selection_box_min[Y], selection_box_br[Y], selection_box_max[Y]);


    // Draw it
    display_texture_off();
    display_color(yellow);

    display_start_line();
        display_vertex_xyz(selection_box_tl[X], selection_box_tl[Y], 0);
        display_vertex_xyz(selection_box_br[X], selection_box_tl[Y], 0);
    display_end();

    display_start_line();
        display_vertex_xyz(selection_box_br[X], selection_box_tl[Y], 0);
        display_vertex_xyz(selection_box_br[X], selection_box_br[Y], 0);
    display_end();

    display_start_line();
        display_vertex_xyz(selection_box_br[X], selection_box_br[Y], 0);
        display_vertex_xyz(selection_box_tl[X], selection_box_br[Y], 0);
    display_end();

    display_start_line();
        display_vertex_xyz(selection_box_tl[X], selection_box_br[Y], 0);
        display_vertex_xyz(selection_box_tl[X], selection_box_tl[Y], 0);
    display_end();

    display_texture_on();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_get_point_xy(float x, float y, float z, float* x_spot, float* y_spot)
{
    // <ZZ> This function gets the onscreen coordinates of a given point, based on the current
    //      projection and model matrices...  If it's not onscreen, it sets the values to -9999
    //      something...
    float feedback_buffer[4];

    glFeedbackBuffer(4, GL_2D, feedback_buffer);
    glRenderMode(GL_FEEDBACK);
    glBegin(GL_POINTS);
        display_vertex_xyz(x, y, z);
    display_end();
    if(glRenderMode(GL_RENDER))
    {
        *x_spot = feedback_buffer[1]*virtual_x/((float)screen_x);
        *y_spot = virtual_y - (feedback_buffer[2]*virtual_y/((float)screen_y));
    }
    else
    {
        *x_spot = -9999.9f;
        *y_spot = -9999.9f;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_get_point_xyd(float x, float y, float z, float* x_spot, float* y_spot, float* d_spot)
{
    // <ZZ> This function gets the onscreen coordinates of a given point, based on the current
    //      projection and model matrices.  D is depth...  If it's not onscreen, it sets the
    //      values to -9999 something...
    float feedback_buffer[4];

    glFeedbackBuffer(4, GL_3D, feedback_buffer);
    glRenderMode(GL_FEEDBACK);
    glBegin(GL_POINTS);
        display_vertex_xyz(x, y, z);
    display_end();
    if(glRenderMode(GL_RENDER))
    {
        *x_spot = feedback_buffer[1]*virtual_x/((float)screen_x);
        *y_spot = virtual_y - (feedback_buffer[2]*virtual_y/((float)screen_y));
        *d_spot = feedback_buffer[3];
    }
    else
    {
        *x_spot = -9999.9f;
        *y_spot = -9999.9f;
        *d_spot = -9999.9f;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
float script_window_w;
float script_window_h;
unsigned char render_get_point_xyz(float x, float y, float* x_spot, float* y_spot, float* z_spot)
{
    // <ZZ> This function gets the model coordinates of an onscreen point, based on the current
    //      model matrix.  Only works in ortho mode.
    float temp_matrix[16];


    if(x > script_window_x && y > script_window_y)
    {
        if(x < (script_window_x+script_window_w) && y < (script_window_y+script_window_h))
        {
            glGetFloatv(GL_MODELVIEW_MATRIX, temp_matrix);
            x = x - script_window_x;
            y = y - script_window_y;
            x = x / script_window_w;
            y = y / script_window_h;
            x = (x-0.5f)*4.0f;  // Assume -2 to 2 ortho
            y = (y-0.5f)*-4.0f;  // Assume -2 to 2 ortho
            if(selection_view == VIEW_TOP_XY)
            {
                x = (x-temp_matrix[12])/temp_matrix[0];
                y = (y-temp_matrix[13])/temp_matrix[5];
                *x_spot = x;
                *y_spot = y;
                *z_spot = 0;
            }
            if(selection_view == VIEW_SIDE_YZ)
            {
                x = (temp_matrix[12]-x)/temp_matrix[9];
                y = (y-temp_matrix[13])/temp_matrix[2];
                *x_spot = 0;
                *y_spot = x;
                *z_spot = y;
            }
            if(selection_view == VIEW_FRONT_XZ)
            {
                x = (x-temp_matrix[12])/temp_matrix[0];
                y = (y-temp_matrix[13])/temp_matrix[6];
                *x_spot = x;
                *y_spot = 0;
                *z_spot = y;
            }
            return TRUE;
        }
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_model_move(void)
{
    // <ZZ> This function moves vertices and joints and stuff around in an RDY model
    unsigned short index;
    float x, y, z;
    float distance;
    float rotate_matrix[9];


    // Move, Rotate, or Scale selected vertices...
    if(selection_move == MOVE_MODE_ROTATE)
    {
        // Build the rotation matrix...
        x = select_offset_xyz[X] - select_center_xyz[X];
        y = select_offset_xyz[Y] - select_center_xyz[Y];
        z = select_offset_xyz[Z] - select_center_xyz[Z];
        if(rotation_view == VIEW_TOP_XY)         z = 0;
        else if(rotation_view == VIEW_FRONT_XZ)  y = 0;
        else if(rotation_view == VIEW_SIDE_YZ)   x = 0;
        distance = (float) sqrt(x*x + y*y + z*z);
        if(distance > 0.20f)
        {
            // Start from identity
            rotate_matrix[0] = 1;  rotate_matrix[3] = 0; rotate_matrix[6] = 0;
            rotate_matrix[1] = 0;  rotate_matrix[4] = 1; rotate_matrix[7] = 0;
            rotate_matrix[2] = 0;  rotate_matrix[5] = 0; rotate_matrix[8] = 1;
            x = x/distance;  y = y/distance;  z = z/distance;
            if(rotation_view == VIEW_TOP_XY)
            {
                // Rotation in XY plane (top)
                rotate_matrix[0] = y;  rotate_matrix[3] = x;
                rotate_matrix[1] = -x;  rotate_matrix[4] = y;
sprintf(DEBUG_STRING, "Rotation = %f", atan2(-x, y)*57.29577502f);
            }
            else if(rotation_view == VIEW_FRONT_XZ)
            {
                // Rotation in XZ plane (front)
                rotate_matrix[0] = -z;  rotate_matrix[6] = -x;
                rotate_matrix[2] = x;  rotate_matrix[8] = -z;
sprintf(DEBUG_STRING, "Rotation = %f", -atan2(x, -z)*57.29577502f);
            }
            else if(rotation_view == VIEW_SIDE_YZ)
            {
                // Rotation in YZ plane (side)
                rotate_matrix[4] = -z;  rotate_matrix[7] = -y;
                rotate_matrix[5] = y;  rotate_matrix[8] = -z;
sprintf(DEBUG_STRING, "Rotation = %f", atan2(y, -z)*57.29577502f);
            }
        }
        else
        {
            return;
        }
    }


if(key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT])
{
    if(rotation_view == VIEW_TOP_XY)
    {
        select_offset_xyz[Y] = select_center_xyz[Y];
sprintf(NAME_STRING, "Offset X == %f", select_offset_xyz[X] - select_center_xyz[X]);
    }
    else if(rotation_view == VIEW_FRONT_XZ)
    {
        select_offset_xyz[X] = select_center_xyz[X];
sprintf(NAME_STRING, "Offset Z == %f", select_offset_xyz[Z] - select_center_xyz[Z]);
    }
    else if(rotation_view == VIEW_SIDE_YZ)
    {
        select_offset_xyz[Z] = select_center_xyz[Z];
sprintf(NAME_STRING, "Offset Y == %f", select_offset_xyz[Y] - select_center_xyz[Y]);
    }
}

    repeat(index, select_num)
    {
        x = select_xyz[index][X];
        y = select_xyz[index][Y];
        z = select_xyz[index][Z];


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
            x = ((x-select_center_xyz[X])*(1.0f + select_offset_xyz[X] - select_center_xyz[X])) + select_center_xyz[X];
            y = ((y-select_center_xyz[Y])*(1.0f + select_offset_xyz[Y] - select_center_xyz[Y])) + select_center_xyz[Y];
            z = ((z-select_center_xyz[Z])*(1.0f + select_offset_xyz[Z] - select_center_xyz[Z])) + select_center_xyz[Z];
        }
        else if(selection_move == MOVE_MODE_ROTATE)
        {
            // Rotation
            x -= select_center_xyz[X];
            y -= select_center_xyz[Y];
            z -= select_center_xyz[Z];
            select_data[index][X] = x*rotate_matrix[0] + y*rotate_matrix[3] + z*rotate_matrix[6];
            select_data[index][Y] = x*rotate_matrix[1] + y*rotate_matrix[4] + z*rotate_matrix[7];
            select_data[index][Z] = x*rotate_matrix[2] + y*rotate_matrix[5] + z*rotate_matrix[8];
            x = select_data[index][X];
            y = select_data[index][Y];
            z = select_data[index][Z];
            x += select_center_xyz[X];
            y += select_center_xyz[Y];
            z += select_center_xyz[Z];
        }

        // Save
        select_data[index][X] = x;
        select_data[index][Y] = y;
        select_data[index][Z] = z;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_tex_move(unsigned char limit)
{
    // <ZZ> This function moves tex vertices around in an RDY model
    unsigned short index;
    float x, y;
    float distance;
    float rotate_matrix[9];


    // Move, Rotate, or Scale selected vertices...
    if(selection_move == MOVE_MODE_ROTATE)
    {
        // Build the rotation matrix...
        x = select_offset_xyz[X] - select_center_xyz[X];
        y = select_offset_xyz[Y] - select_center_xyz[Y];
        distance = (float) sqrt(x*x + y*y);
        if(distance > 0.0001f)
        {
            // Rotation in XY plane (top)
            x = x/distance;  y = y/distance;
            rotate_matrix[0] = y;  rotate_matrix[3] = x;
            rotate_matrix[1] = -x;  rotate_matrix[4] = y;
        }
        else
        {
            return;
        }
    }

    repeat(index, select_num)
    {
        x = select_xyz[index][X];
        y = select_xyz[index][Y];


        if(selection_move == MOVE_MODE_MOVE)
        {
            // Translation
            x += select_offset_xyz[X] - select_center_xyz[X];
            y += select_offset_xyz[Y] - select_center_xyz[Y];
        }
        else if(selection_move == MOVE_MODE_SCALE)
        {
            // Scaling
            x -= select_center_xyz[X];
            y -= select_center_xyz[Y];
            if(limit)
            {
                x *= (1.0f + ((select_offset_xyz[X] - select_center_xyz[X])*4.0f));
                y *= (1.0f + ((select_offset_xyz[Y] - select_center_xyz[Y])*4.0f));
            }
            else
            {
                x *= (1.0f + (select_offset_xyz[X] - select_center_xyz[X]));
                y *= (1.0f + (select_offset_xyz[Y] - select_center_xyz[Y]));
            }
            x += select_center_xyz[X];
            y += select_center_xyz[Y];
        }
        else if(selection_move == MOVE_MODE_ROTATE)
        {
            // Rotation
            x -= select_center_xyz[X];
            y -= select_center_xyz[Y];
            select_data[index][X] = x*rotate_matrix[0] + y*rotate_matrix[3];
            select_data[index][Y] = x*rotate_matrix[1] + y*rotate_matrix[4];
            x = select_data[index][X];
            y = select_data[index][Y];
            x += select_center_xyz[X];
            y += select_center_xyz[Y];
        }

        // Save
        if(limit)
        {
            if(x > 1.0f)  x = 1.0f;
            if(x < 0.0f)  x = 0.0f;
            if(y > 1.0f)  y = 1.0f;
            if(y < 0.0f)  y = 0.0f;
        }
        select_data[index][X] = x;
        select_data[index][Y] = y;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
void setup_shadow(void)
{
    // <ZZ> This function fills in the shadow_texture table
    unsigned char* data;
    int i;
    unsigned int value;


    // Clear out the table
    log_message("INFO:   Setting up the shadow texture...");
    repeat(i, MAX_SHADOW_TEXTURE)
    {
        shadow_texture[i] = 0;
    }


    // Fill in the table
    i = 0;
    if(sdf_open("SHADOW.TXT"))
    {
        while(sdf_read_line() && i < MAX_SHADOW_TEXTURE)
        {
            // Check for a file...
            if(src_read_token(token_buffer[0]))
            {
                if(token_buffer[0][0] == '-' || token_buffer[0][0] == '=')
                {
                    src_read_token((token_buffer[0]+1));
                }
                #ifdef VERBOSE_COMPILE
                    log_message("INFO:     Looking for %s", token_buffer[0]);
                #endif
                data = sdf_find_index(token_buffer[0]);
                value = 0;
                if(data)
                {
                    #ifdef VERBOSE_COMPILE
                        log_message("INFO:     Found file");
                    #endif
                    data = (unsigned char*) sdf_read_unsigned_int(data);
                    value = *((unsigned int*) (data+2));
                }
                shadow_texture[i] = value;
                i++;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define DIVIDE_BY_255 0.003921568627450980392156862745098f
void render_bone_frame(unsigned char* base_model_data, unsigned char* bone_data, unsigned char* current_frame_data)
{
    // <ZZ> This function generates vertex positions and normals, based on bone positions.
    //      Current_frame_data starts with bone information (skips first 11 values of RDY bone frame)...
    unsigned short num_vertex;
    unsigned short num_bone;
    unsigned short i;
    unsigned char j;
    float* vertex_xyz;
    float* normal_xyz;
    float* start_xyz;
    float* end_xyz;
    float  height_xyz[3];
    float* side_xyz;
    float* front_xyz;
    float* joint_data;
    float* scalars;
    float height;
    float front;
    float side;
    float cross_xyz[3];
    unsigned char bone_binding[2];
    unsigned char bone_weight;
    unsigned short joint;


// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!


// !!!BAD!!!
// !!!BAD!!!  Optimize this!!!
// !!!BAD!!!


// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



//log_message("INFO:   Rendering bone frame...");
    num_vertex = *((unsigned short*) base_model_data);  base_model_data+=6;
    num_bone = *((unsigned short*) base_model_data);  base_model_data+=2;
    joint_data = (float*) (current_frame_data + (num_bone<<3) + (num_bone<<4));
    repeat(i, num_vertex)
    {
        vertex_xyz = (float*) base_model_data;  base_model_data+=12;
        bone_binding[0] = *(base_model_data);  base_model_data++;
        bone_binding[1] = *(base_model_data);  base_model_data++;
        bone_weight = *(base_model_data);  base_model_data++;
        normal_xyz = (float*) base_model_data;  base_model_data+=12;

        vertex_xyz[X] = 0;
        vertex_xyz[Y] = 0;
        vertex_xyz[Z] = 0;
        scalars = (float*) base_model_data;


#ifdef DEVTOOL
        if((bone_weight & 128) && global_billboard_active)  // So modeler doesn't do billboard on boning models...
#else
        if(bone_weight & 128)
#endif
        {
            // Anchor flag is set...  Do billboard style vertex coordinates...  !!!ANCHOR!!!
            bone_weight<<=1;  // Get rid of anchor flag...  !!!ANCHOR!!!

            normal_xyz[X] = 1.0f;
            normal_xyz[Y] = 0.0f;
            normal_xyz[Z] = 0.0f;

//log_message("INFO:   Calc'in bill-vertex %d", i);

            repeat(j, 2)
            {
                joint = *((unsigned short*) (bone_data + (bone_binding[j]<<3) + bone_binding[j] + 1));
                start_xyz = (joint_data + (joint<<1) + joint);
                joint = *((unsigned short*) (bone_data + (bone_binding[j]<<3) + bone_binding[j] + 3));
                end_xyz = (joint_data + (joint<<1) + joint);
                height_xyz[X] = end_xyz[X]-start_xyz[X];
                height_xyz[Y] = end_xyz[Y]-start_xyz[Y];
                height_xyz[Z] = end_xyz[Z]-start_xyz[Z];
                height = *scalars;
                front = *(scalars+4);
//log_message("INFO:     Front %d == %f", j, front);
                scalars++;


                // Normalize the vector...
                cross_product(camera_fore_xyz, height_xyz, cross_xyz);
                side = *((float*) (bone_data + (bone_binding[j]<<3) + bone_binding[j] + 5));
side = side-(0.5f*(dot_product(camera_fore_xyz, height_xyz)));
                cross_xyz[X]/=side;
                cross_xyz[Y]/=side;
                cross_xyz[Z]/=side;



                vertex_xyz[X] += (start_xyz[X] + (height_xyz[X] * height) + (cross_xyz[X] * front) ) * bone_weight;
                vertex_xyz[Y] += (start_xyz[Y] + (height_xyz[Y] * height) + (cross_xyz[Y] * front) ) * bone_weight;
                vertex_xyz[Z] += (start_xyz[Z] + (height_xyz[Z] * height) + (cross_xyz[Z] * front) ) * bone_weight;
                bone_weight = 255 - bone_weight;
            }
            // Divide by 255 to correct for bone_weight
            vertex_xyz[X] = vertex_xyz[X] * DIVIDE_BY_255;
            vertex_xyz[Y] = vertex_xyz[Y] * DIVIDE_BY_255;
            vertex_xyz[Z] = vertex_xyz[Z] * DIVIDE_BY_255;
//log_message("INFO:     Position == %f, %f, %f", vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z]);
            base_model_data+=37;
        }
        else
        {
            // Anchor flag is not set...  Do normal vertex coordinates...
            bone_weight<<=1;  // Get rid of anchor flag...  !!!ANCHOR!!!
            repeat(j, 2)
            {
                front_xyz = (float*) (current_frame_data + (bone_binding[j]<<4) + (bone_binding[j]<<3));
                side_xyz = front_xyz + 3;
//log_message("INFO:     Vertex %d, Bone %d", i, j);


                joint = *((unsigned short*) (bone_data + (bone_binding[j]<<3) + bone_binding[j] + 1));
                start_xyz = (joint_data + (joint<<1) + joint);
//log_message("INFO:       Joint 1 = %d", joint);
//log_message("INFO:       Start = (%f, %f, %f)", start_xyz[X], start_xyz[Y], start_xyz[Z]);


                joint = *((unsigned short*) (bone_data + (bone_binding[j]<<3) + bone_binding[j] + 3));
                end_xyz = (joint_data + (joint<<1) + joint);
//log_message("INFO:       Joint 2 = %d", joint);
//log_message("INFO:       End = (%f, %f, %f)", end_xyz[X], end_xyz[Y], end_xyz[Z]);


                height_xyz[X] = end_xyz[X]-start_xyz[X];
                height_xyz[Y] = end_xyz[Y]-start_xyz[Y];
                height_xyz[Z] = end_xyz[Z]-start_xyz[Z];
//log_message("INFO:       Height = (%f, %f, %f)", height_xyz[X], height_xyz[Y], height_xyz[Z]);
//log_message("INFO:       Front = (%f, %f, %f)", front_xyz[X], front_xyz[Y], front_xyz[Z]);
//log_message("INFO:       Side = (%f, %f, %f)", side_xyz[X], side_xyz[Y], side_xyz[Z]);

                if(j == 0)
                {
                    height = *(scalars+6);
                    front = *(scalars+7);
                    side = *(scalars+8);
                    normal_xyz[X] = (height_xyz[X] * height) + (front_xyz[X] * front) + (side_xyz[X] * side);
                    normal_xyz[Y] = (height_xyz[Y] * height) + (front_xyz[Y] * front) + (side_xyz[Y] * side);
                    normal_xyz[Z] = (height_xyz[Z] * height) + (front_xyz[Z] * front) + (side_xyz[Z] * side);
//log_message("INFO:       Generd = (%f, %f, %f)", normal_xyz[X], normal_xyz[Y], normal_xyz[Z]);
                }


                height = *scalars;
                front = *(scalars+2);
                side = *(scalars+4);
                scalars++;


//log_message("INFO:       Height = %f", height);
//log_message("INFO:       Front = %f", front);
//log_message("INFO:       Side = %f", side);

                vertex_xyz[X] += (start_xyz[X] + (height_xyz[X] * height) + (front_xyz[X] * front) + (side_xyz[X] * side)) * bone_weight;
                vertex_xyz[Y] += (start_xyz[Y] + (height_xyz[Y] * height) + (front_xyz[Y] * front) + (side_xyz[Y] * side)) * bone_weight;
                vertex_xyz[Z] += (start_xyz[Z] + (height_xyz[Z] * height) + (front_xyz[Z] * front) + (side_xyz[Z] * side)) * bone_weight;
                bone_weight = 255 - bone_weight;
            }
            // Divide by 255 to correct for bone_weight
            vertex_xyz[X] = vertex_xyz[X] * DIVIDE_BY_255;
            vertex_xyz[Y] = vertex_xyz[Y] * DIVIDE_BY_255;
            vertex_xyz[Z] = vertex_xyz[Z] * DIVIDE_BY_255;
//log_message("INFO:       Vertex at (%f, %f, %f)", vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z]);
            base_model_data+=37;
        }
    }
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
#define PLANE_NORMAL_TOLERANCE 0.02f
void render_generate_bone_normals(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function regenerates bone normals, for when the joints move around...
    float* front_xyz;
    float* side_xyz;
    float bone_xyz[3];
    float distance;
    unsigned char* normal_data;
    unsigned char* bone_data;
    unsigned char* joint_position_data_start;
    unsigned char* frame_data;
    float* joint_position_data;
    unsigned short joint[2];
    unsigned short num_bone;
    unsigned char num_base_model;
    unsigned char base_model;
    unsigned short num_bone_frame;
    unsigned char detail_level;
    unsigned short i;
    float new_front_xyz[3];
    float new_side_xyz[3];



    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
//log_message("INFO:   Base model %d", base_model);
    detail_level = 0;

    data = data + (base_model*20) + (num_base_model*20*detail_level);
    bone_data = *((unsigned char**) data);
    num_bone = *((unsigned short*) (bone_data+6));
    bone_data = *((unsigned char**) (data+16));


    normal_data = frame_data+11;
    joint_position_data_start = normal_data + (num_bone<<4) + (num_bone<<3);
//joint_position_data = (float*) joint_position_data_start;
//joint_position_data+=6;
//*joint_position_data += 0.1f;


    // Make it not rotate in weird planes...
//    keep[X] = (rotation_view == VIEW_SIDE_YZ);
//    keep[Y] = (rotation_view == VIEW_FRONT_XZ);
//    keep[Z] = (rotation_view == VIEW_TOP_XY);
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Should only keep for main bones being moved...  Others may float around...  Maybe???
// !!!BAD!!!
// !!!BAD!!!


    repeat(i, num_bone)
    {
//log_message("INFO:   Generate bone %d", i);
        // Front and side normals
        front_xyz = (float*) normal_data;  normal_data+=12;
        side_xyz = (float*) normal_data;  normal_data+=12;


        // Get the current bone vector...  Normalize it...
        joint[0] = *((unsigned short*) (bone_data+1));
        joint[1] = *((unsigned short*) (bone_data+3));
        bone_data+=9;
//log_message("INFO:     Joint %d to %d", joint[0], joint[1]);
        joint_position_data = ((float*) (joint_position_data_start + (joint[1]<<3) + (joint[1]<<2)));
        bone_xyz[X] = joint_position_data[X];
        bone_xyz[Y] = joint_position_data[Y];
        bone_xyz[Z] = joint_position_data[Z];
        joint_position_data = ((float*) (joint_position_data_start + (joint[0]<<3) + (joint[0]<<2)));
//log_message("INFO:     Start = (%f, %f, %f)", joint_position_data[X], joint_position_data[Y], joint_position_data[Z]);
//log_message("INFO:     End = (%f, %f, %f)", bone_xyz[X], bone_xyz[Y], bone_xyz[Z]);
        bone_xyz[X] -= joint_position_data[X];
        bone_xyz[Y] -= joint_position_data[Y];
        bone_xyz[Z] -= joint_position_data[Z];


        // Calculate new front normal by crossing side and bone
        cross_product(bone_xyz, side_xyz, new_front_xyz);
        distance = vector_length(new_front_xyz);
        if(distance > 0.00001f)
        {
            new_front_xyz[X]/=distance;  new_front_xyz[Y]/=distance;  new_front_xyz[Z]/=distance;
        }
        else
        {
            new_front_xyz[Y] = 1.0f;
        }

        // Calculate new side normal by crossing front and bone
        cross_product(bone_xyz, new_front_xyz, new_side_xyz);
        distance = vector_length(new_side_xyz);
        if(distance > 0.00001f)
        {
            new_side_xyz[X]/=distance;  new_side_xyz[Y]/=distance;  new_side_xyz[Z]/=distance;
        }
        else
        {
            new_side_xyz[X] = 1.0f;
        }

        // Only apply changes if past a certain threshold...
        distance = (float) fabs(side_xyz[X] - new_side_xyz[X]);
        distance += (float) fabs(side_xyz[Y] - new_side_xyz[Y]);
        distance += (float) fabs(side_xyz[Z] - new_side_xyz[Z]);
        distance += (float) fabs(front_xyz[X] - new_front_xyz[X]);
        distance += (float) fabs(front_xyz[Y] - new_front_xyz[Y]);
        distance += (float) fabs(front_xyz[Z] - new_front_xyz[Z]);
        if(distance > 0.01f)
        {
            side_xyz[X] = -new_side_xyz[X];
            side_xyz[Y] = -new_side_xyz[Y];
            side_xyz[Z] = -new_side_xyz[Z];
            front_xyz[X] = new_front_xyz[X];
            front_xyz[Y] = new_front_xyz[Y];
            front_xyz[Z] = new_front_xyz[Z];
        }
    }




/*
        distance = vector_length(bone_xyz);
        if(distance > 0.001f)
        {
            bone_xyz[X]/=distance;
            bone_xyz[Y]/=distance;
            bone_xyz[Z]/=distance;


            // Determine new front normal by pushing and pulling it until it is the correct distance from the bone vector...
            repeat(j, 20)
            {
                spring_xyz[X] = front_xyz[X] - bone_xyz[X];
                spring_xyz[Y] = front_xyz[Y] - bone_xyz[Y];
                spring_xyz[Z] = front_xyz[Z] - bone_xyz[Z];
                distance = vector_length(spring_xyz);
                if(distance > 0.001f)
                {
                    distance = 1.4142135623730950488016887242097f / distance;
                    front_xyz[X] = bone_xyz[X] + (spring_xyz[X] * distance);
                    front_xyz[Y] = bone_xyz[Y] + (spring_xyz[Y] * distance);
                    front_xyz[Z] = bone_xyz[Z] + (spring_xyz[Z] * distance);
                    distance = vector_length(front_xyz);
                    if(distance > 0.001f)
                    {
                        front_xyz[X]/=distance;
                        front_xyz[Y]/=distance;
                        front_xyz[Z]/=distance;
                    }
                }


                // Try to keep the front normal from moving in the wrong plane...
                if(keep[X])
                {
                    front_xyz[X] = old_front_xyz[X];
                }
                if(keep[Y])
                {
                    front_xyz[Y] = old_front_xyz[Y];
                }
                if(keep[Z])
                {
                    front_xyz[Z] = old_front_xyz[Z];
                }
            }



            // Calculate side normal with a cross product
            cross_product(front_xyz, bone_xyz, side_xyz);
            distance = vector_length(side_xyz);
            if(distance > 0.001f)
            {
                side_xyz[X]/=distance;
                side_xyz[Y]/=distance;
                side_xyz[Z]/=distance;
            }
        }
    }
*/

}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_tex_vertex(unsigned char* data, unsigned short frame, float* coordinates_xy, unsigned short tex_vertex_to_delete, unsigned short tex_vertex_replacement)
{
    // <ZZ> This function adds a new texture vertex to a model file.  If coordinates_xy is NULL,
    //      it deletes tex_vertex_to_delete, instead of adding one.  Returns TRUE if it worked,
    //      FALSE otherwise.  If tex_vertex_replacement is != MAX_TEX_VERTEX, and we're deleting,
    //      then a new tex_vertex is substituted for the old one...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_tex_vertex;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* tex_vertex_data;
    unsigned char* texture_data;
    unsigned char* texture_data_start;
    unsigned short num_primitive, num_primitive_vertex;
    unsigned char texture_mode;
    unsigned char texture_flags;
    unsigned short i, j, k;
    signed short amount_to_add;
    unsigned char base_model, detail_level;



    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    tex_vertex_data =  *((unsigned char**) data);  data+=4;
    texture_data = *((unsigned char**) data);

    base_model_data+=2;
    num_tex_vertex = *((unsigned short*) base_model_data);
    amount_to_add = 0;
    if(coordinates_xy != NULL)
    {
        // Insert a texture vertex
        if(sdf_insert_data(tex_vertex_data+(num_tex_vertex<<3), NULL, 8))
        {
            // Added the tex vertex successfully...  Write coordinates
            *((float*) (tex_vertex_data+(num_tex_vertex<<3))) = coordinates_xy[X];
            *((float*) (tex_vertex_data+4+(num_tex_vertex<<3))) = coordinates_xy[Y];


            // Select the new vertex
            select_add(num_tex_vertex, ((float*) (tex_vertex_data+(num_tex_vertex<<3))));


            // Update number of tex vertices for this base model
            num_tex_vertex++;
            *((unsigned short*) (base_model_data)) = num_tex_vertex;
            amount_to_add = 8;
        }
    }
    else
    {
        // Delete a tex vertex
        if(tex_vertex_to_delete < num_tex_vertex && num_tex_vertex > 1)
        {
//log_message("INFO:   Deleting tex vertex %d", tex_vertex_to_delete);
            // Make sure the tex vertex isn't in use
            texture_data_start = texture_data;
            repeat(i, MAX_DDD_TEXTURE)
            {
                texture_mode = *texture_data;  texture_data++;
                if(texture_mode != 0)
                {
                    texture_flags = *texture_data;
                    texture_data+=2;

                    // Strips...
                    num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                    repeat(j, num_primitive)
                    {
                        num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(k, num_primitive_vertex)
                        {
                            texture_data+=2;
                            if(tex_vertex_to_delete == *((unsigned short*) texture_data))
                            {
                                if(tex_vertex_replacement == MAX_TEX_VERTEX)
                                {
                                    return FALSE;
                                }
                                else
                                {
                                    // Replace the tex vertex with a lower one...
                                    *((unsigned short*) texture_data) = tex_vertex_replacement;
                                }
                            }
                            texture_data+=2;
                        }
                    }


                    // Fans...  Shouldn't be any in DEVTOOL mode...
                    num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                    repeat(j, num_primitive)
                    {
                        num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(k, num_primitive_vertex)
                        {
                            texture_data+=4;
                        }
                    }
                }
            }
//log_message("INFO:     Tex vertex not in use...  Safe to delete");


            // Delete the texture vertex
            if(sdf_insert_data(tex_vertex_data+(tex_vertex_to_delete<<3), NULL, -8))
            {
//log_message("INFO:     Deleted 8 bytes");

                // Renumber higher tex vertices
                texture_data = texture_data_start-8;
                repeat(i, MAX_DDD_TEXTURE)
                {
                    texture_mode = *texture_data;  texture_data++;
                    if(texture_mode != 0)
                    {
//log_message("INFO:     Texture %d is on", i);
                        texture_data+=2;


                        // Strips...
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
//log_message("INFO:     %d strips", num_primitive);
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=2;
                                if(tex_vertex_to_delete < *((unsigned short*) texture_data)) *((unsigned short*) texture_data)-=1;
                                texture_data+=2;
                            }
                        }


                        // Fans...  Shouldn't be any in DEVTOOL mode...
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
//log_message("INFO:     %d fans", num_primitive);
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=4;
                            }
                        }
                    }
                }



                // Update number of tex vertices for this base model
                num_tex_vertex--;
                *((unsigned short*) (base_model_data)) = num_tex_vertex;
                amount_to_add = -8;
//log_message("INFO:     Done deleting...  Just gotta finish up");
//log_message("INFO:     Left with %d tex vertices...", num_tex_vertex);
            }
        }
    }


    // Finish up...
    if(amount_to_add != 0)
    {
        // Update all base model pointers at start of file
        *((unsigned char**) data) += amount_to_add;  data+=4;
        *((unsigned char**) data) += amount_to_add;  data+=4;
        *((unsigned char**) data) += amount_to_add;  data+=4;
        base_model++;
        while(base_model < num_base_model)
        {
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            base_model++;
        }


        // Update bone frame pointers, only if bone frames are internal to this file (not linked to another RDY)
        if((flags & DDD_EXTERNAL_BONE_FRAMES) == 0)
        {
            repeat(i, num_bone_frame)
            {
                *((unsigned char**) data) += amount_to_add;  data+=4;
            }
        }
        return TRUE;
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
unsigned char render_pregenerate_normals(unsigned char* data, unsigned short frame, unsigned char detail_level)
{
    // <ZZ> This function generates vertex normal info for an RDY file.  It also swaps
    //      bone attachments if the second attachment is higher weighted than the
    //      first, as normals only attach to the first bone...  Normals should
    //      attach to the bone of higher weight...  Returns TRUE if it worked,
    //      FALSE if not (because frame invalid).
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short base_model;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned char* vertex_data_start;
    unsigned char* bone_data_start;
    float* bone_normal_data_start;
    float* joint_data_start;
    unsigned short num_primitive, num_primitive_vertex, num_vertex, num_joint, num_bone;
    unsigned char texture_mode;
    unsigned short i, j, k, m;
    unsigned short vertex[3];
    unsigned short joint[2];
    unsigned short bone;
    float length, height, front, side;
    float* normal_xyz;
    float start_xyz[3];
    float end_xyz[3];
    float front_xyz[3];
    float side_xyz[3];
    float height_xyz[3];
    float point_xyz[3];
    unsigned char tex_alpha;


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);



    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    frame_data+=11;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=8;
    texture_data = *((unsigned char**) data); data+=8;
    bone_data_start = *((unsigned char**) data);


    num_vertex = *((unsigned short*) base_model_data);
    num_joint = *((unsigned short*) (base_model_data+4));
    num_bone = *((unsigned short*) (base_model_data+6));
    vertex_data_start = base_model_data+8;
    bone_normal_data_start = (float*) frame_data;
    joint_data_start = (float*) (frame_data+(num_bone*24));


    // Clear out the normals for each vertex (swap bone bindings if need be)
    repeat(i, num_vertex)
    {
        normal_xyz = ((float*) (vertex_data_start+15+(i<<6)));
        normal_xyz[X] = 0.0f;
        normal_xyz[Y] = 0.0f;
        normal_xyz[Z] = 0.0f;
//        if(*(vertex_data_start+14+(i<<6)) < 128)
        if(((unsigned char) ((*(vertex_data_start+14+(i<<6)))<<1)) < 124)  // !!!ANCHOR!!!
        {
            // Swap the bindings
//            *(vertex_data_start+14+(i<<6)) = 255 - *(vertex_data_start+14+(i<<6));
            *(vertex_data_start+14+(i<<6)) = (127 - ((*(vertex_data_start+14+(i<<6))) & 127)) | ((*(vertex_data_start+14+(i<<6))) & 128);  // !!!ANCHOR!!!


            k = *(vertex_data_start+12+(i<<6));
            *(vertex_data_start+12+(i<<6)) = *(vertex_data_start+13+(i<<6));
            *(vertex_data_start+13+(i<<6)) = (unsigned char) k;
            // Swap the scalars
            front_xyz[X] = *((float*) (vertex_data_start+27+(i<<6)));
            front_xyz[Y] = *((float*) (vertex_data_start+35+(i<<6)));
            front_xyz[Z] = *((float*) (vertex_data_start+43+(i<<6)));
            *((float*) (vertex_data_start+27+(i<<6))) = *((float*) (vertex_data_start+31+(i<<6)));
            *((float*) (vertex_data_start+35+(i<<6))) = *((float*) (vertex_data_start+39+(i<<6)));
            *((float*) (vertex_data_start+43+(i<<6))) = *((float*) (vertex_data_start+47+(i<<6)));
            *((float*) (vertex_data_start+31+(i<<6))) = front_xyz[X];
            *((float*) (vertex_data_start+39+(i<<6))) = front_xyz[Y];
            *((float*) (vertex_data_start+47+(i<<6))) = front_xyz[Z];
        }
    }

    // Go through each texture triangle strip and fan, accumulating normal data as we go...
    repeat(i, MAX_DDD_TEXTURE)
    {
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            texture_data++;     // Skip flags
            tex_alpha = *texture_data;  texture_data++;


            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(j, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                repeat(k, num_primitive_vertex)
                {
                    vertex[k%3] = *((unsigned short*) texture_data);  texture_data+=4; 


                    if(k > 1)
                    {
                        // We have a triangle...  Calculate its normal, then accumulate into each vertex...
                        start_xyz[X] = *((float*) (vertex_data_start+(vertex[0]<<6)));
                        start_xyz[Y] = *((float*) (vertex_data_start+4+(vertex[0]<<6)));
                        start_xyz[Z] = *((float*) (vertex_data_start+8+(vertex[0]<<6)));
                        end_xyz[X] = *((float*) (vertex_data_start+(vertex[1]<<6)));
                        end_xyz[Y] = *((float*) (vertex_data_start+4+(vertex[1]<<6)));
                        end_xyz[Z] = *((float*) (vertex_data_start+8+(vertex[1]<<6)));
                        end_xyz[X]-=start_xyz[X];
                        end_xyz[Y]-=start_xyz[Y];
                        end_xyz[Z]-=start_xyz[Z];
                        side_xyz[X]= *((float*) (vertex_data_start+(vertex[2]<<6)));
                        side_xyz[Y]= *((float*) (vertex_data_start+4+(vertex[2]<<6)));
                        side_xyz[Z]= *((float*) (vertex_data_start+8+(vertex[2]<<6)));
                        side_xyz[X]-=start_xyz[X];
                        side_xyz[Y]-=start_xyz[Y];
                        side_xyz[Z]-=start_xyz[Z];
                        cross_product(end_xyz, side_xyz, front_xyz);
                        if(k&1)
                        {
                            front_xyz[X] = -front_xyz[X];
                            front_xyz[Y] = -front_xyz[Y];
                            front_xyz[Z] = -front_xyz[Z];
                        }
                        if(tex_alpha < 255)
                        {
                            // Make multipass textures not mess up cartoon lines...
                            front_xyz[X] *= 0.125f;
                            front_xyz[Y] *= 0.125f;
                            front_xyz[Z] *= 0.125f;
                        }
                        repeat(m, 3)
                        {
                            normal_xyz = ((float*) (vertex_data_start+15+(vertex[m]<<6)));
                            normal_xyz[X] += front_xyz[X];
                            normal_xyz[Y] += front_xyz[Y];
                            normal_xyz[Z] += front_xyz[Z];
                        }
                    }
                }
            }


            // Fans...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(j, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                repeat(k, num_primitive_vertex)
                {
                    if(k == 0)
                    {
                        vertex[k] = *((unsigned short*) texture_data);
                    }
                    else
                    {
                        vertex[2-(k&1)] = *((unsigned short*) texture_data);
                    }
                    texture_data+=4; 

                    if(k > 1)
                    {
                        // We have a triangle...  Calculate its normal, then accumulate into each vertex...
                        start_xyz[X] = *((float*) (vertex_data_start+(vertex[0]<<6)));
                        start_xyz[Y] = *((float*) (vertex_data_start+4+(vertex[0]<<6)));
                        start_xyz[Z] = *((float*) (vertex_data_start+8+(vertex[0]<<6)));
                        end_xyz[X] = *((float*) (vertex_data_start+(vertex[1]<<6)));
                        end_xyz[Y] = *((float*) (vertex_data_start+4+(vertex[1]<<6)));
                        end_xyz[Z] = *((float*) (vertex_data_start+8+(vertex[1]<<6)));
                        end_xyz[X]-=start_xyz[X];
                        end_xyz[Y]-=start_xyz[Y];
                        end_xyz[Z]-=start_xyz[Z];
                        side_xyz[X]= *((float*) (vertex_data_start+(vertex[2]<<6)));
                        side_xyz[Y]= *((float*) (vertex_data_start+4+(vertex[2]<<6)));
                        side_xyz[Z]= *((float*) (vertex_data_start+8+(vertex[2]<<6)));
                        side_xyz[X]-=start_xyz[X];
                        side_xyz[Y]-=start_xyz[Y];
                        side_xyz[Z]-=start_xyz[Z];
                        cross_product(end_xyz, side_xyz, front_xyz);
                        if(tex_alpha < 255)
                        {
                            // Make multipass textures not mess up cartoon lines...
                            front_xyz[X] *= 0.125f;
                            front_xyz[Y] *= 0.125f;
                            front_xyz[Z] *= 0.125f;
                        }
                        repeat(m, 3)
                        {
                            normal_xyz = ((float*) (vertex_data_start+15+(vertex[m]<<6)));
                            normal_xyz[X] += front_xyz[X];
                            normal_xyz[Y] += front_xyz[Y];
                            normal_xyz[Z] += front_xyz[Z];
                        }
                    }
                }
            }
        }
        else
        {
        }
    }


    // Normalize the normals for each vertex, and calculate the bone binding scalars
    repeat(i, num_vertex)
    {
        normal_xyz = ((float*) (vertex_data_start+15+(i<<6)));
        length = vector_length(normal_xyz);
        if(length > 0.01f)
        {
            normal_xyz[X] /= length;
            normal_xyz[Y] /= length;
            normal_xyz[Z] /= length;
        }
        bone = *(vertex_data_start+12+(i<<6));
        joint[0] = *((unsigned short*)(bone_data_start+1+(bone*9)));
        joint[1] = *((unsigned short*)(bone_data_start+3+(bone*9)));


        // Yucky stuff...  Similar to crunch_vertex...
        start_xyz[X] = *(joint_data_start + (joint[0]*3));
        start_xyz[Y] = *(joint_data_start + 1 + (joint[0]*3));
        start_xyz[Z] = *(joint_data_start + 2 + (joint[0]*3));

        end_xyz[X] = *(joint_data_start + (joint[1]*3));
        end_xyz[Y] = *(joint_data_start + 1 + (joint[1]*3));
        end_xyz[Z] = *(joint_data_start + 2 + (joint[1]*3));

        front_xyz[X] = *((float*) (bone_normal_data_start + (bone*6)));
        front_xyz[Y] = *((float*) (bone_normal_data_start + 1 + (bone*6)));
        front_xyz[Z] = *((float*) (bone_normal_data_start + 2 + (bone*6)));
        side_xyz[X] = *((float*) (bone_normal_data_start + 3 + (bone*6)));
        side_xyz[Y] = *((float*) (bone_normal_data_start + 4 + (bone*6)));
        side_xyz[Z] = *((float*) (bone_normal_data_start + 5 + (bone*6)));

        // Point to find scalars for...
        point_xyz[X] = normal_xyz[X] + start_xyz[X];
        point_xyz[Y] = normal_xyz[Y] + start_xyz[Y];
        point_xyz[Z] = normal_xyz[Z] + start_xyz[Z];

        // Vector from bone start to bone end
        end_xyz[X]-=start_xyz[X];
        end_xyz[Y]-=start_xyz[Y];
        end_xyz[Z]-=start_xyz[Z];

        // Vector from bone start to point
        height_xyz[X] = normal_xyz[X];
        height_xyz[Y] = normal_xyz[Y];
        height_xyz[Z] = normal_xyz[Z];

        // Calculate height value
        length = dot_product(end_xyz, end_xyz);
        height = dot_product(end_xyz, height_xyz);
        if(length > 0) height = height/length;
        else height = 0;

        // Calculate the height position
        height_xyz[X] = start_xyz[X] + (end_xyz[X]*height);
        height_xyz[Y] = start_xyz[Y] + (end_xyz[Y]*height);
        height_xyz[Z] = start_xyz[Z] + (end_xyz[Z]*height);

        // Calculate vector from height position (along bone) to point
        height_xyz[X] = point_xyz[X] - height_xyz[X];
        height_xyz[Y] = point_xyz[Y] - height_xyz[Y];
        height_xyz[Z] = point_xyz[Z] - height_xyz[Z];

        // Calculate front and side values
        front = dot_product(front_xyz, height_xyz);
        side = dot_product(side_xyz, height_xyz);

        // Write the normal scalars
        normal_xyz = ((float*) (vertex_data_start+51+(i<<6)));
        normal_xyz[X] = height;
        normal_xyz[Y] = front;
        normal_xyz[Z] = side;
    }
    return TRUE;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void clean_up_tex_vertex(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function deletes all unused tex vertices in a given model.
    unsigned short i;
    select_clear();
    repeat(i, MAX_VERTEX)
    {
        if(render_insert_tex_vertex(data, frame, NULL, i, MAX_TEX_VERTEX))
        {
            // Successfully deleted a tex vertex...  Later ones pushed back...
            i--;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void remove_all_tex_vertex(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function deletes all tex vertices in a given model.  Except 0.
    unsigned short i;
    select_clear();
log_message("INFO:   Detexture...");
    i = 1;
    while(i < MAX_VERTEX)
    {
        if(render_insert_tex_vertex(data, frame, NULL, i, 0))
        {
            // Successfully deleted a tex vertex...  Later ones pushed back...
            i--;
        }
        i++;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_joint_size(unsigned char* data, unsigned short frame, unsigned short joint, unsigned char size)
{
    // <ZZ> This function sets the size of a given joint
    unsigned char* frame_data;
    unsigned char* joint_data;
    unsigned char base_model;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_joint;
    unsigned char detail_level;

    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;

    data = data + (base_model*20) + (num_base_model*20*detail_level);
    joint_data = *((unsigned char**) data);
    num_joint = *((unsigned short*) (joint_data+4));
    joint_data = *((unsigned char**) (data+12));
    if(joint < num_joint)
    {
        joint_data+=(joint<<2);
        *((float*) joint_data) = size * JOINT_COLLISION_SCALE;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_attach_vertex_to_bone(unsigned char* data, unsigned short frame, unsigned short vertex)
{
    // <ZZ> This function attaches a vertex to the nearest bone or nearest two bones...  Will
    //      not attach to special bones (weapon grips), unless there are no other options...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex, num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* bone_data;
    unsigned short i, j, k, joint;
    unsigned char base_model, detail_level;
    unsigned char found_match;
    float joint_distance[MAX_JOINT];
    unsigned char joint_order[MAX_JOINT];
    float* joint_xyz;
    float vector_xyz[3];
    float vertex_xyz[3];
    unsigned short num_normal_bone;



    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    bone_data =  *((unsigned char**) (data+12));
    num_vertex = *((unsigned short*) base_model_data); base_model_data+=4;
    if(vertex >= num_vertex) return;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;


    // Figure out how many non-grip bones there are...
    num_normal_bone = 0;
    repeat(i, num_bone)
    {
        if(bone_data[i*9] == 0)
        {
            num_normal_bone++;
        }
    }

    // Figure out coordinates of current vertex (assume that it has been updated recently...)
    vertex_xyz[X] = *((float*) (base_model_data+(vertex<<6)));
    vertex_xyz[Y] = *((float*) (base_model_data+4+(vertex<<6)));
    vertex_xyz[Z] = *((float*) (base_model_data+8+(vertex<<6)));
log_message("INFO:   Attaching vertex...");
log_message("INFO:     Prior Actual = (%f, %f, %f)", vertex_xyz[X], vertex_xyz[Y], vertex_xyz[Z]);


    // Make a list of joint distances, then order 'em nearest first...
    joint_xyz = (float*) (frame_data + 11 + (num_bone*24));
    if(num_joint > MAX_JOINT)  num_joint = MAX_JOINT;
    repeat(i, num_joint)
    {
        vector_xyz[X] = joint_xyz[X] - vertex_xyz[X];
        vector_xyz[Y] = joint_xyz[Y] - vertex_xyz[Y];
        vector_xyz[Z] = joint_xyz[Z] - vertex_xyz[Z];
        joint_distance[i] = vector_length(vector_xyz);
        joint_xyz+=3;
    }
    repeat(i, num_joint)
    {
        k = 0;  // Count of how many are before this one...
        j = 0;
        while(j < i)
        {
            if(joint_distance[i] >= joint_distance[j]) k++;
            j++;
        }
        j++;  // Skip the current joint...  Don't compare with self...
        while(j < num_joint)
        {
            if(joint_distance[i] > joint_distance[j]) k++;
            j++;
        }
        joint_order[k] = (unsigned char) i;
    }


    // Find the nearest joint that's connected to another...
    found_match = FALSE;
    i = 0;
    while(found_match == FALSE && i < num_joint)
    {
        j = joint_order[i];
        repeat(k, num_bone)
        {
            if(num_normal_bone > 0)
            {
                if(j == *((unsigned short*) (bone_data+1+(k*9))) || j == *((unsigned short*) (bone_data+3+(k*9))))
                {
                    // Only pick non-grip joints...
                    if(*(bone_data+(k*9)) == 0)
                    {
                        joint = j;
                        found_match = TRUE;
                        k = num_bone;
                    }
                }
            }
            else
            {
                if(j == *((unsigned short*) (bone_data+1+(k*9))) || j == *((unsigned short*) (bone_data+3+(k*9))))
                {
                    // Pick any joint
                    joint = j;
                    found_match = TRUE;
                    k = num_bone;
                }
            }
        }
        i++;
    }
log_message("INFO:     Nearest joint with a bone is %d", joint);


    // Go through every joint after, looking for nearest 2 connected bones...
    if(found_match)
    {
        found_match = 0;
        while(found_match < 2 && i < num_joint)
        {
            j = joint_order[i];
            repeat(k, num_bone)
            {
                if(joint == *((unsigned short*) (bone_data+1+(k*9))) || joint == *((unsigned short*) (bone_data+3+(k*9))))
                {
                    if(j == *((unsigned short*) (bone_data+1+(k*9))) || j == *((unsigned short*) (bone_data+3+(k*9))))
                    {
                        // Only take non-grip bones...
                        if(bone_data[k*9] == 0 || num_normal_bone == 0)
                        {
                            // Record the bone...
                            *(base_model_data+12+found_match+(vertex<<6)) = (unsigned char) k;
log_message("INFO:     Attached to bone %d", k);
                            found_match++;
                            k = num_bone;
                        }
                    }
                }
            }
            i++;
        }
    }



    // If we only found one bone, force the weight to be full 255...
    if(found_match == 1)
    {
        *(base_model_data+13+(vertex<<6)) = *(base_model_data+12+(vertex<<6));
//        *(base_model_data+14+(vertex<<6)) = 255;
        *(base_model_data+14+(vertex<<6)) = 127 | ((*(base_model_data+14+(vertex<<6))) & 128);  // Max weight is 127...  Anchor bit is topmost...  !!!ANCHOR!!!
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_solid_box(unsigned char* color, float tlx, float tly, float tlz, float brx, float bry, float brz)
{
    // <ZZ> This function draws a 3D box for debug purposes...
    display_texture_off();
    display_blend_off();
    display_shade_off();
    display_color(color);


    // Top...
    display_start_fan();
        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(brx, tly, tlz);
        display_vertex_xyz(brx, bry, tlz);
        display_vertex_xyz(tlx, bry, tlz);
    display_end();


    // Bottom...
    display_start_fan();
        display_vertex_xyz(tlx, tly, brz);
        display_vertex_xyz(tlx, bry, brz);
        display_vertex_xyz(brx, bry, brz);
        display_vertex_xyz(brx, tly, brz);
    display_end();


    // Front...
    display_start_fan();
        display_vertex_xyz(tlx, bry, tlz);
        display_vertex_xyz(brx, bry, tlz);
        display_vertex_xyz(brx, bry, brz);
        display_vertex_xyz(tlx, bry, brz);
    display_end();


    // Back...
    display_start_fan();
        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(tlx, tly, brz);
        display_vertex_xyz(brx, tly, brz);
        display_vertex_xyz(brx, tly, tlz);
    display_end();



    // Left...
    display_start_fan();
        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(tlx, bry, tlz);
        display_vertex_xyz(tlx, bry, brz);
        display_vertex_xyz(tlx, tly, brz);
    display_end();


    // Right...
    display_start_fan();
        display_vertex_xyz(brx, tly, tlz);
        display_vertex_xyz(brx, tly, brz);
        display_vertex_xyz(brx, bry, brz);
        display_vertex_xyz(brx, bry, tlz);
    display_end();


    // Draw edge lines...
    display_color(white);
    display_start_line();
        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(brx, tly, tlz);

        display_vertex_xyz(tlx, bry, tlz);
        display_vertex_xyz(brx, bry, tlz);

        display_vertex_xyz(tlx, tly, brz);
        display_vertex_xyz(brx, tly, brz);

        display_vertex_xyz(tlx, bry, brz);
        display_vertex_xyz(brx, bry, brz);



        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(tlx, bry, tlz);

        display_vertex_xyz(brx, tly, tlz);
        display_vertex_xyz(brx, bry, tlz);

        display_vertex_xyz(tlx, tly, brz);
        display_vertex_xyz(tlx, bry, brz);

        display_vertex_xyz(brx, tly, brz);
        display_vertex_xyz(brx, bry, brz);



        display_vertex_xyz(tlx, tly, tlz);
        display_vertex_xyz(tlx, tly, brz);

        display_vertex_xyz(brx, tly, tlz);
        display_vertex_xyz(brx, tly, brz);

        display_vertex_xyz(tlx, bry, tlz);
        display_vertex_xyz(tlx, bry, brz);

        display_vertex_xyz(brx, bry, tlz);
        display_vertex_xyz(brx, bry, brz);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------

#define MAX_CARTOON_LINE 8192
unsigned char draw_cartoon_line[MAX_CARTOON_LINE];
unsigned char cartoon_line_alpha[MAX_CARTOON_LINE];  // Used in room drawing...


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
float global_gnomify_working_direction_xyz[3];
void render_gnomify_working_direction(unsigned char* data, unsigned short frame, unsigned short joint_one, unsigned short joint_two)
{
    // <ZZ> This function sets the working direction (a vector from one joint to another) for moving
    //      around joints for the modeler's g'nomify tool...
    unsigned char num_detail_level, num_base_model, base_model, detail_level;
    unsigned short num_bone_frame, num_joint, num_bone;
    unsigned char* frame_data;
    float* joint_one_xyz;
    float* joint_two_xyz;

    // Default to 0, 0, 0 if we fail...
    global_gnomify_working_direction_xyz[X] = 0.0f;
    global_gnomify_working_direction_xyz[Y] = 0.0f;
    global_gnomify_working_direction_xyz[Z] = 0.0f;

    // Read the RDY file's header...
    data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) { return; }
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    data = *((unsigned char**) data);
    data+=4;
    num_joint = *((unsigned short*) data);  data+=2;
    num_bone =  *((unsigned short*) data);
    if(joint_one >= num_joint || joint_two >= num_joint) { return; }

    // Find the joints...
    frame_data+=11;  // Skip bone_frame header...
    frame_data+=24*num_bone;
    joint_one_xyz = (float*) (frame_data + (12*joint_one));
    joint_two_xyz = (float*) (frame_data + (12*joint_two));

    // Find the vector...
    global_gnomify_working_direction_xyz[X] = joint_two_xyz[X]-joint_one_xyz[X];
    global_gnomify_working_direction_xyz[Y] = joint_two_xyz[Y]-joint_one_xyz[Y];
    global_gnomify_working_direction_xyz[Z] = joint_two_xyz[Z]-joint_one_xyz[Z];
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_gnomify_affect_joint(unsigned char* data, unsigned short frame, unsigned short joint, unsigned char percent)
{
    // <ZZ> This function offsets the specified joint's position by the working direction vector.
    //      Percent is used to scale the effect (0 is no effect, 100 is full effect).

    unsigned char num_detail_level, num_base_model, base_model, detail_level;
    unsigned short num_bone_frame, num_joint, num_bone;
    unsigned char* frame_data;
    float* joint_xyz;
    float scale;

    // Read the RDY file's header...
    data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) { return; }
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    data = *((unsigned char**) data);
    data+=4;
    num_joint = *((unsigned short*) data);  data+=2;
    num_bone =  *((unsigned short*) data);
    if(joint >= num_joint) { return; }

    // Find the joints...
    frame_data+=11;  // Skip bone_frame header...
    frame_data+=24*num_bone;
    joint_xyz = (float*) (frame_data + (12*joint));

    // Apply the vector...
    scale = percent * 0.01f;
    joint_xyz[X] += global_gnomify_working_direction_xyz[X] * scale;
    joint_xyz[Y] += global_gnomify_working_direction_xyz[Y] * scale;
    joint_xyz[Z] += global_gnomify_working_direction_xyz[Z] * scale;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_joint_from_vertex_location(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function moves a specified joint to be at the same exact position as the given vertex...
    //      Useful for adding weapon bones to models after they've been animated (because I was too lazy
    //      to do it the right way in the first place)...  Assumes last 4 vertices and last 4 joints...
    unsigned char num_detail_level, num_base_model, base_model, detail_level;
    unsigned short num_bone_frame, num_joint, num_bone, num_vertex;
    unsigned char* frame_data;
    unsigned char* vertex_data;
    unsigned short joint, vertex;
    unsigned short i;
    float* joint_xyz;
    float* vertex_xyz;

    // Default to 0, 0, 0 if we fail...
    global_gnomify_working_direction_xyz[X] = 0.0f;
    global_gnomify_working_direction_xyz[Y] = 0.0f;
    global_gnomify_working_direction_xyz[Z] = 0.0f;

    // Read the RDY file's header...
    data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) { return; }
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    data = *((unsigned char**) data);
    num_vertex = *((unsigned short*) data);  data+=4;
    num_joint = *((unsigned short*) data);  data+=2;
    num_bone =  *((unsigned short*) data);  data+=2;
    vertex_data = data;
    if(num_joint < 4 || num_vertex < 4) { return; }
    joint = num_joint-4;
    vertex = num_vertex-4;
    frame_data+=11;  // Skip bone_frame header...
    frame_data+=24*num_bone;


    repeat(i, 4)
    {
        // Find the vertex location...
        vertex_xyz = (float*) (vertex_data + (64*vertex));


        // Find the joint location...
        joint_xyz = (float*) (frame_data + (12*joint));


        // Modify the joint location...
        joint_xyz[X] = vertex_xyz[X];
        joint_xyz[Y] = vertex_xyz[Y];
        joint_xyz[Z] = vertex_xyz[Z];
        joint++;
        vertex++;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
unsigned char global_rdy_show_joint_numbers = TRUE;
unsigned char global_rdy_detail_level = 0;
unsigned short global_num_bone = 0;    // Yicky thing for helping models in scripts
unsigned char* global_bone_data = NULL;   // Yicky thing for helping models in scripts
float eye_quad_xy[4][2] = { {0.0f, 0.0f}, {0.0f, 0.5f}, {0.5f, 0.0f}, {0.5f, 0.5f} };
unsigned char eye_frame_quad[32] = {0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
void render_rdy(unsigned char* data, unsigned short frame, unsigned char mode, unsigned char** texture_file_data_block, unsigned char main_alpha, unsigned char* bone_frame_data, unsigned char petrify, unsigned char eye_frame)
{
    // <ZZ> This function draws an RDY object model.  If bone_frame_data is non-NULL, it is used
    //      to generate vertex locations using render_bone_frame...  Should skip first 11 bytes
    //      of bone frame...
    //      Eye frame is for the per-texture RENDER_EYE_FLAG for tex vertex offsets...  Should be
    //      from 0-31
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex;
    unsigned short num_tex_vertex;
    unsigned short num_joint;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* vertex_data;
    unsigned char* tex_vertex_data;
    unsigned char* texture_data;
    unsigned char* joint_data;
    unsigned char* bone_data;
    unsigned char* frame_data;
    float* texture_matrix;
    unsigned char texture_mode;
    unsigned char texture_flags;
    unsigned char texture_alpha;
    unsigned short num_primitive;
    unsigned short num_primitive_vertex;
    unsigned short i, j, k;
    unsigned short vertex;
    unsigned short tex_vertex;
    unsigned char base_model, detail_level;
    unsigned int color_argument;
    float start_xy[2];
    float texpos_xy[2];
    float dot;
    float* onscreen_joint_xyss;
    float* inworld_joint_xyz;
    float distance_xyz[3];
    unsigned char we_have_to_draw_it;
    unsigned char* texture_data_start;
    float distance_to_camera;
    float cartoon_line_size;


#ifndef BACKFACE_CARTOON_LINES
    float perp_xy[2];
    float* vertex_bxy;
#endif


#ifdef DEVTOOL
    float x, y, z, d;
    float best_depth;
    unsigned char* start_data;
    unsigned short m;
    unsigned char found_connection;
    unsigned char* best_data;
    unsigned char* primitive_data;
    unsigned short best_vertex;
    unsigned char texture_to_skin;
    float triangle_xy[3][2];
    float tex_vertex_onscreen_xy[MAX_VERTEX][2];
    unsigned short vertex_to_tex_vertex[MAX_VERTEX];
    unsigned short tex_vertex_to_add;
    float onscreen_min_x, onscreen_max_x, onscreen_min_y, onscreen_max_y;
    unsigned char* joint_position_data;
    unsigned char  temporary_particle_data[PARTICLE_SIZE];
    start_data = data;
#endif



    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) frame = 0;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);



    distance_to_camera = 0.0f;


    if(drawing_world)
    {
        distance_xyz[X] = ((float*) onscreen_joint_character_data)[X]-camera_xyz[X];
        distance_xyz[Y] = ((float*) onscreen_joint_character_data)[Y]-camera_xyz[Y];
        distance_xyz[Z] = ((float*) onscreen_joint_character_data)[Z]+3.00f-camera_xyz[Z];
        distance_to_camera = distance_xyz[X]*camera_fore_xyz[X] + distance_xyz[Y]*camera_fore_xyz[Y] + distance_xyz[Z]*camera_fore_xyz[Z];
        if(distance_to_camera < 0.001f)
        {
            distance_to_camera = 0.001f;
        }
        if(distance_to_camera > 160.0f)
        {
            distance_to_camera = 160.0f;
        }
        cartoon_line_size = (distance_to_camera+60.0f)*CHAR_LINE_SIZE;
        detail_level = (unsigned char) ((global_rdy_detail_level)*distance_to_camera/160.0f);
    }
    else
    {
        cartoon_line_size = 0.125f;
        detail_level = global_rdy_detail_level;
    }



    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);

    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    tex_vertex_data = *((unsigned char**) data);  data+=4;
    texture_data = *((unsigned char**) data);  data+=4;
    joint_data = *((unsigned char**) data);  data+=4;
    bone_data = *((unsigned char**) data);


    vertex_data = base_model_data;
    num_vertex = *((unsigned short*) vertex_data); vertex_data+=2;
    num_tex_vertex = *((unsigned short*) vertex_data); vertex_data+=2;
    num_joint = *((unsigned short*) vertex_data); vertex_data+=2;
    num_bone = *((unsigned short*) vertex_data); vertex_data+=2;



    // Figure out if any of the joints are onscreen or not...
    if(onscreen_joint_active)
    {
        #ifdef DEVTOOL
            // Draw joints...
            if(key_down[SDL_SCANCODE_F11])
            {
                display_texture_off();
                inworld_joint_xyz = (float*) (bone_frame_data + (num_bone<<4) + (num_bone<<3));
                repeat(i, num_joint)
                {
                    display_solid_marker(dark_red, inworld_joint_xyz[X], inworld_joint_xyz[Y], inworld_joint_xyz[Z], *((float*) (joint_data+(i<<2))));
                    inworld_joint_xyz += 3;
                }
            }
        #endif




        // Find the bone numbers for named bones...  Left, Right, Left2, Right2...  Also get
        // the frame event flags...
        repeat(i, 8)
        {
            temp_character_bone_number[i] = 255;
        }
        repeat(i, num_bone)
        {
            temp_character_bone_number[(*(bone_data+(i*9))) & 7] = (unsigned char) i;
        }
        temp_character_frame_event = frame_data[1];




        // Go ahead and do the onscreen joint stuff...
        onscreen_joint_xyss = (float*) fourthbuffer;
        inworld_joint_xyz = (float*) (bone_frame_data + (num_bone<<4) + (num_bone<<3));
        num_onscreen_joint = 0;
        repeat(i, num_joint)
        {
            // Transform by the onscreen matrix...
            onscreen_joint_xyss[X] = onscreen_matrix[0] * inworld_joint_xyz[X] + onscreen_matrix[4] * inworld_joint_xyz[Y] + onscreen_matrix[8]  * inworld_joint_xyz[Z] + onscreen_matrix[12];
            onscreen_joint_xyss[Y] = onscreen_matrix[1] * inworld_joint_xyz[X] + onscreen_matrix[5] * inworld_joint_xyz[Y] + onscreen_matrix[9]  * inworld_joint_xyz[Z] + onscreen_matrix[13];
            dot                    = onscreen_matrix[3] * inworld_joint_xyz[X] + onscreen_matrix[7] * inworld_joint_xyz[Y] + onscreen_matrix[11] * inworld_joint_xyz[Z] + onscreen_matrix[15];
            if(dot != 0.0f)
            {
                onscreen_joint_xyss[X]/=dot;      onscreen_joint_xyss[Y]/=dot;
                onscreen_joint_xyss[X]*=virtual_x/2;   onscreen_joint_xyss[X]+=virtual_x/2;
                onscreen_joint_xyss[Y]*=-virtual_y/2;  onscreen_joint_xyss[Y]+=virtual_y/2;
                num_onscreen_joint++;
                dot = (ONSCREEN_CLICK_SCALE/dot);
                onscreen_joint_xyss[2] = (*((float*) (joint_data+(i<<2)))) * dot;  // Joint size...
                onscreen_joint_xyss[3] = ((*((float*) (joint_data+(i<<2)))) + 1.5f) * dot;  // Joint size...  Enlarged to make onscreen check pass easier...
                onscreen_joint_xyss += 4;
            }
            inworld_joint_xyz += 3;
        }


        // All onscreen locations figured...  Do we have to draw it?
        we_have_to_draw_it = FALSE;
        onscreen_joint_xyss = (float*) fourthbuffer;
        repeat(i, num_onscreen_joint)
        {
            if(onscreen_joint_xyss[X] > -onscreen_joint_xyss[3] && onscreen_joint_xyss[Y] > -onscreen_joint_xyss[3] && onscreen_joint_xyss[3] > 0.0f)
            {
                if(onscreen_joint_xyss[X] < (virtual_x+onscreen_joint_xyss[3]) && onscreen_joint_xyss[Y] < (virtual_y+onscreen_joint_xyss[3]))
                {
                    we_have_to_draw_it = TRUE;
                    i = num_onscreen_joint;
                }
            }
            onscreen_joint_xyss += 4;
        }
        if(we_have_to_draw_it == FALSE)
        {
            onscreen_joint_active = FALSE;
            return;
        }
onscreen_draw_count++;
        // Figure out if the cursor is over this character...
        if(mouse_last_object==NULL)
        {
            // Was this mouse over this character last time?
            if(mouse_last_item == onscreen_joint_character)
            {
                // Yup, highlight it...
                if(onscreen_joint_character < MAX_CHARACTER)
                {
                    // But only if we move mouse...  'Cause it's annoying to have characters flashing red all the time...
                    if(mouse_text_timer > 0 || mouse_x != mouse_last_x || mouse_y != mouse_last_y)
                    {
                        line_color[0] = 255;
                        line_color[1] = 64;

                        // Handle mouse over names...
                        mouse_text_timer=MOUSE_TEXT_TIME;
                        if(onscreen_joint_character < MAX_CHARACTER)
                        {
                            // Call the character's script to get its name...
                            fast_run_script(main_character_script_start[onscreen_joint_character], FAST_FUNCTION_GETNAME, main_character_data[onscreen_joint_character]);
                            strcpy(mouse_text, NAME_STRING);
                        }
                    }
                }
            }
            // Is it over it now?
            onscreen_joint_xyss = (float*) fourthbuffer;
            repeat(i, num_onscreen_joint)
            {
                distance_xyz[X] = onscreen_joint_xyss[X] - mouse_x;
                dot = onscreen_joint_xyss[Y] - mouse_y;
                if((dot*dot + distance_xyz[X]*distance_xyz[X]) < (onscreen_joint_xyss[2]*onscreen_joint_xyss[2]))
                {
                    // What distance are we at?
                    distance_xyz[X] = ((float*) onscreen_joint_character_data)[X] - camera_xyz[X];
                    distance_xyz[Y] = ((float*) onscreen_joint_character_data)[Y] - camera_xyz[Y];
                    distance_xyz[Z] = ((float*) onscreen_joint_character_data)[Z] - camera_xyz[Z];
                    dot = distance_xyz[X]*distance_xyz[X] + distance_xyz[Y]*distance_xyz[Y] + distance_xyz[Z]*distance_xyz[Z];
                    if(dot < mouse_character_distance)
                    {
                        // Remember that we're over it, but don't color line yet in case there's a better one...
                        mouse_character_distance = dot;
                        mouse_current_item = onscreen_joint_character;
                        i = num_onscreen_joint;
                    }
                }
                onscreen_joint_xyss += 4;
            }
        }
    }
    else
    {
        // Window rendering mode has to remember this stuff for attached parts...
        global_bone_data = bone_data;
        global_num_bone = num_bone;
    }


    // Generate vertex coordinates based on bone locations...
    if(bone_frame_data)
    {
        render_bone_frame(base_model_data, bone_data, bone_frame_data);
    }


#ifdef DEVTOOL
    switch(mode & 240)
    {
        case WIN_3D_VERTEX:
            // Show selected joints...
            display_texture_off();
            joint_position_data = frame_data + 11 + (num_bone*24);
            best_data = joint_position_data;
            if((mode&15) == 11)
            {
                primitive_data = joint_data;
                display_zbuffer_off();
                repeat(i, num_joint)
                {
                    d = *((float*) primitive_data);
                    if(d < 0.05f) d = 0.05f;
                    display_solid_marker(dark_red, *((float*) (best_data)), *((float*) (best_data+4)), *((float*) (best_data+8)), d);
                    best_data+=12;
                    primitive_data+=4;
                }
                display_zbuffer_on();
                primitive_data = joint_data;
                best_data = joint_position_data;
                repeat(i, num_joint)
                {
                    d = *((float*) primitive_data);
                    if(d < 0.05f) d = 0.05f;
                    if(select_inlist(i))
                    {
                        // Selected joint
                        display_marker(white, *((float*) (best_data)), *((float*) (best_data+4)), *((float*) (best_data+8)), d);
                    }
                    else
                    {
                        // Unselected joint
                        display_marker(red, *((float*) (best_data)), *((float*) (best_data+4)), *((float*) (best_data+8)), d);
                    }
                    best_data+=12;
                    primitive_data+=4;
                }
            }
            else
            {
                repeat(i, num_joint)
                {
                    if((mode&15) >= 6 && select_inlist(i))
                    {
                        // Selected joint
                        display_marker(white, *((float*) (best_data)), *((float*) (best_data+4)), *((float*) (best_data+8)), 0.05f);
                        if(global_rdy_show_joint_numbers)
                        {
                            display_paperdoll_on();
                            display_texture_on();
                            (*((float*) (temporary_particle_data))) = (*((float*) (best_data)));
                            (*((float*) (temporary_particle_data+4))) = (*((float*) (best_data+4)));
                            (*((float*) (temporary_particle_data+8))) = (*((float*) (best_data+8)));
                            (*((float*) (temporary_particle_data+36))) = 0.5f;
                            (*((float*) (temporary_particle_data+40))) = 0.5f;
                            (*((unsigned int*) (temporary_particle_data+44))) = texture_ascii_ptr;
                            temporary_particle_data[48] = 255;
                            temporary_particle_data[49] = 255;
                            temporary_particle_data[50] = 255;
                            temporary_particle_data[51] = (unsigned char) i;
                            (*((unsigned short*) (temporary_particle_data+56))) = 0;
                            (*((unsigned short*) (temporary_particle_data+60))) = PART_SPIN_ON | PART_NUMBER_ON;
                            temporary_particle_data[79] = 255;
                            particle_draw(temporary_particle_data);
                            display_texture_off();
                            display_paperdoll_off();
                        }
                    }
                    else
                    {
                        // Unselected joint
                        display_marker(red, *((float*) (best_data)), *((float*) (best_data+4)), *((float*) (best_data+8)), 0.05f);
                    }
                    best_data+=12;
                }
            }


            // Show movement offset...
            if((mode&15) == 13)
            {
                display_color(green);
                display_start_line();
                    display_vertex_xyz(0, 0, 0);
                    display_vertex_xyz(*((float*) (frame_data+3)), *((float*) (frame_data+7)), 0);
                display_end();
            }


            // Select vertices on box close...
            if(selection_close_type == BORDER_SELECT)
            {
                data = vertex_data;
                repeat(i, num_vertex)
                {
                    if(!select_inlist(i) && *(data+63) == FALSE)
                    {
                        render_get_point_xy(*((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), &x, &y);
                        if(x > selection_box_tl[X] &&  x < selection_box_br[X])
                        {
                            if(y > selection_box_tl[Y] &&  y < selection_box_br[Y])
                            {
                                select_add(i, ((float*) data));
                            }
                        }
                    }
                    data+=64;
                }
            }
            else if(selection_close_type == BORDER_CROSS_HAIRS && selection_view != VIEW_3D_XYZ)
            {
                if((mode & 15) != 1 && mode != 38)
                {
                    render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &select_center_xyz[Y], &select_center_xyz[Z]);
                }
                else
                {
                    if(selection_view == VIEW_TOP_XY)
                    {
                        render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &select_center_xyz[Y], &dot);
                    }
                    else if(selection_view == VIEW_FRONT_XZ)
                    {
                        render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &dot, &select_center_xyz[Z]);
                    }
                    else
                    {
                        render_get_point_xyz(mouse_x, mouse_y, &dot, &select_center_xyz[Y], &select_center_xyz[Z]);
                    }
                    if(key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT])
                    {
                        // Snapped to nearest quarter foot...
                        select_center_xyz[X] = ((int) (select_center_xyz[X]*4.0f)) * 0.25f;
                        select_center_xyz[Y] = ((int) (select_center_xyz[Y]*4.0f)) * 0.25f;
                        select_center_xyz[Z] = ((int) (select_center_xyz[Z]*4.0f)) * 0.25f;
                    }
                }       
                if((mode & 15) == 13)
                {
                    // Movement offset mode...
                    *((float*) (frame_data+3)) = select_center_xyz[X];
                    *((float*) (frame_data+7)) = select_center_xyz[Y];
                    if(key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT])
                    {
                        // Snap to 0 on x axis...
                        *((float*) (frame_data+3)) = 0.0f;
                    }
                }
                select_update_xyz();
            }
            else if(selection_close_type == BORDER_POINT_PICK)
            {
                best_vertex = 65535;
                best_depth = 99999.0f;
                if((mode&15) >= 6)
                {
                    // Selecting joints
                    data = joint_position_data;
                    j = 12;
                    k = num_joint;
                }
                else
                {
                    // Selecting vertices
                    data = vertex_data;
                    j = 64;
                    k = num_vertex;
                }

                // Look through the coordinates
                repeat(i, k)
                {
                    if(!select_inlist(i))
                    {
                        // Check hidden flag...
                        found_connection = TRUE;
                        if(j == 64)
                        {
                            if(*(data+63))
                            {
                                found_connection = FALSE;
                            }
                        }
                        if(found_connection)
                        {
                            render_get_point_xyd(*((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), &x, &y, &d);
                            x = x-mouse_x;
                            y = y-mouse_y;
                            x = (float) sqrt(x*x + y*y);
                            if(x < 5.0f && d < best_depth)
                            {
                                best_depth = d;
                                best_vertex = i;
                                best_data = data;
                            }
                        }
                    }
                    data+=j;
                }
                if(best_vertex != 65535)
                {
                    select_add(best_vertex, ((float*) best_data));
                }
            }
            else if(selection_close_type == BORDER_MOVE && selection_view != VIEW_3D_XYZ)
            {
                if(select_move_on)
                {
                    render_get_point_xyz(mouse_x, mouse_y, &select_offset_xyz[X], &select_offset_xyz[Y], &select_offset_xyz[Z]);
                    render_model_move();
                    if((mode&15) == 8 && select_num > 0)
                    {
                        render_fix_model_to_bone_length(start_data, frame, 9999);
                    }
                }
                else
                {
                    render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &select_center_xyz[Y], &select_center_xyz[Z]);
                    select_update_xyz();
                    if((mode&15) < 6)
                    {
                        repeat(i, select_num)
                        {
                            render_attach_vertex_to_bone(start_data, frame, select_list[i]);
                            render_crunch_vertex(start_data, frame, select_list[i], TRUE, 0);
                        }
                    }
                    else
                    {
                        if(key_down[SDL_SCANCODE_LSHIFT] == FALSE && key_down[SDL_SCANCODE_RSHIFT] == FALSE)
                        {
                            select_clear();
                        }
                    }
                }
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_ALL)
            {
                data = vertex_data;
                repeat(i, num_vertex)
                {
                    if(!select_inlist(i) && *(data+63) == FALSE)
                    {
                        select_add(i, ((float*) data));
                    }
                    data+=64;
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_INVERT)
            {
                data = vertex_data;
                repeat(i, num_vertex)
                {
                    if(!select_inlist(i))
                    {
                        if(*(data+63) == FALSE)
                        {
                            select_add(i, ((float*) data));
                        }
                    }
                    else
                    {
                        // Items with a NULL data get removed later
                        select_data[select_index] = NULL;
                    }
                    data+=64;
                }
                repeat(i, select_num)
                {
                    if(select_data[i] == NULL)
                    {
                        select_remove(i);
                        i--;
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_CONNECTED)
            {
                // Flag all of our original vertices
                repeat(i, select_num)
                {
                    select_flag[i] = TRUE;
                }
                // Look for any new vertices that are connected to an original vertex by means of a strip
                data = vertex_data;
                repeat(i, num_vertex)
                {
                    if(!select_inlist(i) && *(data+63) == FALSE)
                    {
                        // Check if there's a connection
                        found_connection = FALSE;
                        best_data = texture_data;
                        repeat(m, MAX_DDD_TEXTURE)
                        {
                            texture_mode = *best_data;  best_data++;
                            if(texture_mode != 0)
                            {
                                best_data+=2;

                                // Strips...
                                num_primitive = *((unsigned short*) best_data);  best_data+=2;
                                repeat(j, num_primitive)
                                {
                                    num_primitive_vertex = *((unsigned short*) best_data);  best_data+=2;
                                    best_vertex = FALSE;
                                    primitive_data = best_data;
                                    repeat(k, num_primitive_vertex)
                                    {
                                        vertex = *((unsigned short*) best_data);  best_data+=4;
                                        if(vertex == i)  best_vertex = TRUE;
                                    }
                                    if(best_vertex)
                                    {
                                        best_data = primitive_data;
                                        repeat(k, num_primitive_vertex)
                                        {
                                            vertex = *((unsigned short*) best_data);  best_data+=4;
                                            if(select_inlist(vertex))
                                            {
                                                if(select_flag[select_index])
                                                {
                                                    found_connection = TRUE;
                                                }
                                            }
                                        }
                                    }
                                }

                                // Skip fans...
                                num_primitive = *((unsigned short*) best_data);  best_data+=2;
                                repeat(j, num_primitive)
                                {
                                    num_primitive_vertex = *((unsigned short*) best_data);  best_data+=2;
                                    repeat(k, num_primitive_vertex)
                                    {
                                        best_data+=4;
                                    }
                                }
                            }
                        }
                        if(found_connection)
                        {
                            select_add(i, ((float*) data));
                            select_flag[select_num-1] = FALSE;
                        }
                    }
                    data+=64;
                }
                selection_close_type = 0;
            }



            // Show all of the vertices and their normals
            data = vertex_data;
            repeat(i, num_vertex)
            {
                // Check hidden flag...
                if(*(data+63) == FALSE)
                {
                    if((mode&15) < 6 && select_inlist(i))
                    {
                        // Selected vertex
                        if((*(data+14)) & 128)
                        {
                            // Anchor flag'd vertex...  !!!ANCHOR!!!
                            display_marker(yellow, *((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), 0.05f);
                        }
                        else
                        {
                            // Normal selected vertex...
                            display_marker(white, *((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), 0.05f);
                        }

// Do anchor switch key...  !!!ANCHOR!!!
if(do_anchor_swap)
{
    (*(data+14)) += 128;
}
                    }
                    else
                    {
                        // Unselected vertex
                        if((*(data+14)) & 128)
                        {
                            // Anchor flag'd vertex...  !!!ANCHOR!!!
                            display_marker(gold, *((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), 0.05f);
                        }
                        else
                        {
                            // Normal vertex...
                            display_marker(green, *((float*) (data)), *((float*) (data+4)), *((float*) (data+8)), 0.05f);
                        }
                    }
                    // Normal
                    display_start_line();
                        display_vertex((float*) data);
                        display_vertex_xyz(*((float*) (data))+(*((float*) (data+15))*0.15f), *((float*) (data+4))+(*((float*) (data+19))*0.15f), *((float*) (data+8))+(*((float*) (data+23))*0.15f));
                    display_end();
                }
                data+=64;
            }
if(do_anchor_swap)
{
    do_anchor_swap = FALSE;
}
            // Display the crosshairs...
            if(((mode & 15) >= 1 && (mode&15) <= 4) || (mode&15)==6)
            {
                display_marker(white, select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z], 500.00f);
            }
            display_texture_on();


            break;
        case WIN_3D_BONE:
            // Show all of the joints
            display_texture_off();
            data = frame_data + 11;
            best_data = data;
            data += (num_bone*24);
            joint_position_data = data;
            // Show all of the bones
            data = bone_data;
            repeat(i, num_bone)
            {
                if((*data) == 0)
                {
                    display_color(white);
                }
                else
                {
                    display_color(instrument_color[(*data)&7]);
                }
                data++;
                j = *((unsigned short*) data);  data+=2;
                k = *((unsigned short*) data);  data+=2;
                j = (j<<3)+(j<<2);
                k = (k<<3)+(k<<2);
                display_start_line();
                    display_vertex((float*) (joint_position_data+j));
                    display_vertex((float*) (joint_position_data+k));
                display_end();
                data+=4;
            }
            // Show all of the forward normals
            display_color(gold);
            data = bone_data;
            repeat(i, num_bone)
            {
                data++;
                j = *((unsigned short*) data);  data+=2;
                k = *((unsigned short*) data);  data+=2;
                j = j*12;
                k = k*12;
                m = i*24;
                display_start_line();
                    x = (0.60f * (*((float*) (joint_position_data+j)))) + (0.40f * (*((float*) (joint_position_data+k))));
                    y = (0.60f * (*((float*) (joint_position_data+j+4)))) + (0.40f * (*((float*) (joint_position_data+k+4))));
                    z = (0.60f * (*((float*) (joint_position_data+j+8)))) + (0.40f * (*((float*) (joint_position_data+k+8))));
                    display_vertex_xyz(x, y, z);
                    x = 0.50f * ((*((float*) (joint_position_data+j))) + (*((float*) (joint_position_data+k))));
                    y = 0.50f * ((*((float*) (joint_position_data+j+4))) + (*((float*) (joint_position_data+k+4))));
                    z = 0.50f * ((*((float*) (joint_position_data+j+8))) + (*((float*) (joint_position_data+k+8))));
                    x += 0.25f * (*((float*) (best_data+m)));
                    y += 0.25f * (*((float*) (best_data+m+4)));
                    z += 0.25f * (*((float*) (best_data+m+8)));
                    display_vertex_xyz(x, y, z);
                display_end();


                display_start_line();
                    display_vertex_xyz(x, y, z);
                    x = (0.40f * (*((float*) (joint_position_data+j)))) + (0.60f * (*((float*) (joint_position_data+k))));
                    y = (0.40f * (*((float*) (joint_position_data+j+4)))) + (0.60f * (*((float*) (joint_position_data+k+4))));
                    z = (0.40f * (*((float*) (joint_position_data+j+8)))) + (0.60f * (*((float*) (joint_position_data+k+8))));
                    display_vertex_xyz(x, y, z);
                display_end();
                data+=4;
            }



            // Show all of the side normals
            display_color(gold);
            data = bone_data;
            repeat(i, num_bone)
            {
                data++;
                j = *((unsigned short*) data);  data+=2;
                k = *((unsigned short*) data);  data+=2;
                j = j*12;
                k = k*12;
                m = (i*24)+12;
                display_start_line();
                    x = (0.60f * (*((float*) (joint_position_data+j)))) + (0.40f * (*((float*) (joint_position_data+k))));
                    y = (0.60f * (*((float*) (joint_position_data+j+4)))) + (0.40f * (*((float*) (joint_position_data+k+4))));
                    z = (0.60f * (*((float*) (joint_position_data+j+8)))) + (0.40f * (*((float*) (joint_position_data+k+8))));
                    display_vertex_xyz(x, y, z);
                    x = 0.50f * ((*((float*) (joint_position_data+j))) + (*((float*) (joint_position_data+k))));
                    y = 0.50f * ((*((float*) (joint_position_data+j+4))) + (*((float*) (joint_position_data+k+4))));
                    z = 0.50f * ((*((float*) (joint_position_data+j+8))) + (*((float*) (joint_position_data+k+8))));
                    x += 0.25f * (*((float*) (best_data+m)));
                    y += 0.25f * (*((float*) (best_data+m+4)));
                    z += 0.25f * (*((float*) (best_data+m+8)));
                    display_vertex_xyz(x, y, z);
                display_end();


                display_start_line();
                    display_vertex_xyz(x, y, z);
                    x = (0.40f * (*((float*) (joint_position_data+j)))) + (0.60f * (*((float*) (joint_position_data+k))));
                    y = (0.40f * (*((float*) (joint_position_data+j+4)))) + (0.60f * (*((float*) (joint_position_data+k+4))));
                    z = (0.40f * (*((float*) (joint_position_data+j+8)))) + (0.60f * (*((float*) (joint_position_data+k+8))));
                    display_vertex_xyz(x, y, z);
                display_end();
                data+=4;
            }



            display_texture_on();
            break;
        case WIN_3D_BONE_UPDATE:
            if(select_move_on == FALSE || (mode & 15))
            {
                render_bone_frame(base_model_data, bone_data, (frame_data+11));
                render_pregenerate_normals(start_data, frame, 0);
            }
            break;
        case WIN_3D_TEXVERTEX:
            // Show all of the texture vertices
            display_texture_off();
            data = tex_vertex_data;
            repeat(i, num_tex_vertex)
            {
                x = *((float*) data);  data+=4;
                y = *((float*) data);  data+=4;
                x = (x*script_window_scale*25.0f)+script_window_x;
                y = (y*script_window_scale*25.0f)+script_window_y;
                if(select_inlist(i))
                {
                    // Selected texture vertex
                    display_2d_marker(white, x, y, 2.0f);
                }
                else
                {
                    // Unselected texture vertex
                    display_2d_marker(green, x, y, 2.0f);
                }
            }


            // Select tex vertices on box close...
            if(selection_close_type == BORDER_SELECT)
            {
                data = tex_vertex_data;
                repeat(i, num_tex_vertex)
                {
                    x = *((float*) data);;
                    y = *((float*) (data+4));
                    x = (x*script_window_scale*25.0f)+script_window_x;
                    y = (y*script_window_scale*25.0f)+script_window_y;
                    if(!select_inlist(i))
                    {
                        if(x > (selection_box_tl[X]-1.0f) &&  x < (selection_box_br[X]+1.0f))
                        {
                            if(y > (selection_box_tl[Y]-1.0f) &&  y < (selection_box_br[Y]+1.0f))
                            {
                                select_add(i, ((float*) data));
                            }
                        }
                    }
                    data+=8;
                }
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_ALL)
            {
                data = tex_vertex_data;
                repeat(i, num_tex_vertex)
                {
                    if(!select_inlist(i))
                    {
                        select_add(i, ((float*) data));
                    }
                    data+=8;
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_INVERT)
            {
                data = tex_vertex_data;
                repeat(i, num_tex_vertex)
                {
                    if(!select_inlist(i))
                    {
                        select_add(i, ((float*) data));
                    }
                    else
                    {
                        // Items with a NULL data get removed later
                        select_data[select_index] = NULL;
                    }
                    data+=8;
                }
                repeat(i, select_num)
                {
                    if(select_data[i] == NULL)
                    {
                        select_remove(i);
                        i--;
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_CONNECTED)
            {
                // Flag all of our original vertices
                repeat(i, select_num)
                {
                    select_flag[i] = TRUE;
                }
                // Look for any new vertices that are connected to an original vertex by means of a strip
                data = tex_vertex_data;
                repeat(i, num_tex_vertex)
                {
                    if(!select_inlist(i))
                    {
                        // Check if there's a connection
                        found_connection = FALSE;
                        best_data = texture_data;
                        repeat(m, MAX_DDD_TEXTURE)
                        {
                            texture_mode = *best_data;  best_data++;
                            if(texture_mode != 0)
                            {
                                best_data+=2;

                                // Strips...
                                num_primitive = *((unsigned short*) best_data);  best_data+=2;
                                repeat(j, num_primitive)
                                {
                                    num_primitive_vertex = *((unsigned short*) best_data);  best_data+=2;
                                    best_vertex = FALSE;
                                    primitive_data = best_data;
                                    repeat(k, num_primitive_vertex)
                                    {
                                        best_data+=2;
                                        vertex = *((unsigned short*) best_data);  best_data+=2;
                                        if(vertex == i)  best_vertex = TRUE;
                                    }
                                    if(best_vertex)
                                    {
                                        best_data = primitive_data;
                                        repeat(k, num_primitive_vertex)
                                        {
                                            best_data+=2;
                                            vertex = *((unsigned short*) best_data);  best_data+=2;
                                            if(select_inlist(vertex))
                                            {
                                                if(select_flag[select_index])
                                                {
                                                    found_connection = TRUE;
                                                }
                                            }
                                        }
                                    }
                                }

                                // Skip fans...
                                num_primitive = *((unsigned short*) best_data);  best_data+=2;
                                repeat(j, num_primitive)
                                {
                                    num_primitive_vertex = *((unsigned short*) best_data);  best_data+=2;
                                    repeat(k, num_primitive_vertex)
                                    {
                                        best_data+=4;
                                    }
                                }
                            }
                        }
                        if(found_connection)
                        {
                            select_add(i, ((float*) data));
                            select_flag[select_num-1] = FALSE;
                        }
                    }
                    data+=8;
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_MOVE)
            {
                if(select_move_on)
                {
                    select_offset_xyz[X] = (mouse_x-script_window_x)/(25.0f*script_window_scale);
                    select_offset_xyz[Y] = (mouse_y-script_window_y)/(25.0f*script_window_scale);
                    render_tex_move(TRUE);
                }
                else
                {
                    select_center_xyz[X] = (mouse_x-script_window_x)/(25.0f*script_window_scale);
                    select_center_xyz[Y] = (mouse_y-script_window_y)/(25.0f*script_window_scale);
                    select_update_xy();
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_CROSS_HAIRS)
            {
                select_center_xyz[X] = (mouse_x-script_window_x)/(25.0f*script_window_scale);
                select_center_xyz[Y] = (mouse_y-script_window_y)/(25.0f*script_window_scale);
                selection_close_type = 0;
            }


            // Show all of the texture lines
			display_color(green);
            repeat(i, MAX_DDD_TEXTURE)
            {
                texture_mode = *texture_data;  texture_data++;
                if(texture_mode != 0)
                {
                    texture_data+=2;
                    // Strips...
                    if(i == main_alpha)  // Main alpha used for TEXVERTEX mode to tell which texture is being drawn, so we know which lines to slap down...
                    {
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            display_start_line_loop();
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=2;
                                tex_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                x = *((float*) (tex_vertex_data+(tex_vertex<<3)));
                                y = *((float*) (tex_vertex_data+4+(tex_vertex<<3)));
                                x = (x*script_window_scale*25.0f)+script_window_x;
                                y = (y*script_window_scale*25.0f)+script_window_y;
                                display_vertex_xyz(x, y, 0.0f);
                            }
                            display_end();
                        }
                    }
                    else
                    {
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=4;
                            }
                        }
                    }

                    // Fans...
                    num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                    repeat(j, num_primitive)
                    {
                        num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(k, num_primitive_vertex)
                        {
                            texture_data+=4;
                        }
                    }
                }
            }
            display_texture_on();
            break;
        case WIN_3D_SKIN_FROM_CAMERA:
            // Clear vertex to texture vertex table...  65535...  Also clear out onscreen coordinates...
            #if DETAIL_LEVEL_MAX != 1
                break;
            #endif
//log_message("INFO:   Skin from camera!!!");
            repeat(i, MAX_VERTEX)
            {
                vertex_to_tex_vertex[i] = 65535;
                tex_vertex_onscreen_xy[i][X] = 0.0f;
                tex_vertex_onscreen_xy[i][Y] = 0.0f;
            }
            onscreen_min_x = 9999;
            onscreen_max_x = 0;
            onscreen_min_y = 9999;
            onscreen_max_y = 0;
            tex_vertex_to_add = 0;
//log_message("INFO:     Cleared out old stuff");


            // Generate new tex vertices based on visible selected triangles
            texture_to_skin = mode & 3;
            repeat(i, MAX_DDD_TEXTURE)
            {
                texture_mode = *texture_data;  texture_data++;
                if(texture_mode != 0)
                {
//log_message("INFO:     Texture %d is on", i);
                    texture_data+=2;
                    if(i == texture_to_skin)
                    {
//log_message("INFO:     Texture %d is our texture_to_skin", i);
                        // Go through each strip looking for selected vertices, triangle visible to camera
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
//log_message("INFO:     %d Triangles to search", num_primitive);
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            primitive_data = texture_data;


                            // Are all 3 points of the triangle selected?
                            found_connection = TRUE;
                            repeat(k, num_primitive_vertex)
                            {
                                vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                if(select_inlist(vertex) == FALSE) found_connection = FALSE;
                            }
                            if(found_connection && num_primitive_vertex == 3)
                            {
//log_message("INFO:     All points of triangle %d are selected", j);
                                // Make sure the triangle is visible
                                texture_data = primitive_data;
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                    data = vertex_data + (vertex<<6);
                                    render_get_point_xy(*((float*) data), *((float*) (data+4)), *((float*) (data+8)), &triangle_xy[k][X], &triangle_xy[k][Y]);
//log_message("INFO:       Point %d at (%f, %f)", k, triangle_xy[k][X], triangle_xy[k][Y]);
                                }
                                // First side of triangle...
                                triangle_xy[1][X] -= triangle_xy[0][X];
                                triangle_xy[1][Y] -= triangle_xy[0][Y];
                                // Third side of triangle...
                                triangle_xy[2][X] -= triangle_xy[0][X];
                                triangle_xy[2][Y] -= triangle_xy[0][Y];
                                // Perpenicular...
                                x = triangle_xy[1][Y];
                                y = -triangle_xy[1][X];
                                // Is third point left or right of the first side?  Dot perpendicular to figure...
                                x = x*triangle_xy[2][X] + y*triangle_xy[2][Y];
                                if(x <= 0)
                                {
//log_message("INFO:     Triangle %d is visible too", j);
                                    // Reconstitute onscreen coordinates
                                    triangle_xy[1][X] += triangle_xy[0][X];
                                    triangle_xy[1][Y] += triangle_xy[0][Y];
                                    triangle_xy[2][X] += triangle_xy[0][X];
                                    triangle_xy[2][Y] += triangle_xy[0][Y];


                                    // Triangle is visible...  Pick tex vertices to use now...  Add to file at end...
                                    // If vertex has been used already, use a matching tex vertex
                                    texture_data = primitive_data;
                                    repeat(k, num_primitive_vertex)
                                    {
                                        vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                        if(vertex_to_tex_vertex[vertex] == 65535)
                                        {
                                            vertex_to_tex_vertex[vertex] = tex_vertex_to_add+num_tex_vertex;
                                            x = triangle_xy[k][X];
                                            y = triangle_xy[k][Y];
                                            tex_vertex_onscreen_xy[tex_vertex_to_add+num_tex_vertex][X] = x;
                                            tex_vertex_onscreen_xy[tex_vertex_to_add+num_tex_vertex][Y] = y;
                                            if(x < onscreen_min_x)  onscreen_min_x = x;
                                            if(x > onscreen_max_x)  onscreen_max_x = x;
                                            if(y < onscreen_min_y)  onscreen_min_y = y;
                                            if(y > onscreen_max_y)  onscreen_max_y = y;
                                            tex_vertex_to_add++;
                                        }
                                        *((unsigned short*) texture_data) = vertex_to_tex_vertex[vertex];  texture_data+=2;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        // Skip strips for this texture
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=4;
                            }
                        }
                    }


                    // Skip fans entirely
                    num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                    repeat(j, num_primitive)
                    {
                        num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(k, num_primitive_vertex)
                        {
                            texture_data+=4;
                        }
                    }
                }
            }
            // Finish up putting in tex vertices
            if(tex_vertex_to_add > 0)
            {
                // Scale texture vertex coordinates to range from 0 to 1
                onscreen_max_x -= onscreen_min_x;
//log_message("INFO:   %d new tex vertices...", tex_vertex_to_add);
                onscreen_max_y -= onscreen_min_y;
                repeat(i, tex_vertex_to_add)
                {
                    tex_vertex_onscreen_xy[i+num_tex_vertex][X] -= onscreen_min_x;
                    tex_vertex_onscreen_xy[i+num_tex_vertex][Y] -= onscreen_min_y;
                    tex_vertex_onscreen_xy[i+num_tex_vertex][X] /= onscreen_max_x;
                    tex_vertex_onscreen_xy[i+num_tex_vertex][Y] /= onscreen_max_y;
//log_message("INFO:     %d at (%f, %f)", i+num_tex_vertex, tex_vertex_onscreen_xy[i+num_tex_vertex][X], tex_vertex_onscreen_xy[i+num_tex_vertex][Y]);

                    // Add the vertex to the file...
                    render_insert_tex_vertex(start_data, frame, tex_vertex_onscreen_xy[i+num_tex_vertex], 0, MAX_TEX_VERTEX);
                }
            }
            // Get rid of unused tex vertices
            clean_up_tex_vertex(start_data, frame);
            break;
        case WIN_3D_MODEL:
#endif
            texture_data_start = texture_data;




            // Draw all visible cartoon lines (for solid characters)
/*
#ifdef DEVTOOL
            if(((key_down[SDLK_F12] && selection_view == VIEW_3D_XYZ) || (line_mode&1)) && main_alpha == 255)
#else
            if((line_mode&1) && main_alpha == 255)
#endif
            {
                // Cartoon line data start...
                bone_data += (num_bone*9);
                num_primitive = *((unsigned short*) bone_data);  bone_data+=2;


                // Clear out visibility count for each vertex...
                // Get onscreen coordinates of each vertex...  Stored in fourthbuffer...
                vertex_bxy = (float*) fourthbuffer;
                texture_data = vertex_data;
                repeat(i, num_vertex)
                {
                    // Proper onscreen coordinate figurin'
                    vertex_bxy[X] = onscreen_matrix[0] * ((float*) texture_data)[X] + onscreen_matrix[4] * ((float*) texture_data)[Y] + onscreen_matrix[8]  * ((float*) texture_data)[Z] + onscreen_matrix[12];
                    vertex_bxy[Y] = onscreen_matrix[1] * ((float*) texture_data)[X] + onscreen_matrix[5] * ((float*) texture_data)[Y] + onscreen_matrix[9]  * ((float*) texture_data)[Z] + onscreen_matrix[13];
                    dot           = onscreen_matrix[3] * ((float*) texture_data)[X] + onscreen_matrix[7] * ((float*) texture_data)[Y] + onscreen_matrix[11] * ((float*) texture_data)[Z] + onscreen_matrix[15];

                    if(dot != 0.0f)
                    {
                        vertex_bxy[X]/=dot; vertex_bxy[Y]/=dot;
                    }
                    texture_data+=64;
                    vertex_bxy+=2;
                }
                vertex_bxy-=num_vertex<<1;


                // Determine visibility of each cartoon line...
                texture_data = bone_data;
                repeat(i, num_primitive)
                {
                    // Find a perpendicular to the draw line...
                    vertex = (*((unsigned short*) (texture_data)))<<1;
                    perp_xy[Y] = -vertex_bxy[vertex];
                    vertex++;
                    perp_xy[X] = vertex_bxy[vertex];
                    vertex = (*((unsigned short*) (texture_data+2)))<<1;
                    start_xy[X] = vertex_bxy[vertex];
                    perp_xy[Y] += vertex_bxy[vertex];
                    vertex++;
                    start_xy[Y] = vertex_bxy[vertex];
                    perp_xy[X] -= vertex_bxy[vertex];


                    // Check each check point's location against the perpendicular...
                    vertex = (*((unsigned short*) (texture_data+4)))<<1;
                    texpos_xy[X] = vertex_bxy[vertex] - start_xy[X];
                    vertex++;
                    texpos_xy[Y] = vertex_bxy[vertex] - start_xy[Y];
                    dot = texpos_xy[X] * perp_xy[X] + texpos_xy[Y] * perp_xy[Y];
                    vertex = (*((unsigned short*) (texture_data+6)))<<1;
                    texpos_xy[X] = vertex_bxy[vertex] - start_xy[X];
                    vertex++;
                    texpos_xy[Y] = vertex_bxy[vertex] - start_xy[Y];

                    dot *= (texpos_xy[X] * perp_xy[X] + texpos_xy[Y] * perp_xy[Y]);
                    draw_cartoon_line[i] = (dot >= 0.0f);

                    texture_data+=8;
                }



                // Happy fat lines...
                display_texture_off();
                display_cull_off();
                display_blend_off();
                display_color(line_color);
                repeat(i, num_primitive)
                {
                    if(draw_cartoon_line[i])
                    {
                        // Find both points of line...  Not really texture stuff...
                        vertex = *((unsigned short*) bone_data);
                        data = vertex_data + (vertex<<6);
                        tex_vertex = *((unsigned short*) (bone_data+2));
                        texture_data = vertex_data + (tex_vertex<<6);


                        // Start drawing the faded poly
                        display_start_fan();


                        // First point...
                        vertex_bxy[X] = ((float*) data)[X];
                        vertex_bxy[Y] = ((float*) data)[Y];
                        vertex_bxy[Z] = ((float*) data)[Z];
                        display_vertex(vertex_bxy);


                        // Second point...
                        data+=15;
                        vertex_bxy[X] += ((float*) data)[X]*FAT_LINE_SIZE;
                        vertex_bxy[Y] += ((float*) data)[Y]*FAT_LINE_SIZE;
                        vertex_bxy[Z] += ((float*) data)[Z]*FAT_LINE_SIZE;
                        display_vertex(vertex_bxy);


                        // Third point...
                        vertex_bxy[X] = ((float*) texture_data)[X];
                        vertex_bxy[Y] = ((float*) texture_data)[Y];
                        vertex_bxy[Z] = ((float*) texture_data)[Z];
                        texture_data+=15;
                        vertex_bxy[X+3] = vertex_bxy[X] + ((((float*) texture_data)[X])*FAT_LINE_SIZE);
                        vertex_bxy[Y+3] = vertex_bxy[Y] + ((((float*) texture_data)[Y])*FAT_LINE_SIZE);
                        vertex_bxy[Z+3] = vertex_bxy[Z] + ((((float*) texture_data)[Z])*FAT_LINE_SIZE);
                        display_vertex(vertex_bxy+3);

                        // Fourth point
                        display_vertex(vertex_bxy);
                        display_end();
                    }
                    bone_data+=8;
                }
                display_cull_on();
                display_texture_on();
            }
*/





// !!!BAD!!!
// !!!BAD!!!  Optimize this!!!  Especially with vertex = ((blah))...  data = vertex_data + (vertex<<6)...  No need to do first assignment...
// !!!BAD!!!




            // Draw the character model
            texture_data = texture_data_start;
            color_argument = *((unsigned int*) (texture_file_data_block+4));
            repeat(i, MAX_DDD_TEXTURE)
            {
                texture_mode = *texture_data;  texture_data++;
                if(texture_mode != 0)
                {
                    // Funky thing to make layer'd triangles not blip - since we don't do multitexturing...
                    display_depth_layer(i);


                    texture_flags = *texture_data;  texture_data++;
                    texture_alpha = *texture_data;  texture_data++;




                    // Tell the graphics card which texture we're gonna use...
                    if(petrify)
                    {
                        // Use the petrify texture...
                        if((texture_flags & RENDER_ENVIRO_FLAG))
                        {
                            // DWhite.RGB...
                            display_pick_texture(texture_petrify);
                        }
                        else if((*texture_file_data_block) == NULL)
                        {
                            // Shouldn't happen...
                            display_pick_texture(0);
                        }
                        else
                        {
                            if((*((unsigned int*) ((*texture_file_data_block)+2))) == texture_armor)
                            {
                                // Use the stone armor texture...
                                display_pick_texture(texture_armorst);
                            }
                            else
                            {
                                // Use the actual texture...  Okay for things like eyes that don't have
                                // any color to 'em...
                                display_pick_texture(*((unsigned int*) ((*texture_file_data_block)+2)));
                            }
                        }
                    }
                    else
                    {
                        // Use the model's texture...
                        if((*texture_file_data_block) == NULL)
                        {
                            // Shouldn't happen...
                            display_pick_texture(0);
                        }
                        else
                        {
                            display_pick_texture(*((unsigned int*) ((*texture_file_data_block)+2)));
                        }
                    }





                    // Check color flags...
                    if((texture_flags & RENDER_COLOR_FLAG))
                    {
                        if(petrify)
                        {
                            color_temp[0] = 163;
                            color_temp[1] = 147;
                            color_temp[2] = 126;
                        }
                        else
                        {
                            color_temp[0] = (unsigned char) ((color_argument>>16)&255);
                            color_temp[1] = (unsigned char) ((color_argument>>8)&255);
                            color_temp[2] = (unsigned char) (color_argument&255);
                        }
                    }
                    else
                    {
                        color_temp[0] = 255;
                        color_temp[1] = 255;
                        color_temp[2] = 255;
                    }



                    // Check transparency flags...
                    if(texture_flags & RENDER_LIGHT_FLAG)
                    {
                        display_blend_light();
                        color_temp[3] = (main_alpha * texture_alpha) >> 8;
                        display_color_alpha(color_temp);
                    }
                    else
                    {
                        // Tint the object's color by the lighting values we setup earlier...
                        color_temp[0] = (color_temp[0] * global_render_light_color_rgb[0]) >> 8;
                        color_temp[1] = (color_temp[1] * global_render_light_color_rgb[1]) >> 8;
                        color_temp[2] = (color_temp[2] * global_render_light_color_rgb[2]) >> 8;
                        if(main_alpha < 255 || texture_alpha < 255)
                        {
                            if(texture_alpha < 255)
                            {
                                display_zbuffer_write_off();
                            }
                            display_blend_trans();
                            color_temp[3] = (main_alpha * texture_alpha) >> 8;
                            if(texture_flags & RENDER_EYE_FLAG)
                            {
                                // Make eyes always bright, regardless of invisibility amount...
                                color_temp[3] = (255 + color_temp[3])>>1;
                            }
                            display_color_alpha(color_temp);
                        }
                        else
                        {
                            display_blend_off();
                            display_color(color_temp);
                        }
                    }


                    // Are we doing paperdoll style transparency?
                    if(texture_flags & RENDER_PAPER_FLAG)
                    {
                        display_paperdoll_on();
                    }


                    // Figure out if we're not culling...
                    if(texture_flags & RENDER_NOCULL_FLAG)
                    {
                        display_cull_off();
                    }


                    // Which type of texturing are we doing?
                    if(texture_flags & RENDER_ENVIRO_FLAG)
                    {
                        // Environment mapped...
                        texture_matrix = rotate_enviro_matrix;


                        // Are we doing cartoon (positional) lighting?
                        if(texture_flags & RENDER_CARTOON_FLAG)
                        {
                            // Pan around texture to simulate light direction...
                            start_xy[X] = global_render_light_offset_xy[X];
                            start_xy[Y] = global_render_light_offset_xy[Y];
                        }
                        else
                        {
                            // Simple texture map...
                            start_xy[X] = 0.5f;
                            start_xy[Y] = 0.5f;
                        }


                        // Strips...
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            display_start_strip();
                            repeat(k, num_primitive_vertex)
                            {
                                vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                data = vertex_data + (vertex<<6);
                                texpos_xy[X] = (*((float*) (data+15)) * texture_matrix[0]) + (*((float*) (data+19)) * texture_matrix[1]) + (*((float*) (data+23)) * texture_matrix[2]) + start_xy[X];
                                texpos_xy[Y] = (*((float*) (data+15)) * texture_matrix[3]) + (*((float*) (data+19)) * texture_matrix[4]) + (*((float*) (data+23)) * texture_matrix[5]) + start_xy[Y];
                                display_texpos(texpos_xy);
                                display_vertex((float*) data);
                            }
                            display_end();
                        }


                        // Fans...
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            display_start_fan();
                            repeat(k, num_primitive_vertex)
                            {
                                vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                data = vertex_data + (vertex<<6);
                                texpos_xy[X] = (*((float*) (data+15)) * texture_matrix[0]) + (*((float*) (data+19)) * texture_matrix[1]) + (*((float*) (data+23)) * texture_matrix[2]) + start_xy[X];
                                texpos_xy[Y] = (*((float*) (data+15)) * texture_matrix[3]) + (*((float*) (data+19)) * texture_matrix[4]) + (*((float*) (data+23)) * texture_matrix[5]) + start_xy[Y];
                                display_texpos(texpos_xy);
                                display_vertex((float*) data);
                            }
                            display_end();
                        }
                    }
                    else
                    {
                        // Non-enviroment map'd textured...
                        if(texture_flags & RENDER_EYE_FLAG)
                        {
                            // Texture is an eye texture...  That means its tex vertex locations are offset by the
                            // eye_quad_xy thing...
                            if(texture_flags & RENDER_CARTOON_FLAG)
                            {
                                if(texture_flags & RENDER_NO_LINE_FLAG)
                                {
                                    // Quick quads animation (for gears)...
                                    start_xy[X] = eye_quad_xy[(eye_frame>>1)&3][X];
                                    start_xy[Y] = eye_quad_xy[(eye_frame>>1)&3][Y];
                                }
                                else
                                {
                                    // Smoothly scrolling texture...
                                    start_xy[X] = 0.0f;
                                    start_xy[Y] = eye_frame * 0.03125f;
                                }
                            }
                            else
                            {
                                // Normal bounced animation...  0, 1, 2, 3, 2, 1...
                                eye_frame&=31;
                                start_xy[X] = eye_quad_xy[eye_frame_quad[eye_frame]][X];
                                start_xy[Y] = eye_quad_xy[eye_frame_quad[eye_frame]][Y];
                            }
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_strip();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    tex_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    data = tex_vertex_data + (tex_vertex<<3);
                                    texpos_xy[X] = (*((float*) data)) + start_xy[X];  data+=4;
                                    texpos_xy[Y] = (*((float*) data)) + start_xy[Y];  data+=4;
                                    display_texpos(texpos_xy);
                                    data = vertex_data + (vertex<<6);  display_vertex((float*) data);
                                }
                                display_end();
                            }


                            // Fans...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_fan();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    tex_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    data = tex_vertex_data + (tex_vertex<<3);
                                    texpos_xy[X] = (*((float*) data)) + start_xy[X];  data+=4;
                                    texpos_xy[Y] = (*((float*) data)) + start_xy[Y];  data+=4;
                                    display_texpos(texpos_xy);
                                    data = vertex_data + (vertex<<6);  display_vertex((float*) data);
                                }
                                display_end();
                            }

                        }
                        else
                        {
                            // Just a normal simple, everyday texture...
                            // Strips...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_strip();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    tex_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    data = tex_vertex_data + (tex_vertex<<3);  display_texpos((float*) data);
                                    data = vertex_data + (vertex<<6);  display_vertex((float*) data);
                                }
                                display_end();
                            }


                            // Fans...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_fan();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    tex_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                    data = tex_vertex_data + (tex_vertex<<3);  display_texpos((float*) data);
                                    data = vertex_data + (vertex<<6);  display_vertex((float*) data);
                                }
                                display_end();
                            }
                        }
                    }


                    // Turn culling back on...
                    if(texture_flags & RENDER_NOCULL_FLAG)
                    {
                        display_cull_on();
                    }

                    // Turn paperdoll off...
                    if(texture_flags & RENDER_PAPER_FLAG)
                    {
                        display_paperdoll_off();
                    }

                    // Turn z writes back on...
                    if(!(texture_flags & RENDER_LIGHT_FLAG) && (texture_alpha < 255))
                    {
                        display_zbuffer_write_on();
                    }
                }
                texture_file_data_block++;
            }











//            // Draw all visible cartoon lines (for partially transparent characters)
//#ifdef DEVTOOL
//            if(((key_down[SDLK_F12] && selection_view == VIEW_3D_XYZ) || (line_mode&1)) && main_alpha != 255)
//#else
//            if((line_mode&1) && main_alpha != 255)
//#endif
















            // Backface cartoon line stuff...  Looks much nicer...  No blipping...
#ifdef BACKFACE_CARTOON_LINES
            if(line_mode&1)
            {
                texture_data = texture_data_start;
                display_texture_off();
                display_cull_frontface();
                display_blend_off();
                display_color(line_color);
                repeat(i, MAX_DDD_TEXTURE)
                {
                    texture_mode = *texture_data;  texture_data++;
                    if(texture_mode != 0)
                    {
                        // Funky thing to make layer'd triangles not blip - since we don't do multitexturing...
                        display_depth_layer(i);

                        texture_flags = *texture_data;  texture_data++;
                        texture_alpha = *texture_data;  texture_data++;


                        // Which type of texturing are we doing?
                        if(texture_flags & RENDER_NO_LINE_FLAG)
                        {
                            // We're just skipping this texture...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                texture_data+=4*num_primitive_vertex;
                            }
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                texture_data+=4*num_primitive_vertex;
                            }
                        }
                        else
                        {
                            // Draw all of the silly backface things...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_strip();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                    data = vertex_data + (vertex<<6);
                                    distance_xyz[X] = ((float*) data)[X] + ((float*) (data+15))[X]*cartoon_line_size;
                                    distance_xyz[Y] = ((float*) data)[Y] + ((float*) (data+15))[Y]*cartoon_line_size;
                                    distance_xyz[Z] = ((float*) data)[Z] + ((float*) (data+15))[Z]*cartoon_line_size;
                                    display_vertex(distance_xyz);
                                }
                                display_end();
                            }


                            // Fans...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                display_start_fan();
                                repeat(k, num_primitive_vertex)
                                {
                                    vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                    data = vertex_data + (vertex<<6);
                                    distance_xyz[X] = ((float*) data)[X] + ((float*) (data+15))[X]*cartoon_line_size;
                                    distance_xyz[Y] = ((float*) data)[Y] + ((float*) (data+15))[Y]*cartoon_line_size;
                                    distance_xyz[Z] = ((float*) data)[Z] + ((float*) (data+15))[Z]*cartoon_line_size;
                                    display_vertex(distance_xyz);
                                }
                                display_end();
                            }
                        }
                    }
                    texture_file_data_block++;
                }
                display_texture_on();
                display_cull_on();
            }








#else
            // Draw cartoon lines
#ifdef DEVTOOL
            if((key_down[SDL_SCANCODE_F12] && selection_view == VIEW_3D_XYZ) || (line_mode&1))
#else
            if(line_mode&1)
#endif
            {
                // Cartoon line data start...
                bone_data += (num_bone*9);
                num_primitive = *((unsigned short*) bone_data);  bone_data+=2;


                // Clear out visibility count for each vertex...
                // Get onscreen coordinates of each vertex...  Stored in fourthbuffer...
                if(num_primitive > 0)
                {
                    vertex_bxy = (float*) fourthbuffer;
                    texture_data = vertex_data;
                    repeat(i, num_vertex)
                    {
                        // Proper onscreen coordinate figurin'
                        vertex_bxy[X] = onscreen_matrix[0] * ((float*) texture_data)[X] + onscreen_matrix[4] * ((float*) texture_data)[Y] + onscreen_matrix[8]  * ((float*) texture_data)[Z] + onscreen_matrix[12];
                        vertex_bxy[Y] = onscreen_matrix[1] * ((float*) texture_data)[X] + onscreen_matrix[5] * ((float*) texture_data)[Y] + onscreen_matrix[9]  * ((float*) texture_data)[Z] + onscreen_matrix[13];
                        dot           = onscreen_matrix[3] * ((float*) texture_data)[X] + onscreen_matrix[7] * ((float*) texture_data)[Y] + onscreen_matrix[11] * ((float*) texture_data)[Z] + onscreen_matrix[15];


                        if(dot != 0.0f)
                        {
                            vertex_bxy[X]/=dot; vertex_bxy[Y]/=dot;
                        }
                        texture_data+=64;
                        vertex_bxy+=2;
                    }
                    vertex_bxy-=num_vertex<<1;


                    // Determine visibility of each cartoon line...
                    texture_data = bone_data;
                    repeat(i, num_primitive)
                    {
                        // Find a perpendicular to the draw line...
                        vertex = (*((unsigned short*) (texture_data)))<<1;
                        perp_xy[Y] = -vertex_bxy[vertex];
                        vertex++;
                        perp_xy[X] = vertex_bxy[vertex];
                        vertex = (*((unsigned short*) (texture_data+2)))<<1;
                        start_xy[X] = vertex_bxy[vertex];
                        perp_xy[Y] += vertex_bxy[vertex];
                        vertex++;
                        start_xy[Y] = vertex_bxy[vertex];
                        perp_xy[X] -= vertex_bxy[vertex];


                        // Check each check point's location against the perpendicular...
                        vertex = (*((unsigned short*) (texture_data+4)))<<1;
                        texpos_xy[X] = vertex_bxy[vertex] - start_xy[X];
                        vertex++;
                        texpos_xy[Y] = vertex_bxy[vertex] - start_xy[Y];
                        dot = texpos_xy[X] * perp_xy[X] + texpos_xy[Y] * perp_xy[Y];
                        vertex = (*((unsigned short*) (texture_data+6)))<<1;
                        texpos_xy[X] = vertex_bxy[vertex] - start_xy[X];
                        vertex++;
                        texpos_xy[Y] = vertex_bxy[vertex] - start_xy[Y];

                        dot *= (texpos_xy[X] * perp_xy[X] + texpos_xy[Y] * perp_xy[Y]);
                        draw_cartoon_line[i] = (dot >= 0.0f);

                        texture_data+=8;
                    }



                    // Happy fat lines...
                    display_texture_off();
                    display_cull_off();
                    display_blend_off();
                    display_color(line_color);
                    repeat(i, num_primitive)
                    {
                        if(draw_cartoon_line[i])
                        {
                            // Find both points of line...  Not really texture stuff...
                            vertex = *((unsigned short*) bone_data);
                            data = vertex_data + (vertex<<6);
                            tex_vertex = *((unsigned short*) (bone_data+2));
                            texture_data = vertex_data + (tex_vertex<<6);


                            // Start drawing the polygon...
                            display_start_fan();


                            // First point...
                            vertex_bxy[X] = ((float*) data)[X];
                            vertex_bxy[Y] = ((float*) data)[Y];
                            vertex_bxy[Z] = ((float*) data)[Z];
                            display_vertex(vertex_bxy);


                            // Second point...
                            data+=15;
                            vertex_bxy[X] += ((float*) data)[X]*cartoon_line_size;
                            vertex_bxy[Y] += ((float*) data)[Y]*cartoon_line_size;
                            vertex_bxy[Z] += ((float*) data)[Z]*cartoon_line_size;
                            display_vertex(vertex_bxy);


                            // Third point...
                            vertex_bxy[X] = ((float*) texture_data)[X];
                            vertex_bxy[Y] = ((float*) texture_data)[Y];
                            vertex_bxy[Z] = ((float*) texture_data)[Z];
                            texture_data+=15;
                            vertex_bxy[X+3] = vertex_bxy[X] + ((((float*) texture_data)[X])*cartoon_line_size);
                            vertex_bxy[Y+3] = vertex_bxy[Y] + ((((float*) texture_data)[Y])*cartoon_line_size);
                            vertex_bxy[Z+3] = vertex_bxy[Z] + ((((float*) texture_data)[Z])*cartoon_line_size);
                            display_vertex(vertex_bxy+3);

                            // Fourth point
                            display_vertex(vertex_bxy);
                            display_end();
                        }
                        bone_data+=8;
                    }
                    display_cull_on();
                    display_texture_on();
                }
            }
#endif




#ifdef DEVTOOL
            // Draw all triangle lines (T key in modeler)...
            if(triangle_lines)
            {
                display_color(red);
                display_texture_off();
                texture_data = texture_data_start;
                repeat(i, MAX_DDD_TEXTURE)
                {
                    texture_mode = *texture_data;  texture_data++;
                    if(texture_mode != 0)
                    {
                        texture_data+=2;

                        // Strips...
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            display_start_line_loop();
                            repeat(k, num_primitive_vertex)
                            {
                                vertex = *((unsigned short*) texture_data);  texture_data+=4;
                                data = vertex_data + (vertex<<6);
                                display_vertex((float*) data);
                            }
                            display_end();
                        }

                        // Skip fans
                        texture_data+=2;
                    }
                }
                display_end();
                display_texture_on();
            }
            break;
    }
#endif
}

//-----------------------------------------------------------------------------------------------
#define SHADOW_Z_CORRECTION 0.100f
#define SHADOW_MESH_SIZE 5
#define SHADOW_MESH_END  4   // _SIZE-1...
float shadow_mesh_xyz[SHADOW_MESH_SIZE][SHADOW_MESH_SIZE][3];
float shadow_mesh_tex_xy[SHADOW_MESH_SIZE][SHADOW_MESH_SIZE][2];
void render_shadow_setup(void)
{
    // <ZZ> This function sets up the texture coordinates for the shadow meshes...
    unsigned short x, y;
    repeat(y, SHADOW_MESH_SIZE)
    {
        repeat(x, SHADOW_MESH_SIZE)
        {
            shadow_mesh_tex_xy[y][x][X] = ((float)x)/((float) (SHADOW_MESH_SIZE-1));
            shadow_mesh_tex_xy[y][x][Y] = ((float)y)/((float) (SHADOW_MESH_SIZE-1));
        }
    }
}

//-----------------------------------------------------------------------------------------------
void render_rdy_character_shadow(unsigned char* data, unsigned char* character_data, unsigned char main_alpha, float scale, float z)
{
    // <ZZ> This function draws an RDY object model's shadow.  This is one used ingame...
    //      Other one is for windows...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_joint;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* texture_data;
    unsigned char* frame_data;
    unsigned short i;
    unsigned char base_model, detail_level;
    unsigned char alpha;
    unsigned short frame;
    float vertex_xyz[3];
    float x, y;

    x = *((float*) (character_data));
    y = *((float*) (character_data+4));
    vertex_xyz[Z] = z+SHADOW_Z_CORRECTION;
    main_alpha = ((main_alpha) * (*(character_data+79))) >> 8;  // Modulate by character alpha...
    frame = *((unsigned short*) (character_data + 178));

    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) { frame = 0; }
    data+=(ACTION_MAX<<1);
    texture_data = data;
    data+=(MAX_DDD_SHADOW_TEXTURE);

    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);

    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    data = frame_data + 11 + (num_bone*24) + (num_joint*12);


    // Draw the shadow
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        // Shadow has several layers...
        alpha = *data;  data++;
        if(alpha)
        {
            alpha = (alpha * main_alpha) >> 8;
            shadow_color[3] = alpha;
            display_color_alpha(shadow_color);
            display_pick_texture(shadow_texture[texture_data[i]]);


            // Draw a simple flat shadow at the character's foot level...
            display_start_fan();
                display_texpos_xy(0.0f, 0.0f);
                vertex_xyz[X] = (((*((float*) (character_data+108)))*(*((float*) (data))) + (*((float*) (character_data+120)))*(*((float*) (data+4))))*scale) + x;
                vertex_xyz[Y] = (((*((float*) (character_data+112)))*(*((float*) (data))) + (*((float*) (character_data+124)))*(*((float*) (data+4))))*scale) + y;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(1.0f, 0.0f);
                vertex_xyz[X] = (((*((float*) (character_data+108)))*(*((float*) (data))) + (*((float*) (character_data+120)))*(*((float*) (data+4))))*scale) + x;
                vertex_xyz[Y] = (((*((float*) (character_data+112)))*(*((float*) (data))) + (*((float*) (character_data+124)))*(*((float*) (data+4))))*scale) + y;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(1.0f, 1.0f);
                vertex_xyz[X] = (((*((float*) (character_data+108)))*(*((float*) (data))) + (*((float*) (character_data+120)))*(*((float*) (data+4))))*scale) + x;
                vertex_xyz[Y] = (((*((float*) (character_data+112)))*(*((float*) (data))) + (*((float*) (character_data+124)))*(*((float*) (data+4))))*scale) + y;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(0.0f, 1.0f);
                vertex_xyz[X] = (((*((float*) (character_data+108)))*(*((float*) (data))) + (*((float*) (character_data+120)))*(*((float*) (data+4))))*scale) + x;
                vertex_xyz[Y] = (((*((float*) (character_data+112)))*(*((float*) (data))) + (*((float*) (character_data+124)))*(*((float*) (data+4))))*scale) + y;
                display_vertex(vertex_xyz);
                data+=8;
            display_end();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void render_rdy_shadow(unsigned char* data, unsigned short frame, float x, float y, float z, unsigned char mode)
{
    // <ZZ> This function draws an RDY object model's shadow.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_joint;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* texture_data;
    unsigned char* frame_data;
    unsigned short i;
    unsigned char base_model, detail_level;
    unsigned char alpha;
    float vertex_xyz[3];

    #ifdef DEVTOOL
        float onscreen_x, onscreen_y;
        unsigned short item, j;
    #endif


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    texture_data = data;
    data+=(MAX_DDD_SHADOW_TEXTURE);

    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);

    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;


    // Special tools for working with shadows...
    #ifdef DEVTOOL
        if((mode & 15) == 1)
        {
            // Select and modify points...
            if(selection_close_type == BORDER_SELECT)
            {
                data = frame_data + 11 + (num_bone*24) + (num_joint*12);
                repeat(i, MAX_DDD_SHADOW_TEXTURE)
                {
                    alpha = *data;  data++;
                    if(alpha)
                    {
                        repeat(j, 4)
                        {
                            item = (i<<4) + j;
                            if(!select_inlist(item))
                            {
                                render_get_point_xy(*((float*) (data))+x, *((float*) (data+4))+y, 0.0f, &onscreen_x, &onscreen_y);
                                if(onscreen_x > selection_box_tl[X] &&  onscreen_x < selection_box_br[X])
                                {
                                    if(onscreen_y > selection_box_tl[Y] &&  onscreen_y < selection_box_br[Y])
                                    {
                                        select_add(item, ((float*) data));
                                    }
                                }
                            }
                            data+=8;
                        }
                    }
                }
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_ALL)
            {
                data = frame_data + 11 + (num_bone*24) + (num_joint*12);
                repeat(i, MAX_DDD_SHADOW_TEXTURE)
                {
                    alpha = *data;  data++;
                    if(alpha)
                    {
                        repeat(j, 4)
                        {
                            item = (i<<4) + j;
                            if(!select_inlist(item))
                            {
                                select_add(item, ((float*) data));
                            }
                            data+=8;
                        }
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_INVERT)
            {
                data = frame_data + 11 + (num_bone*24) + (num_joint*12);
                repeat(i, MAX_DDD_SHADOW_TEXTURE)
                {
                    alpha = *data;  data++;
                    if(alpha)
                    {
                        repeat(j, 4)
                        {
                            item = (i<<4) + j;
                            if(!select_inlist(item))
                            {
                                select_add(item, ((float*) data));
                            }
                            else
                            {
                                // Items with a NULL data get removed later
                                select_data[select_index] = NULL;
                            }
                            data+=8;
                        }
                    }
                }
                repeat(i, select_num)
                {
                    if(select_data[i] == NULL)
                    {
                        select_remove(i);
                        i--;
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_SPECIAL_SELECT_CONNECTED)
            {
                data = frame_data + 11 + (num_bone*24) + (num_joint*12);
                repeat(i, MAX_DDD_SHADOW_TEXTURE)
                {
                    alpha = *data;  data++;
                    if(alpha)
                    {
                        item = i<<4;
                        if(select_inlist(item) || select_inlist((unsigned short) (item+1)) || select_inlist((unsigned short) (item+2)) || select_inlist((unsigned short) (item+3)))
                        {
                            repeat(j, 4)
                            {
                                item = (i<<4) + j;
                                if(!select_inlist(item))
                                {
                                    select_add(item, ((float*) data));
                                }
                                data+=8;
                            }
                        }
                        else
                        {
                            data+=32;
                        }
                    }
                    else
                    {
                        data+=32;
                    }
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_MOVE)
            {
                if(select_move_on)
                {
                    render_get_point_xyz(mouse_x, mouse_y, &select_offset_xyz[X], &select_offset_xyz[Y], &onscreen_y);
                    render_tex_move(FALSE);
                }
                else
                {
                    render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &select_center_xyz[Y], &onscreen_y);
                    select_update_xy();
                }
                selection_close_type = 0;
            }
            else if(selection_close_type == BORDER_CROSS_HAIRS)
            {
                render_get_point_xyz(mouse_x, mouse_y, &select_center_xyz[X], &select_center_xyz[Y], &onscreen_y);
                selection_close_type = 0;
            }


            // Draw the shadow vertices
            display_texture_off();
            data = frame_data + 11 + (num_bone*24) + (num_joint*12);
            repeat(i, MAX_DDD_SHADOW_TEXTURE)
            {
                alpha = *data;  data++;
                if(alpha)
                {
                    repeat(j, 4)
                    {
                        item = (i<<4) + j;
                        if(select_inlist(item))
                        {
                            display_marker(white, *((float*) (data))+x, *((float*) (data+4))+y, z, 0.05f);
                        }
                        else
                        {
                            display_marker(blue, *((float*) (data))+x, *((float*) (data+4))+y, z, 0.05f);
                        }
                        data+=8;
                    }
                }
            }
        }
        display_texture_on();
    #endif


    display_zbuffer_write_off();


    // Draw the shadow
    data = frame_data + 11 + (num_bone*24) + (num_joint*12);
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        alpha = *data;  data++;
        if(alpha)
        {
            shadow_color[3] = alpha;
            display_color_alpha(shadow_color);
            display_pick_texture(shadow_texture[texture_data[i]]);
            display_start_fan();
                display_texpos_xy(0.0f, 0.0f);
                vertex_xyz[X] = (script_matrix[0])*(*((float*) (data))) + (script_matrix[3])*(*((float*) (data+4))) + x;
                vertex_xyz[Y] = (script_matrix[1])*(*((float*) (data))) + (script_matrix[4])*(*((float*) (data+4))) + y;
                vertex_xyz[Z] = z;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(1.0f, 0.0f);
                vertex_xyz[X] = (script_matrix[0])*(*((float*) (data))) + (script_matrix[3])*(*((float*) (data+4))) + x;
                vertex_xyz[Y] = (script_matrix[1])*(*((float*) (data))) + (script_matrix[4])*(*((float*) (data+4))) + y;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(1.0f, 1.0f);
                vertex_xyz[X] = (script_matrix[0])*(*((float*) (data))) + (script_matrix[3])*(*((float*) (data+4))) + x;
                vertex_xyz[Y] = (script_matrix[1])*(*((float*) (data))) + (script_matrix[4])*(*((float*) (data+4))) + y;
                display_vertex(vertex_xyz);
                data+=8;

                display_texpos_xy(0.0f, 1.0f);
                vertex_xyz[X] = (script_matrix[0])*(*((float*) (data))) + (script_matrix[3])*(*((float*) (data+4))) + x;
                vertex_xyz[Y] = (script_matrix[1])*(*((float*) (data))) + (script_matrix[4])*(*((float*) (data+4))) + y;
                display_vertex(vertex_xyz);
                data+=8;


//                display_texpos_xy(0.0f, 0.0f);
//                display_vertex_xyz(*((float*) (data))+x, *((float*) (data+4))+y, z);
//                data+=8;

//                display_texpos_xy(1.0f, 0.0f);
//                display_vertex_xyz(*((float*) (data))+x, *((float*) (data+4))+y, z);
//                data+=8;

//                display_texpos_xy(1.0f, 1.0f);
//                display_vertex_xyz(*((float*) (data))+x, *((float*) (data+4))+y, z);
//                data+=8;

//                display_texpos_xy(0.0f, 1.0f);
//                display_vertex_xyz(*((float*) (data))+x, *((float*) (data+4))+y, z);
//                data+=8;
            display_end();
        }
    }


    display_zbuffer_write_on();
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned short* last_triangle_added_data;
unsigned char render_insert_triangle(unsigned char* data, unsigned short frame, unsigned short insert_mode, unsigned char texture)
{
    // <ZZ> This function adds a triangle to a given model, using the first three selected vertices
    //      for placement.  If insert_mode is FALSE, it removes the given triangle.  A return
    //      value of TRUE means that it worked, FALSE means it didn't.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned char* temp_data;
    unsigned short i, j, k;
    unsigned char texture_mode;
    unsigned short num_primitive, num_primitive_vertex;
    int amount_added;
    unsigned char base_model, detail_level;


    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    if(texture >= MAX_DDD_TEXTURE) return FALSE;
    if(select_num < 3) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    data+=8;
    texture_data = *((unsigned char**) data);  data+=4;



    // Walk through texture data until we come to the appropriate texture
    repeat(i, texture)
    {
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            texture_data+=2;

            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(j, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                repeat(k, num_primitive_vertex)
                {
                    texture_data+=4;
                }
            }


            // Fans...  Shouldn't be any in DEVTOOL mode...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(j, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                repeat(k, num_primitive_vertex)
                {
                    texture_data+=4;
                }
            }
        }
    }



    // Pick the type of operation
    amount_added = 0;
    if(insert_mode)
    {
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            // Texture is turned on...
            texture_data+=2;


            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            if(sdf_insert_data(texture_data, NULL, 14))
            {
                num_primitive++;
                *((unsigned short*) (texture_data-2)) = num_primitive;
                last_triangle_added_data = ((unsigned short*) texture_data);
                *((unsigned short*) (texture_data)) = 3;
                *((unsigned short*) (texture_data+2))  = select_list[0];
                *((unsigned short*) (texture_data+4))  = 0;
                *((unsigned short*) (texture_data+6))  = select_list[1];
                *((unsigned short*) (texture_data+8))  = 0;
                *((unsigned short*) (texture_data+10)) = select_list[2];
                *((unsigned short*) (texture_data+12)) = 0;
                amount_added = 14;
            }
        }
        else
        {
            // Need to turn texture on
            if(sdf_insert_data(texture_data, NULL, 20))
            {
                *(texture_data-1) = 1;                                          // Texture mode
                *(texture_data) = 0;                                            // Flags
                *(texture_data+1) = 255;                                        // Alpha
                *((unsigned short*) (texture_data+2))  = 1;                     // Number of strips
                last_triangle_added_data = ((unsigned short*) (texture_data+4));
                *((unsigned short*) (texture_data+4))  = 3;                     // Number of vertices
                *((unsigned short*) (texture_data+6))  = select_list[0];
                *((unsigned short*) (texture_data+8))  = 0;
                *((unsigned short*) (texture_data+10))  = select_list[1];
                *((unsigned short*) (texture_data+12))  = 0;
                *((unsigned short*) (texture_data+14)) = select_list[2];
                *((unsigned short*) (texture_data+16)) = 0;
                *((unsigned short*) (texture_data+18)) = 0;                     // Number of fans
                amount_added = 20;
            }
        }
    }
    else
    {
        // Deleting...
        temp_data = texture_data;
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            // Texture is turned on...
            texture_data+=2;


            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(i, num_primitive)
            {
                amount_added = 0; 
                num_primitive_vertex = *((unsigned short*) (texture_data));  texture_data+=2;                
                repeat(j, num_primitive_vertex)
                {
                    if(*((unsigned short*) (texture_data)) == select_list[0] || *((unsigned short*) (texture_data)) == select_list[1] || *((unsigned short*) (texture_data)) == select_list[2])
                    {
                        amount_added++;
                    }
                    texture_data+=4;
                }
                if(amount_added == num_primitive_vertex)
                {
                    // Found a match...  Delete it...
                    if(num_primitive == 1)
                    {
                        // Last one for this texture...  Delete the whole thing...  Assume there aren't any fans...
                        if(sdf_insert_data(temp_data, NULL, -8-(num_primitive_vertex<<2)))
                        {
                            *temp_data = 0;  // Turn off the texture
                            amount_added = -8-(num_primitive_vertex<<2);
                        }
                    }
                    else
                    {
                        // Delete the strip and down the counter
                        texture_data -= (num_primitive_vertex<<2)+2;
                        if(sdf_insert_data(texture_data, NULL, -2-(num_primitive_vertex<<2)))
                        {
                            *((unsigned short*) (temp_data+3)) = num_primitive-1;  // Down the count for number of strips
                            amount_added = -2-(num_primitive_vertex<<2);
                        }
                    }
                    i = num_primitive;
                }
                else
                {
                    // No match yet
                    amount_added = 0;
                }
            }
        }
    }



    if(amount_added != 0)
    {
        // Update all base model pointers at start of file
        *((unsigned char**) data) += amount_added;  data+=4;
        *((unsigned char**) data) += amount_added;  data+=4;
        base_model++;
        while(base_model < num_base_model)
        {
            *((unsigned char**) data) += amount_added;  data+=4;
            *((unsigned char**) data) += amount_added;  data+=4;
            *((unsigned char**) data) += amount_added;  data+=4;
            *((unsigned char**) data) += amount_added;  data+=4;
            *((unsigned char**) data) += amount_added;  data+=4;
            base_model++;
        }


        // Update bone frame pointers, only if bone frames are internal to this file (not linked to another RDY)
        if((flags & DDD_EXTERNAL_BONE_FRAMES) == 0)
        {
            repeat(i, num_bone_frame)
            {
                *((unsigned char**) data) += amount_added;  data+=4;
            }
        }
        return TRUE;
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_vertex(unsigned char* data, unsigned short frame, float* vertex_xyz, unsigned short vertex_to_remove, unsigned short vertex_replacement)
{
    // <ZZ> This function adds a vertex to a given model.  Returns TRUE if it worked.  If vertex_xyz
    //      is NULL, the function removes a vertex instead (and associated triangles).  Vertex_replacement
    //      is used to replace a vertex (with a lower valued one) instead of deleting associated triangles.
    //      A value of MAX_VERTEX means to delete the triangles...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex, num_tex_vertex, num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned char* temp_data;
    unsigned char* start_data;
    unsigned char* bone_data;
    unsigned short i, j, k;
    unsigned char texture_mode;
    unsigned short amount_removed, num_primitive, num_primitive_vertex, vertex;
    unsigned char base_model, detail_level;
    unsigned char found_match;



    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    bone_data =  *((unsigned char**) (data+12));

    num_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_tex_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(vertex_xyz != NULL)
    {
        // Insert a vertex
        if(sdf_insert_data(base_model_data+(num_vertex<<6), NULL, 64))
        {
            // Added the point successfully...  Write coordinates
            select_add(num_vertex, ((float*) (base_model_data+(num_vertex<<6))));
            *((float*) (base_model_data+(num_vertex<<6))) = vertex_xyz[X];
            *((float*) (base_model_data+4+(num_vertex<<6))) = vertex_xyz[Y];
            *((float*) (base_model_data+8+(num_vertex<<6))) = vertex_xyz[Z];


            // Default bone info
            *(base_model_data+12+(num_vertex<<6)) = 0;
            *(base_model_data+13+(num_vertex<<6)) = 0;
//            *(base_model_data+14+(num_vertex<<6)) = 128;
            *(base_model_data+14+(num_vertex<<6)) = 64;  // !!!ANCHOR!!!


            // Update number of vertices for this base model
            num_vertex++;
            *((unsigned short*) (base_model_data-8) ) = num_vertex;


            // Update all base model pointers at start of file
            *((unsigned char**) data) += 64;  data+=4;
            *((unsigned char**) data) += 64;  data+=4;
            *((unsigned char**) data) += 64;  data+=4;
            *((unsigned char**) data) += 64;  data+=4;
            base_model++;
            while(base_model < num_base_model)
            {
                *((unsigned char**) data) += 64;  data+=4;
                *((unsigned char**) data) += 64;  data+=4;
                *((unsigned char**) data) += 64;  data+=4;
                *((unsigned char**) data) += 64;  data+=4;
                *((unsigned char**) data) += 64;  data+=4;
                base_model++;
            }


            // Update bone frame pointers, only if bone frames are internal to this file (not linked to another RDY)
            if((flags & DDD_EXTERNAL_BONE_FRAMES) == 0)
            {
                repeat(i, num_bone_frame)
                {
                    *((unsigned char**) data) += 64;  data+=4;
                }
            }


            // Calculate scalars and weight and attachments...
            render_attach_vertex_to_bone(start_data, frame, (unsigned short) (num_vertex-1));
            render_crunch_vertex(start_data, frame, (unsigned short) (num_vertex-1), TRUE, 0);
            return TRUE;
        }
    }
    else
    {
        // Delete a given vertex
        if(vertex_to_remove < num_vertex)
        {
            if(sdf_insert_data(base_model_data+(vertex_to_remove<<6), NULL, -64))
            {
                // Deleted the vertex successfully...  Update number of vertices for this base model
                num_vertex--;
                *((unsigned short*) (base_model_data-8) ) = num_vertex;
                amount_removed = 64;


                // Delete all connected textures...
                texture_data = base_model_data+(num_vertex<<6);
                texture_data += (num_tex_vertex<<3);


                repeat(i, MAX_DDD_TEXTURE)
                {
                    texture_mode = *texture_data;  texture_data++;
                    if(texture_mode != 0)
                    {
                        texture_data+=2;

                        // Strips...
                        temp_data = texture_data;
                        num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                        repeat(j, num_primitive)
                        {
                            found_match = FALSE;
                            num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(k, num_primitive_vertex)
                            {
                                vertex = *((unsigned short*) texture_data);
                                if(vertex == vertex_to_remove) found_match = TRUE;
                                if(vertex > vertex_to_remove)
                                {
                                    *((unsigned short*) texture_data) = vertex-1;
                                }
                                if(found_match && vertex_replacement != MAX_VERTEX)
                                {
                                    // Don't really delete the triangle, just replace the deleted vertex with this one...
                                    *((unsigned short*) texture_data) = vertex_replacement;
                                    found_match = FALSE;
                                }
                                texture_data+=4;
                            }
                            if(found_match)
                            {
                                // This triangle uses the vertex...  Delete it...
                                if(sdf_insert_data(texture_data-2-(num_primitive_vertex<<2), NULL, -2-(num_primitive_vertex<<2)))
                                {
                                    amount_removed+=2+(num_primitive_vertex<<2);
                                    texture_data-=2+(num_primitive_vertex<<2);
                                    num_primitive--;  j--;
                                }
                            }
                        }
                        *((unsigned short*) temp_data) = num_primitive;
                        if(num_primitive == 0)
                        {
                            // Whole texture has been deleted
                            if(sdf_insert_data(temp_data-2, NULL, -6))
                            {
                                *(temp_data-3) = 0;
                                texture_data = temp_data-2;
                                amount_removed+=6;
                            }    
                        }
                        else
                        {
                            // Fans...  Shouldn't be any in DEVTOOL mode...
                            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                            repeat(j, num_primitive)
                            {
                                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                                repeat(k, num_primitive_vertex)
                                {
                                    texture_data+=4;
                                }
                            }
                        }
                    }
                }




                // Update all base model pointers at start of file
                *((unsigned char**) data) -= 64;  data+=4;
                *((unsigned char**) data) -= 64;  data+=4;
                *((unsigned char**) data) -= amount_removed;  data+=4;
                *((unsigned char**) data) -= amount_removed;  data+=4;
                base_model++;
                while(base_model < num_base_model)
                {
                    *((unsigned char**) data) -= amount_removed;  data+=4;
                    *((unsigned char**) data) -= amount_removed;  data+=4;
                    *((unsigned char**) data) -= amount_removed;  data+=4;
                    *((unsigned char**) data) -= amount_removed;  data+=4;
                    *((unsigned char**) data) -= amount_removed;  data+=4;
                    base_model++;
                }


                // Update bone frame pointers, only if bone frames are internal to this file (not linked to another RDY)
                if((flags & DDD_EXTERNAL_BONE_FRAMES) == 0)
                {
                    repeat(i, num_bone_frame)
                    {
                        *((unsigned char**) data) -= amount_removed;  data+=4;
                    }
                }
                return TRUE;
            }

        }
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
int num_copy_vertex = 0;
int num_copy_tex_vertex = 0;
int num_copy_triangle[MAX_DDD_TEXTURE];
float copy_vertex_xyz[MAX_VERTEX][3];
float copy_tex_vertex_xy[MAX_TEX_VERTEX][2];
unsigned short copy_tex_vertex_number[MAX_TEX_VERTEX];
unsigned short copy_triangle_vertices[MAX_DDD_TEXTURE][MAX_TRIANGLE][3];
unsigned short copy_triangle_tex_vertices[MAX_DDD_TEXTURE][MAX_TRIANGLE][3];
void render_copy_selected(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function copies selected points, tex vertices, and triangles to a
    //      temporary buffer area, so it can be pasted in later.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex;
    unsigned char* base_model_data;
    float* tex_vertex_data;
    unsigned char* texture_data;
    unsigned char* frame_data;
    unsigned short i, j, k, m;
    unsigned char base_model;
    unsigned short vertex[4];
    unsigned short tex_vertex[4];
    unsigned char tex_vertex_new[4];
    unsigned char texture_mode;
    unsigned short num_primitive;
    unsigned short num_primitive_vertex;
    unsigned char found_match;


    // Clear out the last copy
    num_copy_vertex = 0;
    num_copy_tex_vertex = 0;
    repeat(i, MAX_DDD_TEXTURE)
    {
        num_copy_triangle[i] = 0;
    }


    // Read the rdy header
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    num_vertex = *((unsigned short*) (base_model_data));
    tex_vertex_data = *((float**) (data+4));
    texture_data = *((unsigned char**) (data+8));


    // Save the selected vertex coordinates
    repeat(i, select_num)
    {
        copy_vertex_xyz[i][X] = select_xyz[i][X];
        copy_vertex_xyz[i][Y] = select_xyz[i][Y];
        copy_vertex_xyz[i][Z] = select_xyz[i][Z];
    }
    num_copy_vertex = select_num;


    // Save the selected triangles, and related tex vertices as we go
    repeat(i, MAX_DDD_TEXTURE)
    {
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            texture_data+=2;

            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(j, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                // See if it's selected...
                found_match = TRUE;
                repeat(k, num_primitive_vertex)
                {
                    vertex[k&3] = *((unsigned short*) texture_data);  texture_data+=2;
                    tex_vertex[k&3] = *((unsigned short*) texture_data);  texture_data+=2;
                    if(select_inlist(vertex[k&3]))
                    {
                        vertex[k&3] = select_index;
                        tex_vertex_new[k&3] = TRUE;
                        repeat(m, num_copy_tex_vertex)
                        {
                            if(tex_vertex[k&3] == copy_tex_vertex_number[m])
                            {
                                // Tex vertex has already been copied...
                                tex_vertex[k&3] = m;
                                m = num_copy_tex_vertex;
                                tex_vertex_new[k&3] = FALSE;
                            }
                        }
                    }
                    else found_match = FALSE;
                }
                if(found_match)
                {
                    // Add any new tex vertices...
                    repeat(k, 3)
                    {
                        if(tex_vertex_new[k])
                        {
                            copy_tex_vertex_xy[num_copy_tex_vertex][X] = *(tex_vertex_data + (tex_vertex[k]<<1));
                            copy_tex_vertex_xy[num_copy_tex_vertex][Y] = *(tex_vertex_data + (tex_vertex[k]<<1) + 1);
                            copy_tex_vertex_number[num_copy_tex_vertex] = tex_vertex[k];
                            tex_vertex[k] = num_copy_tex_vertex;
                            num_copy_tex_vertex++;
                        }
                    }


                    // Copy this triangle...
                    repeat(k, 3)
                    {
                        copy_triangle_vertices[i][num_copy_triangle[i]][k] = vertex[k];
                        copy_triangle_tex_vertices[i][num_copy_triangle[i]][k] = tex_vertex[k];
                    }
                    num_copy_triangle[i]++;
                }
            }


            // Fans...  Shouldn't be any in DEVTOOL mode...
            texture_data+=2;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_paste_selected(unsigned char* data, unsigned short frame, unsigned char start_texture)
{
    // <ZZ> This function pastes data that had been copied back into a model...  If start_texture
    //      is > 0, the textures are rotated up...  For making multitexturing easier...
    unsigned char num_base_model;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char* frame_data;
    float* tex_vertex_data;
    unsigned short i, j;
    unsigned char base_model;
    unsigned short first_vertex;
    unsigned short first_tex_vertex;


    // Read the rdy header to find out our starting vertices...
    start_data = data;
    data+=3;
    num_base_model = *data;  data+=3;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    tex_vertex_data = *((float**) (data+4));
    first_vertex = *((unsigned short*) (base_model_data));
    first_tex_vertex = *((unsigned short*) (base_model_data+2));
    base_model_data+=8;


    // Add all new vertices
    repeat(i, num_copy_vertex)
    {
        if(render_insert_vertex(start_data, frame, copy_vertex_xyz[i], 0, MAX_VERTEX) == FALSE) return;
    }


    // Add all new tex vertices
    repeat(i, num_copy_tex_vertex)
    {
        if(render_insert_tex_vertex(start_data, frame, copy_tex_vertex_xy[i], 0, MAX_TEX_VERTEX) == FALSE) return;
    }


    // Add all new triangles
    repeat(i, MAX_DDD_TEXTURE)
    {
        repeat(j, num_copy_triangle[i])
        {
            select_clear();
            select_add((unsigned short) (copy_triangle_vertices[i][j][0]+first_vertex), (float*)(base_model_data + ((copy_triangle_vertices[i][j][0]+first_vertex)<<6)));
            select_add((unsigned short) (copy_triangle_vertices[i][j][1]+first_vertex), (float*)(base_model_data + ((copy_triangle_vertices[i][j][1]+first_vertex)<<6)));
            select_add((unsigned short) (copy_triangle_vertices[i][j][2]+first_vertex), (float*)(base_model_data + ((copy_triangle_vertices[i][j][2]+first_vertex)<<6)));
            if(render_insert_triangle(start_data, frame, TRUE, (unsigned char) ((i+start_texture)&3)) == FALSE) return;
            else
            {
                *(last_triangle_added_data+2) = copy_triangle_tex_vertices[i][j][0]+first_tex_vertex;
                *(last_triangle_added_data+4) = copy_triangle_tex_vertices[i][j][1]+first_tex_vertex;
                *(last_triangle_added_data+6) = copy_triangle_tex_vertices[i][j][2]+first_tex_vertex;
            }
        }
    }


    // Now select the newly pasted vertices...
    select_clear();
    repeat(i, num_copy_vertex)
    {
        select_add((unsigned short) (i+first_vertex), (float*)(base_model_data + ((i+first_vertex)<<6)));
    }
}
#endif


//-----------------------------------------------------------------------------------------------
void render_fill_temp_character_bone_number(unsigned char* data)
{
    // <ZZ> This function helps us find a bone by a given name, by building us a cute little table...
    unsigned char num_base_model;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* bone_data;
    unsigned char* frame_data;
    unsigned short i;
    unsigned char base_model;
    unsigned short frame;



    data+=3;
    num_base_model = *data;  data+=3;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame = 0;
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data = *((unsigned char**) data);
    base_model_data+=6;
    num_bone = *((unsigned short*) base_model_data);


    // Find the bone numbers for named bones...  Left, Right, Left2, Right2, Saddle...
    repeat(i, 8)
    {
        temp_character_bone_number[i] = 255;
    }
    repeat(i, num_bone)
    {
        temp_character_bone_number[(*(bone_data+(i*9))) & 7] = (unsigned char) i;
    }
}


//-----------------------------------------------------------------------------------------------
unsigned char render_bone_id(unsigned char* data, unsigned short frame, unsigned short joint_one, unsigned short joint_two, unsigned char bone_id)
{
    // <ZZ> This function sets the id of a given bone.  If the id is non-zero, the length is set
    //      to 1.0f, as it's being used as a weapon grip or a saddle.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* frame_data_start;
    unsigned char* bone_data;
    unsigned short i;
    unsigned char base_model;




    flags = *((unsigned short*) data); data+=2;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return FALSE;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data_start = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data_start+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);  data+=16;
    bone_data =  *((unsigned char**) (data));

    base_model_data+=6;
    num_bone = *((unsigned short*) base_model_data);



    // Look for a matching joint pair
    repeat(i, num_bone)
    {
        if(joint_one == *((unsigned short*) (bone_data+1)) || joint_one == *((unsigned short*) (bone_data+3)))
        {
            if(joint_two == *((unsigned short*) (bone_data+1)) || joint_two == *((unsigned short*) (bone_data+3)))
            {
                // Write the ID and length
                *bone_data = bone_id;
                if(bone_id != 0)
                {
                    *((float*) (bone_data+5)) = 1.0f;
                }
                i = num_bone;
            }
        }
        bone_data+=9;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_bone(unsigned char* data, unsigned short frame, unsigned short joint_one, unsigned short joint_two, unsigned char insert_bone, unsigned char bone_id)
{
    // <ZZ> This function adds a bone to a given model.  Returns TRUE if it worked.  If insert_bone
    //      is FALSE, the function removes a bone instead.  Recalculates all vertex attachments
    //      afterwards.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex, num_tex_vertex, num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* frame_data_start;
    unsigned char* start_data;
    unsigned char* bone_data;
    unsigned char* joint_data;
    unsigned short i, j;
    unsigned char base_model, detail_level;
    int amount_to_add;
    unsigned char single_joint;
    unsigned char remove_bone[MAX_BONE];
    unsigned char removed_all_bones;



    single_joint = FALSE;
    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return FALSE;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data_start = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data_start+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    joint_data =  *((unsigned char**) (data+8));
    bone_data =  *((unsigned char**) (data+12));

    num_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_tex_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(insert_bone)
    {
        // Insert the bone
        if(joint_one >= num_joint)  return FALSE;
        if(joint_two >= num_joint)  return FALSE;
        if(num_bone == 255) return FALSE;



        // Go through all frame data looking for matching base models to add bone normals...
        repeat(i, num_bone_frame)
        {
            frame_data = *((unsigned char**) (frame_data_start+(i<<2)));
            j = *(frame_data+2);  // Base model for this frame...
            if(j == base_model)
            {
                // Need to update this frame...  Insert mocked up normals...
                frame_data += 11;
                frame_data += (num_bone*24);
                if(sdf_insert_data(frame_data, NULL, 24))
                {
                    // Put in the normal info...
                    *((float*) (frame_data)) = 0;
                    *((float*) (frame_data+4)) = 1;
                    *((float*) (frame_data+8)) = 0;
                    *((float*) (frame_data+12)) = 1;
                    *((float*) (frame_data+16)) = 0;
                    *((float*) (frame_data+20)) = 0;


                    // Update frame data pointers...
                    j = i+1;
                    while(j < num_bone_frame)
                    {
                        *((unsigned char**) (frame_data_start+(j<<2))) += 24;
                        j++;
                    }
                }
            }
        }



        bone_data+=(num_bone*9);
        if(sdf_insert_data(bone_data, NULL, 9))
        {
            // Write the ID and bone attachments
            *bone_data = bone_id;
            *((unsigned short*) (bone_data+1)) = joint_one;
            *((unsigned short*) (bone_data+3)) = joint_two;


            // Update number of bones for this base model
            num_bone++;
            *((unsigned short*) (base_model_data-2) ) = num_bone;
            amount_to_add = 9;
        }
    }
    else
    {
        // Delete a bone which is described by a pair of joints, or delete all bones touching a single joint
        if(joint_one >= num_joint && joint_two >= num_joint) return FALSE;
        single_joint = (joint_one >= num_joint || joint_two >= num_joint);
        if(single_joint && joint_one >= num_joint)  joint_one = joint_two;

log_message("INFO:   Prior bone list, Removing joint %d", joint_one);

        // Make a list of the bones we need to remove
        removed_all_bones = TRUE;
        amount_to_add = 0;
        repeat(i, num_bone)
        {
            remove_bone[i] = FALSE;
log_message("INFO:     Bone %d, Joint %d to %d", i, *((unsigned short*) (bone_data + (i*9) + 1)), *((unsigned short*) (bone_data + (i*9) + 3)));

            if(single_joint)
            {
                if(*((unsigned short*) (bone_data + (i*9) + 1)) == joint_one || *((unsigned short*) (bone_data + (i*9) + 3)) == joint_one)
                {
                    if(amount_to_add < (num_bone-1))
                    {
                        remove_bone[i] = TRUE;
                        amount_to_add++;
                    }
                    else
                    {
log_message("INFO:     Cannot remove bone %d", i);
                        removed_all_bones = FALSE;
                    }
                }
            }
            else
            {
                if((*((unsigned short*) (bone_data + (i*9) + 1)) == joint_one && *((unsigned short*) (bone_data + (i*9) + 3)) == joint_two) || (*((unsigned short*) (bone_data + (i*9) + 3)) == joint_one && *((unsigned short*) (bone_data + (i*9) + 1)) == joint_two))
                {
                    if(amount_to_add < (num_bone-1))
                    {
                        remove_bone[i] = TRUE;
                        amount_to_add++;
                    }
                    else
                    {
log_message("INFO:     Cannot remove bone %d", i);
                        removed_all_bones = FALSE;
                    }
                }
            }
        }




        // Go through all frame data looking for matching base models to remove bone normals...  Do backwards...
        repeat(i, num_bone_frame)
        {
            frame_data = *((unsigned char**) (frame_data_start+(i<<2)));
            j = *(frame_data+2);  // Base model for this frame...
            if(j == base_model)
            {
                // Need to update this frame...
                amount_to_add = 0;
                frame_data += 11;
                frame_data += (num_bone*24);
                j = num_bone;
                while(j > 0)
                {
                    j--;
                    frame_data-=24;
                    if(remove_bone[j])
                    {
                        if(sdf_insert_data(frame_data, NULL, -24))
                        {
                            amount_to_add-=24;
                        }
                    }
                }



                // Update frame data pointers...
                j = i+1;
                while(j < num_bone_frame)
                {
                    *((unsigned char**) (frame_data_start+(j<<2))) += amount_to_add;
                    j++;
                }
            }
        }



        // Remove the bones we found earlier (do backwards to keep numbering correct)
        // Renumber joints if single_joint mode, because that only happens when a joint is removed...
        j = num_bone;
        bone_data += (num_bone*9);
        amount_to_add = 0;
        while(j > 0)
        {
            j--;
            bone_data-=9;
            if(remove_bone[j])
            {
                if(sdf_insert_data(bone_data, NULL, -9))
                {
                    amount_to_add-=9;
                    num_bone--;
                }
            }
            else if(single_joint && removed_all_bones)
            {
                // Mark down higher numbered joints
                if(*((unsigned short*) (bone_data+1)) > joint_one)
                {
                    *((unsigned short*) (bone_data+1)) += -1;
                }
                if(*((unsigned short*) (bone_data+3)) > joint_one)
                {
                    *((unsigned short*) (bone_data+3)) += -1;
                }
            }
        }


log_message("INFO:   Removed %d bones", amount_to_add/-9);



// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
log_message("INFO:   Final bone list, %d bones, Removed joint %d", num_bone, joint_one);
repeat(j, num_bone)
{
    log_message("INFO:     Bone %d, Joint %d to %d", j, *((unsigned short*) (bone_data + 1)), *((unsigned short*) (bone_data + 3)));
    bone_data+=9;
}
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!


        // Write the new number of bones...
        *((unsigned short*) (base_model_data-2)) = num_bone;
        if(amount_to_add == 0 && removed_all_bones)
        {
            return TRUE;
        }
    }



    if(amount_to_add != 0)
    {
        // Update all base model pointers at start of file
        data+=16;
        base_model++;
        while(base_model < num_base_model)
        {
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            *((unsigned char**) data) += amount_to_add;  data+=4;
            base_model++;
        }


        // Update bone frame pointers...
        repeat(i, num_bone_frame)
        {
            *((unsigned char**) data) += amount_to_add;  data+=4;
        }


        // Must recalculate vertex attachments and scalars...
        if(insert_bone)
        {
            num_bone--;
            render_crunch_bone(start_data, frame, num_bone, 0);
        }
        render_generate_bone_normals(start_data, frame);
        repeat(i, num_vertex)
        {
            render_attach_vertex_to_bone(start_data, frame, i);
            render_crunch_vertex(start_data, frame, i, TRUE, 0);
        }
        return removed_all_bones;
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_joint(unsigned char* data, unsigned short frame, float* joint_xyz, unsigned short joint_to_remove, unsigned char joint_size)
{
    // <ZZ> This function adds a joint to a given model.  Returns TRUE if it worked.  If joint_xyz
    //      is NULL, the function removes a joint instead (and associated bones).
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex, num_tex_vertex, num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* frame_data_start;
    unsigned char* start_data;
    unsigned char* bone_data;
    unsigned char* joint_data;
    unsigned short i, j;
    unsigned char base_model, detail_level;
    unsigned char bone_delete_okay;


    // Remove bones attached to joint, if we're deleting a joint...
log_message("INFO:   ");
log_message("INFO:   ");
log_message("INFO:   ");
    start_data = data;
    if(joint_xyz == NULL)
    {
        // Do up here so our pointers don't get messed up later...
        bone_delete_okay = render_insert_bone(start_data, frame, joint_to_remove, 1024, FALSE, 0);
        if(bone_delete_okay)
        {
            log_message("INFO:   Insert bone said it was okay to delete a joint");
        }
    }



    flags = *((unsigned short*) data); data+=2;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return FALSE;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Check filesize to make sure we have room...  Offset of last frame data to find...
// !!!BAD!!!



    // Go to the current base model
    frame_data_start = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data_start+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    joint_data =  *((unsigned char**) (data+8));
    bone_data =  *((unsigned char**) (data+12));

    num_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_tex_vertex = *((unsigned short*) base_model_data); base_model_data+=2;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(joint_xyz != NULL)
    {
        // Insert a joint...  Go through all frame data looking for matching base models to add joint position...
        repeat(i, num_bone_frame)
        {
            frame_data = *((unsigned char**) (frame_data_start+(i<<2)));
            j = *(frame_data+2);  // Base model for this frame...
            if(j == base_model)
            {
                // Need to update this frame...  Insert coordinates...
                frame_data += 11 + (num_bone*24);
                frame_data += (num_joint*12);
                if(sdf_insert_data(frame_data, NULL, 12))
                {
                    // Put in the coordinate info...
                    *((float*) (frame_data)) = joint_xyz[X];
                    *((float*) (frame_data+4)) = joint_xyz[Y];
                    *((float*) (frame_data+8)) = joint_xyz[Z];


                    // Update frame data pointers...
                    j = i+1;
                    while(j < num_bone_frame)
                    {
                        *((unsigned char**) (frame_data_start+(j<<2))) += 12;
                        j++;
                    }
                }
            }
        }


        // Insert joint info to base model..
        if(sdf_insert_data(joint_data+(num_joint<<2), NULL, 4))
        {
            // Added the joint to base model data...  Write size...
            *((float*) (joint_data+(num_joint<<2))) = joint_size*JOINT_COLLISION_SCALE;


            // Update number of joints for this base model
            num_joint++;
            *((unsigned short*) (base_model_data-4) ) = num_joint;


            // Update all base model pointers at start of file
            data+=12;
            *((unsigned char**) data) += 4;  data+=4;
            base_model++;
            while(base_model < num_base_model)
            {
                *((unsigned char**) data) += 4;  data+=4;
                *((unsigned char**) data) += 4;  data+=4;
                *((unsigned char**) data) += 4;  data+=4;
                *((unsigned char**) data) += 4;  data+=4;
                *((unsigned char**) data) += 4;  data+=4;
                base_model++;
            }


            // Update bone frame pointers...
            repeat(i, num_bone_frame)
            {
                *((unsigned char**) data) += 4;  data+=4;
            }
log_message("INFO:   Done with insert_joint");
            return TRUE;
        }
        // If it got here, we probably screwed everything up...
        log_message("ERROR:  Model corrupted by insert_joint()...  Shoulda fixed that...");
    }
    else
    {
        // Delete a given joint
        if(joint_to_remove < num_joint)
        {
            // Remove any attached bones to start with...  Should also renumber higher joints...
            if(bone_delete_okay)
            {
log_message("INFO:   Deleting joint %d", joint_to_remove);
                // Delete joint coordinates in every frame of animation
                repeat(i, num_bone_frame)
                {
                    frame_data = *((unsigned char**) (frame_data_start+(i<<2)));
                    j = *(frame_data+2);  // Base model for this frame...
                    if(j == base_model)
                    {
                        // Need to update this frame...  Insert coordinates...
                        frame_data += 11 + (num_bone*24);
                        frame_data += (joint_to_remove*12);
                        if(sdf_insert_data(frame_data, NULL, -12))
                        {
                            // Update frame data pointers...
                            j = i+1;
                            while(j < num_bone_frame)
                            {
                                *((unsigned char**) (frame_data_start+(j<<2))) -= 12;
                                j++;
                            }
                        }
                    }
                }


                // Delete joint info from base model...
                if(sdf_insert_data(joint_data+(joint_to_remove<<2), NULL, -4))
                {
                    // Update number of joints for this base model
                    num_joint--;
                    *((unsigned short*) (base_model_data-4) ) = num_joint;


                    // Update all base model pointers at start of file
                    data+=12;
                    *((unsigned char**) data) -= 4;  data+=4;
                    base_model++;
                    while(base_model < num_base_model)
                    {
                        *((unsigned char**) data) -= 4;  data+=4;
                        *((unsigned char**) data) -= 4;  data+=4;
                        *((unsigned char**) data) -= 4;  data+=4;
                        *((unsigned char**) data) -= 4;  data+=4;
                        *((unsigned char**) data) -= 4;  data+=4;
                        base_model++;
                    }


                    // Update bone frame pointers...
                    repeat(i, num_bone_frame)
                    {
                        *((unsigned char**) data) -= 4;  data+=4;
                    }




                    // Must recalculate vertex attachments and scalars...
                    render_generate_bone_normals(start_data, frame);
                    repeat(i, num_vertex)
                    {
                        render_attach_vertex_to_bone(start_data, frame, i);
                        render_crunch_vertex(start_data, frame, i, TRUE, 0);
                    }
log_message("INFO:   Done with insert_joint");
                    return TRUE;
                }
            }
        }
    }
log_message("INFO:   Done with insert_joint");
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void scale_all_joints_and_vertices(unsigned char* data, float scale)
{
    // <ZZ> This function scales the joints and vertices...  Last minute sorta thing...
    float* position_xyz;
    unsigned short frame;
    unsigned short num_bone, num_joint, num_vertex;
    unsigned char* data_start;
    unsigned char* frame_data;
    unsigned char* base_data;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char base_model, detail_level;
    float temp;
    unsigned short i;

    data_start = data;

    // Find our bone frames...
    data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);



    // Scale the bone lengths and vertex positions...
    repeat(base_model, num_base_model)
    {
        detail_level = 0;
        base_data = data + (base_model*20) + (num_base_model*20*detail_level);
        base_data = *((unsigned char**) base_data);
        num_vertex = *((unsigned short*) base_data);  base_data+=6;
        num_bone = *((unsigned short*) base_data);  base_data+=2;


        repeat(i, num_vertex)
        {
            position_xyz = (float*) base_data;
            position_xyz[X] *= scale;
            position_xyz[Y] *= scale;
            position_xyz[Z] *= scale;
            base_data+=64;
        }



        base_data = data + (base_model*20) + 16 + (num_base_model*20*detail_level);
        base_data = *((unsigned char**) base_data);  base_data+=5;
        repeat(i, num_bone)
        {
            temp = *((float*) base_data);
            *((float*) base_data) = temp * scale;
            base_data+=9;
        }
    }



    // Scale the joint locations...
    frame = 0;
    repeat(frame, num_bone_frame)
    {
        frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));


        // Find number of bones and joints for bone frame...
        base_model = *(frame_data + 2);
        detail_level = 0;
        base_data = data + (base_model*20) + (num_base_model*20*detail_level);
        base_data = *((unsigned char**) base_data);  base_data+=4;
        num_joint = *((unsigned short*) base_data);  base_data+=2;
        num_bone = *((unsigned short*) base_data);


        // Skip to joint positions...
        // Scale all of the bones...
        frame_data += 11 + (num_bone*24);


        // Now handle each joint's scaling...
        position_xyz = (float*) frame_data;
        repeat(i, num_joint)
        {
            position_xyz[X] *= scale;
            position_xyz[Y] *= scale;
            position_xyz[Z] *= scale;
            position_xyz+=3;
        }
    }



    repeat(i, num_vertex)
    {
        render_attach_vertex_to_bone(data_start, 0, i);
        render_crunch_vertex(data_start, 0, i, TRUE, 0);
    }

}
#endif


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void scale_selected_vertices(unsigned char* data, unsigned short frame, unsigned char axis, float scale)
{
    // <ZZ> This function scales the selected points about a certain axis.
    unsigned short i;


    // Scale each of 'em...
    if(axis <= Z)
    {
        repeat(i, select_num)
        {
            select_xyz[i][axis] = select_xyz[i][axis]*scale;
            select_data[i][axis] = select_data[i][axis]*scale;
            // Recrunch the bone scalars too
            render_attach_vertex_to_bone(data, frame, select_list[i]);
            render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
        }
    }
}
#endif


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void scale_selected_vertices_centrid(unsigned char* data, unsigned short frame, float scale)
{
    // <ZZ> This function scales the selected points towards another point...
    unsigned short i;
    float centrid_xyz[3], inverse;

    if(select_num < 1)
    {
        return;
    }

    // Find the centrid...
    centrid_xyz[X] = 0.0f;
    centrid_xyz[Y] = 0.0f;
    centrid_xyz[Z] = 0.0f;
    repeat(i, select_num)
    {
        centrid_xyz[X] += select_xyz[i][X];
        centrid_xyz[Y] += select_xyz[i][Y];
        centrid_xyz[Z] += select_xyz[i][Z];
    }
    centrid_xyz[X] /= select_num;
    centrid_xyz[Y] /= select_num;
    centrid_xyz[Z] /= select_num;
//centrid_xyz[Z] = 0.125f;
    inverse = 1.0f - scale;
    centrid_xyz[X] *= inverse;
    centrid_xyz[Y] *= inverse;
    centrid_xyz[Z] *= inverse;


    // Scale each of 'em...
    repeat(i, select_num)
    {
        select_xyz[i][X] = (select_xyz[i][X]*scale) + centrid_xyz[X];
        select_data[i][X] = (select_data[i][X]*scale) + centrid_xyz[X];
        select_xyz[i][Y] = (select_xyz[i][Y]*scale) + centrid_xyz[Y];
        select_data[i][Y] = (select_data[i][Y]*scale) + centrid_xyz[Y];
        select_xyz[i][Z] = (select_xyz[i][Z]*scale) + centrid_xyz[Z];
        select_data[i][Z] = (select_data[i][Z]*scale) + centrid_xyz[Z];
        // Recrunch the bone scalars too
        render_attach_vertex_to_bone(data, frame, select_list[i]);
        render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void move_selected_vertices(unsigned char* data, unsigned short frame, unsigned char axis, float offset)
{
    // <ZZ> This function moves the selected points on a certain axis.
    unsigned short i;


    // Move each of 'em...
    if(axis <= Z)
    {
        repeat(i, select_num)
        {
            select_xyz[i][axis] = select_xyz[i][axis]+offset;
            select_data[i][axis] = select_data[i][axis]+offset;
            // Recrunch the bone scalars too
            render_attach_vertex_to_bone(data, frame, select_list[i]);
            render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
        }
    }
}
#endif


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void set_selected_vertices(unsigned char* data, unsigned short frame, unsigned char axis, float position)
{
    // <ZZ> This function sets the position of selected points on a certain axis.
    unsigned short i;


    // Move each of 'em...
    if(axis <= Z)
    {
        repeat(i, select_num)
        {
            select_xyz[i][axis] = position;
            select_data[i][axis] = position;
            // Recrunch the bone scalars too
            render_attach_vertex_to_bone(data, frame, select_list[i]);
            render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
        }
    }
}
#endif


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void break_anim_joints(unsigned char* data, unsigned short frame)
{
    // <ZZ> A better version of the function below...  Gets velocity of each joint from the
    //      its position in the previous frame...
    float* position_xyz;
    float* last_position_xyz;
    float* next_position_xyz;
    float velocity_xyz[3];
    unsigned short last_frame, next_frame;
    unsigned short num_bone, num_joint;
    unsigned char* data_start;
    unsigned char* frame_data;
    unsigned char* next_frame_data;
    unsigned char* last_frame_data;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char base_model, detail_level;
    unsigned short i;
float z;


    data_start = data;


    // Error check...
    if(frame < 2) { return; }
    last_frame = frame-2;
    next_frame = frame-1;


    // Find our bone frames...
    data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
    next_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(next_frame<<2)));


    // Find number of bones and joints for bone frame...
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    data = *((unsigned char**) data);  data+=4;
    num_joint = *((unsigned short*) data);  data+=2;
    num_bone = *((unsigned short*) data);


    // Error check...
    if(base_model != (*(last_frame_data+2))) { return; }
    if(base_model != (*(next_frame_data+2))) { return; }


    // Skip to joint positions...
    frame_data += 11 + (num_bone*24);
    last_frame_data += 11 + (num_bone*24);
    next_frame_data += 11 + (num_bone*24);


    // Now handle each joint's movement offset...
    position_xyz = (float*) frame_data;
    last_position_xyz = (float*) last_frame_data;
    next_position_xyz = (float*) next_frame_data;

    // Floor level...
    z = 0.0f;
//    if(frame > 5)
//    {
//        z = (float) (frame - 5);
//        if(z > 10.0)
//        {
//            z = 0.75f;
//        }
//        else
//        {
//            z = z * 0.075f;
//        }
//    }
    repeat(i, num_joint)
    {
        velocity_xyz[X] = next_position_xyz[X] - last_position_xyz[X];
        velocity_xyz[Y] = next_position_xyz[Y] - last_position_xyz[Y];
        velocity_xyz[Z] = next_position_xyz[Z] - last_position_xyz[Z];
        velocity_xyz[Z] -= 0.03f;
        position_xyz[X] += velocity_xyz[X];
        position_xyz[Y] += velocity_xyz[Y];
        position_xyz[Z] += velocity_xyz[Z];
        if(position_xyz[Z] < z)
        {
            position_xyz[X] -= velocity_xyz[X]*0.25f;
            position_xyz[Y] -= velocity_xyz[Y]*0.25f;

            position_xyz[Z] = position_xyz[Z]-z;
            position_xyz[Z] = position_xyz[Z]*-0.5f;
            position_xyz[Z] = position_xyz[Z]+z;
        }
        position_xyz+=3;
        last_position_xyz+=3;
        next_position_xyz+=3;
    }



    render_fix_model_to_bone_length(data_start, frame, 9999);
    render_generate_bone_normals(data_start, frame);
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
float break_anim_velocity_xyz[MAX_SELECT][3];
void break_anim_selected_vertices(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function helps me make the break animations for tiles...  Assumes 32 frame
    //      animation...
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned char texture_mode;
    unsigned short num_primitive, num_primitive_vertex;
    unsigned char base_model, detail_level;
    float velocity_xyz[3];
    float offset_xyz[3];
    unsigned short v1, v2, v3;
    unsigned char* data_start;
    float percent, inverse;

    unsigned short i, j, k, m;
    float distance;


    // Calculate velocity for each point...
    repeat(i, select_num)
    {
        distance = vector_length(select_xyz[i]);
        if(distance < 0.001f)  { distance = 0.001f; }
        distance*=5.0f;
        break_anim_velocity_xyz[i][X] = select_xyz[i][X]/distance;
        break_anim_velocity_xyz[i][Y] = select_xyz[i][Y]/distance;
        break_anim_velocity_xyz[i][Z] = 2.0f*select_xyz[i][Z]/distance;
    }


    // Bleed velocities based on connectivity...
    data_start = data;
    repeat(m, 20)
    {
        data = data_start;
        data+=2;
        num_detail_level = *data;  data++;
        num_base_model = *data;  data++;
        num_bone_frame = *((unsigned short*) data); data+=2;
        if(frame >= num_bone_frame) return;
        data+=(ACTION_MAX<<1);
        data+=(MAX_DDD_SHADOW_TEXTURE);
        frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
        base_model = *(frame_data + 2);
        detail_level = 0;
        data = data + (base_model*20) + (num_base_model*20*detail_level);
        data+=8;
        texture_data = *((unsigned char**) data);  data+=4;
        repeat(i, MAX_DDD_TEXTURE)
        {
            texture_mode = *texture_data;  texture_data++;
            if(texture_mode != 0)
            {
                texture_data+=2;

                // Strips...
                num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                repeat(j, num_primitive)
                {
                    num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                    if(num_primitive_vertex == 3)
                    {
                        v1 = *((unsigned short*) texture_data);
                        texture_data+=4;
                        v2 = *((unsigned short*) texture_data);
                        texture_data+=4;
                        v3 = *((unsigned short*) texture_data);
                        texture_data+=4;
                        if(select_inlist(v1))
                        {
                            v1 = select_index;
                            if(select_inlist(v2))
                            {
                                v2 = select_index;
                                if(select_inlist(v3))
                                {
                                    v3 = select_index;
                                    velocity_xyz[X] = 0.33333f * (break_anim_velocity_xyz[v1][X] + break_anim_velocity_xyz[v2][X] + break_anim_velocity_xyz[v3][X]);
                                    velocity_xyz[Y] = 0.33333f * (break_anim_velocity_xyz[v1][Y] + break_anim_velocity_xyz[v2][Y] + break_anim_velocity_xyz[v3][Y]);
                                    velocity_xyz[Z] = 0.33333f * (break_anim_velocity_xyz[v1][Z] + break_anim_velocity_xyz[v2][Z] + break_anim_velocity_xyz[v3][Z]);
                                    break_anim_velocity_xyz[v1][X] = velocity_xyz[X];  break_anim_velocity_xyz[v1][Y] = velocity_xyz[Y];  break_anim_velocity_xyz[v1][Z] = velocity_xyz[Z];
                                    break_anim_velocity_xyz[v2][X] = velocity_xyz[X];  break_anim_velocity_xyz[v2][Y] = velocity_xyz[Y];  break_anim_velocity_xyz[v2][Z] = velocity_xyz[Z];
                                    break_anim_velocity_xyz[v3][X] = velocity_xyz[X];  break_anim_velocity_xyz[v3][Y] = velocity_xyz[Y];  break_anim_velocity_xyz[v3][Z] = velocity_xyz[Z];
                                }
                            }
                        }
                    }
                    else
                    {
                        // Shouldn't happen
                        repeat(k, num_primitive_vertex)
                        {
                            texture_data+=4;
                        }
                    }
                }
            }
        }
    }



    // Shrink positions towards end...
    if(frame > 23)
    {
        repeat(m, ((frame-23)*8))
        {
            percent = 0.01f;
            inverse = 1.0f - percent;
            data = data_start;
            data+=2;
            num_detail_level = *data;  data++;
            num_base_model = *data;  data++;
            num_bone_frame = *((unsigned short*) data); data+=2;
            if(frame >= num_bone_frame) return;
            data+=(ACTION_MAX<<1);
            data+=(MAX_DDD_SHADOW_TEXTURE);
            frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
            base_model = *(frame_data + 2);
            detail_level = 0;
            data = data + (base_model*20) + (num_base_model*20*detail_level);
            data+=8;
            texture_data = *((unsigned char**) data);  data+=4;
            repeat(i, MAX_DDD_TEXTURE)
            {
                texture_mode = *texture_data;  texture_data++;
                if(texture_mode != 0)
                {
                    texture_data+=2;

                    // Strips...
                    num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
                    repeat(j, num_primitive)
                    {
                        num_primitive_vertex = *((unsigned short*) texture_data);  texture_data+=2;
                        if(num_primitive_vertex == 3)
                        {
                            v1 = *((unsigned short*) texture_data);
                            texture_data+=4;
                            v2 = *((unsigned short*) texture_data);
                            texture_data+=4;
                            v3 = *((unsigned short*) texture_data);
                            texture_data+=4;
                            if(select_inlist(v1))
                            {
                                v1 = select_index;
                                if(select_inlist(v2))
                                {
                                    v2 = select_index;
                                    if(select_inlist(v3))
                                    {
                                        v3 = select_index;
                                        velocity_xyz[X] = 0.33333f * (select_xyz[v1][X] + select_xyz[v2][X] + select_xyz[v3][X]);
                                        velocity_xyz[Y] = 0.33333f * (select_xyz[v1][Y] + select_xyz[v2][Y] + select_xyz[v3][Y]);
                                        velocity_xyz[Z] = 0.33333f * (select_xyz[v1][Z] + select_xyz[v2][Z] + select_xyz[v3][Z]);

                                        offset_xyz[X] = ((inverse*select_xyz[v1][X]) + (percent*velocity_xyz[X])) - select_xyz[v1][X];  offset_xyz[Y] = ((inverse*select_xyz[v1][Y]) + (percent*velocity_xyz[Y])) - select_xyz[v1][Y];  offset_xyz[Z] = ((inverse*select_xyz[v1][Z]) + (percent*velocity_xyz[Z])) - select_xyz[v1][Z];
                                        select_xyz[v1][X] += offset_xyz[X];  select_xyz[v1][Y] += offset_xyz[Y];  select_xyz[v1][Z] += offset_xyz[Z];
                                        select_data[v1][X] += offset_xyz[X];  select_data[v1][Y] += offset_xyz[Y];  select_data[v1][Z] += offset_xyz[Z];

                                        offset_xyz[X] = ((inverse*select_xyz[v2][X]) + (percent*velocity_xyz[X])) - select_xyz[v2][X];  offset_xyz[Y] = ((inverse*select_xyz[v2][Y]) + (percent*velocity_xyz[Y])) - select_xyz[v2][Y];  offset_xyz[Z] = ((inverse*select_xyz[v2][Z]) + (percent*velocity_xyz[Z])) - select_xyz[v2][Z];
                                        select_xyz[v2][X] += offset_xyz[X];  select_xyz[v2][Y] += offset_xyz[Y];  select_xyz[v2][Z] += offset_xyz[Z];
                                        select_data[v2][X] += offset_xyz[X];  select_data[v2][Y] += offset_xyz[Y];  select_data[v2][Z] += offset_xyz[Z];

                                        offset_xyz[X] = ((inverse*select_xyz[v3][X]) + (percent*velocity_xyz[X])) - select_xyz[v3][X];  offset_xyz[Y] = ((inverse*select_xyz[v3][Y]) + (percent*velocity_xyz[Y])) - select_xyz[v3][Y];  offset_xyz[Z] = ((inverse*select_xyz[v3][Z]) + (percent*velocity_xyz[Z])) - select_xyz[v3][Z];
                                        select_xyz[v3][X] += offset_xyz[X];  select_xyz[v3][Y] += offset_xyz[Y];  select_xyz[v3][Z] += offset_xyz[Z];
                                        select_data[v3][X] += offset_xyz[X];  select_data[v3][Y] += offset_xyz[Y];  select_data[v3][Z] += offset_xyz[Z];
                                    }
                                }
                            }
                        }
                        else
                        {
                            // Shouldn't happen
                            repeat(k, num_primitive_vertex)
                            {
                                texture_data+=4;
                            }
                        }
                    }
                }
            }
        }
    }


    // Move each of 'em...
    repeat(i, select_num)
    {
        repeat(j, frame)
        {
            select_xyz[i][X] += break_anim_velocity_xyz[i][X];
            select_data[i][X] += break_anim_velocity_xyz[i][X];
            select_xyz[i][Y] += break_anim_velocity_xyz[i][Y];
            select_data[i][Y] += break_anim_velocity_xyz[i][Y];
            select_xyz[i][Z] += break_anim_velocity_xyz[i][Z];
            select_data[i][Z] += break_anim_velocity_xyz[i][Z];
            if(select_xyz[i][Z] < 0.0f)
            {
                select_xyz[i][Z] = 0.0f;
                select_data[i][Z] = 0.0f;
                break_anim_velocity_xyz[i][Z] = -0.5f*break_anim_velocity_xyz[i][Z];
                break_anim_velocity_xyz[i][X] *= 0.75f;
                break_anim_velocity_xyz[i][Y] *= 0.75f;
            }


            // Change velocity...
            break_anim_velocity_xyz[i][X] *= 0.95f;
            break_anim_velocity_xyz[i][Y] *= 0.95f;
//            break_anim_velocity_xyz[i][Z] -= 0.05f;
            break_anim_velocity_xyz[i][Z] -= 0.03f;


        }



        // Recrunch the bone scalars too
        render_attach_vertex_to_bone(data_start, frame, select_list[i]);
        render_crunch_vertex(data_start, frame, select_list[i], TRUE, 0);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void tree_rotate_selected_vertices(unsigned char* data, unsigned short frame, float angle)
{
    // <ZZ> This function rotates the selected vertices by any angle, around the y axis
    //      (forward).  For doing the tree fall animation...
    unsigned short i;
    float x, z;
    float sine, cosine;

    sine = (float) sin(angle*2.0f*PI/360.0f);
    cosine = (float) cos(angle*2.0f*PI/360.0f);
    repeat(i, select_num)
    {
        x = select_xyz[i][X];
        z = select_xyz[i][Z];
        select_xyz[i][X] = (cosine*x)+(sine*z);
        select_xyz[i][Z] = (cosine*z)-(sine*x);
        select_data[i][X] = select_xyz[i][X];
        select_data[i][Z] = select_xyz[i][Z];
        // Recrunch the bone scalars too
        render_attach_vertex_to_bone(data, frame, select_list[i]);
        render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void rotate_selected_vertices(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function rotates the selected vertices 90' clockwise, around the
    //      z axis (up).
    unsigned short i;
    float x, y;


    repeat(i, select_num)
    {
        x = select_xyz[i][X];
        y = select_xyz[i][Y];
        select_xyz[i][X] = -y;
        select_xyz[i][Y] = x;
        select_data[i][X] = -y;
        select_data[i][Y] = x;
        // Recrunch the bone scalars too
        render_attach_vertex_to_bone(data, frame, select_list[i]);
        render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void flip_selected_vertices(unsigned char* data, unsigned short frame, unsigned char axis)
{
    // <ZZ> This function flips the selected points about a certain axis.  Also flips the
    //      normals of selected triangles...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned short i, j, k, m;
    unsigned char texture_mode;
    unsigned short num_primitive, num_primitive_vertex;
    int amount_added;
    unsigned char base_model, detail_level;


    // Flip each of 'em...
    if(axis <= Z)
    {
        repeat(i, select_num)
        {
            select_xyz[i][axis] = -select_xyz[i][axis];
            select_data[i][axis] = -select_data[i][axis];
            // Recrunch the bone scalars too
            render_attach_vertex_to_bone(data, frame, select_list[i]);
            render_crunch_vertex(data, frame, select_list[i], TRUE, 0);
        }
    }


    // Run through each triangle, flipping normals if all three points are selected...
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    if(select_num < 3) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    texture_data = *((unsigned char**) (data+8));


    // Go through each triangle...
    repeat(m, MAX_DDD_TEXTURE)
    {
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            // Texture is turned on...
            texture_data+=2;


            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(i, num_primitive)
            {
                amount_added = 0; 
                num_primitive_vertex = *((unsigned short*) (texture_data));  texture_data+=2;
                repeat(j, num_primitive_vertex)
                {
                    if(select_inlist(*((unsigned short*) texture_data)))
                    {
                        amount_added++;
                    }
                    texture_data+=4;
                }
                if(amount_added == num_primitive_vertex)
                {
                    // Found a match...  Flip it...
                    j = *((unsigned short*) (texture_data-4));
                    k = *((unsigned short*) (texture_data-2));
                    *((unsigned short*) (texture_data-4)) = *((unsigned short*) (texture_data-8));
                    *((unsigned short*) (texture_data-2)) = *((unsigned short*) (texture_data-6));
                    *((unsigned short*) (texture_data-8)) = j;
                    *((unsigned short*) (texture_data-6)) = k;
                }
            }

            // Skip fans...
            texture_data+=2;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char* get_start_of_triangles(unsigned char* data, unsigned short frame, unsigned char texture)
{
    // <ZZ> This function returns a pointer to the start of a set of triangles within an RDY
    //      file.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;
    unsigned char* texture_data;
    unsigned short i, j, m;
    unsigned char texture_mode;
    unsigned short num_primitive, num_primitive_vertex;
    unsigned char base_model, detail_level;
    unsigned short num_bone, num_joint;


    // Run through each triangle until we get to the desired texture...
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return NULL;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);

    texture_data = *((unsigned char**) (data));
    num_joint = *((unsigned short*) (texture_data+4));
    num_bone = *((unsigned short*) (texture_data+6));

    texture_data = *((unsigned char**) (data+8));


    // Go through each triangle...
    repeat(m, MAX_DDD_TEXTURE)
    {
        if(m == texture)  return texture_data;
        texture_mode = *texture_data;  texture_data++;
        if(texture_mode != 0)
        {
            // Texture is turned on...
            texture_data+=2;


            // Strips...
            num_primitive = *((unsigned short*) texture_data);  texture_data+=2;
            repeat(i, num_primitive)
            {
                num_primitive_vertex = *((unsigned short*) (texture_data));  texture_data+=2;
                repeat(j, num_primitive_vertex)
                {
                    texture_data+=4;
                }
            }

            // Skip fans...
            texture_data+=2;
        }
    }

    // Requested texture was more than we're supposed to have...  Means we're requesting cartoon line data...


    // Skip joints...
    texture_data+=4*num_joint;

    // Skip bones...
    texture_data+=9*num_bone;

    // Should be at cartoon lines...
    return texture_data;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void weld_selected_vertices(unsigned char* data, unsigned short frame, unsigned char glue_instead)
{
    // <ZZ> This function finds the best pairs of vertices within the selected group, and turns
    //      each pair into a single vertex.  Renumbers triangle associations to match.
    //      If glue_instead is set, it doesn't delete the extra vertices...
    unsigned short i, j;
    float distance_xyz[3];
    float distance;
    float best_distance;
    unsigned short best_partner;


    // Need an even number of 'em
    if(select_num & 1)
    {
        if((key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT]) && select_num > 2)
        {
            select_num--;
        }
        else
        {
            message_add("ERROR:  Need an even number to weld (shift to force)", NULL, FALSE);
            return;
        }
    }


    // Flag each of our selected vertices as having no partner...
    repeat(i, select_num)
    {
        select_flag[i] = MAX_VERTEX;
    }


    // Find a partner for each vertex
    repeat(i, select_num)
    {
        // Does it have a partner yet?
        if(select_flag[i] == MAX_VERTEX)
        {
            // Nope, so find one...
            best_partner = MAX_VERTEX;
            best_distance = 9999.9f;
            j = i+1;
            while(j < select_num)
            {
                // Can we pair it with this one?
                if(select_flag[j] == MAX_VERTEX)
                {
                    // Yup, so check how close they are...
                    distance_xyz[X] = select_xyz[j][X]-select_xyz[i][X];
                    distance_xyz[Y] = select_xyz[j][Y]-select_xyz[i][Y];
                    distance_xyz[Z] = select_xyz[j][Z]-select_xyz[i][Z];
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
            select_flag[i] = best_partner;
            select_flag[best_partner] = i;
            // Find the center of the points, and move both to that location
            select_data[i][X] = (select_xyz[i][X] + select_xyz[best_partner][X]) * 0.5f;
            select_data[i][Y] = (select_xyz[i][Y] + select_xyz[best_partner][Y]) * 0.5f;
            select_data[i][Z] = (select_xyz[i][Z] + select_xyz[best_partner][Z]) * 0.5f;
            select_data[best_partner][X] = select_data[i][X];
            select_data[best_partner][Y] = select_data[i][Y];
            select_data[best_partner][Z] = select_data[i][Z];
        }
    }
    select_update_xyz();


    if(glue_instead)
    {
        // Recrunch all selected vertices, so they're in the right positions....
        repeat(i, 5000)
        {
            if(select_inlist(i))
            {
                render_attach_vertex_to_bone(data, frame, i);
                render_crunch_vertex(data, frame, i, TRUE, 0);
            }
        }
    }
    else
    {
        // Delete the higher vertex of each pair...  Do in weird order so it doesn't mess up...
        // Do with hacked in replacement thing...
        i = 5000;
        while(i > 0)
        {
            i--;
            if(select_inlist(i))
            {
                if(select_flag[select_index] != MAX_VERTEX)
                {
                    // Delete the vertex, and replace any reference with one to its partner...
                    // Flag its partner to not be deleted...
                    best_partner = select_flag[select_index];
                    render_insert_vertex(data, frame, NULL, i, select_list[best_partner]);
                    select_flag[best_partner] = MAX_VERTEX;

                    // Recrunch the bone scalars for the undeleted partner...
                    render_attach_vertex_to_bone(data, frame, select_list[best_partner]);
                    render_crunch_vertex(data, frame, select_list[best_partner], TRUE, 0);
                }
            }
        }
    }
    select_clear();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void weld_selected_tex_vertices(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function finds the best pairs of tex vertices within the selected group, and turns
    //      each pair into a single tex vertex.  Renumbers triangle associations to match.
    unsigned short i, j;
    float distance_xyz[3];
    float distance;
    float best_distance;
    unsigned short best_partner;


    // Need an even number of 'em
    if(select_num & 1)
    {
        if((key_down[SDL_SCANCODE_LSHIFT] || key_down[SDL_SCANCODE_RSHIFT]) && select_num > 2)
        {
            select_num--;
        }
        else
        {
            message_add("ERROR:  Need an even number to weld (shift to force)", NULL, FALSE);
            return;
        }
    }


    // Flag each of our selected vertices as having no partner...
    repeat(i, select_num)
    {
        select_flag[i] = MAX_TEX_VERTEX;
    }


    // Find a partner for each vertex
    repeat(i, select_num)
    {
        // Does it have a partner yet?
        if(select_flag[i] == MAX_TEX_VERTEX)
        {
            // Nope, so find one...
            best_partner = MAX_TEX_VERTEX;
            best_distance = 9999.9f;
            j = i+1;
            while(j < select_num)
            {
                // Can we pair it with this one?
                if(select_flag[j] == MAX_TEX_VERTEX)
                {
                    // Yup, so check how close they are...
                    distance_xyz[X] = select_xyz[j][X]-select_xyz[i][X];
                    distance_xyz[Y] = select_xyz[j][Y]-select_xyz[i][Y];
                    distance_xyz[Z] = 0;
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
            if(best_partner == MAX_TEX_VERTEX)
            {
                log_message("ERROR:  Partnering error...  Shouldn't happen...");
                return;
            }
            // Remember our partners...
            select_flag[i] = best_partner;
            select_flag[best_partner] = i;
            // Find the center of the points, and move both to that location
            select_data[i][X] = (select_xyz[i][X] + select_xyz[best_partner][X]) * 0.5f;
            select_data[i][Y] = (select_xyz[i][Y] + select_xyz[best_partner][Y]) * 0.5f;
            select_data[best_partner][X] = select_data[i][X];
            select_data[best_partner][Y] = select_data[i][Y];
        }
    }
    select_update_xy();


    // Delete the higher vertex of each pair...  Do in weird order so it doesn't mess up...
    // Do with hacked in replacement thing...
    i = 5000;
    while(i > 0)
    {
        i--;
        if(select_inlist(i))
        {
            if(select_flag[select_index] != MAX_TEX_VERTEX)
            {
                // Delete the vertex, and replace any reference with one to its partner...
                // Flag its partner to not be deleted...
                best_partner = select_flag[select_index];
                render_insert_tex_vertex(data, frame, NULL, i, select_list[best_partner]);
                select_flag[best_partner] = MAX_TEX_VERTEX;
            }
        }
    }
    select_clear();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void scale_selected_tex_vertices(unsigned char* data, unsigned short frame, unsigned char axis, float scale)
{
    // <ZZ> This function shrinks the selected texture vertices...
    float add;
    unsigned short i;
    int temp;

    if(axis < 2)
    {
        // Scaling the x or y axis...
        add = (1.0f-scale)*0.5f;
        repeat(i, select_num)
        {
            select_data[i][axis] = (select_data[i][axis]*scale)+add;
        }
    }
    else
    {
        // Really doing eye placement thing...  Lock vertices to nearest position...
        repeat(i, select_num)
        {
            temp = (int) ((select_data[i][X]*2.0f) + 0.5f);  select_data[i][X] = (temp * 0.5f);
            temp = (int) ((select_data[i][Y]*2.0f) + 0.5f);  select_data[i][Y] = (temp * 0.5f);
        }
    }
    select_update_xy();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_bounding_box()
{
    // <ZZ> This function draws a bounding box test thing for the modeler
    display_color(white);


    // Small box...
    display_start_line_loop();
        display_vertex_xyz(-1, -1, 0);
        display_vertex_xyz(1, -1, 0);
        display_vertex_xyz(1, 1, 0);
        display_vertex_xyz(-1, 1, 0);
    display_end();


    // Medium box...
    display_start_line_loop();
        display_vertex_xyz(-2, -2, 0);
        display_vertex_xyz(2, -2, 0);
        display_vertex_xyz(2, 2, 0);
        display_vertex_xyz(-2, 2, 0);
    display_end();


    // Large box...
    display_start_line_loop();
        display_vertex_xyz(-3, -3, 0);
        display_vertex_xyz(3, -3, 0);
        display_vertex_xyz(3, 3, 0);
        display_vertex_xyz(-3, 3, 0);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_axis()
{
    // <ZZ> This function draws 3D axis lines
    display_start_line();
        display_color(red);
        display_vertex_xyz(0, 0, 0);
        display_vertex_xyz(1, 0, 0);
    display_end();

    display_start_line();
        display_color(green);
        display_vertex_xyz(0, 0, 0);
        display_vertex_xyz(0, 1, 0);
    display_end();

    display_start_line();
        display_color(blue);
        display_vertex_xyz(0, 0, 0);
        display_vertex_xyz(0, 0, 1);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned short render_change_frame_base_model(unsigned char* data, unsigned short frame, unsigned short new_base_model)
{
    // <ZZ> This function changes the base model for a given frame, and replaces bone/joint locations
    //      with values from the Boning Model frame.  If new_base_model is 65535, the function returns
    //      the current base_model for the given frame.  Otherwise, it returns TRUE if the frame
    //      changed okay, or FALSE if there was a problem.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char** frame_data_start;
    unsigned char** base_model_data_start;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char* frame_data;
    unsigned char* new_frame_data;
    unsigned short i, j;
    unsigned short num_bone;
    unsigned short num_joint;
    unsigned char base_model;
    signed int size;
    unsigned short copy_from_frame;
    signed int copy_from_size;
    unsigned char alpha;


    // Go to the current base model, and determine its size
    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

    base_model_data_start = (unsigned char**) data;
    frame_data_start =  (unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX));
    if(new_base_model == 65535)
    {
        // Return the base model for the specified frame...
        frame_data = frame_data_start[frame];
        return frame_data[2];
    }
    if(new_base_model >= num_base_model) return FALSE;


    // Need to change the base model for the given frame...
    // Start by finding the first frame that uses the desired base model...  Quit if there isn't one...
    j = num_bone_frame;
    repeat(i, num_bone_frame)
    {
        frame_data = frame_data_start[i];
        if(frame_data[2] == new_base_model)
        {
            j = i;
            i = num_bone_frame;
        }
    }
    if(j == num_bone_frame)  return FALSE;  // Not found...
    copy_from_frame = j;


    // Determine the size of the current frame...
    frame_data = frame_data_start[frame];
    base_model = frame_data[2];
    base_model_data = base_model_data_start[base_model*5];
    num_joint = *((unsigned short*) (base_model_data+4));
    num_bone = *((unsigned short*) (base_model_data+6));
    size = 11 + (24*num_bone) + (12*num_joint);
    frame_data += size;
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        alpha = *frame_data;  frame_data++;  size++;
        if(alpha)
        {
            size+=32;  frame_data+=32;
        }
    }
    frame_data-=size;
    new_frame_data = frame_data;


    // Determine the size of the desired base model frame...
    frame_data = frame_data_start[copy_from_frame];
    base_model = frame_data[2];
    base_model_data = base_model_data_start[base_model*5];
    num_joint = *((unsigned short*) (base_model_data+4));
    num_bone = *((unsigned short*) (base_model_data+6));
    copy_from_size = 11 + (24*num_bone) + (12*num_joint);
    frame_data += copy_from_size;
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        alpha = *frame_data;  frame_data++;  copy_from_size++;
        if(alpha)
        {
            copy_from_size+=32;  frame_data+=32;
        }
    }
    frame_data-=size;


    // Delete the current frame, and move the desired one into its spot...
    memcpy(mainbuffer, frame_data, copy_from_size);
    if(sdf_insert_data(new_frame_data, NULL, -size))
    {
        sdf_insert_data(new_frame_data, mainbuffer, copy_from_size);
        // Change the pointers to all of the later frames to be correct...
        size = copy_from_size-size;  // Amount to push 'em back...
        i = frame+1;
        while(i < num_bone_frame)
        {
            frame_data_start[i]+=size;
            i++;
        }
        return TRUE;
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_base_model(unsigned char* data, unsigned short frame, unsigned char insert_mode)
{
    // <ZZ> This function adds a new base model to a given file.  New one is a copy of the given
    //      one.  If insert mode is FALSE, the given base_model is deleted instead (frame is really
    //      base model in this case...)
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char** frame_data_start;
    unsigned char** base_model_data_start;
    unsigned char* base_model_data;
    unsigned char* bone_data;
    unsigned char* start_data;
    unsigned char* frame_data;
    unsigned short i;
    unsigned char detail_level;
    unsigned short num_bone;
    unsigned short num_line;
    signed int size;
    unsigned short base_model;

    // Go to the current base model, and determine its size
    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(insert_mode)
    {
        if(frame >= num_bone_frame) return FALSE;
    }
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);

    base_model_data_start = (unsigned char**) data;
    frame_data_start =  (unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX));
    if(insert_mode)
    {
        frame_data = frame_data_start[frame];
        base_model = frame_data[2];
    }
    else
    {
        base_model = frame;
    }
    if(base_model >= num_base_model) return FALSE;
    detail_level = 0;
    base_model_data = data + (base_model*20) + (num_base_model*20*detail_level);
    data = base_model_data;
    bone_data = *((unsigned char**) (base_model_data+16));
    base_model_data = *((unsigned char**) (base_model_data));
    num_bone = *((unsigned short*) (base_model_data+6));
    size = ((unsigned int) bone_data) - ((unsigned int) base_model_data);
    size += num_bone*9;
    num_line = *((unsigned short*) (base_model_data+size));
    size += (num_line*8)+2;

    // Which mode are we in?
    if(insert_mode)
    {
        // Try to copy the data...
        if(num_base_model == 255) return FALSE;
        if(sdf_insert_data(base_model_data+size, base_model_data, size))
        {
            // Increase the number of base models...
            num_base_model++;
            *(start_data + 3) = num_base_model;

            // Now insert a new pointer block in the base model stuff...
            sdf_insert_data(data+20, data, 20);

            // Okay, now update all of the base model pointers
            repeat(i, num_base_model)
            {
                if(i > base_model)
                {
                    base_model_data_start[0] += size + 20;
                    base_model_data_start[1] += size + 20;
                    base_model_data_start[2] += size + 20;
                    base_model_data_start[3] += size + 20;
                    base_model_data_start[4] += size + 20;
                }
                else
                {
                    base_model_data_start[0] += 20;
                    base_model_data_start[1] += 20;
                    base_model_data_start[2] += 20;
                    base_model_data_start[3] += 20;
                    base_model_data_start[4] += 20;
                }
                base_model_data_start+=5;
            }
            frame_data_start+=5;


            // Now update all of the frame pointers, update base model references while we're at it...
            repeat(i, num_bone_frame)
            {
                frame_data_start[i] += size + 20;
                frame_data = frame_data_start[i];
                if(frame_data[2] > base_model)
                {
                    frame_data[2]++;
                }
            }


            // Now change the base model of the current frame to be the new base model...
            frame_data = frame_data_start[frame];
            frame_data[2]++;
            return TRUE;
        }
    }
    else
    {
        // Make sure the base model isn't being used
        repeat(i, num_bone_frame)
        {
            frame_data = frame_data_start[i];
            if(frame_data[2] == base_model)
            {
                return FALSE;
            }
        }

        // Try to delete the data...
        if(num_base_model == 1) return FALSE;
        if(sdf_insert_data(base_model_data, NULL, -size))
        {
            // Decrease the number of base models...
            num_base_model--;
            *(start_data + 3) = num_base_model;

            // Now get rid of the pointer block in the base model stuff...
            sdf_insert_data(data, NULL, -20);

            // Okay, now update all of the base model pointers
            repeat(i, num_base_model)
            {
                if(i >= base_model)
                {
                    base_model_data_start[0] -= size + 20;
                    base_model_data_start[1] -= size + 20;
                    base_model_data_start[2] -= size + 20;
                    base_model_data_start[3] -= size + 20;
                    base_model_data_start[4] -= size + 20;
                }
                else
                {
                    base_model_data_start[0] -= 20;
                    base_model_data_start[1] -= 20;
                    base_model_data_start[2] -= 20;
                    base_model_data_start[3] -= 20;
                    base_model_data_start[4] -= 20;
                }
                base_model_data_start+=5;
            }
            frame_data_start-=5;


            // Now update all of the frame pointers, update base model references while we're at it...
            repeat(i, num_bone_frame)
            {
                frame_data_start[i] -= size + 20;
                frame_data = frame_data_start[i];
                if(frame_data[2] > base_model)
                {
                    frame_data[2]--;
                }
            }
            return TRUE;
        }
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
unsigned char* get_start_of_frame(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function returns a pointer to the start of a given frame's data.
    //      Returns NULL if external frames or if there's some other problem...
    unsigned short flags;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;


    flags = *((unsigned short*) data); data+=3;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return NULL;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return NULL;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data+(frame<<2)));
    return frame_data;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char* get_frame_pointer(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function returns a pointer to the frame's pointer...  Blarg...
    unsigned short flags;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;


    flags = *((unsigned short*) data); data+=3;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return NULL;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return NULL;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = frame_data+(frame<<2);
    return frame_data;
}
#endif

//-----------------------------------------------------------------------------------------------
unsigned short get_number_of_bones(unsigned char* data)
{
    // <ZZ> This function returns the number of bones in base model 0 of the given RDY file...
    data+=6;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to base model 0
    data = (*((unsigned char**) data)) + 6;
    return (*((unsigned short*) data));
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char* get_start_of_frame_shadows(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function returns a pointer to the start of a frame's shadow data...
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned char* frame_data;
    unsigned short num_bone, num_joint;
    unsigned char base_model;


    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return NULL;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data+(frame<<2)));
    base_model = frame_data[2];
    data = data + (base_model*20);
    data = *((unsigned char**) data);  data+=4;
    num_joint = *((unsigned short*) data); data+=2;
    num_bone = *((unsigned short*) data);
    frame_data += 11 + (24*num_bone) + (12*num_joint);
    return frame_data;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char render_insert_frame(unsigned char* data, unsigned short frame, unsigned char insert_mode)
{
    // <ZZ> This function inserts a copy of the given frame directly after the original.  Deletes
    //      the given frame if insert_mode is FALSE.  Returns TRUE if it worked, FALSE if not.
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* frame_data;
    unsigned char* frame_data_start;
    unsigned char* new_frame_data;
    unsigned char* start_data;
    unsigned char* base_model_data_start;
    unsigned short i, j;
    unsigned short size;
    unsigned char alpha;
    unsigned char base_model, detail_level;

    start_data=data;
    flags = *((unsigned short*) data); data+=2;
    if((flags & DDD_EXTERNAL_BONE_FRAMES)) return FALSE;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    base_model_data_start = data;


    // Go to the current base model
    frame_data_start = data+(num_base_model*20*DETAIL_LEVEL_MAX);
    frame_data = *((unsigned char**) (frame_data_start+(frame<<2)));
    base_model = *(frame_data + 2);
    detail_level = 0;
    data = data + (base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);  data+=4;
    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;


    // Determine data size...   Need to count shadows...
    size = 11 + (24*num_bone) + (12*num_joint);
    frame_data += size;
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        alpha = *frame_data;  frame_data++;  size++;
        if(alpha)
        {
            size+=32;  frame_data+=32;
        }
    }


    if(insert_mode)
    {
log_message("INFO:   Frame insert, %d to %d", frame, frame+1);
log_message("INFO:     Frame %d is %d bytes", frame, size);


        // Insert a frame...  Copy current one...
        if(sdf_insert_data(frame_data, frame_data-size, size))
        {
log_message("INFO:     Inserted data");
            // Update number of frames for this base model
            new_frame_data = frame_data;
            num_bone_frame++;
            *((unsigned short*) (start_data+4)) = num_bone_frame;
log_message("INFO:     Num_bone_frame updated");


            // Update bone frame pointers...
            frame++;
            frame_data = frame_data_start+(frame<<2);
            if(sdf_insert_data(frame_data, NULL, 4))
            {
log_message("INFO:     Added bone frame pointer for frame %d", frame);
                *((unsigned char**) frame_data) = new_frame_data;
                frame_data+=4;
                frame++;
                while(frame < num_bone_frame)
                {
log_message("INFO:     Pushed back pointer for frame %d by size", frame);
                    *((unsigned char**) frame_data) += size;  frame_data+=4;
                    frame++;
                }



                // Update every every base model pointer to account for
                // scoochin' 'em back with last insert...
                base_model_data = base_model_data_start;
                repeat(i, num_detail_level)
                {
                    repeat(j, num_base_model)
                    {
log_message("INFO:     Pushed base %d, Detail %d, back by 4", j, i);
                        *((unsigned char**) base_model_data) += 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) += 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) += 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) += 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) += 4;  base_model_data+=4;
                    }
                }


                // Update every bone frame pointer to account for
                // scoochin' 'em back with last insert...
                frame_data = frame_data_start;
                repeat(frame, num_bone_frame)
                {
log_message("INFO:     Pushed back frame %d by 4", frame);
                    *((unsigned char**) frame_data) += 4;  frame_data+=4;
                }



                return TRUE;
            }
        }
    }
    else
    {
        // Delete the frame
        if(num_bone_frame < 2) return FALSE;
        frame_data-=size;
        if(sdf_insert_data(frame_data, NULL, -size))
        {
            // Update number of joints for this base model
            num_bone_frame--;
            *((unsigned short*) (start_data+4)) = num_bone_frame;


            // Update bone frame pointers...
            frame_data = frame_data_start+(frame<<2);
            if(sdf_insert_data(frame_data, NULL, -4))
            {
                while(frame < num_bone_frame)
                {
                    *((unsigned char**) frame_data) -= size;  frame_data+=4;
                    frame++;
                }



                // Update every every base model pointer to account for
                // scoochin' 'em forward with last insert...
                base_model_data = base_model_data_start;
                repeat(i, num_detail_level)
                {
                    repeat(j, num_base_model)
                    {
                        *((unsigned char**) base_model_data) -= 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) -= 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) -= 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) -= 4;  base_model_data+=4;
                        *((unsigned char**) base_model_data) -= 4;  base_model_data+=4;
                    }
                }


                // Update every bone frame pointer to account for
                // scoochin' 'em forward with last insert...
                frame_data = frame_data_start;
                repeat(frame, num_bone_frame)
                {
                    *((unsigned char**) frame_data) -= 4;  frame_data+=4;
                }


                return TRUE;
            }
        }
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned char no_link[] = "NO LINK";
unsigned char linkage_filename[16];
unsigned char* render_get_set_external_linkage(unsigned char* data, unsigned char* link_data)
{
    // <ZZ> If link_data is given, this function links one RDY (data) file to the animations
    //      of another RDY file (link_data).  If link_data is NULL, the function returns the
    //      name of the RDY file used for animation frames, or the string "NO LINK".
    //      In order to link, the two files must have the same number of base models.  The link
    //      file must have an equal or greater number of bones and joints for each base model.
    //      The joints and bones should also be in the same order, so the animation doesn't get
    //      all weird, but that isn't checked...  If setting linkage doesn't work, it returns
    //      NULL.  Non-NULL means that it worked...  If link_data is the same as data, the file
    //      should be unlinked, though an export is necessary to save the frames...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame, num_bone, num_joint;
    unsigned short link_num_bone_frame, link_num_bone, link_num_joint;
    unsigned char* base_model_data;
    unsigned char** base_model_data_start;
    unsigned char** frame_data_start;
    unsigned char** link_base_model_data_start;
    unsigned char** link_frame_data_start;
    unsigned char* start_data;
    unsigned short i, j, k;
    unsigned char file_type;
    unsigned int file_start;
    unsigned int file_size;
    unsigned int first_frame_data;
    signed int push_back;
    unsigned char* index;


    // Read the header...
    start_data = data;
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    base_model_data_start = (unsigned char**) data;
    frame_data_start = (unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX));


    if(link_data)
    {
        if(link_data == start_data)
        {
            // Just unflag it...
            flags &= (0xFFFF - DDD_EXTERNAL_BONE_FRAMES);
            *((unsigned short*) start_data) = flags;
        }
        else
        {
            // Read the link file's header...
            link_data+=3;
            if(num_base_model != *link_data) return NULL;
            link_data++;
            link_num_bone_frame = *((unsigned short*) link_data); link_data+=2;
            link_data+=(ACTION_MAX<<1);
            link_data+=(MAX_DDD_SHADOW_TEXTURE);
            link_base_model_data_start = (unsigned char**) link_data;
            link_frame_data_start = (unsigned char**) (link_data+(num_base_model*20*DETAIL_LEVEL_MAX));


            // Trying to set linkage...  Check to make sure it won't explode everything...
            repeat(i, num_base_model)
            {
                // Read in number of bones and joints
                base_model_data = base_model_data_start[i*5];
                num_joint = *((unsigned short*) (base_model_data+4));
                num_bone = *((unsigned short*) (base_model_data+6));
                base_model_data = link_base_model_data_start[i*5];
                link_num_joint = *((unsigned short*) (base_model_data+4));
                link_num_bone = *((unsigned short*) (base_model_data+6));
                if(num_bone > link_num_bone) return NULL;
                if(num_joint > link_num_joint) return NULL;
            }


            // Okay, it shouldn't blow up...  Set the flag...
            flags |= DDD_EXTERNAL_BONE_FRAMES;
            *((unsigned short*) start_data) = flags;


            // Copy the frame pointer block from the linked file...  Destroy old frame pointers...
            sdf_insert_data((unsigned char*) frame_data_start, NULL, -(num_bone_frame<<2));
            sdf_insert_data((unsigned char*) frame_data_start, (unsigned char*) link_frame_data_start, (link_num_bone_frame<<2));


            // Write the number of frames
            *((unsigned short*) (start_data+4)) = link_num_bone_frame;


            // Push back/forward the base model pointers to make up for adding/deleting frames
            push_back = link_num_bone_frame;
            push_back -= num_bone_frame;
            push_back = push_back << 2;
            repeat(i, num_detail_level)
            {
                repeat(j, num_base_model)
                {
                    repeat(k, 5)
                    {
                        *base_model_data_start+=push_back;
                        base_model_data_start++;
                    }
                }
            }
        }
        // Return non-NULL to show that it worked...
        return ((unsigned char*) 1);
    }
    else
    {
        // Trying to see if it's linked or not...  Check the flag...
        if((flags & DDD_EXTERNAL_BONE_FRAMES) == 0) return no_link;
        if(num_bone_frame == 0) return no_link;


        // Looks like it is linked, search through all files to find the filename...
        first_frame_data = (unsigned int) (frame_data_start[0]);
        repeat(i, sdf_num_files)
        {
            index = sdf_index + (i<<4);

            // Only check RDY files...
            file_type = *(index+4) & 15;
            if(file_type == SDF_FILE_IS_RDY)
            {
                // See if the first frame is in the current file's data block...
                file_start = sdf_read_unsigned_int(index);
                file_size = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;
                if(first_frame_data >= file_start && first_frame_data < (file_start+file_size))
                {
                    // Found a match...
                    sdf_get_filename(i, linkage_filename, &file_type);
                    return linkage_filename;
                }
            }
        }


        // Didn't find a match...
        return no_link;
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_center_frame(unsigned char* data, unsigned short frame, unsigned char axis)
{
    // <ZZ> This function centers the joints of a model on the given axis...
    unsigned char* frame_data;
    float* joint_data;
    float* joint_data_start;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char base_model;
    unsigned short i;
    float min, max, offset;


    start_data = data;
    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = frame_data[2];
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    frame_data += 11;


    // Find the min and max on the given axis...
    frame_data += (num_bone*24);  // Skip bones...
    joint_data = (float*) frame_data;
    joint_data_start = joint_data;
    min = 10000.0f;
    max =-10000.0f;
    repeat(i, num_joint)
    {
        if(joint_data[axis] < min) { min = joint_data[axis]; }
        if(joint_data[axis] > max) { max = joint_data[axis]; }
        joint_data+=3;
    }



    // Scooch the joints around...
    joint_data = joint_data_start;
    offset = ((max-min)*0.5f)-max;

if(axis == Z)
{
    offset = -0.25f;
}
if(axis == 3)
{
    offset = 0.25f;
    axis = Z;
}


    repeat(i, num_joint)
    {
        joint_data[axis]+=offset;
        joint_data+=3;
    }


    // Fix the model for bone length and stuff...
    if(!key_down[SDL_SCANCODE_F])
    {
        render_fix_model_to_bone_length(start_data, frame, 9999);
    }
    render_generate_bone_normals(start_data, frame);
}
#endif


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned short render_marked_frame = 0;
void render_copy_frame(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function copies the marked frame to the given one...
    unsigned short last_frame;
    unsigned char* frame_data;
    unsigned char* last_frame_data;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char base_model;
    unsigned short i, j;


    start_data = data;
    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    last_frame = render_marked_frame;
    if(frame >= num_bone_frame) return;
    if(last_frame >= num_bone_frame) return;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
    base_model = frame_data[2];
    if(last_frame_data[2] != base_model) last_frame_data = frame_data;
    if(last_frame_data == frame_data) return;  // Copying to same frame...
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    frame_data += 11;
    last_frame_data += 11;

    // Do the joint/bone copy...
    num_joint=(num_joint*3) + (num_bone*6);
    repeat(i, num_joint)
    {
        *((float*) frame_data) = *((float*) last_frame_data);
        frame_data+=4;
        last_frame_data+=4;
    }


    // Do the shadow copy...
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        if(*last_frame_data && *frame_data)
        {
            // All are turned on...  Do a copy...
            *frame_data = *last_frame_data;
            frame_data++;  last_frame_data++;
            repeat(j, 8)
            {
                *((float*) frame_data) = *((float*) last_frame_data);
                frame_data+=4;  last_frame_data+=4;
            }
        }
        else
        {
            // At least one is off...  Skip 'em...
            if(*frame_data == 0)
            {
                frame_data++;
            }
            else
            {
                frame_data+=33;
            }
            if(*last_frame_data == 0)
            {
                last_frame_data++;
            }
            else
            {
                last_frame_data+=33;
            }
        }
    }



    // Fix the model for bone length and stuff...
    render_fix_model_to_bone_length(start_data, frame, 9999);
    render_generate_bone_normals(start_data, frame);
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned short original_vertex_angle[MAX_SELECT];
unsigned char global_autoshadow_vertex = 255;
void render_auto_shadow(unsigned char* data, unsigned short frame, unsigned char shadow)
{
    // <ZZ> This function takes the given shadow layer for the given frame and applies it
    //      across all frames of the RDY file...  Rotates and scales the shadow layer to
    //      represent the selected vertices...
    unsigned short flags;
    unsigned char num_detail_level;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_vertex;
    unsigned short num_bone;
    unsigned short num_joint;
    unsigned char* base_model_data;
    unsigned char* vertex_data;
    unsigned char* bone_data;
    unsigned char* frame_data;
    unsigned char* block_start;
    unsigned char base_model, detail_level, original_base_model, alpha;
    unsigned short i, j, vertex;
    float original_centrid_xy[2];
    float current_centrid_xy[2];
    float original_centrid_distance;
    float current_centrid_distance;
    float original_corner_xy[4][2];
    float vector_xyz[3];
    unsigned short current_vertex_angle;
    int average_vertex_angle_change;
    signed short current_vertex_angle_change;
    float angle, sine, cosine, scale;


    // Error check...
    if(shadow > MAX_DDD_SHADOW_TEXTURE || select_num < 1)
    {
        return;
    }


    // Read the model's header block...
    flags = *((unsigned short*) data); data+=2;
    num_detail_level = *data;  data++;
    detail_level = 0;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) frame = 0;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    block_start = data;



    // Find the centrid (xy) for the selected vertices...
    original_centrid_xy[X] = 0.0f;
    original_centrid_xy[Y] = 0.0f;
    repeat(vertex, select_num)
    {
        original_centrid_xy[X] += select_data[vertex][X];
        original_centrid_xy[Y] += select_data[vertex][Y];
    }
    original_centrid_xy[X]/=select_num;
    original_centrid_xy[Y]/=select_num;
log_message("INFO:   Original centrid at %f, %f", original_centrid_xy[X], original_centrid_xy[Y]);



    // Find the average distance (xy) from the centrid to each selected vertex...
    // Find the angle (topview) from the centrid to each selected vertex too...
    original_centrid_distance = 0.0f;
    vector_xyz[Z] = 0.0f;
    repeat(vertex, select_num)
    {
        vector_xyz[X] = select_data[vertex][X] - original_centrid_xy[X];
        vector_xyz[Y] = select_data[vertex][Y] - original_centrid_xy[Y];
        original_centrid_distance += vector_length(vector_xyz);
        original_vertex_angle[vertex] = (unsigned short) (atan2(vector_xyz[Y], vector_xyz[X]) * 65536.0f / (2.0 * PI));
    }
    original_centrid_distance/=select_num;
log_message("INFO:   Original (frame %d) centrid distance is %f", frame, original_centrid_distance);



    // Find the original base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    original_base_model = *(frame_data + 2);

    // Find and remember the shadow data for the original frame...
    data = data + (original_base_model*20) + (num_base_model*20*detail_level);
    base_model_data = *((unsigned char**) data);
    num_joint = *((unsigned short*) (base_model_data+4));
    num_bone = *((unsigned short*) (base_model_data+6));
    frame_data = frame_data + (24*num_bone) + (12*num_joint) + 11;
    // Skip through to the selected shadow layer...
    repeat(j, shadow)
    {
        alpha = frame_data[0];  frame_data++;
        if(alpha > 0) { frame_data+=32; }
    }
    alpha = frame_data[0];  frame_data++;
    if(alpha)
    {
        // Remember the shadow coordinates for the original frame...
        repeat(j, 4)
        {
            original_corner_xy[j][X] = *((float*) frame_data);  frame_data+=4;
            original_corner_xy[j][Y] = *((float*) frame_data);  frame_data+=4;
            original_corner_xy[j][X] -= original_centrid_xy[X];
            original_corner_xy[j][Y] -= original_centrid_xy[Y];
        }
    }
    else
    {
        // Shadow layer isn't active...  That's not gonna work...
        return;
    }




    // Read through each frame of the model...
    repeat(i, num_bone_frame)
    {
        // Go to the current base model
        data = block_start;
        frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(i<<2)));
        base_model = *(frame_data + 2);
        if(frame_data[0] > 0 && i != frame && base_model == original_base_model)
        {
            // Generate the vertex coordinates for this frame...
            data = data + (base_model*20) + (num_base_model*20*detail_level);
            base_model_data = *((unsigned char**) data);  data+=16;
            bone_data = *((unsigned char**) data);
            num_vertex = *((unsigned short*) base_model_data);
            num_joint = *((unsigned short*) (base_model_data+4));
            num_bone = *((unsigned short*) (base_model_data+6));
            vertex_data = base_model_data+8;
            frame_data = frame_data+11;
            render_bone_frame(base_model_data, bone_data, frame_data);


            // Find the centrid of the selected vertices (should now be in different locations from before)
            current_centrid_xy[X] = 0.0f;
            current_centrid_xy[Y] = 0.0f;
            repeat(vertex, select_num)
            {
                current_centrid_xy[X] += select_data[vertex][X];
                current_centrid_xy[Y] += select_data[vertex][Y];
            }
            current_centrid_xy[X]/=select_num;
            current_centrid_xy[Y]/=select_num;
log_message("INFO:   Current (frame %d) centrid at %f, %f", i, current_centrid_xy[X], current_centrid_xy[Y]);


            // Find the new average distance from the centrid for each of the selected vertices...
            // Find the average difference in angle for each of the selected vertices too (as compared to the original)...
            current_centrid_distance = 0.0f;
            vector_xyz[Z] = 0.0f;
            average_vertex_angle_change = 0;
            repeat(vertex, select_num)
            {
                vector_xyz[X] = select_data[vertex][X] - current_centrid_xy[X];
                vector_xyz[Y] = select_data[vertex][Y] - current_centrid_xy[Y];
                current_centrid_distance += vector_length(vector_xyz);
                current_vertex_angle = (unsigned short) (atan2(vector_xyz[Y], vector_xyz[X]) * 65536.0f / (2.0 * PI));

                current_vertex_angle_change = current_vertex_angle - original_vertex_angle[vertex];
                average_vertex_angle_change -= current_vertex_angle_change;
            }
            current_centrid_distance/=select_num;
            average_vertex_angle_change/=select_num;
log_message("INFO:   Current centrid distance is %f", current_centrid_distance);
log_message("INFO:   Average angle change is %f degrees", average_vertex_angle_change * 360.0f / 65536.0f);





            // Jump ahead to the shadow data for this frame...
            frame_data = frame_data + (24*num_bone) + (12*num_joint);


            // Skip through to the selected shadow layer...
            repeat(j, shadow)
            {
                alpha = frame_data[0];  frame_data++;
                if(alpha > 0) { frame_data+=32; }
            }
            alpha = frame_data[0];  frame_data++;
            if(alpha)
            {
                // Apply the original shadow coordinates to the new frame, based on centrid
                // location, average angle difference, and average distance difference....
                // Note: Shadow must be turned on in frame for autoshadow to work...
                angle = average_vertex_angle_change * 2.0f * PI / 65536.0f;
                sine = (float) sin(angle);
                cosine = (float) cos(angle);
                if(original_centrid_distance < 0.000001f)
                {
                    scale = 1.0f;
                }
                else
                {
                    scale = current_centrid_distance / original_centrid_distance;
                }

                repeat(j, 4)
                {
                    if(j == global_autoshadow_vertex || global_autoshadow_vertex > 3)
                    {
                        vector_xyz[X] = (sine*original_corner_xy[j][Y]) + (cosine*original_corner_xy[j][X]);
                        vector_xyz[Y] = (-sine*original_corner_xy[j][X]) + (cosine*original_corner_xy[j][Y]);
                        vector_xyz[X]*=scale;
                        vector_xyz[Y]*=scale;
                        vector_xyz[X]+=current_centrid_xy[X];
                        vector_xyz[Y]+=current_centrid_xy[Y];

                        *((float*) frame_data) = vector_xyz[X];  frame_data+=4;
                        *((float*) frame_data) = vector_xyz[Y];  frame_data+=4;
                    }
                    else
                    {
                        frame_data+=8;
                    }    
                }
            }
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_interpolate(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function averages the joint positions of the current frame with those of the
    //      next and last frames, provided that all of those frames use the same base model.
    //      Also interpolates shadow vertex coordinates...
    unsigned short next_frame;
    unsigned short last_frame;
    unsigned char* frame_data;
    unsigned char* next_frame_data;
    unsigned char* last_frame_data;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char base_model;
    unsigned short i, j;
    unsigned char looking;

//float x, y, z, angle;

    start_data = data;
    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    next_frame = frame+1;
    if(next_frame >= num_bone_frame) next_frame = frame;
    last_frame = frame;
    if(last_frame > 0) last_frame--;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    next_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(next_frame<<2)));
    last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
    base_model = frame_data[2];
    if(next_frame_data[2] != base_model) next_frame_data = frame_data;
    if(last_frame_data[2] != base_model) last_frame_data = frame_data;
    if(next_frame_data[0] != frame_data[0]) next_frame_data = frame_data;
    if(last_frame_data[0] != frame_data[0]) last_frame_data = frame_data;
    if(next_frame_data == frame_data)
    {
        // Find first frame of this action...
        next_frame = frame;
        looking = TRUE;
        while(looking)
        {
            if(next_frame > 0)
            {
                next_frame--;
                next_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(next_frame<<2)));
                if(next_frame_data[2] != base_model) looking = FALSE;
                if(next_frame_data[0] != frame_data[0]) looking = FALSE;
                if(looking == FALSE)
                {
                    next_frame++;
                    next_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(next_frame<<2)));
                }
            }
            else
            {
                looking = FALSE;
                next_frame = frame;
                next_frame_data =  frame_data;
            }
        }
    }
    if(last_frame_data == frame_data)
    {
        // Find the final frame of this action...
        last_frame = frame;
        looking = TRUE;
        while(looking)
        {
            if(last_frame < (num_bone_frame - 1))
            {
                last_frame++;
                last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
                if(last_frame_data[2] != base_model) looking = FALSE;
                if(last_frame_data[0] != frame_data[0]) looking = FALSE;
                if(looking == FALSE)
                {
                    last_frame--;
                    last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
                }
            }
            else
            {
                looking = FALSE;
                last_frame = frame;
                last_frame_data =  frame_data;
            }
        }
    }



    if(next_frame_data == last_frame_data) return;
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    frame_data += 11;
    next_frame_data += 11;
    last_frame_data += 11;











    // Interpolate bone normals
    num_bone = num_bone*6;
    repeat(i, num_bone)
    {
        *((float*) frame_data) = (*((float*) frame_data) + *((float*) frame_data) + *((float*) next_frame_data) + *((float*) last_frame_data)) * 0.25f;
        frame_data+=4;
        next_frame_data+=4;
        last_frame_data+=4;
    }


    // Interpolate joint positions
    num_joint= num_joint*3;
    repeat(i, num_joint)
    {
        *((float*) frame_data) = (*((float*) frame_data) + *((float*) frame_data) + *((float*) next_frame_data) + *((float*) last_frame_data)) * 0.25f;
        frame_data+=4;
        next_frame_data+=4;
        last_frame_data+=4;
    }



    // Do the shadow interpolations...
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        if(*next_frame_data && *last_frame_data && *frame_data)
        {
            // All are turned on...  Do an interpolation...
            j = *frame_data;
            j += *frame_data;
            j += *next_frame_data;
            j += *last_frame_data;
            *frame_data = j>>2;
            frame_data++;  next_frame_data++;  last_frame_data++;
            repeat(j, 8)
            {
                *((float*) frame_data) = (*((float*) frame_data) + *((float*) frame_data) + *((float*) next_frame_data) + *((float*) last_frame_data)) * 0.25f;
                frame_data+=4;  next_frame_data+=4;  last_frame_data+=4;
            }
        }
        else
        {
            // At least one is off...  Skip 'em...
            if(*frame_data == 0)
            {
                frame_data++;
            }
            else
            {
                frame_data+=33;
            }
            if(*next_frame_data == 0)
            {
                next_frame_data++;
            }
            else
            {
                next_frame_data+=33;
            }
            if(*last_frame_data == 0)
            {
                last_frame_data++;
            }
            else
            {
                last_frame_data+=33;
            }
        }
    }



    // Fix the model for bone length and stuff...
    if(!key_down[SDL_SCANCODE_F])
    {
        render_fix_model_to_bone_length(start_data, frame, 9999);
    }
    render_generate_bone_normals(start_data, frame);
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void render_shadow_reset(unsigned char* data, unsigned short frame)
{
    // <ZZ> This function resets the shadow vertex coordinates of the given frame to those
    //      of the last frame...
    unsigned short last_frame;
    unsigned char* frame_data;
    unsigned char* last_frame_data;
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone, num_joint;
    unsigned char* base_model_data;
    unsigned char* start_data;
    unsigned char base_model;
    unsigned short i, j;


    start_data = data;
    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return;
    last_frame = frame;
    if(last_frame > 0) last_frame--;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    last_frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(last_frame<<2)));
    base_model = frame_data[2];
    if(last_frame_data[2] != base_model) return;
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    base_model_data+=4;
    num_joint = *((unsigned short*) base_model_data); base_model_data+=2;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    frame_data += 11;
    last_frame_data += 11;


    // Skip the joint positions...
    num_joint=(num_joint*3) + (num_bone*6);
    frame_data+=(num_joint*4);
    last_frame_data+=(num_joint*4);


    // Do the shadow copy...
    repeat(i, MAX_DDD_SHADOW_TEXTURE)
    {
        if(*last_frame_data && *frame_data)
        {
            // All are turned on...  Do an interpolation...
            *frame_data = *last_frame_data;
            frame_data++;  last_frame_data++;
            repeat(j, 8)
            {
                *((float*) frame_data) = *((float*) last_frame_data);
                frame_data+=4;  last_frame_data+=4;
            }
        }
        else
        {
            // At least one is off...  Skip 'em...
            if(*frame_data == 0)
            {
                frame_data++;
            }
            else
            {
                frame_data+=33;
            }
            if(*last_frame_data == 0)
            {
                last_frame_data++;
            }
            else
            {
                last_frame_data+=33;
            }
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
