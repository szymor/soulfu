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

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Put stuff up here....
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



#define LEFT 0
#define RIGHT 1

#define INFI_LOOP_FADE 3                // How fast insect buzzing can go away (to make it not choppy)


#define SDL_BUFFER_SIZE 2048
#define SOUND_BUFFER_SIZE 65536         // Must be 65536, for unsigned short wrap around...
unsigned short sound_buffer_read;
unsigned short sound_buffer_write;
unsigned short sound_buffer_advancement;
signed int     sound_buffer[SOUND_BUFFER_SIZE];

unsigned char sound_flip_pan = FALSE;   // Flip left & right speakers...

unsigned char main_volume;              // For fades...
unsigned char master_sfx_volume;        // User option
unsigned char master_music_volume;      // User option



#define MAX_CHANNEL 64                  // Maximum number of simultaneous sounds
signed char    channel_active[MAX_CHANNEL];
signed short*  channel_data[MAX_CHANNEL];
signed int     channel_start_delay[MAX_CHANNEL];
unsigned int   channel_position[MAX_CHANNEL];
unsigned short channel_position_add[MAX_CHANNEL];
unsigned int   channel_position_end[MAX_CHANNEL];
unsigned int   channel_total_position[MAX_CHANNEL];
unsigned int   channel_total_position_end[MAX_CHANNEL];
unsigned char  channel_volume[MAX_CHANNEL][2];
unsigned char  channel_new_volume[MAX_CHANNEL][2];
unsigned int   channel_loop_start[MAX_CHANNEL];
unsigned int   channel_fade_start[MAX_CHANNEL];     // Time after which it starts fading out
unsigned int   channel_fade_step[MAX_CHANNEL][2];   // Fade by 1 every so many samples
unsigned int   channel_fade_count[MAX_CHANNEL][2];  // For counting our steps
unsigned char  channel_infinite_loop[MAX_CHANNEL];
unsigned char  unused_channel[MAX_CHANNEL];
unsigned char  unused_channel_count;


#define MUSIC_STOP 0
#define MUSIC_ONCE 1
#define MUSIC_PLAY 2
unsigned char* music_file_instrument = NULL;
unsigned char* music_file_note = NULL;
int music_num_instrument;
int music_num_note;
int music_first_note;
int music_next_note;
int music_intro_time;
int music_tempo;
signed int music_time;
unsigned char music_mode = MUSIC_STOP;

#define MAX_PITCH 32
unsigned short pitch_table[MAX_PITCH] = {512, 484, 455, 430, 405, 383, 362, 341, 322, 304, 287, 271, 256, 242, 228, 215, 203, 192, 181, 171, 161, 152, 144, 135, 128};

#define MAX_INSTRUMENT 256
unsigned char* music_instrument_table[MAX_INSTRUMENT];

