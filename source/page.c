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

// <ZZ> This file contains functions related to pages of books...
#define PAGE_COL (25+2)                     // The number of columns in the page animation
#define PAGE_FRAME 32                       // The number of frames in the page animation
float page_xy[PAGE_FRAME][PAGE_COL*2];      // The page animation


//-----------------------------------------------------------------------------------------------
void page_setup(void)
{
    // <ZZ> This function loads the page data from a datafile...
    int j;
    int i;
    unsigned char* page_anim;



    // Try to find the datafile...
    log_message("INFO:   Loading the page animation");
    page_anim = sdf_find_filetype("PAGEANIM", SDF_FILE_IS_DAT);
    if(page_anim)
    {
        // Read in the page data...
        page_anim = (unsigned char*) sdf_read_unsigned_int(page_anim);
        repeat(j, PAGE_FRAME)
        {
            repeat(i, PAGE_COL)
            {
                // Data is stored as signed chars for portability...
                page_xy[j][(i<<1)] = *((signed char*) page_anim)/127.0f;  page_anim++;
                page_xy[j][(i<<1)+1] = *((signed char*) page_anim)/127.0f;  page_anim++;
            }
        }
        log_message("INFO:   Page animation loaded");
    }
    else
    {
        // Default to a simple book...
        repeat(j, PAGE_FRAME)
        {
            repeat(i, PAGE_COL)
            {
                page_xy[j][(i<<1)+1] = (float) (-cos((i)*3.1415f*1.5f/PAGE_COL)*.25f);
                page_xy[j][(i<<1)] = (float) (sqrt(1.0f - (page_xy[0][(i<<1)+1]*page_xy[0][(i<<1)+1])));
            }
        }
        j = PAGE_FRAME-1;
        repeat(i, PAGE_COL)
        {
            page_xy[j][(i<<1)] = -page_xy[j][(i<<1)];
        }
        log_message("INFO:   Page animation regenerated");
    }
}

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void page_edit_tool(void)
{
    // <ZZ> This function lets me edit the page turn animation...  Shouldn't be preset in final
    //      release...
    static int frame = 0;
    static int point = 0;
    int i;
    int j;
    int last_frame;
    int next_frame;
    float x;
    float y;
    float distance;
    unsigned char* page_anim;




    // Draw a test book...
    // Left page
    display_book_page(NULL, 200.0, 67.0, page_xy[PAGE_FRAME-1], 7.0f, PAGE_COL-2, 15, TRUE, 0);

    // Right page
    display_book_page(NULL, 200.0, 67.0, page_xy[0], 7.0f, PAGE_COL-2, 15, FALSE, 0);

    // Current page
    display_book_page(NULL, 200.0, 67.0, page_xy[frame], 7.0f, PAGE_COL-2, 15, FALSE, 0);
    display_book_page(NULL, 200.0, 67.0, page_xy[frame], 7.0f, PAGE_COL-2, 15, TRUE, 0);



    // Draw the guideline points...
    display_pick_texture(texture_ascii);
    repeat(i, PAGE_FRAME)
    {
        if(i == frame)
        {
            display_color(green);
        }
        else
        {
            display_color(blue);
        }
        x = 200.0f - (float) (sin((i*3.1415929f/PAGE_FRAME)-(1.57079645-0.0490873890625))*185.0f);
        y = 270.0f - (float) (cos((i*3.1415929f/PAGE_FRAME)-(1.57079645-0.0490873890625))*75.0f);
        display_font('%', x-5.0f, y-5.0f, 10.0f);
    }




    // Draw the points...
    display_color(white);
    x = 200.0f;
    y = 257.0f;
    display_font('%', x-5.0f, y-5.0f, 10.0f);
    repeat(i, PAGE_COL)
    {
        if(i == point)
        {
            display_color(red);
        }
        else
        {
            display_color(white);
        }
        x+=(page_xy[frame][i<<1]*7.0f);
        y+=(page_xy[frame][(i<<1)+1]*7.0f);
        display_font('%', x-5.0f, y-5.0f, 10.0f);
    }



    // Adjust point positions...
    if(mouse_down[2])
    {
        j = frame;
//        while(j < PAGE_FRAME-1)
//        {
            x = 200.0f;
            y = 240.0f;
            repeat(i, point+1)
            {
                x+=(page_xy[j][i<<1]*7.0f);
                y+=(page_xy[j][(i<<1)+1]*7.0f);
            }
            x-=5.0f;
            y-=5.0f;
            page_xy[j][(point<<1)] += (mouse_x-x)/7.0f;
            page_xy[j][(point<<1)+1] += (mouse_y-y)/7.0f;
            x = page_xy[j][(point<<1)];
            y = page_xy[j][(point<<1)+1];
            distance = (float) sqrt(x*x + y*y);
            if(distance < 0.001)  { distance = 1;  x = 1; }
            x = x/distance;
            y = y/distance;
            page_xy[j][(point<<1)] = x;
            page_xy[j][(point<<1)+1] = y;
//            j++;
//        }
    }



    // Frame and point changes...
    if(key_pressed[SDL_SCANCODE_Q]) { frame=(frame-1)&31;}
    if(key_pressed[SDL_SCANCODE_W]) { frame=(frame+1)&31;}
    if(key_pressed[SDL_SCANCODE_E]) { point=(point-1);  if(point < 0) { point = PAGE_COL-1; } }
    if(key_pressed[SDL_SCANCODE_R]) { point=(point+1);  if(point > PAGE_COL-1) { point = 0; } }
    if(key_down[SDL_SCANCODE_T]) { frame=(frame-1)&31;}
    if(key_down[SDL_SCANCODE_Y]) { frame=(frame+1)&31;}
    if(key_pressed[SDL_SCANCODE_P])
    {
        // Save the page anim...
        // Try to find the datafile...
        page_anim = sdf_find_filetype("PAGEANIM", SDF_FILE_IS_DAT);
        if(page_anim)
        {
            // Read in the page data...
            page_anim = (unsigned char*) sdf_read_unsigned_int(page_anim);
            repeat(j, PAGE_FRAME)
            {
                repeat(i, PAGE_COL)
                {
                    // Data is stored as signed chars for portability...
                    *((signed char*) page_anim) = (signed char) (page_xy[j][(i<<1)]*127.0f);  page_anim++;
                    *((signed char*) page_anim) = (signed char) (page_xy[j][(i<<1)+1]*127.0f);  page_anim++;
                }
            }
        }
        sdf_export_file("PAGEANIM.DAT", NULL);
    }


    if(key_pressed[SDL_SCANCODE_O])
    {
        // Smooth frames...
        repeat(j, PAGE_FRAME)
        {
            last_frame = j-1;  if(last_frame < 0) last_frame = 0;
            next_frame = j+1;  if(next_frame > PAGE_FRAME-1) next_frame = PAGE_FRAME-1;
            repeat(i, PAGE_COL)
            {
                page_xy[j][(i<<1)]   = ((6.0f*page_xy[j][(i<<1)]) + page_xy[next_frame][(i<<1)] + page_xy[last_frame][(i<<1)]) / 8.0f;
                page_xy[j][(i<<1)+1] = ((6.0f*page_xy[j][(i<<1)+1]) + page_xy[next_frame][(i<<1)+1] + page_xy[last_frame][(i<<1)+1]) / 8.0f;
            }
        }
    }

}
#endif

//-----------------------------------------------------------------------------------------------
