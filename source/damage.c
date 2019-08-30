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

// <ZZ> This file has stuff related to damage...
unsigned char* pnumber_file = NULL;

signed short global_damage_amount;
#define MAX_DAMAGE_TYPE 7
#define DAMAGE_EDGE      0
#define DAMAGE_BASH      1
#define DAMAGE_ACID      2
#define DAMAGE_FIRE      3
#define DAMAGE_ICE       4
#define DAMAGE_VOLT      5
#define DAMAGE_WOUND     6
unsigned char damage_color_rgb[MAX_DAMAGE_TYPE][3] = {{200, 0, 0},
                                                       {200, 0, 0},
                                                       {117, 214, 0},
                                                       {255, 128, 0},
                                                       {142, 255, 255},
                                                       {255, 255, 0},
                                                       {200, 0, 0}};

unsigned short global_attack_spin = 0;
unsigned short global_attacker = MAX_CHARACTER;
signed char global_defense_rating[MAX_DAMAGE_TYPE];

//-----------------------------------------------------------------------------------------------
void damage_setup()
{
    // <ZZ> Sets up stuff for damaging characters...
    pnumber_file = sdf_find_filetype("PNUMBER", SDF_FILE_IS_RUN);
    if(pnumber_file)
    {
        pnumber_file = (unsigned char*) sdf_read_unsigned_int(pnumber_file);
    }
}

//-----------------------------------------------------------------------------------------------
void damage_character(unsigned short character, unsigned char damage_type, unsigned short damage_amount, unsigned short wound_amount, unsigned char attacker_team)
{
    // <ZZ> This function does damage to a character.
    float spawn_xyz[3];
    unsigned char* character_data;
    unsigned char* child_data;
    unsigned char* backup_data;
    unsigned short backup_item;
    signed char resistance;
    signed short temp;
    unsigned char min_hits;
    unsigned char friendly_fire;
    unsigned char start_hits;


    // Find the character...
    if(character < MAX_CHARACTER && damage_type < MAX_DAMAGE_TYPE)
    {
        if(main_character_on[character])
        {
            // Make sure script doesn't blow up...
            backup_data = current_object_data;
            backup_item = current_object_item;
            character_data = main_character_data[character];
            friendly_fire = (attacker_team == character_data[78] || character_data[78] == TEAM_NEUTRAL);
            // Neutral characters are able to damage others because explodie barrels need to
            // deal damage...
            start_hits = character_data[82];



//log_message("INFO:   Damage done to character %d", character);
//log_message("INFO:     Type == %d, Amount == %d, Wound == %d", damage_type, damage_amount, wound_amount);

            // Do the damage...  Reduce amounts by defense...  Tap wounds...
            resistance = (signed char) character_data[97+damage_type];
//log_message("INFO:     Resistance == %d", resistance);
            if(resistance < 0)
            {
                // Character is susceptible to damage...  Character takes an extra point
                // for a point, up to the limit marked by the resistance...
                resistance = -resistance;
                damage_amount = (damage_amount > resistance) ? (damage_amount+resistance) : (damage_amount<<1);
            }
            else
            {
                // Character resists damage...  Damage amount is reduced by the resistance
                // amount.
                damage_amount = (damage_amount > resistance) ? (damage_amount-resistance) : 0;
            }
            resistance = (signed char) character_data[97+DAMAGE_WOUND];
//log_message("INFO:     Wound resistance == %d", resistance);
            if(resistance < 0)
            {
                // Character is susceptible to wounding...  Character takes an extra point
                // for a point, up to the limit marked by the resistance...
                resistance = -resistance;
                wound_amount = (wound_amount > resistance) ? (wound_amount+resistance) : (wound_amount<<1);
            }
            else
            {
                // Character resists wounding...  Wound amount is reduced by the resistance
                // amount.
                wound_amount = (wound_amount > resistance) ? (wound_amount-resistance) : 0;
            }


            // Modify damage for petrification...
            if((*((unsigned short*) (character_data+42))) > 0)
            {
                damage_amount>>=1;
            }
//log_message("INFO:     Type == %d, Amount == %d, Wound == %d", damage_type, damage_amount, wound_amount);


            // Apply damage...
            if(friendly_fire)
            {
                if(character_data[78] == TEAM_NEUTRAL && !(CHAR_FLAGS & CHAR_CAN_BE_MOUNTED))
                {
                    // Friendly fire against neutral target (that isn't a mount)...  Don't lower hits below half of max...
                    damage_amount = damage_amount+wound_amount;  // No wound damage...
                    min_hits = character_data[80]>>1;
                    if(min_hits == 0)
                    {
                        // Should never kill...
                        min_hits++;
                    }
                    if(character_data[82] < min_hits)
                    {
                        // Hmmm...  That's weird...  Let's not do anything...
                        damage_amount = 0;
                    }
                    else
                    {
                        temp = ((signed short) character_data[82])-damage_amount;
                        character_data[82] = (unsigned char) temp;
                        if(temp < min_hits)
                        {
                            damage_amount = damage_amount-(min_hits-temp);
                            character_data[82] = min_hits;
                        }
                    }
                }
            }
            else
            {
                // Apply normal damage...
                character_data[82] = (character_data[82] > damage_amount) ? (character_data[82]-damage_amount) : 0;
            }




            // Apply wound damage...
            if(!friendly_fire)
            {
                character_data[82] = (character_data[82] > wound_amount) ? (character_data[82]-wound_amount) : 0;
                if((character_data[81] + wound_amount) < character_data[80])
                {
                    character_data[81] += wound_amount;
                }
                else
                {
                    character_data[81] = character_data[80];
                }
            }


//log_message("INFO:     Type == %d, Amount == %d, Wound == %d", damage_type, damage_amount, wound_amount);


            // Spawn the bouncing number particle, but only for things that aren't invincible (255 max hits)
            if(character_data[80] < 255)
            {
                // Okay to spawn it...
                if(pnumber_file && damage_type != DAMAGE_WOUND)
                {
                    damage_amount = start_hits - character_data[82];
                    global_damage_amount = damage_amount;
                    spawn_xyz[X] = *((float*) (character_data));
                    spawn_xyz[Y] = *((float*) (character_data+4));
                    spawn_xyz[Z] = (*((float*) (character_data+8))) + (character_data[175] - 1);


    //log_message("INFO:     Particle number amount == %d", damage_amount);

                    child_data = obj_spawn(PARTICLE, spawn_xyz[X], spawn_xyz[Y], spawn_xyz[Z], pnumber_file, MAX_PARTICLE);
                    current_object_item = backup_item;
                    current_object_data = backup_data;
                    if(child_data)
                    {
                        // Tint the number according to the damage type...
                        child_data[48] = damage_color_rgb[damage_type][0];
                        child_data[49] = damage_color_rgb[damage_type][1];
                        child_data[50] = damage_color_rgb[damage_type][2];
                        child_data[51] = (unsigned char) damage_amount;
                    }
                }
            }


            // Run the damage event on the target
            character_data[67] = EVENT_DAMAGED;
            fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
            current_object_item = backup_item;
            current_object_data = backup_data;
        }
    }
}

//-----------------------------------------------------------------------------------------------
