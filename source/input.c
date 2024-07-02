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
// !!!BAD!!!  Stuff up here
// !!!BAD!!!
// !!!BAD!!!

#define JOY_TOLERANCE 5000                      // Joystick must move so much before it activates...


#define MAX_KEY SDL_NUM_SCANCODES                       // The number of keys
#define MAX_KEY_BUFFER 256                      // Must be 256
#define MAX_ASCII 128                           // Must be 128
unsigned char key_down[MAX_KEY];                // TRUE if key is held down
unsigned char key_pressed[MAX_KEY];             // TRUE if key was just pressed (updated once per timing cycle)
unsigned char key_unpressed[MAX_KEY];           // TRUE if key was just released (updated once per timing cycle)
unsigned char window_key_pressed[MAX_KEY];      // TRUE if key was just pressed (updated once per display)
unsigned char key_buffer[MAX_KEY_BUFFER];       // For reading keyboard strings
unsigned char key_buffer_read;                  // FIFO read position
unsigned char key_buffer_write;                 // FIFO write position
unsigned char key_shift[MAX_ASCII];             // Convert an SDLK_ to ASCII caps
unsigned short last_key_pressed = 0;            // The sdlk value of the last key pressed...



#define MOUSE_TEXT_TIME 10                      // Number of ticks mouse text should show after taking off of character...
#define MAX_MOUSE_BUTTON 4
#define BUTTON0    0
#define BUTTON1    1
#define BUTTON2    2
#define BUTTON3    3
unsigned char* mouse_last_click_object;         // Pointer to data of object last clicked on...
char mouse_text[MAX_STRING_SIZE];               // The alternate text...  Tool tips...
char mouse_text_timer;                          // For names of mouse-over'd characters/items
float mouse_x;                                  // Left/Right cursor position, 0-399.99
float mouse_y;                                  // Up/Down cursor position, 0-299.99
float mouse_last_x;                             // Last Left/Right cursor position, 0-399.99
float mouse_last_y;                             // Last Up/Down cursor position, 0-299.99
float mouse_character_distance;                 // Distance from highlighted character (for finding nearest)
unsigned short mouse_idle_timer = 0;            // Time mouse has been unmoved...
unsigned char mouse_alpha = 255;                // Mouse transparency...
unsigned char mouse_down[MAX_MOUSE_BUTTON];     // Current button states for the mouse
unsigned char mouse_pressed[MAX_MOUSE_BUTTON];  // Button clicks...  like mouse_button
unsigned char mouse_unpressed[MAX_MOUSE_BUTTON];// Button releases...  ditto
unsigned char* mouse_last_object;               // Window the mouse cursor was last over
unsigned short mouse_last_item;                 // Last input item from that window...
unsigned char* mouse_current_object;            // Window the mouse cursor is currently over
unsigned short mouse_current_item;              // Current input item from that window...
unsigned char* last_input_object = NULL;        // For WindowInput()
unsigned short last_input_item = NO_ITEM;       // For WindowInput()
unsigned char  last_input_mouse = 0;            // For WindowInput()
signed int last_input_cursor_pos = 0;           // For WindowInput()
unsigned char mouse_draw = TRUE;                //
unsigned char mouse_camera_active = FALSE;      //


#define MAX_JOYSTICK 8
#define MAX_JOYSTICK_BUTTON 16
int num_joystick;
void* joystick_structure[MAX_JOYSTICK];         // Used for closing the joysticks...
unsigned char joystick_button_pressed[MAX_JOYSTICK][MAX_JOYSTICK_BUTTON];
unsigned char joystick_button_unpressed[MAX_JOYSTICK][MAX_JOYSTICK_BUTTON];
unsigned char joystick_button_down[MAX_JOYSTICK][MAX_JOYSTICK_BUTTON];
float joystick_position_xy[MAX_JOYSTICK][2];    // -1.0 to 1.0...


//-------------------------------------------------------------------------------------------
#define input_quick_reset_key_buffer() { key_buffer_read = 0;  key_buffer_write = 0; }

