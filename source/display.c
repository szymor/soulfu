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

// <ZZ> This file has all the stuff for displaying the screen...  3D card does everything...
//      These are the more general purpose functions, wrappers, and 2D windowing stuff...
//      Render.c has the 3D model rendering stuff...
//  x   display_clear_zbuffer       - Clears the zbuffer
//  x   display_clear_buffers       - Clears the zbuffer and the display
//  x   display_swap                - Swaps the display onto the screen after we've rendered it
//  x   display_viewport            - Tells us where to render on the screen
//  x   display_zbuffer_on          - Turns on zbuffering
//  x   display_zbuffer_off         - Turns off zbuffering
//  x   display_cull_on             - Turns on backface culling
//  x   display_cull_off            - Turns off backface culling
//  x   display_shade_on            - Turns on smooth shading
//  x   display_shade_off           - Turns off smooth shading
//      display_blend_trans         - Turns on transparency style average blending
//      display_blend_light         - Turns on light style additive blending
//      display_blend_off           - Turns off blending
//      display_depth_scene         - Sets up depth buffering range for the main 3D stuff
//      display_depth_overlay       - Sets up depth buffering range for windows
//  x   display_texture_on          - Turns on texturing
//  x   display_texture_off         - Turns off texturing
//  x   display_pick_texture        - Picks the texture to use
//  x   display_start_strip         - Starts a triangle strip
//  x   display_start_fan           - Starts a triangle fan
//  x   display_color               - Sets the color for the next vertex
//      display_texpos              - Sets the texture position for the next vertex
//  x   display_vertex              - Adds a vertex
//  x   display_end                 - Ends a triangle strip or fan
//      display_slider              - Draws a horizontal or vertical slider bar...
//      display_highlight           - Draws a white rectangle for selection lists...
//      display_window
//      display_image               - Draws a 2D textured image
//      display_font
//      display_kanji
//      display_string
//      display_mini_list
//      display_input
//      display_emacs
//  x   blurry_alpha_helper         - Helper for display_load_texture
//  x   display_load_texture        - Loads an RGB file onto the graphics card as a texture
//  x   display_load_all_textures   - Loads all of the textures onto the card
//  x   display_setup               - Sets up all of the mandatory silly stuff for SDL/GL/whatever else
//      display_render              - The main function for rendering a scene
//      display_start_fade          - Starts to fade out the screen (circle effect)
//      display_fade                - Draws the fade effect
//      display_look_at             - Sets up the camera matrix...


//#define ZNEAR 1.25f                     // The near frustum plane
#define ZNEAR 0.625f                     // The near frustum plane
#define ALPHA_TOLERANCE 64              // Tolerance for color key transparency
#define ALPHA_BLUR 75                   // Amount to fade neighbors in '=' prefixed files
#define ALPHA_MIN  0                    // Minimum alpha allowed for neighbors

#define FADE_IN             -4          //
#define FADE_OUT            4           //
#define FADE_TYPE_NONE      0           // No fade
#define FADE_TYPE_CIRCLE    1           // Circle fade
#define FADE_TYPE_WARNING   2           // Warning flash
#define FADE_TYPE_FULL      3           // Fullscreen fade
#define CIRCLE_QUARTER 8                // Points in a quarter circle
#define CIRCLE_POINTS (CIRCLE_QUARTER*4)// Number of points in the circle fade effect
#define CIRCLE_TOTAL_POINTS (CIRCLE_POINTS + 5)  // 4 for corners, 1 redundant
#define WARNING_SIZE 20.0f              //
float circle_xyz[CIRCLE_TOTAL_POINTS][3];  // Points for the circle fade
float fade_x;                           // Center of the circle
float fade_y;                           // Center of the circle
unsigned char fade_type;                //
signed short fade_time = 0;             // 0 is normal, 255 is fully faded
signed char fade_direction;             // In or Out
unsigned char fade_color[4] = {255, 255, 255, 80};
unsigned char display_full_screen = TRUE;
signed short screen_shake_timer = 0;
float screen_shake_amount = 0.0f;
float screen_frustum_x;
float screen_frustum_y;

unsigned char line_mode = 0;            // Global control for cartoon lines
float initial_camera_matrix[16];        // A matrix to speed up/simplify 3D drawing routines
float rotate_camera_matrix[16];         // A matrix to speed up/simplify 3D drawing routines
float window_camera_matrix[16];         // A matrix to setup window drawing routines
float onscreen_matrix[16];              // A matrix for figurin' out onscreen point locations...
float modelview_matrix[16];             // A matrix for figurin' out onscreen point locations...
float rotate_enviro_matrix[6];          // Enviromap mini matrix...


unsigned short camera_rotation_xy[2] = {0,  MIN_CAMERA_Y};      // The current camera rotation...
int camera_rotation_add_xy[2] = {0, 0}; // Movement offsets for camera...
//unsigned short camera_to_rotation_xy[2] = {0,  MIN_CAMERA_Y};   // The desired camera rotation...
float camera_distance = 30.0f;          // The last distance of the camera from the players
float camera_to_distance = 30.0f;       // The desired distance of the camera from the players
float camera_xyz[3];                    // The camera location
float camera_fore_xyz[3];               // The forward vector of the camera
float camera_side_xyz[3];               // The side vector of the camera
float map_side_xy[2];                   // For doin' the view triangle on the automap...
float target_xyz[3];                    // The location of the camera's target point (look at)
unsigned char temp_texture_data[512*512*4];  // Temporary memory for loading textures onto the card


// Important textures...
unsigned int texture_ascii   = 0;
unsigned int texture_ascii_ptr = 0;  // Used for drawing joint number particles in modeler...
unsigned int texture_bookfnt = 0;
unsigned int texture_paper[2]= { 0, 0 };
unsigned int texture_winalt  = 0;
unsigned int texture_winside = 0;
unsigned int texture_wincorn = 0;
unsigned int texture_wintrim = 0;
unsigned int texture_winmini = 0;
unsigned int texture_winminy = 0;
unsigned int texture_winslid = 0;
unsigned int texture_wininput= 0;
unsigned int texture_petrify = 0;
unsigned int texture_armor   = 0;
unsigned int texture_armorst = 0;
unsigned int texture_automap_stair = 0;
unsigned int texture_automap_town  = 0;
unsigned int texture_automap_boss  = 0;
unsigned int texture_automap_virtue  = 0;

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
unsigned int texture_bad_mpsamp00 = 0;
unsigned int texture_bad_mpsamp01 = 0;
unsigned int texture_bad_mpfence = 0;
unsigned int texture_bad_mppave = 0;
unsigned int texture_bad_mpdecal = 0;
unsigned int texture_bad_mphsbt00 = 0;
unsigned int texture_bad_mphsmd00 = 0;
unsigned int texture_bad_mphstp00 = 0;
unsigned int texture_bad_mpflor00 = 0;
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



unsigned int max_texture_size = 256;



//-----------------------------------------------------------------------------------------------
// <ZZ> Some macros to do simple stuff...
#define ONE_OVER_255 0.003921568627450980392156862745098f
#define display_clear_zbuffer()         { glClear(GL_DEPTH_BUFFER_BIT); }
#define display_clear_buffers()         { glClearColor(color_temp[0]*ONE_OVER_255, color_temp[1]*ONE_OVER_255, color_temp[2]*ONE_OVER_255, 1.0);  if(volumetric_shadows_on) { glClearStencil(8);  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); } else { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); } }
#define display_swap()                  { SDL_GL_SwapBuffers(); }
#define display_viewport(x, y, w, h)    { glViewport(x, y, w, h); }
float global_depth_min = 0.066f;
#define display_zbuffer_on()            { glEnable(GL_DEPTH_TEST);  glDepthMask(TRUE);  glDepthFunc(GL_LEQUAL); }
#define display_zbuffer_shadow()        { glEnable(GL_DEPTH_TEST);  glDepthMask(FALSE); glDepthFunc(GL_GEQUAL); }
#define display_zbuffer_off()           { glDisable(GL_DEPTH_TEST); glDepthMask(FALSE); }
#define display_zbuffer_write_on()      { glDepthMask(TRUE); }
#define display_zbuffer_write_off()     { glDepthMask(FALSE); }
#define display_cull_on()               { glEnable(GL_CULL_FACE); glFrontFace(GL_CW); }
#define display_cull_off()              { glDisable(GL_CULL_FACE); }
#define display_cull_frontface()        { glEnable(GL_CULL_FACE); glFrontFace(GL_CCW); }
#define display_shade_on()              { glShadeModel(GL_SMOOTH); }
#define display_shade_off()             { glShadeModel(GL_FLAT); }
#define display_paperdoll_on()          { glEnable(GL_ALPHA_TEST); }
#define display_paperdoll_func(ALPHA)   { glAlphaFunc(GL_GREATER, ALPHA*0.00390625f); }
#define display_paperdoll_off()         { glDisable(GL_ALPHA_TEST); }
//#define display_blend_mult()            { glEnable(GL_BLEND); glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR); }
//#define display_blend_mult()            { glEnable(GL_BLEND); glBlendFunc(GL_DST_COLOR, GL_ZERO); }
//#define display_blend_supermult()       { glEnable(GL_BLEND); glBlendFunc(GL_DST_COLOR, GL_SRC_ALPHA); }
#define display_blend_trans()           { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); }
#define display_blend_double()          { glEnable(GL_BLEND); glBlendFunc(GL_DST_COLOR, GL_ONE); }
#define display_blend_light()           { glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE); }
#define display_blend_off()             { glDisable(GL_BLEND); }
#define display_depth_scene()           { glDepthRange(0.5005f, 1.0f); }
//#define display_depth_shadow()          { glDepthRange(0.5f, 0.9995f); }
//#define display_depth_backface()        { glDepthRange(0.5010f, 1.0f); }
#ifdef DEVTOOL
    #define display_depth_layer(LAYER)  { };
#else
    #define display_depth_layer(LAYER)  { glDepthRange(0.5006f - (LAYER*0.0002f), 1.0f); }
#endif
#define display_depth_overlay(DEPTH)    { glDepthRange(e, e+(0.5/((float)MAX_WINDOW))); }
#define display_texture_on()            { glEnable(GL_TEXTURE_2D); }
#define display_texture_off()           { glDisable(GL_TEXTURE_2D); }
#define display_pick_texture(X)         { glBindTexture(GL_TEXTURE_2D, X); }
#define display_start_strip()           { glBegin(GL_TRIANGLE_STRIP); }
#define display_start_fan()             { glBegin(GL_TRIANGLE_FAN); }
#define display_start_points()          { glBegin(GL_POINTS); }
#define display_color(PTR)              { glColor3ubv(PTR); }
#define display_texpos(PTR)             { glTexCoord2fv(PTR); } 
#define display_vertex(PTR)             { glVertex3fv(PTR); }
#define display_point(PTR)              { glVertex2fv(PTR); }
#define display_perspective_off()       { glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST); }
#define display_perspective_on()        { glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); }
#define display_end()                   { glEnd(); }
#define display_line_size(S)            { glLineWidth((float) S); }
#define display_point_size(S)           { glPointSize((float) S); }
float fog_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
#define display_fog_on()                { glEnable(GL_FOG); glFogf(GL_FOG_MODE, GL_LINEAR); glFogfv(GL_FOG_COLOR, fog_color); glFogf(GL_FOG_START, 1500.0f); glFogf(GL_FOG_END, MAX_TERRAIN_FOG_DISTANCE); }
#define display_fog_off()               { glDisable(GL_FOG); }
double clip_equation[4] = { 0.0, 0.0, 1.0, 0.0};
#define display_clip_on()               { glClipPlane(GL_CLIP_PLANE0, clip_equation); glEnable(GL_CLIP_PLANE0); }
#define display_clip_off()              { glDisable(GL_CLIP_PLANE0); }
#define display_flip_z()                { glScalef(1.0, 1.0, -1.0); }
#define display_draw_on()               { glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); }
#define display_draw_off()              { glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); }



