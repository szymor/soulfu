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

// <ZZ> This file contains various tools to help make the game...
//      tool_tracker        - A music making tool...
//      tool_kanjiedit      - A font making tool...

#define KANJI_ADD_VERTEX    0
#define KANJI_ADD_TRIANGLE  1
#define KANJI_MOV_VERTEX    2
#define KANJI_DEL_VERTEX    3


#define PITCH_DIVISIONS 25
signed char tracker_adding_note;
unsigned short tracker_adding_note_time;
unsigned short tracker_adding_note_duration;
unsigned char tracker_adding_note_pitch;
signed char tracker_stall;
unsigned short kanji_copy_from = 0;
//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
int tool_tracker(float tlx, float tly, float brx, float bry, unsigned char* mus_data, int position, int snap, int instrument, int volume, unsigned char pan)
{
    // <ZZ> This function handles the tracker interface, and draws most of the tracker window.
    float x, y, divx, divy, timx;
    int i, notes, start_note, time, duration;
    signed char keepgoing;
    unsigned char* start_data;
    unsigned char note_instrument;
    unsigned char note_pitch;
    unsigned char j;
    unsigned char* note_string_location;
    int steal_value;
    int blockx, blocky;
    char note_string[32];
    signed char added_okay;


    display_texture_off();
    divy = PITCH_DIVISIONS;
    divy = (bry-tly)/divy;
    steal_value = -1;


    // Draw the notes...  But first skip over the header...
    sscanf(mus_data, "%d", &i);  // Number of instruments
    mus_data+=16+(i<<4);
    sscanf(mus_data, "%d", &notes);  // Number of notes...
    note_string_location = mus_data;
    mus_data+=16;


    // Now find the starting note for our position...
    i = 0;
    position = position<<4;
    keepgoing = TRUE;
    while(i < notes && keepgoing)
    {
        time = sdf_read_unsigned_short(mus_data);
        duration = sdf_read_unsigned_short(mus_data+2);
        if(time+duration > position)
        {
            // Found the first note
            keepgoing = FALSE;
        }
        else
        {
            // Not the note we're lookin' for...
            mus_data+=8;
            i++;
        }
    }
    start_note = i;
    start_data = mus_data;



    // Now draw the notes belonging to unselected instruments
    timx = (brx-tlx)/64;
    repeat(j, 2)
    {
        i = start_note;
        mus_data = start_data;
        keepgoing = TRUE;
        while(i < notes && keepgoing)
        {
            time = sdf_read_unsigned_short(mus_data);
            if(time > (position+64))
            {
                // Stop if we don't need to draw any more...  Notes should be in order of their play times...
                keepgoing = FALSE;
            }
            else
            {
                duration = sdf_read_unsigned_short(mus_data+2);
                if(time < position)
                {
                    // Clip the left side...
                    duration-=(position-time);
                    time+=(position-time);
                }
                if(time+duration > (position+64))
                {
                    // Clip the right side...
                    duration = position+64-time;
                }
                if(duration > 0)
                {
                    // Draw the note
                    note_instrument = *(mus_data+4);
                    if((note_instrument == instrument && j == 1) || (note_instrument != instrument && j == 0))
                    {
                        note_pitch = *(mus_data+6);
                        if(note_pitch < PITCH_DIVISIONS)
                        {
                            if(j == 0)
                            {
                                display_color_alpha(instrument_color[note_instrument & 7]);
                            }
                            else
                            {
                                display_color(instrument_color[note_instrument & 7]);
                            }
                            x = tlx+((time-position)*timx);
                            y = tly+(note_pitch*divy);
                            display_start_fan();
                                display_vertex_xy(x, y);
                                display_vertex_xy(x+(timx*duration), y);
                                display_vertex_xy(x+(timx*duration), y+divy);
                                display_vertex_xy(x, y+divy);
                            display_end();
                        }
                    }
                }
                mus_data+=8;
                i++;
            }
        }
    }





    // The border...
    display_color(gold);
    display_start_line();
        display_vertex_xy(tlx, tly);
        display_vertex_xy(brx, tly);
    display_end();
    display_start_line();
        display_vertex_xy(tlx, bry);
        display_vertex_xy(brx, bry);
    display_end();
    display_start_line();
        display_vertex_xy(tlx, tly);
        display_vertex_xy(tlx, bry);
    display_end();
    display_start_line();
        display_vertex_xy(brx, tly);
        display_vertex_xy(brx, bry);
    display_end();


    // The second divisions...
    display_start_line();
        x = tlx + ((brx-tlx)*.25f);
        display_vertex_xy(x, tly);
        display_vertex_xy(x, bry);
    display_end();
    display_start_line();
        x = tlx + ((brx-tlx)*.50f);
        display_vertex_xy(x, tly);
        display_vertex_xy(x, bry);
    display_end();
    display_start_line();
        x = tlx + ((brx-tlx)*.75f);
        display_vertex_xy(x, tly);
        display_vertex_xy(x, bry);
    display_end();


    // The second subdivisions...
    display_color_alpha(gold);
    divx = (float) snap;
    divx = (brx-tlx)/(divx*4.0f);
    x = tlx;
    repeat(i, (snap<<2))
    {
        if((i&15) != 0)
        {
            display_start_line();
                display_vertex_xy(x, tly);
                display_vertex_xy(x, bry);
            display_end();
        }
        x+=divx;
    }


    // The pitch subdivisions...
    display_color_alpha(gold);
    y = tly+divy;
    repeat(i, (PITCH_DIVISIONS-1))
    {
        if(i == (PITCH_DIVISIONS/2)-1)
        {
            display_color(gold);
        }
        display_start_line();
            display_vertex_xy(tlx, y);
            display_vertex_xy(brx, y);
        display_end();
        if(i == (PITCH_DIVISIONS/2))
        {
            display_color_alpha(gold);
        }
        y+=divy;
    }



    // Check for mouse over...  Add notes and remove notes...
    tracker_stall = FALSE;
    if(mouse_x > tlx && mouse_y > tly)
    {
        if(mouse_x < brx && mouse_y < bry)
        {
            mouse_current_object = current_object_data;
            mouse_current_item = current_object_item;
            if(mouse_last_object==current_object_data && mouse_last_item == current_object_item)
            {
                blockx = (int) ((mouse_x-tlx)/divx);
                blocky = (int) ((mouse_y-tly)/divy);
                if(tracker_adding_note)
                {
                    // Draw a long box...
                    duration = (blockx*16/snap)+position;
                    time = tracker_adding_note_time;
                    if(duration < time)
                    {
                        // Switch 'em around if dragging to the left...
                        time = duration;
                        duration = tracker_adding_note_time;
                    }
                    duration+=16/snap;
                    duration-=time;

                    // Clip left...
                    if(time < position)
                    {
                        duration-=(position-time);
                        time=position;
                    }
                    // Clip right...
                    if(duration+time > position+64)
                    {
                        duration = position+64-time;
                    }
                    // Draw it already...
                    x = tlx+((time-position)*timx);
                    y = tly+(tracker_adding_note_pitch*divy);
                    display_start_fan();
                        display_vertex_xy(x, y);
                        display_vertex_xy(x+(duration*timx), y);
                        display_vertex_xy(x+(duration*timx), y+divy);
                        display_vertex_xy(x, y+divy);
                    display_end();
                }
                else
                {
                    // Draw a normal box...
                    x = tlx+(blockx*divx);
                    y = tly+(blocky*divy);
                    display_color_alpha(gold);
                    display_start_fan();
                        display_vertex_xy(x, y);
                        display_vertex_xy(x+divx, y);
                        display_vertex_xy(x+divx, y+divy);
                        display_vertex_xy(x, y+divy);
                    display_end();
                }



                if(tracker_stall == FALSE)
                {
                    if(mouse_pressed[0] && tracker_adding_note == FALSE)
                    {
                        // Start to add a note
                        tracker_adding_note = TRUE;
                        tracker_adding_note_time = (blockx*16/snap)+position;
                        tracker_adding_note_pitch = blocky;
                    }
                    if(mouse_unpressed[0] && tracker_adding_note == TRUE)
                    {
                        // Finish adding a note
                        tracker_adding_note = FALSE;


                        // Figure out the duration...
                        tracker_adding_note_duration = (blockx*16/snap)+position;
                        if(tracker_adding_note_duration < tracker_adding_note_time)
                        {
                            // Switch 'em around if dragging to the left...
                            i = tracker_adding_note_duration;
                            tracker_adding_note_duration = tracker_adding_note_time;
                            tracker_adding_note_time = i;
                        }
                        tracker_adding_note_duration+=16/snap;
                        tracker_adding_note_duration-=tracker_adding_note_time;


                        // Create the note data string...
                        sdf_write_unsigned_short(note_string, tracker_adding_note_time);
                        sdf_write_unsigned_short(note_string+2, tracker_adding_note_duration);
                        *(note_string+4) = (unsigned char) instrument;
                        *(note_string+5) = (unsigned char) volume;
                        *(note_string+6) = (unsigned char) tracker_adding_note_pitch;
                        *(note_string+7) = (unsigned char) pan;


                        // Figure out where to put the string...
                        mus_data = note_string_location+16;
                        added_okay = FALSE;
                        repeat(i, notes)
                        {
                            time = sdf_read_unsigned_short(mus_data);
                            if(time > tracker_adding_note_time)
                            {
                                // Insert the note...
                                added_okay = sdf_insert_data(mus_data, note_string, 8);
                                i = notes+200;
                            }
                            mus_data+=8;
                        }
                        if(i == notes && added_okay == FALSE)
                        {
                            // Got to end of file without adding, so try appending to end...
                            added_okay = sdf_insert_data(mus_data, note_string, 8);
                        }



                        // Then write the note header...
                        if(added_okay)
                        {
                            notes++;
                            sprintf(note_string, "%d NOTES          ", notes);
                            memcpy(note_string_location, note_string, 16);
                        }


                        // Stop the music
                        play_music(NULL, 0, MUSIC_STOP);


                        // Don't allow any further operations...
                        tracker_stall = TRUE;
                    }
                    if(mouse_pressed[1] || mouse_pressed[2])
                    {
                        // Remove a note...  Search for the note we're removing...
                        repeat(j, 2)
                        {
                            i = start_note;
                            mus_data = start_data;
                            keepgoing = TRUE;
                            while(i < notes && keepgoing)
                            {
                                time = sdf_read_unsigned_short(mus_data);
                                if(time > (position+64))
                                {
                                    // Stop if we don't need to look any more...  Notes should be in order of their play times...
                                    keepgoing = FALSE;
                                }
                                else
                                {
                                    duration = sdf_read_unsigned_short(mus_data+2);
                                    if(time < position)
                                    {
                                        // Clip the left side...
                                        duration-=(position-time);
                                        time+=(position-time);
                                    }
                                    if(time+duration > (position+64))
                                    {
                                        // Clip the right side...
                                        duration = position+64-time;
                                    }
                                    if(duration > 0)
                                    {
                                        // Check the note
                                        note_instrument = *(mus_data+4);
                                        if((note_instrument == instrument && j == 0) || (note_instrument != instrument && j == 1))
                                        {
                                            note_pitch = *(mus_data+6);
                                            if(note_pitch < PITCH_DIVISIONS)
                                            {
                                                x = tlx+((time-position)*timx);
                                                y = tly+(note_pitch*divy);

                                                // Check if mouse is inside the drawn image...
                                                if(mouse_x > x && mouse_y > y)
                                                {
                                                    if(mouse_x < x+(timx*duration) && mouse_y < y+divy)
                                                    {
                                                        // Looks like this is the note to kill...
                                                        if(mouse_pressed[2])
                                                        {
                                                            // Oops...  Just stealing the pan, volume, etc....
                                                            steal_value = *(mus_data+5);  steal_value = steal_value<<8;  // Volume
                                                            steal_value+= *(mus_data+7);  steal_value = steal_value<<8;  // Pan
                                                            steal_value+= note_instrument;                               // Instrument

                                                            // Don't allow any further operations...
                                                            tracker_stall = TRUE;
                                                            keepgoing = FALSE;
                                                            j = 3;
                                                        }
                                                        else
                                                        {
                                                            // Now get rid of it...
                                                            notes--;
                                                            sprintf(note_string, "%d NOTES          ", notes);
                                                            memcpy(note_string_location, note_string, 16);
                                                            sdf_insert_data(mus_data, NULL, -8);


                                                            // Stop the music
                                                            play_music(NULL, 0, MUSIC_STOP);


                                                            // Don't allow any further operations...
                                                            tracker_stall = TRUE;
                                                            keepgoing = FALSE;
                                                            j = 3;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                    mus_data+=8;
                                    i++;
                                }
                            }
                        }
                    }
                }




            }
        }
    }


    display_texture_on();
    return steal_value;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
unsigned short kanji_select[3] = {NO_ITEM, NO_ITEM, NO_ITEM};
unsigned short num_kanji_select = 0;
unsigned short kanji_last_mode = 55;
void tool_kanjiedit(unsigned short kanji, float x, float y, float scale, unsigned short edit_mode)
{
    // <ZZ> This function draws the specified kanji character at x,y on the screen.  Screen size
    //      is assumed to be 400x300.  Also allows editing...
    unsigned short num_kanji;
    unsigned char* data;
    unsigned int offset;
    unsigned short num_vertex;
    unsigned short num_triangle;
    unsigned char* vertex_data;
    unsigned char* triangle_data;
    unsigned short vertex;
    float vx, vy;
    unsigned short i;
    signed short bytes_added;
    unsigned char new_data[4];
    unsigned short distance;
    signed short distance_x;
    signed short distance_y;
    unsigned short best_distance;
    unsigned short best_vertex;


    scale = scale*0.00390625f;


    // Do we have the kanji file?
    if(kanji_data)
    {
        // Make sure the desired character is valid...
        num_kanji = *kanji_data;  num_kanji<<=8;  num_kanji+= *(kanji_data+1);
        if(kanji < num_kanji)
        {
            display_texture_off();
            data = kanji_data+2+(kanji<<2);
            offset = sdf_read_unsigned_int(data);  data+=4;
            data = kanji_data+offset;
            num_vertex = *data;  data++;
            vertex_data = data;
            data+=num_vertex<<1;
            num_triangle = *data;  data++;
            triangle_data = data;


            // Draw the kanji character
            display_color(white);
            repeat(i, num_triangle)
            {
                display_start_fan();
                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale);  vertex++;  vy = y+(vertex_data[vertex]*scale);
                    display_vertex_xy(vx, vy);

                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale);  vertex++;  vy = y+(vertex_data[vertex]*scale);
                    display_vertex_xy(vx, vy);

                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale);  vertex++;  vy = y+(vertex_data[vertex]*scale);
                    display_vertex_xy(vx, vy);
                display_end();
            }


            // Unselect vertices
            if(edit_mode != kanji_last_mode)
            {
                num_kanji_select = 0;
                kanji_select[0] = NO_ITEM;
                kanji_select[1] = NO_ITEM;
                kanji_select[2] = NO_ITEM;
            }
            kanji_last_mode = edit_mode;


            // Draw its vertices
            repeat(i, num_vertex)
            {
                vertex=i<<1;
                vx = x+(vertex_data[vertex]*scale);  vertex++;  vy = y+(vertex_data[vertex]*scale);
                if(i == kanji_select[0] || i == kanji_select[1] || i == kanji_select[2])
                {
                    display_2d_marker(red, vx, vy, script_window_scale*0.25f);
                }
                else
                {
                    display_2d_marker(blue, vx, vy, script_window_scale*0.25f);
                }
            }


            // Draw the guidelines
            display_color(red);
            display_start_line();
                vx = x + (16*scale);  vy = y + (16*scale);
                display_vertex_xy(vx, vy);
                vy = y + (240*scale);
                display_vertex_xy(vx, vy);
            display_end();
            display_start_line();
                vx = x + (240*scale);  vy = y + (16*scale);
                display_vertex_xy(vx, vy);
                vy = y + (240*scale);
                display_vertex_xy(vx, vy);
            display_end();
            display_start_line();
                vx = x + (16*scale);  vy = y + (16*scale);
                display_vertex_xy(vx, vy);
                vx = x + (240*scale);
                display_vertex_xy(vx, vy);
            display_end();
            display_start_line();
                vx = x + (16*scale);  vy = y + (240*scale);
                display_vertex_xy(vx, vy);
                vx = x + (240*scale);
                display_vertex_xy(vx, vy);
            display_end();



            // Handle clicks
            if(edit_mode == KANJI_MOV_VERTEX && mouse_down[0] == 0)
            {
                kanji_select[0] = NO_ITEM;
                num_kanji_select = 0;
            }
            bytes_added = 0;
            if(mouse_x > x && mouse_y > y)
            {
                if(mouse_x < x+(255*scale) && mouse_y < y+(255*scale))
                {
                    mouse_current_object = current_object_data;
                    if(mouse_last_object == current_object_data)
                    {
                        vx = (mouse_x - x)/scale;
                        vy = (mouse_y - y)/scale;
                        if(edit_mode == KANJI_MOV_VERTEX && num_kanji_select == 1 && kanji_select[0] < num_vertex)
                        {
                            vertex = kanji_select[0];  vertex<<=1;
                            vertex_data[vertex] = (unsigned char) vx;  vertex++;
                            vertex_data[vertex] = (unsigned char) vy;
                        }
                        if(mouse_pressed[0])
                        {
                            if(edit_mode == KANJI_ADD_VERTEX)
                            {
                                if(num_vertex < 255)
                                {
                                    new_data[X] = (unsigned char) vx;
                                    new_data[Y] = (unsigned char) vy;
                                    if(sdf_insert_data(triangle_data-1, new_data, 2))
                                    {
                                        num_vertex++;
                                        *(vertex_data-1) = (unsigned char) num_vertex;
                                        bytes_added = 2;
                                    }
                                }
                            }
                            else
                            {
                                // Find the nearest vertex to vx, vy
                                best_vertex = NO_ITEM;
                                best_distance = 10;
                                repeat(i, num_vertex)
                                {
                                    vertex=i<<1;
                                    distance_x =  (signed short) (vertex_data[vertex]-((unsigned char) vx));  vertex++;
                                    distance_y =  (signed short) (vertex_data[vertex]-((unsigned char) vy));
                                    ABS(distance_x);
                                    ABS(distance_y);
                                    distance = distance_x+distance_y;
                                    if(distance < best_distance)
                                    {
                                        best_vertex = i;
                                        best_distance = distance;
                                    }
                                }
                                if(best_vertex < num_vertex)
                                {
                                    // Found the closest vertex...
                                    vertex=best_vertex<<1;
                                    vx = x+(vertex_data[vertex]*scale);  vertex++;  vy = y+(vertex_data[vertex]*scale);
                                    display_2d_marker(red, vx, vy, script_window_scale*0.25f);

                                    if(edit_mode == KANJI_MOV_VERTEX)
                                    {
                                        kanji_select[0] = best_vertex;
                                        num_kanji_select = 1;
                                    }
                                    else if(edit_mode == KANJI_ADD_TRIANGLE)
                                    {
                                        if(num_kanji_select < 3)
                                        {
                                            kanji_select[num_kanji_select] = best_vertex;
                                            num_kanji_select++;
                                        }
                                        if(num_kanji_select == 3)
                                        {
                                            if(num_triangle < 255)
                                            {
                                                // Add a triangle...
                                                if(kanji_select[0] < num_vertex && kanji_select[1] < num_vertex  && kanji_select[2] < num_vertex)
                                                {
                                                    new_data[0] = (unsigned char) kanji_select[0];
                                                    new_data[1] = (unsigned char) kanji_select[1];
                                                    new_data[2] = (unsigned char) kanji_select[2];
                                                    if(sdf_insert_data(triangle_data, new_data, 3))
                                                    {
                                                        num_triangle++;
                                                        *(triangle_data-1) = (unsigned char) num_triangle;
                                                        bytes_added = 3;
                                                    }
                                                }
                                            }
                                            num_kanji_select = 0;
                                            kanji_select[0] = NO_ITEM;
                                            kanji_select[1] = NO_ITEM;
                                            kanji_select[2] = NO_ITEM;
                                        }
                                    }
                                    else if(edit_mode == KANJI_DEL_VERTEX)
                                    {
                                        if(sdf_insert_data(vertex_data+(best_vertex<<1), NULL, -2))
                                        {
                                            num_vertex--;
                                            *(vertex_data-1) = (unsigned char) num_vertex;
                                            bytes_added = -2;


                                            // Delete any shared triangles...  Correct vertex indices too...
                                            triangle_data-=2;
                                            data = triangle_data;
                                            repeat(i, num_triangle)
                                            {
                                                if(*(data) == best_vertex || *(data+1) == best_vertex || *(data+2) == best_vertex)
                                                {
                                                    // Delete shared triangle...
                                                    i--;
                                                    num_triangle--;
                                                    sdf_insert_data(data, NULL, -3);
                                                    bytes_added-=3;
                                                }
                                                else
                                                {
                                                    // Renumber higher vertices...
                                                    if(*(data) > best_vertex)    (*(data))--;
                                                    if(*(data+1) > best_vertex)  (*(data+1))--;
                                                    if(*(data+2) > best_vertex)  (*(data+2))--;
                                                    data+=3;
                                                }
                                            }
                                            *(triangle_data-1) = (unsigned char) num_triangle;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(bytes_added != 0)
            {
                // Change later offsets...
                kanji++;
                while(kanji < num_kanji)
                {
                    data = kanji_data+2+(kanji<<2);
                    offset = sdf_read_unsigned_int(data);
                    offset+=bytes_added;
                    sdf_write_unsigned_int(data, offset);
                    kanji++;
                }
            }



            display_texture_on();
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------