//-----------------------------------------------------------------------------------------------
void input_setup_key_buffer(void)
{
    // <ZZ> This function deletes all data in the key_buffer
    int i;
    key_buffer_read = 0;
    key_buffer_write = 0;
    repeat(i, MAX_KEY_BUFFER)
    {
        key_buffer[i] = 0;
    }
}

//-------------------------------------------------------------------------------------------
void input_setup_key_shift(void)
{
    // <ZZ> This function sets up a lookup table.  The table converts an SDLK_
    //      value into the corresponding ASCII value for a shifted key.
    int i;


    // Fill the table
    repeat(i, MAX_ASCII)
    {
        // Default value
        key_shift[i] = i;
        // Capital letters
        if(i >= 'a' && i <= 'z')
        {
            key_shift[i] = i - 'a' + 'A';
        }
    }
    // Shifted numbers
    key_shift['0'] = ')';
    key_shift['1'] = '!';
    key_shift['2'] = '@';
    key_shift['3'] = '#';
    key_shift['4'] = '$';
    key_shift['5'] = '%';
    key_shift['6'] = '^';
    key_shift['7'] = '&';
    key_shift['8'] = '*';
    key_shift['9'] = '(';
    // Other symbols
    key_shift['`'] = '~';
    key_shift['-'] = '_';
    key_shift['='] = '+';
    key_shift['['] = '{';
    key_shift[']'] = '}';
    key_shift[';'] = ':';
    key_shift[','] = '<';
    key_shift['.'] = '>';
    key_shift['/'] = '?';
    key_shift['\''] = '\"';
    key_shift['\\'] = '|';
}

//-------------------------------------------------------------------------------------------
void input_free_joysticks(void)
{
    // <ZZ> This function should be called automatically when the program ends...  Just
    //      silly SDL stuff to free up memory.
    int i;

    repeat(i, MAX_JOYSTICK)
    {
        if(joystick_structure[i] != NULL)
        {
            SDL_JoystickClose(joystick_structure[i]);
        }
    }
}

