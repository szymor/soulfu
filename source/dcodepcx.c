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

// <ZZ> This file contains functions to convert PCX files to RGB
//  **  decode_pcx              - The main function to do a PCX conversion


unsigned char pcx_palette[256*3];   // Palette data in 24-bit format
//unsigned char pcx_palette[256*2];   // Palette data in 555 format
//-----------------------------------------------------------------------------------------------
signed char decode_pcx(unsigned char* index, unsigned char* filename)
{
    // <ZZ> This function decompresses a pcx file that has been stored in memory.  Index is a
    //      pointer to the start of the file's index in sdf_index, and can be gotten from
    //      sdf_find_index.  If the function works okay, it should create a new RGB file in the
    //      index and return TRUE.  It might also delete the original compressed file to save
    //      space, but that's a compile time option.  If it fails it should return FALSE, or
    //      it might decide to crash.
    unsigned char* data;    // Compressed
    unsigned int size;      // Compressed
    unsigned char* newdata; // Decompressed
    unsigned int newsize;   // Decompressed
    unsigned char* read;
    unsigned char* write;
    unsigned short output_width;
    unsigned short output_height;
    unsigned short bytes_per_line;
    int i, j;
    int x, y;
    unsigned char red, green, blue; //, hi, lo;


    // Log what we're doing
    #ifdef VERBOSE_COMPILE
        log_message("INFO:     Decoding %s.JPG to %s.RGB", filename, filename);
    #endif


    // Find the location of the file data, and its size...
    data = (unsigned char*) sdf_read_unsigned_int(index);
    size = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;
    newdata = NULL;


    // Make sure we have room in the index for a new file
    #ifdef KEEP_COMPRESSED_FILES
        if(sdf_extra_files <= 0)
        {
            log_message("ERROR:  No room left to add file, program must be restarted");
            return FALSE;
        }
    #endif


    // Read the header info and make sure it's the right type of file...
    if(*(data+1) != 5 || *(data+3) != 8 || *(data+65) != 1)
    {
        log_message("ERROR:  PCX file is incorrect version");
        return FALSE;
    }
    if(*(data+size-769) != 12)
    {
        log_message("ERROR:  PCX file has corrupt palette data");
        return FALSE;
    }


    // Read the x and y sizes
    output_width = *(data+9);  output_width = output_width<<8;  output_width+= *(data+8);  output_width++;
    output_height = *(data+11);  output_height = output_height<<8;  output_height+= *(data+10);  output_height++;
    bytes_per_line = *(data+67);  bytes_per_line = bytes_per_line<<8;  bytes_per_line+= *(data+66);


    // Read the palette data
    read = data+size-768;
    write = pcx_palette;
    repeat(i, 256)
    {
        red = *(read);  read++;
        green = *(read);  read++;
        blue = *(read);  read++;

        *write = red;  write++;
        *write = green;  write++;
        *write = blue;  write++;

//        convert_24bit_to_16bit(red, green, blue, hi, lo);
//        *write = hi;  write++;
//        *write = lo;  write++;
    }


    // Allocate memory for the new file...  2 byte flags, 4 byte texture, 2 byte x, 2 byte y, x*y*3 bytes for data
//    newsize = (2*(output_width)*(output_height)) + 10;
    newsize = (3*(output_width)*(output_height)) + 10;
    newdata = malloc(newsize);
    if(newdata)
    {
        // Do we create a new index?
        #ifdef KEEP_COMPRESSED_FILES
            index = sdf_get_new_index();
        #endif


        // Write the index...
        sdf_write_unsigned_int(index, (unsigned int) newdata);
        sdf_write_unsigned_int(index+4, newsize);
        *(index+4) = SDF_FILE_IS_RGB;
        repeat(j, 8) { *(index+8+j) = 0; }
        memcpy(index+8, filename, strlen(filename));


        // Write the texture info and file dimensions to the first 10 bytes
        sdf_write_unsigned_short(newdata, (unsigned short) 0);   newdata+=2;
        sdf_write_unsigned_int(newdata, (unsigned int) 0);   newdata+=4;
        sdf_write_unsigned_short(newdata, (unsigned short) output_width);   newdata+=2;
        sdf_write_unsigned_short(newdata, (unsigned short) output_height);  newdata+=2;
    }
    else
    {
        log_message("ERROR:  Not enough memory to decompress");
        return FALSE;
    }


    // Decompress the file
    read = data+128;
    repeat(y, output_height)
    {
        x = 0;
//        write = newdata + (y*2*output_width);
        write = newdata + (y*3*output_width);
        while (x < bytes_per_line)
        {
            j = *read;  read++;
            i = 1;
            if((j & 0xC0) == 0xC0)
            {
                i = (j & 0x3F);
                j = *read;  read++;
            }
            while(i > 0)
            {
                if(x < output_width)
                {
                    *write = pcx_palette[j*3];
                    write++;
                    *write = pcx_palette[(j*3)+1];
                    write++;
                    *write = pcx_palette[(j*3)+2];
                    write++;
                }
                x++;
                i--;
            }
        }
    }


    // Decide if we should get rid of the compressed file or not...
    #ifndef KEEP_COMPRESSED_FILES
        // Don't need to worry about index, since it should've been overwritten
        free(data);
        sdf_can_save = FALSE;
    #endif
    return TRUE;
}

//-----------------------------------------------------------------------------------------------

