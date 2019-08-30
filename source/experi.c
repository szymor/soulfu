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

// <ZZ> This file has stuff for giving experience...

#define MAX_EXPERIENCE_TYPE  8
#define EXPERIENCE_GIVE                0
#define EXPERIENCE_GET_NEEDED          1
#define EXPERIENCE_VIRTUE_COMPASSION   2
#define EXPERIENCE_VIRTUE_DILLIGENCE   3
#define EXPERIENCE_VIRTUE_HONOR        4
#define EXPERIENCE_VIRTUE_FAITH        5
#define EXPERIENCE_VIRTUE_COURAGE      6


#define MAX_LEVEL 10
unsigned short level_experience_needed[MAX_LEVEL] = {0, 100, 250, 700, 1500, 2500, 4000, 5700, 7700, 10000};
//unsigned short level_experience_needed[MAX_LEVEL] = {0, 200, 500, 900, 1400, 2000, 2700, 3500, 4400, 5400};
//unsigned short level_experience_needed[MAX_LEVEL] = {0, 100, 300, 600, 1000, 1500, 2100, 2800, 3600, 4500};
//unsigned short level_experience_needed[MAX_LEVEL] = {0, 100, 250, 450, 700, 1000, 1350, 1750, 2200, 2700};
unsigned char  level_badges_needed[MAX_LEVEL] = {0, 0, 0, 0, 0, 1, 2, 3, 4, 5};


//-----------------------------------------------------------------------------------------------
unsigned short experience_function_character(unsigned short character, unsigned char experience_type, signed short experience_amount, unsigned char affecting_single_character)
{
    // <ZZ> This function gives experience to a character.
    unsigned char* character_data;
    unsigned char* backup_data;
    unsigned short backup_item;
    unsigned short rider;
    unsigned char can_level_up;
    unsigned char good_guy;
    unsigned char number_of_badges, i;
    unsigned char level;
    unsigned char intelligence;

    // Find the character...
    if(character < MAX_CHARACTER && experience_type < MAX_EXPERIENCE_TYPE)
    {
        if(main_character_on[character])
        {
            // Check some stuff...
            character_data = main_character_data[character];


            // Pass along effect to rider if target character is a mount...
            rider = *((unsigned short*) (character_data+106));
            if(rider < MAX_CHARACTER)
            {
                // Don't pass it along if it's a team effect (or rider would be hit twice)
                if(main_character_on[rider] && experience_type != EXPERIENCE_GET_NEEDED && affecting_single_character)
                {
                    character = rider;
                    character_data = main_character_data[character];
                }
            }



            // What type?
            if(experience_type < EXPERIENCE_VIRTUE_COMPASSION)
            {
                // Really giving experience, not virtue...  Check level...
                // Level 0 characters (monsters) aren't elligible for experience...
                good_guy = (character_data[78] == TEAM_GOOD);  // Bad guys don't worry about badges...
                level = character_data[73];
                if(character_data[90] == 255)
                {
                    // The character has the badge of dilligence...  Let's only give 'em half experience...
                    experience_amount = (experience_amount+1)>>1;
                }
                if(level > 0 && level < MAX_LEVEL && experience_amount > 0)
                {
                    // Figure out how much experience is needed for the character's next level...
                    number_of_badges = 0;
                    repeat(i, 5)
                    {
                        if(character_data[89+i] == 255)  number_of_badges++;
                    }
                    can_level_up = (number_of_badges >= level_badges_needed[level]) || (good_guy==FALSE);
                    if(can_level_up)
                    {
                        // Were we really asking for how much we need?
                        if(experience_type == EXPERIENCE_GET_NEEDED)
                        {
                            return level_experience_needed[level];
                        }


                        // Able to gain experience...  So gain it already...
                        // But first modify the amount by our intelligence...
                        intelligence = character_data[88];
                        intelligence = (intelligence > 50) ? 50 : intelligence;
                        intelligence = intelligence + 10;
                        experience_amount = experience_amount * intelligence / 25;
                        if(affecting_single_character && experience_amount == 0)
                        {
                            experience_amount++;
                        }
                        (*((unsigned short*) (character_data+70))) += experience_amount;


                        // Check for level ups...
                        if((*((unsigned short*) (character_data+70))) >= level_experience_needed[level])
                        {
                            // Character leveled up...  Reset experience to start of level...
                            (*((unsigned short*) (character_data+70))) = level_experience_needed[level];
                            character_data[73] = level+1;
                            if(paying_customer)
                            {
                                // Paying customers get 5 stat points per level...
                                character_data[74] += 5;
                            }
                            else
                            {
                                // Demo players only get 2 stat points per level...
                                character_data[74] += 2;
                            }
                            if(character_data[74] > 100)
                            {
                                character_data[74] = 100;
                            }

                            // Run the level up event...
                            backup_data = current_object_data;
                            backup_item = current_object_item;
                            character_data[67] = EVENT_LEVEL_UP;
                            fast_run_script(main_character_script_start[character], FAST_FUNCTION_EVENT, character_data);
                            current_object_item = backup_item;
                            current_object_data = backup_data;
                        }
                    }
                }
            }
            else
            {
                // Giving virtue to the character...
                // Make sure we don't have a badge already...
                experience_type+=87;  // Offset into character data...
                if(character_data[experience_type] != 255)
                {
                    // The character doesn't have this badge yet...
                    experience_amount += character_data[experience_type];
                    experience_amount = (experience_amount < 0) ? 0 : experience_amount;
                    experience_amount = (experience_amount > 100) ? 100 : experience_amount;
                    character_data[experience_type] = (unsigned char) experience_amount;
                }
            }
        }
    }
    return 65535;
}

//-----------------------------------------------------------------------------------------------