#define display_stencilbuffer_on()      { glEnable(GL_STENCIL_TEST); glStencilMask(255); } // glEnable(GL_STENCIL);}
#define display_stencilbuffer_off()     { glDisable(GL_STENCIL_TEST); } // glDisable(GL_STENCIL); }
#define display_stencilbuffer_back()    { glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); glStencilFunc(GL_ALWAYS, 0, 255); }  // For drawing backfaces of volumetric shadows...  Increment stencil buffer...
#define display_stencilbuffer_front()   { glStencilOp(GL_KEEP, GL_KEEP, GL_DECR); glStencilFunc(GL_ALWAYS, 0, 255);  } // For drawing frontfaces of volumetric shadows...  Decrement stencil buffer...
#define display_stencilbuffer_shadow(BIAS)  { glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP); glStencilFunc(GL_NOTEQUAL,  BIAS, 255);  } // For drawing final shadow over entire screen...  Only passes if stencil buffer equals BIAS...



#define display_start_line()            { glBegin(GL_LINES); }
#define display_start_line_loop()       { glBegin(GL_LINE_LOOP); }
#define display_vertex_xy(X, Y)         { glVertex2f(X, Y); }
#define display_vertex_xyz(X, Y, Z)     { glVertex3f(X, Y, Z); }
#define display_texpos_xy(X, Y)         { glTexCoord2f(X, Y); }
#define display_color_alpha(PTR)        { glColor4ubv(PTR); }


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_blur_image(unsigned char* data, unsigned short x_size, unsigned short y_size)
{
    // <ZZ> This function blurs an RGB (3 byte) image...
    unsigned short x, y;
    unsigned char last_red, last_green, last_blue;
    unsigned char red, green, blue;

    // Blur horizontally...
    repeat(y, y_size)
    {
        last_red = *(data + (((y*x_size)+x_size-1)*3));
        last_green = *(data + (((y*x_size)+x_size-1)*3)+1);
        last_blue = *(data + (((y*x_size)+x_size-1)*3)+2);
        repeat(x, x_size)
        {
            red = *(data + (((y*x_size)+x)*3));
            green = *(data + (((y*x_size)+x)*3)+1);
            blue = *(data + (((y*x_size)+x)*3)+2);
            last_red = (last_red>>1) + (red>>1);
            last_green = (last_green>>1) + (green>>1);
            last_blue = (last_blue>>1) + (blue>>1);
            *(data + (((y*x_size)+x)*3))   = last_red;
            *(data + (((y*x_size)+x)*3)+1) = last_green;
            *(data + (((y*x_size)+x)*3)+2) = last_blue;
            last_red = red;
            last_green = green;
            last_blue = blue;
        }
    }



    // Blur vertically...
    repeat(x, x_size)
    {
        last_red = *(data + ((((y_size-1)*x_size)+x)*3));
        last_green = *(data + ((((y_size-1)*x_size)+x)*3)+1);
        last_blue = *(data + ((((y_size-1)*x_size)+x)*3)+2);
        repeat(y, y_size)
        {
            red = *(data + (((y*x_size)+x)*3));
            green = *(data + (((y*x_size)+x)*3)+1);
            blue = *(data + (((y*x_size)+x)*3)+2);
            last_red = (last_red>>1) + (red>>1);
            last_green = (last_green>>1) + (green>>1);
            last_blue = (last_blue>>1) + (blue>>1);
            *(data + (((y*x_size)+x)*3))   = last_red;
            *(data + (((y*x_size)+x)*3)+1) = last_green;
            *(data + (((y*x_size)+x)*3)+2) = last_blue;
            last_red = red;
            last_green = green;
            last_blue = blue;
        }
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_export_tga(unsigned char* filename, unsigned char* data_start, unsigned short x_size, unsigned short y_size, signed char bytes_per_pixel)
{
    // <ZZ> This function saves a TGA file...
    FILE* openfile;
    unsigned short x, y;
    unsigned char value;
    unsigned char* data;


    openfile = fopen(filename, "wb");
    if(openfile == NULL) return;


    // Write the TGA header...
    putc(0,openfile);
    putc(0,openfile);
    putc(2,openfile);
    putc(0,openfile); putc(0,openfile);
    putc(0,openfile); putc(0,openfile);
    putc(0,openfile);
    putc(0,openfile); putc(0,openfile);
    putc(0,openfile); putc(0,openfile);
    putc((x_size & 0x00FF),openfile);
    putc((x_size & 0xFF00)>>8,openfile);
    putc((y_size & 0x00FF),openfile);
    putc((y_size & 0xFF00)>>8,openfile);
    putc(24,openfile);
    putc(0,openfile);


    // Write the file
    if(bytes_per_pixel == 1)
    {
        // 8-bit monochrome data
        repeat(y, y_size)
        {
            data = data_start+((y_size-y-1)*x_size);
            repeat(x, x_size)
            {
                putc(*data,openfile);
                putc(*data,openfile);
                putc(*data,openfile);  data++;
            }
        }
    }
    if(bytes_per_pixel == 2)
    {
        // 16-bit monochrome data (system endian unsigned shorts)
        repeat(y, y_size)
        {
            data = data_start+((y_size-y-1)*x_size*2);
            repeat(x, x_size)
            {
                value = (*((unsigned short*) data))>>8;  data+=2;
                putc(value,openfile);
                putc(value,openfile);
                putc(value,openfile);
            }
        }
    }
    if(bytes_per_pixel == -2)
    {
        // 16-bit monochrome data (system endian signed shorts)
        repeat(y, y_size)
        {
            data = data_start+((y_size-y-1)*x_size*2);
            repeat(x, x_size)
            {
                value = ((*((signed short*) data))+32767)>>8;  data+=2;
                putc(value,openfile);
                putc(value,openfile);
                putc(value,openfile);
            }
        }
    }
    if(bytes_per_pixel == 3)
    {
        // 24-bit RGB data
        repeat(y, y_size)
        {
            data = data_start+((y_size-y-1)*x_size*3);
            repeat(x, x_size)
            {
                putc(data[2],openfile);  // Blue
                putc(data[1],openfile);  // Green
                putc(data[0],openfile);  // Red
                data+=3;
            }
        }
    }
    fclose(openfile);
}
#endif

//-----------------------------------------------------------------------------------------------
void display_get_onscreen_xyd(float* in_xyz, float* out_xyd)
{
    // <ZZ> Test for another way of getting onscreen point locations, rather than using
    //      glFeedback...

    out_xyd[X] = onscreen_matrix[0] * in_xyz[X] + onscreen_matrix[4] * in_xyz[Y] + onscreen_matrix[8]  * in_xyz[Z] + onscreen_matrix[12];
    out_xyd[Y] = onscreen_matrix[1] * in_xyz[X] + onscreen_matrix[5] * in_xyz[Y] + onscreen_matrix[9]  * in_xyz[Z] + onscreen_matrix[13];
    out_xyd[2] = onscreen_matrix[3] * in_xyz[X] + onscreen_matrix[7] * in_xyz[Y] + onscreen_matrix[11] * in_xyz[Z] + onscreen_matrix[15];
    if(out_xyd[2] != 0.0f)
    {
         out_xyd[X]/=out_xyd[2]; out_xyd[Y]/=out_xyd[2];
         out_xyd[X]*=virtual_x/2;   out_xyd[X]+=virtual_x/2;
         out_xyd[Y]*=-virtual_y/2;  out_xyd[Y]+=virtual_y/2;
    }
}

//-----------------------------------------------------------------------------------------------
void display_slider(float x, float y, unsigned char w, unsigned char h, float scale, float position)
{
    // <ZZ> This function draws a slider bar at x,y.  W is how long it is (horizontal), or H is
    //      how tall it is (vertical).  Scale of 10.0f is normal.  Position is 0 to 1, and
    //      represents the position of the dinglethorpe
    float scale_w;
    float scale_h;
    float scale_x;
    float scale_y;
    float offset;



    // Error check...
    if(w == 0 || h == 0)  return;

    display_pick_texture(texture_winslid);
    if(h > w)
    {
        // Vertical bar
        offset = .28125f*scale;
        scale_h = scale*h;
        scale_w = .4375f*scale;
        scale_y = .35f*scale;
        display_color(white);
        display_start_fan();
            display_texpos_xy(.78125, 0);   display_vertex_xy(x+offset, y+scale_y);
            display_texpos_xy(1, 0);        display_vertex_xy(x+offset+scale_w, y+scale_y);
            display_texpos_xy(1, h);        display_vertex_xy(x+offset+scale_w, y+scale_h-scale_y);
            display_texpos_xy(.78125, h);   display_vertex_xy(x+offset, y+scale_h-scale_y);
        display_end();


        // Book ends...  Bottom
        offset = .09375f*scale;
        scale_x = .8125f*scale;
        scale_y = .5f*scale;
        display_start_fan();
            display_texpos_xy(.15625f, 0.75f);   display_vertex_xy(x+offset, y+scale_h-scale_y);
            display_texpos_xy(.5625f, 0.75f);    display_vertex_xy(x+offset+scale_x, y+scale_h-scale_y);
            display_texpos_xy(.5625f, 1);        display_vertex_xy(x+offset+scale_x, y+scale_h);
            display_texpos_xy(.15625f, 1);       display_vertex_xy(x+offset, y+scale_h);
        display_end();


        // Book ends...  Top
        display_start_fan();
            display_texpos_xy(.15625f, 1);       display_vertex_xy(x+offset, y);
            display_texpos_xy(.5625f, 1);        display_vertex_xy(x+offset+scale_x, y);
            display_texpos_xy(.5625f, 0.75f);    display_vertex_xy(x+offset+scale_x, y+scale_y);
            display_texpos_xy(.15625f, 0.75f);   display_vertex_xy(x+offset, y+scale_y);
        display_end();


        // Dinglethorpe center position...
        x = x+scale_y;
        y = y + (position*(scale_h-scale))+scale_y;
    }
    else
    {
        // Horizontal bar
        offset = .28125f*scale;
        scale_h = .4375f*scale;
        scale_w = scale*w;
        scale_x = .35f*scale;
        display_color(white);
        display_start_fan();
            display_texpos_xy(.78125, 0);   display_vertex_xy(x+scale_x, y+scale_h+offset);
            display_texpos_xy(1, 0);        display_vertex_xy(x+scale_x, y+offset);
            display_texpos_xy(1, w);        display_vertex_xy(x+scale_w-scale_x, y+offset);
            display_texpos_xy(.78125, w);   display_vertex_xy(x+scale_w-scale_x, y+scale_h+offset);
        display_end();


        // Book ends...  Right
        offset = .09375f*scale;
        scale_x = .5f*scale;
        scale_y = .8125f*scale;
        display_start_fan();
            display_texpos_xy(.15625f, 0.75f);       display_vertex_xy(x-scale_x+scale_w,y+offset+scale_y);
            display_texpos_xy(.5625f, 0.75f);        display_vertex_xy(x-scale_x+scale_w,y+offset);
            display_texpos_xy(.5625f, 1);            display_vertex_xy(x+scale_w,y+offset);
            display_texpos_xy(.15625f, 1);           display_vertex_xy(x+scale_w,y+offset+scale_y);
        display_end();


        // Book ends...  Left
        display_start_fan();
            display_texpos_xy(.15625f, 1);           display_vertex_xy(x,y+offset+scale_y);
            display_texpos_xy(.5625f, 1);            display_vertex_xy(x,y+offset);
            display_texpos_xy(.5625f, 0.75f);        display_vertex_xy(x+scale_x,y+offset);
            display_texpos_xy(.15625f, 0.75f);       display_vertex_xy(x+scale_x,y+offset+scale_y);
        display_end();



        // Dinglethorpe center position...
        y = y+scale_x;
        x = x + (position*(scale_w-scale))+scale_x;
    }




    // Draw the dinglethorpe...  Rotate as it moves
// !!!BAD!!!
// !!!BAD!!!  Do sin/cos look up
// !!!BAD!!!
    scale_x = ((float) cos(position*3.1415f*4))*scale;
    scale_y = ((float) sin(position*3.1415f*4))*scale;

    display_start_fan();
        display_texpos_xy(.03125f, .03125f);     display_vertex_xy(x-scale_x, y-scale_y);
        display_texpos_xy(.75f, .03125f);        display_vertex_xy(x+scale_y, y-scale_x);
        display_texpos_xy(.75f, .71875f);        display_vertex_xy(x+scale_x, y+scale_y);
        display_texpos_xy(.03125f, .71875f);     display_vertex_xy(x-scale_y, y+scale_x);
    display_end();


}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro draws a partially transparent rectangle from the given coordinates.
#define display_highlight(x, y, xr, yb)                                     \
{                                                                           \
    display_texture_off();                                                  \
    display_color_alpha(white);                                             \
    display_start_fan();                                                    \
        display_vertex_xy(x, y);                                            \
        display_vertex_xy(xr, y);                                           \
        display_vertex_xy(xr, yb);                                          \
        display_vertex_xy(x, yb);                                           \
    display_end();                                                          \
    display_texture_on();                                                   \
}

//-----------------------------------------------------------------------------------------------
void display_window(float x, float y, float w, float h, float scale, unsigned short side_mask)
{
    // <ZZ> This function draws a standard-style window.  The top left of the font area is given
    //      in x,y.  W is the number of fonts that can fit across.  H is the number of fonts that
    //      can fit downwards.  Scale of 10.0 is normal size.  Side_mask tells us which of the 4
    //      sides and 4 corners of the border to draw.  Also tells us which of the 4 fancy
    //      corner trims to draw, and also whether or not to draw the background.  Alpha blend
    //      should be turned on.  Shading should be off (flat).  Perspective and zbuffering
    //      should be off.  W and H should both be positive whole numbers...  Texturing should be
    //      on too.
    float scale_w;
    float scale_h;
    float half;


    // Do some calculations...
    scale_w = scale*w;
    scale_h = scale*h;
    half = scale*.5f;


    // First draw the background...
    if(side_mask & 240)
    {
        display_texture_off();
        display_color_alpha(brown);
        display_start_fan();
            display_vertex_xy(x-half, y-half);
            display_vertex_xy(x+scale_w+half, y-half);
            display_vertex_xy(x+scale_w+half, y+scale_h+half);
            display_vertex_xy(x-half, y+scale_h+half);
        display_end();
        display_texture_on();
    }
    display_color(white);


    // Now draw the sides and corners...
    if(side_mask & 61440)
    {
        display_pick_texture(texture_winside);
    }


    // Top side...
    if(side_mask & 32768)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x, y-scale);
            display_texpos_xy(w, 0);  display_vertex_xy(x+scale_w, y-scale);
            display_texpos_xy(w, 1);  display_vertex_xy(x+scale_w, y);
            display_texpos_xy(0, 1);  display_vertex_xy(x, y);
        display_end();
    }


    // Right side
    if(side_mask & 16384)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+scale_w+scale, y);
            display_texpos_xy(h, 0);  display_vertex_xy(x+scale_w+scale, y+scale_h);
            display_texpos_xy(h, 1);  display_vertex_xy(x+scale_w, y+scale_h);
            display_texpos_xy(0, 1);  display_vertex_xy(x+scale_w, y);
        display_end();
    }


    // Bottom side
    if(side_mask & 8192)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+scale_w, y+scale_h+scale);
            display_texpos_xy(w, 0);  display_vertex_xy(x, y+scale_h+scale);
            display_texpos_xy(w, 1);  display_vertex_xy(x, y+scale_h);
            display_texpos_xy(0, 1);  display_vertex_xy(x+scale_w, y+scale_h);
        display_end();
    }


    // Left side
    if(side_mask & 4096)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x-scale, y+scale_h);
            display_texpos_xy(h, 0);  display_vertex_xy(x-scale, y);
            display_texpos_xy(h, 1);  display_vertex_xy(x, y);
            display_texpos_xy(0, 1);  display_vertex_xy(x, y+scale_h);
        display_end();
    }


    // Now draw the corners...
    if(side_mask & 3840)
    {
        display_pick_texture(texture_wincorn);
    }


    // Top left corner
    if(side_mask & 2048)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x-scale, y-scale);
            display_texpos_xy(1, 0);  display_vertex_xy(x, y-scale);
            display_texpos_xy(1, 1);  display_vertex_xy(x, y);
            display_texpos_xy(1, 0);  display_vertex_xy(x-scale, y);
        display_end();
    }



    // Top right corner
    if(side_mask & 1024)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+scale_w+scale, y-scale);
            display_texpos_xy(1, 0);  display_vertex_xy(x+scale_w+scale, y);
            display_texpos_xy(1, 1);  display_vertex_xy(x+scale_w, y);
            display_texpos_xy(1, 0);  display_vertex_xy(x+scale_w, y-scale);
        display_end();
    }


    // Bottom right corner
    if(side_mask & 512)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+scale_w+scale, y+scale_h+scale);
            display_texpos_xy(1, 0);  display_vertex_xy(x+scale_w, y+scale_h+scale);
            display_texpos_xy(1, 1);  display_vertex_xy(x+scale_w, y+scale_h);
            display_texpos_xy(1, 0);  display_vertex_xy(x+scale_w+scale, y+scale_h);
        display_end();
    }


    // Bottom left corner
    if(side_mask & 256)
    {
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x-scale, y+scale_h+scale);
            display_texpos_xy(1, 0);  display_vertex_xy(x-scale, y+scale_h);
            display_texpos_xy(1, 1);  display_vertex_xy(x, y+scale_h);
            display_texpos_xy(1, 0);  display_vertex_xy(x, y+scale_h+scale);
        display_end();
    }



    // Now draw the trim... 
    if(side_mask & 15)
    {
        display_pick_texture(texture_wintrim);
        scale = scale * .115f;
    }


    // Top left corner
    if(side_mask & 8)
    {
        x-=scale;
        y-=scale;
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x-half,  y-half);
            display_texpos_xy(1, 0);  display_vertex_xy(x+half,  y-half);
            display_texpos_xy(1, 1);  display_vertex_xy(x+half,  y+half);
            display_texpos_xy(0, 1);  display_vertex_xy(x-half,  y+half);
        display_end();
        x+=scale;
        y+=scale;
    }


    // Top right corner
    if(side_mask & 4)
    {
        x+=scale;
        y-=scale;
        x+=scale_w;
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+half,  y-half);
            display_texpos_xy(1, 0);  display_vertex_xy(x+half,  y+half);
            display_texpos_xy(1, 1);  display_vertex_xy(x-half,  y+half);
            display_texpos_xy(0, 1);  display_vertex_xy(x-half,  y-half);
        display_end();
        x-=scale_w;
        x-=scale;
        y+=scale;
    }


    // Bottom right corner
    if(side_mask & 2)
    {
        x+=scale;
        y+=scale;
        y+=scale_h;
        x+=scale_w;
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x+half,  y+half);
            display_texpos_xy(1, 0);  display_vertex_xy(x-half,  y+half);
            display_texpos_xy(1, 1);  display_vertex_xy(x-half,  y-half);
            display_texpos_xy(0, 1);  display_vertex_xy(x+half,  y-half);
        display_end();
        x-=scale_w;
        y-=scale_h;
        x-=scale;
        y-=scale;
    }


    // Bottom left corner
    if(side_mask & 1)
    {
        x-=scale;
        y+=scale;
        y+=scale_h;
        display_start_fan();
            display_texpos_xy(0, 0);  display_vertex_xy(x-half,  y+half);
            display_texpos_xy(1, 0);  display_vertex_xy(x-half,  y-half);
            display_texpos_xy(1, 1);  display_vertex_xy(x+half,  y-half);
            display_texpos_xy(0, 1);  display_vertex_xy(x+half,  y+half);
        display_end();
        y-=scale_h;
        x+=scale;
        y-=scale;
    }
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro draws a 2D image...
#define display_image(tlx, tly, brx, bry, image_number)             \
{                                                                   \
    display_pick_texture(image_number);                             \
    display_start_fan();                                            \
      display_texpos_xy(0, 0);  display_vertex_xy(tlx, tly);        \
      display_texpos_xy(1, 0);  display_vertex_xy(brx, tly);        \
      display_texpos_xy(1, 1);  display_vertex_xy(brx, bry);        \
      display_texpos_xy(0, 1);  display_vertex_xy(tlx, bry);        \
    display_end();                                                  \
}


