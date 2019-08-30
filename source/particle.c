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

// <ZZ> This file contains functions for particles...
float texpos_corner_xy[4][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}};
float texpos_number_xy[4][2] = {{0.0f, 0.1875f}, {1.0f, 0.1875f}, {1.0f, 0.25f}, {0.0f, 0.25f}};

//-----------------------------------------------------------------------------------------------
#define ATTACHED_SCALAR_MAX  4.0f
#define ATTACHED_SCALAR_MULTIPLY 16.0f
#define ATTACHED_SCALAR_DIVIDE   0.0625f
#define ATTACHED_SCALAR_ADD 64
unsigned char particle_attach_to_character(unsigned short particle, unsigned short character, unsigned char bone_type)
{
    // <ZZ> This function attaches a particle to a given character, returning TRUE if it worked,
    //      or FALSE if it didn't...  World bone frame information must've been generated for character
    //      prior to calling...
    unsigned char num_base_model;
    unsigned short num_bone_frame;
    unsigned short num_bone;
    unsigned short num_vertex;
    unsigned char* vertex_data;
    unsigned char* base_model_data;
    unsigned char* bone_data;
    unsigned char* frame_data;
    unsigned short i, j;
    unsigned short nearest_bone;
    unsigned char* nearest_vertex_data;
    float nearest_distance;
    float distance;
    float distance_xyz[3];
    float direction_xyz[3];
    unsigned char base_model;
    unsigned short frame;
    unsigned char* character_data;
    unsigned char* data;
    float* joint_data;
    unsigned short joint[2];
    unsigned char* particle_data;
    unsigned char binding;
    float* particle_xyz;
    float midpoint_xyz[3];
    float length;
    float scalars_xyz[3];


    // Get information from the character variables...
    if(!main_character_on[character])  return FALSE;
    character_data = main_character_data[character];
    data = *((unsigned char**) (character_data + 256));
    if(data == NULL) return FALSE;
    frame = *((unsigned short*) (character_data + 178));


    // Start reading the RDY file...
    data+=3;
    num_base_model = *data;  data++;
    num_bone_frame = *((unsigned short*) data); data+=2;
    if(frame >= num_bone_frame) return FALSE;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);


    // Go to the current base model
    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
    base_model = *(frame_data + 2);
    data = data + (base_model*20);
    base_model_data = *((unsigned char**) data);
    num_bone = *((unsigned short*) (base_model_data+6));


    // Skip to the bone connection info of the base model...
    bone_data = *((unsigned char**) (data+16));


    // Go to the current frame...
    frame_data = temp_character_bone_frame[character];
    joint_data = (float*) (frame_data+(num_bone*24));



    // Make sure frame data is valid...
    if(frame_data)
    {
        // Get the particle information...
        particle_data = main_particle_data[particle];
        particle_xyz = (float*) (particle_data);
        if(PART_FLAGS & PART_SPIN_ON)
        {
            // Single point particle
            midpoint_xyz[X] = particle_xyz[X];
            midpoint_xyz[Y] = particle_xyz[Y];
            midpoint_xyz[Z] = particle_xyz[Z];
        }
        else
        {
            // Double point particle
            midpoint_xyz[X] = (particle_xyz[X] + particle_xyz[3]) * 0.5f;
            midpoint_xyz[Y] = (particle_xyz[Y] + particle_xyz[4]) * 0.5f;
            midpoint_xyz[Z] = (particle_xyz[Z] + particle_xyz[5]) * 0.5f;
        }





        // Bone type 255 is special cased as a vertex-overlaping type...
        if(bone_type == 255)
        {
            // Search for closest vertex in the model...
            nearest_distance = 9999.9f;
            nearest_vertex_data = NULL;


            // Go through each model segment...
            repeat(j, 10)
            {
                // Find out some stuff about our sub-model...
                data = *((unsigned char**) (character_data + 256 + (j*24)));
                if(data)
                {
                    // Start reading the RDY file...
                    data+=3;
                    num_base_model = *data;  data+=3;
                    data+=(ACTION_MAX<<1);
                    data+=(MAX_DDD_SHADOW_TEXTURE);


                    // Assume base model 0...
                    base_model_data = *((unsigned char**) data);
                    num_vertex = *((unsigned short*) base_model_data);
                    num_bone = *((unsigned short*) (base_model_data+6));


                    // Skip to the bone connection info of the base model...
                    vertex_data = base_model_data+8;
                    bone_data = *((unsigned char**) (data+16));


                    // Generate vertex coordinates for this sub-model, based on bone locations...
                    render_bone_frame(base_model_data, bone_data, frame_data);
                    repeat(i, num_vertex)
                    {
                        distance_xyz[X] = (*((float*) vertex_data))     - midpoint_xyz[X];
                        distance_xyz[Y] = (*((float*) (vertex_data+4))) - midpoint_xyz[Y];
                        distance_xyz[Z] = (*((float*) (vertex_data+8))) - midpoint_xyz[Z];
                        distance = vector_length(distance_xyz);

                        // See if it's the best vertex or not...
                        if(distance < nearest_distance)
                        {
                            // Remember it...
                            nearest_vertex_data = vertex_data;
                            nearest_distance = distance;
                        }
                        vertex_data+=64;
                    }
                }
            }

//0  - X
//4  - Y
//8  - Z
//12 - bind A
//13 - bind B
//14 - weight
//15 - normal X
//19 - normal Y
//23 - normal Z
//27 - height percent A
//31 - height percent B
//35 - forward offset A
//39 - forward offset B
//43 - side offset A
//47 - side offset B

            // Did we find a valid vertex?
            if(nearest_vertex_data)
            {
                // Fill in the particle information...
                binding = (nearest_vertex_data[14]>>6)&1;
                particle_data[64] = nearest_vertex_data[12+binding];
            
                // Front end (xyz) of particle...
                binding = binding<<2;
                scalars_xyz[X] = *((float*) (nearest_vertex_data+27+binding));  // Height
                scalars_xyz[Y] = *((float*) (nearest_vertex_data+35+binding));  // Forward
                scalars_xyz[Z] = *((float*) (nearest_vertex_data+43+binding));  // Side
                if(scalars_xyz[X] > ATTACHED_SCALAR_MAX)  scalars_xyz[X] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[X] < -ATTACHED_SCALAR_MAX)  scalars_xyz[X] = -ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Y] > ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Y] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = -ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Z] > ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Z] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = -ATTACHED_SCALAR_MAX;
                particle_data[68] = (unsigned char) ((scalars_xyz[X] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
                particle_data[69] = (unsigned char) ((scalars_xyz[Y] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
                particle_data[70] = (unsigned char) ((scalars_xyz[Z] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);


                // Back end (last_xyz) of particle...
                scalars_xyz[X] *= (1.0f + *((float*) (particle_data+36)));
                scalars_xyz[Y] *= (1.0f + *((float*) (particle_data+36)));
                scalars_xyz[Z] *= (1.0f + *((float*) (particle_data+36)));
                if(scalars_xyz[X] > ATTACHED_SCALAR_MAX)  scalars_xyz[X] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[X] < -ATTACHED_SCALAR_MAX)  scalars_xyz[X] = -ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Y] > ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Y] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = -ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Z] > ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = ATTACHED_SCALAR_MAX;
                if(scalars_xyz[Z] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = -ATTACHED_SCALAR_MAX;
                particle_data[71] = (unsigned char) ((scalars_xyz[X] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
                particle_data[72] = (unsigned char) ((scalars_xyz[Y] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
                particle_data[73] = (unsigned char) ((scalars_xyz[Z] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);


                // Set the particle to be attached to the bone we found...
                (*((unsigned short*) (particle_data+60))) |= PART_STUCK_ON;
                *((unsigned short*) (particle_data+86)) = character;
                return TRUE;
            }
        }
        else
        {
            // Search for the nearest bone...
            nearest_bone = 0;
            nearest_distance = 9999.9f;
            repeat(i, num_bone)
            {
                // Only attach to normal bones, not weapon grips and other types...  Or attach only to weapons...
                if((*bone_data == bone_type))
                {
                    joint[0] = *((unsigned short*) (bone_data+1));
                    joint[1] = *((unsigned short*) (bone_data+3));
                    joint[0]*=3;
                    joint[1]*=3;

                    // Find the bone's midpoint...
                    distance_xyz[X] = (joint_data[joint[0]] + joint_data[joint[1]]) * 0.5f;
                    distance_xyz[Y] = (joint_data[joint[0]+1] + joint_data[joint[1]+1]) * 0.5f;
                    distance_xyz[Z] = (joint_data[joint[0]+2] + joint_data[joint[1]+2]) * 0.5f;

                    // Find the distance from midpoint to midpoint...
                    distance_xyz[X] -= midpoint_xyz[X];
                    distance_xyz[Y] -= midpoint_xyz[Y];
                    distance_xyz[Z] -= midpoint_xyz[Z];
                    distance = vector_length(distance_xyz);

                    // See if it's the best bone or not...
                    if(distance < nearest_distance)
                    {
                        nearest_bone = i;
                        nearest_distance = distance;
                    }
                }
                bone_data+=9;
            }


            // Set the particle to be attached to the bone we found...
            (*((unsigned short*) (particle_data+60))) |= PART_STUCK_ON;
            particle_data[64] = (unsigned char) nearest_bone;
            *((unsigned short*) (particle_data+86)) = character;


            // Figger' the normal scalars for generating the particle position...
            bone_data = *((unsigned char**) (data+16));
            bone_data += (nearest_bone * 9);
            joint[0] = *((unsigned short*) (bone_data+1));
            joint[1] = *((unsigned short*) (bone_data+3));
            joint[0]*=3;
            joint[1]*=3;
            frame_data += (nearest_bone*24);
            direction_xyz[X] = (joint_data[joint[1]] - joint_data[joint[0]]);
            direction_xyz[Y] = (joint_data[joint[1]+1] - joint_data[joint[0]+1]);
            direction_xyz[Z] = (joint_data[joint[1]+2] - joint_data[joint[0]+2]);
            distance_xyz[X] = particle_xyz[X] - joint_data[joint[0]];
            distance_xyz[Y] = particle_xyz[Y] - joint_data[joint[0]+1];
            distance_xyz[Z] = particle_xyz[Z] - joint_data[joint[0]+2];
            length = (*((float*) (bone_data+5)));
            if(length < 0.00001f)  length = 0.00001f;
            length = length*length;
            scalars_xyz[X] = dot_product(direction_xyz, distance_xyz) / (length); // Height
            scalars_xyz[Y] = dot_product(((float*) frame_data), distance_xyz); // Forward
            scalars_xyz[Z] = dot_product(((float*) (frame_data+12)), distance_xyz); // Side
            if(scalars_xyz[X] > ATTACHED_SCALAR_MAX)  scalars_xyz[X] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[X] < -ATTACHED_SCALAR_MAX)  scalars_xyz[X] = -ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Y] > ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Y] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = -ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Z] > ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Z] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = -ATTACHED_SCALAR_MAX;
            particle_data[68] = (unsigned char) ((scalars_xyz[X] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
            particle_data[69] = (unsigned char) ((scalars_xyz[Y] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
            particle_data[70] = (unsigned char) ((scalars_xyz[Z] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);





            // Figger' the normal scalars for generating the particle's last position...
            particle_xyz = (float*) (particle_data+12);
            direction_xyz[X] = (joint_data[joint[1]] - joint_data[joint[0]]);
            direction_xyz[Y] = (joint_data[joint[1]+1] - joint_data[joint[0]+1]);
            direction_xyz[Z] = (joint_data[joint[1]+2] - joint_data[joint[0]+2]);
            distance_xyz[X] = particle_xyz[X] - joint_data[joint[0]];
            distance_xyz[Y] = particle_xyz[Y] - joint_data[joint[0]+1];
            distance_xyz[Z] = particle_xyz[Z] - joint_data[joint[0]+2];
            scalars_xyz[X] = dot_product(direction_xyz, distance_xyz) / (length); // Height
            scalars_xyz[Y] = dot_product(((float*) frame_data), distance_xyz); // Forward
            scalars_xyz[Z] = dot_product(((float*) (frame_data+12)), distance_xyz); // Side
            if(scalars_xyz[X] > ATTACHED_SCALAR_MAX)  scalars_xyz[X] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[X] < -ATTACHED_SCALAR_MAX)  scalars_xyz[X] = -ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Y] > ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Y] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Y] = -ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Z] > ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = ATTACHED_SCALAR_MAX;
            if(scalars_xyz[Z] < -ATTACHED_SCALAR_MAX)  scalars_xyz[Z] = -ATTACHED_SCALAR_MAX;
            particle_data[71] = (unsigned char) ((scalars_xyz[X] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
            particle_data[72] = (unsigned char) ((scalars_xyz[Y] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
            particle_data[73] = (unsigned char) ((scalars_xyz[Z] * ATTACHED_SCALAR_MULTIPLY) + ATTACHED_SCALAR_ADD);
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
void particle_draw(unsigned char* particle_data)
{
    // <ZZ> This function draws a particle.  Culling should be off.  Blend mode should be
    //      set prior to call.
    float* current_xyz;
    float last_xyz[3];
    float line_xyz[3];
    float offset_xyz[3];
    float up_xyz[3] = {0.0f, 0.0f, 1.0f};
    float height;
    unsigned char* texture;
    unsigned short flags;
float bad_rotation;
    unsigned char number, digit;
    float cosine;
    float sine;
    unsigned char color[4];


    // Figure out the texture...
    texture = *((unsigned char**) (particle_data+44));
    if(texture ==  NULL) return;
    texture+=2;
    display_pick_texture(*((unsigned int*) texture));


    // Get the particle color...
    color[0] = particle_data[48];
    color[1] = particle_data[49];
    color[2] = particle_data[50];
    // Modify lighting for falling in pits...
    if(((float*) particle_data)[Z] < ROOM_PIT_HIGH_LEVEL)
    {
        if(((float*) particle_data)[Z] < ROOM_PIT_LOW_LEVEL)
        {
            color[0] = 0;
            color[1] = 0;
            color[2] = 0;
        }
        else
        {
            number = ((unsigned char) 255) - ((unsigned char) ((16.0f * (ROOM_PIT_HIGH_LEVEL - ((float*) particle_data)[Z]))));
            color[0] = color[0]*number>>8;
            color[1] = color[1]*number>>8;
            color[2] = color[2]*number>>8;
        }
    }
    color[3] = particle_data[79];
    display_color_alpha(color);



    // Two different styles of rendering...  Line like between current and last points...
    // Or single point with a spinny amount...
    current_xyz = (float*) particle_data;
    flags = *((unsigned short*)(particle_data+60));
    if(flags & PART_SPIN_ON)
    {
        // One point rendering...
        // Can be either normal spinny particle (aligned against camera) or a flat spinny
        // particle (flat on xy plane)
        if(flags & PART_FLAT_ON)
        {
            // Calculate the positions of the corners of the flat particle...
            bad_rotation = *((unsigned short*)(particle_data+56)) * 2.0f * PI / 65536.0f;
            cosine = ((float) cos(bad_rotation))*0.5f;
            sine = ((float) sin(bad_rotation))*0.5f;


            // Rotate according to spinny amount...
            line_xyz[X] = sine;
            line_xyz[Y] = cosine;
            offset_xyz[X] = cosine;
            offset_xyz[Y] = -sine;


            // Scale by length & width...
            line_xyz[X] *= (*((float*)(particle_data+36)));
            line_xyz[Y] *= (*((float*)(particle_data+36)));
            offset_xyz[X] *= (*((float*)(particle_data+40)));
            offset_xyz[Y] *= (*((float*)(particle_data+40)));


            // Render the particle...
            last_xyz[Z] = current_xyz[Z];
            display_start_fan();
                last_xyz[X] = current_xyz[X] + line_xyz[X]-offset_xyz[X];
                last_xyz[Y] = current_xyz[Y] + line_xyz[Y]-offset_xyz[Y];
                display_texpos(texpos_corner_xy[0]);  display_vertex(last_xyz);

                last_xyz[X] = current_xyz[X] + line_xyz[X]+offset_xyz[X];
                last_xyz[Y] = current_xyz[Y] + line_xyz[Y]+offset_xyz[Y];
                display_texpos(texpos_corner_xy[1]);  display_vertex(last_xyz);

                last_xyz[X] = current_xyz[X] - line_xyz[X]+offset_xyz[X];
                last_xyz[Y] = current_xyz[Y] - line_xyz[Y]+offset_xyz[Y];
                display_texpos(texpos_corner_xy[2]);  display_vertex(last_xyz);

                last_xyz[X] = current_xyz[X] - line_xyz[X]-offset_xyz[X];
                last_xyz[Y] = current_xyz[Y] - line_xyz[Y]-offset_xyz[Y];
                display_texpos(texpos_corner_xy[3]);  display_vertex(last_xyz);
            display_end();
        }
        else
        {
            last_xyz[X] = rotate_camera_matrix[2]*0.5f;
            last_xyz[Y] = rotate_camera_matrix[6]*0.5f;
            last_xyz[Z] = rotate_camera_matrix[10]*0.5f;


            offset_xyz[X] = rotate_camera_matrix[0]*0.5f;
            offset_xyz[Y] = rotate_camera_matrix[4]*0.5f;
            offset_xyz[Z] = rotate_camera_matrix[8]*0.5f;


            // !!!BAD!!!
            // !!!BAD!!!  Need to redo for precomputed sin/cos
            // !!!BAD!!!
            bad_rotation = *((unsigned short*)(particle_data+56)) * 2.0f * PI / 65536.0f;
            cosine = (float) cos(bad_rotation);
            sine = (float) sin(bad_rotation);


            // Rotate according to spinny amount...
            line_xyz[X] = sine*offset_xyz[X] + cosine*last_xyz[X];
            line_xyz[Y] = sine*offset_xyz[Y] + cosine*last_xyz[Y];
            line_xyz[Z] = sine*offset_xyz[Z] + cosine*last_xyz[Z];
            sine = -sine;
            offset_xyz[X] = sine*last_xyz[X] + cosine*offset_xyz[X];
            offset_xyz[Y] = sine*last_xyz[Y] + cosine*offset_xyz[Y];
            offset_xyz[Z] = sine*last_xyz[Z] + cosine*offset_xyz[Z];


            // Scale by length and width...
            line_xyz[X] *= (*((float*)(particle_data+36)));
            line_xyz[Y] *= (*((float*)(particle_data+36)));
            line_xyz[Z] *= (*((float*)(particle_data+36)));
            offset_xyz[X] *= (*((float*)(particle_data+40)));
            offset_xyz[Y] *= (*((float*)(particle_data+40)));
            offset_xyz[Z] *= (*((float*)(particle_data+40)));


            if(*((unsigned short*)  (particle_data+60)) & PART_NUMBER_ON)
            {
                // Particle is a number particle...
                number = particle_data[51];
                if(number < 10)
                {
                    // Single digit number...  Generate texture coordinates...  Y already filled in...
                    texpos_number_xy[0][X] = number*0.0625f;
                    texpos_number_xy[1][X] = (number+1)*0.0625f;
                    texpos_number_xy[2][X] = texpos_number_xy[1][X];
                    texpos_number_xy[3][X] = texpos_number_xy[0][X];



                    // Render the particle...
                    display_start_fan();
                        last_xyz[X] = current_xyz[X] + line_xyz[X]-offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y]-offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z]-offset_xyz[Z];
                        display_texpos(texpos_number_xy[0]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] + line_xyz[X]+offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y]+offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z]+offset_xyz[Z];
                        display_texpos(texpos_number_xy[1]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X]+offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y]+offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z]+offset_xyz[Z];
                        display_texpos(texpos_number_xy[2]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X]-offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y]-offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z]-offset_xyz[Z];
                        display_texpos(texpos_number_xy[3]);  display_vertex(last_xyz);
                    display_end();
                }
                else
                {
                    // Double digit number (limit to 99)
                    if(number > 99) number = 99;
                    offset_xyz[X] += offset_xyz[X];
                    offset_xyz[Y] += offset_xyz[Y];
                    offset_xyz[Z] += offset_xyz[Z];


                    // Draw the high digit first...
                    digit = number/10;
                    texpos_number_xy[0][X] = digit*0.0625f;
                    texpos_number_xy[1][X] = (digit+1)*0.0625f;
                    texpos_number_xy[2][X] = texpos_number_xy[1][X];
                    texpos_number_xy[3][X] = texpos_number_xy[0][X];
                    display_start_fan();
                        last_xyz[X] = current_xyz[X] + line_xyz[X]-offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y]-offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z]-offset_xyz[Z];
                        display_texpos(texpos_number_xy[0]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] + line_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z];
                        display_texpos(texpos_number_xy[1]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z];
                        display_texpos(texpos_number_xy[2]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X]-offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y]-offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z]-offset_xyz[Z];
                        display_texpos(texpos_number_xy[3]);  display_vertex(last_xyz);
                    display_end();



                    // Draw the low digit second...
                    digit = number%10;
                    texpos_number_xy[0][X] = digit*0.0625f;
                    texpos_number_xy[1][X] = (digit+1)*0.0625f;
                    texpos_number_xy[2][X] = texpos_number_xy[1][X];
                    texpos_number_xy[3][X] = texpos_number_xy[0][X];
                    display_start_fan();
                        last_xyz[X] = current_xyz[X] + line_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z];
                        display_texpos(texpos_number_xy[0]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] + line_xyz[X]+offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] + line_xyz[Y]+offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] + line_xyz[Z]+offset_xyz[Z];
                        display_texpos(texpos_number_xy[1]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X]+offset_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y]+offset_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z]+offset_xyz[Z];
                        display_texpos(texpos_number_xy[2]);  display_vertex(last_xyz);

                        last_xyz[X] = current_xyz[X] - line_xyz[X];
                        last_xyz[Y] = current_xyz[Y] - line_xyz[Y];
                        last_xyz[Z] = current_xyz[Z] - line_xyz[Z];
                        display_texpos(texpos_number_xy[3]);  display_vertex(last_xyz);
                    display_end();
                }
            }
            else
            {
                // Render the particle...
                display_start_fan();
                    last_xyz[X] = current_xyz[X] + line_xyz[X]-offset_xyz[X];
                    last_xyz[Y] = current_xyz[Y] + line_xyz[Y]-offset_xyz[Y];
                    last_xyz[Z] = current_xyz[Z] + line_xyz[Z]-offset_xyz[Z];
                    display_texpos(texpos_corner_xy[0]);  display_vertex(last_xyz);

                    last_xyz[X] = current_xyz[X] + line_xyz[X]+offset_xyz[X];
                    last_xyz[Y] = current_xyz[Y] + line_xyz[Y]+offset_xyz[Y];
                    last_xyz[Z] = current_xyz[Z] + line_xyz[Z]+offset_xyz[Z];
                    display_texpos(texpos_corner_xy[1]);  display_vertex(last_xyz);

                    last_xyz[X] = current_xyz[X] - line_xyz[X]+offset_xyz[X];
                    last_xyz[Y] = current_xyz[Y] - line_xyz[Y]+offset_xyz[Y];
                    last_xyz[Z] = current_xyz[Z] - line_xyz[Z]+offset_xyz[Z];
                    display_texpos(texpos_corner_xy[2]);  display_vertex(last_xyz);

                    last_xyz[X] = current_xyz[X] - line_xyz[X]-offset_xyz[X];
                    last_xyz[Y] = current_xyz[Y] - line_xyz[Y]-offset_xyz[Y];
                    last_xyz[Z] = current_xyz[Z] - line_xyz[Z]-offset_xyz[Z];
                    display_texpos(texpos_corner_xy[3]);  display_vertex(last_xyz);
                display_end();
            }
        }
    }
    else
    {
        // Two point rendering...
        last_xyz[X] = *((float*) (particle_data+12));
        last_xyz[Y] = *((float*) (particle_data+16));
        last_xyz[Z] = *((float*) (particle_data+20));

        line_xyz[X] = current_xyz[X] - last_xyz[X];
        line_xyz[Y] = current_xyz[Y] - last_xyz[Y];
        line_xyz[Z] = current_xyz[Z] - last_xyz[Z];

        // Change last xyz if fit to length flag is set...
        if(flags & PART_FIT_LENGTH_ON)
        {
            height = vector_length(line_xyz);
            // Scale by length...
            if(height > 0.00001f)
            {
                height /= *((float*)(particle_data+36));
                line_xyz[X] /= height;
                line_xyz[Y] /= height;
                line_xyz[Z] /= height;
                last_xyz[X] = current_xyz[X] - line_xyz[X];
                last_xyz[Y] = current_xyz[Y] - line_xyz[Y];
                last_xyz[Z] = current_xyz[Z] - line_xyz[Z];
            }
        }
        else
        {
            height = *((float*)(particle_data+36));
            if(height > 1.001f)
            {
                line_xyz[X] *= height;
                line_xyz[Y] *= height;
                line_xyz[Z] *= height;
                last_xyz[X] = current_xyz[X] - line_xyz[X];
                last_xyz[Y] = current_xyz[Y] - line_xyz[Y];
                last_xyz[Z] = current_xyz[Z] - line_xyz[Z];
            }
        }



        if(flags & PART_FLAT_ON)
        {
            cross_product(line_xyz, up_xyz, offset_xyz);
        }
        else
        {
            cross_product(line_xyz, camera_fore_xyz, offset_xyz);
        }
        height = vector_length(offset_xyz);
        if(height > 0.00001f)
        {
            height*=2.0f;
            // Scale by width...
            height /= *((float*)(particle_data+40));
            offset_xyz[X] /= height;
            offset_xyz[Y] /= height;
            offset_xyz[Z] /= height;


            display_start_fan();
                line_xyz[X] = current_xyz[X] - offset_xyz[X];
                line_xyz[Y] = current_xyz[Y] - offset_xyz[Y];
                line_xyz[Z] = current_xyz[Z] - offset_xyz[Z];
                display_texpos(texpos_corner_xy[0]);  display_vertex(line_xyz);

                line_xyz[X] = current_xyz[X] + offset_xyz[X];
                line_xyz[Y] = current_xyz[Y] + offset_xyz[Y];
                line_xyz[Z] = current_xyz[Z] + offset_xyz[Z];
                display_texpos(texpos_corner_xy[1]);  display_vertex(line_xyz);

                line_xyz[X] = last_xyz[X] + offset_xyz[X];
                line_xyz[Y] = last_xyz[Y] + offset_xyz[Y];
                line_xyz[Z] = last_xyz[Z] + offset_xyz[Z];
                display_texpos(texpos_corner_xy[2]);  display_vertex(line_xyz);

                line_xyz[X] = last_xyz[X] - offset_xyz[X];
                line_xyz[Y] = last_xyz[Y] - offset_xyz[Y];
                line_xyz[Z] = last_xyz[Z] - offset_xyz[Z];
                display_texpos(texpos_corner_xy[3]);  display_vertex(line_xyz);
            display_end();
        }
    }
}

