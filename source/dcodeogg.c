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

// <ZZ> This file contains functions to convert OGG files to RAW
//  **  decode_ogg            - The main function to do an OGG conversion


ogg_int16_t convbuffer[4096];
int convsize=4096;

unsigned char* bling_sound=NULL;
unsigned char* buzzer_sound=NULL;
unsigned char* cash_sound=NULL;

//-----------------------------------------------------------------------------------------------
signed char decode_ogg(unsigned char* index, unsigned char* filename)
{
    // <ZZ> This function decompresses an ogg file that has been stored in memory.  Index is a
    //      pointer to the start of the file's index in sdf_index, and can be gotten from
    //      sdf_find_index.  If the function works okay, it should create a new RAW file in the
    //      index and return TRUE.  It might also delete the original compressed file to save
    //      space, but that's a compile time option.  If it fails it should return FALSE, or
    //      it might decide to crash.
    unsigned char* data;       // Compressed
    unsigned int size;         // Compressed
    ogg_int16_t* tempbuffer;   // The mainbuffer
    unsigned int bufferspace;  // Number of bytes written to the mainbuffer
    unsigned char* newdata; // Decompressed
    unsigned int newsize;   // Decompressed
    float rate;


    // OGG Garbage
    ogg_sync_state   oy;
    ogg_stream_state os;
    ogg_page         og;
    ogg_packet       op;
    vorbis_info      vi;
    vorbis_comment   vc;
    vorbis_dsp_state vd;
    vorbis_block     vb;
    char *buffer;
    int  bytes;
    int eos;
    int i;
    int result;
    float **pcm;
    int samples;
    int j;
    int clipflag;
    int bout;
    ogg_int16_t* ptr;
    float* mono;
    int val;



    // Log what we're doing
    #ifdef VERBOSE_COMPILE
        log_message("INFO:     Decoding %s.OGG to %s.RAW", filename, filename);
    #endif


    // Find the location of the file data, and its size...
    data = (unsigned char*) sdf_read_unsigned_int(index);
    size = sdf_read_unsigned_int(index+4) & 0x00FFFFFF;


    // Make sure we have room in the index for a new file
    #ifdef KEEP_COMPRESSED_FILES
        if(sdf_extra_files <= 0)
        {
            log_message("ERROR:  No room left to add file, program must be restarted");
            return FALSE;
        }
    #endif


    // Decompress headers
    bufferspace = 0;
    tempbuffer = (ogg_int16_t*) mainbuffer;
    ogg_sync_init(&oy);  
    while(1)
    {
        eos = 0;
        buffer=ogg_sync_buffer(&oy,4096);
        // Replacement for stdin...
        // Replacement for stdin...
        // Replacement for stdin...
            // bytes=fread(buffer,1,4096,stdin);
            bytes = 4096;
            if(size < 4096) bytes = size;
            memcpy(buffer, data, bytes);
            size -= bytes;
            data += bytes;
        // Replacement for stdin...
        // Replacement for stdin...
        // Replacement for stdin...
        ogg_sync_wrote(&oy,bytes);
        if(ogg_sync_pageout(&oy,&og)!=1)
        {
            if(bytes<4096)break;
            log_message("ERROR:  File wasn't really an OGG file");
            return FALSE;
        }
  
        ogg_stream_init(&os,ogg_page_serialno(&og));
        vorbis_info_init(&vi);
        vorbis_comment_init(&vc);

        if(ogg_stream_pagein(&os,&og)<0)
        {
            log_message("ERROR:  OGG data corrupt");
            return FALSE;
        }
    
        if(ogg_stream_packetout(&os,&op)!=1)
        {
            log_message("ERROR:  OGG data corrupt");
            return FALSE;
        }
    
        if(vorbis_synthesis_headerin(&vi,&vc,&op)<0)
        { 
            log_message("ERROR:  No audio data in OGG file");
            return FALSE;
        }
    

    
        i=0;
        while(i<2)
        {
            while(i<2)
            {
            	result=ogg_sync_pageout(&oy,&og);
            	if(result==0)break;
            	if(result==1)
                {
            	    ogg_stream_pagein(&os,&og);
                    while(i<2)
                    {
                        result=ogg_stream_packetout(&os,&op);
                        if(result==0)break;
                        if(result<0)
                        {
                            log_message("ERROR:  OGG data is corrupt");
                            return FALSE;
	                    }
                	    vorbis_synthesis_headerin(&vi,&vc,&op);
                	    i++;
                    }
                }
            }
            buffer=ogg_sync_buffer(&oy,4096);


            // Replacement for stdin...
            // Replacement for stdin...
            // Replacement for stdin...
                // bytes=fread(buffer,1,4096,stdin);
                bytes = 4096;
                if(size < 4096) bytes = size;
                memcpy(buffer, data, bytes);
                size -= bytes;
                data += bytes;
            // Replacement for stdin...
            // Replacement for stdin...
            // Replacement for stdin...


            if(bytes==0 && i<2)
            {
                log_message("ERROR:  OGG file was missing data");
                exit(1);
            }
            ogg_sync_wrote(&oy,bytes);
        }


        // Start to decode actual data here...
        convsize=4096/vi.channels;
        vorbis_synthesis_init(&vd,&vi);
        vorbis_block_init(&vd,&vb);
        while(!eos)
        {
            while(!eos)
            {
                result=ogg_sync_pageout(&oy,&og);
                if(result==0)break;
                if(result<0)
                {
                    log_message("ERROR:  OGG data was corrupt");
                }
                else
                {
                    ogg_stream_pagein(&os,&og);
                    while(1)
                    {
                        result=ogg_stream_packetout(&os,&op);
                        if(result==0)break;
                        if(result<0)
                        {
                        }
                        else
                        {
                            if(vorbis_synthesis(&vb,&op)==0)  vorbis_synthesis_blockin(&vd,&vb);
                            while((samples=vorbis_synthesis_pcmout(&vd,&pcm))>0)
                            {
                                clipflag=0;
                                bout=(samples<convsize?samples:convsize);
		
                                for(i=0;i<vi.channels;i++)
                                {
                                    ptr=convbuffer+i;
                                    mono=pcm[i];
                                    for(j=0;j<bout;j++)
                                    {
                                        val=(int) (mono[j]*32767.f);
                                        if(val>32767) val=32767;
                                        if(val<-32768) val=-32768;
                                        *ptr=val;
                                        ptr+=vi.channels;
                                    }
                                }
                                // Replacement for stdout...
                                // Replacement for stdout...
                                // Replacement for stdout...
                                    // Copy the decoded data into the mainbuffer (44khz -> 22khz)
                                    rate = (float) 1.0;
                                    if(vi.rate > 0) rate = ((float) 22050)/vi.rate;
                                    j = (int) (bout*vi.channels*rate);
                                    repeat(i, j)
                                    {
                                        // Take samples and stick 'em in the mainbuffer
                                        tempbuffer[bufferspace] = convbuffer[((int) (i/rate))];
                                        bufferspace++;
                                    }
                                // Replacement for stdout...
                                // Replacement for stdout...
                                // Replacement for stdout...
                                vorbis_synthesis_read(&vd,bout);
                            }	    
                        }
                    }
                    if(ogg_page_eos(&og))eos=1;
                }
            }
            if(!eos)
            {
                buffer=ogg_sync_buffer(&oy,4096);
                // Replacement for stdin...
                // Replacement for stdin...
                // Replacement for stdin...
                    // bytes=fread(buffer,1,4096,stdin);
                    bytes = 4096;
                    if(size < 4096) bytes = size;
                    memcpy(buffer, data, bytes);
                    size -= bytes;
                    data += bytes;
                // Replacement for stdin...
                // Replacement for stdin...
                // Replacement for stdin...
                ogg_sync_wrote(&oy,bytes);
                if(bytes==0)eos=1;
            }
        }
        ogg_stream_clear(&os);
        vorbis_block_clear(&vb);
        vorbis_dsp_clear(&vd);
        vorbis_comment_clear(&vc);
        vorbis_info_clear(&vi);
    }
    ogg_sync_clear(&oy);



    // Allocate memory for the new file...
    newsize = bufferspace<<1;
    newdata = malloc(newsize+4);
    if(newdata)
    {
        // Do we create a new index?
        #ifdef KEEP_COMPRESSED_FILES
            index = sdf_get_new_index();
        #endif


        // Write the index...
        sdf_write_unsigned_int(index, (unsigned int) newdata);
        sdf_write_unsigned_int(index+4, newsize+4);
        *(index+4) = SDF_FILE_IS_RAW;
        repeat(j, 8) { *(index+8+j) = 0; }
        memcpy(index+8, filename, strlen(filename));


        // Copy the data from the mainbuffer to the newly created file
        *((unsigned int*) newdata) = (newsize>>1);
        memcpy(newdata+4, tempbuffer, newsize);
    }
    else
    {
        log_message("ERROR:  Not enough memory to decompress");
        return FALSE;
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