//-----------------------------------------------------------------------------------------------
void display_mouse_text_box(float tlx, float tly, float brx, float bry, unsigned int image_number)
{
    // <ZZ> Draws a little box for the mouse text with nice little edges...
    float xlx, xrx;

    xlx = tlx+(script_window_scale*0.5f);
    xrx = brx-(script_window_scale*0.5f);

    display_pick_texture(image_number);
    display_start_fan();
      display_texpos_xy(0.000f, 0.00f);  display_vertex_xy(tlx, tly);
      display_texpos_xy(0.025f, 0.00f);  display_vertex_xy(xlx, tly);
      display_texpos_xy(0.025f, 1.00f);  display_vertex_xy(xlx, bry);
      display_texpos_xy(0.000f, 1.00f);  display_vertex_xy(tlx, bry);
    display_end();
    display_start_fan();
      display_texpos_xy(0.025f, 0.00f);  display_vertex_xy(xlx, tly);
      display_texpos_xy(0.975f, 0.00f);  display_vertex_xy(xrx, tly);
      display_texpos_xy(0.975f, 1.00f);  display_vertex_xy(xrx, bry);
      display_texpos_xy(0.025f, 1.00f);  display_vertex_xy(xlx, bry);
    display_end();
    display_start_fan();
      display_texpos_xy(0.975f, 0.00f);  display_vertex_xy(xrx, tly);
      display_texpos_xy(1.000f, 0.00f);  display_vertex_xy(brx, tly);
      display_texpos_xy(1.000f, 1.00f);  display_vertex_xy(brx, bry);
      display_texpos_xy(0.975f, 1.00f);  display_vertex_xy(xrx, bry);
    display_end();
}


