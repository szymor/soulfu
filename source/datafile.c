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

// <ZZ> This file contains functions to control the main data file (datafile.sdf).
//  **  sdf_read_unsigned_int   - Reads a 4 byte big endian value from memory (helper)
//  **  sdf_write_unsigned_int  - Writes a 4 byte big endian value to memory (helper)
//  **  sdf_read_unsigned_short - Reads a 2 byte big endian value from memory (helper)
//  **  sdf_write_unsigned_short- Writes a 2 byte big endian value to memory (helper)
//  **  sdf_get_filename        - Gets the filename and type of a given file (helper)
//  **  sdf_unload              - Frees memory, automatically called via atexit()
//  **  sdf_load                - Loads datafile.sdf into memory
//  **  sdf_fix_filename        - Turns "TROLL.JPG" into "TROLL" and SDF_FILE_IS_JPG (helper)
//  **  sdf_find_index          - Returns a pointer to a given file's index information (helper)
//  **  sdf_find_filetype       - Like find_index, but args are like "TROLL" and SDF_FILE_IS_JPG
//  **  sdf_delete_file         - Deletes a file from the memory version of datafile.sdf
//  **  sdf_add_file            - Adds a file into the memory version of datafile.sdf
//  **  sdf_new_file            - Adds new a file into the memort version of datafile.sdf
//  **  sdf_save                - Saves the memory version of datafile.sdf to disk
//  **  sdf_export_file         - Exports a given file to disk
//  **  sdf_decode              - Decompresses all of the files in the datafile
//  **  sdf_open                - Opens a .TXT file in the datafile for reading
//  **  sdf_read_line           - Reads the next line of an open .TXT file
//  **  sdf_insert_data         - Adds some data to a file, or removes some data (if to_add < 0)
//  **  sdf_flag_set            - Sets a given flag for a single file
//  **  sdf_flag_clear          - Clears a given flag for all of the files
//  **  sdf_checksum            - Generates a checksum for a piece of data
//  **  sdf_delete_all_files    - Deletes all files of a given type
//  **  sdf_find_index_by_data  - Looks for a file index, given a pointer to the file's data start...

//-----------------------------------------------------------------------------------------------
#define SDF_INSTRUMENT      '-' // .OGG files prefixed with this are treated as instruments
#define SDF_NO_ECHO         '=' // .OGG files prefixed with this are not given an echo
#define SDF_ECHO_LOOP       '+' // .OGG files prefixed with this are echoed for loop playback
#define SDF_ECHO            'E' // Not a prefix


#define SDF_HEADER_SIZE     64  // Size of the datafile.sdf header
#define SDF_EXTRA_INDEX  32768  // Allow up to (##/16) files to be sdf_add_file()'d to the index
#define SDF_ALL            255  // For decompressing all files...
#define SDF_FLAG_WAS_UPDATED 128  // Upper 4 bits...
#define SDF_FLAG_NO_UPDATE  64  // Upper 4 bits...
#define SDF_FLAG_C          32  // Upper 4 bits...
#define SDF_FLAG_D          16  // Upper 4 bits...
#define SDF_FILETYPE_COUNT  16  // The number of defined extensions
  #define SDF_FILE_IS_UNUSED 0  // Lower 4 bits...  File was deleted (don't save index to disk)
  #define SDF_FILE_IS_TXT    1  // Lower 4 bits...  File is a text or random naming file
  #define SDF_FILE_IS_JPG    2  // Lower 4 bits...  File is a JPEG texture
  #define SDF_FILE_IS_OGG    3  // Lower 4 bits...  File is an Ogg Vorbis sound
  #define SDF_FILE_IS_RGB    4  // Lower 4 bits...  File is a decompressed texture (don't save)
  #define SDF_FILE_IS_RAW    5  // Lower 4 bits...  File is a decompressed sound (don't save)
  #define SDF_FILE_IS_SRF    6  // Lower 4 bits...  File is a Supercool Room Format File
  #define SDF_FILE_IS_MUS    7  // Lower 4 bits...  File is a Music file (my format)
  #define SDF_FILE_IS_DAT    8  // Lower 4 bits...  File is a Data file (raw data)
  #define SDF_FILE_IS_SRC    9  // Lower 4 bits...  File is a script source file
  #define SDF_FILE_IS_RUN   10  // Lower 4 bits...  File is a compiled script file
  #define SDF_FILE_IS_PCX   11  // Lower 4 bits...  File is a PCX texture
  #define SDF_FILE_IS_LAN   12  // Lower 4 bits...  File is a Language text file
  #define SDF_FILE_IS_DDD   13  // Lower 4 bits...  File is a 3D model file
  #define SDF_FILE_IS_RDY   14  // Lower 4 bits...  File is a 3D model file, ready for use
//  #define SDF_FILE_IS_TIL   15  // Lower 4 bits...  File is a 3D model file, used for tiles


unsigned char* sdf_index;       // The datafile index after it has been loaded into memory
char* sdf_read_file = NULL;     // A pointer to the current read position for sdf_open
int sdf_read_first_line;        // FALSE until sdf_read_line has been called...
int sdf_read_line_number = 0;   // The current line number
int sdf_read_remaining;         // The number of bytes left to read
int sdf_is_loaded = FALSE;      // Did we load the datafile yet?
int sdf_num_files = 0;          // The number of files in the datafile after it has been loaded
int sdf_extra_files = 0;        // The number of files we can still add
int sdf_can_save = FALSE;       // Has the datafile been decompressed?
char sdf_extension[16][4] =     // The extension strings that the program recognizes
  { "BAD", "TXT", "JPG", "OGG",
    "RGB", "RAW", "SRF", "MUS",
    "DAT", "SRC", "RUN", "PCX",
    "LAN", "DDD", "RDY", "TIL" };

#define TEXT_SIZE 131072
int sdf_dev_size[16] =          // The minimum file size in DEVTOOL mode...
  { 0, TEXT_SIZE, 0, 0,
    0, 0, 65536, 65536,
    8192, TEXT_SIZE, 0, 0,
    65536, 0, 2048000, 0 };


unsigned char sdf_need_update[16] =     // What file types need to be updated (unless flag set)
  { 0, 1, 1, 1,
    0, 0, 1, 1,
    1, 0, 1, 1,
    1, 1, 0, 0 };


unsigned char sdf_should_save[16] =     // What file types should save with sdf_save()
  { 0, 1, 1, 1,
    0, 0, 1, 1,
    1, 1, 1, 1,
    1, 1, 0, 0 };



#define OBFUSCATEA ((unsigned char) 97)
#define OBFUSCATEB ((unsigned char) 11)
#define OBFUSCATEC ((unsigned char) 53)
#define OBFUSCATED ((unsigned char) 37)


//-----------------------------------------------------------------------------------------------
float sdf_read_float(unsigned char* location)
{
    // <ZZ> This function reads an float value at the given memory location, assuming that
    //      the memory is stored in big endian format.  The function returns the value it
    //      read.
    unsigned int value;
// !!!BAD!!!
// !!!BAD!!!  This might not work...
// !!!BAD!!!
    value  = *location;     value = value<<8;
    value |= *(location+1); value = value<<8;
    value |= *(location+2); value = value<<8;
    value |= *(location+3);
    return *((float*) &value);
}

//-----------------------------------------------------------------------------------------------
unsigned int sdf_read_unsigned_int(unsigned char* location)
{
    // <ZZ> This function reads an unsigned integer at the given memory location, assuming that
    //      the memory is stored in big endian format.  The function returns the value it
    //      read.
    unsigned int value;
    value  = *location;     value = value<<8;
    value |= *(location+1); value = value<<8;
    value |= *(location+2); value = value<<8;
    value |= *(location+3);
    return value;
}

//-----------------------------------------------------------------------------------------------
void sdf_write_unsigned_int(unsigned char* location, unsigned int value)
{
    // <ZZ> This function writes an unsigned integer to the given memory location, assuming that
    //      the memory is stored in big endian format.
    *(location+3) = (unsigned char) (value & 255);  value = value>>8;
    *(location+2) = (unsigned char) (value & 255);  value = value>>8;
    *(location+1) = (unsigned char) (value & 255);  value = value>>8;
    *(location)   = (unsigned char) (value & 255);
}