//-------------------------------------------------------------------------------------------
void input_update(void)
{
    // <ZZ> This function helps in doing the mouse/window interface & stuff...
    unsigned short i, j, button;
    unsigned short joystick;


    // For WindowInput()...  Clicking outside the input box unsets it...
    if(mouse_pressed[BUTTON0])
    {
        if(mouse_current_object != last_input_object || mouse_current_item != last_input_item)
        {
            last_input_object = NULL;
            last_input_item = NO_ITEM;
        }
    }
    if(input_active > 0)
    {
        input_active--;
    }
    if(mouse_idle_timer < 65535)
    {
        mouse_idle_timer++;
    }


    // For all the window functions...
    if(mouse_pressed[BUTTON0])
    {
        mouse_last_click_object=mouse_current_object;
    }
    mouse_last_object = mouse_current_object;
    mouse_last_item = mouse_current_item;
    mouse_current_object = NULL;
    mouse_current_item = NO_ITEM;
    mouse_last_x = mouse_x;
    mouse_last_y = mouse_y;
    if(mouse_character_distance > 999999.0f)
    {
        i = mouse_text_timer;
        i -= main_frame_skip;
        if(i > 255) { mouse_text_timer = 0; }
        else { mouse_text_timer = (unsigned char) i; }
    }
    mouse_character_distance = 1000000.0f;
    repeat(i, MAX_MOUSE_BUTTON)
    {
        mouse_pressed[i] = 0;
        mouse_unpressed[i] = 0;
    }


    // For player controls...
    repeat(i, MAX_LOCAL_PLAYER)
    {
        player_device_inventory_toggle[i] = FALSE;
        player_device_inventory_down[i] = FALSE;
        if(player_device_type[i] > 1)
        {
            // Joystick controls...
            joystick = (player_device_type[i]-2) & (MAX_JOYSTICK-1);
            player_device_xy[i][X] = joystick_position_xy[joystick][X];
            player_device_xy[i][Y] = joystick_position_xy[joystick][Y];


            // Fill in button presses...  Only set 'em (don't unset 'em here...  unset when used...)
            repeat(j, 4)
            {
                button = player_device_button[i][j];
                player_device_button_pressed[i][j] = FALSE;
                player_device_button_unpressed[i][j] = FALSE;
                if(button < MAX_JOYSTICK_BUTTON)
                {
                    player_device_button_pressed[i][j] |= joystick_button_pressed[joystick][button];
                    if(joystick_button_pressed[joystick][button] && player_device_button_unpressed[i][j])
                    {
                        player_device_button_unpressed[i][j] = FALSE;
                    }
                    player_device_button_unpressed[i][j] |= joystick_button_unpressed[joystick][button];
                }
            }
            button = player_device_button[i][PLAYER_DEVICE_BUTTON_ITEMS];
            if(button < MAX_JOYSTICK_BUTTON)
            {
                player_device_inventory_toggle[i] = joystick_button_pressed[joystick][button];
                player_device_inventory_down[i] = joystick_button_down[joystick][button];
            }
        }
        else
        {
            player_device_xy[i][X] = 0.0f;
            player_device_xy[i][Y] = 0.0f;
            if(player_device_type[i] == 1)
            {
                if(global_block_keyboard_timer > 0 || input_active > 0)
                {
                    // Keyboard controls don't work right now...  (Rogue book window is open or typing in a message window...)
                    repeat(j, 4)
                    {
                        player_device_button_unpressed[i][j] = FALSE;
                        if(player_device_button_pressed[i][j])
                        {
                            player_device_button_unpressed[i][j] = TRUE;
                        }
                        player_device_button_pressed[i][j] = FALSE;
                    }
                }
                else
                {
                    // Keyboard controls...
                    if(key_down[player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_UP] % MAX_KEY])
                    {
                        player_device_xy[i][Y]-=1.0f;
                    }
                    if(key_down[player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_DOWN] % MAX_KEY])
                    {
                        player_device_xy[i][Y]+=1.0f;
                    }
                    if(key_down[player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_LEFT] % MAX_KEY])
                    {
                        player_device_xy[i][X]-=1.0f;
                    }
                    if(key_down[player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_RIGHT] % MAX_KEY])
                    {
                        player_device_xy[i][X]+=1.0f;
                    }


                    // Button presses...
                    repeat(j, 4)
                    {
                        button = player_device_button[i][j] % MAX_KEY;
                        player_device_button_pressed[i][j] |= key_pressed[button];
                        if(key_pressed[button] && player_device_button_unpressed[i][j])
                        {
                            player_device_button_unpressed[i][j] = FALSE;
                        }
                        player_device_button_unpressed[i][j] |= key_unpressed[button];
                    }
                    button = player_device_button[i][PLAYER_DEVICE_BUTTON_ITEMS] % MAX_KEY;
                    player_device_inventory_toggle[i] = key_pressed[button];
                    player_device_inventory_down[i] = key_down[button];
                }
            }
        }
        player_device_button_pressed_copy[i][0] = player_device_button_pressed[i][0];
        player_device_button_pressed_copy[i][1] = player_device_button_pressed[i][1];
        player_device_button_pressed_copy[i][2] = player_device_button_pressed[i][2];
        player_device_button_pressed_copy[i][3] = player_device_button_pressed[i][3];
    }


    // Block keyboard input for players...
    if(global_block_keyboard_timer > 0)
    {
        global_block_keyboard_timer--;
    }
}