//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_marker(unsigned char* color, float x, float y, float z, float scale)
{
    display_color(color);
    display_start_line();
        display_vertex_xyz(x-scale, y, z);
        display_vertex_xyz(x+scale, y, z);

        display_vertex_xyz(x, y-scale, z);
        display_vertex_xyz(x, y+scale, z);

        display_vertex_xyz(x, y, z-scale);
        display_vertex_xyz(x, y, z+scale);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_crosshairs(unsigned char* color, float x, float y, float scale)
{
    display_color(color);
    display_start_line();
        display_vertex_xyz(-scale, y, 0.0f);
        display_vertex_xyz(scale, y, 0.0f);

        display_vertex_xyz(x, -scale, 0.0f);
        display_vertex_xyz(x, scale, 0.0f);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_riser_marker(unsigned char* color, float x, float y, float z, float scale)
{
    display_color(color);
    display_start_line();
        display_vertex_xyz(x-scale, y, z);
        display_vertex_xyz(x+scale, y, z);

        display_vertex_xyz(x, y-scale, z);
        display_vertex_xyz(x, y+scale, z);

        display_vertex_xyz(x, y, 0.0f);
        display_vertex_xyz(x, y, z);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_2d_marker(unsigned char* color, float x, float y, float scale)
{
    display_color(color);
    display_start_line();
        display_vertex_xyz(x-scale, y, 0);
        display_vertex_xyz(x+scale, y, 0);

        display_vertex_xyz(x, y-scale, 0);
        display_vertex_xyz(x, y+scale, 0);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void display_solid_marker(unsigned char* color, float x, float y, float z, float scale)
{
    display_color(color);  
    display_start_fan();
        display_vertex_xyz(x, y, z-scale);
        display_vertex_xyz(x, y-scale, z);
        display_vertex_xyz(x+scale, y, z);
        display_vertex_xyz(x, y+scale, z);
        display_vertex_xyz(x-scale, y, z);
        display_vertex_xyz(x, y-scale, z);
    display_end();

    display_start_fan();
        display_vertex_xyz(x, y, z+scale);
        display_vertex_xyz(x, y-scale, z);
        display_vertex_xyz(x-scale, y, z);
        display_vertex_xyz(x, y+scale, z);
        display_vertex_xyz(x+scale, y, z);
        display_vertex_xyz(x, y-scale, z);
    display_end();
}
#endif

//-----------------------------------------------------------------------------------------------
void display_font(unsigned char ascii, float x, float y, float scale)
{
    // <ZZ> This function draws the specified ascii character at x,y on the screen.  Screen size
    //      is assumed to be 400x300.  Note that the proper texture must be picked before calling
    //      this function, and the vertex color should be set to white (unless tinting is
    //      desired).  Perspective should also be turned off prior to calling, and zbuffering
    //      should be turned off too.  Alpha blending should be turned on.  Shading should be
    //      turned off (we want 'em flat shaded).  Scale of 10 is normal size.  Texturing should
    //      be on too.
    float tx, ty;

    tx = ((float) (ascii&15))*.0625f;
    ty = ((float) (ascii>>4))*.0625f;
    display_start_fan();
        display_texpos_xy(tx, ty);  display_vertex_xy(x, y);
        display_texpos_xy(tx+.0625f, ty);  display_vertex_xy(x+scale, y);
        display_texpos_xy(tx+.0625f, ty+.0625f);  display_vertex_xy(x+scale, y+scale);
        display_texpos_xy(tx, ty+.0625f);  display_vertex_xy(x, y+scale);
    display_end();

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Should probably inline this in display_string...
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!

}

//-----------------------------------------------------------------------------------------------
void display_kanji(unsigned short kanji, float x, float y, float scale_x, float scale_y)
{
    // <ZZ> This function draws the specified kanji character at x,y on the screen.  Screen size
    //      is assumed to be 400x300.
    unsigned short num_kanji;
    unsigned char* data;
    unsigned int offset;
    unsigned short num_vertex;
    unsigned short num_triangle;
    unsigned char* vertex_data;
    unsigned short vertex;
    float vx, vy;
    unsigned short i;


    scale_x *= 0.003921568627451f;
    scale_y *= 0.003921568627451f;


    // Do we have the kanji file?
    if(kanji_data)
    {
        // Make sure the desired character is valid...
        num_kanji = *kanji_data;  num_kanji<<=8;  num_kanji+= *(kanji_data+1);
        if(kanji < num_kanji)
        {
            display_texture_off();
            data = kanji_data+2+(kanji<<2);
            offset =*data;  offset<<=8;  data++;  offset+=*data;  offset<<=8;  data++;  offset+=*data;  offset<<=8;  data++;  offset+=*data;
            data = kanji_data+offset;
            num_vertex = *data;  data++;
            vertex_data = data;
            data+=num_vertex<<1;
            num_triangle = *data;  data++;


            repeat(i, num_triangle)
            {
                display_start_fan();
                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale_x);  vertex++;  vy = y+(vertex_data[vertex]*scale_y);
                    display_vertex_xy(vx, vy);

                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale_x);  vertex++;  vy = y+(vertex_data[vertex]*scale_y);
                    display_vertex_xy(vx, vy);

                    vertex = *data;  vertex<<=1;  data++;
                    vx = x+(vertex_data[vertex]*scale_x);  vertex++;  vy = y+(vertex_data[vertex]*scale_y);
                    display_vertex_xy(vx, vy);
                display_end();
            }


            display_texture_on();
        }
    }


// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Should probably inline this in display_string...
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!

}

//-----------------------------------------------------------------------------------------------
void display_book_font(unsigned char ascii, float x, float y, float xtr, float ytr, float scale)
{
    // <ZZ> This function works just like display_font(), except this one can skew the fonts...
    float tx, ty;

    tx = ((float) (ascii&15))/16.0f;
    ty = ((float) (ascii>>4))/16.0f;
    display_start_fan();
        display_texpos_xy(tx, ty);  display_vertex_xy(x, y);
        display_texpos_xy(tx+.0625f, ty);  display_vertex_xy(xtr, ytr);
        display_texpos_xy(tx+.0625f, ty+.0625f);  display_vertex_xy(xtr, ytr+scale);
        display_texpos_xy(tx, ty+.0625f);  display_vertex_xy(x, y+scale);
    display_end();

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Must inline this!!!  All of the data is setup already, just stick it in whenever
// !!!BAD!!!  everything is working perfect...
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!

}

//-----------------------------------------------------------------------------------------------
void display_book_page(unsigned char* page_start, float x_start, float y_start, float* offsets_xy, float scale, int num_columns, int num_rows, signed char draw_back_page, unsigned char page_number)
{
    // <ZZ> This function draws the text of a book...  And the text...
    //      Offsets_xy is an array that controls how the page curves...
    unsigned char* page;
    int current_row, current_column;
    float vertical_size;
    float x, y, x_right, y_right;
    float y_down, y_right_down;
    float tx, ty;


    // Figure out which texture to use for the paper...  Alternate every page
    page_number = page_number>>1;  // Paper pages, not printed sides...
    display_color(white);
    display_pick_texture(texture_paper[page_number&1]);


    // Draw the blank paper...
    x = x_start;
    y = y_start-scale;  // Space for page edge...
    vertical_size = (num_rows+2)*scale;
    y_down = y+vertical_size;
    num_columns+=2;  // 2 additional columns for gutter and edge...
    tx = 0;
    ty = 0.9765625f / num_columns;
    repeat(current_column, num_columns)
    {
        x_right = x+(offsets_xy[(current_column<<1)]*scale);
        y_right = y+(offsets_xy[(current_column<<1)+1]*scale);
        y_right_down = y_right + vertical_size;
        // Draw the paper...
        if(x_right > x)
        {
            if(draw_back_page == FALSE)
            {
                display_start_fan();
                    display_texpos_xy(tx, 0);  display_vertex_xy(x, y);
                    display_texpos_xy(tx+ty, 0);  display_vertex_xy(x_right, y_right);
                    display_texpos_xy(tx+ty, 1);  display_vertex_xy(x_right, y_right_down);
                    display_texpos_xy(tx, 1);  display_vertex_xy(x, y_down);
                display_end();
            }
        }
        else
        {
            if(draw_back_page)
            {
                display_start_fan();
                    display_texpos_xy(tx+ty, 0);  display_vertex_xy(x_right, y_right);
                    display_texpos_xy(tx, 0);  display_vertex_xy(x, y);
                    display_texpos_xy(tx, 1);  display_vertex_xy(x, y_down);
                    display_texpos_xy(tx+ty, 1);  display_vertex_xy(x_right, y_right_down);
                display_end();
            }
        }
        x = x_right;
        y = y_right;
        y_down = y_right_down;
        tx+=ty;
    }
    num_columns-=2;




    // Draw the text on the page...
    if(page_start != NULL)
    {
        display_pick_texture(texture_bookfnt);
        if(draw_back_page)
        {
            page_start = page_start + num_columns - 1;
        }
        x_start += (offsets_xy[0]*scale);  // Skip over the gutter...
        y_start += (offsets_xy[1]*scale);
        x = x_start;
        repeat(current_column, num_columns)
        {
            y = y_start;
            x_right = x+(offsets_xy[(current_column<<1)+2]*scale);
            y_right = y+(offsets_xy[(current_column<<1)+3]*scale);
            y_start = y_right;
            if(x_right > x)
            {
                if(draw_back_page == FALSE)
                {
                    // Normal page text...
                    page = page_start + current_column;
                    // Walk down the page vertically writing characters...
                    repeat(current_row, num_rows)
                    {
                        y_down = y+scale;
                        y_right_down = y_right+scale;
                        if(*page != ' ')
                        {
                            display_book_font(*page, x, y, x_right, y_right, scale); 
                        }
                        y=y_down;
                        y_right=y_right_down;
                        page+=num_columns;
                    }
                }
            }
            else
            {
                if(draw_back_page)
                {
                    // Text for next page...
                    page = page_start - current_column;
                    // Walk down the page vertically writing characters...
                    repeat(current_row, num_rows)
                    {
                        y_down = y+scale;
                        y_right_down = y_right+scale;
                        if(*page != ' ')
                        {
                            display_book_font(*page, x_right, y_right, x, y, scale); 
                        }
                        y=y_down;
                        y_right=y_right_down;
                        page+=num_columns;
                    }
                }
            }
            x=x_right;
        }
    }
}

//-----------------------------------------------------------------------------------------------
void display_string(unsigned char* ascii_string, float x, float y, float scale)
{
    // <ZZ> This function displays a string onscreen at the given location.  All of the notes for
    //      display_font apply here too.
    unsigned short i;

    i = 0;
    while(ascii_string[i] != 0)
    {
        if(ascii_string[i] != ' ')  display_font(ascii_string[i], x, y, scale);
        x += scale;
        i++;
    }
}

//-----------------------------------------------------------------------------------------------
void display_mini_list(char* options, float x, float y, unsigned char w, unsigned char h, float scale, unsigned short value)
{
    // <ZZ> This function draws a mini list input box.  Options is a string of different choices
    //      for the user to pick from.  X and y are the top left coordinates.  10.0 scale is
    //      normal.  W is the number of characters that can fit in a box (width).  Expansion
    //      ranges from 0 to 255, with 255 being fully expanded, 0 being unhighlighted, 128 being
    //      highlighted.  H is the number of options.
    float offset;
    float scale_w;
    float scale_h;
    float scale_t;
    float half;
    unsigned char current_choice;
    unsigned char expansion;
    unsigned char alpha_complex_uv[4];
    char *next_option;
    int i;


    // Decode the value argument...
    current_choice = value>>8;
    expansion = value&255;


    // Draw the top level choice box...
    display_pick_texture(texture_winmini);
    half = scale * .5f;
    alpha_complex_uv[0] = 255;
    alpha_complex_uv[1] = 255;
    alpha_complex_uv[2] = 255;
    if(expansion > 127)
    {
        // Solid blue box
        offset = 0.5f;
        alpha_complex_uv[3] = 255;
    }
    else
    {
        // Transparent grey box
        offset = 0.0f;
        alpha_complex_uv[3] = expansion+64;
    }
    scale_w = scale*w;
    display_color_alpha(alpha_complex_uv);
    display_start_fan();
        display_texpos_xy(0, 0+offset);       display_vertex_xy(x, y);
        display_texpos_xy(1, 0+offset);       display_vertex_xy(x+scale_w, y);
        display_texpos_xy(1, 0.53125f+offset);  display_vertex_xy(x+scale_w, y+scale);
        display_texpos_xy(0, 0.53125f+offset);  display_vertex_xy(x, y+scale);
    display_end();




    // Now draw the expanded list, if the mini list is clicked...
    if(expansion > 128)
    {
        alpha_complex_uv[0] = 128;
        alpha_complex_uv[1] = 128;
        alpha_complex_uv[2] = 128;
        alpha_complex_uv[3] = 230;
        display_color_alpha(alpha_complex_uv);
        scale_t = ((expansion-128) * h) * .0078125f;
        scale_h = scale_t * scale;

        // Top segment, with corners...
        display_start_fan();
            display_texpos_xy(0, 0.5f);       display_vertex_xy(x, y+scale);
            display_texpos_xy(1, 0.5f);       display_vertex_xy(x+scale_w, y+scale);
            display_texpos_xy(1, 0.765625f);  display_vertex_xy(x+scale_w, y+half+scale);
            display_texpos_xy(0, 0.765625f);  display_vertex_xy(x, y+half+scale);
        display_end();


        // Bottom segment, with corners...
        display_start_fan();
            display_texpos_xy(0, 0.765625f);  display_vertex_xy(x, y+scale+scale_h+half);
            display_texpos_xy(1, 0.765625f);  display_vertex_xy(x+scale_w, y+scale+scale_h+half);
            display_texpos_xy(1, 1.03125f);   display_vertex_xy(x+scale_w, y+half+scale+scale_h+half);
            display_texpos_xy(0, 1.03125f);   display_vertex_xy(x, y+half+scale+scale_h+half);
        display_end();


        // Mid segment...
        display_pick_texture(texture_winminy);
        display_start_fan();
            display_texpos_xy(0, 0.0);        display_vertex_xy(x, y+half+scale);
            display_texpos_xy(1, 0.0);        display_vertex_xy(x+scale_w, y+half+scale);
            display_texpos_xy(1, scale_t);    display_vertex_xy(x+scale_w, y+scale+scale_h+half);
            display_texpos_xy(0, scale_t);    display_vertex_xy(x, y+scale+scale_h+half);
        display_end();
    }



    // Now draw the text for the list
    display_pick_texture(texture_ascii);
    scale_h = scale;
    display_color(white);
    scale_w = (expansion-128) * .0078125f;
    scale_t = scale * scale_w;
    scale_w *= half;
    repeat(i, h)
    {
        next_option = strpbrk(options, ",");
        if(next_option) *next_option = 0;
        if(expansion > 128)
        {
            display_string(options, x+half, y+scale_h+scale_w, scale);
            scale_h+=scale_t;
        }
        if(i == current_choice)
        {
            display_string(options, x+half, y, scale);
        }
        if(next_option) *next_option = ',';
        options = next_option+1;
    }
}

//-----------------------------------------------------------------------------------------------
void display_input(char* current_text, float x, float y, signed char w, float scale, signed char current_position)
{
    // <ZZ> This function draws a text input box.  Current_text is the text that has been input
    //      so far.  X and y are the top left coordinates.  10.0 scale is normal.  W is the
    //      number of characters that can fit in a box (width).  Current_position is the spot where
    //      the next letter goes (-1 for not typing).
    float scale_w;


    // Draw the top level choice box...
    display_pick_texture(texture_wininput);
    display_color_alpha(whiter);
    scale_w = scale*w;
    display_start_fan();
        display_texpos_xy(0, 0);       display_vertex_xy(x, y);
        display_texpos_xy(1, 0);       display_vertex_xy(x+scale_w, y);
        display_texpos_xy(1, 1.0625f);  display_vertex_xy(x+scale_w, y+scale);
        display_texpos_xy(0, 1.0625f);  display_vertex_xy(x, y+scale);
    display_end();


    // Then draw the text string...
    x+=scale*.5f;
    display_pick_texture(texture_ascii);
    display_color(white);
    display_string(current_text, x, y, scale);


    // Then draw the cursor...
    if(current_position >= 0 && current_position < w)
    {
        if((main_video_frame_skipped>>3)&1)
        {
            display_string("_", x+(scale*current_position), y, scale);
        }
    }
}

//-----------------------------------------------------------------------------------------------
void display_emacs(float x, float y, int w, int h, unsigned char curx, unsigned char cury, unsigned char scrollx, unsigned short scrolly, unsigned char* text, float scale)
{
    // <ZZ> This function draws an emacs text editor box, with text and cursor.
    //      Must be in DEVTOOL mode...
    float xr;
    float yb;
    int read;
    int line;
    unsigned char letter;
    int length;

    // Draw the background
    xr = x+(scale*w);
    yb = y+(scale*h);
    display_texture_off();
    display_color_alpha(dark_green);
    display_start_fan();
        display_vertex_xy(x, y);
        display_vertex_xy(xr, y);
        display_vertex_xy(xr, yb);
        display_vertex_xy(x, yb);
    display_end();
    display_texture_on();


    // Skip down several rows depending on the scrolly setting...  Make sure we don't run out of data...
    line = 0;
    read = 0;
    while(line < scrolly && read < TEXT_SIZE)
    {
        if(text[read] == 0)
        {
            line++;
        }
        read++;
    }


    // Make sure file ends with a 0...
    text[TEXT_SIZE-1] = 0;


    // Print out each line until we're done...
    scale = scale*.66666666f;
    w += w>>1;
    h += h>>1;
    display_pick_texture(texture_ascii);
    display_color(light_green);
    text+=read;
    line = 0;
    yb = y;
    while(line < h && read < TEXT_SIZE)
    {
        length = strlen(text);
        if(length > scrollx)
        {
            text+=scrollx;
            length-=scrollx;
            if(length > w)
            {
                // Cropped string
                letter = text[w];
                text[w] = 0;
                display_string(text, x, yb, scale);
                text[w] = letter;
            }
            else
            {
                // Full string
                display_string(text, x, yb, scale);
            }
        }
        length++;
        read+=length;
        text+=length;
        yb+=scale;
        line++;
    }


    // Then draw the cursor...
    if(curx < w && cury < h)
    {
        if((main_video_frame_skipped>>3)&1)
        {
            display_string("_", x+(scale*curx), y+(scale*cury), scale);
        }
    }
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro helps out with display_load_texture...
#define blurry_alpha_helper(X, Y, AMOUNT)                                   \
{                                                                           \
    if((x+X) >= 0 && (y+Y) >= 0 && (x+X) < size_x && (y+Y) < size_y)        \
    {                                                                       \
        write = data + (((X) + (size_x*(Y)))<<2);                           \
        if((((int) (*write))-(AMOUNT)) > ALPHA_MIN)  (*write)-=(AMOUNT);    \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
// <ZZ> This macro helps out with display_load_texture...
#define bleed_edge_helper(X, Y)                                             \
{                                                                           \
    if((x+X) >= 0 && (y+Y) >= 0 && (x+X) < size_x && (y+Y) < size_y)        \
    {                                                                       \
        write = data + (((X) + (size_x*(Y)))<<2);                           \
        if(write[0] > data[0] && write[1] > data[1] && write[2] > data[2])  \
        {                                                                   \
            data[0] = write[0];                                             \
            data[1] = write[1];                                             \
            data[2] = write[2];                                             \
        }                                                                   \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
void display_unload_all_textures(void)
{
    // <ZZ> This function removes all texture data from the video card
    int i;
    unsigned char *index, *data;
    unsigned char  filetype;


    log_message("INFO:   Removing all textures from the graphics card...");
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        filetype = *(index+4);
        if((filetype & 15) == SDF_FILE_IS_RGB)
        {
            data = (unsigned char*) sdf_read_unsigned_int(index);
            if((*(data+1)) & TEXTURE_ON_CARD)  glDeleteTextures(1, (unsigned int*) (data+2));
        }
    }
}

//-----------------------------------------------------------------------------------------------
signed char display_load_texture(unsigned char* index)
{
    // <ZZ> This function loads an RGB file onto the graphics card as a texture.  Index is the
    //      SDF index for the file.  Returns TRUE (good) or FALSE (bad).
    char filename[9];
    unsigned char* data;
    unsigned char* write;
//    unsigned char hi_byte;
//    unsigned char lo_byte;
    unsigned char red;
    unsigned char green;
    unsigned char blue;
    unsigned char alpha;
    unsigned short level;
    short variance;
    signed char alpha_channel_used;
    unsigned char file_is_shadow;
    unsigned char file_is_light;
    unsigned char file_is_mip;
    int x;
    int y;
    int size_x;
    int size_y;
    int color;


    // Make sure the file is the correct type...
    if(*(index+4) != SDF_FILE_IS_RGB) return FALSE;


    // Make sure the file isn't prefixed with a '+' sign...  Means don't load onto card...
    if(*(index+8) == '+') return FALSE;


    // Make sure the file isn't prefixed with a 'HITE'...  Means it's a special heightmap thing that doesn't get loaded onto card...
    if((*(index+8) == 'H') && (*(index+9) == 'I') && (*(index+10) == 'T') && (*(index+11) == 'E')) return FALSE;


    // Log what we're doing...
    memcpy(filename, index+8, 8);
    filename[8] = 0;
    #ifdef VERBOSE_COMPILE
        log_message("INFO:     Loading %s.RGB as a texture", filename);
    #endif


    // Start reading the file...
    data = (unsigned char*) sdf_read_unsigned_int(index);
    size_x = sdf_read_unsigned_short(data+6);
    size_y = sdf_read_unsigned_short(data+8);
    data+=10;


    // Don't allow incorrect texture sizes...
    if(size_x != 8 && size_x != 16 && size_x != 32 && size_x != 64 && size_x != 128 && size_x != 256 && size_x != 512)
    {
        log_message("ERROR:  Texture size X was bad (8, 16, 32, 64, 128, 256, 512)...  %dx%d", size_x, size_y);
        return FALSE;
    }
    if(size_y != 8 && size_y != 16 && size_y != 32 && size_y != 64 && size_y != 128 && size_y != 256 && size_y != 512)
    {
        log_message("ERROR:  Texture size Y was bad (8, 16, 32, 64, 128, 256, 512)...  %dx%d", size_x, size_y);
        return FALSE;
    }


    // Start writing the data to a temporary location
    file_is_shadow = (filename[0] == 'S' && filename[1] == 'H' && filename[2] == 'A' && filename[3] == 'D');
    file_is_shadow |= (filename[0] == 'M' && filename[1] == 'P' && filename[2] == 'S' && filename[3] == 'H' && filename[4] == 'A' && filename[5] == 'D');
    file_is_light  = (filename[0] == 'L' && filename[1] == 'I' && filename[2] == 'T' && filename[3] == 'E');
    file_is_light |= (filename[0] == 'M' && filename[1] == 'P' && filename[2] == 'L' && filename[3] == 'I' && filename[4] == 'T' && filename[5] == 'E');
    file_is_mip = FALSE;
    if(mip_map_active)
    {
        // User allows mip mapping...
        file_is_mip  = (filename[0] == 'M' && filename[1] == 'P') || ((filename[0] == '=' || filename[0] == '-') && filename[1] == 'M' && filename[2] == 'P');
    }
    alpha_channel_used = (filename[0] == '-' || filename[0] == '=' || file_is_shadow || file_is_light);
    write = temp_texture_data;
    repeat(y, size_y)
    {
        repeat(x, size_x)
        {
            red = *data;  data++;
            green = *data;  data++;
            blue = *data;  data++;
            *write = red;  write++;
            *write = green;  write++;
            *write = blue;  write++;
            if(alpha_channel_used)
            {
                *(write) = 255;  write++;
            }
        }
    }


    // Go through and set alpha channel to 0 in places where it's color keyed...
    if(alpha_channel_used)
    {
        red = temp_texture_data[0];
        green = temp_texture_data[1];
        blue = temp_texture_data[2];
        data = temp_texture_data;

        // Turn off all texels that are close in value...
        repeat(y, size_y)
        {
            repeat(x, size_x)
            {
                // Copy in the RGB values...
                variance = 0;
                if((*data) > red) variance += (*data) - red;  else variance += red - (*data);  data++;
                if((*data) > green) variance += (*data) - green;  else variance += green - (*data);  data++;
                if((*data) > blue) variance += (*data) - blue;  else variance += blue - (*data);  data++;

                // Write the new Alpha value...
                if(variance < ALPHA_TOLERANCE || file_is_shadow || file_is_light)
                {
                    if(file_is_light)
                    {
                        // Set the alpha equal to the highest channel value......
                        (*data) = (*(data-1));
                        if((*(data-2)) > (*data))  { (*data) = (*(data-2)); }
                        if((*(data-3)) > (*data))  { (*data) = (*(data-3)); }

                        // Multiply the color, so the highest channel is 255...
                        if((*data) > 0)
                        {
                            (*(data-1)) = (*(data-1)) * 255 / (*data);
                            (*(data-2)) = (*(data-2)) * 255 / (*data);
                            (*(data-3)) = (*(data-3)) * 255 / (*data);
                        }
                    }
                    else if(file_is_shadow)
                    {
                        // Set the alpha depending on darkness of rgb...
                        color = (*(data-3)) + (*(data-2)) + (*(data-1));
                        color = color / 3;
                        color = 250 - color;
                        if(color < 0) color = 0;
                        (*data) = color;

                        // ...And set the color to black...
                        (*(data-3)) = 0;
                        (*(data-2)) = 0;
                        (*(data-1)) = 0;
                    }
                    else
                    {
                        // Set the color to black...
                        (*(data-3)) = 0;
                        (*(data-2)) = 0;
                        (*(data-1)) = 0;

                        // ...And the alpha to 0...
                        (*data) = 0;

                        // A super alpha file has blurry alpha along the edges...
                        if(filename[0] == '=')
                        {
                            blurry_alpha_helper(-1, -1, ALPHA_BLUR/2);
                            blurry_alpha_helper( 0, -1, ALPHA_BLUR);
                            blurry_alpha_helper( 1, -1, ALPHA_BLUR/2);

                            blurry_alpha_helper(-1,  0, ALPHA_BLUR);
                            blurry_alpha_helper( 1,  0, ALPHA_BLUR);

                            blurry_alpha_helper(-1,  1, ALPHA_BLUR/2);
                            blurry_alpha_helper( 0,  1, ALPHA_BLUR);
                            blurry_alpha_helper( 1,  1, ALPHA_BLUR/2);
                        }
                    }
                }
                data++;
            }
        }


        // Bleed edges on super alpha files...
        if(filename[0] == '=')
        {
            data = temp_texture_data;

            // Look for blocks that are alpha 0...
            repeat(y, size_y)
            {
                repeat(x, size_x)
                {
                    if(data[3] == 0)
                    {
                        // It's transparent...  Get the color from one of it's neighbors...
                        bleed_edge_helper(-1, 0);
                        bleed_edge_helper(1, 0);
                        bleed_edge_helper(0, -1);
                        bleed_edge_helper(0, 1);
                    }
                    data+=4;
                }
            }
        }
    }


    // Now load the texture onto the graphics card...  We do have a graphics card, right?
    data = (unsigned char*) sdf_read_unsigned_int(index);
    if((*(data+1)) & TEXTURE_ON_CARD)  glDeleteTextures(1, (unsigned int*) (data+2));
    switch(filename[0])
    {
        case '-':
            // The texture should be alpha blended to cut out color keys...
            sdf_write_unsigned_short(data, (unsigned short) TEXTURE_ALPHA);
            break;
        case '=':
            // The texture should be alpha blended to cut out color keys and to antialias...
            sdf_write_unsigned_short(data, (unsigned short) TEXTURE_SUPER_ALPHA);
            break;
        default:
            // The texture is a normal RGB texture...
            sdf_write_unsigned_short(data, (unsigned short) TEXTURE_NO_ALPHA);
            break;
    }
    glGenTextures(1, (unsigned int*) (data+2));
    glBindTexture(GL_TEXTURE_2D, *((unsigned int*) (data+2)));

/* changes for the skyboxing mode by MiR 2008 Block 1*/
    char skbx[6];
    int numsky;
    int numtex;
    if(strncmp(filename, "SKYBOX",6) == 0) {
        //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        sscanf(filename,"%6s%1d%1d",&skbx,&numsky,&numtex);
        //log_message("found skybox %d %d",numsky,numtex);
        if (numsky>=0&&numsky<10&&numtex>0&&numtex<7) {
            g_skyTexIDs[numsky][numtex-1]=*((unsigned int*) (data+2));
        }
    } else {
/* end of block 1 */

    // Fast and Ugly mode...
    if(fast_and_ugly_active)
    {
        file_is_mip = FALSE;
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        if(file_is_mip)
        {
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);  // Best, but looks noisy (graphics card issue?)
//            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST); // Works well, but has crease line...
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        }
    }
/* changes for the skyboxing mode by MiR 2008 Block 1*/
    } // to end the else case from above
/* End of BLock 2 , no further changes down this way */

// !!!BAD!!!
// !!!BAD!!!  Leave this code here, but don't use...  It's for the old tile based stuff...
// !!!BAD!!!
//    // Make mip mapped files not wrap...
//    if(file_is_mip)
//    {
//        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
//        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
//    }
//
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



    if(alpha_channel_used)
    {
        if(file_is_mip)
        {
            level=0;
            while(size_x >= 1 || size_y >= 1)
            {
                // Add the image to the texture...
                if(((unsigned int) size_x) <= max_texture_size && ((unsigned int) size_y) <= max_texture_size)
                {
                    glTexImage2D(GL_TEXTURE_2D, level, 4, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_texture_data);
//log_message("INFO:     Adding (alpha) mip level %d (%dx%d) to %s.RGB", level, size_x, size_y, filename);
                    level++;
                }
                size_x>>=1;
                size_y>>=1;


                // Fix for non-square images...
                if(size_x == 0 || size_y == 0)
                {
                    if(size_x < size_y)
                    {
                        size_x++;
                    }
                    else if(size_y < size_x)
                    {
                        size_y++;
                    }
                }


                // Generate the mip map image...
                repeat(y, size_y)
                {
                    repeat(x, size_x)
                    {
                        // Rescale the image by half in each direction...  4 samples...
                        write = temp_texture_data + (((y*size_x<<2)+(x<<1))*4);
                        red = write[0]>>2;
                        green = write[1]>>2;
                        blue = write[2]>>2;
                        alpha = write[3]>>2;
                        write+=4;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;
                        alpha += write[3]>>2;
                        write+=size_x*8;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;
                        alpha += write[3]>>2;
                        write-=4;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;
                        alpha += write[3]>>2;


                        // Write the final color...
                        write = temp_texture_data + (((y*size_x) + x)*4);
                        write[0] = red;
                        write[1] = green;
                        write[2] = blue;
                        write[3] = alpha;
                    }
                }
            }
        }
        else
        {
            // Non-mipmapped texture...
            glTexImage2D(GL_TEXTURE_2D, 0, 4, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, temp_texture_data);
        }
    }
    else
    {
        if(file_is_mip)
        {
            level=0;
            while(size_x >= 1 || size_y >= 1)
            {
                // Add the image to the texture...
                if(((unsigned int) size_x) <= max_texture_size && ((unsigned int) size_y) <= max_texture_size)
                {
                    glTexImage2D(GL_TEXTURE_2D, level, 3, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, temp_texture_data);
//log_message("INFO:     Adding mip level %d (%dx%d) to %s.RGB", level, size_x, size_y, filename);
// !!!BAD!!!
// !!!BAD!!! Mip map scaling test export...
// !!!BAD!!!
//if(strcmp(filename, "MPSAMP01") == 0)
//{
//    sprintf(DEBUG_STRING, "%s-%03dx%03d.TGA", filename, size_x, size_y);
//    display_export_tga(DEBUG_STRING, temp_texture_data, (unsigned short) size_x, (unsigned short) size_y, 3);
//    DEBUG_STRING[0] = 0;
//}
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                    level++;
                }
                size_x>>=1;
                size_y>>=1;


                // Fix for non-square images...
                if(size_x == 0 || size_y == 0)
                {
                    if(size_x < size_y)
                    {
                        size_x++;
                    }
                    else if(size_y < size_x)
                    {
                        size_y++;
                    }
                }


                // Generate the mip map image...
                repeat(y, size_y)
                {
                    repeat(x, size_x)
                    {
                        // Rescale the image by half in each direction...  4 samples...
                        write = temp_texture_data + (((y*size_x<<2)+(x<<1))*3);
                        red = write[0]>>2;
                        green = write[1]>>2;
                        blue = write[2]>>2;
                        write+=3;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;
                        write+=size_x*6;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;
                        write-=3;
                        red += write[0]>>2;
                        green += write[1]>>2;
                        blue += write[2]>>2;


                        // Write the final color...
                        write = temp_texture_data + (((y*size_x) + x)*3);
                        write[0] = red;
                        write[1] = green;
                        write[2] = blue;
                    }
                }
            }
        }
        else
        {
            // Non-mipmapped texture...
            glTexImage2D(GL_TEXTURE_2D, 0, 3, size_x, size_y, 0, GL_RGB, GL_UNSIGNED_BYTE, temp_texture_data);
        }
    }


    *(data+1) |= TEXTURE_ON_CARD;
    #ifdef VERBOSE_COMPILE
        log_message("INFO:     Loaded as texture number %d", *((unsigned int*) (data+2)));
    #endif



    // Remember important textures that I hardcoded for windows and stuff...
    if(strcmp(filename, "=ASCII") == 0)   texture_ascii    = *((unsigned int*) (data+2));
    if(strcmp(filename, "=ASCII") == 0)   texture_ascii_ptr= (unsigned int) data;
    if(strcmp(filename, "=BOOKFNT") == 0) texture_bookfnt  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=PAPER0") == 0)  texture_paper[0] = *((unsigned int*) (data+2));
    if(strcmp(filename, "=PAPER1") == 0)  texture_paper[1] = *((unsigned int*) (data+2));
    if(strcmp(filename, "WINALT") == 0)   texture_winalt   = *((unsigned int*) (data+2));
    if(strcmp(filename, "=WINSIDE") == 0) texture_winside  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=WINCORN") == 0) texture_wincorn  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=WINTRIM") == 0) texture_wintrim  = *((unsigned int*) (data+2));
    if(strcmp(filename, "WINMINI") == 0)  texture_winmini  = *((unsigned int*) (data+2));
    if(strcmp(filename, "WINMINY") == 0)  texture_winminy  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=WINSLID") == 0) texture_winslid  = *((unsigned int*) (data+2));
    if(strcmp(filename, "WININPUT") == 0) texture_wininput = *((unsigned int*) (data+2));
    if(strcmp(filename, "DWHITE") == 0)   texture_petrify  = *((unsigned int*) (data+2));

    if(strcmp(filename, "=ARMOR") == 0)   texture_armor    = *((unsigned int*) (data+2));
    if(strcmp(filename, "=ARMORST") == 0) texture_armorst  = *((unsigned int*) (data+2));

    if(strcmp(filename, "=AMSTAIR") == 0) texture_automap_stair = *((unsigned int*) (data+2));
    if(strcmp(filename, "=AMTOWN") == 0)  texture_automap_town  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=AMBOSS") == 0)  texture_automap_boss  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=AMVIRTU") == 0) texture_automap_virtue  = *((unsigned int*) (data+2));

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
    if(strcmp(filename, "MPSAMP00") == 0) texture_bad_mpsamp00 = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPSAMP01") == 0) texture_bad_mpsamp01 = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPFENCE") == 0)  texture_bad_mpfence  = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPPAVE") == 0)  texture_bad_mppave  = *((unsigned int*) (data+2));
    if(strcmp(filename, "=MPDECAL") == 0)  texture_bad_mpdecal  = *((unsigned int*) (data+2));



    if(strcmp(filename, "MPHSBT00") == 0) texture_bad_mphsbt00 = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPHSMD00") == 0) texture_bad_mphsmd00 = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPHSTP00") == 0) texture_bad_mphstp00 = *((unsigned int*) (data+2));
    if(strcmp(filename, "MPFLOR00") == 0) texture_bad_mpflor00 = *((unsigned int*) (data+2));
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!


	return TRUE;
}