//-----------------------------------------------------------------------------------------------
void play_sound(unsigned int start_delay, unsigned char* sound_file_start, unsigned char volume, unsigned char pan, unsigned short pitch_skip, unsigned char* loop_data)
{
    // <ZZ> This function plays a sound.  If start_delay is 0, the sound is played as soon as
    //      possible, otherwise the sound is offset to the correct location in the sound_buffer
    //      (mostly for playing music...).  Volume 255 is max.  Pan 128 is middle.  Pitch_skip
    //      256 is natural frequency.  Loop_data should be NULL, unless the sound
    //      is a musical instrument with loop information.  Note that this function doesn't
    //      actually *play* the sound, it merely assigns a channel and sets up all of the
    //      necessary data.
    unsigned char channel;
    unsigned char fade_percent;
    unsigned short duration;
    unsigned int samples;


    // Make sure we have a free channel to play the sound on...
    if(unused_channel_count > 0 && sound_file_start && volume > 2)
    {
        // Get the channel...
        unused_channel_count--;
        channel = unused_channel[unused_channel_count];


        // Flip left & right speakers...
        pan = (sound_flip_pan == TRUE) ? (~pan): pan;


        // New stuff...
        samples = *((unsigned int*) sound_file_start); // 4 byte header for RAW...  
        channel_total_position[channel] = 0;
        channel_total_position_end[channel] = 0;
        if(pitch_skip > 0)
        {
            channel_total_position_end[channel] = (samples<<8)/pitch_skip;
        }
        channel_fade_start[channel] = channel_total_position_end[channel];


        // Fill in the channel information
        channel_active[channel] = TRUE;
        channel_data[channel] = (signed short*) (sound_file_start+4);
        channel_start_delay[channel] = start_delay<<1;
        channel_position[channel] = 0;
        channel_position_add[channel] = pitch_skip;
        channel_position_end[channel] = samples << 10; // 10 for subsampling...
        channel_volume[channel][LEFT] =  (pan <= 128 ? volume : (((255-pan)*volume)>>7));
        channel_volume[channel][RIGHT] = (pan >= 128 ? volume : ((pan*volume)>>7));
        channel_infinite_loop[channel] = FALSE;


        if(loop_data)
        {
            // Read from the extended loop_data...
            duration = (*loop_data);  loop_data++;  duration = duration<<8;
            duration+= (*loop_data);  loop_data++;
            fade_percent =  (*loop_data);  loop_data++;


            // Is looping/fading enabled???
            if(fade_percent != 0)
            {
                // Yup, so make sure we have room to loop...
                channel_loop_start[channel] = (*loop_data);  loop_data++;  channel_loop_start[channel] = channel_loop_start[channel]<<8;
                channel_loop_start[channel]+= (*loop_data);  loop_data++;  channel_loop_start[channel] = channel_loop_start[channel]<<8;
                channel_loop_start[channel]+= (*loop_data);  channel_loop_start[channel] = channel_loop_start[channel]<<10;  // 10 for subsampling
                if(channel_loop_start[channel] > channel_position_end[channel]-65536)
                {
                    channel_loop_start[channel] = channel_position_end[channel]-65536;
                    if(channel_loop_start[channel] >= channel_position_end[channel])
                    {
                        // Check for overflow...
                        channel_loop_start[channel] = 0;
                    }
                }


                // Figure out the actual duration...  Convert from 16th seconds...
                channel_total_position_end[channel] = duration*music_tempo;
                channel_fade_start[channel] = (channel_total_position_end[channel] * (255-fade_percent)) >> 8;  // Percentage in 256th notation...


                // Figure out how we fade the left side...
                if(channel_volume[channel][LEFT] > 0)
                {
                    channel_fade_step[channel][LEFT] = ((channel_total_position_end[channel] - channel_fade_start[channel]) / channel_volume[channel][LEFT]);
                }
                else
                {
                    channel_fade_step[channel][LEFT] = 0xFFFFFFFF;
                }
                channel_fade_count[channel][LEFT] = channel_fade_step[channel][LEFT];


                // Same for the right side...
                if(channel_volume[channel][RIGHT] > 0)
                {
                    channel_fade_step[channel][RIGHT] = ((channel_total_position_end[channel] - channel_fade_start[channel]) / channel_volume[channel][RIGHT]);
                }
                else
                {
                    channel_fade_step[channel][RIGHT] = 0xFFFFFFFF;
                }
                channel_fade_count[channel][RIGHT] = channel_fade_step[channel][RIGHT];
            }
            else
            {
                if(duration == 65535)
                {
                    // Infinite loop
                    channel_loop_start[channel] = (*loop_data);  loop_data++;  channel_loop_start[channel] = channel_loop_start[channel]<<8;
                    channel_loop_start[channel]+= (*loop_data);  loop_data++;  channel_loop_start[channel] = channel_loop_start[channel]<<8;
                    channel_loop_start[channel]+= (*loop_data);  channel_loop_start[channel] = channel_loop_start[channel]<<10;  // 10 for subsampling
                    if(channel_loop_start[channel] > channel_position_end[channel]-65536)
                    {
                        channel_loop_start[channel] = channel_position_end[channel]-65536;
                        if(channel_loop_start[channel] >= channel_position_end[channel])
                        {
                            // Check for overflow...
                            channel_loop_start[channel] = 0;
                        }
                    }


                    // Figure out the actual duration...  Convert from 16th seconds...
                    channel_total_position_end[channel] = channel_position_end[channel]<<2;
                    channel_fade_start[channel] = channel_total_position_end[channel];
                    channel_fade_step[channel][LEFT] = 0xFFFFFFFF;
                    channel_fade_count[channel][LEFT] = channel_fade_step[channel][LEFT];
                    channel_fade_step[channel][RIGHT] = 0xFFFFFFFF;
                    channel_fade_count[channel][RIGHT] = channel_fade_step[channel][RIGHT];
                    channel_infinite_loop[channel] = TRUE;
                    channel_volume[channel][LEFT] =  0;
                    channel_volume[channel][RIGHT] = 0;
                    channel_new_volume[channel][LEFT] =  0;
                    channel_new_volume[channel][RIGHT] = 0;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void copy_sound_buffer_to_sdl(void *udata, Uint8 *stream, int len)
{
    // <ZZ> This function copies our sound buffer over to SDL.  Automatically called by SDL
    //      whenever it needs more data...  Also clips the sound buffer to 16 bit...
    //      Udata is NULL.  Stream is a pointer to SDL's sound buffer.  Len is the number
    //      of bytes to feed to SDL.
    register signed int value;
    register signed short* write;
    register unsigned short read;
    unsigned short count;



    // Remember how far we advanced for future sounds...
    count = ((unsigned short) len)>>1;             // Worry about shorts, not bytes...
    sound_buffer_advancement+=count;
    music_time+=(count>>1);


    // Copy the data in two segments, to allow for wrap around of the sound_buffer
    write = (signed short*) stream;
    read = sound_buffer_read;
    sound_buffer_read+=count;


    // Copy the first segment over, clipping as we go...
    if(main_volume == 255)
    {
        // Normal operations...
        while(count > 0)
        {
            value = sound_buffer[read];
            clip(-32767, value, 32767);
            *write = (signed short) value;
            write++;
            read++;
            count--;
        }
    }
    else
    {
        // Fading in or out...
        while(count > 0)
        {
            value = sound_buffer[read];
            clip(-32767, value, 32767);
            value = (value*main_volume)>>8;
            *write = (signed short) value;
            write++;
            read++;
            count--;
        }
    }

// !!!BAD!!!
// !!!BAD!!!  Adjust the main volume for fading in or out...
// !!!BAD!!!
}

//-----------------------------------------------------------------------------------------------
void sound_reset_channels(void)
{
    // <ZZ> Stops all currently playing sounds
    repeat(unused_channel_count, MAX_CHANNEL)
    {
        channel_active[unused_channel_count] = FALSE;
        unused_channel[unused_channel_count] = unused_channel_count;
    }
}

//-----------------------------------------------------------------------------------------------
void sound_setup(void)
{
    // <ZZ> This function turns on sound, at 22khz, 16bit, stereo.
    SDL_AudioSpec wanted;
    log_message("INFO:   Trying to turn on audio");
    wanted.freq = 22050;                            // 22khz
    wanted.format = AUDIO_S16;                      // 16 bit
    wanted.channels = 2;                            // Stereo
    wanted.samples = (SDL_BUFFER_SIZE/2);           // Number of samples to copy on each callback
    wanted.callback = copy_sound_buffer_to_sdl;     // Callback function
    wanted.userdata = NULL;
    if (SDL_OpenAudio(&wanted, NULL) < 0)
    {
        log_message("INFO:     SDL couldn't turn on audio.  It's all SDL's fault.");
        return;
    }


    // Clear out the currently playing sounds...
    sound_buffer_read = 0;
    sound_buffer_advancement = 0;
    memset(sound_buffer, 0, SOUND_BUFFER_SIZE<<2);
    sound_reset_channels();



    // ...And begin playback...  Now anything that is written to sound_buffer should
    // eventually propogate to the sound card...
    main_volume = 255;
    master_sfx_volume = 255;
    master_music_volume = 255;
    SDL_PauseAudio(0);
    log_message("INFO:   Audio initialized");
}

//-----------------------------------------------------------------------------------------------
void fill_sound_buffer(void)
{
    // <ZZ> This function adds more data to the sound_buffer, and should be called every frame
    //      update...
    unsigned char channel;
    register unsigned short write;
    signed short* read_data;
    register unsigned int position;
    register signed int value;
    register unsigned char left_volume;
    register unsigned char right_volume;
    unsigned short position_add;
    unsigned short length;
    unsigned short length_save;
    unsigned int position_end;
    unsigned short advancement;


    // Clear out the area that just played, so it can be recycled....
    advancement = sound_buffer_advancement;
    sound_buffer_advancement = 0;
    write = sound_buffer_read-advancement;
    value = sound_buffer_read;
    while(write != value)
    {
        sound_buffer[write] = 0;
        write++;
    }


    // Advance sound_buffer_write to be one click ahead of sound_buffer_read
    sound_buffer_write = sound_buffer_read+SDL_BUFFER_SIZE;


    // Go through each channel...
    repeat(channel, MAX_CHANNEL)
    {
        // Looking for active ones...
        left_volume = channel_volume[channel][LEFT];
        right_volume = channel_volume[channel][RIGHT];
        if(channel_active[channel])
        {
            // Pull the delay forward to adjust for the amount of sound we played...
            channel_start_delay[channel]-=advancement;
            if(channel_start_delay[channel] < 0)  channel_start_delay[channel] = 0;


            // Write as much of the channel as possible, based on its start_delay
            if(channel_start_delay[channel] < (SOUND_BUFFER_SIZE-SDL_BUFFER_SIZE))
            {
                // Gradual fade for looped sounds...
                if(channel_infinite_loop[channel])
                {
                    value = channel_new_volume[channel][LEFT] - left_volume;
                    if(value > 0)
                    {
                        if(value > INFI_LOOP_FADE)
                        {
                            left_volume += INFI_LOOP_FADE;
                        }
                        else
                        {
                            left_volume += value;
                        }
                    }
                    else
                    {
                        if(value < -INFI_LOOP_FADE)
                        {
                            left_volume -= INFI_LOOP_FADE;
                        }
                        else
                        {
                            left_volume += value;
                        }
                    }
                    value = channel_new_volume[channel][RIGHT] - right_volume;
                    if(value > 0)
                    {
                        if(value > INFI_LOOP_FADE)
                        {
                            right_volume += INFI_LOOP_FADE;
                        }
                        else
                        {
                            right_volume += value;
                        }
                    }
                    else
                    {
                        if(value < -INFI_LOOP_FADE)
                        {
                            right_volume -= INFI_LOOP_FADE;
                        }
                        else
                        {
                            right_volume += value;
                        }
                    }
                    channel_volume[channel][LEFT] = left_volume;
                    channel_volume[channel][RIGHT] = right_volume;
                    channel_new_volume[channel][LEFT] = 0;
                    channel_new_volume[channel][RIGHT] = 0;
                }
                if(left_volume > 0 || right_volume > 0)
                {
                    // Make stuff easier to type...
                    read_data = channel_data[channel];
                    position = channel_position[channel];
                    position_add = channel_position_add[channel];


                    // Figure out the index to write to...
                    write = sound_buffer_write+channel_start_delay[channel];



                    // And the length we need to read...
                    length = (SOUND_BUFFER_SIZE-SDL_BUFFER_SIZE) - channel_start_delay[channel];
                    length = length>>1;
                    if((channel_total_position_end[channel] - channel_total_position[channel]) < length)
                    {
                        length = (channel_total_position_end[channel] - channel_total_position[channel]);
                    }


                    // Keep writing blocks of sound until we've filled the sound_buffer...
                    position_end = channel_position_end[channel];
                    while(length > 0)
                    {
                        // Do we need to do a fade out, or can we write the data the easy way?
                        if(channel_total_position[channel] > channel_fade_start[channel])
                        {
                            // Do a fade out...
                            channel_total_position[channel]+=length;  // Same as incrementing each time...
                            while(length > 0)
                            {
                                // Subsample the RAW data for pitch changes...
                                value = read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                value = value>>2;


                                // Write the left sample...
                                sound_buffer[write] += ((value*left_volume)>>8);
                                write++;

                                // Write the right sample...
                                sound_buffer[write] += ((value*right_volume)>>8);
                                write++;


                                // Fade out the left and right sides, by decrementing the volume...  Do every so often...
                                channel_fade_count[channel][LEFT]--;
                                if(channel_fade_count[channel][LEFT] == 0)
                                {
                                    left_volume--;
                                    if(left_volume == 255) left_volume = 0;
                                    channel_fade_count[channel][LEFT] = channel_fade_step[channel][LEFT];
                                }
                                channel_fade_count[channel][RIGHT]--;
                                if(channel_fade_count[channel][RIGHT] == 0)
                                {
                                    right_volume--;
                                    if(right_volume == 255) right_volume = 0;
                                    channel_fade_count[channel][RIGHT] = channel_fade_step[channel][RIGHT];
                                }


                                length--;
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                            }
                            channel_total_position[channel]-=length;  // Same as incrementing each time...
                            channel_volume[channel][LEFT] = left_volume;
                            channel_volume[channel][RIGHT] = right_volume;
                        }
                        else
                        {
                            // Write the data...
                            length_save = length;
                            if(length > 64)
                            {
                                // Need to check every so often for fades...
                                length = 64;
                            }
                            channel_total_position[channel]+=length;  // Same as incrementing each time...
                            while(length > 0)
                            {
                                // Subsample the RAW data for pitch changes...
                                value = read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                                value += read_data[position>>10];
                                value = value>>2;

                                // Write the left sample...
                                sound_buffer[write] += ((value*left_volume)>>8);
                                write++;

                                // Write the right sample...
                                sound_buffer[write] += ((value*right_volume)>>8);
                                write++;

                                length--;
                                position+=position_add;  if(position >= position_end) position = channel_loop_start[channel];
                            }
                            channel_total_position[channel]-=length;  // Same as incrementing each time...
                            if(length_save > 64)
                            {
                                length = length_save - 64 + length;
                            }
                        }
                        if(channel_total_position[channel] >= channel_total_position_end[channel])
                        {
                            if(channel_infinite_loop[channel])
                            {
                                // Start the loop over...
                                channel_total_position[channel] = 0;
                            }
                            else
                            {
                                // Finished playing the sound
                                channel_active[channel] = FALSE;
                                length = 0;
                                unused_channel[unused_channel_count] = channel;
                                unused_channel_count++;
                            }
                        }
                    }


                    // Push the delay back, so it plays in correct position next time...
                    channel_start_delay[channel] = (SOUND_BUFFER_SIZE-SDL_BUFFER_SIZE);


                    // Save other information too...
                    channel_position[channel] = position;
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void fill_music(void)
{
    // <ZZ> This function is called periodically to play new music notes...
    signed int time;
    signed int note_time;
    unsigned char* filedata;
    unsigned char loop_data[6];
    unsigned short instrument;
    unsigned char volume;
    unsigned char pitch;
    unsigned char pan;



    if(music_mode)
    {
        // The music is on, so keep playing notes until we hit the time...
        time = music_time;
        filedata = music_file_note;
        filedata += (music_next_note<<3);


        // Get the time for the first note...
        note_time = (*filedata);  filedata++;  note_time = note_time<<8;
        note_time+= (*filedata);  filedata++;
        note_time = note_time * music_tempo;
        while(note_time < time && music_next_note != music_num_note)
        {
            // Read data other than time...
            loop_data[0] = *filedata;  filedata++;
            loop_data[1] = *filedata;  filedata++;
            instrument = *filedata;  filedata++;
            volume = *filedata;  filedata++;
            pitch = *filedata;  filedata++;  // !!!BAD!!! need pitch table...
            pan = *filedata;  filedata++;
            instrument = instrument << 4;
            loop_data[2] = *(music_file_instrument+(instrument)+12);
            loop_data[3] = *(music_file_instrument+(instrument)+13);
            loop_data[4] = *(music_file_instrument+(instrument)+14);
            loop_data[5] = *(music_file_instrument+(instrument)+15);
            instrument = instrument >> 4;


            // Play the note...
            play_sound(8192 + note_time - time, music_instrument_table[instrument], (unsigned char) ((volume*master_music_volume)>>8), pan, pitch_table[pitch&(MAX_PITCH-1)], loop_data);


            // Read the next note's time...
            music_next_note++;
            if(music_next_note != music_num_note)
            {
                note_time = (*filedata);  filedata++;  note_time = note_time<<8;
                note_time+= (*filedata);  filedata++;
                note_time = note_time * music_tempo;
            }
        }
        if(music_next_note == music_num_note)
        {
            if(music_mode == MUSIC_ONCE)
            {
                music_mode = MUSIC_STOP;
            }
            else
            {
                music_next_note = music_first_note;
                note_time = time - note_time;
                time = loop_data[0];  time = time<<8;  time+= loop_data[1];
                music_time = music_intro_time + note_time - (time * music_tempo);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
void play_music(unsigned char* filedata, unsigned short start_time, unsigned char mode)
{
    // <ZZ> This function starts a new song playing...
    int note;
    int instrument;
    unsigned short time;


    music_file_instrument = NULL;
    music_file_note = NULL;
    music_mode = MUSIC_STOP;
    if(filedata && mode && master_music_volume > 2)
    {
        // Actually gonna try playing something, so figure out how many instruments there are...
        music_mode = mode;
        sscanf(filedata, "%d", &music_num_instrument);
        filedata+=16;
        music_file_instrument = filedata;



        // Read the tempo...
        music_tempo = 100;
        if(music_num_instrument > 0)
        {
            music_tempo = *(filedata+9);
        }
        if(music_tempo > 0)
        {
            music_tempo = 137813 / music_tempo;
        }
        else
        {
            music_tempo = 1378;  // 22khz / 16per sec
        }


        // Build the instrument table...
        repeat(instrument, music_num_instrument)
        {
            music_instrument_table[instrument] = sdf_find_filetype(filedata, SDF_FILE_IS_RAW);
            if(music_instrument_table[instrument])
            {
                music_instrument_table[instrument] = (unsigned char*) sdf_read_unsigned_int(music_instrument_table[instrument]);
            }
            else
            {
                log_message("ERROR:  Instrument file %s.RAW couldn't be found", filedata);
                music_instrument_table[instrument] = NULL;
            }
            filedata+=16;
        }
        while(instrument < MAX_INSTRUMENT)
        {
            music_instrument_table[instrument] = NULL;
            instrument++;
        }


        // Finish up...
        sscanf(filedata, "%d", &music_num_note);
        filedata+=16;
        music_file_note = filedata;
        music_time = 0;


        // Assume we'll play the whole song...
        music_first_note = 0;
        music_next_note = 0;
        start_time = start_time << 4;
        music_intro_time = start_time * music_tempo;


        // But check to make sure...
        if(start_time != 0)
        {
            // Walk through the notes until we find the start and end locations...
            note = 0;
            time = 0;
            while(note < music_num_note && time < start_time)
            {
                time = (*filedata);  time = time<<8;  time+=(*(filedata+1));
                filedata+=8;
                note++;
            }
            music_first_note = note-1;
            if(mode == MUSIC_ONCE)
            {
                music_next_note = music_first_note;
                music_time = music_intro_time;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
