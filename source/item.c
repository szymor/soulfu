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

// <ZZ> This file contains functions related to player inventory & item registry...

#define MAX_ITEM_TYPE 256
unsigned char* item_type_script[MAX_ITEM_TYPE];
unsigned char* item_type_icon[MAX_ITEM_TYPE];
unsigned char* item_type_overlay[MAX_ITEM_TYPE];
signed short   item_type_price[MAX_ITEM_TYPE];
unsigned short item_type_flags[MAX_ITEM_TYPE];
unsigned char  item_type_str[MAX_ITEM_TYPE];
unsigned char  item_type_dex[MAX_ITEM_TYPE];
unsigned char  item_type_int[MAX_ITEM_TYPE];
unsigned char  item_type_mana[MAX_ITEM_TYPE];
unsigned char  item_type_ammo[MAX_ITEM_TYPE];
unsigned short weapon_setup_grip = 0;  // For scripted ModelAssign() call to weapons...

// Item flags...
#define ITEM_FLAG_WEAPON     1
#define ITEM_FLAG_SHIELD     2
#define ITEM_FLAG_SPECIAL    4
#define ITEM_FLAG_HELM       8
#define ITEM_FLAG_BODY       16
#define ITEM_FLAG_LEGS       32
#define ITEM_FLAG_SPELL      64



unsigned short global_item_index = 0;
unsigned char  global_item_bone_name = 0;
float weapon_refresh_xyz[3];

//-----------------------------------------------------------------------------------------------
void item_type_setup()
{
    // <ZZ> This function clears out all of the item data...
    unsigned short i;
    repeat(i, MAX_ITEM_TYPE)
    {
        item_type_script[i] = NULL;
        item_type_icon[i] = NULL;
        item_type_overlay[i] = NULL;
        item_type_price[i] = 0;
        item_type_flags[i] = 0;
        item_type_str[i] = 0;
        item_type_dex[i] = 0;
        item_type_int[i] = 0;
        item_type_mana[i] = 0;
        item_type_ammo[i] = 0;
    }
}

//-----------------------------------------------------------------------------------------------
unsigned char item_get_type_name(unsigned short item_type)
{
    // <ZZ> This function calls the GetName() script for a given item type...
    //      Should fill in NAME_STRING for later use...  Returns TRUE if it worked...
    unsigned char* backup_address;
    unsigned short backup_item;


    NAME_STRING[0] = 0;
    item_type = item_type%MAX_ITEM_TYPE;
    if(item_type_script[item_type])
    {
        backup_address = current_object_data;
        backup_item = current_object_item;
        fast_run_script(item_type_script[item_type], FAST_FUNCTION_GETNAME, current_object_data);
        current_object_data = backup_address;
        current_object_item = backup_item;
        if(NAME_STRING[0] != 0)
        {
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
unsigned char item_find_random_xyz(unsigned short character_index, unsigned char character_bone_name)
{
    // <ZZ> This function finds a random xyz position along one of the bones of a character's weapon.
    //      Fills in global weapon_refresh_xyz for return value.  Used to find where a weapon's
    //      refresh (enchantment) particles should be spawn'd...  Returns TRUE if it work'd...
    unsigned short i;
    unsigned short num_bone, num_joint;
    unsigned short joint[2];
    float percent, inverse;
    float* joint_xyz[2];
    unsigned char* base_model_data;
    unsigned char* bone_data;
    unsigned char* joint_data;
    unsigned char* character_rdy_file;
    unsigned char* item_rdy_file;


    // Find the RDY files for the character and the item...
    if(character_bone_name != LEFT_BONE && character_bone_name != RIGHT_BONE)
    {
        // Only work for left and right weapons...
        return FALSE;
    }
    character_index = character_index & (MAX_CHARACTER-1);
    character_rdy_file = *((unsigned char**)(main_character_data[character_index] + 256));
    if(character_bone_name == LEFT_BONE)
    {
        item_rdy_file = *((unsigned char**)(main_character_data[character_index] + 520));
    }
    else
    {
        item_rdy_file = *((unsigned char**)(main_character_data[character_index] + 544));
    }
    if(item_rdy_file == NULL || character_rdy_file == NULL)
    {
        return FALSE;
    }



    // Read the character's RDY file, find data positions for base model 0...
    character_rdy_file+=6;
    character_rdy_file+=(ACTION_MAX<<1);
    character_rdy_file+=(MAX_DDD_SHADOW_TEXTURE);
    base_model_data = (*((unsigned char**) character_rdy_file));
    bone_data = (*((unsigned char**) (character_rdy_file+16)));
    num_bone = *((unsigned short*) (base_model_data+6));



    // Find the bone numbers for the character's named bones...  Left, Right, Left2, Right2...
    character_bone_name = character_bone_name & 7;
    temp_character_bone_number[character_bone_name] = 255;
    repeat(i, num_bone)
    {
        temp_character_bone_number[(*(bone_data+(i*9))) & 7] = (unsigned char) i;
    }


    // Did we find a bone number?
    if(temp_character_bone_number[character_bone_name] != 255)
    {
        // Yes, we did, so character seems to have this bone alright...
        // Generate the matrix for the weapon bone...
        script_matrix_good_bone(temp_character_bone_number[character_bone_name], temp_character_bone_frame[character_index], main_character_data[character_index]);


        // Generate joint positions for the item...  Assume frame 1...
        render_generate_model_world_data(item_rdy_file, 1, script_matrix, fourthbuffer);  // Generate new bone frame in fourthbuffer


        // Read the item's RDY file, find number of bones & joints for base model 0...
        // !!!BAD!!!
        // !!!BAD!!! Assume that # of bones and joints will be same for all base models of weapon...
        // !!!BAD!!!
        item_rdy_file+=6;
        item_rdy_file+=(ACTION_MAX<<1);
        item_rdy_file+=(MAX_DDD_SHADOW_TEXTURE);
        base_model_data = (*((unsigned char**) item_rdy_file));
        bone_data = (*((unsigned char**) (item_rdy_file+16)));
        num_joint = *((unsigned short*) (base_model_data+4));
        num_bone = *((unsigned short*) (base_model_data+6));
        if(num_bone > 0)
        {
            joint_data = fourthbuffer + (24*num_bone);
            i = random_number;
            i = i%num_bone;
            bone_data = bone_data + (i*9) + 1;
            joint[0] = *((unsigned short*) bone_data);
            joint[1] = *((unsigned short*) (bone_data+2));
            joint_xyz[0] = (float*) (joint_data+(joint[0]*12));
            joint_xyz[1] = (float*) (joint_data+(joint[1]*12));

            i = random_number;
            percent = i*ONE_OVER_256;
            inverse = 1.0f-percent;
            weapon_refresh_xyz[X] = joint_xyz[0][X]*percent + joint_xyz[1][X]*inverse;
            weapon_refresh_xyz[Y] = joint_xyz[0][Y]*percent + joint_xyz[1][Y]*inverse;
            weapon_refresh_xyz[Z] = joint_xyz[0][Z]*percent + joint_xyz[1][Z]*inverse;
            return TRUE;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