//-----------------------------------------------------------------------------------------------
void display_load_all_textures(void)
{
    // <ZZ> This function loads all of the textures in the datafile onto the graphics card.
    int i;
    unsigned char* index;


    // Go through each file and try to load it...
    log_message("INFO:   Trying to load all textures into graphics memory...");
    repeat(i, sdf_num_files)
    {
        index = sdf_index + (i<<4);

        display_load_texture(index);
    }
    setup_shadow();
}

//-----------------------------------------------------------------------------------------------
unsigned char display_extension_supported(unsigned char* extension_name)
{
    // <ZZ> This function returns TRUE if the given extension is supported...
    unsigned char* extensions;
	unsigned char* data;
    unsigned char perfect_match;
    unsigned short length;

    extensions = (unsigned char*) glGetString(GL_EXTENSIONS);
    data = extensions;
    length = strlen(extension_name);
    while(data)
    {
        data = strstr(data, extension_name);
        if(data)
        {
            // Found a matching substring, but make sure it doesn't have something in front
            // of it, or at its end...
            perfect_match = TRUE;
            if(data != extensions)
            {
                if( (*(data-1)) !=' ')
                {
                    // Had somethin' funny in front...  Means we didn't really match it...
                    perfect_match = FALSE;
                }
            }
            if(perfect_match)
            {
                if( (*(data+length)) ==' ' || (*(data+length)) == 0)
                {
                    return TRUE;
                }
            }
            data+=length;
        }
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
signed char display_setup(unsigned short size_x, unsigned short size_y, unsigned char color_depth, unsigned char z_depth, unsigned char full_screen)
{
    // <ZZ> This function initializes the display via SDL and GL.
    int rgb_size[3];
    int value;
    int i;
    unsigned int flags;


    // Let's get started...
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    {
        log_message("ERROR:  Couldn't turn on SDL...  That's bad");
        return FALSE;
    }
    if(volumetric_shadows_on)
    {
        log_message("INFO:   Trying to turn on the display with volumetric shadows");
    }
    else
    {
        log_message("INFO:   Trying to turn on the display");
    }


    // Figure out all of the display settings...
    if(color_depth == 0)
    {
        if(SDL_GetVideoInfo()->vfmt->BitsPerPixel <= 8) color_depth = 8;
        else color_depth = 16;
    }
    switch (color_depth)
    {
        case 8:
            rgb_size[0] = 2;
            rgb_size[1] = 3;
            rgb_size[2] = 3;
            break;
        case 15:
        case 16:
            rgb_size[0] = 5;
            rgb_size[1] = 5;
            rgb_size[2] = 5;
            break;
        default:
            rgb_size[0] = 8;
            rgb_size[1] = 8;
            rgb_size[2] = 8;
            break;
    }
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rgb_size[0]);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, rgb_size[1]);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, rgb_size[2]);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, z_depth);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, TRUE);
    if(volumetric_shadows_on)
    {
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    }


    flags = 0;
    display_full_screen = full_screen; // remember global value...
    if(full_screen) { flags = SDL_FULLSCREEN;  log_message("INFO:   Requested fullscreen mode..."); }
    if(SDL_SetVideoMode(size_x, size_y, color_depth, SDL_OPENGL | flags) == NULL)
    {
        log_message("ERROR:  SDL couldn't turn on GL, trying 640x480 mode...  %s", SDL_GetError());
        color_depth = 16;
        z_depth = 16;
        rgb_size[0] = 5;
        rgb_size[1] = 5;
        rgb_size[2] = 5;
        size_x = 640;
        size_y = 480;
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, rgb_size[0]);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, rgb_size[1]);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, rgb_size[2]);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, z_depth);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, TRUE);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 0);
        volumetric_shadows_on = FALSE;
        if(SDL_SetVideoMode(size_x, size_y, color_depth, SDL_OPENGL | SDL_FULLSCREEN) == NULL)
        {
            log_message("ERROR:  Even 640x480 didn't work...  %s", SDL_GetError());
            SDL_Quit();
            return FALSE;
        }
    }


    // Remember how big our display is...
    screen_x = size_x;
    screen_y = size_y;
    virtual_y = 300.0f;
    virtual_x = screen_x * virtual_y / screen_y;
    display_viewport(0,0,screen_x,screen_y);


    // Setup the fades...
    repeat(i, CIRCLE_TOTAL_POINTS)
    {
        circle_xyz[i][2] = 0.0;
    }
    circle_xyz[0][0] = 0.0;  circle_xyz[0][1] = 0.0;
    circle_xyz[1][0] = virtual_x;  circle_xyz[1][1] = 0.0;
    circle_xyz[2][0] = virtual_x;  circle_xyz[2][1] = virtual_y;
    circle_xyz[3][0] = 0.0;  circle_xyz[3][1] = virtual_y;



    // Log some information
    log_message("INFO:     Vendor        == %s", glGetString(GL_VENDOR));
    log_message("INFO:     Renderer      == %s", glGetString(GL_RENDERER));
    log_message("INFO:     Version       == %s", glGetString(GL_VERSION));
    glGetIntegerv(GL_MAX_CLIP_PLANES, &value);
    log_message("INFO:     Clip Planes   == %d", value);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);
    log_message("INFO:     Max Tex Size  == %d", value);
    max_texture_size = (unsigned int) value;
    SDL_GL_GetAttribute( SDL_GL_DOUBLEBUFFER, &value );
    log_message("INFO:     Double Buffer == %d", value);
    SDL_GL_GetAttribute( SDL_GL_STENCIL_SIZE, &value );
    log_message("INFO:     Stencil Bits  == %d", value);
    log_message("INFO:     Resolution    == %dx%d", screen_x, screen_y);
    log_message("INFO:     Color Depth...");
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    log_message("INFO:       Red bits    == %d", value);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    log_message("INFO:       Green bits  == %d", value);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    log_message("INFO:       Blue bits   == %d", value);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    log_message("INFO:     Z Depth       == %d", value);


    // Set the window manager title bar
    SDL_WM_SetCaption( "SoulFu", "SoulFu" );


    // Keep the mouse inside the window
    if(full_screen)
    {
        SDL_WM_GrabInput(SDL_GRAB_ON);
    }


    // Hide the mouse cursor
    SDL_ShowCursor(0);


    // Clear the buffers
    display_clear_buffers();
    display_swap();


    // Make sure the graphics library doesn't try to do its own lighting...
    glDisable(GL_LIGHTING);


    // Setup our camera's field of view...
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    screen_frustum_x = virtual_x / 1000;
    screen_frustum_y = virtual_y / 1000;
    glFrustum(-screen_frustum_x, screen_frustum_x, -screen_frustum_y, screen_frustum_y, ZNEAR, MAX_TERRAIN_DISTANCE);


    // Setup the texture matrix...
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();


    // Setup our initial camera matrix
    // This fixes rotation so it makes sense...  x,y checkerboard...  z height off ground
    // It also scoots the camera back a tad, so the near clipping plane is where it should be...
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glGetFloatv(GL_MODELVIEW_MATRIX, rotate_camera_matrix);
    glTranslatef(0.0, 0.0, -(ZNEAR+1));
    glRotatef(90.0, 1.0, 0.0, 0.0);
    glRotatef(180.0, 0.0, 1.0, 0.0);
    glScalef(-1.0, 1.0, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, initial_camera_matrix);



    // Setup the window matrix...  Don't worry about projection...  Just turn off z...
    // Top left corner is 0, 0...  Bottom right is 400, 300...
    glLoadIdentity();
    glScalef(1.0, -1.0, 1.0);
    glTranslatef(-2*screen_frustum_x, -2*screen_frustum_y, -2*ZNEAR);
    glScalef(0.004f, 0.004f, 1.0);
    glGetFloatv(GL_MODELVIEW_MATRIX, window_camera_matrix);
    log_message("INFO:   ------------------------------------------");


    // Setup the paperdoll stuff...  Draw or don't draw texture elements based on alpha value...  Used for unsorted transparency (trees)...
    display_paperdoll_func(1);


    // Remember to quit...
    atexit(SDL_Quit);
    return TRUE;
}