//-----------------------------------------------------------------------------------------------
void particle_draw_below_water()
{
    // <ZZ> This function draws all of the particles in the room that are below the water level...
    unsigned short i;


    // Change some display settings...
    display_cull_off();
    display_zbuffer_write_off();



    // Draw all of the transparent alpha particles...
    display_blend_trans();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if(!(*((unsigned short*)(main_particle_data[i]+60)) & (PART_LIGHT_ON)) && (*((float*) (main_particle_data[i]+8)) <= room_water_level) && !(*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER)))
            {
                if(main_particle_data[i][79] < 255)
                {
                    particle_draw(main_particle_data[i]);
                }
            }
        }
    }


    // Draw all of the solid alpha particles...
    display_blend_trans();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if(!(*((unsigned short*)(main_particle_data[i]+60)) & (PART_LIGHT_ON)) && (*((float*) (main_particle_data[i]+8)) <= room_water_level) && !(*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER)))
            {
                if(main_particle_data[i][79] > 254)
                {
                    particle_draw(main_particle_data[i]);
                }
            }
        }
    }


    // Draw all of the light particles...
    display_blend_light();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if((*((unsigned short*)(main_particle_data[i]+60)) & PART_LIGHT_ON) && (*((float*) (main_particle_data[i]+8)) <= room_water_level) && !(*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER)))
            {
                particle_draw(main_particle_data[i]);
            }
        }
    }


    // Reset the video stuff to what it was...
    display_zbuffer_write_on();
    display_cull_on();
}