//-------------------------------------------------------------------------------------------
void input_setup(void)
{
    // <ZZ> This function sets up all of our Human Interface Devices...
    int i, j;


    // Setup keyboard information
    repeat(i, MAX_KEY)
    {
        key_down[i] = FALSE;
        key_pressed[i] = FALSE;
        key_unpressed[i] = FALSE;
        window_key_pressed[i] = FALSE;
    }
    input_setup_key_buffer();
    input_setup_key_shift();


    // Setup mouse information
    mouse_last_click_object = NULL;
    mouse_x = virtual_x / 2;
    mouse_y = virtual_y / 2;
    repeat(i, MAX_MOUSE_BUTTON)
    {
        mouse_down[i] = 0;
        mouse_pressed[i] = 0;
        mouse_unpressed[i] = 0;
    }
    mouse_current_object = NULL;
    mouse_current_item = NO_ITEM;
    mouse_last_object = NULL;
    mouse_last_item = NO_ITEM;
    mouse_text[0] = 0;
    mouse_idle_timer = 0;
    mouse_alpha = 255;


    // Turn on the joysticks
    repeat(i, MAX_JOYSTICK)
    {
        joystick_structure[i] = NULL;
        joystick_position_xy[i][X] = 0.0f;
        joystick_position_xy[i][Y] = 0.0f;
        repeat(j, MAX_JOYSTICK_BUTTON)
        {
            joystick_button_pressed[i][j] = FALSE;
            joystick_button_unpressed[i][j] = FALSE;
            joystick_button_down[i][j] = FALSE;
        }
    }


    SDL_JoystickEventState(SDL_ENABLE);
    num_joystick = SDL_NumJoysticks();
    log_message("INFO:   Turning on %d joysticks...", num_joystick);
    repeat(i, num_joystick)
    {
        if(i < MAX_JOYSTICK)
        {
            joystick_structure[i] = SDL_JoystickOpen(i);
            log_message("INFO:     %d...  %s", i, SDL_JoystickName(joystick_structure[i]));
        }
    }
    atexit(input_free_joysticks);


    // Turn off all of the player devices...
    repeat(i, MAX_LOCAL_PLAYER)
    {
        player_device_type[i] = PLAYER_DEVICE_NONE;
        player_device_inventory_toggle[i] = FALSE;
        player_device_inventory_down[i] = FALSE;
        player_device_controls_active[i] = TRUE;
        local_player_character[i] = MAX_CHARACTER;
    }
}

//-------------------------------------------------------------------------------------------
unsigned char input_read_key_buffer(void)
{
    // <ZZ> This function returns the next character in the key_buffer, or 0 if there is
    //      is no data
    unsigned char returnvalue;


    // Is there data?
    returnvalue = 0;
    if(key_buffer_read != key_buffer_write)
    {
        returnvalue = key_buffer[key_buffer_read];
        key_buffer_read++;
    }
    return returnvalue;
}

//-------------------------------------------------------------------------------------------
void input_write_key_buffer(unsigned char keytowrite)
{
    // <ZZ> This function adds a character to the key_buffer
    key_buffer_write++;
    if(key_buffer_read == key_buffer_write)
    {
        // Wasn't room in the buffer, so back up
        key_buffer_write--;
    }
    else
    {
        // Tested okay, so backup and add the key
        key_buffer_write--;
        key_buffer[key_buffer_write] = keytowrite;
        key_buffer_write++;
    }
}

//-------------------------------------------------------------------------------------------
void input_reset_window_key_pressed(void)
{
    // <ZZ> This function clears out all of the keypresses for windows
    int i;


    // Unpress all of the keys
    repeat(i, MAX_KEY)
    {
        window_key_pressed[i] = FALSE;
    }
}