//-----------------------------------------------------------------------------------------------
void display_character_dot(unsigned char* color, float x, float y, float z, float scale)
{
    // <ZZ> Draws a circle for character movement...  Circle texture should already be picked...
    z+=CHARACTER_Z_FLOAT;
    display_color_alpha(color);
    display_start_fan();
        display_texpos_xy(0, 0);  display_vertex_xyz(x-scale, y-scale, z);
        display_texpos_xy(1, 0);  display_vertex_xyz(x+scale, y-scale, z);
        display_texpos_xy(1, 1);  display_vertex_xyz(x+scale, y+scale, z);
        display_texpos_xy(0, 1);  display_vertex_xyz(x-scale, y+scale, z);
    display_end();
}

//-----------------------------------------------------------------------------------------------
void display_loadin(float amount_done)
{
    // <ZZ> Draws some stuff onscreen so the player doesn't get too impatient waiting for it
    //      to start...  Amount_done ranges from 0.0 (0%) to 1.0 (100%)
    float x, x_add;
    unsigned short i;
    display_blend_off();
    display_shade_off();
    display_zbuffer_off();
    display_cull_on();
    display_clear_buffers();
    glLoadMatrixf(window_camera_matrix);
    x_add = 15.0f;
    x = (virtual_x - (x_add*7.0f))*0.5f;
    repeat(i, 7)
    {
        display_color(grey);
        display_kanji(i, x-2.5f, 135.0f, 30.0f, 30.0f);
        display_color(white);
        display_kanji(i, x, 137.5f, 25.0f, 25.0f);
        x+=x_add;
    }
    // Draw the amount done bar...
    display_texture_off();
    if(amount_done < 0.0f) { amount_done = 0.0f; }
    if(amount_done > 1.0f) { amount_done = 1.0f; }
    amount_done*=virtual_x-200.0f;
    display_color(grey);
    display_start_fan();
        display_vertex_xy(100.0f-2.5f, 250.0f-2.5f);
        display_vertex_xy(virtual_x-100.0f+2.5f, 250.0f-2.5f);
        display_vertex_xy(virtual_x-100.0f+2.5f, 275.0f+2.5f);
        display_vertex_xy(100.0f-2.5f, 275.0f+2.5f);
    display_end();
    display_color(white);
    display_start_fan();
        display_vertex_xy(100.0f, 250.0f);
        display_vertex_xy(100.0f+amount_done, 250.0f);
        display_vertex_xy(100.0f+amount_done, 275.0f);
        display_vertex_xy(100.0f, 275.0f);
    display_end();
    display_swap();
    display_texture_on();
}