//-----------------------------------------------------------------------------------------------
void particle_draw_above_water()
{
    // <ZZ> This function draws all of the particles in the room that are above the water level...
    unsigned short i;


    // Change some display settings...
    display_cull_off();
    display_zbuffer_write_off();


    // Draw all of the transparent alpha particles...
    display_blend_trans();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if(!(*((unsigned short*)(main_particle_data[i]+60)) & (PART_LIGHT_ON)) && ((*((float*) (main_particle_data[i]+8)) > room_water_level) || (*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER))))
            {
                if(main_particle_data[i][79] < 255)
                {
                    particle_draw(main_particle_data[i]);
                }
            }
        }
    }


    // Draw all of the solid alpha particles...
    display_blend_trans();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if(!(*((unsigned short*)(main_particle_data[i]+60)) & (PART_LIGHT_ON)) && ((*((float*) (main_particle_data[i]+8)) > room_water_level) || (*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER))))
            {
                if(main_particle_data[i][79] > 254)
                {
                    particle_draw(main_particle_data[i]);
                }
            }
        }
    }


    // Draw all of the light particles...
    display_blend_light();
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            if((*((unsigned short*)(main_particle_data[i]+60)) & PART_LIGHT_ON) && ((*((float*) (main_particle_data[i]+8)) > room_water_level) || (*((unsigned short*)(main_particle_data[i]+60)) & (PART_AFTER_WATER))))
            {
                particle_draw(main_particle_data[i]);
            }
        }
    }


    // Reset the video stuff to what it was...
    display_zbuffer_write_on();
    display_cull_on();
}

