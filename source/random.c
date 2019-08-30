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

// <ZZ> This file contains functions pertaining to random numbers
//      random_setup            - Sets up the random number table
//      random_number           - Returns a random number from 0 to 255
//      random_dice             - Simulates a dice roll

unsigned short next_random = 0;         // The next random number to return
unsigned short max_random = 0;          // The number of random numbers in the table
unsigned short and_random = 0;          // For looping random table...

//-----------------------------------------------------------------------------------------------
signed char random_setup(int seed)
{
    // <ZZ> This function figures out where the random number table is, and picks the first
    //      random number to use.
    max_random = 0;
    next_random = 0;
    random_table = sdf_find_index("RANDOM.DAT");
    if(random_table)
    {
        max_random = (unsigned short) (sdf_read_unsigned_int(random_table+4) & 0x0000FFFF);
        and_random = 0;
        if(max_random > 0)
        {
            // Table size should be a power of 2...
            and_random = max_random-1;
        }
        random_table = (unsigned char*) sdf_read_unsigned_int(random_table);
        next_random = seed;
        log_message("INFO:   Found the random number table");
        return TRUE;
    }
    log_message("ERROR:  Could not find the random number table");
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro returns a random number...  No fancy stuff...  Usage...  x = random_number;
//      Result should range from 0 - 255...
#define random_number  (random_table[next_random&and_random]);  { next_random++; }
// !!!BAD!!!
// !!!BAD!!!  Will need a larger table...
// !!!BAD!!!


//-----------------------------------------------------------------------------------------------
unsigned short random_dice(unsigned char number, unsigned short sides)
{
    // <ZZ> This function rolls some dice and returns the result.  random_dice(2, 6) == 2d6...
    unsigned char i;
    unsigned short value;
    unsigned char temp;


    value = 0;
    if(sides > 0)
    {
        repeat(i, number)
        {
            // Roll one die...
            temp = random_number;
            temp = temp%sides;
            temp++;

            // And add it to the running total
            value += temp;
        }
    }
    return value;
}

//-----------------------------------------------------------------------------------------------
unsigned char* random_name(unsigned char* filedata)
{
    // <ZZ> This function returns a pointer to a random name string (NAME_STRING) from one of
    //      the random naming datafiles.  Data is in the format of a null-terminated text file,
    //      with commas seperating random choices and new lines (null terms) seperating
    //      segment blocks.  The '-' character is used to repeat the last value in the
    //      preceding block...  "Dun-agorn" becomes "Dunnagorn"...

    #define MAX_NAME_SEGMENT 8
    unsigned short segment, num_segment;
    unsigned short num_choice[MAX_NAME_SEGMENT];
    unsigned char* segment_start[MAX_NAME_SEGMENT];
    unsigned short num, choice;
    unsigned char* read;
    unsigned char last_value;
    int filesize;
    int i;
    unsigned short name_size;
    unsigned char* index;



//log_message("INFO:   Generating random name...");


    NAME_STRING[0] = 0;
    name_size = 0;
    num = sdf_find_index_by_data(filedata);
    if(num != 65535)
    {
        index = sdf_index + (num<<4);

        // Found the file in the SDF datafile...
        filesize = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;


        // Clear out our totals...
        num_segment = 0;
        repeat(i, MAX_NAME_SEGMENT)
        {
            num_choice[i] = 0;
            segment_start[i] = filedata;
        }


        // Read through the text file, looking for commas and null terms...
        read = filedata;
        last_value = 0;
        repeat(i, filesize)
        {
            if(*read == ',')
            {
                // We found a new choice...
                num_choice[num_segment]++;
//log_message("INFO:       Found choice");
            }
            else if(*read == 0)
            {
                // Was the last value a null term too?
                if(last_value == 0)
                {
                    // Yup, that means we're all done with our preliminary scan...
                    // Two returns in a row should only happen at end of file...
                    i = filesize;
                }
                else
                {
                    // We found a new choice and segment...
//log_message("INFO:     Found segment");
                    num_choice[num_segment]++;
                    num_segment++;
                    if(num_segment >= MAX_NAME_SEGMENT)
                    {
                        // We can't handle any more segments...  Time to quit...
                        i = filesize;
                    }
                    else
                    {
                        // Remember where the next segment starts...
                        segment_start[num_segment] = read+1;
                    }
                }
            }


            last_value = *read;
            read++;
        }



//log_message("INFO:   Picking segments...");

        // Should now have a rough idea of how many name segments and stuff we're lookin' at...
        // Lets randomly pick some...
        repeat(segment, num_segment)
        {
//log_message("INFO:     Segment %d", segment);
            // Pick a random choice for each segment...
            if(num_choice[segment] > 0)
            {
                choice = random_number;
                choice = choice % num_choice[segment];
//log_message("INFO:       Choice %d (of %d)", choice, num_choice[segment]);


                // Now search for the selected choice by reading through where commas are...
                read = segment_start[segment];
                i = 0;
                while(i < choice)
                {
                    if(*read == ',')
                    {
                        i++;
                    }
                    read++;
                }


                // Read should now be at the start of our segment choice...  Copy it into the name string...
                while(*read != ',' && *read != 0 && name_size < 100)
                {
                    NAME_STRING[name_size] = *read;
                    if(NAME_STRING[name_size] == '-' && name_size > 0 && segment > 0)
                    {
                        NAME_STRING[name_size] = NAME_STRING[name_size-1];
                    }
                    read++;
                    name_size++;
                }
                NAME_STRING[name_size] = 0;
            }
        }
    }




    NAME_STRING[15] = 0;  // Never allow names longer than 15 characters...
//log_message("INFO:   New name is %s", NAME_STRING);
    return NAME_STRING;
}


/*
log_message("INFO:   Getting random name...");


    index = sdf_find_index_by_data(filedata);
    if(index != 65535)
    {
        // Found the file in the SDF datafile...
        filesize = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;


        count = filesize >> 4;
log_message("INFO:     %d names in file...", count);
        if(count > 0)
        {
            count = random_number;
            count = count % (*filedata);
            filedata+=(count << 4) + 16;
        }
    }
    return filedata;
*/

//-----------------------------------------------------------------------------------------------