//-----------------------------------------------------------------------------------------------
void display_start_fade(unsigned char type, signed char direction, float x, float y, unsigned char* color)
{
    // <ZZ> This function begins a new fade
    if(type != FADE_TYPE_WARNING || fade_type == FADE_TYPE_NONE || fade_type == FADE_TYPE_WARNING)
    {
        fade_type = type;
        fade_x = x;
        fade_y = y;
        fade_direction = direction;
        fade_color[0] = color[0];
        fade_color[1] = color[1];
        fade_color[2] = color[2];
        fade_color[3] = 255;
        fade_time = 0;
        if(direction < 0)
        {
            fade_time = 255;
        }
    }
}

//-----------------------------------------------------------------------------------------------
#define circle_quarter(corner, first, count)                                                \
{                                                                                           \
    display_start_fan();                                                                    \
        display_vertex(circle_xyz[corner]);                                                 \
        display_vertex(circle_xyz[(corner+1)&3]);                                           \
        repeat(i, (count+1))                                                                \
        {                                                                                   \
            display_vertex(circle_xyz[first+(count-i)]);                                    \
        }                                                                                   \
    display_end();                                                                          \
}

//-----------------------------------------------------------------------------------------------
#define warning_side(corner)                                                                \
{                                                                                           \
    display_start_fan();                                                                    \
        fade_color[3] = (unsigned char) (fade_time<<2);                                     \
        display_color_alpha(fade_color);                                                    \
        display_vertex(circle_xyz[corner]);                                                 \
        display_vertex(circle_xyz[(corner+1)&3]);                                           \
        fade_color[3] = 0;                                                                  \
        display_color_alpha(fade_color);                                                    \
        display_vertex(circle_xyz[((corner+1)&3)+4]);                                       \
        display_vertex(circle_xyz[corner+4]);                                               \
    display_end();                                                                          \
}

//-----------------------------------------------------------------------------------------------
void display_fade(void)
{
    // <ZZ> This function draws the fade out effect on screen...
    int i;
    float scale_outer;
    if(fade_type != FADE_TYPE_NONE)
    {
        // Change the time
        fade_time += fade_direction * main_frame_skip;
        clip(0, fade_time, 255);


        if(fade_type == FADE_TYPE_CIRCLE)
        {
            // Generate the points...
            scale_outer = 1.00f*(255-fade_time);
            repeat(i, CIRCLE_POINTS+1)
            {
                // !!!BAD!!!
                // !!!BAD!!! Sin Cos lookup...
                // !!!BAD!!!
                circle_xyz[i+4][0] = ((scale_outer) * ((float)  sin(i * (6.2831858 / CIRCLE_POINTS))) ) + fade_x;
                circle_xyz[i+4][1] = ((scale_outer) * ((float) -cos(i * (6.2831858 / CIRCLE_POINTS))) ) + fade_y;
            }


            // Draw the main circle
            display_color(fade_color);
            display_shade_off();
            circle_quarter(1, 4, CIRCLE_QUARTER);
            circle_quarter(2, (4+CIRCLE_QUARTER), CIRCLE_QUARTER);
            circle_quarter(3, (4+2*CIRCLE_QUARTER), CIRCLE_QUARTER);
            circle_quarter(0, (4+3*CIRCLE_QUARTER), CIRCLE_QUARTER);
        }
        if(fade_type == FADE_TYPE_WARNING)
        {
            // Generate the points...
            circle_xyz[4][0] = WARNING_SIZE;   circle_xyz[4][1] = WARNING_SIZE;
            circle_xyz[5][0] = virtual_x-WARNING_SIZE;   circle_xyz[5][1] = WARNING_SIZE;
            circle_xyz[6][0] = virtual_x-WARNING_SIZE;   circle_xyz[6][1] = virtual_y-WARNING_SIZE;
            circle_xyz[7][0] = WARNING_SIZE;   circle_xyz[7][1] = virtual_y-WARNING_SIZE;


            // Draw the effect
            if(fade_time > 30) { fade_direction = FADE_IN;  fade_time = 30; }
            display_blend_trans();
            warning_side(0);
            warning_side(1);
            warning_side(2);
            warning_side(3);
        }
        if(fade_type == FADE_TYPE_FULL)
        {
            // Draw the effect
            display_blend_trans();
            display_start_fan();
                fade_color[3] = (unsigned char) fade_time;
                display_color_alpha(fade_color);
                display_vertex(circle_xyz[0]);
                display_vertex(circle_xyz[1]);
                display_vertex(circle_xyz[2]);
                display_vertex(circle_xyz[3]);
            display_end();
        }


        if(fade_time == 255) fade_direction = -fade_direction;
        if(fade_time == 0) fade_type = FADE_TYPE_NONE;
    }
}