//-----------------------------------------------------------------------------------------------
void particle_update_all()
{
    // <ZZ> This function moves the particles in the room, calls their refreshes, updates
    //      timers, etc.
    unsigned short i, j, character, target, stuckto;
    unsigned char* particle_data;
    unsigned short flags;
    float distance, collide_distance;
    unsigned char* data;
    unsigned char* frame_data;
    unsigned char* base_model_data;
    unsigned char* character_data;
    unsigned char* target_data;
    float x, y, z;
    float* joint_data;
    unsigned short joint[2];
    unsigned short num_bone, bone, num_base_model, num_bone_frame, frame;
    unsigned char base_model;
    unsigned char frame_event_flags;
    float scalar_xyz[3];
    unsigned short fast_count;
    #ifdef CAN_BLOW_SELF_UP
        unsigned char old_team;
    #endif


    // Update all of the particles...
    fast_count = 0;
    repeat(i, MAX_PARTICLE)
    {
        if(main_particle_on[i])
        {
            particle_data = main_particle_data[i];
            flags = *((unsigned short*) (particle_data+60));


            // Spin
            *((unsigned short*) (particle_data+56)) += *((unsigned short*) (particle_data+58));



            // Update position for stuck particles...
            if(flags & PART_STUCK_ON)
            {
                // Figger out who we're stuck to...
                bone = particle_data[64];
                character = (*((unsigned short*) (particle_data+86))) & (MAX_CHARACTER-1);
                character_data = main_character_data[character];
                data = *((unsigned char**) (character_data + 256));
                if(main_character_on[character] && data != NULL)
                {
                    // Get information from the character variables...
                    frame = *((unsigned short*) (character_data + 178));

                    // Start reading the RDY file...
                    data+=3;
                    num_base_model = *data;  data++;
                    num_bone_frame = *((unsigned short*) data); data+=2;
                    if(frame >= num_bone_frame) frame = 0;
                    data+=(ACTION_MAX<<1);
                    data+=(MAX_DDD_SHADOW_TEXTURE);


                    // Go to the current base model
                    frame_data =  *((unsigned char**) (data+(num_base_model*20*DETAIL_LEVEL_MAX)+(frame<<2)));
                    base_model = *(frame_data + 2);
                    data = data + (base_model*20);
                    base_model_data = *((unsigned char**) data);
                    base_model_data+=6;
                    num_bone = *((unsigned short*) base_model_data);
                    if(bone >= num_bone) bone = 0;


                    // Skip to the bone connection info of the base model...
                    base_model_data = *((unsigned char**) (data+16));
                    base_model_data += bone*9;
                    joint[0] = *((unsigned short*) (base_model_data+1));
                    joint[1] = *((unsigned short*) (base_model_data+3));
                    joint[0]*=3;
                    joint[1]*=3;


                    // Go to the current frame...
                    frame_data = temp_character_bone_frame[character];
                    if(frame_data)
                    {
                        joint_data = (float*) (frame_data+(num_bone*24));
                        frame_data+= (24*bone);


                        // Find the particle position...  Not too complicated...
                        scalar_xyz[X] = (((signed short) particle_data[68]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        scalar_xyz[Y] = (((signed short) particle_data[69]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        scalar_xyz[Z] = (((signed short) particle_data[70]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        *((float*) (particle_data)) = joint_data[joint[0]] + (((joint_data[joint[1]] - joint_data[joint[0]])) * scalar_xyz[X]) + ((*((float*) (frame_data))) * scalar_xyz[Y]) + ((*((float*) (frame_data+12))) * scalar_xyz[Z]);
                        *((float*) (particle_data+4)) = joint_data[joint[0]+1] + (((joint_data[joint[1]+1] - joint_data[joint[0]+1])) * scalar_xyz[X]) + ((*((float*) (frame_data+4))) * scalar_xyz[Y]) + ((*((float*) (frame_data+16))) * scalar_xyz[Z]);
                        *((float*) (particle_data+8)) = joint_data[joint[0]+2] + (((joint_data[joint[1]+2] - joint_data[joint[0]+2])) * scalar_xyz[X]) + ((*((float*) (frame_data+8))) * scalar_xyz[Y]) + ((*((float*) (frame_data+20))) * scalar_xyz[Z]);


                        // Find the last particle position...  Not too complicated...
                        scalar_xyz[X] = (((signed short) particle_data[71]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        scalar_xyz[Y] = (((signed short) particle_data[72]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        scalar_xyz[Z] = (((signed short) particle_data[73]) - ATTACHED_SCALAR_ADD) * ATTACHED_SCALAR_DIVIDE;
                        *((float*) (particle_data+12)) = joint_data[joint[0]] + (((joint_data[joint[1]] - joint_data[joint[0]])) * scalar_xyz[X]) + ((*((float*) (frame_data))) * scalar_xyz[Y]) + ((*((float*) (frame_data+12))) * scalar_xyz[Z]);
                        *((float*) (particle_data+16)) = joint_data[joint[0]+1] + (((joint_data[joint[1]+1] - joint_data[joint[0]+1])) * scalar_xyz[X]) + ((*((float*) (frame_data+4))) * scalar_xyz[Y]) + ((*((float*) (frame_data+16))) * scalar_xyz[Z]);
                        *((float*) (particle_data+20)) = joint_data[joint[0]+2] + (((joint_data[joint[1]+2] - joint_data[joint[0]+2])) * scalar_xyz[X]) + ((*((float*) (frame_data+8))) * scalar_xyz[Y]) + ((*((float*) (frame_data+20))) * scalar_xyz[Z]);
                    }
                }
                else
                {
                    // Poof particle...  Either character it was attached to poof'd, or that character's model has become invalid...
                    // Should automatically get poofed when obj_destroy() is called for character, so this is a double check...
                    obj_destroy(main_particle_data[i]);
                }
            }
            // Update motion...
            else
            {
                // Do water collisions...
                if((*((float*) (particle_data+8))) < room_water_level && (flags & PART_WALL_COLLISION_ON))
                {
                    // Particle is now in water...
                    if(!(flags & PART_IN_WATER))
                    {
                        // But wasn it in water last frame?
                        particle_data[67] = (particle_data[84] == TRUE) ? EVENT_SPAWNED_IN_WATER : EVENT_HIT_WATER;
                        fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                        PART_FLAGS |= PART_IN_WATER;
                    }
                }
                else
                {
                    // Particle is not in water...
                    PART_FLAGS &= (65535-PART_IN_WATER);
                }
                // Clear the Just Spawn'd flag...
                particle_data[84] = 0;


                // Remember last_xyz...
                if(particle_data[65])
                {
                    // Trail is set, so the last_xyz gets sluggified...
                    distance = particle_data[65]/255.0f;
                    collide_distance = 1.0f-distance;
                    *((float*) (particle_data+12)) = distance*(*((float*) (particle_data+12))) + collide_distance*(*((float*) (particle_data)));
                    *((float*) (particle_data+16)) = distance*(*((float*) (particle_data+16))) + collide_distance*(*((float*) (particle_data+4)));
                    *((float*) (particle_data+20)) = distance*(*((float*) (particle_data+20))) + collide_distance*(*((float*) (particle_data+8)));
                }
                else
                {
                    // Last_xyz is the actual last position (normal way it works)
                    *((float*) (particle_data+12)) = *((float*) (particle_data));
                    *((float*) (particle_data+16)) = *((float*) (particle_data+4));
                    *((float*) (particle_data+20)) = *((float*) (particle_data+8));
                }

                // Gravity
                if(flags & PART_GRAVITY_ON)
                {
                    *((float*) (particle_data+32)) += GRAVITY;

                    if(flags & PART_IN_WATER)
                    {
                        // Solid objects (affected by gravity and wall collisions) should be slowed by water friction...
                        (*((float*) (particle_data+24))) *= 0.90f;
                        (*((float*) (particle_data+28))) *= 0.90f;
                        (*((float*) (particle_data+32))) *= 0.75f;
                    }
                }


                // Motion...
                *((float*) (particle_data))   += *((float*) (particle_data+24));
                *((float*) (particle_data+4)) += *((float*) (particle_data+28));
                *((float*) (particle_data+8)) += *((float*) (particle_data+32));
            }



            // Do wall/floor collisions...
            if(flags & PART_WALL_COLLISION_ON)
            {
                x = *((float*) (particle_data));
                y = *((float*) (particle_data+4));
                z = room_heightmap_height(roombuffer, x, y);
                if(z > ((*((float*) (particle_data+8)))-(*((float*) (particle_data+80)))))
                {
                    // Bottom of particle (gotten using colsize) is below the floor...
                    x = *((float*) (particle_data+32));  // vz...
                    ABS(x);
                    if(z > ((*((float*) (particle_data+8)))+x))
                    {
                        // Top of particle (gotten using vz) is also below the floor...
                        // That sounds like a wall to me...
                        particle_data[67] = EVENT_HIT_WALL;
                        fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                    }
                    else
                    {
                        // Looks like a floor...
                        particle_data[67] = EVENT_HIT_FLOOR;
                        fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                    }
                }
            }




            // Do particle-door collisions...
            if(((flags & PART_CHAR_COLLISION_ON) && ((!(flags & PART_SLOW_ON)) || (main_game_frame&3) == 0)) || (flags & PART_WALL_COLLISION_ON))
            {
                // Size of particle...
                collide_distance = *((float*) (particle_data+40));
                collide_distance *= 0.5f;
                repeat(character, MAX_CHARACTER)
                {
                    character_data = main_character_data[character];
                    if(main_character_on[character] && (character_data[104] & MORE_FLAG_DOOR) && (*((unsigned short*) (character_data+170)) == 0))
                    {
                        // The character is an open/closed door...
                        // Open doors stop particless from going through 'em if the particle is too high up (like an
                        // arrow flying through the wall above the door)...
                        x = (*((float*) (particle_data))) - (*((float*) (character_data)));
                        y = (*((float*) (particle_data+4))) - (*((float*) (character_data+4)));
                        distance = x*x + y*y;
                        z = collide_distance + *((float*) (character_data+160));
                        z = z*z;  // Minimum distance between two before a collision (squared to make check easier)
                        if(distance < z)
                        {
                            // Then check the dot to divide the door in two (front and back)...
                            distance = ((*((float*) (character_data+120))) * x) + ((*((float*) (character_data+124))) * y);
                            distance = collide_distance + 0.5f - distance;
                            if(distance > 0.0f)
                            {
                                // Particle is on the other side of the door...  Might be okay if the
                                // door is open (Knock Out)...
                                z = (((*((float*) (character_data+8)))+character_data[175])-((*((float*) (particle_data+8)))+(*((float*) (particle_data+80))))); // Distance between particle and top of door passage (in feet)
                                if(character_data[66] != ACTION_KNOCK_OUT || (z < 0.0f))
                                {
                                    // We have a collision...  Run the bump event...
                                    if((flags & PART_CHAR_COLLISION_ON))
                                    {
                                        // Particle is some sort of attack particle...
                                        particle_data[67] = EVENT_HIT_CHARACTER;
                                        stuckto = (*((unsigned short*) (particle_data+86))) & (MAX_CHARACTER-1);

                                        // Particles that are stuck to door shouldn't hit it...  Otherwise arrows keep triggering a hit...
                                        if(stuckto != character)
                                        {
                                            // Check to see if the door is "blocking" (frame event flags on metal grate door - so it triggers a blocked hit animation...)
                                            frame_event_flags = 0;
                                            frame = *((unsigned short*) (character_data + 178));
                                            data = (*((unsigned char**) (character_data+256)))+3;
                                            num_base_model = *data;  data++;
                                            num_bone_frame = *((unsigned short*) data); data+=2;
                                            data+=(ACTION_MAX<<1);
                                            data+=(MAX_DDD_SHADOW_TEXTURE);
                                            data+=(num_base_model*20*DETAIL_LEVEL_MAX);
                                            if(frame < num_bone_frame)
                                            {
                                                data =  *((unsigned char**) (data+(frame<<2)));
                                                frame_event_flags = *(data+1);
                                                if(frame_event_flags & FRAME_EVENT_BLOCK)
                                                {
                                                    particle_data[67] = EVENT_BLOCKED;
                                                }
                                            }

                                            target = (*((unsigned short*) (particle_data+62))) & (MAX_CHARACTER-1);
                                            *((unsigned short*) (particle_data+62)) = character;  // Set the .target...
                                            fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                                            *((unsigned short*) (particle_data+62)) = target;
                                        }
                                    }
                                    else
                                    {
                                        // Particle is just a bouncy particle hitting into the door...
                                        particle_data[67] = EVENT_HIT_WALL;
                                        fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                                    }
                                    // Don't bother with any further door collisions for this particle...
                                    character = MAX_CHARACTER;
                                }
                            }
                        }
                    }
                }
            }


            // Do particle-character collisions...
            if((flags & PART_CHAR_COLLISION_ON) && ((!(flags & PART_SLOW_ON)) || (main_game_frame&3) == 0))
            {
                // Size of particle...
                collide_distance = *((float*) (particle_data+40));
                collide_distance *= 0.5f;
                repeat(j, num_character_collision_list)
                {
                    // Check teams...
                    character = character_collision_list[j];
                    character_data = main_character_data[character];


                    // X and Y collision...
                    x = (*((float*) (character_data))) - (*((float*) (particle_data)));
                    y = (*((float*) (character_data+4))) - (*((float*) (particle_data+4)));
                    distance = x*x + y*y;
                    z = (*((float*) (character_data+160)));
                    z += collide_distance;
                    z = z*z;  // distance to check square'd...
                    if(distance < z)
                    {
                        distance = (*((float*) (character_data+8))) - (*((float*) (particle_data+8)));
                        if(distance < 0.0f)
                        {
                            distance = -(distance + ((float) (character_data[175])));
                        }
                        if(distance < collide_distance)
                        {
                            // Make sure character isn't invincible, or the particle's owner...
//                            if(((*((unsigned short*) (character_data+170))) == 0) && (((*((unsigned short*)(particle_data+76)) != character) && (*((unsigned short*)(particle_data+76)) != *((unsigned short*)(character_data+106)))) || ((*((unsigned short*)(particle_data+60)) & PART_HIT_OWNER_ON) && !(character_data[105] & VIRTUE_FLAG_IMMUNE))  ))
//(*((unsigned short*) (character_data+170)) == 0)
//(*((unsigned short*) (particle_data+76)) != character)
//(*((unsigned short*) (particle_data+76)) != *((unsigned short*) (character_data+106)))
//(*((unsigned short*)(particle_data+60)) & PART_HIT_OWNER_ON)
//(character_data[105] & VIRTUE_FLAG_IMMUNE)
//((character.invincible == 0) || (part.flags & PART_IGNORE_INTIMER)) && (((particle.owner != character.index) && (particle.owner != character.rider)) || ((particle.flags & PART_HIT_OWNER_ON) && !(character.vflags & VIRTUE_FLAG_IMMUNE)) )


          

                            if(((*((unsigned short*) (character_data+170)) == 0) || (*((unsigned short*) (particle_data+60)) & PART_IGNORE_INTIMER))&& (((*((unsigned short*) (particle_data+76)) != character) && (*((unsigned short*) (particle_data+76)) != *((unsigned short*) (character_data+106)))) || ((*((unsigned short*)(particle_data+60)) & PART_HIT_OWNER_ON) && !(character_data[105] & VIRTUE_FLAG_IMMUNE)) ))
                            {
                                // Make sure the particle is still on...  May have poofed on a wall...  Also make sure it's still doing collisions...
                                if(main_particle_on[i] && ((*((unsigned short*) (particle_data+60))) & PART_CHAR_COLLISION_ON))
                                {
                                    // We have a particle-character collision...  Need some sort of callback...
                                    particle_data[67] = EVENT_HIT_CHARACTER;



//log_message("INFO:   Character %d hit by particle %d", character, i);

                                    // But the particle might've been blocked...  Check for that...
                                    // Need to get the frame event flags...
                                    frame_event_flags = 0;
                                    frame = *((unsigned short*) (character_data + 178));
//log_message("INFO:     Character frame == %d", frame);
//log_message("INFO:     Character action == %d", character_data[65]);
//log_message("INFO:     Character daction == %d", character_data[66]);
                                    data = (*((unsigned char**) (character_data+256)))+3;
                                    num_base_model = *data;  data++;
                                    num_bone_frame = *((unsigned short*) data); data+=2;
                                    data+=(ACTION_MAX<<1);
                                    data+=(MAX_DDD_SHADOW_TEXTURE);
                                    data+=(num_base_model*20*DETAIL_LEVEL_MAX);
                                    if(frame < num_bone_frame)
                                    {
                                        data =  *((unsigned char**) (data+(frame<<2)));
                                        frame_event_flags = *(data+1);
                                    }
                                    if((frame_event_flags & FRAME_EVENT_BLOCK) && (*((unsigned short*) (character_data + 42))) == 0)
                                    {
                                        // Character is trying to block particle collision...  Need to check facing against particle velocity...
                                        joint_data = (float*) (character_data+120);  // Character forward normal vector
                                        if(flags & PART_STUCK_ON)
                                        {
                                            // Use stuck character's fore normal for particle direction...
                                            // stuckto and target_data here refers to the attacker (who's weapon the particle is stuck to)...
                                            stuckto = (*((unsigned short*) (particle_data+86))) & (MAX_CHARACTER-1);
                                            target_data = main_character_data[stuckto];
                                            scalar_xyz[X] = *((float*) (target_data+120));
                                            scalar_xyz[Y] = *((float*) (target_data+124));
                                            scalar_xyz[Z] = *((float*) (target_data+128));
                                        }
                                        else
                                        {
                                            // Use particle velocity for it's direction...
                                            scalar_xyz[X] = *((float*) (particle_data+24));
                                            scalar_xyz[Y] = *((float*) (particle_data+28));
                                            scalar_xyz[Z] = *((float*) (particle_data+32));
                                        }
//log_message("INFO:     Block...  Part vel xyz == %f, %f, %f", scalar_xyz[X], scalar_xyz[Y], scalar_xyz[Z]);
//log_message("INFO:     Block...  Character facing == %f, %f, %f", ((float*) (character_data+120))[X], ((float*) (character_data+120))[Y], ((float*) (character_data+120))[Z]);
                                        distance = dot_product(scalar_xyz, joint_data);
//log_message("INFO:     Block...  Dot == %f", distance);
                                        if(distance < 0.0f)
                                        {
//log_message("INFO:     Block worked...");
                                            // Character is facing particle...  Block was successful...
                                            particle_data[67] = EVENT_BLOCKED;
                                            if(character_data[216] & ENCHANT_FLAG_MIRROR_DEFLECT)
                                            {
                                                if(!(flags & PART_STUCK_ON))
                                                {
                                                    particle_data[67] = EVENT_DEFLECTED;
                                                }
                                            }

                                            // Tell character that block worked...
                                            character_data[67] = EVENT_BLOCKED;
                                            fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                                        }
                                    }
                                    if(character_data[216] & ENCHANT_FLAG_DEFLECT)
                                    {
                                        if(!(flags & PART_STUCK_ON))
                                        {
                                            particle_data[67] = EVENT_DEFLECTED;
                                        }
                                    }


                                    // Call the event function for the particle, 'cause something happened...
                                    #ifdef CAN_BLOW_SELF_UP
                                        if(*((unsigned short*)(particle_data+76)) == character)
                                        {
                                            // Temporarily modify the particle's team if it hit it's owner, so it does full damage...
                                            // This should rarely happen...
                                            old_team = particle_data[78];
                                            particle_data[78] = 255;
                                        }
                                    #endif
                                    target = (*((unsigned short*) (particle_data+62))) & (MAX_CHARACTER-1);
                                    *((unsigned short*) (particle_data+62)) = character;  // Temporarily set the .target...
                                    fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                                    *((unsigned short*) (particle_data+62)) = target;
                                    #ifdef CAN_BLOW_SELF_UP
                                        if(*((unsigned short*)(particle_data+76)) == character)
                                        {
                                            // Reset the team...
                                            particle_data[78] = old_team;
                                        }
                                    #endif
                                    if(!main_particle_on[i])
                                    {
                                        // Handle poofing nicely...
                                        j = num_character_collision_list;
                                    }
                                }
                            }
                        }
                    }
                }
            }





            // Refresh timer...  Need to check if on in case it poofed...
            if(*((unsigned short*) (particle_data+52)) != 0 && main_particle_on[i])
            {
                (*((unsigned short*) (particle_data+52)))--;
                if(*((unsigned short*) (particle_data+52)) == 0)
                {
                    // Call the refresh function
                    fast_run_script(main_particle_script_start[i], FAST_FUNCTION_REFRESH, particle_data);
                }
            }


            // Event timer...  Need to check if on in case it poofed...
            if(*((unsigned short*) (particle_data+54)) != 0 && main_particle_on[i])
            {
                (*((unsigned short*) (particle_data+54)))--;
                if(*((unsigned short*) (particle_data+54)) == 0)
                {
                    // Call the event handler...
                    particle_data[67] = EVENT_TIMER;
                    fast_run_script(main_particle_script_start[i], FAST_FUNCTION_EVENT, particle_data);
                }
            }



            // Keep working on the same particle if it's flag'd to move extra fast...
            if(flags & PART_FAST_ON)
            {
                fast_count++;
                if(fast_count < 1000)
                {
                    i--;
                }
                else
                {
                    // Uh, oh...  This particle was updated 1000 times, but it still hasn't
                    // Poof'd...  That's like a Shotgonne shootin' straight up or something...
                    // We'd better poof it automagically to make sure it's okay...
                    obj_destroy(particle_data);
                    fast_count = 0;
                }
            }
            else
            {
                fast_count = 0;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