//-----------------------------------------------------------------------------------------------
unsigned short sdf_read_unsigned_short(unsigned char* location)
{
    // <ZZ> This function reads an unsigned short at the given memory location, assuming that
    //      the memory is stored in big endian format.  The function returns the value it
    //      read.
    unsigned short value;
    value  = *location;    value = value<<8;
    value |= *(location+1);
    return value;
}

//-----------------------------------------------------------------------------------------------
void sdf_write_unsigned_short(unsigned char* location, unsigned short value)
{
    // <ZZ> This function writes an unsigned short to the given memory location, assuming that
    //      the memory is stored in big endian format.
    *(location+1) = (unsigned char) (value & 255);  value = value>>8;
    *(location)   = (unsigned char) (value & 255);
}

//-----------------------------------------------------------------------------------------------
signed char sdf_get_filename(int num, char* filename, unsigned char* filetype)
{
    // <ZZ> This function gets the filename and type of a file that has been loaded into datafile
    //      memory.  The function returns TRUE and fills in filename and filetype if it
    //      works, or it returns FALSE if it broke.  Index is the actual file number, starting
    //      from 0.  The filename buffer should be 9 characters long, while the filetype buffer
    //      should be a single character.  NULL can be passed to either filename or filetype
    //      if it isn't needed.

    unsigned char * index = NULL;

    //make sure we have a non-empty archive
    if(NULL==sdf_index)
    {
      log_message("ERROR:     sdf_get_filename() called with invalid archive");
      return FALSE;
    };

    if(num < sdf_num_files)
    {
        index = sdf_index + (num<<4);

        if(filename != NULL)
        {
            filename[0] = *(index+ 8);
            filename[1] = *(index+ 9);
            filename[2] = *(index+10);
            filename[3] = *(index+11);
            filename[4] = *(index+12);
            filename[5] = *(index+13);
            filename[6] = *(index+14);
            filename[7] = *(index+15);
            filename[8] = 0;
        }
        if(filetype != NULL)
        {
            *filetype = (*(index+4))&15;
        }
        return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
void sdf_unload(void)
{
    // <ZZ> This function tries to free() any memory taken up by the datafile.  It should be
    //      run automatically at program termination.
    int i;
    unsigned char flags;
    unsigned char *memory, *index;

    if(NULL!=sdf_index) 
    {
        // Go through each file in the index, and get rid of its memory
        repeat(i, sdf_num_files)
        {
            index = sdf_index + (i<<4);

            flags = *(index+4);
            if((flags & 15) != SDF_FILE_IS_UNUSED)
            {
                memory = (unsigned char*) sdf_read_unsigned_int(index);
                free(memory);
            }
        }

        // Now get rid of the index
        log_message("INFO:     deallocation of the sdf archive file index");
        free(sdf_index);
    };

    if(sdf_is_loaded) log_message("INFO:   DATAFILE.SDF unloaded correctly");

    sdf_is_loaded = FALSE;
    sdf_can_save = FALSE;
    
}

//-----------------------------------------------------------------------------------------------
void sdf_flag_clear(unsigned char flag)
{
    // <ZZ> This function clears a given flag for all of the files...  Flags may be or'd for
    //      multiple flags...
    int i;
    unsigned char * index;

    //make sure we have a non-empty archive
    if(NULL==sdf_index)
    {
      log_message("ERROR:     sdf_flag_clear() called with invalid archive");
      return;
    };

    // Go through each file in the index
    flag = ~flag;
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        *(index+4) &= flag;
    }
}

//-----------------------------------------------------------------------------------------------
signed char sdf_create(unsigned int num_files)
{
  // <BB> This function "creates" an empty sdf archive in memory   

  signed char retval = FALSE;

  //unload any previous sdf archive
  sdf_unload();

  if(!sdf_is_loaded)
  {
    sdf_read_first_line  = FALSE;               // FALSE until sdf_read_line has been called...
    sdf_read_line_number = 0;                   // The current line number
    sdf_read_remaining   = 0;                   // The number of bytes left to read
    sdf_num_files        = num_files;           // The number of files in the datafile after it has been loaded
    sdf_extra_files      = SDF_EXTRA_INDEX>>4;  // The number of files we can still add
    sdf_can_save         = FALSE;               // Has the datafile been decompressed?

    retval = TRUE;
  };

  return retval;
};


//-----------------------------------------------------------------------------------------------
signed char sdf_load(const char *datafile)
{
  // <ZZ> This function tries to load datafile.sdf by checking its size, checking the header,
  //      allocating memory, and reading all of the data.  It also registers sdf_unload to
  //      run atexit() to free up the memory.  The function returns FALSE on an error, TRUE
  //      if everything loaded okay.  If it worked, several global variables should also be
  //      set, indicating the number of files and stuff like that.

  unsigned char header[SDF_HEADER_SIZE];
  unsigned char* filedata;
  unsigned int offset;
  char filename[16];
  unsigned char filetype;
  int num_files;
  int filesize;
  int datasize;
  int allocsize;
  int i, j;
  FILE *openfile;
  unsigned char *index;


  // Reset the language file pointers
  repeat(i, LANGUAGE_MAX)
  {
    language_file[i] = NULL;
  }


  // Open the data file in binary mode
  log_message("INFO:   Trying to open DATAFILE.SDF");
  openfile = fopen(datafile, "rb");
  if(openfile)
  {
    // Check how big our data file is...
    fseek(openfile, 0, SEEK_END);
    filesize = ftell(openfile);
    fseek(openfile, 0, SEEK_SET);
    if(filesize > 80)
    {
      log_message("INFO:   File is %d bytes", filesize);
      // Read the header of the datafile
      if(fread(header, 1, SDF_HEADER_SIZE, openfile) == SDF_HEADER_SIZE)
      {
        // Check the first two lines (31 bytes) of the header to make sure it's okay
        header[31] = 0;
        if(strncmp(header, "----------------SOULFU DATA FIL" , 32)== 0)
        {
          // Get the number of files from the third line
          sscanf(header+32, "%d", &num_files);
          log_message("INFO:   %d files in datafile", num_files);


          if(num_files > 0)
          {
            sdf_create(num_files);

            if( sdf_allocate() )
            {
              // Read the index 
              fseek(openfile, 64, SEEK_SET);
              fread(sdf_index, 4, num_files<<2, openfile);


              // Unobfuscate the index...
              repeat(i, num_files)
              {
                index = sdf_index + (i<<4);

                *(index++) -= OBFUSCATEA;
                *(index++) -= OBFUSCATEA;
                *(index++) -= OBFUSCATEA;
                *(index++) -= OBFUSCATEA;
                *(index++) -= OBFUSCATEB;
                *(index++) -= OBFUSCATEB;
                *(index++) -= OBFUSCATEB;
                *(index++) -= OBFUSCATEB;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
                *(index++) -= OBFUSCATEC;
              }



              // Read the data for each file in the index...
              sdf_is_loaded = TRUE;
              repeat(i, num_files)
              {
                index = sdf_index + (i<<4);

                // Get memory for this file data
                datasize = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;
                offset = sdf_read_unsigned_int(index);
                sdf_get_filename(i, filename, &filetype);
                allocsize = datasize;
#ifdef DEVTOOL
                // Allocate extra memory for certain file types, so we can edit 'em...
                if(datasize < sdf_dev_size[filetype&15])  allocsize = sdf_dev_size[filetype&15];
#endif
                filedata = malloc(allocsize);
                if(filedata)
                {
                  // Log some information...
#ifdef VERBOSE_COMPILE
                  log_message("INFO:     Allocated %d (%d) bytes for %s.%s", allocsize, datasize, filename, sdf_extension[filetype&15]);
#endif


                  // Clear out the data
                  memset(filedata, 0, allocsize);


                  // Read the main file data
                  fseek(openfile, 80+(num_files<<4)+offset, SEEK_SET);
                  fread(filedata, 4, (datasize>>2), openfile);
                  fread(filedata+(datasize&0xFFFFFFFC), 1, (datasize&3), openfile);


                  // Unobfuscate the file data...
                  repeat(j, datasize)
                  {
                    filedata[j] -= OBFUSCATED;
                  }


                  // Replace the 4 byte location offset with an actual address
                  sdf_write_unsigned_int(index, ((unsigned int) filedata));
                }
                else
                {
                  log_message("ERROR:  Out of memory when loading DATAFILE.SDF");
                  // Don't bother to load the rest, just mark 'em as unused
                  while(i < num_files)
                  {
                    *(index+4) = SDF_FILE_IS_UNUSED;
                    i++;
                  }
                  // Now unload all of the data
                  sdf_unload();
                }
              }

              if(sdf_is_loaded)
              {
                // Register the unload function to be called on exit
                atexit(sdf_unload);


                // Figure out how many more files we can add
                sdf_extra_files = SDF_EXTRA_INDEX>>4;


                // Everything worked okay
                sdf_can_save = TRUE;
                fclose(openfile);



                // Just in case flags are messed up...
                sdf_flag_clear(SDF_FLAG_WAS_UPDATED);
                return TRUE;
              }
            }
            else
            {
              log_message("ERROR:  Out of memory when loading file index");
            }
          }
          else
          {
            log_message("ERROR:  No files in DATAFILE.SDF");
          }
        }
        else
        {
          log_message("ERROR:  Header incorrect");
        }
      }
      else
      {
        log_message("ERROR:  Header could not be read");
      }
    }
    else
    {
      log_message("ERROR:  File is too small");
    }
    fclose(openfile);
  }
  else
  {
    log_message("ERROR:  DATAFILE.SDF not open");
  }
  return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_fix_filename(char* filename, char* newfilename, unsigned char* newfiletype)
{
    // <ZZ> This function takes a standard 8.3 filename (ie. "TROLL.JPG") and splits it into
    //      a new 8 character name ("TROLL") (9 with null on the end) and a filetype
    //      (SDF_FILE_IS_JPG).  The function then tries to write the results into newfilename
    //      and newfiletype.  If it can't figure out what a filetype is (ie. extension of ".BAD")
    //      or it otherwise fails, it returns FALSE.  It returns TRUE if it manages to work
    //      correctly.  Note that if the file name is longer than 8.3, the function tries to
    //      slice the name down.  newfilename should be 9 characters long, newfiletype should
    //      be 1 character.
    int length;
    char extension[4];
    int i, j;
    char founddot;


    length = strlen(filename);
    if(length > 4)
    {
        // Split off the name portion by replacing the '.' with a 0...
        i = 0;
        j = 0;
        founddot = FALSE;
        while(filename[i] != 0)
        {
            if(founddot && j < 3)
            {
                extension[j] = filename[i];
                j++;
            }
            if(filename[i] == '.') founddot = TRUE;
            if(founddot)
            {
                if(i < 9) newfilename[i] = 0;
            }
            else
            {
                if(i < 8) newfilename[i] = filename[i];
                else if(i == 9) newfilename[i] = 0;
            }
            i++;
        }
        while(i < 9)
        {
            newfilename[i] = 0;
            i++;
        }
        newfilename[8] = 0;
        extension[j] = 0;


        // Make the filename and extension uppercase
        make_uppercase(newfilename);
        make_uppercase(extension);


        // Check the extension against known types
        *newfiletype = SDF_FILE_IS_UNUSED;
        repeat(i, SDF_FILETYPE_COUNT) { if(strcmp(extension, sdf_extension[i]) == 0) *newfiletype = i; }


        // Did we find a valid extension?
        if(*newfiletype != SDF_FILE_IS_UNUSED) return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
unsigned char* sdf_find_index(char* filename)
{
    // <ZZ> This function looks in the sdf_index for a file based on the given filename.  If it
    //      finds a matching entry, it returns a pointer to the start of that entry's 16 byte
    //      block (4 byte location, 1 byte flag, 3 byte size, 8 byte name).  If not, it returns
    //      NULL.  Note that the extension to the filename must be one of the valid types...
    int i;
    unsigned char* index;
    unsigned char newfilename[9], newfiletype;

    //make sure we have a non-empty archive
    if(NULL==sdf_index)
    {
      log_message("ERROR:     sdf_find_index() called with invalid archive");
      return NULL;
    };

    // Convert the filename into an 8 character name and a 1 character extension
    if(sdf_fix_filename(filename, newfilename, &newfiletype))
    {
        // Now look through the index for a matching filetype and filename
        repeat(i, sdf_num_files)
        {
            index = sdf_index + (i<<4);

            // First look at filetypes
            if( (*(index+4) & 15) == newfiletype)
            {
                // Now check the name...
                if( 0 == strncmp(index+8, newfilename, 8) )
                {
                    // Looks like everything matches up, so we're done...
                    return index;
                }
            }
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------
unsigned char* sdf_find_filetype(char* filename, char filetype)
{
    // <ZZ> This function looks in the sdf_index for a file based on the given filename, without
    //      an extension.  The extension is given using the filetype.  If it finds a matching
    //      entry, it returns a pointer to the start of that entry's 16 byte block.  If not, it
    //      returns NULL.
    int i;
    int j;
    unsigned char* index;

    //make sure we have a non-empty archive
    if(NULL==sdf_index)
    {
      log_message("ERROR:     sdf_find_filetype() called with invalid archive");
      return NULL;
    };


    // Now look through the index for a matching filetype and filename
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        // First look at filetypes
        if( ((*(index+4)) & 15) == filetype)
        {
            // Now check the name...
            repeat(j, 8)
            {
                if(filename[j] != 0)
                {
                    if(*(index+8+j) != filename[j])  j = 100;
                }
                else
                {
                    j = 8;
                }
            }
            if(j < 100)
            {
                // We found a match...
                return index;
            }
        }
    }
    return NULL;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_delete_file(char* filename)
{
    // <ZZ> This function attempts to get rid of a file from the memory copy of datafile.sdf, by
    //      marking the file as having been deleted and by free()ing its memory.  It should
    //      return FALSE if there's been an error, or TRUE if the file was deleted correctly.
    //      Note that the extension to the filename must be one of the valid types...
    unsigned char* index;
    unsigned char* memory;



    // Find the file we're looking for...
    index = sdf_find_index(filename);
    if(index != NULL)
    {
        // Free up the file's memory block
        memory = (unsigned char*) sdf_read_unsigned_int(index);
        free(memory);


        // Now mark the file as being unused so it doesn't sdf_save() with the other files
        *(index+4) = SDF_FILE_IS_UNUSED;
        log_message("INFO:   Deleted %s from the datafile", filename);
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_allocate()
{
  //if there was a blank SDF before, create the index
  if(NULL == sdf_index)
  {
    // Get memory for the file index, 16 bytes per entry + some extra...
    log_message("INFO:     first allocation of the sdf archive file index");
    sdf_index = calloc((sdf_num_files << 4) + (sdf_extra_files << 4), sizeof(unsigned char));
  };

  return NULL != sdf_index;
};

//-----------------------------------------------------------------------------------------------
unsigned char * sdf_get_new_index()
{
  unsigned char * retval = NULL;

  //make sure that there is a valid index
  if( sdf_allocate() )
  {
    //return the last position
    retval = sdf_index + (sdf_num_files << 4);

    //bump the counters
    sdf_num_files++;
    sdf_extra_files--;
  };

  return retval;
};

//-----------------------------------------------------------------------------------------------
signed char sdf_add_file(char* sdf_filename, char* input_filename)
{
    // <ZZ> This function attempts to add a file from disk to the memory resident copy of
    //      datafile.sdf, by creating an index for the new file and requesting memory for
    //      its contents.  This function doesn't reuse space from deleted files or realloc
    //      more index space, because that would be a hassle.  So...  Only SDF_EXTRA_INDEX/16
    //      files may be added to the index before closing the program.  The function returns
    //      FALSE if something blew up, or TRUE if the file was added correctly.  Note that
    //      the extension to the sdf_filename must be one of the valid types...  Also note that
    //      trying to replace an existing sdf_filename may cause it to be sdf_delete_file()'d.

    FILE* openfile;
    int filesize;
    unsigned char* memory;
    unsigned char filetype;
    unsigned char newfilename[9];
    unsigned char temp[256];
    char readchar;
    int i, j;
    int allocsize;
    int headersize;
    int line;
    unsigned char * index = NULL;

    if(NULL==input_filename) input_filename = sdf_filename;

    // Delete any existing files with the given name
    sdf_delete_file(sdf_filename);

    // Make sure we have room in the index for a new file
    headersize = 0;
    if(sdf_extra_files > 0)
    {
        // Try to open the file on disk
        openfile = fopen(input_filename, "rb");
        if(openfile)
        {
            // Convert the sdf_filename into the proper format
            if(sdf_fix_filename(sdf_filename, newfilename, &filetype))
            {
                
                sprintf(temp, "%s", sdf_filename);


                if(filetype == SDF_FILE_IS_TXT || filetype == SDF_FILE_IS_SRC || filetype == SDF_FILE_IS_LAN)
                {
                    // 0D 0A pairs are turned into just 0, and a 0 is appended to the end...
                    filesize = 0;
                    if(filetype == SDF_FILE_IS_LAN)
                    {
                        // 4 bytes linecount, 4 byte address 0
                        filesize = 8;
                        headersize+=2;
                    }
                    while(fscanf(openfile, "%c", &readchar) != EOF)
                    {
                        if(readchar != 0x0D) filesize++;
                        else
                        {
                            if(filetype == SDF_FILE_IS_LAN)
                            {
                                filesize+=4;
                                headersize++;
                            }
                        }
                    }
                    if(readchar != 0x0A && readchar != 0x0D) filesize++;  // Extra byte for end 0
                }
                else
                {
                    // Load the data exactly
                    fseek(openfile, 0, SEEK_END);
                    filesize = ftell(openfile);
                }
                // Go back to the start of the file...
                fseek(openfile, 0, SEEK_SET);



                if(filesize > 0)
                {
                    // Allocate memory for the new file
                    allocsize = filesize;
                    #ifdef DEVTOOL
                        // Allocate extra memory for certain file types, so we can edit 'em...
                        if(filesize < sdf_dev_size[filetype&15])  allocsize = sdf_dev_size[filetype&15];
                    #endif
                    memory = malloc(allocsize);
                    if(memory)
                    {
                        // Log some information...
                        #ifdef VERBOSE_COMPILE
                            log_message("INFO:     Allocated %d (%d) bytes for %s", allocsize, filesize, temp);
                        #endif


                        // Create a new index
                        index = sdf_get_new_index();

                        // Fill in the data
                        sdf_write_unsigned_int(index, (unsigned int) memory);
                        sdf_write_unsigned_int(index+4, filesize);
                        *(index+4) = filetype | SDF_FLAG_WAS_UPDATED;
                        repeat(j, 8) { *(index+8+j) = 0; }
                        memcpy(index+8, newfilename, strlen(newfilename));

                        // Clear out the data
                        memset(memory, 0, allocsize);


                        // Setup the header data for a language file...
                        if(filetype == SDF_FILE_IS_LAN)
                        {
                            sdf_write_unsigned_int(memory, headersize-1);
                            sdf_write_unsigned_int(memory+4, (headersize<<2));
                        }


                        if(filetype == SDF_FILE_IS_TXT || filetype == SDF_FILE_IS_SRC || filetype == SDF_FILE_IS_LAN)
                        {
                            // Parse out 0D 0A pairs and put a 0 on the end
                            line = 2;
                            j = 0;
                            while(fscanf(openfile, "%c", &readchar) != EOF)
                            {
                                if(readchar != 0x0D)
                                {
                                    if(readchar == '\t') readchar = ' ';
                                    if(readchar == 0x0A)
                                    {
                                        readchar = 0;
                                        if(filetype == SDF_FILE_IS_LAN && line < headersize)
                                        {
                                            // Save the address of the new line...
                                            sdf_write_unsigned_int(memory+(line<<2), (headersize<<2)+j+1);
                                            line++;
                                        }
                                    }
                                    if(j < filesize) *(memory+(headersize<<2)+j) = readchar;
                                    j++;
                                }
                            }
                            if(j < filesize) *(memory+(headersize<<2)+j) = 0;
                            *(memory+(headersize<<2)+filesize-1) = 0;
                        }
                        else
                        {
                            // Do a straight read...
                            fread(memory, 4, filesize>>2, openfile);
                            fread(memory+(filesize&0xFFFFFFFC), 1, (filesize&3), openfile);
                        }

                        fclose(openfile);
                        log_message("INFO:   Added %s to the datafile", temp);
                        update_performed = TRUE;
                        return TRUE;
                    }
                    else
                    {
                        log_message("ERROR:  Not enough memory to add file %s", temp);
                    }
                }
                else
                {
                    log_message("ERROR:  Filesize for %s seems incorrect", temp);
                }
            }
            else
            {
                log_message("ERROR:  Incorrect sdf_filename or type couldn't be added");
            }
            fclose(openfile);
        }
        else
        {
//            log_message("ERROR:  File %s could not be opened", sdf_filename);
        }
    }
    else
    {
        log_message("ERROR:  No room left to add file, program must be restarted");
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_new_file(char* filename)
{
    // <ZZ> Makes a file on disk, then tries adding it in...
    FILE* openfile;

    openfile = fopen(filename, "w");
    if(openfile)
    {
        fprintf(openfile, "New File\n");
        fclose(openfile);
        return sdf_add_file(filename, NULL);
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_save(char* filename)
{
    // <ZZ> This function writes the memory copy of datafile.sdf to disk.  Files that have been
    //      marked for deletion in the index are skipped.  The function should return FALSE if
    //      there's a problem writing, or TRUE if it all worked.

    FILE* openfile;
    int i;
    unsigned int j;
    unsigned char* read;
    unsigned char flags;
    unsigned int offset;
    unsigned int size;
    char temp[32];
    unsigned int files_to_write;


    if(sdf_is_loaded && sdf_can_save)
    {
        // Open the file for writing
        openfile = fopen(filename, "wb");
        if(openfile)
        {
            // Figure out how many files there are to write
            files_to_write = 0;
            repeat(i, sdf_num_files)
            {
                read = sdf_index + (i<<4);

                // Should this file be saved?
                flags = *(read+4);
                if(sdf_should_save[flags & 15])
                {
                    files_to_write++;
                }
            }



            // Write the header
            fprintf(openfile, "----------------");
            fprintf(openfile, "SOULFU DATA FILE");
            sprintf(temp, "%d FILES         ", files_to_write);
            repeat(j, 16) { fprintf(openfile, "%c", temp[j]); }
            fprintf(openfile, "----------------");



            // Write the index
            offset = 0;
            repeat(i, sdf_num_files)
            {
                read = sdf_index + (i<<4);

                // Should this file be saved?
                flags = *(read+4);
                if(sdf_should_save[flags & 15])
                {
                    // Write the offset
                    fprintf(openfile, "%c", ((unsigned char) ((offset >> 24) & 255)) + OBFUSCATEA);
                    fprintf(openfile, "%c", ((unsigned char) ((offset >> 16) & 255)) + OBFUSCATEA);
                    fprintf(openfile, "%c", ((unsigned char) ((offset >> 8) & 255)) + OBFUSCATEA);
                    fprintf(openfile, "%c", ((unsigned char) (offset & 255)) + OBFUSCATEA);


                    // Write the flags
                    fprintf(openfile, "%c", flags + OBFUSCATEB);


                    // Write the size
                    size = sdf_read_unsigned_int(read+4) & 0x00FFFFFF;
                    fprintf(openfile, "%c", ((unsigned char) ((size >> 16) & 255)) + OBFUSCATEB);
                    fprintf(openfile, "%c", ((unsigned char) ((size >> 8) & 255)) + OBFUSCATEB);
                    fprintf(openfile, "%c", ((unsigned char) (size & 255)) + OBFUSCATEB);
                    offset+=size;


                    // Write the filename
                    repeat(j, 8) { fprintf(openfile, "%c", *(read+j+8) + OBFUSCATEC); }
                }
            }
            // End of the index...
            fprintf(openfile, "----------------");


            // Write the filedata
            repeat(i, sdf_num_files)
            {
                read = sdf_index + (i<<4);

                // Should this file be saved?
                flags = *(read+4);
                if(sdf_should_save[flags & 15])
                {
                    // Get the size and memory location
                    offset = sdf_read_unsigned_int(read);
                    size = sdf_read_unsigned_int(read+4) & 0x00FFFFFF;

                    repeat(j, size)
                    {
                        ((unsigned char*) offset)[j] += OBFUSCATED;
                    }


                    // Write the data
                    fwrite((unsigned char*) offset, 1, size, openfile);


                    repeat(j, size)
                    {
                        ((unsigned char*) offset)[j] -= OBFUSCATED;
                    }
                }
            }



            // Close the file
            fclose(openfile);
            log_message("INFO:   DATAFILE.SDF file saved");
            return TRUE;
        }
        else
        {
            log_message("ERROR:  DATAFILE.SDF file could not be opened for saving");
        }
    }
    else
    {
        log_message("ERROR:  DATAFILE.SDF can't save before loading/after decompression");
    }

    return FALSE;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
    #define EXPORTRDYASDDD
    #define TELL_ME_ABOUT_EXPORT
#endif

signed char sdf_export_file(char* filename, char* filename_path)
{
    // <ZZ> This function writes a single file from memory to disk.  The function should return
    //      FALSE if there's a problem writing, or TRUE if it all worked.
    FILE* openfile;
    unsigned char* read;
    unsigned int size;
    unsigned char file_type;
    unsigned int lan_lines;
    #ifdef EXPORTRDYASDDD
        int length;
        float max_distance;
        unsigned short i, j, k, b;
        unsigned char c;
        float x, y, z;
        unsigned char* data_index_list;
        unsigned char* data_index_list_start;
        unsigned char* data_temp;
        unsigned char* data_other;
        unsigned char* link_file_name;
        unsigned char* start_data;
        unsigned short flags;
        unsigned char num_detail_level;
        unsigned char num_base_model;
        unsigned short num_bone_frame;
        unsigned short num_vertex;
        unsigned short num_tex_vertex;
        unsigned short num_joint;
        unsigned short num_bone;
        unsigned short num_strips;
        unsigned short vertex;
        unsigned short tex_vertex;
        unsigned short base_model;
        #define SPIT_UNSIGNED_CHAR(A) { fputc((A&255), openfile); }
        #define SPIT_UNSIGNED_SHORT(A) { fputc((A>>8), openfile); fputc((A&255), openfile); }
   #endif

    if(NULL==filename_path) filename_path = filename;


    // See if we can find the file
    read = sdf_find_index(filename);
    if(read)
    {
        // Open the file for writing
        #ifdef EXPORTRDYASDDD
            // Export RDY files as DDD...
            length = strlen(filename);
            if(length > 4)
            {
                if(filename[length-3] == 'R' || filename[length-3] == 'r')
                {
                    if(filename[length-2] == 'D' || filename[length-2] == 'd')
                    {
                        if(filename[length-1] == 'Y' || filename[length-1] == 'y')
                        {
                            filename[length-3] = 'D';
                            filename[length-2] = 'D';
                            filename[length-1] = 'D';
                        }
                    }
                }
            }
        #endif
        openfile = fopen(filename_path, "wb");
        if(openfile)
        {
            // Get the size and data location
            file_type = (*(read+4)) & 15;
            size = sdf_read_unsigned_int(read+4) & 0x00FFFFFF;
            read = (unsigned char*) sdf_read_unsigned_int(read);


            #ifdef EXPORTRDYASDDD
                // Export RDY files as DDD...
                if(file_type == SDF_FILE_IS_RDY)
                {
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Exporting RDY file (%s)", filename);
#endif
                    // Put the name back to how we found it...
                    start_data = read;
                    if(length > 4)
                    {
                        filename[length-3] = 'R';
                        filename[length-2] = 'D';
                        filename[length-1] = 'Y';
                    }
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Restored filename (%s)", filename);
#endif

                    // Figure out the optimal number to use for scale...
                    flags = *((unsigned short*) read);  read+=2;
                    num_detail_level = *read;  read++;
                    num_base_model = *read;  read++;
                    num_bone_frame = *((unsigned short*) read);  read+=2;
                    data_index_list = read+(ACTION_MAX<<1)+(MAX_DDD_SHADOW_TEXTURE);
                    data_index_list_start = data_index_list;
                    max_distance = 1.0f;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Starting detail levels");
#endif
                    repeat(i, num_detail_level)
                    {
                        repeat(j, num_base_model)
                        {
                            data_temp = *((unsigned char**) data_index_list);  data_index_list += 20;
                            num_vertex = *((unsigned short*) data_temp);  data_temp+= 8;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Num vertex = %d", num_vertex);
#endif
                            repeat(k, num_vertex)
                            {
                                x = *((float *) data_temp);  data_temp+=4;
                                y = *((float *) data_temp);  data_temp+=4;
                                z = *((float *) data_temp);  data_temp+=4;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Vertex %d at (%f, %f, %f)", k, x, y, z);
#endif
                                data_temp+=52;
                                ABS(x);
                                ABS(y);
                                ABS(z);
                                if(x > max_distance) max_distance = x;
                                if(y > max_distance) max_distance = y;
                                if(z > max_distance) max_distance = z;
                            }
                        }
                    }
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Got vertex scale...  %f", max_distance);
#endif
                    repeat(i, num_bone_frame)
                    {
                        // Find out how many joints in the base model for this frame...
                        data_temp = *((unsigned char**) data_index_list);
                        base_model = *(data_temp+2);
                        data_temp = *((unsigned char**) (data_index_list_start+(base_model*20)));
                        data_temp+=4;
                        num_joint = *((unsigned short*) data_temp);
                        data_temp+=2;
                        num_bone = *((unsigned short*) data_temp);
                        data_temp = *((unsigned char**) data_index_list);  data_index_list += 4;
                        data_temp += (num_bone*24)+11;
                        repeat(j, num_joint)
                        {
                                x = *((float *) data_temp);  data_temp+=4;
                                y = *((float *) data_temp);  data_temp+=4;
                                z = *((float *) data_temp);  data_temp+=4;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Joint %d at (%f, %f, %f)", j, x, y, z);
#endif
                                ABS(x);
                                ABS(y);
                                ABS(z);
                                if(x > max_distance) max_distance = x;
                                if(y > max_distance) max_distance = y;
                                if(z > max_distance) max_distance = z;
                        }
                        repeat(j, MAX_DDD_SHADOW_TEXTURE)
                        {
                            k = *data_temp;  data_temp++;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Shadow %d alpha = %d", j, k);
#endif
                            if(k)
                            {
                                repeat(k, 4)
                                {
                                    x = *((float *) data_temp);  data_temp+=4;
                                    y = *((float *) data_temp);  data_temp+=4;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Shadow vertex %d at (%f, %f)", k, x, y);
#endif
                                    ABS(x);
                                    ABS(y);
                                    if(x > max_distance) max_distance = x;
                                    if(y > max_distance) max_distance = y;
                                }
                            }
                        }
                    }
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Got joint/shadow scale...  %f", max_distance);
#endif


                    // Now write the data...
                    i = (unsigned short) max_distance;
                    i++;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:   Starting to spit DDD file...");
#endif
                    SPIT_UNSIGNED_SHORT(i);                 // Scale
                    max_distance = (float) i;
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Final max distance = %f", max_distance);
#endif
                    SPIT_UNSIGNED_SHORT(flags);             // Flags
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Flags = %d", flags);
#endif
                    SPIT_UNSIGNED_CHAR(0);                  // Padding
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Padding = %d", 0);
#endif
                    SPIT_UNSIGNED_CHAR(num_base_model);     // Number of base models
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Num_base_model = %d", num_base_model);
#endif
                    SPIT_UNSIGNED_SHORT(num_bone_frame);    // Frames of animation
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Num_bone_frame = %d", num_bone_frame);
#endif
                    // Skip action list...
                    read+=(ACTION_MAX<<1);
                    repeat(i, MAX_DDD_SHADOW_TEXTURE)
                    {
                        j = *read;  read++;
                        SPIT_UNSIGNED_CHAR(j);              // Shadow texture index
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Shadow texture %d = %d", i, j);
#endif
                    }
                    // Spit out the external linkage, if it's valid...
                    if(flags & DDD_EXTERNAL_BONE_FRAMES)
                    {
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     External linkage is valid, so spitting filename...");
#endif
                        link_file_name = render_get_set_external_linkage(start_data, NULL);
                        j = 0;
                        repeat(i, 8)
                        {
                            if(j == 0)
                            {
                                SPIT_UNSIGNED_CHAR(link_file_name[i]);
#ifdef TELL_ME_ABOUT_EXPORT
    if(link_file_name[i] != 0)
    {
        log_message("INFO:       %c", link_file_name[i]);
    }
    else
    {
        log_message("INFO:       0");
    }
#endif
                                if(link_file_name[i] == 0) j++;
                            }
                            else
                            {
                                SPIT_UNSIGNED_CHAR(0);
#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:       0");
#endif
                            }
                        }
                    }


#ifdef TELL_ME_ABOUT_EXPORT
    log_message("INFO:     Done with Action Export...");
#endif

                    // Jump to first base model of last detail level...
                    read+=((DETAIL_LEVEL_MAX-1)*num_base_model*20);
                    data_index_list = read;
                    repeat(i, num_base_model)
                    {
//log_message("INFO:     Reading base model indices");
                        data_temp = *((unsigned char**) read);  read += 20;
//log_message("INFO:     data_temp = %u", data_temp);
                        num_vertex = *((unsigned short*) data_temp);  data_temp+=2;
                        num_tex_vertex = *((unsigned short*) data_temp);  data_temp+=2;
                        num_joint = *((unsigned short*) data_temp);  data_temp+=2;
                        num_bone = *((unsigned short*) data_temp);  data_temp+=2;
                        SPIT_UNSIGNED_SHORT(num_vertex);
                        SPIT_UNSIGNED_SHORT(num_tex_vertex);
                        SPIT_UNSIGNED_SHORT(num_joint);
                        SPIT_UNSIGNED_SHORT(num_bone);
//log_message("INFO:     num_vertex = %d", num_vertex);
//log_message("INFO:     num_tex_vertex = %d", num_tex_vertex);
//log_message("INFO:     num_joint = %d", num_joint);
//log_message("INFO:     num_bone = %d", num_bone);
                        repeat(j, num_vertex)
                        {
                            // Vertex coordinates
                            x = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                            y = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                            z = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
//log_message("INFO:     vertex position = (%f, %f, %f)", x, y, z);
//log_message("INFO:     exported vertex position = (%d, %d, %d)", (unsigned short) ((signed short) x), (unsigned short) ((signed short) y), (unsigned short) ((signed short) z));
                            k = (unsigned short) ((signed short) x);
                            SPIT_UNSIGNED_SHORT(k);
                            k = (unsigned short) ((signed short) y);
                            SPIT_UNSIGNED_SHORT(k);
                            k = (unsigned short) ((signed short) z);
                            SPIT_UNSIGNED_SHORT(k);
                            // Bone bindings and weighting
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            data_temp+=36;  // Skip normal and scalar coordinates
                            data_temp+=13;  // Skip padding
                        }
//log_message("INFO:     Done with vertices");
                        repeat(j, num_tex_vertex)
                        {
                            x = *((float *) data_temp) * 256.0f;  data_temp+=4;
                            y = *((float *) data_temp) * 256.0f;  data_temp+=4;
                            k = (unsigned short) ((signed short) x);
                            SPIT_UNSIGNED_SHORT(k);
                            k = (unsigned short) ((signed short) y);
                            SPIT_UNSIGNED_SHORT(k);
                        }
//log_message("INFO:     Done with tex vertices");
                        repeat(j, MAX_DDD_TEXTURE)
                        {
                            if(*data_temp)
                            {
                                SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                                SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                                SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                                num_strips = *((unsigned short*) data_temp);  data_temp+=2;
                                SPIT_UNSIGNED_SHORT(num_strips);
                                repeat(k, num_strips)
                                {
                                    data_temp+=2;  // Skip number of points, assume 3...
                                    repeat(b, 3)
                                    {
                                        vertex = *((unsigned short*) data_temp);  data_temp+=2;
                                        tex_vertex = *((unsigned short*) data_temp);  data_temp+=2;
                                        SPIT_UNSIGNED_SHORT(vertex);
                                        SPIT_UNSIGNED_SHORT(tex_vertex);
                                    }
                                }
                                // No fans in Devtool...
                                data_temp+=2;
                            }
                            else
                            {
                                SPIT_UNSIGNED_CHAR(0);
                                data_temp++;
                            }
                        }
//log_message("INFO:     Done with textures");
                        repeat(j, num_joint)
                        {
                            k = (unsigned short) (*((float*) data_temp) / JOINT_COLLISION_SCALE);
                            SPIT_UNSIGNED_CHAR(k);  data_temp+=4;
                        }
//log_message("INFO:     Done with joints");
                        repeat(j, num_bone)
                        {
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            k = *((unsigned short*) data_temp);  data_temp+=2;
                            b = *((unsigned short*) data_temp);  data_temp+=2;
                            SPIT_UNSIGNED_SHORT(k);
                            SPIT_UNSIGNED_SHORT(b);
                            data_temp+=4;  // Skip bone length
                        }
//log_message("INFO:     Done with bones");
                    }



                    // Jump to first frame of animation
                    if(!(flags & DDD_EXTERNAL_BONE_FRAMES))
                    {
                        data_temp = *((unsigned char**) read);
                        repeat(i, num_bone_frame)
                        {
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            SPIT_UNSIGNED_CHAR(*data_temp);  data_temp++;
                            base_model = *data_temp;  data_temp++;
                            SPIT_UNSIGNED_CHAR(base_model);
                            k = (unsigned short) ((signed short) (*((float*) data_temp)*256.0f));  data_temp+=4;
                            SPIT_UNSIGNED_SHORT(k);
                            k = (unsigned short) ((signed short) (*((float*) data_temp)*256.0f));  data_temp+=4;
                            SPIT_UNSIGNED_SHORT(k);

                            data_other = data_index_list + (base_model*20);
                            data_other = *((unsigned char**) data_other);
                            data_other += 4;
                            num_joint = *((unsigned short*) data_other);  data_other+=2;
                            num_bone = *((unsigned short*) data_other);
                            repeat(j, num_bone)
                            {
                                // Write forward normal
                                x = *((float*) data_temp);  data_temp+=4;
                                y = *((float*) data_temp);  data_temp+=4;
                                z = *((float*) data_temp);  data_temp+=4;
                                x = (x*32767.0f);
                                y = (y*32767.0f);
                                z = (z*32767.0f);
                                SPIT_UNSIGNED_SHORT(((signed short) x));
                                SPIT_UNSIGNED_SHORT(((signed short) y));
                                SPIT_UNSIGNED_SHORT(((signed short) z));
                                // Skip side normal
                                data_temp+=12;
                            }
                            repeat(j, num_joint)
                            {
                                x = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                                y = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                                z = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
    //log_message("INFO:   joint position = (%f, %f, %f)", x, y, z);
                                k = (unsigned short) ((signed short) x);
                                SPIT_UNSIGNED_SHORT(k);
                                k = (unsigned short) ((signed short) y);
                                SPIT_UNSIGNED_SHORT(k);
                                k = (unsigned short) ((signed short) z);
                                SPIT_UNSIGNED_SHORT(k);
                            }
                            repeat(k, MAX_DDD_SHADOW_TEXTURE)
                            {
                                c = *data_temp;  data_temp++;  // Alpha
                                SPIT_UNSIGNED_CHAR(c);
                                if(c)
                                {
                                    repeat(c, 4)
                                    {
                                        x = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                                        y = *((float *) data_temp) * DDD_SCALE_WEIGHT / max_distance;  data_temp+=4;
                                        b = (unsigned short) ((signed short) x);
                                        SPIT_UNSIGNED_SHORT(b);
                                        b = (unsigned short) ((signed short) y);
                                        SPIT_UNSIGNED_SHORT(b);
                                    }
                                }
                            }
                        }
                    }
                }
                else
            #endif
                {
                    if(file_type == SDF_FILE_IS_TXT || file_type == SDF_FILE_IS_SRC || file_type == SDF_FILE_IS_LAN)
                    {
                        // Chop out language file header...
                        if(file_type == SDF_FILE_IS_LAN)
                        {
                            lan_lines = sdf_read_unsigned_int(read);
                            read = read + 4 + (lan_lines<<2);
                            size = size - (4 + (lan_lines<<2));
                        }

                        // Write data in original format...  0D 0A style returns instead of null term...
                        while(size > 0)
                        {
                            if(*read == 0)
                            {
                                fprintf(openfile, "\r\n");
                            }
                            else
                            {
                                fprintf(openfile, "%c", *read);
                            }
                            size--;
                            read++;
                        }
                    }
                    else
                    {
                        // Compute the waypoint table data before saving...
                        #ifdef DEVTOOL
                            if(file_type == SDF_FILE_IS_SRF)
                            {
                                room_srf_build_waypoint_table(read);
                            }
                        #endif


                        // Write the raw data
                        fwrite(read, 1, size, openfile);
                    }
                }



            // Close the file
            fclose(openfile);
            log_message("INFO:   Exported file %s from datafile...", filename);
            return TRUE;
        }
        else
        {
            log_message("ERROR:  %s could not be opened for writing", filename);
        }
    }
    else
    {
        log_message("ERROR:  %s could not be found in datafile", filename);
    }



    return FALSE;
}

//-----------------------------------------------------------------------------------------------
void sdf_decode(unsigned char mask, unsigned char draw_loadin)
{
    // <ZZ> This function chews on the raw data for every compressed file until it becomes
    //      something useful (ie. convert JPG->RGB).  Depending on the compile time options,
    //      the function may try to recycle space by deleting the compressed data, or it may
    //      not.  Mask lets me pick SDF_ALL or SDF_FLAG_WAS_UPDATED...  Draw_loadin tells us
    //      whether or not we should draw the start up loading screen...
    int i;
    unsigned char filename[9];
    unsigned char filetype;
    signed char lastoneworked;
    unsigned char delete_name[13];
    unsigned char * index;


    // Go through each file in the index
    log_message("INFO:   Decompressing SDF subfiles (%d of 'em)...", sdf_num_files);
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        // Draw the loading screen...
        if(draw_loadin && ((i & 63)==63))
        {
            display_loadin(0.50f * ((float) i)/((float) sdf_num_files));
        }


        // Check the type of file to decompress, then hand off to a subroutine...
        lastoneworked = TRUE;
        if(*(index+4) & mask)
        {
            sdf_get_filename(i, filename, &filetype);
            switch(filetype)
            {
                case SDF_FILE_IS_JPG:
                    if(mask != SDF_ALL)
                    {
                        sprintf(delete_name, "%s.RGB", filename);
                        sdf_delete_file(delete_name);
                    }
                    lastoneworked = decode_jpg(index, filename);
                    break;

                case SDF_FILE_IS_OGG:
                    if(mask != SDF_ALL)
                    {
                        sprintf(delete_name, "%s.RAW", filename);
                        sdf_delete_file(delete_name);
                    }
                    lastoneworked = decode_ogg(index, filename);
                    break;

                case SDF_FILE_IS_PCX:
                    if(mask != SDF_ALL)
                    {
                        sprintf(delete_name, "%s.RGB", filename);
                        sdf_delete_file(delete_name);
                    }
                    lastoneworked = decode_pcx(index, filename);
                    break;

                case SDF_FILE_IS_DDD:
                    if(mask != SDF_ALL)
                    {
                        sprintf(delete_name, "%s.RDY", filename);
                        sdf_delete_file(delete_name);
                    }
                    lastoneworked = decode_ddd(index, filename);
//                    if(filename[0] == 'T' && filename[1] == 'I' && filename[2] == 'L' && filename[3] == 'E')
//                    {
//                        // Need to do funny RDY+DDD->TIL thing for tiles...
//                        lastoneworked = tile_build_from_ddd_and_rdy(global_ddd_file_start, global_rdy_file_start, filename) & lastoneworked;
//                    }
                    break;

                default:
                    break;
            }
        }
        if(!lastoneworked)
        {
            log_message("ERROR:  Problem decoding %s.%s", filename, sdf_extension[filetype]);
        }
    }


    // Finish up
    log_message("INFO:   SDF Decompression finished");
}

//-----------------------------------------------------------------------------------------------
signed char sdf_open(char* filename)
{
    // <ZZ> This function opens a file in the database for reading line by line like a text
    //      document.  It returns TRUE if the file was found, or FALSE if not.
    sdf_read_line_number = 0;
    sdf_read_remaining = 0;
    sdf_read_first_line = FALSE;
    sdf_read_file = sdf_find_index(filename);
    if(sdf_read_file)
    {
        sdf_read_remaining = sdf_read_unsigned_int(sdf_read_file+4) & 0x00FFFFFF;
        sdf_read_file = (unsigned char*) sdf_read_unsigned_int(sdf_read_file);
        return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char sdf_read_line(void)
{
    // <ZZ> This function places sdf_read_file at the start of the next good line in a file
    //      opened by sdf_open().  If something goes wrong, it returns FALSE.  If something
    //      goes right, it returns TRUE.


    // Make sure we have an open file
    if(sdf_read_file)
    {
        // Don't advance the read head on the first call...
        if(sdf_read_first_line == FALSE)
        {
            sdf_read_first_line = TRUE;
        }
        else
        {
            // Skip over the line until we hit a 0
            while((*sdf_read_file) != 0 && sdf_read_remaining > 0)
            {
                sdf_read_remaining--;
                sdf_read_file++;
            }
        }


        // Skip any 0's, so sdf_read_file starts on some good characters...
        while((*sdf_read_file) == 0 && sdf_read_remaining > 0)
        {
            sdf_read_remaining--;
            sdf_read_file++;
            sdf_read_line_number++;
        }


        // Make sure we didn't run out of data...
        if(sdf_read_remaining) return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
signed char sdf_insert_data(unsigned char* file_pos, unsigned char* data_to_add, int bytes_to_add)
{
    // <ZZ> This function adds some bytes to a file, at any location within the file.
    unsigned char* index;
    unsigned int file_start;
    signed int file_size;
    unsigned char file_type;
    int bytes_to_subtract;
    int i, j;
    unsigned int offset, file_offset;


    // First find the index information for the file, by checking every entry we've got...
    index = sdf_index;
    repeat(i, sdf_num_files)
    {
        file_start = sdf_read_unsigned_int(index);
        file_size  = sdf_read_unsigned_int(index+4)&0x00FFFFFF;
        file_type = (*(index+4));
        if((file_type&15) == SDF_FILE_IS_SRC || (file_type&15) == SDF_FILE_IS_TXT)
        {
            // Don't allow last 0 to be removed...
            file_size--;
        }

        if((unsigned int) file_pos >= file_start && (unsigned int) file_pos <= (file_start+file_size) && (file_type&15) != SDF_FILE_IS_UNUSED)
        {
            // We found a match...  Make sure we have enough memory reserved to perform the operation...
            if(bytes_to_add < 0)
            {
                // Removing data...  Figure out how many bytes we're really getting rid of...
                bytes_to_subtract = -bytes_to_add;
                if(file_start+file_size > ((unsigned int) file_pos)+bytes_to_subtract)
                {
                    // Trying to remove interior bytes...  Need to shuffle data...
                    memmove(file_pos, file_pos+bytes_to_subtract, file_size+file_start-((unsigned int) file_pos));
                }
                file_size-=bytes_to_subtract;


                // Adjust header offsets...
                file_offset = ((unsigned int) file_pos) - file_start;
                if((file_type&15) == SDF_FILE_IS_SRF)
                {
                    repeat(j, 14)
                    {
                        offset = sdf_read_unsigned_int((unsigned char*) (file_start+40+(j<<2)));
                        if(offset >= file_offset)
                        {
                            if(offset > ((unsigned int) bytes_to_subtract))
                            {
                                offset-=bytes_to_subtract;
                                sdf_write_unsigned_int((unsigned char*) (file_start+40+(j<<2)), offset);
                            }
                        }
                    }
                }


                // Pad end of file with 0's
                memset(((unsigned char*) file_start)+file_size, 0, bytes_to_subtract);
                if((file_type&15) == SDF_FILE_IS_SRC || (file_type&15) == SDF_FILE_IS_TXT)
                {
                    // Don't allow last 0 to be removed...
                    file_size++;
                }


                // Then write down how big the file is, so we don't forget...
                if(file_size < 0) file_size = 0;
                sdf_write_unsigned_int(index+4, file_size);
                *(index+4) = file_type | SDF_FLAG_WAS_UPDATED;
                return TRUE;
            }
            else
            {
                // Adding data...
                if((int) (file_size + bytes_to_add) < sdf_dev_size[file_type&15])
                {
                    if((file_type&15) == SDF_FILE_IS_SRC || (file_type&15) == SDF_FILE_IS_TXT)
                    {
                        // Don't allow last 0 to be removed...
                        file_size++;
                    }


                    // We have enough room to add the data...  Start by shuffling the old data so we
                    // don't overwrite anything...
                    memmove(file_pos+bytes_to_add, file_pos, file_size+file_start-((unsigned int) file_pos));


                    // Then copy in the new data
                    if(data_to_add)
                    {
                        // Copy from the specified location
                        memcpy(file_pos, data_to_add, bytes_to_add);
                    }
                    else
                    {
                        // Zero out the new data
                        memset(file_pos, 0, bytes_to_add);
                    }



                    // Adjust header offsets...
                    file_offset = ((unsigned int) file_pos) - file_start;
                    if((file_type&15) == SDF_FILE_IS_SRF)
                    {
                        repeat(j, 14)
                        {
                            offset = sdf_read_unsigned_int((unsigned char*) (file_start+40+(j<<2)));
                            if(offset >= file_offset)
                            {
                                offset+=bytes_to_add;
                                sdf_write_unsigned_int((unsigned char*) (file_start+40+(j<<2)), offset);
                            }
                        }
                    }


                    // Then write down how big the file is, so we don't forget...
                    file_size+=bytes_to_add;
                    sdf_write_unsigned_int(index+4, file_size);
                    *(index+4) = file_type | SDF_FLAG_WAS_UPDATED;
                    return TRUE;
                }
                return FALSE;
            }
        }
        else
        {
            // Keep looking...
            index+=16;
        }
    }
    return FALSE;
}
#endif

//-----------------------------------------------------------------------------------------------
void sdf_flag_set(unsigned char *filename, unsigned char flag)
{
    // <ZZ> This function sets a flag for a single file.  Flags may be or'd...
    unsigned char* file_index;

    file_index = sdf_find_index(filename);
    if(file_index)
    {
        *(file_index+4) |= flag;
    }
}

//-----------------------------------------------------------------------------------------------
unsigned int sdf_checksum(unsigned char* data, int size)
{
    // <ZZ> This function generates a checksum based on the file's data...
    unsigned int checksum;
    int i, j;
    checksum = 0;
    i = 0;
    j = 0;
    while(i < size)
    {
        checksum = checksum ^ (((unsigned int) random_table[*data]) << j);
        data++;
        i++;
        j++;
        if(j > 24)  j = 0;
    }
    return checksum;
}

//-----------------------------------------------------------------------------------------------
void sdf_delete_all_files(unsigned char type_to_delete, unsigned char* prefix_string)
{
    // <ZZ> Deletes all files of a given type
    int i;
    unsigned char filename[9];
    unsigned char newfilename[20];
    unsigned char filetype;
    unsigned short length, j, found_mismatch;


    // Go through each file in the index
    length = 0;
    if(prefix_string)
    {
        log_message("INFO:   Deleting all %s files with prefix %s...", sdf_extension[type_to_delete], prefix_string);
        length = strlen(prefix_string);
    }
    else
    {
        log_message("INFO:   Deleting all %s files...", sdf_extension[type_to_delete]);
    }

    repeat(i, sdf_num_files)
    {
        sdf_get_filename(i, filename, &filetype);

        if((filetype&15) == type_to_delete)
        {
            found_mismatch = FALSE;
            repeat(j, length)
            {
                if(filename[j] != prefix_string[j])
                {
                    found_mismatch = TRUE;
                    j = length;
                }
            }

            if(found_mismatch == FALSE)
            {
                sprintf(newfilename, "%s.%s", filename, sdf_extension[type_to_delete]);
                sdf_delete_file(newfilename);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void sdf_list_all_files(unsigned char type_to_list, unsigned char* prefix_string, unsigned char export_too)
{
    // <ZZ> Lists all files of a given type
    int i;
    unsigned char filename[9];
    unsigned char filetype;
    unsigned short length, j, found_mismatch;


    // Go through each file in the index
    length = 0;
    if(prefix_string)
    {
        log_message("INFO:   Listing all %s files with prefix %s...", sdf_extension[type_to_list], prefix_string);
        length = strlen(prefix_string);
    }
    else
    {
        log_message("INFO:   Listing all %s files...", sdf_extension[type_to_list]);
    }
    if(export_too)
    {
        log_message("INFO:   Exporting 'em all too...");
    }

    repeat(i, sdf_num_files)
    {
        sdf_get_filename(i, filename, &filetype);
        if((filetype&15) == type_to_list)
        {
            found_mismatch = FALSE;
            repeat(j, length)
            {
                if(filename[j] != prefix_string[j])
                {
                    found_mismatch = TRUE;
                    j = length;
                }
            }

            if(found_mismatch == FALSE)
            {
                log_message("INFO:     Found %s.%s", filename, sdf_extension[type_to_list]);
                if(export_too)
                {
                    sprintf(run_string[0], "%s.%s", filename, sdf_extension[type_to_list]);
                    sdf_export_file(run_string[0], NULL);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void sdf_reorganize_index(void)
{
    // <ZZ> This function gets rid of all the deleted file entries in the index.  Simple cleanup
    //      routine...
    //
    // <BB> Modified to use a lot less copying

    int i, j;
    unsigned char filename[9];
    unsigned char filetype;
    unsigned char *index_top, *index_bot;


    // Go through each file in the index
    log_message("INFO:   Reorganizing index...");

    i = 0;
    j = sdf_num_files-1;
    while(i < j)
    {
        //find first non-empty value, starting at the top of the stack
        while(j >= i)
        {
          sdf_get_filename(j, filename, &filetype);
          if(SDF_FILE_IS_UNUSED != (filetype&15)) break;

          //a file at the top of the stack is "free", so adjust the stack counters
          sdf_num_files--;
          sdf_extra_files++;
          j--;
        };

        //if the index stack is now packed, we are done
        if(j==i) return;

        index_bot = sdf_index + (i<<4);
        index_top = sdf_index + (j<<4);

        sdf_get_filename(i, filename, &filetype);
        if((filetype&15) == SDF_FILE_IS_UNUSED)
        {
          //copy the non-trivial
          memcpy(index_bot, index_top, 16);

          sdf_num_files--;
          sdf_extra_files++;
        }
        i++;
    }
}

//-----------------------------------------------------------------------------------------------
int sdf_find_index_by_data(unsigned char* file_start)
{
    // <ZZ> This function returns the index number of a file, given a pointer to the file's starting
    //      data...  Returns 65535 if no match found...
    int i;
    unsigned char* index;

    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        if(SDF_FILE_IS_UNUSED != (*(index+4)&15))
        {
            index = (unsigned char*) sdf_read_unsigned_int(index);
            if(index == file_start)
            {
                return i;
            }
        }
    }
    return 65535;
}

//-----------------------------------------------------------------------------------------------
// OLD NETWORK STUFF
/*
unsigned short sdf_find_filetype_index(unsigned char* filedata, unsigned char filetype)
{
    // <ZZ> This function tells us that a given file is the fourth .RGB file, or the
    //      28th .RUN file, etc...  Used for network transfers, since actual
    //      memory addresses can't be used...  Returns 65535 if no match was found...
    unsigned short i;
    unsigned short search_index;
    unsigned char* search_filedata;
    unsigned char* index;



    // Search through every file in the datafile...
    search_index = 0;
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        // Match types...
        if(filetype == ((*(index+4))&15)  )
        {
            // Types match, but does the file data?
            search_filedata = (unsigned char*) sdf_read_unsigned_int(index);
            if(search_filedata == filedata)
            {
                // Looks like we found our file...
                return search_index;
            }
            search_index++;
        }
    }
    // We didn't find our file...
    return 65535;
}

//-----------------------------------------------------------------------------------------------
unsigned char* sdf_resolve_filetype_index(unsigned short filetype_index, unsigned char filetype)
{
    // <ZZ> This function returns the start of a file's data, when it's told something like
    //      fourth .RGB file, or 28th .RUN file...  Used for network transfers, since actual
    //      memory addresses can't be used...  Returns NULL if the index couldn't be resolved...
    unsigned short i;
    unsigned short search_index;
    unsigned char* search_filedata;
    unsigned char* index;


    if(filetype_index < 65535)
    {
        search_index = 0;
        repeat(i, sdf_num_files)
        {
            index = sdf_index + (i<<4);

            // Match the filetypes...
            if(filetype == ((*(index+4))&15)  )
            {
                // Okay...  Looks like this file is the correct type, but is it the
                // one we're lookin' for?
                if(filetype_index == search_index)
                {
                    // Yup it is...
                    search_filedata = (unsigned char*) sdf_read_unsigned_int(index);
                    return search_filedata;
                }
                search_index++;
            }
        }
    }
    // The filetype index wasn't valid...
    return NULL;
}
*/

//-----------------------------------------------------------------------------------------------