//-----------------------------------------------------------------------------------------------
void input_read(void)
{
    // <ZZ> This function asks SDL what's happening, and translates that into something more
    //      useful.  It fills up lots of input arrays...  This new version will hopefully
    //      not have the same trouble with stuck keys...
    SDL_Event event;
    unsigned char button_state, button_down;
    unsigned short i, j, num_keys;
    SDL_Scancode key;
    const unsigned char* key_state;
    int temp;



    // Unpress all of the keys
    repeat(i, MAX_KEY)
    {
        key_pressed[i] = FALSE;
        key_unpressed[i] = FALSE;
    }


    // Unpress all of the joystick buttons...
    repeat(i, num_joystick)
    {
        repeat(j, MAX_JOYSTICK_BUTTON)
        {
            joystick_button_pressed[i][j] = FALSE;
            joystick_button_unpressed[i][j] = FALSE;
        }
    }



    // Handle events that we've gotten since last update...
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                key = (unsigned short) event.key.keysym.scancode;
                if(key < MAX_KEY)
                {
                    if(!key_down[key])
                    {
                        key_down[key] = TRUE;
                        key_pressed[key] = TRUE;
                        window_key_pressed[key] = TRUE;


                        // Remember the key...
                        last_key_pressed = key;
                    }


                    // If it's an ASCII character, add it to the key_buffer...
                    SDL_Keycode kc = SDL_GetKeyFromScancode(key);
                    if((kc < MAX_ASCII && kc >= ' ') || kc == SDLK_BACKSPACE || kc == SDLK_RETURN)
                    {
                        if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                        {
                            // Write a shifted character
                            input_write_key_buffer(key_shift[kc]);
                        }
                        else
                        {
                            // Write a lowercase character
                            input_write_key_buffer((unsigned char) kc);
                        }
                    }
//                    if(key == SDLK_ESCAPE)
//                    {
//                        quitgame = TRUE;
//                        log_message("INFO:   Escape key pressed...");
//                    }
                }
                break;
            case SDL_KEYUP:
                key = (unsigned short) event.key.keysym.scancode;
                if(key < MAX_KEY)
                {
                    if(key_down[key])
                    {
                        key_down[key] = FALSE;
                        key_unpressed[key] = TRUE;
                    }
                }
                break;
            case SDL_QUIT:
                quitgame = TRUE;
                break;
            case SDL_MOUSEMOTION:
                mouse_idle_timer = 0;
                if(display_full_screen)
                {
                    mouse_x += event.motion.xrel * 0.625f;
                    mouse_y += event.motion.yrel * 0.625f;
                    clip(0.0f, mouse_x, virtual_x);
                    clip(0.0f, mouse_y, virtual_y);
                }
                else
                {
                    mouse_x = event.motion.x * virtual_x / screen_x;
                    mouse_y = event.motion.y * virtual_y / screen_y;
                }
                break;
            case SDL_JOYAXISMOTION:
                if(event.jaxis.which < MAX_JOYSTICK)
                {
                    if(event.jaxis.axis == X || event.jaxis.axis == Y)
                    {
                        joystick_position_xy[event.jaxis.which][event.jaxis.axis] = 0.0f;
                        if(event.jaxis.value < -JOY_TOLERANCE)
                        {
                            joystick_position_xy[event.jaxis.which][event.jaxis.axis] = (event.jaxis.value+JOY_TOLERANCE)/(32768.0f-JOY_TOLERANCE);
                        }
                        else if(event.jaxis.value > JOY_TOLERANCE)
                        {
                            joystick_position_xy[event.jaxis.which][event.jaxis.axis] = (event.jaxis.value-JOY_TOLERANCE)/(32768.0f-JOY_TOLERANCE);
                        }
                    }
                }    
                break;
            case SDL_JOYBUTTONUP:
                if(event.jbutton.which < MAX_JOYSTICK)
                {
                    if(event.jbutton.button < MAX_JOYSTICK_BUTTON)
                    {
                        joystick_button_unpressed[event.jbutton.which][event.jbutton.button] = TRUE;
                        joystick_button_down[event.jbutton.which][event.jbutton.button] = FALSE;
                    }
                }
                break;
            case SDL_JOYBUTTONDOWN:
                if(event.jbutton.which < MAX_JOYSTICK)
                {
                    if(event.jbutton.button < MAX_JOYSTICK_BUTTON)
                    {
                        joystick_button_pressed[event.jbutton.which][event.jbutton.button] = TRUE;
                        joystick_button_down[event.jbutton.which][event.jbutton.button] = TRUE;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_idle_timer = 0;
                i = (event.button.button-1);
                if(i < MAX_MOUSE_BUTTON)
                {
                    if(mouse_down[i])
                    {
                        mouse_unpressed[i] = TRUE;
                        mouse_down[i] = FALSE;
                    }
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_idle_timer = 0;
                i = (event.button.button-1);
                if(i < MAX_MOUSE_BUTTON)
                {
                    if(mouse_draw)
                    {
                        if(!mouse_down[i])
                        {
                            mouse_pressed[i] = TRUE;
                            mouse_down[i] = TRUE;
                        }
                    }
                }
                break;
            default:
                break;
        }
    }


    // Do a device state check on the keyboard every 16 drawn frames to make sure our
    // information is accurate (just in case we still get a stuck key)...
    if((main_video_frame & 15) == 0)
    {
        key_state = SDL_GetKeyboardState(&temp);
        num_keys = (unsigned short) temp;
        if(num_keys > MAX_KEY)
        {
            // Shouldn't happen...
            num_keys = MAX_KEY;
        }
        repeat(key, num_keys)
        {
            if(key_state[key] != key_down[key])
            {
                if(key_state[key])
                {
                    // Key is newly pressed...
                    key_pressed[key] = TRUE;
                    window_key_pressed[key] = TRUE;
                    last_key_pressed = key;
                }
                else
                {
                    // Key was just let up...
                    key_unpressed[key] = TRUE;
                }
                key_down[key] = key_state[key];
            }
        }
    }


    // Do a device state check on the mouse every 16 drawn frames to make sure our
    // information is accurate (just in case we still get a stuck button)...
    if((main_video_frame & 15) == 8)
    {
        button_state = SDL_GetMouseState(NULL, NULL);
        repeat(i, MAX_MOUSE_BUTTON)
        {
            button_down = (button_state >> i) & 1;
            if(button_down != mouse_down[i])
            {
                if(button_down)
                {
                    if(mouse_draw)
                    {
                        // Mouse button is newly pressed...
                        mouse_pressed[i] = TRUE;
                    }
                }
                else
                {
                    // Mouse button was just let up...
                    mouse_unpressed[i] = TRUE;
                }
                mouse_down[i] = button_down;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------
/* <ZZ> Old input_read() with stuck keys
void input_read(void)
{
    // <ZZ> This function asks SDL what's happening, and translates that into something more
    //      useful.  It fills up lots of input arrays...
    SDL_Event event;
    int i, j, num_event;



    // Unpress all of the keys
    repeat(i, MAX_KEY)
    {
        key_pressed[i] = FALSE;
        key_unpressed[i] = FALSE;
    }


    // Unpress all of the joystick buttons...
    repeat(i, num_joystick)
    {
        repeat(j, MAX_JOYSTICK_BUTTON)
        {
            joystick_button_pressed[i][j] = FALSE;
            joystick_button_unpressed[i][j] = FALSE;
        }
    }


    // Handle events that we've gotten since last update...
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_KEYDOWN:
                key_down[event.key.keysym.sym] = TRUE;
                key_pressed[event.key.keysym.sym] = TRUE;
                window_key_pressed[event.key.keysym.sym] = TRUE;


                // Remember the key...
                last_key_pressed = (unsigned short) event.key.keysym.sym;


                // If it's an ASCII character, add it to the key_buffer...
                if((event.key.keysym.sym < MAX_ASCII && event.key.keysym.sym >= ' ') || event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_RETURN)
                {
                    if(event.key.keysym.mod & (KMOD_LSHIFT | KMOD_RSHIFT))
                    {
                        // Write a shifted character
                        input_write_key_buffer(key_shift[(unsigned char) event.key.keysym.sym]);
                    }
                    else
                    {
                        // Write a lowercase character
                        input_write_key_buffer((unsigned char) event.key.keysym.sym);
                    }
                }
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
//                #ifdef DEVTOOL
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                    if(event.key.keysym.sym == SDLK_ESCAPE)
                    {
                        quitgame = TRUE;
                        log_message("INFO:   Escape key pressed (DEVTOOL only)");
                    }
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
//                #endif
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                break;
            case SDL_KEYUP:
                key_down[event.key.keysym.sym] = FALSE;
                key_unpressed[event.key.keysym.sym] = TRUE;
                break;
            case SDL_QUIT:
                quitgame = TRUE;
                break;
            case SDL_MOUSEMOTION:
                if(display_full_screen)
                {
                    mouse_x += event.motion.xrel * 0.625f;
                    mouse_y += event.motion.yrel * 0.625f;
                    clip(0.0f, mouse_x, 400.0f);
                    clip(0.0f, mouse_y, 300.0f);
                }
                else
                {
                    mouse_x = event.motion.x * 400.0f / screen_x;
                    mouse_y = event.motion.y * 300.0f / screen_y;
                }
                break;
            case SDL_JOYAXISMOTION:
                if(event.jaxis.which < MAX_JOYSTICK)
                {
                    if(event.jaxis.axis == X || event.jaxis.axis == Y)
                    {
                        joystick_position_xy[event.jaxis.which][event.jaxis.axis] = 0.0f;
                        if(event.jaxis.value < -JOY_TOLERANCE)
                        {
                            joystick_position_xy[event.jaxis.which][event.jaxis.axis] = (event.jaxis.value+JOY_TOLERANCE)/(32768.0f-JOY_TOLERANCE);
                        }
                        else if(event.jaxis.value > JOY_TOLERANCE)
                        {
                            joystick_position_xy[event.jaxis.which][event.jaxis.axis] = (event.jaxis.value-JOY_TOLERANCE)/(32768.0f-JOY_TOLERANCE);
                        }
                    }
                }    
                break;
            case SDL_JOYBUTTONUP:
                if(event.jbutton.which < MAX_JOYSTICK)
                {
                    if(event.jbutton.button < MAX_JOYSTICK_BUTTON)
                    {
                        joystick_button_unpressed[event.jbutton.which][event.jbutton.button] = TRUE;
                        joystick_button_down[event.jbutton.which][event.jbutton.button] = FALSE;
                    }
                }
                break;
            case SDL_JOYBUTTONDOWN:
                if(event.jbutton.which < MAX_JOYSTICK)
                {
                    if(event.jbutton.button < MAX_JOYSTICK_BUTTON)
                    {
                        joystick_button_pressed[event.jbutton.which][event.jbutton.button] = TRUE;
                        joystick_button_down[event.jbutton.which][event.jbutton.button] = TRUE;
                    }
                }
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
                i = (event.button.button-1);
                if(i < MAX_MOUSE_BUTTON)
                {
                    if(event.button.state == SDL_PRESSED)
                    {
                        if(mouse_draw)
                        {
                            mouse_pressed[i] = TRUE;
                            mouse_down[i] = TRUE;
                        }
                    }
                    if(event.button.state == SDL_RELEASED)
                    {
                        mouse_unpressed[i] = TRUE;
                        mouse_down[i] = FALSE;
                    }
                }
                break;
            default:
                break;
        }
    }
}
*/

//-------------------------------------------------------------------------------------------
void input_camera_controls(void)
{
    // <ZZ> This function allows the players to rotate the camera and zoom in and out...
    signed short off_x, off_y;
    unsigned char count;
    unsigned short i;
    unsigned char joystick;
    unsigned short button;
    SDL_Event event;



    if(play_game_active)
    {
        // Make sure the mouse isn't over a window...
        if(mouse_last_object == NULL)
        {
            // Also that camera control isn't turn'd off...
            if(mouse_camera_active)
            {
                if(mouse_down[BUTTON0] || mouse_down[BUTTON1] || mouse_down[BUTTON2])
                {
                    // Motion...
                    off_x = (signed short) (mouse_x - mouse_last_x);
                    off_y = (signed short) (mouse_y - mouse_last_y);


                    // Keep the mouse in one spot...
                    SDL_WarpMouseInWindow(main_window,
                        (Uint16) (mouse_last_x * screen_x / virtual_x),
                        (Uint16) (mouse_last_y * screen_y / virtual_y));
                    mouse_x = mouse_last_x;
                    mouse_y = mouse_last_y;
                    // Flush out that last mouse event...
                    while(SDL_PollEvent(&event));


                    // Count number of buttons pressed...
                    count = 0;
                    if(mouse_down[BUTTON0]) count++;
                    if(mouse_down[BUTTON1]) count++;
                    if(mouse_down[BUTTON2]) count++;
                    if(mouse_down[BUTTON3]) count++;


                    // Zoom control if two buttons held...
                    if(count > 1)
                    {
                        // Zoom control...
                        camera_to_distance += (off_y*CAMERA_ZOOM_RATE);
                        if(camera_to_distance < MIN_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MIN_CAMERA_DISTANCE;
                        }
                        if(camera_to_distance > MAX_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MAX_CAMERA_DISTANCE;
                        }
                    }
                    else
                    {
                        // Rotational control...
                        camera_rotation_add_xy[X] += (signed int) (off_x * CAMERA_ROTATION_RATE);
                        camera_rotation_add_xy[Y] -= (signed int) (off_y * CAMERA_ROTATION_RATE);
                    }
                }
                else
                {
                    // No buttons down means we're done moving the camera...
                    mouse_camera_active = FALSE;
                }
            }
            else
            {
                // Check for initiation of mouse camera control (clicking on empty area)...
                if(mouse_pressed[BUTTON0] || mouse_pressed[BUTTON1] || mouse_pressed[BUTTON2])
                {
                    // Allow camera movement next time...
                    mouse_camera_active = TRUE;
                }
            }
        }


        // Allow special +/- zoom controls...  Also 7 and 9 on keypad for rotation...
        if(input_active == 0 && global_block_keyboard_timer == 0)
        {
            // Zoom in
            if(key_down[SDL_SCANCODE_KP_PLUS] || key_down[SDL_SCANCODE_EQUALS])
            {
                camera_to_distance -= (CAMERA_ZOOM_RATE*main_frame_skip);
                if(camera_to_distance < MIN_CAMERA_DISTANCE)
                {
                    camera_to_distance = MIN_CAMERA_DISTANCE;
                }
            }


            // Zoom out
            if(key_down[SDL_SCANCODE_KP_MINUS] || key_down[SDL_SCANCODE_MINUS])
            {
                camera_to_distance += (CAMERA_ZOOM_RATE*main_frame_skip);
                if(camera_to_distance > MAX_CAMERA_DISTANCE)
                {
                    camera_to_distance = MAX_CAMERA_DISTANCE;
                }
            }


            // Rotation
            off_x = 0;
            if(key_down[SDL_SCANCODE_KP_7]) { off_x += 3; }
            if(key_down[SDL_SCANCODE_KP_9]) { off_x -= 3; }
            camera_rotation_add_xy[X] += (signed int) (off_x * CAMERA_ROTATION_RATE * main_frame_skip);
        }


        // Allow joystick players to move camera too...
        repeat(i, MAX_LOCAL_PLAYER)
        {
            // Is the player a joystick player?
            if(player_device_type[i] > 1)
            {
                // Yup...  Check buttons
                joystick = player_device_type[i] - PLAYER_DEVICE_JOYSTICK_1;


                // Zoom in button...
                button = player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_UP];
                if(button < MAX_JOYSTICK_BUTTON)
                {
                    if(joystick_button_down[i][button])
                    {
                        camera_to_distance -= (CAMERA_ZOOM_RATE*main_frame_skip);
                        if(camera_to_distance < MIN_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MIN_CAMERA_DISTANCE;
                        }
                    }
                }


                // Zoom out button...
                button = player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_DOWN];
                if(button < MAX_JOYSTICK_BUTTON)
                {
                    if(joystick_button_down[i][button])
                    {
                        camera_to_distance += (CAMERA_ZOOM_RATE*main_frame_skip);
                        if(camera_to_distance > MAX_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MAX_CAMERA_DISTANCE;
                        }
                    }
                }


                // Rotate buttons...
                off_x = 0;
                button = player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_LEFT];
                if(button < MAX_JOYSTICK_BUTTON) { if(joystick_button_down[i][button]) { off_x += 3; } }
                button = player_device_button[i][PLAYER_DEVICE_BUTTON_MOVE_RIGHT];
                if(button < MAX_JOYSTICK_BUTTON) { if(joystick_button_down[i][button]) { off_x -= 3; } }
                camera_rotation_add_xy[X] += (signed int) (off_x * CAMERA_ROTATION_RATE * main_frame_skip);
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