//-----------------------------------------------------------------------------------------------
void display_camera_position(unsigned short times_to_slog, float slog_weight, float slog_z_weight)
{
    // <ZZ> This function generates the camera's xyz position, based on its rotation...
    float angle_xy[2];
    unsigned short i;
    unsigned short num_local_player;
    unsigned short character;
    unsigned char found;
    unsigned short mount;
    float inverse_weight;
    float inverse_z_weight;
    float target_temp_xyz[3];
    float offset_xyz[3];
    float shake_modifier;
    float centrid_distance;


    // Track local player characters...  Centrid of local players is the camera target...
    target_temp_xyz[X] = 0.0f;
    target_temp_xyz[Y] = 0.0f;
    target_temp_xyz[Z] = 0.0f;
    num_local_player = 0;
    repeat(i, MAX_LOCAL_PLAYER)
    {
        // Is this player active?
        if(player_device_type[i])
        {
            // Find this player's character
            character = local_player_character[i];
            if(character < MAX_CHARACTER)
            {
                if(main_character_on[character])
                {
                    target_temp_xyz[X] += ((float*) (main_character_data[character]))[X];
                    target_temp_xyz[Y] += ((float*) (main_character_data[character]))[Y];
                    target_temp_xyz[Z] += ((float*) (main_character_data[character]))[Z] + 4.0f;  // Plus 4 so we center more on character faces...
                    num_local_player++;
                }
            }
        }
    }
    if(num_local_player > 0)
    {
        target_temp_xyz[X]/=num_local_player;
        target_temp_xyz[Y]/=num_local_player;
        target_temp_xyz[Z]/=num_local_player;
    }
    else
    {
        target_temp_xyz[X] = target_xyz[X];
        target_temp_xyz[Y] = target_xyz[Y];
        target_temp_xyz[Z] = target_xyz[Z];
    }



    centrid_distance = 0.0f;
    if(num_local_player > 1)
    {
        // Do auto zoom out if more than one player...
        repeat(i, MAX_LOCAL_PLAYER)
        {
            // Is this player active?
            if(player_device_type[i])
            {
                // Find this player's character
                character = local_player_character[i];
                if(character < MAX_CHARACTER)
                {
                    if(main_character_on[character])
                    {
                        offset_xyz[X] = target_temp_xyz[X] - ((float*) (main_character_data[character]))[X];
                        offset_xyz[Y] = target_temp_xyz[Y] - ((float*) (main_character_data[character]))[Y];
                        offset_xyz[Z] = 0.0f;
                        centrid_distance += vector_length(offset_xyz);
                    }
                }
            }
        }
        centrid_distance /= num_local_player;
        centrid_distance += (num_local_player-1)*3.0f;
    }


    // Do auto zoom out if any player is mounted...
    found = FALSE;
    repeat(i, MAX_LOCAL_PLAYER)
    {
        // Is this player active?
        if(player_device_type[i])
        {
            // Find this player's character
            character = local_player_character[i];
            if(character < MAX_CHARACTER)
            {
                if(main_character_on[character])
                {
                    mount = *((unsigned short*) (main_character_data[character]+164));
                    if(mount < MAX_CHARACTER)
                    {
                        found = TRUE;
                    }
                }
            }
        }
    }
    if(found)
    {
         centrid_distance += 20.0f;
    }


    // Slog the target so it's more squishy...
    inverse_weight = 1.0f - slog_weight;
    inverse_z_weight = 1.0f - slog_z_weight;
    times_to_slog++;
    repeat(i, times_to_slog)
    {
        target_xyz[X] = (target_xyz[X]*slog_weight) + (target_temp_xyz[X]*inverse_weight);
        target_xyz[Y] = (target_xyz[Y]*slog_weight) + (target_temp_xyz[Y]*inverse_weight);
        target_xyz[Z] = (target_xyz[Z]*slog_z_weight) + (target_temp_xyz[Z]*inverse_z_weight);

//        camera_distance = (camera_distance*0.95f) + (camera_to_distance*0.05f);
        camera_distance = (camera_distance*0.90f) + (camera_to_distance*0.05f) + ((camera_distance+centrid_distance)*0.05f);
        camera_to_distance = (camera_to_distance*0.99f) + (35.0f*0.01f);  // Gradually return camera to default zoom...

        camera_rotation_xy[X] += camera_rotation_add_xy[X]>>4;
        camera_rotation_add_xy[X] -= camera_rotation_add_xy[X]>>4;
        camera_rotation_xy[Y] += camera_rotation_add_xy[Y]>>4;
        camera_rotation_add_xy[Y] -= camera_rotation_add_xy[Y]>>4;
    }



    // Limit camera rotation
    if(((signed short) camera_rotation_xy[Y]) < MIN_CAMERA_Y)
    {
        camera_rotation_xy[Y] = MIN_CAMERA_Y;
    }
    if(camera_rotation_xy[Y] > MAX_CAMERA_Y)
    {
        camera_rotation_xy[Y] = MAX_CAMERA_Y;
    }


    // Do screen shake effect...
    if(screen_shake_timer > 0)
    {
        offset_xyz[X] = (random_table[((screen_shake_timer>>2))&and_random]-128) * screen_shake_amount;
        offset_xyz[Y] = (random_table[((screen_shake_timer>>2)+1)&and_random]-128) * screen_shake_amount;
        offset_xyz[Z] = (random_table[((screen_shake_timer>>2)+2)&and_random]-128) * screen_shake_amount;
        shake_modifier = (float) sin((screen_shake_timer & 3) * 0.7854f);
        offset_xyz[X] *= shake_modifier;
        offset_xyz[Y] *= shake_modifier;
        offset_xyz[Z] *= shake_modifier;

        target_xyz[X] += offset_xyz[X];
        target_xyz[Y] += offset_xyz[Y];
        target_xyz[Z] += offset_xyz[Z];

        screen_shake_timer -= main_frame_skip;
    }








    // !!!BAD!!!
    // !!!BAD!!!  Use look up table...
    // !!!BAD!!!
    angle_xy[X] = camera_rotation_xy[X]*2*PI/65536.0f;
    angle_xy[Y] = camera_rotation_xy[Y]*2*PI/65536.0f;
    camera_xyz[X] = (float) sin(angle_xy[X]);
    camera_xyz[Y] = (float) cos(angle_xy[X]);

    camera_xyz[Z] = (float) cos(angle_xy[Y]);
    camera_xyz[X] *= camera_xyz[Z];
    camera_xyz[Y] *= camera_xyz[Z];
    camera_xyz[Z] = (float) sin(angle_xy[Y]);

    camera_xyz[X] *= camera_distance;
    camera_xyz[Y] *= camera_distance;
    camera_xyz[Z] *= camera_distance;

    camera_xyz[X] += target_xyz[X];
    camera_xyz[Y] += target_xyz[Y];
    camera_xyz[Z] += target_xyz[Z];

//sprintf(DEBUG_STRING, "%3.6f, %3.6f, %3.6f", target_xyz[X], target_xyz[Y], target_xyz[Z]);
}

//-----------------------------------------------------------------------------------------------
void display_look_at(float* lilcam_xyz, float* liltarg_xyz)
{
    float distance, x, y, z;


    // Front
    glLoadMatrixf(initial_camera_matrix);
    x = lilcam_xyz[X]-liltarg_xyz[X];
    y = lilcam_xyz[Y]-liltarg_xyz[Y];
    z = lilcam_xyz[Z]-liltarg_xyz[Z];
    distance = (float) sqrt(x*x + y*y + z*z);
    if(distance < 0.0001) distance = 0.0001f;
    rotate_camera_matrix[1] = x/distance;
    rotate_camera_matrix[5] = y/distance;
    rotate_camera_matrix[9] = z/distance;

    // Side
    x = rotate_camera_matrix[1];
    y = rotate_camera_matrix[5];
    distance = (float) sqrt(x*x + y*y);
    if(distance < 0.0001) {  y = 1.0f;  distance = 1.0f; }
    rotate_camera_matrix[0] = y/distance;
    rotate_camera_matrix[4] = -x/distance;
    rotate_camera_matrix[8] = 0;

    // Up
    rotate_camera_matrix[2] = rotate_camera_matrix[4]*rotate_camera_matrix[9];
    rotate_camera_matrix[6] = -rotate_camera_matrix[0]*rotate_camera_matrix[9];
    rotate_camera_matrix[10] = rotate_camera_matrix[0]*rotate_camera_matrix[5]-rotate_camera_matrix[4]*rotate_camera_matrix[1];
    glMultMatrixf(rotate_camera_matrix);
    glTranslatef(-lilcam_xyz[X], -lilcam_xyz[Y], -lilcam_xyz[Z]);


    // Stuff for cartoon lighting
    rotate_enviro_matrix[0] = rotate_camera_matrix[0]*0.5f;
    rotate_enviro_matrix[1] = rotate_camera_matrix[4]*0.5f;
    rotate_enviro_matrix[2] = rotate_camera_matrix[8]*0.5f;
    rotate_enviro_matrix[3] = rotate_camera_matrix[2]*-0.5f;
    rotate_enviro_matrix[4] = rotate_camera_matrix[6]*-0.5f;
    rotate_enviro_matrix[5] = rotate_camera_matrix[10]*-0.5f;


    // Remember where the camera is lookin'
    camera_fore_xyz[X] = -rotate_camera_matrix[1];
    camera_fore_xyz[Y] = -rotate_camera_matrix[5];
    camera_fore_xyz[Z] = -rotate_camera_matrix[9];
    camera_side_xyz[X] = rotate_camera_matrix[0];
    camera_side_xyz[Y] = rotate_camera_matrix[4];
    camera_side_xyz[Z] = rotate_camera_matrix[8];


    // Figure out the onscreen point matrix...
    glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMultMatrixf(modelview_matrix);
    glGetFloatv(GL_PROJECTION_MATRIX, onscreen_matrix);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------------------------
unsigned int display_get_texture(char* filename)
{
    // <ZZ> Helper function.  Returns GL texture name for a given file...  Returns 0
    //      if the file can't be found...
    unsigned char* data;

    data = sdf_find_filetype(filename, SDF_FILE_IS_RGB);
    if(data)
    {
        data = (unsigned char*) sdf_read_unsigned_int(data);
        return (*((unsigned int*) (data+2)));
    }
    return 0;
}

//-----------------------------------------------------------------------------------------------
void display_kanji_setup(void)
{
    // <ZZ> This function finds the kanji file for us...
    kanji_data = sdf_find_filetype("KANJI", SDF_FILE_IS_DAT);
    if(kanji_data)
    {
        kanji_data = (unsigned char*) sdf_read_unsigned_int(kanji_data);
    }
}

//-----------------------------------------------------------------------------------------------
