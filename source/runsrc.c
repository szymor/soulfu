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

// <ZZ> This file contains functions to run scripts after they've been compiled
//      run_script                  - The main function to run a script


// For WindowEmacs
#define EMACS_SIZE 2048
char emacs_buffer[EMACS_SIZE];
#define EMACS_HORIZ_SIZE 74
char emacs_horiz[EMACS_HORIZ_SIZE] = "//-----------------------------------------------------------------------";
#define EMACS_BAD_SIZE 13
char emacs_bad[EMACS_BAD_SIZE] = "// !!!BAD!!!";
int emacs_buffer_size = 0;
int emacs_buffer_write = 0;
int emacs_return_count = 0;



// For SystemGet and SystemSet
#define SYS_INVALID                 0
#define SYS_PLAYERDEVICE            1
#define SYS_NUMJOYSTICK             2
#define SYS_WINDOWSCALE             3
#define SYS_KEYPRESSED              4
#define SYS_KEYDOWN                 5
#define SYS_TOPWINDOW               6
#define SYS_SFXVOLUME               7
#define SYS_MUSICVOLUME             8
#define SYS_MESSAGE                 9
#define SYS_USERLANGUAGE            10
#define SYS_LANGUAGEFILE            11
#define SYS_QUITGAME                12
#define SYS_FILENAME                13
#define SYS_FILESIZE                14
#define SYS_FILEFTPFLAG             15
#define SYS_FILECOUNT               16
#define SYS_FILEFREE                17
#define SYS_DEVTOOL                 18
#define SYS_CURSORDRAW              19
#define SYS_CURSORPOS               20
#define SYS_CURSORBUTTONDOWN        21
#define SYS_MODELVIEW               22
#define SYS_MODELMOVE               23
#define SYS_MODELPLOP               24
#define SYS_MODELSELECT             25
#define SYS_MODELDELETE             26
#define SYS_MODELPLOPTRIANGLE       27
#define SYS_MODELDELETETRIANGLE     28
#define SYS_MODELREGENERATE         29
#define SYS_MODELPLOPJOINT          30
#define SYS_MODELPLOPBONE           31
#define SYS_MODELDELETEBONE         32
#define SYS_MODELDELETEJOINT        33
#define SYS_MODELJOINTSIZE          34
#define SYS_MODELBONEID             35
#define SYS_MODELCRUNCH             36
#define SYS_MODELROTATEBONES        37
#define SYS_MODELUNROTATEBONES      38
#define SYS_MODELHIDE               39
#define SYS_MODELTRIANGLELINES      40
#define SYS_MODELCOPYPASTE          41
#define SYS_MODELWELDVERTICES       42
#define SYS_MODELWELDTEXVERTICES    43
#define SYS_MODELFLIP               44
#define SYS_MODELSCALE              45
#define SYS_MODELTEXSCALE           46
#define SYS_MODELANCHOR             47
#define SYS_MODELTEXFLAGSALPHA      48
#define SYS_MODELMAXFRAME           49
#define SYS_MODELADDFRAME           50
#define SYS_MODELFRAMEFLAGS         51
#define SYS_MODELFRAMEACTIONNAME    52
#define SYS_MODELACTIONSTART        53
#define SYS_MODELADDBASEMODEL       54
#define SYS_MODELFRAMEBASEMODEL     55
#define SYS_MODELFRAMEOFFSETX       56
#define SYS_MODELFRAMEOFFSETY       57
#define SYS_MODELEXTERNALFILENAME   58
#define SYS_MODELSHADOWTEXTURE      59
#define SYS_MODELSHADOWALPHA        60
#define SYS_MODELINTERPOLATE        61
#define SYS_MODELCENTER             62
#define SYS_MODELNUMTRIANGLE        63
#define SYS_MODELNUMCARTOONLINE     64
#define SYS_MODELSARELINKABLE       65
#define SYS_MODELDETEXTURE          66
#define SYS_MODELPLOPATSTRING       67
#define SYS_MODELMARKFRAME          68
#define SYS_MODELCOPYFRAME          69
#define SYS_MODELAUTOSHADOW         70
#define SYS_LASTKEYPRESSED          71
#define SYS_WATERLAYERSACTIVE       72
#define SYS_CARTOONMODE             73
#define SYS_CURSORLASTPOS           74
#define SYS_CURSORINOBJECT          75
#define SYS_NUMKANJI                76
#define SYS_NUMKANJITRIANGLE        77
#define SYS_KANJICOPY               78
#define SYS_KANJIPASTE              79
#define SYS_KANJIDELETE             80
#define SYS_CURSORSCREENPOS         81
#define SYS_DAMAGEAMOUNT            82
#define SYS_COLLISIONCHAR           83
#define SYS_MAINVIDEOFRAME          84
#define SYS_MAINGAMEFRAME           85
#define SYS_MAINFRAMESKIP           86
#define SYS_FINDDATASIZE            87
#define SYS_BLOCKKEYBOARD           88
#define SYS_BILLBOARDACTIVE         89
#define SYS_ARCTANANGLE             90
#define SYS_MAINSERVERLOCATED       91
#define SYS_SHARDLIST               92
#define SYS_SHARDLISTPING           93
#define SYS_SHARDLISTPLAYERS        94
#define SYS_VERSIONERROR            95
#define SYS_JOINGAME                96
#define SYS_STARTGAME               97
#define SYS_LEAVEGAME               98
#define SYS_NETWORKON               99
#define SYS_MAINGAMEACTIVE          100
#define SYS_NETWORKGAMEACTIVE       101
#define SYS_TRYINGTOJOINGAME        102
#define SYS_JOINPROGRESS            103
#define SYS_GAMESEED                104
#define SYS_LOCALPASSWORDCODE       105
#define SYS_NUMNETWORKPLAYER        106
#define SYS_NETWORKFINISHED         107
#define SYS_SERVERSTATISTICS        108
#define SYS_LOCALPLAYER             109
#define SYS_SCREENSHAKE             110
#define SYS_INCLUDEPASSWORD         111
#define SYS_FPS                     112
#define SYS_RANDOMSEED              113
#define SYS_MIPMAPACTIVE            114
#define SYS_WATERTEXTURE            115
#define SYS_PLAYERCONTROLHANDLED    116
#define SYS_GLOBALSPAWN             117
#define SYS_GLOBALATTACKSPIN        118
#define SYS_GLOBALATTACKER          119
#define SYS_DEBUGACTIVE             120
#define SYS_CURRENTITEM             121
#define SYS_DAMAGECOLOR             122
#define SYS_ITEMREGISTRYCLEAR       123
#define SYS_ITEMREGISTRYSCRIPT      124
#define SYS_ITEMREGISTRYICON        125
#define SYS_ITEMREGISTRYOVERLAY     126
#define SYS_ITEMREGISTRYPRICE       127
#define SYS_ITEMREGISTRYFLAGS       128
#define SYS_ITEMREGISTRYNAME        129
#define SYS_ITEMREGISTRYSTR         130
#define SYS_ITEMREGISTRYDEX         131
#define SYS_ITEMREGISTRYINT         132
#define SYS_ITEMREGISTRYMANA        133
#define SYS_ITEMREGISTRYAMMO        134
#define SYS_WEAPONGRIP              135
#define SYS_WEAPONMODELSETUP        136
#define SYS_WEAPONEVENT             137
#define SYS_WEAPONFRAMEEVENT        138
#define SYS_WEAPONUNPRESSED         139
#define SYS_CHARFASTFUNCTION        140
#define SYS_FASTANDUGLY             141
#define SYS_DEFENSERATING           142
#define SYS_CLEARDEFENSERATING      143
#define SYS_ITEMDEFENSERATING       144
#define SYS_CURSORBUTTONPRESSED     145
#define SYS_CAMERAANGLE             146
#define SYS_STARTFADE               147
#define SYS_MEMBUFFER               148
#define SYS_ROOMUNCOMPRESS          149
#define SYS_ROOMPLOPATSTRING        150
#define SYS_ROOMSELECT              151
#define SYS_ROOMPLOPVERTEX          152
#define SYS_ROOMDELETEVERTEX        153
#define SYS_ROOMWELDVERTICES        154
#define SYS_ROOMCLEAREXTERIORWALL   155
#define SYS_ROOMPLOPEXTERIORWALL    156
#define SYS_ROOMEXTERIORWALLFLAGS   157
#define SYS_ROOMPLOPWAYPOINT        158
#define SYS_ROOMDELETEWAYPOINT      159
#define SYS_ROOMLINKWAYPOINT        160
#define SYS_ROOMUNLINKWAYPOINT      161
#define SYS_ROOMDELETEBRIDGE        162
#define SYS_ROOMPLOPTRIANGLE        163
#define SYS_ROOMPLOPFAN             164
#define SYS_ROOMDELETETRIANGLE      165
#define SYS_ROOMGROUP               166
#define SYS_ROOMOBJECT              167
#define SYS_ROOMAUTOTEXTURE         168
#define SYS_ROOMAUTOTRIM            169
#define SYS_ROOMTEXTUREFLAGS        170
#define SYS_ROOMHARDPLOPPER         171
#define SYS_ROOMCOPYPASTE           172
#define SYS_MOUSETEXT               173
#define SYS_MOUSELASTOBJECT         174
#define SYS_MOUSELASTITEM           175
#define SYS_MOUSELASTSCRIPT         176
#define SYS_BUMPABORT               177
#define SYS_MODELAUTOVERTEX         178
#define SYS_ITEMINDEX               179
#define SYS_WEAPONREFRESHXYZ        180
#define SYS_WEAPONREFRESHFLASH      181
#define SYS_WEAPONREFRESHBONENAME   182
#define SYS_FASTFUNCTION            183
#define SYS_KEEPITEM                184
#define SYS_MAKEINPUTACTIVE         185
#define SYS_FREEPARTICLE            186
#define SYS_GNOMIFYVECTOR           187
#define SYS_GNOMIFYJOINT            188
#define SYS_JOINTFROMVERTEX         189
#define SYS_ROOMWATERLEVEL          190
#define SYS_ROOMWATERTYPE           191
#define SYS_MAPSIDENORMAL           192
#define SYS_MESSAGESIZE             193
#define SYS_LASTINPUTCURSORPOS      194
#define SYS_MESSAGERESET            195
#define SYS_FASTFUNCTIONFOUND       196
#define SYS_INPUTACTIVE             197
#define SYS_LOCALPLAYERINPUT        198
#define SYS_ROOMMONSTERTYPE         199
#define SYS_SANDTEXTURE             200
#define SYS_PAYINGCUSTOMER          201
#define SYS_FILESETFLAG             202
#define SYS_ENCHANTCURSOR           203
#define SYS_CHARACTERSCRIPTFILE     204
#define SYS_PARTICLESCRIPTFILE      205
#define SYS_FLIPPAN                 206
#define SYS_MAPCLEAR                207
#define SYS_MAPROOM                 208
#define SYS_MAPAUTOMAPPRIME         209
#define SYS_MAPAUTOMAPDRAW          210
#define SYS_MAPOBJECTRECORD         211
#define SYS_MAPOBJECTDEFEATED       212
#define SYS_MAPDOOROPEN             213
#define SYS_CAMERARESET             214
#define SYS_LOCALPLAYERZ            215
#define SYS_RESPAWNCHARACTER        216
#define SYS_RESERVECHARACTER        217
#define SYS_SWAPCHARACTERS          218
#define SYS_LUCK                    219
#define SYS_ROOMTEXTURE             220
#define SYS_DAMAGECHARACTER         221
#define SYS_NETWORKSCRIPT           222
#define SYS_ROOMMETALBOXITEM        223
#define SYS_CAMERAZOOM              224
#define SYS_CAMERASPIN              225
#define SYS_ROOMRESTOCK             226
#define SYS_SAVE                    227
#define SYS_LOAD                    228
#define SYS_MODELCHECKHACK          255



// For SYS_NETWORKSCRIPT
#define NETWORK_SCRIPT_NEWLY_SPAWNED    0
#define NETWORK_SCRIPT_EXTRA_DATA       1
#define NETWORK_SCRIPT_REMOTE_INDEX     2
#define NETWORK_SCRIPT_NETLIST_INDEX    3
#define NETWORK_SCRIPT_X                4
#define NETWORK_SCRIPT_Y                5
#define NETWORK_SCRIPT_Z                6
#define NETWORK_SCRIPT_FACING           7
#define NETWORK_SCRIPT_ACTION           8
#define NETWORK_SCRIPT_TEAM             9
#define NETWORK_SCRIPT_POISON           10
#define NETWORK_SCRIPT_PETRIFY          11
#define NETWORK_SCRIPT_ALPHA            12
#define NETWORK_SCRIPT_DEFLECT          13
#define NETWORK_SCRIPT_HASTE            14
#define NETWORK_SCRIPT_OTHER_ENCHANT    15
#define NETWORK_SCRIPT_EQLEFT           16
#define NETWORK_SCRIPT_EQRIGHT          17
#define NETWORK_SCRIPT_EQCOL01          18
#define NETWORK_SCRIPT_EQCOL23          19
#define NETWORK_SCRIPT_EQSPEC1          20
#define NETWORK_SCRIPT_EQSPEC2          21
#define NETWORK_SCRIPT_EQHELM           22
#define NETWORK_SCRIPT_EQBODY           23
#define NETWORK_SCRIPT_EQLEGS           24
#define NETWORK_SCRIPT_CLASS            25
#define NETWORK_SCRIPT_MOUNT_INDEX      26





// For luck
unsigned short global_luck_timer = 0;


// For SYS_MAPROOM...
#define MAP_ROOM_SRF                  0
#define MAP_ROOM_X                    1
#define MAP_ROOM_Y                    2
#define MAP_ROOM_DOOR_X               3
#define MAP_ROOM_DOOR_Y               4
#define MAP_ROOM_DOOR_Z               5
#define MAP_ROOM_ROTATION             6
#define MAP_ROOM_SEED                 7
#define MAP_ROOM_TWSET                8
#define MAP_ROOM_LEVEL                9
#define MAP_ROOM_FLAGS               10
#define MAP_ROOM_DIFFICULTY          11
#define MAP_ROOM_AREA                12
#define MAP_ROOM_DEFEATED_CHARACTER  13
#define MAP_ROOM_NEXT_ROOM           14
#define MAP_ROOM_FROM_ROOM           15
#define MAP_ROOM_MULTI_CONNECT       16
#define MAP_ROOM_NUMBER              17
#define MAP_ROOM_CURRENT             18
#define MAP_ROOM_DOOR_PUSHBACK       19
#define MAP_ROOM_DOOR_SPIN           20
#define MAP_ROOM_LAST_TOWN           21
#define MAP_ROOM_REMOVE             128
#define MAP_ROOM_UPDATE_FLAGS       254
#define MAP_ROOM_ADD                255


#define GLOBAL_SPAWN_OWNER      0
#define GLOBAL_SPAWN_TARGET     1
#define GLOBAL_SPAWN_TEAM       2
#define GLOBAL_SPAWN_SUBTYPE    3
#define GLOBAL_SPAWN_CLASS      4


#define UPDATE_END                  1
#define UPDATE_RECOMPILE            2
#define UPDATE_SDFSAVE              3
#define UPDATE_SDFREORGANIZE        4
#define UPDATE_RELOADALLTEXTURES    5


//#define TILE_UPDATE_OPEN        0
//#define TILE_UPDATE_X           1
//#define TILE_UPDATE_Y           2
//#define TILE_UPDATE_Z           3
//#define TILE_UPDATE_CLOSE       4
//unsigned char tile_update_which = 0;



// For modeler...
#define SELECT_ALL                  0
#define SELECT_REMOVE               1
#define SELECT_SWAP                 2
#define SELECT_INVERT               3
#define SELECT_CONNECTED            4



// For Window3D
#define WIN_CAMERA  0
#define WIN_TARGET  1
#define WIN_LIGHT   2
#define WIN_ORTHO   3
#define WIN_FRUSTUM 4
#define WIN_SCALE   5



#define POOF_SELF                   0
#define POOF_TARGET                 1
#define POOF_ALL_OTHER_WINDOWS      2
#define WARN_ALL_OTHER_WINDOWS      3
#define POOF_STUCK_PARTICLES        4
#define POOF_TARGET_STUCK_PARTICLES 5
#define POOF_ALL_PARTICLES          6
#define POOF_ALL_CHARACTERS         7


#define ALPHA_NONE 0
#define ALPHA_TRANS 256
#define ALPHA_LIGHT 512

// For FileOpen
#define FILE_NORMAL                 0
#define FILE_EXPORT                 1
#define FILE_IMPORT                 2
#define FILE_DELETE                 3
#define FILE_MAKENEW                4
#define FILE_SIZE                   5

float script_point[4][2];
float script_texpoint[4][2];


signed int return_int;      // Set by run_script...  Only valid if return_int_is_set == TRUE
float return_float;         // Set by run_script...  Only valid if return_int_is_set == FALSE
#define word_temp color_temp

#define MAX_STACK 256

char system_file_name[16];
char unused_file_name[16] = "-UNUSED-";


int global_keep_item = 0;

////Xuln's Saving System////
FILE* savefile;
FILE* loadfile;
FILE* savelog;
int x, size, slot, file_index, str_length, number_of_particles;
unsigned int srf_file_pointer;
unsigned char file[15], file_name[13], file_name2[9];

//-----------------------------------------------------------------------------------------------
// <ZZ> Little function wrapper for doing an enchantment...  Assumes that all of the global
//      enchant_cursor values are set properly...
//-----------------------------------------------------------------------------------------------
void call_enchantment_function()
{
    unsigned char* backup_object_data;
    int            backup_object_item;
    if(enchant_cursor_character < MAX_CHARACTER && enchant_cursor_target < MAX_CHARACTER)
    {
        if(main_character_on[enchant_cursor_character] && main_character_on[enchant_cursor_target])
        {
            global_item_index = enchant_cursor_itemindex;
            backup_object_data = current_object_data;
            backup_object_item = current_object_item;
            fast_run_script(item_type_script[enchant_cursor_itemindex], FAST_FUNCTION_ENCHANTUSAGE, main_character_data[enchant_cursor_character]);
            current_object_data = backup_object_data;
            current_object_item = backup_object_item;
        }
    }
}

//-----------------------------------------------------------------------------------------------
// <ZZ> Autoaim helper function...
//-----------------------------------------------------------------------------------------------
#define AUTOAIM_X                0
#define AUTOAIM_Y                1
#define AUTOAIM_Z                2
#define AUTOAIM_TARGET           3
#define AUTOAIM_CRUNCH           4
#define AUTOAIM_CRUNCH_BALLISTIC 5
float autoaim_velocity_xyz[3];
unsigned short autoaim_target;
void autoaim_helper(float speed_xy, float speed_z, unsigned short spin, unsigned char team, unsigned char dexterity, unsigned short cone, unsigned char height, unsigned char function)
{
    unsigned short i;
    unsigned short random_angle;
    unsigned short left_angle;
    unsigned short right_angle;
    float* character_xyz;
    float* target_xyz;
    float* target_vel_xyz;
    float distance_xyz[3];
    float random_velocity_xyz[3];
    float precise_velocity_xyz[3];
    float left_normal_xy[2];
    float right_normal_xy[2];
    float float_angle;
    float left_dot;
    float right_dot;
    float percent;
    float inverse;
    float best_distance;
    float distance;
    float frames;
    unsigned char* target_data;


// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!! Replace all sin() and cos() calls...
// !!!BAD!!!
// !!!BAD!!!


    // First find the random velocity...  Do that by picking a random angle within the cone...
    i = random_number;  i = i<<8;  i = i | random_number;
    if(cone > 0)
    {
        random_angle = i % cone;
    }
    else
    {
        random_angle = 0;
    }
    random_angle = spin + random_angle - (cone>>1);
    // We've now got our random angle...  Convert to cartesian and scale...
    float_angle = random_angle * 0.0000958737992429f;
    random_velocity_xyz[X] = ((float) cos(float_angle)) * speed_xy;
    random_velocity_xyz[Y] = ((float) sin(float_angle)) * speed_xy;
    if(function == AUTOAIM_CRUNCH_BALLISTIC)
    {
        speed_z = speed_z * 0.01745329f;  // Speed Z specifies maximum angle (in degrees...)
        float_angle = 0.78539805f;  // 45 degrees...
        if(float_angle > speed_z) { float_angle = speed_z; }
        random_velocity_xyz[Z] = speed_xy * ((float) sin(float_angle));
        float_angle = ((float) cos(float_angle));
        random_velocity_xyz[X] *= float_angle;
        random_velocity_xyz[Y] *= float_angle;
    }
    else
    {
        random_velocity_xyz[Z] = 0.0f;
    }
    autoaim_target = MAX_CHARACTER;

    // Now find the precise angle...
    if(dexterity == 0)
    {
        // Don't do funky thing, since our dexterity is so low...
        precise_velocity_xyz[X] = 0.0f;
        precise_velocity_xyz[Y] = 0.0f;
        precise_velocity_xyz[Z] = 0.0f;
    }
    else
    {
        // Find the best target within the cone of attack...
        best_distance = 99999999.0f;
        character_xyz = (float*) current_object_data;


        // Define the cone as two planes passing through our start location...  Each
        // plane has a different x,y normal...  Target is within' cone if dot product
        // is positive for each plane...  More like a wedge than a cone...
        left_angle = spin - (cone>>1);
        float_angle = left_angle * 0.0000958737992429f;
        left_normal_xy[X] = -((float) sin(float_angle));
        left_normal_xy[Y] = ((float) cos(float_angle));

        right_angle = spin + (cone>>1);
        float_angle = right_angle * 0.0000958737992429f;
        right_normal_xy[X] = ((float) sin(float_angle));
        right_normal_xy[Y] = -((float) cos(float_angle));



        // Now go through all characters in the room, looking for a target...
        repeat(i, MAX_CHARACTER)
        {
            if(main_character_on[i])
            {
                target_data = main_character_data[i];
                if(target_data[78] != team && target_data[65] != ACTION_RIDE && (target_data[65] < ACTION_KNOCK_OUT_BEGIN || target_data[65] > ACTION_KNOCK_OUT_END || team != TEAM_GOOD) && !((*((unsigned short*) (target_data+60))) & CHAR_NO_COLLISION))
                {
                    // Looks like a valid character, but is it closer than our best one?
                    target_xyz = (float*) target_data;
                    distance_xyz[X] = target_xyz[X] - character_xyz[X];
                    distance_xyz[Y] = target_xyz[Y] - character_xyz[Y];
                    distance_xyz[Z] = target_xyz[Z] - character_xyz[Z];
                    distance = distance_xyz[X]*distance_xyz[X] + distance_xyz[Y]*distance_xyz[Y] + (distance_xyz[Z]*distance_xyz[Z]*0.2f);  // Actually distance squared, but that should be good enough for what we're doin'...
                    if(target_data[78] == TEAM_NEUTRAL)
                    {
                        // Neutral targets are not as important as enemies...
                        distance+=4.0f;
                        distance*=4.0f;
                    }
                    if((target_xyz[Z]+target_data[175]) < room_water_level && character_xyz[Z] > room_water_level)
                    {
                        // Targets below water are not important if character is above water...
                        distance = best_distance+1.0f;
                    }
                    if(distance < best_distance)
                    {
                        // It's pretty darn close, but is it within our cone?
                        left_dot = distance_xyz[X]*left_normal_xy[X] + distance_xyz[Y]*left_normal_xy[Y];
                        right_dot = distance_xyz[X]*right_normal_xy[X] + distance_xyz[Y]*right_normal_xy[Y];
                        if(left_dot > 0.0f && right_dot > 0.0f)
                        {
                            // It is...  In fact, it's our best so far...
                            autoaim_target = i;
                            best_distance = distance;
                        }
                    }
                }
            }
        }



        precise_velocity_xyz[Z] = random_velocity_xyz[Z];
        if(autoaim_target < MAX_CHARACTER)
        {
            // We found a target to aim at...
            target_data = main_character_data[autoaim_target];
            target_xyz = (float*) target_data;
            target_vel_xyz = (float*) (target_data+24);
            if(target_data[78] == TEAM_NEUTRAL)
            {
                best_distance*=0.0625;     // Neutral targets are weighted as if they were four times as far away...
            }
            distance_xyz[X] = target_xyz[X] - character_xyz[X];
            distance_xyz[Y] = target_xyz[Y] - character_xyz[Y];
            distance_xyz[Z] = target_xyz[Z] - character_xyz[Z] + ((((signed short) target_data[175])-((signed short)height))*0.5f);
            distance = (float) sqrt((distance_xyz[X]*distance_xyz[X]) + (distance_xyz[Y]*distance_xyz[Y]) + (distance_xyz[Z]*distance_xyz[Z]));


            // Figger number of frames til we hit...
            frames = 1.0f;
            if(speed_xy > 0.0f)
            {
                // Determine number of frames...
                frames = (distance / speed_xy);

                // Lead the target by that many frames...  Only lead in xy...
                distance_xyz[X] = target_xyz[X] - character_xyz[X] + (target_vel_xyz[X]*frames);
                distance_xyz[Y] = target_xyz[Y] - character_xyz[Y] + (target_vel_xyz[Y]*frames);
                distance = (float) sqrt((distance_xyz[X]*distance_xyz[X]) + (distance_xyz[Y]*distance_xyz[Y]) + (distance_xyz[Z]*distance_xyz[Z]));

                // Recalculate number of frames to hit lead position
                frames = (distance / speed_xy);
            }


            // Find the xy velocity vector...
            distance = (float) sqrt((distance_xyz[X]*distance_xyz[X]) + (distance_xyz[Y]*distance_xyz[Y]));
            if(distance > 0.0f)
            {
                distance_xyz[X]/=distance;
                distance_xyz[Y]/=distance;
            }
            precise_velocity_xyz[X] = (distance_xyz[X]*speed_xy);
            precise_velocity_xyz[Y] = (distance_xyz[Y]*speed_xy);





            // Now figger the z velocity...
            if(distance > 0.0f && speed_xy > 0.0f)
            {
                if(function == AUTOAIM_CRUNCH_BALLISTIC)
                {
                    // Simple ballistics calculation...  More complicated ones seemed to not
                    // work as well...
                    float_angle = distance_xyz[Z]/distance;
                    float_angle = ((float) atan(float_angle));
                    float_angle = float_angle + (frames*0.030f)/speed_xy;
                    if(distance_xyz[Z] < distance && float_angle > 0.78539805f)
                    {
                        // Arc at 45' for maximum range...
                        float_angle = 0.78539805f;
                    }


                    // Limit our firing angle based on the supplied argument...
                    // Also calculate the final xyz velocities...
                    if(float_angle > speed_z) { float_angle = speed_z; }
                    if(float_angle < -speed_z) { float_angle = -speed_z; }
                    precise_velocity_xyz[Z] = speed_xy*((float) sin(float_angle));
                    float_angle = (float) cos(float_angle);
                    precise_velocity_xyz[X] *= float_angle;
                    precise_velocity_xyz[Y] *= float_angle;
                }
                else
                {
                    // Simple z velocity calculation...
                    // Z velocity is z distance over number of game frames it'll take to hit...
                    precise_velocity_xyz[Z] = (distance_xyz[Z] / frames);

                    // Limit the z velocity...
                    clip(-speed_z, precise_velocity_xyz[Z], speed_z);
                }
            }
        }
        else
        {
            // No target, so just try to shoot as straight as possible...
            float_angle = spin * 0.0000958737992429f;
            precise_velocity_xyz[X] = ((float) cos(float_angle)) * speed_xy;
            precise_velocity_xyz[Y] = ((float) sin(float_angle)) * speed_xy;
            if(function == AUTOAIM_CRUNCH_BALLISTIC)
            {
                // We're arc'ing at 45' for maximum range...
                float_angle = 0.78539805f;  // 45 degrees...
                if(float_angle > speed_z) { float_angle = speed_z; }
                precise_velocity_xyz[Z] = speed_xy * ((float)sin(float_angle));
                float_angle = ((float)cos(float_angle));
                precise_velocity_xyz[X] *= float_angle;
                precise_velocity_xyz[Y] *= float_angle;
            }
        }
    }


    // Now use our dexterity to give us a weighted average between random and precise...
    // 50 dexterity is as good as it gets for aiming...
    if(dexterity > 50)
    {
        dexterity = 50;
    }
    percent = dexterity / 50.0f;
    inverse = 1.0f-percent;
    autoaim_velocity_xyz[X] = (random_velocity_xyz[X]*inverse) + (precise_velocity_xyz[X]*percent);
    autoaim_velocity_xyz[Y] = (random_velocity_xyz[Y]*inverse) + (precise_velocity_xyz[Y]*percent);
    autoaim_velocity_xyz[Z] = precise_velocity_xyz[Z];

// Don't do randomized z vel...  Makes it too hard...
//    autoaim_velocity_xyz[Z] = (random_velocity_xyz[Z]*inverse) + (precise_velocity_xyz[Z]*percent);
}

//-----------------------------------------------------------------------------------------------
// <ZZ> Emacs helper functions...
//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void emacs_copy(unsigned char* call_address)
{
    // <ZZ> Emacs helper...  Copies a line of text into the buffer
    if(emacs_buffer_write == 0) emacs_buffer_size = 0;
    if(emacs_buffer_write == 0) emacs_return_count = 0;
    while(*call_address != 0 && emacs_buffer_size < EMACS_SIZE)
    {
        emacs_buffer[emacs_buffer_size] = *call_address;
        call_address++;
        emacs_buffer_size++;
    }
    if(emacs_buffer_size < EMACS_SIZE)
    {
        emacs_buffer[emacs_buffer_size] = 0;
        emacs_buffer_size++;
        emacs_return_count++;
    }
    emacs_buffer_write = emacs_buffer_size;
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void emacs_delete(unsigned char* call_address)
{
    // <ZZ> Emacs helper...  Deletes a line of text
    unsigned char keep_going;
    keep_going = TRUE;
    while(*call_address != 0 && keep_going)
    {
        keep_going = sdf_insert_data(call_address, NULL, -1);
    }
    if(*call_address == 0 && keep_going)
    {
        sdf_insert_data(call_address, NULL, -1);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
#ifdef DEVTOOL
void emacs_paste(unsigned char* call_address)
{
    // <ZZ> Emacs helper...  Pastes contents of buffer into file
    if(emacs_buffer_size > 0)
    {
        sdf_insert_data(call_address, emacs_buffer, emacs_buffer_size);
    }
}
#endif

//-----------------------------------------------------------------------------------------------
// <ZZ> These macros are for controlling the stacks...
//-----------------------------------------------------------------------------------------------
#define show_int_stack()                                                \
{                                                                       \
    log_message("INFO:     Current integer stack...");                  \
    repeat(i, int_stack_head)                                           \
    {                                                                   \
        log_message("INFO:       %d", int_stack[int_stack_head-i-1]);   \
    }                                                                   \
}

//-----------------------------------------------------------------------------------------------
#define push_int_stack(A)                                               \
{                                                                       \
    int_stack[int_stack_head] = A;                                      \
    int_stack_head++;                                                   \
}

//-----------------------------------------------------------------------------------------------
#define pop_int_stack(A)                                                \
{                                                                       \
    int_stack_head--;                                                   \
    A = int_stack[int_stack_head];                                      \
}

//-----------------------------------------------------------------------------------------------
#define pop_int_stack_cast(A, B)                                        \
{                                                                       \
    int_stack_head--;                                                   \
    A = (B) int_stack[int_stack_head];                                  \
}

//-----------------------------------------------------------------------------------------------
#define get_nopop_int_stack(A)  { A = int_stack[(unsigned char) (int_stack_head-1)]; }
#define operate_int_stack(A, B) { int_stack[(unsigned char) (int_stack_head-1)] = int_stack[(unsigned char) (int_stack_head-1)] A B; }
#define preoperate_int_stack(A) { int_stack[(unsigned char) (int_stack_head-1)] = A (int_stack[(unsigned char) (int_stack_head-1)]); }

//-----------------------------------------------------------------------------------------------
#define show_float_stack()                                                  \
{                                                                           \
    log_message("INFO:     Current float stack...");                        \
    repeat(i, float_stack_head)                                             \
    {                                                                       \
        log_message("INFO:       %f", float_stack[float_stack_head-i-1]);   \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
#define push_float_stack(A)                                             \
{                                                                       \
    float_stack[float_stack_head] = A;                                  \
    float_stack_head++;                                                 \
}

//-----------------------------------------------------------------------------------------------
#define pop_float_stack(A)                                              \
{                                                                       \
    float_stack_head--;                                                 \
    A = float_stack[float_stack_head];                                  \
}

//-----------------------------------------------------------------------------------------------
#define pop_float_stack_cast(A, B)                                      \
{                                                                       \
    float_stack_head--;                                                 \
    A = (B) float_stack[float_stack_head];                              \
}

//-----------------------------------------------------------------------------------------------
#define get_nopop_float_stack(A)  { A = float_stack[(unsigned char) (float_stack_head-1)]; }
#define operate_float_stack(A, B) { float_stack[(unsigned char) (float_stack_head-1)] = float_stack[(unsigned char) (float_stack_head-1)] A B; }
#define preoperate_float_stack(A) { float_stack[(unsigned char) (float_stack_head-1)] = A (float_stack[(unsigned char) (float_stack_head-1)]); }

//-----------------------------------------------------------------------------------------------
// Macro for window promotion...
#define delay_promote() { promotion_buffer[promotion_count&15] = ((int) (current_object_data-main_window_data[0]))/WINDOW_SIZE;  promotion_count++; }


//-----------------------------------------------------------------------------------------------
// Stuff for 3d models in windows...
#define script_matrix_clear()                                                           \
{                                                                                       \
    script_matrix[0] = 1.0f;  script_matrix[1] = 0.0f;  script_matrix[2] = 0.0f;        \
    script_matrix[3] = 0.0f;  script_matrix[4] = 1.0f;  script_matrix[5] = 0.0f;        \
    script_matrix[6] = 0.0f;  script_matrix[7] = 0.0f;  script_matrix[8] = 1.0f;        \
    script_matrix[9] = 0.0f;  script_matrix[10] = 0.0f;  script_matrix[11] = 0.0f;      \
}
#define MODEL_BASE_FILE    256
#define MODEL_CHEST_FILE   280
#define MODEL_ARMS_FILE    304
#define MODEL_HANDS_FILE   328
#define MODEL_LEGS_FILE    352
#define MODEL_SHOES_FILE   376
#define MODEL_HEAD_FILE    400
#define MODEL_EYES_FILE    424
#define MODEL_HAIR_FILE    448
#define MODEL_MOUTH_FILE   472
#define MODEL_RIDER_FILE   496
#define MODEL_LEFT_FILE    520
#define MODEL_RIGHT_FILE   544
#define MODEL_LEFT2_FILE   568
#define MODEL_RIGHT2_FILE  592
// Data size must be 616 for full load of objects...



//-----------------------------------------------------------------------------------------------
void script_matrix_good_bone(unsigned char bone, unsigned char* data_start, unsigned char* data)
{
    // <ZZ> This function fills in script_matrix from a bone's position...  Used to
    //      attach weapons/riders to bones.  Bone is the actual bone index...  Data_start is a pointer
    //      to the bone frame block of a character (generated...  without 11 byte header...)
    //      Data is a pointer to a character's data block...
    unsigned short joint[2];
    unsigned short num_bone;
    unsigned char* base_model_data;


    // Figure out how many bones we have, and what the joints are for our bone...
    data = *((unsigned char**) (data+256));  // Data is now start of character's model data...


    // Go to base model 0...  Count bones of that one...
    data+=6;
    data+=(ACTION_MAX<<1);
    data+=(MAX_DDD_SHADOW_TEXTURE);
    base_model_data = (*((unsigned char**) data))+6;
    num_bone = *((unsigned short*) base_model_data); base_model_data+=2;
    if(bone > num_bone) bone = 0;
    data = (*((unsigned char**) (data+16)));  // Bone data...
    data = data+(bone<<3)+bone+1;
    joint[0] = *((unsigned short*) data);  data+=2;
    joint[1] = *((unsigned short*) data);


    // Skip to the bone normal data for our selected bone...
    data = data_start + (bone<<4) + (bone<<3);


    // Front normal...
    script_matrix[3] = *((float*) data);  data+=4;
    script_matrix[4] = *((float*) data);  data+=4;
    script_matrix[5] = *((float*) data);  data+=4;


    // Side normal...
    script_matrix[0] =  *((float*) data);  data+=4;
    script_matrix[1] =  *((float*) data);  data+=4;
    script_matrix[2] =  *((float*) data);  data+=4;


    // Up normal...  Calculated from joint positions...
    data_start+=(num_bone<<4) + (num_bone<<3);
    data = data_start + (joint[1]<<3) + (joint[1]<<2);
    script_matrix[6] =  *((float*) data);  data+=4;
    script_matrix[7] =  *((float*) data);  data+=4;
    script_matrix[8] =  *((float*) data);  data+=4;
    data = data_start + (joint[0]<<3) + (joint[0]<<2);
    script_matrix[9] =  *((float*) data);  data+=4;
    script_matrix[10] =  *((float*) data);  data+=4;
    script_matrix[11] =  *((float*) data);  data+=4;
    script_matrix[6] -= script_matrix[9];
    script_matrix[7] -= script_matrix[10];
    script_matrix[8] -= script_matrix[11];
}

//-----------------------------------------------------------------------------------------------
void script_matrix_from_bone(unsigned char bone_name)
{
    // <ZZ> This is a yicky function that gets a grip matrix from a given bone...  The bone frame
    //      data is assumed to be in mainbuffer...
    unsigned short i, bone;
    unsigned char* data;
    unsigned short joint[2];
    unsigned char* joint_data_start;



    data = global_bone_data;
    bone = global_num_bone;
    repeat(i, global_num_bone)
    {
        if(*data == bone_name)
        {
            bone = i;
            joint[0] = *((unsigned short*) (data+1));
            joint[1] = *((unsigned short*) (data+3));
            i = global_num_bone;
        }
        data+=9;
    }
    if(bone < global_num_bone)
    {
        data = mainbuffer + (bone<<4) + (bone<<3);


        // Front normal...
        script_matrix[3] = *((float*) data);  data+=4;
        script_matrix[4] = *((float*) data);  data+=4;
        script_matrix[5] = *((float*) data);  data+=4;


        // Side normal...
        script_matrix[0] =  *((float*) data);  data+=4;
        script_matrix[1] =  *((float*) data);  data+=4;
        script_matrix[2] =  *((float*) data);  data+=4;


        // Up normal...  Calculated from joint positions...
        joint_data_start = mainbuffer + (global_num_bone<<4) + (global_num_bone<<3);
        data = joint_data_start + (joint[1]<<3) + (joint[1]<<2);
        script_matrix[6] =  *((float*) data);  data+=4;
        script_matrix[7] =  *((float*) data);  data+=4;
        script_matrix[8] =  *((float*) data);  data+=4;
        data = joint_data_start + (joint[0]<<3) + (joint[0]<<2);
        script_matrix[9] =  *((float*) data);  data+=4;
        script_matrix[10] =  *((float*) data);  data+=4;
        script_matrix[11] =  *((float*) data);  data+=4;
        script_matrix[6] -= script_matrix[9];
        script_matrix[7] -= script_matrix[10];
        script_matrix[8] -= script_matrix[11];
    }
}

//-----------------------------------------------------------------------------------------------
float script_temp_e;
float script_temp_f;
float script_temp_y;
int   script_temp_i;
int   script_temp_j;
int   script_temp_k;
signed char run_script(unsigned char* address, unsigned char* file_start, unsigned char num_int_args, signed int* int_arg_stack, unsigned char num_float_args, float* float_arg_stack)
{
    // <ZZ> This function runs a script, starting with whatever opcode is at address.
    unsigned char opcode;
    signed int i;
    signed int j;
    signed int k;
    signed int m;
    float f;
    float e;
    signed int int_stack[MAX_STACK];
    unsigned char int_stack_head;
    float float_stack[MAX_STACK];
    unsigned char float_stack_head;
    signed char return_int_is_set;
    signed char keepgoing;
    signed int int_variable[MAX_VARIABLE];
    float float_variable[MAX_VARIABLE];
    signed int int_argument[MAX_ARGUMENT];
    float float_argument[MAX_ARGUMENT];
    unsigned char* call_address;
    unsigned char* return_address;
    unsigned char* arg_address;
    unsigned char* file_address;
    unsigned char* index;


    // Read in data from the arguments...
    repeat(i, num_int_args)
    {
        int_variable[i] = int_arg_stack[i];
    }
    repeat(i, num_float_args)
    {
        float_variable[i] = float_arg_stack[i];
    }


    // Reset the stacks...
    int_stack_head = 0;
    float_stack_head = 0;


    // Keep going until we hit a return...
    keepgoing = TRUE;
    while(keepgoing)
    {
        // Read the opcode
//show_int_stack();
//show_float_stack();
        opcode = *address;
//log_message("INFO:     Offset == %d, Opcode == 0x%02x", address-file_start, opcode);
        address++;
        // Handle what it does....
        switch(opcode)
        {
            case OPCODE_EQUALS:
                // Put top value from int stack into the given variable (1 byte extension)
                get_nopop_int_stack(i);
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Left operand is an integer variable...
                        int_variable[opcode&(MAX_VARIABLE-1)] = i;
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                (*((int*) arg_address)) = i;
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                (*((float*) arg_address)) = (float) i;
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                (*((unsigned char*) arg_address)) = (unsigned char) i;
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                (*((unsigned short*) arg_address)) = (unsigned short) i;
                                break;
                            case VAR_STRING:
                                // Copy up to 15 bytes from a string token/variable
                                call_address = (unsigned char*) i;


                                // Now read the values...
                                i = 0;
                                while(i < 15 && *call_address != 0)
                                {
                                    *arg_address = *call_address;
                                    arg_address++;
                                    call_address++;
                                    i++;
                                }
                                *arg_address = 0;
                                break;
                            case VAR_TEXT:
                                // Copy up to 63 bytes from a string token/variable
                                call_address = (unsigned char*) i;

                                // Now read the values...
                                i = 0;
                                while(i < 63 && *call_address != 0)
                                {
                                    *arg_address = *call_address;
                                    arg_address++;
                                    call_address++;
                                    i++;
                                }
                                *arg_address = 0;
                                break;
                        }
                        break;
                    case 192:
                        // Left operand is a float variable...
                        float_variable[opcode&(MAX_VARIABLE-1)] = (float) i;
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable %d for equals command", opcode);
                        break;
                }
                break;
            case OPCODE_ADD:
                // Add the top value on the stack to the next to top value
                pop_int_stack(i);
                operate_int_stack(+, i);
                break;
            case OPCODE_SUBTRACT:
                // Subtract the top value on the stack from the next to top value
                pop_int_stack(i);
                operate_int_stack(-, i);
                break;
            case OPCODE_MULTIPLY:
                // Multiply the top value on the stack with the next to top value
                pop_int_stack(i);
                operate_int_stack(*, i);
                break;
            case OPCODE_DIVIDE:
                // Divide the next to top value on the stack by the top value
                pop_int_stack(i);
                if(i != 0)
                {
                    operate_int_stack(/, i);
                }
                break;
            case OPCODE_INCREMENT:
                // Increment a variable...  1 byte extension follows...
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Integer variable...
                        i = (int_variable[opcode&(MAX_VARIABLE-1)]++);
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                i = ((*((int*) arg_address))++);
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                i = (int) ((*((float*) arg_address))++);
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                i = (int) ((*((unsigned char*) arg_address))++);
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                i = (int) ((*((unsigned short*) arg_address))++);
                                break;
                            default:
                                // No effect on strings and text...
                                log_message("ERROR:  Invalid variable for increment command");
                                i = 0;
                                break;
                        }
                        break;
                    case 192:
                        // Float variable...
                        i = (signed int) (float_variable[opcode&(MAX_VARIABLE-1)]++);
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable for increment command");
                        i = 0;
                        break;
                }
                push_int_stack(i);
                break;
            case OPCODE_DECREMENT:
                // Decrement a variable...  1 byte extension follows...
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Integer variable...
                        i = (int_variable[opcode&(MAX_VARIABLE-1)]--);
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                i = ((*((int*) arg_address))--);
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                i = (int) ((*((float*) arg_address))--);
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                i = (int) ((*((unsigned char*) arg_address))--);
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                i = (int) ((*((unsigned short*) arg_address))--);
                                break;
                            default:
                                // No effect on strings and text...
                                log_message("ERROR:  Invalid variable for decrement command");
                                i = 0;
                                break;
                        }
                        break;
                    case 192:
                        // Float variable...
                        i = (signed int) (float_variable[opcode&(MAX_VARIABLE-1)]--);
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable for decrement command");
                        i = 0;
                        break;
                }
                push_int_stack(i);
                break;
            case OPCODE_ISEQUAL:
                // Leave a one on the stack if the top two values are equal
                pop_int_stack(i);
                operate_int_stack(==, i);
                break;
            case OPCODE_ISNOTEQUAL:
                // Leave a one on the stack if the top two values are not equal
                pop_int_stack(i);
                operate_int_stack(!=, i);
                break;
            case OPCODE_ISGREATEREQUAL:
                // Leave a one on the stack if the next to top >= top value on the stack
                pop_int_stack(i);
                operate_int_stack(>=, i);
                break;
            case OPCODE_ISLESSEREQUAL:
                // Leave a one on the stack if the next to top <= top value on the stack
                pop_int_stack(i);
                operate_int_stack(<=, i);
                break;
            case OPCODE_ISGREATER:
                // Leave a one on the stack if the next to top > top value on the stack
                pop_int_stack(i);
                operate_int_stack(>, i);
                break;
            case OPCODE_ISLESSER:
                // Leave a one on the stack if the next to top < top value on the stack
                pop_int_stack(i);
                operate_int_stack(<, i);
                break;
            case OPCODE_LOGICALAND:
                // Leave a one on the stack if both of the values are positive
                pop_int_stack(i);
                operate_int_stack(&&, i);
                break;
            case OPCODE_LOGICALOR:
                // Leave a one on the stack if either of the values are positive
                pop_int_stack(i);
                operate_int_stack(||, i);
                break;
            case OPCODE_LOGICALNOT:
                // Perform a logical not of the top value on the stack
                preoperate_int_stack(!);
                break;
            case OPCODE_NEGATE:
                // Negates the top value on the stack
                preoperate_int_stack(-);
                break;
            case OPCODE_BITWISEAND:
                // AND the top value on the stack with the next to top value
                pop_int_stack(i);
                operate_int_stack(&, i);
                break;
            case OPCODE_BITWISEOR:
                // OR the top value on the stack with the next to top value
                pop_int_stack(i);
                operate_int_stack(|, i);
                break;
            case OPCODE_BITWISELEFT:
                // Shift left the next to top value on the stack by so many places
                pop_int_stack(i);
                operate_int_stack(<<, i);
                break;
            case OPCODE_BITWISERIGHT:
                // Shift left the next to top value on the stack by so many places
                pop_int_stack(i);
                operate_int_stack(>>, i);
                break;
            case OPCODE_MODULUS:
                // Divide the next to top value on the stack by the top value, Keep the remainder
                pop_int_stack(i);
                if(i != 0)
                {
                    operate_int_stack(%, i);
                }
                break;
            case OPCODE_LOCALMESSAGE:
                // Spits a string into the message buffer...
                pop_int_stack_cast(call_address, unsigned char*);  // Message string
                pop_int_stack_cast(arg_address, unsigned char*);  // Speaker name string
                message_add(call_address, arg_address, FALSE);  // No sanitize...
                push_int_stack(TRUE);
                break;
            case OPCODE_LOGMESSAGE:
                // Spits a string into the log file
                pop_int_stack_cast(call_address, unsigned char*);  // String
                log_message("%s", call_address);
                push_int_stack(TRUE);
                break;
            case OPCODE_FINDSELF:
                // Returns the address of the current object...  For property addressing...
                push_int_stack((int) current_object_data);
                break;
            case OPCODE_SYSTEMSET:
                // Write data to special system values
                pop_int_stack(m);  // New value...
                pop_int_stack(k);  // SubNumber...  like Button 5
                pop_int_stack(j);  // Number...  like Player 2
                pop_int_stack(i);  // Type...
                opcode = TRUE;
                switch(i)
                {
                    case SYS_INVALID:
                        break;
                    case SYS_PLAYERDEVICE:
                        // k is the local player number, j is the request type (DEVICE_NUMBER, DEVICE_LEFT_BUTTON, etc.)
                        // m is the new value (DEVICE_JOYSTICK_0, DEVICE_BUTTON_0, etc.)
                        // Returns FALSE if the change wasn't accepted...
                        j = j&(MAX_LOCAL_PLAYER-1);
                        if(k == PLAYER_DEVICE_TYPE)
                        {
                            // Setting the main player device type...
                            if(m >= PLAYER_DEVICE_JOYSTICK_1)
                            {
                                // Need to check if the joystick is already used...
                                repeat(i, MAX_LOCAL_PLAYER)
                                {
                                    if(i != j && player_device_type[i] == m)
                                    {
                                        opcode = FALSE;
                                    }
                                }

                                // Also check if joystick exists...
                                if((m-1) > num_joystick)
                                {
                                    // Trying to use an unconnected joystick...
                                    opcode = FALSE;
                                }
                            }
                            if(opcode)
                            {
                                // No conflicts...
                                player_device_type[j] = m;
                            }
                        }
                        else
                        {
                            // Setting one of the button bindings...
                            if(k >= 0 && k < MAX_PLAYER_DEVICE_BUTTON)
                            {
                                // Check for key conflicts for keyboard...
                                opcode = TRUE;
                                if(player_device_type[j] == PLAYER_DEVICE_KEYBOARD)
                                {
                                    repeat(script_temp_j, MAX_LOCAL_PLAYER)
                                    {
                                        if(script_temp_j != j && player_device_type[script_temp_j] == PLAYER_DEVICE_KEYBOARD)
                                        {
                                            repeat(script_temp_k, MAX_PLAYER_DEVICE_BUTTON)
                                            {
                                                if(player_device_button[script_temp_j][script_temp_k] == m)
                                                {
                                                    opcode = FALSE;
                                                    script_temp_j = MAX_LOCAL_PLAYER;
                                                    script_temp_k = MAX_PLAYER_DEVICE_BUTTON;
                                                }
                                            }
                                        }
                                    }
                                }
                                if(opcode)
                                {
                                    // No conflicts...
                                    player_device_button[j][k] = m;
                                }
                            }
                        }
                        break;
                    case SYS_WINDOWSCALE:
                        script_window_scale = (m/600.0f) + 6.0f;
                        break;
                    case SYS_TOPWINDOW:
                        // Delay window ordering effect until after all windows have been drawn...
                        delay_promote();
                        break;
                    case SYS_SFXVOLUME:
                        master_sfx_volume = m;
                        break;
                    case SYS_MUSICVOLUME:
                        master_music_volume = m;
                        break;
                    case SYS_USERLANGUAGE:
                        if(m < LANGUAGE_MAX) user_language = m;
                        if(language_file[user_language] == NULL) user_language = 0;
                        if(language_file[user_language] != NULL) user_language_phrases = (int) sdf_read_unsigned_int(language_file[user_language]);
                        else user_language_phrases = 0;
                        break;
                    case SYS_LANGUAGEFILE:
                        if(j < LANGUAGE_MAX) language_file[j] = (unsigned char*) m;
                        break;
                    case SYS_QUITGAME:
                        quitgame = (unsigned char) m;
                        break;
                    case SYS_FILEFTPFLAG:
                        if(j < sdf_num_files)
                        {
                            if(m)
                            {
                                *(index+4) |= SDF_FLAG_NO_UPDATE;
                            }
                            else
                            {
                                *(index+4) &= (~SDF_FLAG_NO_UPDATE);
                            }
                        }
                        break;
                    case SYS_CURSORDRAW:
                        mouse_draw = (unsigned char) m;
                        break;
                    #ifdef DEVTOOL
                        case SYS_MODELVIEW:
                            selection_view = (unsigned char) m;
                            break;
                        case SYS_MODELMOVE:
                            selection_move = (unsigned char) m;
                            break;
                        case SYS_MODELPLOP:
                            if(key_down[SDLK_LSHIFT] || key_down[SDLK_RSHIFT])
                            {
                                // Snapped to nearest quarter foot plop...
                                select_center_xyz[X] = ((int) (select_center_xyz[X]*4.0f)) * 0.25f;
                                select_center_xyz[Y] = ((int) (select_center_xyz[Y]*4.0f)) * 0.25f;
                                select_center_xyz[Z] = ((int) (select_center_xyz[Z]*4.0f)) * 0.25f;
                                render_insert_vertex((unsigned char*) j, (unsigned short) k, select_center_xyz, 0, MAX_VERTEX);
                            }
                            else
                            {
                                // Normal plop vertex...
                                render_insert_vertex((unsigned char*) j, (unsigned short) k, select_center_xyz, 0, MAX_VERTEX);
                            }
                            break;
                        case SYS_MODELSELECT:
                            if(j == SELECT_CONNECTED)
                            {
                                selection_close_type = BORDER_SPECIAL_SELECT_CONNECTED;
                            }
                            else if(j == SELECT_INVERT)
                            {
                                selection_close_type = BORDER_SPECIAL_SELECT_INVERT;
                            }
                            else if(j == SELECT_SWAP)
                            {
                                if(m < select_num && k < select_num)
                                {
                                    j = select_list[m];
                                    e = select_xyz[m][X];
                                    f = select_xyz[m][Y];
                                    script_temp_e = select_xyz[m][Z];
                                    call_address = (unsigned char*) select_data[m];

                                    select_list[m] = select_list[k];
                                    select_xyz[m][X] = select_xyz[k][X];
                                    select_xyz[m][Y] = select_xyz[k][Y];
                                    select_xyz[m][Z] = select_xyz[k][Z];
                                    select_data[m] = select_data[k];

                                    select_list[k] = j;
                                    select_xyz[k][X] = e;
                                    select_xyz[k][X] = f;
                                    select_xyz[k][X] = script_temp_e;
                                    select_data[k] = (float*) call_address;
                                }
                            }
                            else if(j == SELECT_REMOVE)
                            {
                                select_remove((unsigned short) m);
                            }
                            else if (j == SELECT_ALL)
                            {
                                if(m)
                                {
                                    selection_close_type = BORDER_SPECIAL_SELECT_ALL;
                                }
                                else
                                {
                                    select_clear();
                                }
                            }
                            break;
                        case SYS_MODELDELETE:
                            if(select_num > 0)
                            {
                                i = 50000;
                                while(i > 0)
                                {
                                    i--;
                                    if(select_inlist((unsigned short) i))
                                    {
                                        render_insert_vertex((unsigned char*) j, (unsigned short) k, NULL, (unsigned short) i, MAX_VERTEX);
                                    }
                                }
                                select_num = 0;
                            }
                            break;
                        case SYS_MODELPLOPTRIANGLE:
                            render_insert_triangle((unsigned char*) j, (unsigned short) k, TRUE, (unsigned char) m);
                            break;
                        case SYS_MODELDELETETRIANGLE:
                            render_insert_triangle((unsigned char*) j, (unsigned short) k, FALSE, (unsigned char) m);
                            break;
                        case SYS_MODELREGENERATE:
                            render_generate_bone_normals((unsigned char*) j, (unsigned short) k);
                            break;
                        case SYS_MODELPLOPJOINT:
                            render_insert_joint((unsigned char*) j, (unsigned short) k, select_center_xyz, 0, (unsigned char) m);
                            if(key_down[SDLK_x])
                            {
                                select_center_xyz[X]=-select_center_xyz[X];
                                render_insert_joint((unsigned char*) j, (unsigned short) k, select_center_xyz, 0, (unsigned char) m);
                                select_center_xyz[X]=-select_center_xyz[X];
                            }
                            break;
                        case SYS_MODELPLOPBONE:
                            render_insert_bone((unsigned char*) j, (unsigned short) k, select_list[0], select_list[1], TRUE, (unsigned char) m);
                            break;
                        case SYS_MODELDELETEBONE:
                            render_insert_bone((unsigned char*) j, (unsigned short) k, select_list[0], select_list[1], FALSE, 0);
                            break;
                        case SYS_MODELDELETEJOINT:
                            render_insert_joint((unsigned char*) j, (unsigned short) k, NULL, select_list[0], 0);
                            break;
                        case SYS_MODELJOINTSIZE:
                            repeat(i, select_num)
                            {
                                render_joint_size((unsigned char*) j, (unsigned short) k, select_list[i], (unsigned char) m);
                            }
                            break;
                        case SYS_MODELBONEID:
                            render_bone_id((unsigned char*) j, (unsigned short) k, select_list[0], select_list[1], (unsigned char) m);
                            break;
                        case SYS_MODELCRUNCH:
                            render_crunch_rdy((unsigned char*) j);
                            break;
                        case SYS_MODELROTATEBONES:
                            render_rotate_bones((unsigned char*) j, (unsigned short) k, (signed char) m, FALSE);
                            break;
                        case SYS_MODELUNROTATEBONES:
                            render_rotate_bones((unsigned char*) j, (unsigned short) k, (signed char) 0, TRUE);
                            break;
                        case SYS_MODELHIDE:
                            hide_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                        case SYS_MODELTRIANGLELINES:
                            triangle_lines = (triangle_lines+1)&1;
                            break;
                        case SYS_MODELCOPYPASTE:
                            if(m)
                            {
                                render_paste_selected((unsigned char*) j, (unsigned short) k, (unsigned char) (m-1));
                            }
                            else
                            {
                                render_copy_selected((unsigned char*) j, (unsigned short) k);
                            }
                            break;
                        case SYS_MODELWELDVERTICES:
                            weld_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                        case SYS_MODELWELDTEXVERTICES:
                            weld_selected_tex_vertices((unsigned char*) j, (unsigned short) k);
                            break;
                        case SYS_MODELFLIP:
                            flip_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                        case SYS_MODELSCALE:
//                            scale_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 0.95f);
//                            scale_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 0.75f);

// Temporary tools...
//                              set_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, -1.25f);
                            if(m == Z)
                            {
//                                move_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 0.0625f);
//                                move_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, -0.25f);
//                                break_anim_selected_vertices((unsigned char*) j, (unsigned short) k);


                                break_anim_joints((unsigned char*) j, (unsigned short) k);


//                                scale_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 0.86666666666666f);
//                                scale_selected_vertices_centrid((unsigned char*) j, (unsigned short) k, 0.86666666666666f);
                            }
                            else
                            {
//                                move_selected_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 1.00f);
                                if(m == X)
                                {
                                    rotate_selected_vertices((unsigned char*) j, (unsigned short) k);
                                }
                                else
                                {
                                    scale_all_joints_and_vertices((unsigned char*) j, 1.25f);
//                                    scale_selected_vertices((unsigned char*) j, (unsigned short) k, X, 0.9f);
//                                    scale_selected_vertices((unsigned char*) j, (unsigned short) k, Y, 0.9f);
//                                    scale_selected_vertices((unsigned char*) j, (unsigned short) k, Z, 0.9f);
                                }

//                                if(m == X)
//                                {
//                                    tree_rotate_selected_vertices((unsigned char*) j, (unsigned short) k, 0.24f);
//                                }
//                                else
//                                {
//                                    tree_rotate_selected_vertices((unsigned char*) j, (unsigned short) k, -0.24f);
//                                }
                            }
                            break;
                        case SYS_MODELTEXSCALE:
                            scale_selected_tex_vertices((unsigned char*) j, (unsigned short) k, (unsigned char) m, 0.984375f);
                            break;
                        case SYS_MODELANCHOR:
                            do_anchor_swap = TRUE;
                            break;
                        case SYS_MODELTEXFLAGSALPHA:
                            call_address = get_start_of_triangles((unsigned char*) j, (unsigned short) k, (unsigned char) (m>>16));
                            if(call_address)
                            {
                                if(*call_address)
                                {
                                    call_address++;
                                    *call_address = (unsigned char) (m>>8);  // Flags
                                    call_address++;
                                    *call_address = (unsigned char) (m);  // Alpha
                                }
                            }
                            break;
                        case SYS_MODELADDFRAME:
                            render_insert_frame((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            ddd_generate_model_action_list((unsigned char*) j);
                            break;
                        case SYS_MODELFRAMEFLAGS:
                            call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                            if(call_address)
                            {
                                *(call_address + 1) = (unsigned char) m;
                            }
                            break;
                        case SYS_MODELFRAMEACTIONNAME:
                            call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                            if(call_address)
                            {
                                *(call_address) = (unsigned char) m;
                                ddd_generate_model_action_list((unsigned char*) j);
                            }
                            break;
                        case SYS_MODELADDBASEMODEL:
                            render_insert_base_model((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                        case SYS_MODELFRAMEBASEMODEL:
                            render_change_frame_base_model((unsigned char*) j, (unsigned short) k, (unsigned short) m);
                            break;
                        case SYS_MODELEXTERNALFILENAME:
                            render_get_set_external_linkage((unsigned char*) j, (unsigned char*) k);
                            break;
                        case SYS_MODELSHADOWTEXTURE:
                            call_address = (unsigned char*) j;
                            call_address += 6 + (ACTION_MAX<<1) + (k&3);
                            *call_address = (unsigned char) m;
                            break;
                        case SYS_MODELSHADOWALPHA:
                            call_address = get_start_of_frame_shadows((unsigned char*) j, (unsigned short) k);
                            script_temp_j = j;
                            script_temp_k = k;
                            if(call_address)
                            {
                                script_temp_i = 0;  // Number of bytes added to bone frame...
                                repeat(j, 4)
                                {
                                    k = ((m >> (j<<2)) & 15) << 4;  // k is the new alpha for the current shadow...  0 to 240
                                    if(*call_address)
                                    {
                                        // Shadow is turned on...  Are we turning it off, or just modifying the opacity?
                                        *call_address = k;  call_address++;
                                        if(k)
                                        {
                                            // Just modifying opacity...
                                            call_address+=32;
                                        }
                                        else
                                        {
                                            // Turning it off...
                                            sdf_insert_data(call_address, NULL, -32);
                                            select_clear();
                                            script_temp_i -= 32;
                                        }
                                    }
                                    else
                                    {
                                        // Shadow is turned off...  Are we turning it on?
                                        *call_address = k;  call_address++;
                                        if(k)
                                        {
                                            // Turning it on...
                                            sdf_insert_data(call_address, NULL, 32);
                                            *((float*) call_address) = -1.3f;  call_address+=4;
                                            *((float*) call_address) = -1.3f;  call_address+=4;

                                            *((float*) call_address) =  1.3f;  call_address+=4;
                                            *((float*) call_address) = -1.3f;  call_address+=4;

                                            *((float*) call_address) =  1.3f;  call_address+=4;
                                            *((float*) call_address) =  1.3f;  call_address+=4;

                                            *((float*) call_address) = -1.3f;  call_address+=4;
                                            *((float*) call_address) =  1.3f;  call_address+=4;
                                            select_clear();
                                            k = (j<<4);
                                            select_add((unsigned short) k, (float*) (call_address-32)); k++;
                                            select_add((unsigned short) k, (float*) (call_address-24)); k++;
                                            select_add((unsigned short) k, (float*) (call_address-16)); k++;
                                            select_add((unsigned short) k, (float*) (call_address-8));
                                            script_temp_i += 32;
                                        }
                                    }
                                }

                                // Push around later bone frames...
                                if(script_temp_i != 0)
                                {
                                    k = script_temp_k;
                                    j = script_temp_j;
                                    k++;
                                    call_address = get_frame_pointer((unsigned char*) j, (unsigned short) k);
                                    while(call_address)
                                    {
                                        (*((unsigned int*) call_address))=(*((unsigned int*) call_address))+script_temp_i;
                                        k++;
                                        call_address = get_frame_pointer((unsigned char*) j, (unsigned short) k);
                                    }
                                }
                            }
                            break;
                        case SYS_MODELINTERPOLATE:
                            if(m)
                            {
                                render_shadow_reset((unsigned char*) j, (unsigned short) k);
                            }
                            else
                            {
                                render_interpolate((unsigned char*) j, (unsigned short) k);
                            }
                            break;
                        case SYS_MODELCENTER:
                            render_center_frame((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                        case SYS_MODELDETEXTURE:
                            remove_all_tex_vertex((unsigned char*) j, (unsigned short) k);
                            break;
                        case SYS_MODELPLOPATSTRING:
                            sprintf(DEBUG_STRING, "SPACE=Plop %2.2f, %2.2f, %2.2f SHIFT=Snap", select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z]);
                            break;
                        case SYS_MODELMARKFRAME:
                            render_marked_frame = (unsigned short) k;
                            break;
                        case SYS_MODELCOPYFRAME:
                            render_copy_frame((unsigned char*) j, (unsigned short) k);
                            break;
                        case SYS_MODELAUTOSHADOW:
                            render_auto_shadow((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                            break;
                    #endif
                    case SYS_WATERLAYERSACTIVE:
                        water_layers_active = (unsigned char) m;
                        break;
                    case SYS_CARTOONMODE:
                        line_mode = (unsigned char) m;
                        break;
                    case SYS_KANJICOPY:
                        #ifdef DEVTOOL
                            kanji_copy_from = j;
                        #endif
                        break;
                    case SYS_KANJIPASTE:
                        #ifdef DEVTOOL
                            if(kanji_data)
                            {
                                i = sdf_read_unsigned_short(kanji_data);
                                if(kanji_copy_from < i && j < i)
                                {
                                    // Figure out where copy from is...
                                    call_address = kanji_data + sdf_read_unsigned_int(kanji_data+2+(kanji_copy_from<<2));

                                    // Figure out where new one goes...  One we're going to write after...
                                    arg_address =  kanji_data + sdf_read_unsigned_int(kanji_data+2+(j<<2));

                                    // Determine size of copy from...
                                    i = *call_address;
                                    script_temp_i = *(call_address+(i<<1)+1);
                                    i = (i<<1) + 1 + (script_temp_i*3) + 1;


                                    // Figure out where to write...
                                    k = *arg_address;
                                    arg_address += (k<<1)+1;
                                    k = *arg_address;
                                    arg_address += (k*3) + 1;


                                    // Add a new offset block...
                                    j++;
                                    if(sdf_insert_data(kanji_data+2+(j<<2), NULL, 4))
                                    {
                                        // Bump back the pointers...
                                        call_address+=4;
                                        arg_address+=4;
                                        if(kanji_copy_from >= j)
                                        {
                                            kanji_copy_from++;
                                        }


                                        // Fill in the offset...
                                        k = arg_address-kanji_data;
                                        sdf_write_unsigned_int(kanji_data+2+(j<<2), (unsigned int) k);

                                        // Add the new kanji block...
                                        if(sdf_insert_data(arg_address, NULL, i))
                                        {
                                            // Go through all offsets, bumping them back as needed...
                                            script_temp_i = sdf_read_unsigned_short(kanji_data)+1;
                                            repeat(k, script_temp_i)
                                            {
                                                // Find offset for kurrent kanji...
                                                script_temp_j = sdf_read_unsigned_int(kanji_data+2+(k<<2));
                                                if(k < j)
                                                {
                                                    // Comes before added kanji...  Still need to bump back because we added an offset...
                                                    script_temp_j+=4;
                                                }
                                                else if(k > j)
                                                {
                                                    // Comes after added kanji...  Have to bump back for offset and data...
                                                    script_temp_j+=4+i;
                                                }
                                                if(k == kanji_copy_from)
                                                {
                                                    call_address = kanji_data+script_temp_j;
                                                }
                                                sdf_write_unsigned_int(kanji_data+2+(k<<2), (unsigned int) script_temp_j);
                                            }


                                            // Fill in the new kanji block...
                                            memcpy(arg_address, call_address, i);


                                            // Increase the kanji count...
                                            sdf_write_unsigned_short(kanji_data, (unsigned short) script_temp_i);
                                        }
                                    }
                                }
                            }
                        #endif
                        break;
                    case SYS_KANJIDELETE:
                        #ifdef DEVTOOL
                            if(kanji_data)
                            {
                                i = sdf_read_unsigned_short(kanji_data);
                                if(j < i && i > 1)
                                {
                                    // Figure out which one we're deleting...
                                    arg_address =  kanji_data + sdf_read_unsigned_int(kanji_data+2+(j<<2));

                                    // Determine size of deletion...
                                    k = *arg_address;
                                    script_temp_k = *(arg_address+(k<<1)+1);
                                    k = (k<<1) + 1 + (script_temp_k*3) + 1;


                                    // Delete the kanji block...
                                    if(sdf_insert_data(arg_address, NULL, -k))
                                    {
                                        // Delete the offset block...
                                        if(sdf_insert_data(kanji_data+2+(j<<2), NULL, -4))
                                        {
                                            // Update the number of kanji...
                                            i--;
                                            sdf_write_unsigned_short(kanji_data, (unsigned short) i);


                                            // Go through all offsets, bumping them as needed...
                                            script_temp_k = k;
                                            repeat(k, i)
                                            {
                                                // Find offset for kurrent kanji...
                                                script_temp_j = sdf_read_unsigned_int(kanji_data+2+(k<<2));
                                                if(k < j)
                                                {
                                                    // Comes before deleted kanji...  Still need to bump because we deleted an offset...
                                                    script_temp_j-=4;
                                                }
                                                else
                                                {
                                                    // Comes after deleted kanji...  Have to bump for offset and data...
                                                    script_temp_j-=4+script_temp_k;
                                                }
                                                sdf_write_unsigned_int(kanji_data+2+(k<<2), (unsigned int) script_temp_j);
                                            }
                                        }
                                    }
                                }
                            }
                        #endif
                        break;
                    case SYS_BLOCKKEYBOARD:
                        global_block_keyboard_timer = (unsigned short) m;
                        break;
                    #ifdef DEVTOOL
                        case SYS_BILLBOARDACTIVE:
                            global_billboard_active = (unsigned char) m;
                            break;
                    #endif
                    case SYS_JOINGAME:
                        // j is the continent, k is the direction, m is the letter...
// !!!BAD!!!
// !!!BAD!!!  Should probably rework for network...
// !!!BAD!!!
                        break;
                    case SYS_STARTGAME:
                        play_game_active = TRUE;
log_message("INFO:   Starting game");
                        repeat(i, MAX_LOCAL_PLAYER)
                        {
                            repeat(j, 4)
                            {
                                player_device_button_pressed[i][j] = FALSE;
                                player_device_button_unpressed[i][j] = FALSE;
                            }
                        }
                        break;
                    case SYS_LEAVEGAME:
// !!!BAD!!!
// !!!BAD!!!  Should probably rework for network...
// !!!BAD!!!
                        play_game_active = FALSE;
                        main_game_active = FALSE;
                        break;
                    case SYS_LOCALPLAYER:
                        // j is the local player number (0-3), m is the character controlled by that player...
                        j&=(MAX_LOCAL_PLAYER-1);
                        local_player_character[j] = m;
                        if(m < MAX_CHARACTER)
                        {
                            main_character_reserve_on[m] = TRUE;
                        }
                        break;
                    case SYS_SCREENSHAKE:
                        // k is the shake amount (1 - 1000), m is the shake timer (60 is 1 second)...
                        j = FALSE;  // Did we find any local players still playin'?
                        repeat(i, MAX_LOCAL_PLAYER)
                        {
                            // Is this player active?
                            if(player_device_type[i])
                            {
                                // Find this player's character
                                if(local_player_character[i] < MAX_CHARACTER)
                                {
                                    if(main_character_on[local_player_character[i]])
                                    {
                                        j=TRUE;
                                        i = MAX_LOCAL_PLAYER;
                                    }
                                }
                            }
                        }
                        if(j)
                        {
                            e = k * 0.001f;
                            if(e >= screen_shake_amount || screen_shake_timer < 1)
                            {
                                screen_shake_amount = e;
                                screen_shake_timer = m;
                            }
                        }
                        break;
                    case SYS_INCLUDEPASSWORD:
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        break;
                    case SYS_RANDOMSEED:
                        // m is the new random seed...
                        next_random = (unsigned short) m;
                        break;
                    case SYS_MIPMAPACTIVE:
                        // m is either TRUE or FALSE
                        mip_map_active = ((unsigned char) m)&1;
                        break;
                    case SYS_WATERTEXTURE:
                        // j is TRUE for shimmer, FALSE for water...
                        // k is the texture to set (0 to MAX_WATER_FRAME-1)
                        // m is the RGB file
                        if(k > -1 && k < MAX_WATER_FRAME)
                        {
                            if(j)
                            {
                                texture_shimmer[k] = *((unsigned int*) (m+2));
                            }
                            else
                            {
                                texture_water[k] = *((unsigned int*) (m+2));
                            }
                        }
                        break;
                    case SYS_PLAYERCONTROLHANDLED:
log_message("ERROR:  SYS_PLAYERCONTROLHANDLED Called...");
                        break;
                    case SYS_GLOBALSPAWN:
                        // Sets one of the global_spawn variables...
                        // k is the variable to set...
                        // m is the new value...
                        if(k == GLOBAL_SPAWN_OWNER)
                        {
                            global_spawn_owner = (unsigned short) m;
                        }
                        if(k == GLOBAL_SPAWN_TARGET)
                        {
                            global_spawn_target = (unsigned short) m;
                        }
                        if(k == GLOBAL_SPAWN_TEAM)
                        {
                            global_spawn_team = (unsigned char) m;
                        }
                        if(k == GLOBAL_SPAWN_SUBTYPE)
                        {
                            global_spawn_subtype = (unsigned char) m;
                        }
                        if(k == GLOBAL_SPAWN_CLASS)
                        {
                            global_spawn_class = (unsigned char) m;
                        }
                        break;
                    case SYS_GLOBALATTACKSPIN:
                        // Sets the global attack spin...
                        // m is the new value...
                        global_attack_spin = (unsigned short) m;
                        break;
                    case SYS_GLOBALATTACKER:
                        // Sets the global attacker...
                        // m is the new value...
                        global_attacker = (unsigned short) m;
                        break;
                    case SYS_CURRENTITEM:
                        // Sets the current item to a specific value...  Used to prevent conditional item problem in
                        // windows (click items get renumbered if something like a button is only present sometimes,
                        // can be weird...)
                        // m is the new value...
                        current_object_item = m;
                        break;
                    case SYS_ITEMREGISTRYCLEAR:
                        // Clears all item types from the item registry...
                        item_type_setup();
                        break;
                    case SYS_ITEMREGISTRYSCRIPT:
                        // Registers a new item's script...
                        // j is the item type index number...
                        // m is the script file (.RUN)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_script[j] = (unsigned char*) m;
                        break;
                    case SYS_ITEMREGISTRYICON:
                        // Registers a new item's icon...
                        // j is the item type index number...
                        // m is the image file (.RGB)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_icon[j] = (unsigned char*) m;
                        break;
                    case SYS_ITEMREGISTRYOVERLAY:
                        // Registers a new item's icon overlay...
                        // j is the item type index number...
                        // m is the image file (.RGB)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_overlay[j] = (unsigned char*) m;
                        break;
                    case SYS_ITEMREGISTRYPRICE:
                        // Registers a new item's shop price...
                        // j is the item type index number...
                        // m is the price (signed short)
                        j = (j%MAX_ITEM_TYPE);
                        item_type_price[j] = (signed short) m;
                        break;
                    case SYS_ITEMREGISTRYFLAGS:
                        // Registers a new item's flags...
                        // j is the item type index number...
                        // m is the flags (16 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_flags[j] = (unsigned short) m;
                        break;
                    case SYS_ITEMREGISTRYSTR:
                        // Registers a new item's minimum strength requirement...
                        // j is the item type index number...
                        // m is the requirement (8 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_str[j] = (unsigned char) m;
                        break;
                    case SYS_ITEMREGISTRYDEX:
                        // Registers a new item's minimum dexterity requirement...
                        // j is the item type index number...
                        // m is the requirement (8 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_dex[j] = (unsigned char) m;
                        break;
                    case SYS_ITEMREGISTRYINT:
                        // Registers a new item's minimum intelligence requirement...
                        // j is the item type index number...
                        // m is the requirement (8 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_int[j] = (unsigned char) m;
                        break;
                    case SYS_ITEMREGISTRYMANA:
                        // Registers a new item's minimum manamax requirement...
                        // j is the item type index number...
                        // m is the requirement (8 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_mana[j] = (unsigned char) m;
                        break;
                    case SYS_ITEMREGISTRYAMMO:
                        // Registers a new item's ammo number (displayed over top the item's icon)...
                        // j is the item type index number...
                        // m is the number to display (8 bits)...
                        j = (j%MAX_ITEM_TYPE);
                        item_type_ammo[j] = (unsigned char) m;
                        break;
                    case SYS_WEAPONGRIP:
                        // Sets the grip (left or right) for the weapon setup call (& weaponframeevent)...
                        // m is the grip (MODEL_LEFT_FILE or MODEL_RIGHT_FILE or MODEL_LEFT2_FILE or MODEL_RIGHT2_FILE)
                        weapon_setup_grip = (unsigned short) m;
                        break;
                    case SYS_WEAPONMODELSETUP:
                        // Calls the ModelSetup callback for the given weapon...
                        global_item_index = current_object_data[(((weapon_setup_grip-MODEL_LEFT_FILE)/24)&3)+242];
                        i = global_item_index;


                        // Clear out the current ModelAssign() thing...
                        *((unsigned int*) (current_object_data+weapon_setup_grip)) = 0;


                        // Now call that item type's ModelSetup() function...
                        if(item_type_script[i])
                        {
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(item_type_script[i], FAST_FUNCTION_MODELSETUP, current_object_data);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        break;
                    case SYS_WEAPONEVENT:
                        // Calls the Event() callback for the given weapon...
                        i = current_object_data[(((weapon_setup_grip-MODEL_LEFT_FILE)/24)&3)+242];


                        // Now call that item type's Event() function...
                        if(item_type_script[i])
                        {
                            global_item_index = (unsigned short) i;
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(item_type_script[i], FAST_FUNCTION_EVENT, current_object_data);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        break;
                    case SYS_WEAPONFRAMEEVENT:
                        // Calls the FrameEvent() callback for the given weapon...
                        i = current_object_data[(((weapon_setup_grip-MODEL_LEFT_FILE)/24)&3)+242];


                        // Now call that item type's FrameEvent() function...
                        if(item_type_script[i])
                        {
                            global_item_index = (unsigned short) i;
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(item_type_script[i], FAST_FUNCTION_FRAMEEVENT, current_object_data);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        break;
                    case SYS_WEAPONUNPRESSED:
                        // Calls the Unpressed() callback for the given weapon...
                        i = current_object_data[(((weapon_setup_grip-MODEL_LEFT_FILE)/24)&3)+242];


                        // Now call that item type's Unpressed() function...
                        if(item_type_script[i])
                        {
                            global_item_index = (unsigned short) i;
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(item_type_script[i], FAST_FUNCTION_UNPRESSED, current_object_data);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        break;
                    case SYS_CHARFASTFUNCTION:
                        // A generic way of calling any of the FAST_FUNCTION_ type of things, for any character...
                        // j is the character index (0-MAX_CHARACTER-1)
                        // m is the FAST_FUNCTION_ number thing
                        fast_function_found = FALSE;
                        if(j > -1 && j < MAX_CHARACTER)
                        {
                            m = m&254;
                            if(m < (MAX_FAST_FUNCTION<<1))
                            {
                                call_address = current_object_data;
                                script_temp_i = current_object_item;
                                looking_for_fast_function = TRUE;
                                fast_run_script(main_character_script_start[j], m, main_character_data[j]);
                                current_object_data = call_address;
                                current_object_item = script_temp_i;
                            }
                        }
                        break;
                    case SYS_FASTANDUGLY:
                        // Turns on/off bilinear filtering and stuff...
                        fast_and_ugly_active = (unsigned char) m;
                        break;
                    case SYS_DEFENSERATING:
                        // Accumulates a global defense rating in one of several damage type categories...
                        // k is the damage type
                        // m is the defense amount
                        global_defense_rating[k%MAX_DAMAGE_TYPE] += (signed char) m;
                        break;
                    case SYS_CLEARDEFENSERATING:
                        // Clears out the accumulated defense ratings...
                        repeat(m, MAX_DAMAGE_TYPE)
                        {
                            global_defense_rating[m] = 0;
                        }
                        break;
                    case SYS_ITEMDEFENSERATING:
                        // Calls the DefenseRating() function for a given item type...
                        // m is the item type
                        if(item_type_script[m&255])
                        {
                            global_item_index = (unsigned short) m;
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(item_type_script[m&255], FAST_FUNCTION_DEFENSERATING, current_object_data);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        break;
                    case SYS_CAMERAANGLE:
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Make movie record/playback function instead...
// !!!BAD!!!
// !!!BAD!!!
                        // Sets the x rotation for the camera...
                        // m is the new angle...
//                        camera_to_rotation_xy[X] = (unsigned short) m;
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                        break;
                    case SYS_STARTFADE:
                        // Starts one of the special screen fades...
                        // k is the fade type...
                        // m is the color
                        color_temp[0] = (m>>16)&255;
                        color_temp[1] = (m>>8)&255;
                        color_temp[2] = (m)&255;
                        color_temp[3] = 255;
                        if(k == 255)
                        {
                            // Special for camera...
                            display_start_fade(FADE_TYPE_FULL, FADE_OUT*16, 200.0f, 150.0f, color_temp);
                        }
                        else
                        {
                            display_start_fade((unsigned char) k, FADE_OUT, 200.0f, 150.0f, color_temp);
                        }
                        break;
                    case SYS_ROOMUNCOMPRESS:
                        // Loads the specified room into the roombuffer...
                        // j is the SRF file for the room layout
                        // k is the DDD file for walls & pillars
                        // m is the map room number (or -1 to force all doors to spawn for test room)...
                        if(m > -1 && m < num_map_room)
                        {
                            // Actual room...
                            room_uncompress((unsigned char*) j, roombuffer, (unsigned char*) k, *((unsigned short*) (map_room_data[m]+8)), (map_room_data[m]+24), (map_room_data[m]+32), map_room_data[m][30], map_room_data[m][10], map_room_data[m][10]);
                        }
                        else
                        {
                            // Test room...
                            room_uncompress((unsigned char*) j, roombuffer, (unsigned char*) k, 0, NULL, NULL, 0, 0, global_room_active_group);
                        }
                        arg_address = current_object_data;                      // Backup...
                        j = current_object_item;                                // Backup...
                        character_update_all();
                        current_object_data = arg_address;
                        current_object_item = j;
                        global_room_changed = TRUE;
                        main_timer_length = 32;  // Reset the timer to do a few frames...
                        break;
                    #ifdef DEVTOOL
                        case SYS_ROOMPLOPATSTRING:
                            sprintf(NAME_STRING, "%3.2f, %3.2f, %3.2f", select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z]);
                            break;
                        case SYS_ROOMSELECT:
                            if(j == SELECT_CONNECTED)
                            {
                                selection_close_type = BORDER_SPECIAL_SELECT_CONNECTED;
                            }
                            else if(j == SELECT_INVERT)
                            {
                                selection_close_type = BORDER_SPECIAL_SELECT_INVERT;
                            }
                            else if (j == SELECT_ALL)
                            {
                                if(m)
                                {
                                    selection_close_type = BORDER_SPECIAL_SELECT_ALL;
                                }
                                else
                                {
                                    room_select_clear();
                                }
                            }
                            break;
                        case SYS_ROOMPLOPVERTEX:
                            // j is the file, m is TRUE if you want the vertex to be selected after being plop'd...
                            room_srf_add_vertex((unsigned char*) j, select_center_xyz[X], select_center_xyz[Y], select_center_xyz[Z], (unsigned char) m);
                            break;
                        case SYS_ROOMDELETEVERTEX:
                            if(room_select_num > 0)
                            {
                                i = 50000;
                                while(i > 0)
                                {
                                    i--;
                                    if(room_select_inlist((unsigned short) i))
                                    {
                                        room_srf_delete_vertex((unsigned char*) j, (unsigned short) i);
                                    }
                                }
                                room_select_num = 0;
                                room_srf_autotexture((unsigned char*) j);
                            }
                            break;
                        case SYS_ROOMWELDVERTICES:
                            room_srf_weld_selected_vertices((unsigned char*) j);
                            break;
                        case SYS_ROOMCLEAREXTERIORWALL:
                            room_srf_clear_exterior((unsigned char*) j, 0, TRUE);
                            break;
                        case SYS_ROOMPLOPEXTERIORWALL:
                            if(room_select_num > 0)
                            {
                                k = room_select_list[room_select_num-1];
                                room_srf_add_next_exterior_wall((unsigned char*) j, (unsigned short) k);
                                room_srf_minimap_build((unsigned char*) j);
                            }
                            break;
                        case SYS_ROOMEXTERIORWALLFLAGS:
                            // j is pointer to srf data, m is new flag data...
                            if(room_select_num > 1)
                            {
                                i = room_select_list[room_select_num-2];
                                k = room_select_list[room_select_num-1];
                                room_srf_exterior_wall_flags((unsigned char*) j, (unsigned short) i, (unsigned short) k, TRUE, (unsigned char) m);
                            }
                            break;
                        case SYS_ROOMPLOPWAYPOINT:
                            room_srf_add_waypoint((unsigned char*) j, select_center_xyz[X], select_center_xyz[Y]);
                            break;
                        case SYS_ROOMDELETEWAYPOINT:
                            if(room_select_num > 0)
                            {
                                i = 50000;
                                while(i > 0)
                                {
                                    i--;
                                    if(room_select_inlist((unsigned short) i))
                                    {
                                        room_srf_delete_waypoint((unsigned char*) j, (unsigned char) i);
                                    }
                                }
                                room_select_num = 0;
                            }
                            break;
                        case SYS_ROOMLINKWAYPOINT:
                            if(room_select_num == 2)
                            {
                                room_srf_link_waypoint((unsigned char*) j, (unsigned char) room_select_list[0], (unsigned char) room_select_list[1]);
                                room_select_num = 0;
                            }
                            break;
                        case SYS_ROOMUNLINKWAYPOINT:
                            if(room_select_num == 2)
                            {
                                room_srf_unlink_waypoint((unsigned char*) j, (unsigned char) room_select_list[0], (unsigned char) room_select_list[1]);
                                room_select_num = 0;
                            }
                            break;
                        case SYS_ROOMDELETEBRIDGE:
                            if(room_select_num > 0)
                            {
                                room_srf_delete_bridge((unsigned char*) j, (unsigned char) (room_select_list[0]>>1));
                                room_select_num = 0;
                            }
                            break;
                        case SYS_ROOMPLOPTRIANGLE:
                            if(room_select_num > 2)
                            {
                                room_srf_add_texture_triangle((unsigned char*) j, (unsigned char) k, room_select_list[0], room_select_list[1], room_select_list[2], NULL, NULL, NULL);
                                room_select_num = 0;
                                room_srf_autotexture((unsigned char*) j);
                            }
                            break;
                        case SYS_ROOMPLOPFAN:
                            if(room_select_num > 2)
                            {
                                room_srf_add_texture_triangle((unsigned char*) j, (unsigned char) k, room_select_list[0], room_select_list[1], room_select_list[2], NULL, NULL, NULL);
                                i = room_select_list[0];
                                k = (int) room_select_data[0];

                                script_temp_i = room_select_list[2];
                                script_temp_k = (int) room_select_data[2];

                                room_select_num = 0;
                                room_srf_autotexture((unsigned char*) j);
                                room_select_add((unsigned short) i, (unsigned char*) k, 3);
                                room_select_add((unsigned short) script_temp_i, (unsigned char*) script_temp_k, 3);
                            }
                            break;
                        case SYS_ROOMDELETETRIANGLE:
                            if(room_select_num > 2)
                            {
                                room_srf_delete_triangle((unsigned char*) j, room_select_list[0], room_select_list[1], room_select_list[2]);
                                room_select_num = 0;
                                room_srf_autotexture((unsigned char*) j);
                            }
                            break;
                        case SYS_ROOMGROUP:
                            if(k == 0)
                            {
                                room_group_set((unsigned char*) j, (unsigned short) m);
                            }
                            else if(k == 1)
                            {
                                room_group_add((unsigned char*) j, FALSE);
                            }
                            else if(k == 2)
                            {
                                room_group_add((unsigned char*) j, TRUE);
                            }
                            break;
                        case SYS_ROOMOBJECT:
                            if(k == 0)
                            {
                                room_object_set((unsigned char*) j, (unsigned short) m);
                            }
                            else if(k == 1)
                            {
                                room_object_add((unsigned char*) j, FALSE);
                            }
                            else if(k == 2)
                            {
                                room_object_add((unsigned char*) j, TRUE);
                            }
                            break;
                        case SYS_ROOMAUTOTEXTURE:
                            room_srf_autotexture((unsigned char*) j);
                            break;
                        case SYS_ROOMAUTOTRIM:
                            if(m == 1)
                            {
                                // 1 is used to define the length of the wall...
                                autotrim_length = 0.0f;
                                autotrim_offset = 0.0f;
                                autotrim_scaling= WALL_TEXTURE_SCALE;
                                if(room_select_num > 1)
                                {
                                    repeat(k, room_select_num)
                                    {
                                        j = (k+1) % room_select_num;
                                        e = room_select_xyz[j][X] - room_select_xyz[k][X];
                                        f = room_select_xyz[j][Y] - room_select_xyz[k][Y];
                                        autotrim_length += (float) sqrt(e*e + f*f);
                                    }
                                    if(autotrim_length > 0.0f)
                                    {
                                        autotrim_scaling = ((int) (autotrim_length*WALL_TEXTURE_SCALE)) / (autotrim_length*WALL_TEXTURE_SCALE);
                                        autotrim_scaling *= WALL_TEXTURE_SCALE;
                                    }
                                }
sprintf(DEBUG_STRING, "Autotrim length == %f", autotrim_length);
                            }
                            else if(m == 5)
                            {
                                // 5 is used to do the autotrim...
                                room_srf_autotrim((unsigned char*) j);
                            }
                            else if(m > 1)
                            {
                                // 2, 3 and 4 are used to set markers...
                                room_autotrim_select_count[m-2] = room_select_num;
                            }
                            break;
                        case SYS_ROOMTEXTUREFLAGS:
                            // j is the srf file, k is the texture (0-31), m is the new value for the flags...
                            room_srf_textureflags((unsigned char*) j, (unsigned char) k, TRUE, (unsigned char) m);
                            break;
                        case SYS_ROOMHARDPLOPPER:
                            // j is the srf file, k is the width/normalization/other info (size in lower 8 bits) (normalization flag in bit 8 (256)) (base vertex ledge flag in bit 9 (512)) (randomize ledge flag in bit 10 (1024)) (ledge height in bits 11,12,13,14,15), m is the hard plopper type (0==fence, 1==stairs)
                            room_srf_hardplopper((unsigned char*) j, (unsigned char) (k&255), (unsigned char) ((k>>8)&1),(unsigned char) m, (unsigned char) ((k>>9)&1), (unsigned char) ((k>>10)&1), (unsigned char) (((k>>11)&31) + 2));
                            break;
                        case SYS_ROOMCOPYPASTE:
                            // j is the srf file, m is TRUE for paste (copy otherwise)...
                            if(m)
                            {
                                room_srf_paste((unsigned char*) j);
                            }
                            else
                            {
                                room_srf_copy((unsigned char*) j);
                            }
                            break;
                    #endif
                    case SYS_MOUSETEXT:
                        // m is a pointer to a text string...
                        strcpy(mouse_text, (unsigned char*) m);
                        break;
                    case SYS_BUMPABORT:
                        // Stops the hard coded collision stuff after a collision event...
                        global_bump_abort = TRUE;
                        break;
                    #ifdef DEVTOOL
                        case SYS_MODELAUTOVERTEX:
                            // Sets the shadow vertex to apply the auto shadow for...  (for modeler...)
                            // m is the vertex (255 for all)...
                            global_autoshadow_vertex = (unsigned char) m;
                            break;
                    #endif
                    case SYS_ITEMINDEX:
                        // Sets the index of the item that the current script is for...
                        // Used to figure out if weapons & armor are enchanted variants of main type...
                        global_item_index = (unsigned char) m;
                        break;
                    case SYS_WEAPONREFRESHXYZ:
                        // Finds a random XYZ location at which to spawn an enchantment effect
                        // particle.  Should only be called during a weapon item's Refresh() function...
                        weapon_refresh_xyz[X] = 0.0f;
                        weapon_refresh_xyz[Y] = 0.0f;
                        weapon_refresh_xyz[Z] = -500.0f;

                        if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                        {
                            i = (current_object_data-main_character_data[0])/CHARACTER_SIZE;
                            i = i & (MAX_CHARACTER-1);
                            item_find_random_xyz((unsigned short) i, global_item_bone_name);
                        }
                        break;
                    case SYS_WEAPONREFRESHFLASH:
                        // Alters the color of the current character's weapon slot, so that it flashes...
                        // Should only be called during a weapon item's Refresh() function...
                        // j is the first color...
                        // k is the second color...
                        // m is the number of clock ticks for a full color cycle...
                        if(global_item_bone_name == LEFT_BONE || global_item_bone_name == RIGHT_BONE)
                        {
                            arg_address = current_object_data + (global_item_bone_name*24) + 516;
                            if(m > 0)
                            {
                                // Flashing color
                                e = sine_table[(((main_game_frame%m)<<12)/m) & 4095];
                                e = (e + 1.0f) * 0.5f;  // Should now range from 0.0 to 1.0...
                                f = 1.0f - e;
                                m = ((unsigned char) ((j&255)*e)) + ((unsigned char) ((k&255)*f));
                                m|= (((unsigned char) (((j>>8)&255)*e)) + ((unsigned char) (((k>>8)&255)*f))) << 8;
                                m|= (((unsigned char) (((j>>16)&255)*e)) + ((unsigned char) (((k>>16)&255)*f))) << 16;
                                *((int*) arg_address) = m;
                            }
                            else
                            {
                                // Single color
                                *((int*) arg_address) = j;
                            }
                        }
                        break;
                    case SYS_FASTFUNCTION:
                        // A generic way of calling any of the FAST_FUNCTION_ type of things, for any script...
                        // j is the object data (ie. self, target, etc.)
                        // k is the script file start (ie. "FILE:MAPGEN.RUN")
                        // m is the FAST_FUNCTION_ number thing (ie. FAST_FUNCTION_SETUP)
                        m = m&254;
                        fast_function_found = FALSE;
                        if(m < (MAX_FAST_FUNCTION<<1))
                        {
                            if(((unsigned char*) j) != NULL && ((unsigned char*) k) != NULL)
                            {
                                call_address = current_object_data;
                                script_temp_i = current_object_item;
                                looking_for_fast_function = TRUE;
                                fast_run_script(((unsigned char*) k), m, ((unsigned char*) j));
                                current_object_data = call_address;
                                current_object_item = script_temp_i;
                            }
                        }
                        break;
                    case SYS_KEEPITEM:
                        // m is the new value of the global variable...
                        global_keep_item = m;
                        break;
                    case SYS_MAKEINPUTACTIVE:
                        // Makes the last WindowInput the active one...
                        last_input_object = current_object_data;
                        last_input_item = current_object_item-1;
                        input_quick_reset_key_buffer();
                        break;
                    #ifdef DEVTOOL
                        case SYS_GNOMIFYVECTOR:
                            // Sets the g'nomify working direction between two joints on a model...  m has both joints stacked in it...
                            script_temp_i = (unsigned short) (((unsigned int)m)&65535);  // joint one (start of vector)
                            script_temp_j = (unsigned short) (((unsigned int)m)>>16);    // joint two (end of vector)
                            render_gnomify_working_direction((unsigned char*) j, (unsigned short) k, (unsigned short) script_temp_i, (unsigned short) script_temp_j);
                            break;
                        case SYS_GNOMIFYJOINT:
                            // Moves a joint around by a specified amount...  m has joint number and percentage stacked in it...
                            script_temp_i = (unsigned short) (((unsigned int)m)&65535);  // joint number
                            script_temp_j = (unsigned short) (((unsigned int)m)>>16);    // percentage
                            render_gnomify_affect_joint((unsigned char*) j, (unsigned short) k, (unsigned short) script_temp_i, (unsigned char) script_temp_j);
                            break;
                        case SYS_JOINTFROMVERTEX:
                            // Moves a joint to the location of a vertex...  Assumes last joint and last vertex are used...
                            render_joint_from_vertex_location((unsigned char*) j, (unsigned short) k);
                            break;
                    #endif
                    case SYS_MESSAGESIZE:
                        // Sets the number of characters across that should be printed in the message window...
                        message_size = (unsigned short) m;
                        break;
                    case SYS_LASTINPUTCURSORPOS:
                        // Funky thing to manually adjust the cursor position for an input window (for scrolling typer-inner of message window)
                        last_input_cursor_pos = (signed char) m;
                        break;
                    case SYS_MESSAGERESET:
                        // Clears out all of the messages in the message window...
                        message_reset();
                        break;
                    case SYS_FASTFUNCTIONFOUND:
                        // m is either TRUE or FALSE...
                        fast_function_found = m;
                        break;
                    case SYS_INPUTACTIVE:
                        // Sets the input active timer...
                        input_active = (unsigned char) m;
                        break;
                    case SYS_LOCALPLAYERINPUT:
                        // Turns local player input on or off temporarily...  Used for joystick inventory access...
                        // j is the local player number (0-3), m is TRUE to turn input on, FALSE to turn it off...
                        j&=(MAX_LOCAL_PLAYER-1);
                        player_device_controls_active[j] = m;
                        break;
                    case SYS_SANDTEXTURE:
                        // m is the RGB file
                        texture_sand = *((unsigned int*) (m+2));
                        break;
                    case SYS_PAYINGCUSTOMER:
                        // m is either TRUE or FALSE
                        paying_customer = (unsigned char) m;
                        break;
                    case SYS_FILESETFLAG:
                        // j is the file name string (like "WROOMER.SRC" or something)
                        // m is the flag to set (like SDF_FLAG_WAS_UPDATED or something)
                        sdf_flag_set((unsigned char*) j, (unsigned char) m);
                        break;
                    case SYS_ENCHANTCURSOR:
                        // This function is called by an enchantment item script to setup the mouse
                        // cursor for picking the enchantment's target...  If the character using
                        // this script is an AI, then the character's self.target is used as the
                        // target for the spell and it is cast right away...  m should be the
                        // pointer to the casting character's data...  j and k are usually 0 and 0,
                        // but if they are set, j specifies the target's index and k specifies the
                        // target's item and the spell is cast right away (used that way in WSTATUS.SRC)...
                        i = MAX_CHARACTER;
                        if(m)
                        {
                            if(j == 0 && k == 0)
                            {
                                // Using normal call when spell is first used...
                                repeat(k, MAX_LOCAL_PLAYER)
                                {
                                    if(player_device_type[k] && local_player_character[k] < MAX_CHARACTER)
                                    {
                                        if(main_character_data[local_player_character[k]] == ((unsigned char*) m))
                                        {
                                            if(*((unsigned short*)(m+176)) == 0)
                                            {
                                                i = local_player_character[k];
                                                k = MAX_LOCAL_PLAYER;
                                            }
                                        }
                                        else
                                        {
                                            // Check for local helper...
                                            if(*((unsigned short*) (m + 76)) == local_player_character[k])
                                            {
                                                // The given character data is owned by this player (k)...
                                                i = (((unsigned char*) m)-main_character_data[0])/CHARACTER_SIZE;
                                                k = MAX_LOCAL_PLAYER;
                                            }
                                        }
                                    }
                                }
                                if(i < MAX_CHARACTER)
                                {
                                    // The given pointer is a reference to a local player's character data...
                                    if(enchant_cursor_active && (enchant_cursor_character == (unsigned short) i) && enchant_cursor_itemindex == global_item_index)
                                    {
                                        // Enchant mode was active and same player tried to do it again...  That means they changed
                                        // their mind and don't really want an enchantment after all...
                                        enchant_cursor_active = FALSE;
                                    }
                                    else
                                    {
                                        // Remember who's calling for the enchantment...
                                        enchant_cursor_active = TRUE;
                                        enchant_cursor_character = (unsigned short) i;
                                        enchant_cursor_itemindex = global_item_index;
                                    }
                                }
                                else
                                {
                                    // The given pointer is a reference to a non-player-character's data...
                                    // Use the AI's self.target as the target and use right away...
                                    if(((unsigned char*) m) >= main_character_data[0] && ((unsigned char*) m) <= main_character_data[MAX_CHARACTER-1])
                                    {
                                        script_temp_i = enchant_cursor_character;
                                        script_temp_j = enchant_cursor_itemindex;
                                        enchant_cursor_character = (((unsigned char*) m)-main_character_data[0])/CHARACTER_SIZE;
                                        enchant_cursor_character = enchant_cursor_character & (MAX_CHARACTER-1);
                                        enchant_cursor_itemindex = global_item_index;
                                        enchant_cursor_target = *((unsigned short*) (m+62));
                                        enchant_cursor_target_item = 0;
                                        call_enchantment_function();
                                        enchant_cursor_itemindex = script_temp_j;
                                        enchant_cursor_character = script_temp_i;
                                    }
                                }
                            }
                            else
                            {
                                // WSTATUS style call to select an inventory item...
                                if(enchant_cursor_active)
                                {
                                    enchant_cursor_target = j;
                                    enchant_cursor_target_item = k;
                                    call_enchantment_function();
                                    if(return_int)
                                    {
                                        // Revert to normal cursor if EnchantUsage() returns TRUE
                                        enchant_cursor_active = FALSE;
                                    }
                                    mouse_pressed[BUTTON0] = FALSE;
                                }
                            }
                        }
                        break;
                    case SYS_FLIPPAN:
                        // Used to tell the sound routines whether the left & right speakers are
                        // backwards or not...  If m is TRUE, they're backwards...
                        sound_flip_pan = (unsigned char) m;
                        break;
                    case SYS_MAPCLEAR:
                        // Clears the map of all rooms...
                        map_clear();
                        break;
                    case SYS_MAPROOM:
                        // Used to add a room to the map...  Called several times to specify all the parameters...
                        // k is the parameter to set, m is its value...
                        if(k == MAP_ROOM_SRF)
                        {
                            map_add_srf_file = (unsigned char*) m;
                        }
                        if(k == MAP_ROOM_X)
                        {
                            map_add_x = (float) m;
                        }
                        if(k == MAP_ROOM_Y)
                        {
                            map_add_y = (float) m;
                        }
                        if(k == MAP_ROOM_ROTATION)
                        {
                            map_add_rotation = (unsigned short) m;
                        }
                        if(k == MAP_ROOM_SEED)
                        {
                            map_add_seed = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_TWSET)
                        {
                            map_add_twset = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_LEVEL)
                        {
                            map_add_level = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_FLAGS)
                        {
                            map_add_flags = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_DIFFICULTY)
                        {
                            map_add_difficulty = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_AREA)
                        {
                            map_add_area = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_FROM_ROOM)
                        {
                            map_add_from_room = (unsigned short) m;
                        }
                        if(k == MAP_ROOM_MULTI_CONNECT)
                        {
                            map_add_multi_connect = (unsigned char) m;
                        }
                        if(k == MAP_ROOM_CURRENT)
                        {
                            map_current_room = (unsigned short) m;
                        }
                        if(k == MAP_ROOM_DOOR_PUSHBACK)
                        {
                            map_room_door_pushback = m*0.01f;
                        }
                        if(k == MAP_ROOM_LAST_TOWN)
                        {
                            map_last_town_room = (unsigned short) m;
                        }
                        if(k == MAP_ROOM_REMOVE)
                        {
                            // Remove the last room added...
                            map_remove_room();
                        }
                        if(k == MAP_ROOM_UPDATE_FLAGS)
                        {
                            // Hrmmm...  Here we're actually modifying room data, instead of
                            // buffering it for later...  j is our room number...
                            if(j < num_map_room)
                            {
                                map_room_data[j][13] = (unsigned char) m;
                            }
                        }
                        if(k == MAP_ROOM_ADD)
                        {
                            map_add_room(map_add_srf_file, map_add_x, map_add_y, map_add_rotation, map_add_seed, map_add_twset, map_add_level, map_add_flags, map_add_difficulty, map_add_area, map_add_from_room, map_add_multi_connect);
                        }
                        break;
                    case SYS_MAPAUTOMAPPRIME:
                        // Generates a list of rooms to draw for the automap...  j is the camera x position, k is
                        // the camera y position, m is the current level...
                        map_automap_prime((float) j, (float) k, (unsigned char) m, 800.0f);
                        break;
                    case SYS_MAPAUTOMAPDRAW:
                        // Draws all of the rooms for the automap...  Equivalent to calling Window3DRoom() many times,
                        // so make sure camera location & everything are specified first...
                        map_automap_draw();
                        break;
                    case SYS_MAPOBJECTRECORD:
                        // Records which room objects have been poof'd
                        if(map_current_room < num_map_room)
                        {
                            repeat(j, 8)
                            {
                                map_room_data[map_current_room][32+j] = 255;
                            }
//log_message("INFO:   Object Record list...");
                            repeat(j, MAX_CHARACTER)
                            {
                                if(main_character_on[j])
                                {
                                    k = main_character_data[j][249];
//log_message("INFO:     Object %d is on", k);
                                    if(k < 64)
                                    {
                                        opcode = FALSE;
                                        repeat(m, MAX_LOCAL_PLAYER)
                                        {
                                            opcode = opcode || (*((unsigned short*)(main_character_data[j]+76)) == local_player_character[m]);
                                        }
                                        if(main_character_data[j][78] == TEAM_GOOD && opcode)
                                        {
//log_message("INFO:       Object %d was recorded as off, because it was a TEAM_GOOD helper...", k);
                                        }
                                        else
                                        {
                                            map_room_data[map_current_room][32+(k>>3)] = map_room_data[map_current_room][32+(k>>3)] & (255 - (1<<(k&7)));
                                            main_character_data[j][249] = 255;
                                        }
                                    }
                                }
                            }
//log_message("INFO:   Object Record list final...");
                            repeat(j, 8)
                            {
//log_message("INFO:     %d", map_room_data[map_current_room][32+j]);
                            }
                        }
                        break;
                    case SYS_MAPOBJECTDEFEATED:
                        // m is the character to remove from map...  Usually not needed, unless it's to be removed without poofing (or poof comes later)...
                        if(m < MAX_CHARACTER && m >= 0)
                        {
                            main_character_data[m][249] = 255;
                        }
                        break;
                    case SYS_MAPDOOROPEN:
                        // Used to make the map remember that a door is open...
                        // m is the door number...
                        if(map_current_room < num_map_room)
                        {
                            if(m < 5 && m >= 0)
                            {
                                map_room_data[map_current_room][29] |= (1<<m);
                                j = *((unsigned short*) (map_room_data[map_current_room]+14+(m<<1)));  // The next room...
                                if(j < num_map_room)
                                {
                                    repeat(k, 5)
                                    {
                                        if(*((unsigned short*) (map_room_data[j]+14+(k<<1))) == map_current_room)
                                        {
                                            map_room_data[j][29] |= (1<<k);
                                            k = 5;
                                        }
                                    }
                                }
                            }
                        }
                        break;
                    case SYS_CAMERARESET:
                        // Makes the camera recenter on the players...
                        target_xyz[X] = 0.0f;
                        target_xyz[Y] = 0.0f;
                        target_xyz[Z] = 5.0f;
                        display_camera_position(1, 0.0f, 0.0f);
                        break;
                    case SYS_RESPAWNCHARACTER:
                        // Respawns the given character, if it was reserved...
                        // m is the character index...
                        if(m < MAX_CHARACTER && m >= 0)
                        {
                            if(main_character_reserve_on[m])
                            {
                                main_character_on[m] = TRUE;
                            }
                        }
                        break;
                    case SYS_RESERVECHARACTER:
                        // Makes a character able to be respawned...
                        // m is the character index...
                        // j is TRUE to reserve the character, FALSE to unreserve it...
                        if(m < MAX_CHARACTER && m >= 0)
                        {
                            main_character_reserve_on[m] = (unsigned char) j;
                        }
                        break;
                    case SYS_SWAPCHARACTERS:
                        // Switches one character's data with another, basically changing
                        // their slot numbers...  Used in the morph code to ensure that
                        // owners & targets stay the same when the new character is
                        // spawn'd...
                        // k and m are the character indices...
                        if(k < MAX_CHARACTER && k >= 0 && m < MAX_CHARACTER && m >= 0)
                        {
                            // Swap the data...
                            repeat(j, CHARACTER_SIZE)
                            {
                                opcode = main_character_data[k][j];
                                main_character_data[k][j] = main_character_data[m][j];
                                main_character_data[m][j] = opcode;
                            }


                            // Now the script names...
                            repeat(j, 8)
                            {
                                opcode = main_character_script_name[k][j];
                                main_character_script_name[k][j] = main_character_script_name[m][j];
                                main_character_script_name[m][j] = opcode;
                            }

                            // And the script file pointers...
                            call_address = main_character_script_start[k];
                            main_character_script_start[k] = main_character_script_start[m];
                            main_character_script_start[m] = call_address;

                            // And the on/off switch...
                            opcode = main_character_on[k];
                            main_character_on[k] = main_character_on[m];
                            main_character_on[m] = opcode;

                            // And the reserve stuff...
                            opcode = main_character_reserve_on[k];
                            main_character_reserve_on[k] = main_character_reserve_on[m];
                            main_character_reserve_on[m] = opcode;
                        }
                        break;
                    case SYS_LUCK:
                        // Sets the global luck timer...
                        global_luck_timer = (unsigned short) m;
                        break;
                    case SYS_ROOMTEXTURE:
                        // Sets one of the 32 textures being used by the currently running room...  Can change textures while running, but that'd be weird...
                        // k is the texture to set, m is a pointer to the RGB file thing...
                        // If j is TRUE it means we're doing textures for WROOMER...  DEVTOOL stuff...
                        k = k&31;
                        call_address = (unsigned char*) m;
                        if(call_address != NULL)
                        {
                            call_address+=2;
                            if(j)
                            {
                                // Working with WROOMER stuff...
                                #ifdef DEVTOOL
                                    room_editor_texture[k] = *((unsigned int*) call_address);
                                #endif
                            }
                            else
                            {
                                // Normal room texture fill in...
                                arg_address = roombuffer + (*((unsigned int*) (roombuffer+SRF_TEXTURE_OFFSET)));
                                arg_address += (k<<3);
                                *((unsigned int*) (arg_address)) = *((unsigned int*) call_address);
                            }
                        }
                        break;
                    case SYS_DAMAGECHARACTER:
                        // j is the index of the character to damage...
                        // k is (damage type<<16), (damage amount<<8), and (wound amount)
                        // m is the index of the character who's doing the damage...
                        global_attacker = m;
                        opcode = TEAM_NEUTRAL;
                        if(m < MAX_CHARACTER && m >= 0)
                        {
                            if(main_character_on[m])
                            {
                                opcode = main_character_data[m][78];  // The attacker's team;
                                if(m == j)
                                {
                                    // character is attacking itself...
                                    if(opcode == TEAM_GOOD)
                                    {
                                        opcode = TEAM_MONSTER;
                                    }
                                    else
                                    {
                                        opcode = TEAM_GOOD;
                                    }
                                }
                            }
                        }
                        damage_character((unsigned short) j, (unsigned char) (k>>16), (unsigned short) ((k>>8)&255), (unsigned short) (k&255), opcode);
                        break;
                    case SYS_CAMERAZOOM:
                        // m is the new zoom distance...
                        camera_to_distance = (float) m;
                        if(camera_to_distance < MIN_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MIN_CAMERA_DISTANCE;
                        }
                        if(camera_to_distance > MAX_CAMERA_DISTANCE)
                        {
                            camera_to_distance = MAX_CAMERA_DISTANCE;
                        }
                        break;
                    case SYS_CAMERASPIN:
                        // m is the new spin...
                        camera_rotation_xy[X] = (unsigned short) m;
                        break;
                    case SYS_ROOMRESTOCK:
                        // Restocks a random object for the given room...
                        // m is the map room number
                        if(m > -1 && m < num_map_room)
                        {
                            if(!(map_room_data[m][13] & MAP_ROOM_FLAG_BOSS))
                            {
                                room_restock_monsters(*((unsigned char**) map_room_data[m]), (map_room_data[m]+32), map_room_data[m][30], map_room_data[m][10], map_room_data[m][10]);
                            }
                        }
                        break;

                    ////Xuln's Saving System////
                    case SYS_SAVE:
                        // Saves the map and character data in the specified save slot
                        //  j is the slot number
                        slot = j;
                        sprintf(file,"SAVE%d.DAT",slot);
                        savefile = fopen(file, "wb");
                        savelog = fopen("SAVELOG.TXT", "w");
                        if(savefile)
                        {
                            fprintf(savelog,"\n\n------------\n");
                            fprintf(savelog,"***SAVING***\n");
                            fprintf(savelog,"------------\n");
                            fprintf(savelog,"SLOT: %d\n\n", slot);

                            // Save number of rooms
                            fputc(num_map_room, savefile);
                            fprintf(savelog,"Num Map Room: %d\n", num_map_room);

                            // Save current room
                            fputc(map_current_room, savefile);
                            fprintf(savelog,"Current Room: %d\n\n", map_current_room);
                            size += 2;

                            // Save Map
                            repeat(i, num_map_room)
                            {
                                fprintf(savelog,"Saving room %d:\n", i);
                                file_index = sdf_find_index_by_data((*((unsigned char**) (map_room_data[i]))));
                                if(file_index != 65535)
                                {
                                    fprintf(savelog,"   SRF FILE INDEX: %d\n", file_index);
                                    if(sdf_get_filename(file_index, file_name, NULL))
                                    {
                                        fprintf(savelog,"   SRF FILE NAME: %s\n", file_name);
                                        fwrite(file_name, 1, 8, savefile);
                                        size += 8;
                                    }
                                    else
                                    {
                                        fprintf(savelog,"   ERROR: Filename not found!\n");
                                        repeat(x, 8)
                                        {
                                            fputc('\0', savefile);
                                        }
                                    }
                                }
                                else
                                {
                                    fprintf(savelog,"   ERROR: Index not found!\n");
                                    repeat(x, 8)
                                    {
                                        fputc('\0', savefile);
                                    }
                                }
                                fwrite(map_room_data[i]+4, 1, 36, savefile);
                                fprintf(savelog,"   Wrote room %d to file\n\n", i);
                                size += 36;
                            }

                            // Save Characters
                            fprintf(savelog,"\nSaving Characters:\n");
                            fwrite(local_player_character, sizeof(local_player_character), MAX_LOCAL_PLAYER, savefile);
                            fprintf(savelog,"   Player 1 Index: %3d\n", local_player_character[0]);
                            fprintf(savelog,"   Player 2 Index: %3d\n", local_player_character[1]);
                            fprintf(savelog,"   Player 3 Index: %3d\n", local_player_character[2]);
                            fprintf(savelog,"   Player 4 Index: %3d\n\n", local_player_character[3]);
                            repeat(i, MAX_CHARACTER)
                            {
                                fwrite(main_character_script_name[i], 1, 8, savefile);
                                fputc(main_character_on[i], savefile);
                                fputc(main_character_reserve_on[i], savefile);
                                fwrite(main_character_data[i], 1, 255, savefile);
                                fprintf(savelog,"   Character %3d, Script Name: %8s, On: %d, Reserve: %d\n", i, main_character_script_name[i], main_character_on[i], main_character_reserve_on[i]);
                                size += 265;
                            }

                            // Save Particles
                            fprintf(savelog,"\nSaving Particles:\n");
                            repeat(i, MAX_PARTICLE)
                            {
                                if(main_particle_script_start[i] != NULL) {obj_get_script_name(main_particle_script_start[i], main_particle_script_name[i]);}
                                fwrite(main_particle_script_name[i], 1, 8, savefile);
                                fputc(main_particle_on[i], savefile);
                                fwrite(main_particle_data[i], 1, PARTICLE_SIZE, savefile);

                                // Convert file pointers to filenames
                                k = 44;
                                repeat(j, 2)
                                {
                                    if(*((unsigned int*) (main_particle_data[i]+k)) != 0)
                                    {
                                        file_index = sdf_find_index_by_data((*((unsigned char**) (main_particle_data[i]+k))));
                                        if(file_index != 65535)
                                        {
                                            //fprintf(savelog,"   IMAGE%d FILE INDEX: %d\n", j+1, file_index);
                                            if(sdf_get_filename(file_index, file_name, NULL))
                                            {
                                                //fprintf(savelog,"   IMAGE%d FILE NAME: %s\n", j+1, file_name);
                                                fwrite(file_name, 1, 8, savefile);
                                                size += 8;
                                            }
                                            else
                                            {
                                                //fprintf(savelog,"   ERROR: Image%d filename not found!\n", j+1);
                                                repeat(x, 8)
                                                {
                                                    fputc('\0', savefile);
                                                }
                                                // Clear string
                                                str_length = strlen(file_name);
                                                repeat(x, str_length)
                                                {
                                                file_name[x] = '\0';
                                                }
                                            }
                                        }
                                        else
                                        {
                                            if(j == 1)
                                            {
                                                //fprintf(savelog,"   Image2 index was not found... Don't worry that's normal (Most particles don't use image2)\n");
                                            }
                                            else
                                            {
                                                //fprintf(savelog,"   ERROR: Image1 index not found!\n");
                                            }
                                            repeat(x, 8)
                                            {
                                                fputc('\0', savefile);
                                            }
                                            // Clear string
                                            str_length = strlen(file_name);
                                            repeat(x, str_length)
                                            {
                                                file_name[x] = '\0';
                                            }
                                        }
                                    }
                                    else
                                    {
                                        repeat(x, 8)
                                        {
                                            fputc('\0', savefile);
                                        }
                                        // Clear string
                                        str_length = strlen(file_name);
                                        repeat(x, str_length)
                                        {
                                            file_name[x] = '\0';
                                        }

                                    }
                                    if(j == 0) {strcpy(file_name2, file_name);}
                                    k += 24;
                                }

                                fprintf(savelog,"   Particle %4d, Script Name: %8s, On: %d, Image1: %8s, Image2: %8s\n", i, main_particle_script_name[i], main_particle_on[i], file_name2, file_name);
                                size += sizeof(main_particle_data[i]) + 9;
                            }

                            // Save Extra Stuff
                            fwrite(camera_rotation_xy, sizeof(short), 1, savefile);
                            fprintf(savelog,"\nCamera Spin: %d\n", camera_rotation_xy[X]);

                            // The End
                            fclose(savefile);
                            fclose(savelog);
                            log_message("INFO:   Saving game to Slot %d...", slot);
                            log_message("INFO:   Wrote %d bytes to SAVE%d.DAT", size, slot);
                        }
                        else
                        {
                            log_message("ERROR:  Couldn't open SAVE%d.DAT", slot);
                        }
                        break;

                    case SYS_LOAD:
                        // Loads the map and character data in the specified save slot
                        //  j is the slot number
                        slot = j;
                        sprintf(file,"SAVE%d.DAT",slot);
                        loadfile = fopen(file, "rb");
                        savelog = fopen("SAVELOG.TXT", "w");
                        if(loadfile)
                        {
                            fprintf(savelog,"\n\n------------\n");
                            fprintf(savelog,"***LOADING***\n");
                            fprintf(savelog,"------------\n");
                            fprintf(savelog,"SLOT: %d\n\n", slot);

                            // Load Number of Rooms
                            num_map_room = fgetc(loadfile);
                            fprintf(savelog,"Num Map Room: %d\n", num_map_room);

                            // Load Current Room
                            map_current_room = fgetc(loadfile);
                            fprintf(savelog,"Current Room: %d\n\n", map_current_room);

                            // Load Map
                            repeat(i, num_map_room)
                            {
                                fprintf(savelog,"Loading room %d:\n", i);

                                // Clear string
                                str_length = strlen(file_name);
                                repeat(x, str_length)
                                {
                                    file_name[x] = '\0';
                                }

                                // Read SRF filename
                                fread(file_name, 1, 8, loadfile);
                                if(file_name[0] != '\0')
                                {
                                    strcat(file_name, ".SRF");
                                    fprintf(savelog,"   SRF FILE NAME: %s\n", file_name);
                                    srf_file_pointer = (unsigned int) sdf_find_index(file_name);
                                    srf_file_pointer = sdf_read_unsigned_int((unsigned char*) srf_file_pointer);
                                    (*((unsigned char**) (map_room_data[i]+0))) = (unsigned char*) srf_file_pointer;
                                    file_index = sdf_find_index_by_data((*((unsigned char**) (map_room_data[i]+0))));
                                    if(file_index != 65535)
                                    {
                                        fprintf(savelog,"   SRF FILE INDEX: %d\n", file_index);
                                    }
                                    else
                                    {
                                        fprintf(savelog,"   No index found for the specified filename.\n");
                                    }
                                }
                                fread(map_room_data[i]+4, 1, 36, loadfile);
                            }

                            // Load Characters
                            fprintf(savelog,"\nLoading Characters:\n");
                            fread(local_player_character, sizeof(local_player_character), MAX_LOCAL_PLAYER, loadfile);
                            fprintf(savelog,"   Player 1 Index: %3d\n", local_player_character[0]);
                            fprintf(savelog,"   Player 2 Index: %3d\n", local_player_character[1]);
                            fprintf(savelog,"   Player 3 Index: %3d\n", local_player_character[2]);
                            fprintf(savelog,"   Player 4 Index: %3d\n\n", local_player_character[3]);
                            repeat(i, MAX_CHARACTER)
                            {
                                fread(main_character_script_name[i], 1, 8, loadfile);
                                main_character_on[i] = fgetc(loadfile);
                                main_character_reserve_on[i] = fgetc(loadfile);
                                fread(main_character_data[i], 1, 255, loadfile);
                                fprintf(savelog,"   Character %d, Script Name: %s, On: %d, Reserve: %d\n", i, main_character_script_name[i], main_character_on[i], main_character_reserve_on[i]);

                                // Find the script start file pointer
                                index = sdf_find_filetype(main_character_script_name[i], SDF_FILE_IS_RUN);
                                if(index)
                                {
                                    main_character_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
                                }

                                // Clear out model assigns...
                                x = 256;
                                while(x < 616)
                                {
                                    *((unsigned char**)(main_character_data[i]+x)) = NULL;
                                    x+=24;
                                }

                                // Setup Model
                                call_address = current_object_data;
                                script_temp_i = current_object_item;
                                looking_for_fast_function = TRUE;
                                fast_run_script(main_character_script_start[i], FAST_FUNCTION_MODELSETUP, main_character_data[i]);
                                current_object_data = call_address;
                                current_object_item = script_temp_i;
                            }

                            // Load Particles
                            fprintf(savelog,"\nLoading Particles:\n");
                            repeat(i,MAX_PARTICLE)
                            {
                                fread(main_particle_script_name[i], 1, 8, loadfile);
                                main_particle_on[i] = fgetc(loadfile);
                                fread(main_particle_data[i], 1, PARTICLE_SIZE, loadfile);

                                // Clear strings
                                str_length = strlen(file_name);
                                repeat(x, str_length)
                                {
                                    file_name[x] = '\0';
                                }
                                str_length = strlen(file_name2);
                                repeat(x, str_length)
                                {
                                    file_name2[x] = '\0';
                                }

                                // Load image1
                                fread(file_name, 1, 8, loadfile);
                                if(file_name[0] != '\0')
                                {
                                    index = sdf_find_filetype(file_name, SDF_FILE_IS_RGB);
                                    if(index)
                                    {
                                        *((unsigned int**) (main_particle_data[i]+44)) = (unsigned int*) sdf_read_unsigned_int(index);
                                    }
                                }

                                // Load image2
                                fread(file_name2, 1, 8, loadfile);
                                if(file_name2[0] != '\0')
                                {
                                    index = sdf_find_filetype(file_name2, SDF_FILE_IS_RGB);
                                    if(index)
                                    {
                                        *((unsigned int**) (main_particle_data[i]+68)) = (unsigned int*) sdf_read_unsigned_int(index);
                                    }
                                }

                                fprintf(savelog,"   Particle %4d, Script Name: %8s, On: %d, Image1: %8s, Image2: %8s\n", i, main_particle_script_name[i], main_particle_on[i], file_name, file_name2);

                                // Find the script start file pointer
                                index = sdf_find_filetype(main_particle_script_name[i], SDF_FILE_IS_RUN);
                                if(index)
                                {
                                    main_particle_script_start[i] = (unsigned char*) sdf_read_unsigned_int(index);
                                }
                            }

                            // Load Extra Stuff
                            fread(camera_rotation_xy, sizeof(short), 1, loadfile);
                            fprintf(savelog,"   Camera Spin: %hd\n", camera_rotation_xy[X]);

                            // Finish Up
                            fclose(loadfile);
                            fclose(savelog);
                            log_message("INFO:   Loading game from Slot %d", slot);
                        }
                        else
                        {
                            log_message("ERROR:  Couldn't open SAVE%d.DAT", slot);
                        }
                        break;
                    ////End of Xuln's Saving System////

                    case SYS_MODELCHECKHACK:
#ifdef DEVTOOL
                        log_message("INFO:   Model Check Hack...");
                        j = 256;
                        repeat(i, 15)
                        {
                            k = *((int*)(current_object_data+j));
                            if(sdf_find_index_by_data((unsigned char*) k) == 65535)
                            {
                                log_message("ERROR:    No match found for RDY slot %d (%x == %u)", i, (current_object_data+j), k);
                            }
                            j+=24;
                        }
#endif
                        break;
                    default:
                        break;
                }
                push_int_stack(opcode);
                break;
            case OPCODE_SYSTEMGET:
                // Returns a requested value
                pop_int_stack(k);  // SubNumber...  like Button 5
                pop_int_stack(j);  // Number...  like Cursor 2
                pop_int_stack(i);  // Type...
                switch(i)
                {
                    case SYS_INVALID:
                        i = TRUE;
                        break;
                    case SYS_PLAYERDEVICE:
                        // k is the local player number, j is the request type (DEVICE_NUMBER, DEVICE_LEFT_BUTTON, etc.)
                        j = j&(MAX_LOCAL_PLAYER-1);
                        if(k == PLAYER_DEVICE_TYPE)
                        {
                            // Current device...
                            i = player_device_type[j];
                        }
                        else
                        {
                            // Current button bindings...
                            if(k >= 0 && k < MAX_PLAYER_DEVICE_BUTTON)
                            {
                                i = player_device_button[j][k];
                            }
                        }
                        break;
                    case SYS_NUMJOYSTICK:
                        i = num_joystick;
                        break;
                    case SYS_WINDOWSCALE:
                        i = (int) ((script_window_scale-6.0f) * 600.0f);
                        break;
                    case SYS_KEYPRESSED:
                        i = FALSE;
                        if(j < MAX_KEY && input_active == 0)
                        {
                            i = window_key_pressed[j];
                        }
                        break;
                    case SYS_KEYDOWN:
                        i = FALSE;
                        if(j < MAX_KEY && input_active == 0)
                        {
                            i = key_down[j];
                        }
                        break;
                    case SYS_TOPWINDOW:
                        i = main_used_window_count;
                        if(i > 0)
                        {
                            i = main_window_order[main_used_window_count-1];
                            i = (main_window_data[i] == current_object_data);
                        }
                        break;
                    case SYS_SFXVOLUME:
                        i = master_sfx_volume;
                        break;
                    case SYS_MUSICVOLUME:
                        i = master_music_volume;
                        break;
                    case SYS_MESSAGE:
                        i = (signed int) (message_get(j));
                        break;
                    case SYS_USERLANGUAGE:
                        i = user_language;
                        break;
                    case SYS_FILENAME:
                        if(j < sdf_num_files)
                        {
                            opcode = (*(index+4)&15);
                            if(opcode != SDF_FILE_IS_UNUSED)
                            {
                                i = 0;
                                while(i < 8 && sdf_index[(j<<4)+8+i] != 0)
                                {
                                    system_file_name[i] = sdf_index[(j<<4)+8+i];
                                    i++;
                                }
                                system_file_name[i] = '.';
                                i++;
                                repeat(k, 4)
                                {
                                    system_file_name[i] = sdf_extension[opcode][k];
                                    i++;
                                }
                                i = (int) system_file_name;
                            }
                            else
                            {
                                i = (int) unused_file_name;
                            }
                        }
                        else
                        {
                            system_file_name[0] = 0;
                            i = (int) system_file_name;
                        }
                        break;
                    case SYS_FILESIZE:
                        if(j < sdf_num_files)
                        {
                            index = sdf_index + (j<<4);
                            opcode = (*(index+4)&15);
                            if(opcode != SDF_FILE_IS_UNUSED)
                            {
                                k = sdf_read_unsigned_int(index+4)&0x00FFFFFF;
                                sprintf(system_file_name, "%8d", k);
                                i = (int) system_file_name;
                            }
                            else
                            {
                                sprintf(system_file_name, "%8d", 0);
                                i = (int) system_file_name;
                            }
                        }
                        else
                        {
                            system_file_name[0] = 0;
                            i = (int) system_file_name;
                        }
                        break;
                    case SYS_FILEFTPFLAG:
                        i = FALSE;
                        if(j < sdf_num_files)
                        {
                            opcode = *(index+4);
                            if((opcode&15) != SDF_FILE_IS_UNUSED)
                            {
                                i = ((opcode&SDF_FLAG_NO_UPDATE) == SDF_FLAG_NO_UPDATE);
                            }
                        }
                        break;
                    case SYS_FILECOUNT:
                        i = sdf_num_files;
                        break;
                    case SYS_FILEFREE:
                        i = sdf_extra_files;
                        break;
                    case SYS_DEVTOOL:
                        #ifdef DEVTOOL
                            i = TRUE;
                        #else
                            i = FALSE;
                        #endif
                        break;
                    case SYS_CURSORPOS:
                        if(k == X)
                        {
                            e = mouse_x - script_window_x;
                        }
                        else
                        {
                            e = mouse_y - script_window_y;
                        }
                        if(script_window_scale > 0)
                        {
                            i = (int) (e * 100.0f / script_window_scale);
                        }
                        else
                        {
                            i = 0;
                        }
                        break;
                    case SYS_CURSORBUTTONDOWN:
                        k = k&(MAX_MOUSE_BUTTON-1);
                        i = mouse_down[k];
                        break;
                    #ifdef DEVTOOL
                        case SYS_MODELSELECT:
                            i = select_num;
                            break;
                        case SYS_MODELTEXFLAGSALPHA:
                            call_address = get_start_of_triangles((unsigned char*) j, (unsigned short) k, (unsigned char) (k>>16));
                            i = 0;
                            if(call_address)
                            {
                                if(*call_address)
                                {
                                    call_address++;
                                    i = *call_address;  // Flags
                                    i = i << 8;
                                    call_address++;
                                    i += *call_address;  // Alpha
                                }
                            }
                            break;
                    #endif
                    case SYS_MODELMAXFRAME:
                        i = *((unsigned short*) (j+4));
                        break;
                    case SYS_MODELFRAMEFLAGS:
                        call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                        i = 0;
                        if(call_address)
                        {
                            i = *(call_address + 1);
                        }
                        break;
                    case SYS_MODELFRAMEACTIONNAME:
                        call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                        i = 0;
                        if(call_address)
                        {
                            i = *(call_address);
                        }
                        break;
                    case SYS_MODELACTIONSTART:
                        k = k&255;
                        i = 65535;
                        if(k < ACTION_MAX)
                        {
                            call_address = (unsigned char*) j;
                            if(call_address == NULL)
                            {
                                call_address = (unsigned char*)  (*((unsigned int*) (current_object_data+256)));
                            }
                            call_address+=6;
                            i = ((unsigned short*) call_address)[k];
                        }
                        break;
                    #ifdef DEVTOOL
                        case SYS_MODELFRAMEBASEMODEL:
                            i = render_change_frame_base_model((unsigned char*) j, (unsigned short) k, 65535);
                            break;
                        case SYS_MODELFRAMEOFFSETX:
                            call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                            i = 0;
                            if(call_address)
                            {
                                i = (signed int) (*((float*) (call_address+3)) * 256.0f);
                            }
                            break;
                        case SYS_MODELFRAMEOFFSETY:
                            call_address = get_start_of_frame((unsigned char*) j, (unsigned short) k);
                            i = 0;
                            if(call_address)
                            {
                                i = (signed int) (*((float*) (call_address+7)) * 256.0f);
                            }
                            break;
                        case SYS_MODELEXTERNALFILENAME:
                            i = (signed int) render_get_set_external_linkage((unsigned char*) j, NULL);
                            break;
                        case SYS_MODELSHADOWTEXTURE:
                            call_address = (unsigned char*) j;
                            call_address += 6 + (ACTION_MAX<<1) + (k&3);
                            i = *call_address;
                            break;
                        case SYS_MODELSHADOWALPHA:
                            call_address = get_start_of_frame_shadows((unsigned char*) j, (unsigned short) k);
                            i = 0;
                            if(call_address)
                            {
                                repeat(j, 4)
                                {
                                    if(*call_address)
                                    {
                                        i |= (*call_address>>4) << (j<<2);
                                        call_address+=33;
                                    }
                                    else
                                    {
                                        call_address++;
                                    }
                                }
                            }
                            break;
                        case SYS_MODELNUMTRIANGLE:
                            i = 0;
                            repeat(m, 4)
                            {
                                call_address = get_start_of_triangles((unsigned char*) j, (unsigned short) k, (unsigned char) m);
                                if(call_address)
                                {
                                    if(*call_address)
                                    {
                                        call_address+=3;
                                        i += *((unsigned short*) call_address);
                                    }
                                }
                            }
                            break;
                        case SYS_MODELNUMCARTOONLINE:
                            i = 0;
                            call_address = get_start_of_triangles((unsigned char*) j, (unsigned short) k, (unsigned char) 4);
                            if(call_address)
                            {
                                i = *((unsigned short*) call_address);
                            }
                            break;
                    #endif
                    case SYS_MODELSARELINKABLE:
                        i = (get_number_of_bones((unsigned char*) j) == get_number_of_bones((unsigned char*) k));
                        break;
                    case SYS_LASTKEYPRESSED:
                        i = last_key_pressed;
                        last_key_pressed = 0;
                        break;
                    case SYS_CURSORLASTPOS:
                        if(k == X)
                        {
                            e = mouse_last_x - script_window_x;
                        }
                        else
                        {
                            e = mouse_last_y - script_window_y;
                        }
                        if(script_window_scale > 0)
                        {
                            i = (int) (e * 100.0f / script_window_scale);
                        }
                        else
                        {
                            i = 0;
                        }
                        break;
                    case SYS_CURSORINOBJECT:
                        i = (mouse_last_object == current_object_data);
                        break;
                    case SYS_NUMKANJI:
                        i = 0;
                        if(kanji_data)
                        {
                            i = kanji_data[0];  i<<=8;
                            i+= kanji_data[1];
                        }
                        break;
                    case SYS_NUMKANJITRIANGLE:
                        i = 0;
                        if(kanji_data)
                        {
                            i = sdf_read_unsigned_short(kanji_data);
                            if(j < i)
                            {
                                call_address = kanji_data + 2 + (j<<2);
                                j = sdf_read_unsigned_int(call_address);
                                call_address = kanji_data+j;
                                j = *call_address;  call_address++;
                                call_address+=j<<1;
                                i = *call_address;
                            }
                            else
                            {
                                i = 0;
                            }
                        }
                        break;
                    case SYS_CURSORSCREENPOS:
                        if(k == X)
                        {
                            i = (int) mouse_x;
                        }
                        else
                        {
                            i = (int) mouse_y;
                        }
                        break;
                    case SYS_DAMAGEAMOUNT:
                        i = global_damage_amount;
                        break;
                    case SYS_COLLISIONCHAR:
                        i = global_bump_char;
                        break;
                    case SYS_MAINVIDEOFRAME:
                        i = main_video_frame;
                        break;
                    case SYS_MAINGAMEFRAME:
                        i = main_game_frame;
                        break;
                    case SYS_MAINFRAMESKIP:
                        i = main_frame_skip;
                        break;
                    case SYS_FINDDATASIZE:
                        // k is the start of a file's data...
                        call_address = (unsigned char*) k;
                        arg_address = sdf_index;
                        i = 0;
                        repeat(j, sdf_num_files)
                        {
                            if((arg_address[4] & 15) != SDF_FILE_IS_UNUSED)
                            {
                                if(call_address == (unsigned char*)  sdf_read_unsigned_int(arg_address))
                                {
                                    j = sdf_num_files;
                                    i = sdf_read_unsigned_int(arg_address+4)&0x00FFFFFF;
                                }
                            }
                            arg_address+=16;
                        }
                        break;
                    case SYS_ARCTANANGLE:
                        // Return the arctangent angle of the given x, y coordinates
// !!!BAD!!!
// !!!BAD!!!  Better way of doin' this...
// !!!BAD!!!
                        i = (int) ((((float) atan2(-j, k)) + PI) * (180/PI));
                        break;
                    case SYS_MAINSERVERLOCATED:
// !!!BAD!!!
// !!!BAD!!! Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_SHARDLIST:
                        // j is continent, k is direction
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_SHARDLISTPING:
                        // k is the letter...
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_SHARDLISTPLAYERS:
                        // k is the letter...
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_VERSIONERROR:
                        // k is the subrequest...
                        //   0 is do we have an error?
                        //   1 is what executable version is required?
                        //   2 is what data version is required?
                        i = global_version_error;
                        if(k == 1)
                        {
                            i = required_executable_version;
                        }
                        if(k == 2)
                        {
                            i = required_data_version;
                        }
                        break;
                    case SYS_STARTGAME:
                        i = play_game_active;
                        break;
                    case SYS_NETWORKON:
                        i = network_on;
                        break;
                    case SYS_MAINGAMEACTIVE:
                        i = main_game_active;
                        break;
                    case SYS_NETWORKGAMEACTIVE:
// !!!BAD!!!
// !!!BAD!!!  Stupid
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_TRYINGTOJOINGAME:
// !!!BAD!!!
// !!!BAD!!!  Stupid
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_JOINPROGRESS:
// !!!BAD!!!
// !!!BAD!!!  Stupid
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_GAMESEED:
// !!!BAD!!!
// !!!BAD!!!  Stupid...  Change to map_seed...
// !!!BAD!!!
                        i = game_seed;
                        break;
                    case SYS_LOCALPASSWORDCODE:
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_NUMNETWORKPLAYER:
                        i = num_remote;
                        break;
                    case SYS_NETWORKFINISHED:
                        // Only returns TRUE if all the network handling is done...
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = TRUE;
                        break;
                    case SYS_SERVERSTATISTICS:
// !!!BAD!!!
// !!!BAD!!!  Stupid...
// !!!BAD!!!
                        i = 0;
                        break;
                    case SYS_LOCALPLAYER:
                        // j is the local player (0-3)...  Return value needn't be a valid character...
                        j&=(MAX_LOCAL_PLAYER-1);
                        i = local_player_character[j];
                        break;
                    case SYS_FPS:
                        // Return the Frames per second...
                        i = (int) main_timer_fps;
                        break;
                    case SYS_RANDOMSEED:
                        // Return the random seed...
                        i = next_random;
                        break;
                    case SYS_GLOBALATTACKSPIN:
                        // Return the global attack spin...
                        i = global_attack_spin;
                        break;
                    case SYS_GLOBALATTACKER:
                        // Return whoever did damage...
                        i = global_attacker;
                        break;
                    case SYS_DEBUGACTIVE:
                        // Returns whether debug stuff is on or off...
                        #ifdef DEVTOOL
                            i = debug_active;
                        #else
                            i = FALSE;
                        #endif
                        break;
                    case SYS_DAMAGECOLOR:
                        // Returns the color of a specific damage type (for the little numbers that pop up)...
                        // k is the damage type...
                        k = k%(MAX_DAMAGE_TYPE);
                        i = (damage_color_rgb[k][0] << 16) + (damage_color_rgb[k][1] << 8) + (damage_color_rgb[k][2]);
                        break;
                    case SYS_ITEMREGISTRYSCRIPT:
                        // Returns an item's script...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = (unsigned int) item_type_script[j];
                        break;
                    case SYS_ITEMREGISTRYICON:
                        // Returns an item's icon image...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = (unsigned int) item_type_icon[j];
                        break;
                    case SYS_ITEMREGISTRYOVERLAY:
                        // Returns an item's overlay image...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = (unsigned int) item_type_overlay[j];
                        break;
                    case SYS_ITEMREGISTRYPRICE:
                        // Returns an item's shop price...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_price[j];
                        break;
                    case SYS_ITEMREGISTRYFLAGS:
                        // Returns an item's flags...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_flags[j];
                        break;
                    case SYS_ITEMREGISTRYSTR:
                        // Returns an item's minimum strength requirement...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_str[j];
                        break;
                    case SYS_ITEMREGISTRYDEX:
                        // Returns an item's minimum dexterity requirement...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_dex[j];
                        break;
                    case SYS_ITEMREGISTRYINT:
                        // Returns an item's minimum intelligence requirement...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_int[j];
                        break;
                    case SYS_ITEMREGISTRYMANA:
                        // Returns an item's minimum manamax requirement...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_mana[j];
                        break;
                    case SYS_ITEMREGISTRYAMMO:
                        // Returns an item's ammo number (to be displayed over the item's icon)...
                        // j is the item type index number...
                        j = (j%MAX_ITEM_TYPE);
                        i = item_type_ammo[j];
                        break;
                    case SYS_ITEMREGISTRYNAME:
                        // Runs the GetName() function for the given item type...
                        // j is the item type index number...
                        global_item_index = (unsigned short) j;
                        i = item_get_type_name((unsigned short) j);
                        break;
                    case SYS_WEAPONGRIP:
                        // Gets the grip (left or right) for the weapon setup call...
                        i = weapon_setup_grip;
                        break;
                    case SYS_DEFENSERATING:
                        // Returns the accumulated amount for a given damage type...
                        // k is the damage type
                        i = global_defense_rating[k%MAX_DAMAGE_TYPE];
                        break;
                    case SYS_CURSORBUTTONPRESSED:
                        // Returns the cursor button pressed state for a given button
                        // k is the button...
                        k = k&(MAX_MOUSE_BUTTON-1);
                        i = mouse_pressed[k];
                        break;
                    case SYS_MEMBUFFER:
                        // Returns a pointer to the given buffer...
                        // k is the buffer number...
                        if(k < 2)
                        {
                            i = (unsigned int) mainbuffer;
                        }
                        if(k == 2)
                        {
                            i = (unsigned int) subbuffer;
                        }
                        if(k == 3)
                        {
                            i = (unsigned int) thirdbuffer;
                        }
                        if(k == 4)
                        {
                            i = (unsigned int) fourthbuffer;
                        }
                        if(k > 4)
                        {
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
log_message("ERROR:  Membuffer MAPBUFFER requested...");
                            i = (unsigned int) 0;
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                        }
                        break;
                    #ifdef DEVTOOL
                        case SYS_ROOMSELECT:
                            i = room_select_num;
                            break;
                        case SYS_ROOMEXTERIORWALLFLAGS:
                            // j is pointer to srf data...
                            i = 0;
                            if(room_select_num > 1)
                            {
                                i = room_select_list[room_select_num-2];
                                k = room_select_list[room_select_num-1];
                                i = room_srf_exterior_wall_flags((unsigned char*) j, (unsigned short) i, (unsigned short) k, FALSE, 0);
                            }
                            break;
                        case SYS_ROOMGROUP:
                            i = global_room_active_group;
                            break;
                        case SYS_ROOMOBJECT:
                            if(k == 0)
                            {
                                i = global_room_active_object;
                            }
                            else
                            {
                                i = (int) global_room_active_object_data;
                            }
                            break;
                        case SYS_ROOMTEXTUREFLAGS:
                            // j is the srf file, k is the texture (0-31)...
                            i = room_srf_textureflags((unsigned char*) j, (unsigned char) k, FALSE, 0);
                            break;
                    #endif
                    case SYS_MOUSELASTOBJECT:
                        i = (int) mouse_last_object;
                        break;
                    case SYS_MOUSELASTITEM:
                        i = (int) mouse_last_item;
                        break;
                    case SYS_MOUSELASTSCRIPT:
                        i = 0;
                        if(mouse_last_object >= main_window_data[0] && mouse_last_object <= main_window_data[MAX_WINDOW-1])
                        {
                            i = (mouse_last_object-main_window_data[0])/WINDOW_SIZE;
                            i = (int) main_window_script_start[i];
                        }
                        break;
                    case SYS_ITEMINDEX:
                        // Returns the index of the item that the current script is for...
                        // Used to figure out if weapons & armor are enchanted variants of main type...
                        i = global_item_index;
                        break;
                    case SYS_WEAPONREFRESHXYZ:
                        // Returns one of the XYZ coordinates we generated earlier...
                        // Should only be called during a weapon item's Refresh() function,
                        // and only after a set call to SYS_WEAPONREFRESHXYZ...
                        // k is the axis...
                        // Return value is scaled by 1000...
                        i = 0;
                        if(k >= X && k <= Z)
                        {
                            i = (signed int) (weapon_refresh_xyz[k] * 1000.0f);
                        }
                        break;
                    case SYS_WEAPONREFRESHBONENAME:
                        // Returns the bone name used by the weapon refresh functions...  So you can tell
                        // which weapon slot you're dealing with...
                        i = global_item_bone_name;
                        break;
                    case SYS_KEEPITEM:
                        // Returns the value of the global variable...
                        i = global_keep_item;
                        break;
                    case SYS_FREEPARTICLE:
                        // Returns number of unused particles...
                        i = main_unused_particle_count;
                        break;
                    case SYS_ROOMWATERLEVEL:
                        // Returns the room_water_level...  Multiplied by 100 to fit to int...
                        i = (int) (room_water_level * 100.0f);
                        break;
                    case SYS_ROOMWATERTYPE:
                        // Returns the room_water_type...
                        i = room_water_type;
                        break;
                    case SYS_MAPSIDENORMAL:
                        // Returns the vector components that tell which way the camera is facing...  Multiplied by 100 to fit to int...
                        // j is the axis...
                        i = (int) (map_side_xy[j&1] * 100.0f);
                        break;
                    case SYS_MESSAGESIZE:
                        // Returns the number of characters across that should be printed in the message window...
                        i = message_size;
                        break;
                    case SYS_FASTFUNCTIONFOUND:
                        // Returns TRUE if the last SYS_FASTFUNCTION or SYS_CHARFASTFUNCTION found a corresponding script function to run...
                        i = fast_function_found;
                        break;
                    case SYS_INPUTACTIVE:
                        // Returns the current input active timer...
                        i = input_active;
                        break;
                    case SYS_LOCALPLAYERINPUT:
                        // Used by WSTATUS window to read player inputs for inventory control via joystick/keyboard...
                        // j is the local player number (0-3), k is the input button to read...
                        j&=(MAX_LOCAL_PLAYER-1);
                        i = 0;
                        if(k >= 0)
                        {
                            if(k < 4)
                            {
                                i = player_device_button_pressed_copy[j][k];
                            }
                            else
                            {
                                if(k == PLAYER_DEVICE_BUTTON_ITEMS)
                                {
                                    i = player_device_inventory_toggle[j];
                                }
                                if(k == PLAYER_DEVICE_BUTTON_MOVE_UP || k == PLAYER_DEVICE_BUTTON_MOVE_DOWN)
                                {
                                    i = (int) (player_device_xy[j][Y] * 100.0f);
                                }
                                if(k == PLAYER_DEVICE_BUTTON_MOVE_LEFT || k == PLAYER_DEVICE_BUTTON_MOVE_RIGHT)
                                {
                                    i = (int) (player_device_xy[j][X] * 100.0f);
                                }
                                if(k == PLAYER_DEVICE_BUTTON_ITEMS_DOWN)
                                {
                                    i = player_device_inventory_down[j];
                                }
                            }
                        }
                        break;
                    case SYS_ROOMMONSTERTYPE:
                        // k is the subtype to poll (0-3)...  Used for CPORTAL and CRANDOM to
                        // get the proper monster types...  Subtype refers to the portal's subtype...
                        i = roombuffer[36+(k&3)];
                        break;
                    case SYS_PAYINGCUSTOMER:
                        // Returns either TRUE or FALSE
                        i = paying_customer;
                        break;
                    case SYS_ENCHANTCURSOR:
                        // Returns a variety of enchant_cursor info...  k is the subtype...
                        if(k == ENCHANT_CURSOR_TARGET)
                        {
                            i = enchant_cursor_target;
                        }
                        else if(k == ENCHANT_CURSOR_TARGET_ITEM)
                        {
                            i = enchant_cursor_target_item;
                        }
                        else
                        {
                            i = enchant_cursor_active;
                        }
                        break;
                    case SYS_CHARACTERSCRIPTFILE:
                        // Returns a pointer to the .RUN file for the given character index...
                        // k is the character index...
                        i = (int) main_character_script_start[k & (MAX_CHARACTER-1)];
                        break;
                    case SYS_PARTICLESCRIPTFILE:
                        // Returns a pointer to the .RUN file for the given particle index...
                        // k is the particle index...
                        i = (int) main_particle_script_start[k & (MAX_PARTICLE-1)];
                        break;
                    case SYS_MAPROOM:
                        // Used to get map info...  k is the parameter to read...
                        // j is the room number (only needed for some...)
                        // Top 16 bits of j is sometimes used as additional parameter...
                        m = j >> 16;
                        j = j&65535;
                        if(j < num_map_room)
                        {
                            if(k == MAP_ROOM_SRF)
                            {
                                i = (int) (*((unsigned char**) (map_room_data[j])));
                            }
                            if(k == MAP_ROOM_X)
                            {
                                i = ((*((unsigned short*) (map_room_data[j]+4))) - 32768) * 10;
                            }
                            if(k == MAP_ROOM_Y)
                            {
                                i = ((*((unsigned short*) (map_room_data[j]+6))) - 32768) * 10;
                            }
                            if(k == MAP_ROOM_DOOR_X || k == MAP_ROOM_DOOR_Y || k == MAP_ROOM_DOOR_Z)
                            {
                                script_matrix[0] = 0.0f;
                                script_matrix[1] = 0.0f;
                                script_matrix[2] = 0.0f;

                                script_matrix[3] = 0.0f;
                                script_matrix[4] = 0.0f;
                                script_matrix[5] = 0.0f;
                                if(m < 5)
                                {
                                    room_find_wall_center(*((unsigned char**) (map_room_data[j])), *((unsigned short*) (map_room_data[j]+8)), map_room_data[j][24+m], script_matrix, script_matrix+3, map_room_door_pushback);
                                }
                                if(k == MAP_ROOM_DOOR_X)
                                {
                                    i = (int) (script_matrix[0]*100.0f);
                                }
                                if(k == MAP_ROOM_DOOR_Y)
                                {
                                    i = (int) (script_matrix[1]*100.0f);
                                }
                                if(k == MAP_ROOM_DOOR_Z)
                                {
                                    i = (int) (script_matrix[2]*100.0f);
                                }
                            }
                            if(k == MAP_ROOM_ROTATION)
                            {
                                i = *((unsigned short*) (map_room_data[j]+8));
                            }
                            if(k == MAP_ROOM_SEED)
                            {
                                i = map_room_data[j][10];
                            }
                            if(k == MAP_ROOM_TWSET)
                            {
                                i = map_room_data[j][11];
                            }
                            if(k == MAP_ROOM_LEVEL)
                            {
                                i = map_room_data[j][12];
                            }
                            if(k == MAP_ROOM_FLAGS || k == MAP_ROOM_UPDATE_FLAGS)
                            {
                                i = map_room_data[j][13];
                            }
                            if(k == MAP_ROOM_DIFFICULTY)
                            {
                                i = map_room_data[j][30];
                            }
                            if(k == MAP_ROOM_AREA)
                            {
                                i = map_room_data[j][31];
                            }
                            if(k == MAP_ROOM_NEXT_ROOM)
                            {
                                m = m%5;
                                i = *((unsigned short*) (map_room_data[j]+14+(m<<1)));
                            }
                        }
                        if(k == MAP_ROOM_NUMBER)
                        {
                            i = num_map_room;
                        }
                        if(k == MAP_ROOM_CURRENT)
                        {
                            i = map_current_room;
                        }
                        if(k == MAP_ROOM_DOOR_SPIN)
                        {
                            // This is set on a call to room_find_wall_center() with pushback set...
                            i = map_room_door_spin;
                        }
                        if(k == MAP_ROOM_LAST_TOWN)
                        {
                            i = map_last_town_room;
                        }
                        break;
                    case SYS_MAPDOOROPEN:
                        // Used to find if a door is open in the current room...
                        // k is the door number...
                        i = 0;
                        if(map_current_room < num_map_room)
                        {
                            if(k < 5 && k >= 0)
                            {
                                i = (map_room_data[map_current_room][29] >> k) & 1;
                            }
                        }
                        break;
                    case SYS_LOCALPLAYERZ:
                        // Returns the z level that the camera is lookin' at...
                        i = (int) target_xyz[Z];
                        break;
                    case SYS_RESERVECHARACTER:
                        // Returns main_character_reserve_on for a given character...
                        // j is the character index...
                        i = FALSE;
                        if(j < MAX_CHARACTER && j >= 0)
                        {
                            i = main_character_reserve_on[j];
                        }
                        break;
                    case SYS_LUCK:
                        // Returns the global luck timer...
                        i = global_luck_timer;
                        break;
                    case SYS_NETWORKSCRIPT:
                        // Returns the value of the desired network_script_ variable...
                        // k is the variable number...
                        i = 0;
                        i = (k == NETWORK_SCRIPT_NEWLY_SPAWNED) ? network_script_newly_spawned : i;
                        i = (k == NETWORK_SCRIPT_EXTRA_DATA)    ? network_script_extra_data : i;
                        i = (k == NETWORK_SCRIPT_REMOTE_INDEX)  ? network_script_remote_index : i;
                        i = (k == NETWORK_SCRIPT_NETLIST_INDEX) ? network_script_netlist_index : i;
                        i = (k == NETWORK_SCRIPT_X)             ? network_script_x : i;
                        i = (k == NETWORK_SCRIPT_Y)             ? network_script_y : i;
                        i = (k == NETWORK_SCRIPT_Z)             ? network_script_z : i;
                        i = (k == NETWORK_SCRIPT_FACING)        ? network_script_facing : i;
                        i = (k == NETWORK_SCRIPT_ACTION)        ? network_script_action : i;
                        i = (k == NETWORK_SCRIPT_TEAM)          ? network_script_team : i;
                        i = (k == NETWORK_SCRIPT_POISON)        ? network_script_poison : i;
                        i = (k == NETWORK_SCRIPT_PETRIFY)       ? network_script_petrify : i;
                        i = (k == NETWORK_SCRIPT_ALPHA)         ? network_script_alpha : i;
                        i = (k == NETWORK_SCRIPT_DEFLECT)       ? network_script_deflect : i;
                        i = (k == NETWORK_SCRIPT_HASTE)         ? network_script_haste : i;
                        i = (k == NETWORK_SCRIPT_OTHER_ENCHANT) ? network_script_other_enchant : i;
                        i = (k == NETWORK_SCRIPT_EQLEFT)        ? network_script_eqleft : i;
                        i = (k == NETWORK_SCRIPT_EQRIGHT)       ? network_script_eqright : i;
                        i = (k == NETWORK_SCRIPT_EQCOL01)       ? network_script_eqcol01 : i;
                        i = (k == NETWORK_SCRIPT_EQCOL23)       ? network_script_eqcol23 : i;
                        i = (k == NETWORK_SCRIPT_EQSPEC1)       ? network_script_eqspec1 : i;
                        i = (k == NETWORK_SCRIPT_EQSPEC2)       ? network_script_eqspec2 : i;
                        i = (k == NETWORK_SCRIPT_EQHELM)        ? network_script_eqhelm : i;
                        i = (k == NETWORK_SCRIPT_EQBODY)        ? network_script_eqbody : i;
                        i = (k == NETWORK_SCRIPT_EQLEGS)        ? network_script_eqlegs : i;
                        i = (k == NETWORK_SCRIPT_CLASS)         ? network_script_class : i;
                        i = (k == NETWORK_SCRIPT_MOUNT_INDEX)   ? network_script_mount_index : i;
                        break;
                    case SYS_ROOMMETALBOXITEM:
                        // Returns the room_metal_box_item...
                        i = room_metal_box_item;
                        break;
                    case SYS_CAMERAZOOM:
                        // Returns the current camera zoom level...
                        i = (int) camera_to_distance;
                        break;
                    case SYS_CAMERASPIN:
                        // Returns the current camera spin...
                        i = (int) camera_rotation_xy[X];
                        break;
                    case SYS_LOAD:
                        // Check if a file corresponding to the specified load slot exists...
                        slot = j;
                        sprintf(file,"SAVE%d.DAT",j);
                        loadfile = fopen(file, "rb");

                        if(loadfile)
                        {
                            log_message("INFO:   SAVE%d.DAT is a valid savefile", slot);
                            fclose(loadfile);
                            i = TRUE;
                        }
                        else
                        {
                            log_message("INFO:   Couldn't open SAVE%d.DAT", slot);
                            i = FALSE;
                        }
                        break;
                    default:
                        i = TRUE;
                        break;
                }
                push_int_stack(i);
                break;
            case OPCODE_DEBUGMESSAGE:
                // Spits out a debug message...
                pop_int_stack_cast(call_address, unsigned char*);  // String
                message_add(call_address, "DEBUG", FALSE);
                push_int_stack(TRUE);
                break;
            case OPCODE_TOFLOAT:
                // Transfer a value from the int stack to the float stack
                pop_int_stack_cast(f, float);
                push_float_stack(f);
                break;
            case OPCODE_TOINT:
                // Transfer a value from the float stack to the int stack
                pop_float_stack_cast(i, int);
                push_int_stack(i);
                break;
            case OPCODE_F_EQUALS:
                // Put top value from float stack into the given variable (1 byte extension)
                get_nopop_float_stack(f);
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Left operand is an integer variable...
                        int_variable[opcode&(MAX_VARIABLE-1)] = (int) f;
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                (*((int*) arg_address)) = (int) f;
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                (*((float*) arg_address)) = f;
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                (*((unsigned char*) arg_address)) = (unsigned char) f;
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                (*((unsigned short*) arg_address)) = (unsigned short) f;
                                break;
                            default:
                                // Strings and text can't get a float copied into 'em...
                                log_message("ERROR:  Invalid property %d-%c for f_equals command", opcode, property_type[opcode]);
                                break;
                        }
                        break;
                    case 192:
                        // Left operand is an float variable...
                        float_variable[opcode&(MAX_VARIABLE-1)] = f;
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable %d for f_equals command", opcode);
                        break;
                }
                break;
            case OPCODE_F_ADD:
                // Add the top value on the stack to the next to top value
                pop_float_stack(f);
                operate_float_stack(+, f);
                break;
            case OPCODE_F_SUBTRACT:
                // Subtract the top value on the stack from the next to top value
                pop_float_stack(f);
                operate_float_stack(-, f);
                break;
            case OPCODE_F_MULTIPLY:
                // Multiply the top value on the stack with the next to top value
                pop_float_stack(f);
                operate_float_stack(*, f);
                break;
            case OPCODE_F_DIVIDE:
                // Divide the next to top value on the stack by the top value
                pop_float_stack(f);
                if(f > 0.0001 || f < -0.0001)
                {
                    operate_float_stack(/, f);
                }
                break;
            case OPCODE_F_INCREMENT:
                // Decrement a variable...  1 byte extension follows...
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Integer variable...
                        f = (float) (int_variable[opcode&(MAX_VARIABLE-1)]++);
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                f = (float) ((*((int*) arg_address))++);
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                f = ((*((float*) arg_address))++);
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                f = (float) ((*((unsigned char*) arg_address))++);
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                f = (float) ((*((unsigned short*) arg_address))++);
                                break;
                            default:
                                // No effect on strings and text...
                                log_message("ERROR:  Invalid variable for f_increment command");
                                f = 0.0f;
                                break;
                        }
                        break;
                    case 192:
                        // Float variable...
                        f = (float_variable[opcode&(MAX_VARIABLE-1)]++);
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable for f_increment command");
                        f = 0.0f;
                        break;
                }
                push_float_stack(f);
                break;
            case OPCODE_F_DECREMENT:
                // Decrement a variable...  1 byte extension follows...
                opcode = *address;
                address++;
                // Put the data into one of the variables...
                switch(opcode&224)
                {
                    case 128:
                        // Integer variable...
                        f = (float) (int_variable[opcode&(MAX_VARIABLE-1)]--);
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Change the integer value at arg_address...
                                f = (float) ((*((int*) arg_address))--);
                                break;
                            case VAR_FLOAT:
                                // Change the float value at arg_address...
                                f = ((*((float*) arg_address))--);
                                break;
                            case VAR_BYTE:
                                // Change the byte value at arg_address...
                                f = (float) ((*((unsigned char*) arg_address))--);
                                break;
                            case VAR_WORD:
                                // Change the word value at arg_address...
                                f = (float) ((*((unsigned short*) arg_address))--);
                                break;
                            default:
                                // No effect on strings and text...
                                log_message("ERROR:  Invalid variable for f_decrement command");
                                f = 0.0f;
                                break;
                        }
                        break;
                    case 192:
                        // Float variable...
                        f = (float_variable[opcode&(MAX_VARIABLE-1)]--);
                        break;
                    default:
                        // It shouldn't ever get here...
                        log_message("ERROR:  Invalid variable for f_decrement command");
                        f = 0.0f;
                        break;
                }
                push_float_stack(f);
                break;
            case OPCODE_F_ISEQUAL:
                // Leave a one on the stack if the top two values are equal
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e == f);
                break;
            case OPCODE_F_ISNOTEQUAL:
                // Leave a one on the stack if the top two values are not equal
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e != f);
                break;
            case OPCODE_F_ISGREATEREQUAL:
                // Leave a one on the stack if the next to top >= top value on the stack
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e >= f);
                break;
            case OPCODE_F_ISLESSEREQUAL:
                // Leave a one on the stack if the next to top <= top value on the stack
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e <= f);
                break;
            case OPCODE_F_ISGREATER:
                // Leave a one on the stack if the next to top > top value on the stack
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e > f);
                break;
            case OPCODE_F_ISLESSER:
                // Leave a one on the stack if the next to top < top value on the stack
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e < f);
                break;
            case OPCODE_F_LOGICALAND:
                // Leave a one on the stack if both of the values are positive
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e && f);
                break;
            case OPCODE_F_LOGICALOR:
                // Leave a one on the stack if either of the values are positive
                pop_float_stack(f);
                pop_float_stack(e);
                push_int_stack(e || f);
                break;
            case OPCODE_F_LOGICALNOT:
                // Perform a logical not of the top value on the stack
                pop_float_stack(f);
                push_int_stack(!f);
                break;
            case OPCODE_F_NEGATE:
                // Negates the top value on the stack
                preoperate_float_stack(-);
                break;
            case OPCODE_STRING:
                // Returns a pointer to a string...
                pop_int_stack_cast(opcode, unsigned char);
                push_int_stack(((int) run_string[opcode & (MAX_STRING-1)]));
                break;
            case OPCODE_STRINGGETNUMBER:
                // Returns the first decimal value found in a string...
                pop_int_stack_cast(call_address, unsigned char*);
                sscanf(call_address, "%d", &i);
                push_int_stack(i);
                break;
            case OPCODE_STRINGCLEAR:
                // Clears a given string...  Should not be used with token strings...
                pop_int_stack_cast(call_address, unsigned char*);
                *call_address = 0;
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGCLEARALL:
                // Clears all of the string variables...
                repeat(i, MAX_STRING) { run_string[i][0] = 0; }
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGAPPEND:
                // Appends some data to a string variable...
                pop_int_stack(k);                                       // max destination size
                pop_int_stack_cast(arg_address, unsigned char*);        // source
                pop_int_stack_cast(call_address, unsigned char*);       // destination



                // Now read the values...
                k--;
                j = strlen(call_address);
                while(j < k && *arg_address != 0)
                {
                    call_address[j] = *arg_address;
                    arg_address++;
                    j++;
                }
                call_address[j] = 0;
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGCOMPARE:
                // Returns TRUE if the strings match
                pop_int_stack_cast(arg_address, unsigned char*);        // String 2
                pop_int_stack_cast(call_address, unsigned char*);       // String 1


                // Now compare the values...
                while(*arg_address == *call_address && *arg_address != 0)
                {
                    arg_address++;
                    call_address++;
                }
                if(*arg_address == 0 && *call_address == 0) { push_int_stack(TRUE); }
                else { push_int_stack(FALSE); }
                break;
            case OPCODE_STRINGLENGTH:
                // Returns the length of the string
                pop_int_stack_cast(call_address, unsigned char*);       // String
                push_int_stack(strlen(call_address));
                break;
            case OPCODE_STRINGCHOPLEFT:
                // Chops some characters off the left side of a string
                pop_int_stack(j);                                       // Amount to chop
                pop_int_stack_cast(call_address, unsigned char*);       // String



                // Stick a null terminator at the end...
                i = strlen(call_address);
                if(i > j)
                {
                    // Copy data from further ahead back to the start of the string...
                    arg_address = call_address + j;
                    while(*arg_address != 0)
                    {
                        *call_address = *arg_address;
                        arg_address++;
                       call_address++;
                    }
                }
                *call_address = 0;


                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGCHOPRIGHT:
                // Chops some characters off the right side of a string
                pop_int_stack(j);                                       // Amount to chop
                pop_int_stack_cast(call_address, unsigned char*);       // String


                // Stick a null terminator at the end...
                i = strlen(call_address);
                if(i > j)
                {
                    *(call_address+i-j) = 0;
                }
                else *call_address = 0;
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGRANDOMNAME:
                // Prints a random name from a datafile into a given string location
                pop_int_stack_cast(arg_address, unsigned char*);        // File data
                pop_int_stack_cast(call_address, unsigned char*);       // Place to write
                arg_address = random_name(arg_address);

                // Copy the name into the string...  Should be safe to copy all 16 bytes...
                memcpy(call_address, arg_address, 16);
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGSANITIZE:
                // Removes naughty language from a string
// !!!BAD!!!
// !!!BAD!!!  Make this work (need for player names & chat text)...  (Chat text done automagically?)
// !!!BAD!!!
log_message("ERROR:   StringSanitize()");
                break;
            case OPCODE_NETWORKMESSAGE:
                // Spits a string into the message buffer...
                pop_int_stack(i);  // Speaker's class
                pop_int_stack_cast(call_address, unsigned char*);  // Message string
                pop_int_stack_cast(arg_address, unsigned char*);  // Speaker name string
                network_send_chat((unsigned char) i, arg_address, call_address);
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGLANGUAGE:
                // Gives a string from the current .LAN file...  Multilingual support...
                pop_int_stack(j);                                       // Index requested
                if(j <= user_language_phrases && j > 0)
                {
                    call_address = language_file[user_language] + (j<<2) + 1;
                    j = (*call_address);  j = j<<8;  call_address++;
                    j += (*call_address);  j = j<<8;  call_address++;
                    j += (*call_address);
                    j += (unsigned int) language_file[user_language];
                }
                else
                {
                    j = (int) bad_string;
                }
                push_int_stack(j);
                break;
            case OPCODE_STRINGUPPERCASE:
                // Makes a string uppercase
                pop_int_stack_cast(call_address, unsigned char*);       // String
                make_uppercase(call_address);
                push_int_stack(TRUE);
                break;
            case OPCODE_STRINGAPPENDNUMBER:
                // Appends a number to the end of a string...
                pop_int_stack(k);                                       // String length
                pop_int_stack(j);                                       // Number to append
                pop_int_stack_cast(call_address, unsigned char*);       // String


                // Generate the number string...  Backwards...
                m = 0;
                opcode = 0;
                if(j < 0)  { opcode = '-';  j = -j; }
                else if(j == 0) { opcode = '0'; }
                while(j > 0)
                {
                    number_string[m] = (j%10)+'0';  j = j/10;
                    m++;
                }


                // Append the number string to the string...
                if(opcode) { number_string[m] = opcode;  m++;  }
                i = strlen(call_address);
                k--;
                while(m > 0 && i < k)
                {
                    m--;
                    call_address[i] = number_string[m];
                    i++;
                }
                call_address[i] = 0;


                push_int_stack(TRUE);
                break;
            case OPCODE_CALLFUNCTION:
                // Use recursion to call a subfunction in the script...


                // Read in the addresses...
                call_address = (unsigned char*) sdf_read_unsigned_int(address);
                return_address = (unsigned char*) sdf_read_unsigned_int(address+4);
                arg_address = (unsigned char*) sdf_read_unsigned_int(address+8);
                file_address = (unsigned char*) sdf_read_unsigned_int(address+12);


                // Count args in argument list...
                num_int_args = 0;
                num_float_args = 0;
                opcode = *(arg_address);
                while(opcode != 0)
                {
                    if(opcode == VAR_INT)  num_int_args++;
                    else  num_float_args++;
                    arg_address++;
                    opcode = *(arg_address);
                }


                // Decrease stacks as if we were popping args
int_stack_head-=num_int_args;
float_stack_head-=num_float_args;

//
//  Old code didn't wrap edge
//
//                repeat(i, num_int_args)
//                {
//                    if(int_stack_head > 0) int_stack_head--;
//                }
//                repeat(i, num_float_args)
//                {
//                    if(float_stack_head > 0) float_stack_head--;
//                }


                // Copy arguments to a seperate data area...  Needed in case the stack header
                // is lying on the edge, about to wrap around, and some of the arguments fall
                // on one side, some on the other...  Otherwise we could just use the stacks
                // as they are...
                repeat(opcode, num_int_args)
                {
                    int_argument[opcode] = int_stack[(unsigned char)(int_stack_head+opcode)];
                }
                repeat(opcode, num_float_args)
                {
                    float_argument[opcode] = float_stack[(unsigned char)(float_stack_head+opcode)];
                }




                // Call function with information about the arguments it was given...
                if(call_address != NULL)
                {
                    if(run_script(call_address, file_address, num_int_args, int_argument, num_float_args, float_argument))
                    {
                        // The subfunction returned an int...  Push it onto the stack...
                        push_int_stack(return_int);
                    }
                    else
                    {
                        // The subfunction returned a float...  Push it onto the stack...
                        push_float_stack(return_float);
                    }


//
//  Old code...  Didn't wrap around edge...
//
//                    if(run_script(call_address, file_address, num_int_args, &int_stack[int_stack_head], num_float_args, &float_stack[float_stack_head]))
//                    {
//                        // The subfunction returned an int...  Push it onto the stack...
//                        push_int_stack(return_int);
//                    }
//                    else
//                    {
//                        // The subfunction returned a float...  Push it onto the stack...
//                        push_float_stack(return_float);
//                    }
                }
                else
                {
                    // Not the best way to handle an error...
                    push_int_stack(0);
                    push_float_stack(0);
                }


                // Now jump to our next opcode
                address = return_address;
                break;
            case OPCODE_RETURNINT:
                // End of the script, but may return values for recursion
                pop_int_stack(return_int);
                return_int_is_set = TRUE;
                keepgoing = FALSE;
                break;
            case OPCODE_RETURNFLOAT:
                // End of the script, but may return values for recursion
                pop_float_stack(return_float);
                return_int_is_set = FALSE;
                keepgoing = FALSE;
                break;
            case OPCODE_IFFALSEJUMP:
                // Go to a new read address if the top integer isn't TRUE...
                pop_int_stack(i);
                if(i)
                {
                    // Stay where we are...
                    address += 2;
                }
                else
                {
                    // Skip to where the jump says to go
                    address = file_start+sdf_read_unsigned_short(address);
                }
                break;
            case OPCODE_JUMP:
                // Go to a new read address...
                address = file_start+sdf_read_unsigned_short(address);
                break;
            case OPCODE_SQRT:
                // Return the square root of the top float value...
                preoperate_float_stack((float) sqrt);
                break;
            case OPCODE_FILEOPEN:
                // Returns the starting location of the given file...
                // Converts "TEST.TXT" into "FILE:TEST.TXT"
                // Returns NULL if not found...
                // Used for dynamic file opening (for tools)...
                // Very slow...  Should not be used for game things...
                pop_int_stack(i);                                  // mode
                pop_int_stack_cast(call_address, unsigned char*);  // Filename string
                if(i == FILE_EXPORT)
                {
                    sdf_export_file(call_address, NULL);
                }
                if(i == FILE_IMPORT)
                {
                    sdf_add_file(call_address, NULL);
                }
                if(i == FILE_DELETE)
                {
                    sdf_delete_file(call_address);
                }
                if(i == FILE_MAKENEW)
                {
                    sdf_new_file(call_address);
                }

                call_address = sdf_find_index(call_address);
                if(call_address != NULL)
                {
                    if(i == FILE_SIZE)
                    {
                        call_address = (unsigned char*) (sdf_read_unsigned_int(call_address+4)&0x00ffffff);
                    }
                    else
                    {
                        call_address = (unsigned char*) sdf_read_unsigned_int(call_address);
                    }
                }
                push_int_stack(((int) call_address));
                break;
            case OPCODE_FILEREADBYTE:
                // Returns a value from a file...  File must exist and be big enough...
                pop_int_stack(j);                                       // Offset
                pop_int_stack_cast(call_address, unsigned char*);       // File/String start


                // Figure where we're reading from...
                push_int_stack((*(call_address + j)));
                break;
            case OPCODE_FILEWRITEBYTE:
                // Writes a value to a file...  File must exist and be big enough...
                pop_int_stack_cast(opcode, unsigned char);              // Value
                pop_int_stack(i);                                       // Offset
                pop_int_stack_cast(call_address, unsigned char*);       // File/String start
                call_address+=i;
                *call_address = opcode;
                push_int_stack(TRUE);
                break;
            case OPCODE_FILEINSERT:
                // Inserts or removes data from a file (not a string)
#ifdef DEVTOOL
                pop_int_stack(j);                                       // Bytes to insert
                pop_int_stack_cast(arg_address, unsigned char*);        // Data to insert
                pop_int_stack(i);                                       // Offset into file
                pop_int_stack_cast(call_address, unsigned char*);       // File data
                call_address+=i;
                push_int_stack(sdf_insert_data(call_address, arg_address, j));
#endif
                break;
            case OPCODE_SPAWN:
                // Creates a new object and returns a pointer to its data (for property access)
                pop_int_stack_cast(call_address, unsigned char*);       // Script start
                pop_float_stack(script_temp_e);                         // z
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                pop_int_stack_cast(opcode, unsigned char);              // type


                arg_address = current_object_data;                      // Backup...
                j = current_object_item;                                // Backup...
                if(call_address == NULL)
                {
                    // No .RUN file given, assume duplicate of current...  Makes spawning particles easier...
                    call_address = file_start;
                }
                i = (int) obj_spawn(opcode, e, f, script_temp_e, call_address, 65535);
                if(opcode == CHARACTER)
                {
                    // Set the current frame to be the first frame of the current action...
                    current_object_data = (unsigned char*) i;
                    if(current_object_data)
                    {
                        call_address = *((unsigned char**) (current_object_data+256));
                        if(call_address)
                        {
                            file_address=call_address+6+(current_object_data[65]<<1);
                            (*((unsigned short*) (current_object_data+178))) = (*((unsigned short*) file_address));
                            if((*((unsigned short*) (current_object_data+178))) == 65535)
                            {
                                // Desired action from Spawn() not found, so default to stand action...
                                current_object_data[65] = ACTION_STAND;
                                current_object_data[66] = ACTION_STAND;
                                file_address=call_address+6+(current_object_data[65]<<1);
                                (*((unsigned short*) (current_object_data+178))) = (*((unsigned short*) file_address));
                            }
                        }
                    }
                }
                current_object_item = j;                                // Restore...
                current_object_data = arg_address;                      // Restore...
                push_int_stack(i);
                break;
            case OPCODE_GOPOOF:
                // Ends the script like a return and destroys the current object...
                pop_int_stack(i);                                       // Type
                if(i == POOF_SELF)
                {
                    obj_destroy(current_object_data);
                    return_int = 0;
                    return_int_is_set = TRUE;
                    keepgoing = FALSE;
                }
                if(i == POOF_TARGET)
                {
                    i = *((unsigned short*) (current_object_data + 62));
                    if(i < MAX_CHARACTER)
                    {
                        obj_destroy(main_character_data[i]);
                    }
                }
                if(i == POOF_ALL_OTHER_WINDOWS)
                {
                    // Destroy all windows, except self (for menus)
                    i = 0;
                    while(i < main_used_window_count)
                    {
                        j = main_window_order[i];
                        call_address = main_window_data[j];
                        if(call_address != current_object_data)
                        {
                            obj_destroy(call_address);
                        }
                        else
                        {
                            i++;
                        }
                    }
                }
                if(i == WARN_ALL_OTHER_WINDOWS)
                {
                    // Give an Event() to all windows, except self (for menus)...
                    // Since windows don't have an event byte, it's assumed that
                    // the event function means that they're gonna be poof'd soon...
                    repeat(i, main_used_window_count)
                    {
                        j = main_window_order[i];
                        if(main_window_data[j] != current_object_data)
                        {
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(main_window_script_start[j], FAST_FUNCTION_EVENT, main_window_data[j]);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                    }
                }
                if(i == POOF_STUCK_PARTICLES || i == POOF_TARGET_STUCK_PARTICLES)
                {
                    // Find the character we're dealin' with...
                    if(i == POOF_STUCK_PARTICLES)
                    {
                        if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                        {
                            i = (current_object_data-main_character_data[0])/CHARACTER_SIZE;
                            i = i & (MAX_CHARACTER-1);
                        }
                    }
                    else
                    {
                        i = *((unsigned short*) (current_object_data + 62));
                    }
                    if(i < MAX_CHARACTER)
                    {
                        // Destroy all of the particles that are stuck to this character...
                        repeat(j, MAX_PARTICLE)
                        {
                            if(main_particle_on[j])
                            {
                                if((*((unsigned short*) (main_particle_data[j]+60))) & PART_STUCK_ON)
                                {
                                    if((*((unsigned short*) (main_particle_data[j]+86))) == i)
                                    {
                                        main_unused_particle_count++;
                                        main_particle_on[j] = FALSE;
                                    }
                                }
                            }
                        }
                    }
                }
                if(i == POOF_ALL_PARTICLES)
                {
                    obj_poof_all(PARTICLE);
                    enchant_cursor_active = FALSE;
                }
                if(i == POOF_ALL_CHARACTERS)
                {
                    obj_poof_all(CHARACTER);
                    enchant_cursor_active = FALSE;
                }
                break;
            case OPCODE_DISMOUNT:
                // Knock the rider off its mount...
                // Make sure we're dealin' with a character...
                if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                {
                    // Figger if mount is kickin' rider off, or if rider is jumpin' off...
                    j = ((current_object_data - main_character_data[0]) / CHARACTER_SIZE) & (MAX_CHARACTER-1);
                    i = *((unsigned short*) (current_object_data + 106));
                    if(i >= MAX_CHARACTER)
                    {
                        // Self.rider is invalid, so current char must be the rider jumpin' off the mount...
                        i = j;
                        j = *((unsigned short*) (current_object_data + 164));
                    }


                    // i should be rider and j should be mount...
                    if(i < MAX_CHARACTER && j < MAX_CHARACTER)
                    {
                        if(main_character_on[i] && main_character_on[j])
                        {
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            main_character_data[i][67] = EVENT_DISMOUNTED;
                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, main_character_data[i]);
                            main_character_data[j][67] = EVENT_DISMOUNTED;
                            fast_run_script(main_character_script_start[j], FAST_FUNCTION_EVENT, main_character_data[j]);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                            *((unsigned short*) (main_character_data[i] + 106)) = 65535;
                            *((unsigned short*) (main_character_data[i] + 164)) = 65535;
                            *((unsigned short*) (main_character_data[j] + 106)) = 65535;
                            *((unsigned short*) (main_character_data[j] + 164)) = 65535;


                            // Set actions & velocities for rider...
                            main_character_data[i][65] = ACTION_JUMP_BEGIN;
                            main_character_data[i][66] = ACTION_JUMP;
                            if(*((unsigned short*) (main_character_data[i]+60)) & CHAR_IN_WATER)
                            {
                                // Rider is in water, so make sure we don't get stuck unable to dismount...
                                *((unsigned short*) (main_character_data[i]+166)) = 30;
                            }

                            (*((unsigned short*) (main_character_data[i] + 60))) |= CHAR_FALL_ON;
                            *((float*) (main_character_data[i] + 24)) = 0.0f;
                            *((float*) (main_character_data[i] + 28)) = 0.0f;
                            *((float*) (main_character_data[i] + 32)) = 0.65f;


                            // Make sure rider isn't flag'd to stand on platform (in case mount is also a platform)
                            (*((unsigned short*) (main_character_data[i] + 60))) &= (~CHAR_ON_PLATFORM);
                        }
                    }
                }
                break;
            case OPCODE_ROLLDICE:
                // Return the result of a random dice roll...
                pop_int_stack(j);                                   // Sides on dice
                pop_int_stack(i);                                   // Number of dice
                i = random_dice((unsigned char) i, (unsigned short) j);
                push_int_stack(i);
                break;
            case OPCODE_PLAYSOUND:
                // Plays a sound
                pop_int_stack(j);                                    // Volume
                pop_int_stack(i);                                    // Pitch skip
                pop_int_stack_cast(call_address, unsigned char*);    // Raw file start

                // Setup volume and pan...
                opcode = 128;  // Pan
                if((current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1]) || (current_object_data >= main_particle_data[0] && current_object_data <= main_particle_data[MAX_PARTICLE-1]))
                {
                    // We're dealing with a character or particle, so do volume falloff with distance...
                    e = ((float*)current_object_data)[X] - target_xyz[X];
                    f = ((float*)current_object_data)[Y] - target_xyz[Y];
                    script_temp_e = ((float*)current_object_data)[Z] - target_xyz[Z];
                    script_temp_e = e*e + f*f + script_temp_e*script_temp_e;

                    // Should be full volume at 50 feet...
                    script_temp_e = (j*2500.0f)/script_temp_e;
                    clip(0.0f, script_temp_e, ((float) j));
                    j = (unsigned char) script_temp_e;

                    // Now figger pan based on side dot...
                    script_temp_e = 128.0f + ((camera_side_xyz[X]*e + camera_side_xyz[Y]*f)*5.0f);
                    clip(0.0f, script_temp_e, 255.0f);
                    opcode = (unsigned char) script_temp_e;
                }


                play_sound(0, call_address, (unsigned char) ((((unsigned char) j)*master_sfx_volume)>>8), opcode, (unsigned short) i, NULL);
                push_int_stack(TRUE);
                break;
            case OPCODE_PLAYMEGASOUND:
                // Plays a sound in a fully specified way
                pop_int_stack_cast(arg_address, unsigned char*);        // Loop data
                pop_int_stack_cast(opcode, unsigned char);              // Pan
                pop_int_stack(j);                                       // Volume
                pop_int_stack(i);                                       // Pitch skip
                pop_int_stack_cast(call_address, unsigned char*);       // Raw file start

                m = music_tempo;
                music_tempo = 1378;                                     // Duration in 16th seconds...
                play_sound(0, call_address,  (unsigned char) ((((unsigned char) j)*master_sfx_volume)>>8), opcode, (unsigned short) i, arg_address);
                music_tempo = m;
                push_int_stack(TRUE);
                break;
            case OPCODE_DISTANCESOUND:
                // Changes the volume of an infinite looped sound, based on distance to camera
                pop_int_stack(i);                                       // Channel

// !!!BAD!!!
// !!!BAD!!!  Finish me...  Distance check for new_volume...  All new_volume stuff should be taken care of...  Just set i and j correctly depending on distance (instead of hacked into argument)...
// !!!BAD!!!
                opcode = i&(MAX_CHANNEL-1);                             // Channel
                i = i >> 6;                                             // Left volume
                j = i;                                                  // Right volume
                if(channel_new_volume[opcode][LEFT] < i)  channel_new_volume[opcode][LEFT] = i;
                if(channel_new_volume[opcode][RIGHT] < j)  channel_new_volume[opcode][RIGHT] = j;
                push_int_stack(TRUE);
                break;
            case OPCODE_PLAYMUSIC:
                // Plays a music file
                pop_int_stack_cast(opcode, unsigned char);              // Mode
                pop_int_stack(i);                                       // Start time
                pop_int_stack_cast(call_address, unsigned char*);       // Mus file start
                play_music(call_address, (unsigned short) i, opcode);
                push_int_stack(TRUE);
                break;
            case OPCODE_UPDATEFILES:
                // Begins the file update process...
                pop_int_stack_cast(opcode, unsigned char);              // Mode
                if(opcode == UPDATE_END)
                {
                    if(update_performed)
                    {
                        // Unload all of the textures from the graphics card
                        display_unload_all_textures();

                        // Stop playing any sounds
                        sound_reset_channels();

                        // Decompress any changed data files (ogg, jpg, pcx, etc...)  Overwrites old RAW and RGB and EKO files...
                        sdf_decode(SDF_FLAG_WAS_UPDATED, FALSE);
                        ddd_magic_update_thing(SDF_FLAG_WAS_UPDATED);
                        render_crunch_all(SDF_FLAG_WAS_UPDATED);


                        // Relink the RUN files...
                        #ifndef DEVTOOL
                            compiler_error = FALSE;
                        #endif
                        while(src_stage_compile(SRC_FUNCTIONIZE, SDF_ALL) == FALSE);  // Keep linking until all of the errors go away...

                        // Twiddle this and that
                        update_active = FALSE;
                        sdf_flag_clear(SDF_FLAG_WAS_UPDATED);
                        sanitize_file = NULL;
                        #ifndef DEVTOOL
                            message_reset();
                        #endif
                        message_setup();


                        // Update addresses so things don't explode...
                        damage_setup();
                        display_kanji_setup();


                        // Find and run LANGUAGE.Setup() and ITEMREG.Setup()...
                        arg_address = sdf_find_filetype("LANGUAGE", SDF_FILE_IS_RUN);
                        if(arg_address)
                        {
                            arg_address = (unsigned char*) sdf_read_unsigned_int(arg_address);
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(arg_address, FAST_FUNCTION_SETUP, mainbuffer);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }
                        arg_address = sdf_find_filetype("ITEMREG", SDF_FILE_IS_RUN);
                        if(arg_address)
                        {
                            arg_address = (unsigned char*) sdf_read_unsigned_int(arg_address);
                            call_address = current_object_data;
                            script_temp_i = current_object_item;
                            fast_run_script(arg_address, FAST_FUNCTION_SETUP, mainbuffer);
                            current_object_data = call_address;
                            current_object_item = script_temp_i;
                        }


                        // Reload the new textures
                        display_load_all_textures();
                    }
                }
                else if(opcode == UPDATE_RECOMPILE)
                {
                    // Get script names for each object
                    obj_recompile_start();

                    // Recompile all of the SRC files that've been updated...
                    log_error_count = 0;
                    src_define_setup();
                    compiler_error = FALSE;
                    src_stage_compile(SRC_HEADERIZE, SDF_FLAG_WAS_UPDATED);
                    src_stage_compile(SRC_COMPILERIZE, SDF_FLAG_WAS_UPDATED);

                    // Relink the RUN files...
                    while(src_stage_compile(SRC_FUNCTIONIZE, SDF_ALL) == FALSE);  // Keep linking until all of the errors go away...
                    damage_setup();  // Need to keep track of PNUMBER.RUN...


                    // Fill in the object script addresses based on the names...
                    obj_recompile_end();
                    keepgoing = FALSE;


                    // Find and run LANGUAGE.Setup() and ITEMREG.Setup()...
                    arg_address = sdf_find_filetype("LANGUAGE", SDF_FILE_IS_RUN);
                    if(arg_address)
                    {
                        arg_address = (unsigned char*) sdf_read_unsigned_int(arg_address);
                        call_address = current_object_data;
                        script_temp_i = current_object_item;
                        fast_run_script(arg_address, FAST_FUNCTION_SETUP, mainbuffer);
                        current_object_data = call_address;
                        current_object_item = script_temp_i;
                    }
                    arg_address = sdf_find_filetype("ITEMREG", SDF_FILE_IS_RUN);
                    if(arg_address)
                    {
                        arg_address = (unsigned char*) sdf_read_unsigned_int(arg_address);
                        call_address = current_object_data;
                        script_temp_i = current_object_item;
                        fast_run_script(arg_address, FAST_FUNCTION_SETUP, mainbuffer);
                        current_object_data = call_address;
                        current_object_item = script_temp_i;
                    }
                }
                else if(opcode == UPDATE_SDFSAVE)
                {
                    // Save the datafile
                    sdf_save("datafile.sdf");
                }
                else if(opcode == UPDATE_SDFREORGANIZE)
                {
                    // Reorganize the datafile
                    sdf_reorganize_index();
                }
                else
                {
                    // Reload all of the textures...
                    display_unload_all_textures();
                    display_load_all_textures();
                }
                break;
            case OPCODE_SIN:
                pop_float_stack(e);  // angle from 0 to 360...
                e = sine_table[((unsigned short)(e*182.04444f))>>4];
                push_float_stack(e);
                break;
            case OPCODE_ACQUIRETARGET:
                // Should only be called by particles or characters...
                pop_float_stack(script_temp_e);                         // Radius...
                pop_int_stack_cast(call_address, unsigned char*);       // Script File pointer...
                pop_int_stack(j);                                       // Flags...


                script_temp_i = MAX_CHARACTER;  // The best character we've found...
                script_temp_e = script_temp_e*script_temp_e;   // The best distance we've found...  Square'd actually...
                script_temp_f = script_temp_e;  // The original radius square'd...
                repeat(i, MAX_CHARACTER)
                {
                    // Is this character on?
                    if(main_character_on[i])
                    {
                        arg_address = main_character_data[i];


                        // First check team...
                        if((arg_address[78] == TEAM_NEUTRAL && (j & ACQUIRE_SEE_NEUTRAL)) || (arg_address[78] > TEAM_NEUTRAL && arg_address[78] != current_object_data[78] && (j & ACQUIRE_SEE_ENEMY)) || (arg_address[78] == current_object_data[78] && (j & ACQUIRE_SEE_FRIENDLY)))
                        {
                            // Team is valid...  Now check specified script...
                            if(call_address == NULL || main_character_script_start[i] == call_address)
                            {
                                // Script is okay or not specified...  But let's check for some other miscellaneous stuff...
                                if(arg_address[65] != ACTION_RIDE && (*((unsigned short*) (arg_address+170)) < 1000) && ((j & ACQUIRE_IGNORE_COLLISION) || !((*((unsigned short*) (arg_address+60))) & CHAR_NO_COLLISION)))
                                {
                                    // Check for mimic hiding...
                                    if(!((arg_address[105] & VIRTUE_FLAG_STILL_HIDE) && arg_address[65] == ACTION_STAND) || (j & ACQUIRE_SEE_INVISIBLE))
                                    {
                                        // Limit to open mounts if that's what we're after...
                                        if((((*(unsigned short*) (arg_address+60)) & CHAR_CAN_BE_MOUNTED) && (*((unsigned short*) (arg_address+42)) == 0) && arg_address[66] != ACTION_KNOCK_OUT && ((*(unsigned short*) (arg_address+106)) >= MAX_CHARACTER)) || !(j & ACQUIRE_OPEN_MOUNT_ONLY))
                                        {
                                            // Limit to standing characters if that's what we're after...
                                            if(arg_address[65] < ACTION_KNOCK_OUT_BEGIN || arg_address[65] > ACTION_KNOCK_OUT_END || !(j & ACQUIRE_STANDING_ONLY))
                                            {
                                                // Looks like a valid character, but is it in front of us (or can we see behind us)?
                                                e = (*((float*) (arg_address))) - (*((float*) (current_object_data)));
                                                f = (*((float*) (arg_address+4))) - (*((float*) (current_object_data+4)));
                                                script_temp_j = (j & ACQUIRE_SEE_BEHIND);
                                                if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                                                {
                                                    // We're dealing with a character, so do a dot product with our matrix...
                                                    script_temp_j |= ((e*(*((float*) (current_object_data+120))) + f*(*((float*) (current_object_data+124)))) > 0.0f);
                                                    // Also make sure we don't return our own mount or ourself...
                                                    if(i == *((unsigned short*)(current_object_data+164)) || arg_address == current_object_data)
                                                    {
                                                        script_temp_j = FALSE;
                                                    }
                                                }
                                                else
                                                {
                                                    // We're dealing with a particle, so dot with our velocity vector...
                                                    script_temp_j |= ((e*(*((float*) (current_object_data+24))) + f*(*((float*) (current_object_data+28)))) > 0.0f);
                                                }
                                                if(script_temp_j)
                                                {
                                                    // It's either in front of us, or we've got wrap-around eyes...
                                                    e = (e*e) + (f*f);
                                                    if(!(arg_address[79] > 100 || (j & ACQUIRE_SEE_INVISIBLE)))
                                                    {
                                                        // Our target is invisible, and we can't see 'em...  But we can still smell 'em at close range...
                                                        script_temp_f = 225.0f;  // 15 foot radius...
                                                    }
                                                    // But is it within the specified radius?
                                                    if(e < script_temp_f)
                                                    {
                                                        // Shorten the distance depending on max hits of opponent
                                                        e /= (((unsigned short) arg_address[80]) + 1);


                                                        // Is it our best distance yet?
                                                        if(e < script_temp_e)
                                                        {
                                                            // Looks like we've got a new best character...
                                                            script_temp_i = i;
                                                            script_temp_e = e;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                push_int_stack(script_temp_i);  // Return the best character we found...
                break;
            case OPCODE_FINDPATH:
                // Modify gotoxy to be better...
                // Make sure we're dealin' with a character...
                m = FALSE; // return code...
                if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                {
                    m = room_findpath(roombuffer, (float*) current_object_data, (float*) (current_object_data+12), (float*) (current_object_data+12), current_object_data);
                }
                push_int_stack(m);
                break;
            case OPCODE_BUTTONPRESS:
                pop_int_stack(k);                                       // axis (or time)
                pop_int_stack(j);                                       // button
                pop_int_stack(i);                                       // code...
                // Make sure we're dealin' with a character...
                if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                {
                    script_temp_i = ((current_object_data - main_character_data[0]) / CHARACTER_SIZE);
                    character_button_function((unsigned short) script_temp_i, (unsigned char) i, (unsigned char) j, (unsigned char) k);
                }
                break;
            case OPCODE_AUTOAIM:
                pop_int_stack(k);                                       // function return type (X, Y, Z, or CRUNCH...  Must do CRUNCH first)
                pop_int_stack(j);                                       // cone angle (0-65535...  16384 is 90'...)
                pop_int_stack(i);                                       // aimer's dexterity (dexterity of 0 is completely random angle, dexterity 50 is aimed precisely at a target enemy, dexterity 25 is a 50-50 mix)
                pop_int_stack(script_temp_j);                           // aimer's team
                pop_int_stack(script_temp_i);                           // aimer's spin
                pop_float_stack(f);                                     // speed z
                pop_float_stack(e);                                     // speed xy

                // Are we crunchin', or just returning values?
                if(k < AUTOAIM_CRUNCH && k > -1)
                {
                    // Just returning values...
                    if(k == AUTOAIM_TARGET)
                    {
                        // I hope this works...  Converting a short to a float...
// !!!BAD!!!
// !!!BAD!!!  Check this...  May need a +0.5 or somethin'...
// !!!BAD!!!
                        push_float_stack(autoaim_target);
                    }
                    else
                    {
                        push_float_stack(autoaim_velocity_xyz[k]);
                    }
                }
                else
                {
                    // Doin' the big bad crunch...  Or trying anyway...  Make sure we're dealin' with a character first...
                    if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                    {
                        autoaim_helper(e, f, (unsigned short) script_temp_i, (unsigned char) script_temp_j, (unsigned char) i, (unsigned short) j, current_object_data[175], (unsigned char) k);
                    }
                    else
                    {
                        // Looks like we've got a particle...  0 for height...
                        autoaim_helper(e, f, (unsigned short) script_temp_i, (unsigned char) script_temp_j, (unsigned char) i, (unsigned short) j, (unsigned char) 0, (unsigned char) k);
                    }
                    push_float_stack(0.0f);
                }
                break;
            case OPCODE_ROOMHEIGHTXY:
                pop_float_stack(f);                                     // y position
                pop_float_stack(e);                                     // x position
                script_temp_e = room_heightmap_height(roombuffer, e, f);
                push_float_stack(script_temp_e);
                break;
            case OPCODE_WINDOWBORDER:
                // Draws a window border on the screen...
                pop_int_stack(k);                                       // border flags
                pop_int_stack(j);                                       // h
                pop_int_stack(i);                                       // w
                pop_float_stack(script_window_y);                       // y
                pop_float_stack(script_window_x);                       // x
                opcode = (unsigned char) int_stack[(unsigned char) (int_stack_head-1)];  // get_nopop_int_stack(opcode)...  // draggable...
                script_window_x = (script_window_x*script_window_scale)+(*((float*) current_object_data));
                script_window_y = (script_window_y*script_window_scale)+(*((float*) (current_object_data+4)));


                #ifdef DEVTOOL
                    script_window_w = i*script_window_scale;
                    script_window_h = j*script_window_scale;
                #endif


                // Allow the players to drag windows around...  Also check if the cursor is inside the window...
                if(opcode)
                {
                    if(mouse_down[BUTTON0] && mouse_last_object == current_object_data && mouse_last_item == DRAG_ITEM)
                    {
                        if(mouse_last_x > script_window_x-script_window_scale && mouse_last_y > script_window_y-script_window_scale)
                        {
                            if(mouse_last_x < (script_window_x+script_window_scale+(i*script_window_scale)) && mouse_last_y < (script_window_y+(j*script_window_scale)))
                            {
                                #ifdef DEVTOOL
                                    if(opcode != BORDER_DRAG)
                                    {
                                        // Modeler tools...
                                        if(opcode == BORDER_SELECT)
                                        {
                                            // Selection box thing
                                            if(selection_box_on == FALSE)
                                            {
                                                selection_box_on = TRUE;
                                                selection_box_tl[X] = mouse_x;
                                                selection_box_tl[Y] = mouse_y;
                                                selection_box_min[X] = script_window_x;
                                                selection_box_min[Y] = script_window_y;
                                                selection_box_max[X] = script_window_x+(i*script_window_scale);
                                                selection_box_max[Y] = script_window_y+(j*script_window_scale);
                                                if(key_down[SDLK_LSHIFT] == FALSE && key_down[SDLK_RSHIFT] == FALSE)
                                                {
                                                    select_clear();
                                                    room_select_clear();
                                                }
                                            }
                                            selection_box_br[X] = mouse_x;
                                            selection_box_br[Y] = mouse_y;
                                        }
                                        else if(opcode == BORDER_CROSS_HAIRS)
                                        {
                                            selection_close_type = BORDER_CROSS_HAIRS;
                                        }
                                        else if(opcode == BORDER_POINT_PICK)
                                        {
                                            if(select_pick_on == FALSE)
                                            {
                                                selection_close_type = BORDER_POINT_PICK;
                                            }
                                            select_pick_on = TRUE;
                                        }
                                        else if(opcode == BORDER_MOVE)
                                        {
                                            // Movement or rotation or scaling...
                                            if(select_move_on == FALSE)
                                            {
                                                selection_close_type = BORDER_CROSS_HAIRS;
                                                select_move_on = TRUE;
                                                rotation_view = selection_view;
                                            }
                                            else
                                            {
                                                if(rotation_view == selection_view)
                                                {
                                                    selection_close_type = BORDER_MOVE;
                                                }
                                            }
                                        }
                                    }
                                    else
                                #endif
                                    {
                                        // Allow dragging
                                        (*((float*) (current_object_data)))+=mouse_x-mouse_last_x;
                                        (*((float*) (current_object_data+4)))+=mouse_y-mouse_last_y;
                                        script_window_x += mouse_x-mouse_last_x;
                                        script_window_y += mouse_y-mouse_last_y;
                                        #ifdef DEVTOOL
                                            selection_box_on = FALSE;
                                        #endif
                                    }
                            }
                        }
                    }
                }
                if(mouse_x > script_window_x-script_window_scale && mouse_y > script_window_y-script_window_scale)
                {
                    if(mouse_x < (script_window_x+script_window_scale+(i*script_window_scale)) && mouse_y < (script_window_y+script_window_scale+(j*script_window_scale)))
                    {
                        mouse_current_object = current_object_data;
                        mouse_current_item = NO_ITEM;
                        if(mouse_pressed[BUTTON0] && mouse_last_object == current_object_data)
                        {
                            delay_promote();
                        }
                        if(opcode)
                        {
                            if(mouse_pressed[BUTTON0] || mouse_last_item == DRAG_ITEM)
                            {
                                mouse_current_item = DRAG_ITEM;
                            }
                        }
                    }
                }


                // Draw the window
                display_window(script_window_x, script_window_y, (float) i, (float) j, script_window_scale, (unsigned short) k);
                break;
            case OPCODE_WINDOWSTRING:
                // Draws a string within a window border...
                pop_int_stack_cast(arg_address, unsigned char*);        // string
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                pop_int_stack(i);                                       // color
                color_temp[0] = (i>>16);
                color_temp[1] = ((i>>8)&255);
                color_temp[2] = (i&255);
                display_color(color_temp);
                if(i == 0)
                {
                    // Black text should use the book texture...
                    display_pick_texture(texture_bookfnt);
                }
                else
                {
                    // Normal text
                    display_pick_texture(texture_ascii);
                }
                e = (e*script_window_scale)+script_window_x;
                f = (f*script_window_scale)+script_window_y;
                display_string(arg_address, e, f, script_window_scale);
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOWMINILIST:
                // Adds a mini list style input within a window border...
                pop_int_stack_cast(arg_address, unsigned char*);        // option string
                pop_int_stack(k);                                       // expansion state
                pop_int_stack(j);                                       // h
                pop_int_stack(i);                                       // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x

                m = 0;
                opcode = 0;
                if(j > 0 && i > 0)
                {
                    e = (e*script_window_scale)+script_window_x;
                    f = (f*script_window_scale)+script_window_y;
                    display_mini_list(arg_address, e, f, (unsigned char) i, (unsigned char) j, script_window_scale, (unsigned short) k);


                    // Input handling...
                    opcode = (unsigned char) k;  // The expansion information
                    keepgoing = 0;               // Count of mice not over item
                    m = k>>8;                    // The selected list item...


                    if(mouse_last_object==current_object_data && mouse_last_item==current_object_item)
                    {
                        // Able to handle input clicks...
                        if(mouse_pressed[BUTTON0] && opcode < 129)
                        {
                            opcode = 129;
                            // Delay window ordering effect until after all windows have been drawn...
                            delay_promote();
                        }
                        else
                        {
                            if(opcode > 128)
                            {
                                // List is expanding
                                if(opcode < 239)
                                {
                                    opcode += 16;
                                }
                                else
                                {
                                    // List is fully expanded...  Check for selection...
                                    script_temp_f = f + (1.5f*script_window_scale);
                                    opcode = 255;
                                    if(mouse_y > script_temp_f)
                                    {
                                        opcode = (unsigned char) ((mouse_y-script_temp_f)/script_window_scale);
                                        if(opcode < j)
                                        {
                                            script_temp_e = e + (script_window_scale*i);
                                            script_temp_f = script_temp_f + (script_window_scale*opcode);
                                            script_temp_y = script_temp_f + script_window_scale;
                                            display_highlight(e, script_temp_f, script_temp_e, script_temp_y);
                                        }
                                    }
                                    if(mouse_pressed[BUTTON0] || mouse_unpressed[BUTTON0])
                                    {
                                        // Did we pick something?
                                        if(opcode < j)
                                        {
                                            // Remember the selection, and make it shrink again...
                                            mouse_current_item = current_object_item;
                                            m = opcode;
                                            opcode = 128;
                                        }
                                        else
                                        {
                                            if(mouse_pressed[BUTTON0])
                                            {
                                                // Make it shrink again...
                                                opcode = 128;
                                            }
                                            else
                                            {
                                                // Unclick doesn't close on main entry...
                                                opcode = 255;
                                            }
                                        }
                                    }
                                    else
                                    {
                                        // Stay expanded
                                        opcode = 255;
                                    }
                                }
                            }
                            else
                            {
                                // Mouse is over button, so make it highlight...
                                opcode = 128;
                            }
                        }
                    }
                    else
                    {
                        // Mouse isn't over button, so make it shrink...
                        keepgoing++;
                    }



                    // Is it expanded???
                    if(opcode > 128)
                    {
                        // Yup...  Need to check even if cursor isn't inside window...
                        if(mouse_x > e && mouse_y > f)
                        {
                            if(mouse_x < (e+(i*script_window_scale)))
                            {
                                if(mouse_y < (f+((j+2)*script_window_scale)))
                                {
                                    mouse_current_object = current_object_data;
                                    mouse_current_item = current_object_item;
                                }
                            }
                        }
                    }
                    else
                    {
                        // Nope...  Only check if cursor is inside window...
                        if(mouse_current_object==current_object_data)
                        {
                            if(mouse_x > e && mouse_y > f)
                            {
                                if(mouse_x < (e+(i*script_window_scale)))
                                {
                                    if(mouse_y < (f+script_window_scale))
                                    {
                                        mouse_current_item = current_object_item;
                                    }
                                }
                            }
                        }
                    }



                    // Make the button shrink
                    if(keepgoing)
                    {
                        if(opcode > 128)
                        {
                            opcode = 128;
                        }
                        opcode-=4;
                        if(opcode > 128)  opcode = 0;
                    }
                    keepgoing = TRUE;  // so we don't kill the script...
                }

                current_object_item++;
                m = (m<<8)|opcode;
                push_int_stack(m);
                break;
            case OPCODE_WINDOWSLIDER:
                // Adds a slider style input within a window border...
                pop_int_stack(k);                                       // dinglethorpe position
                pop_int_stack(j);                                       // h
                pop_int_stack(i);                                       // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                e = (e*script_window_scale)+script_window_x;
                f = (f*script_window_scale)+script_window_y;
                display_slider(e, f, (unsigned char) i, (unsigned char) j, script_window_scale, ((float) k)/255.0f);


                // Input handling...
                if(i > j)
                {
                    e = e+(.5f*script_window_scale);
                }
                else
                {
                    f = f+(.5f*script_window_scale);
                }


                if(mouse_last_item == current_object_item && mouse_last_object == current_object_data)
                {
                    if(mouse_down[BUTTON0])
                    {
                        mouse_current_item = current_object_item;
                        mouse_current_object = current_object_data;
                        if(i > j)
                        {
                            // Horizontal...
                            script_temp_e = ((i-1)*script_window_scale);
                            k = (int) (((mouse_x-e)/script_temp_e)*255.0f);
                            if(k < 0) k = 0;
                            if(k > 255) k = 255;
                        }
                        else
                        {
                            // Vertical...
                            script_temp_f = ((j-1)*script_window_scale);
                            k = (int) (((mouse_y-f)/script_temp_f)*255.0f);
                            if(k < 0) k = 0;
                            if(k > 255) k = 255;
                        }
                    }
                    else
                    {
                        mouse_last_item = NO_ITEM;
                    }
                }
                else
                {
                    // Is the mouse inside the slider?
                    if(mouse_x > e && mouse_y > f)
                    {
                        if(i > j)
                        {
                            // Horizontal...
                            script_temp_e = e+((i-1)*script_window_scale);
                            if(mouse_x < script_temp_e && mouse_y < (f+script_window_scale))
                            {
                                if(mouse_pressed[BUTTON0])
                                {
                                    mouse_current_item = current_object_item;
                                    mouse_current_object = current_object_data;
                                    // Delay window ordering effect until after all windows have been drawn...
                                    delay_promote();
                                }
                            }
                        }
                        else
                        {
                            // Vertical
                            script_temp_f = f+((j-1)*script_window_scale);
                            if(mouse_x < (e+script_window_scale) && mouse_y < script_temp_f)
                            {
                                if(mouse_pressed[BUTTON0])
                                {
                                    mouse_current_item = current_object_item;
                                    mouse_current_object = current_object_data;
                                    // Delay window ordering effect until after all windows have been drawn...
                                    delay_promote();
                                }
                            }
                        }
                    }
                }


                current_object_item++;
                push_int_stack(k);
                break;
            case OPCODE_WINDOWIMAGE:
                pop_int_stack(k);                                       // current state
                pop_int_stack_cast(arg_address, unsigned char*);        // alt text string
                pop_int_stack_cast(call_address, unsigned char*);       // image file
                pop_float_stack(script_temp_f);                         // h
                pop_float_stack(script_temp_e);                         // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                e = (e*script_window_scale)+script_window_x;            // Top left x
                f = (f*script_window_scale)+script_window_y;            // Top left y
                script_temp_e=(script_window_scale*script_temp_e)+e;    // Bottom right x
                script_temp_f=(script_window_scale*script_temp_f)+f;    // Bottom right y



                // Draw the image...  Can pass NULL instead of a filename to act as a hotspot...
                if(call_address != NULL)
                {
                    if(((unsigned int) call_address) == 1)
                    {
                        // Kanji image...
                        if(arg_address)
                        {
                            display_color(black);
                        }
                        else
                        {
                            display_color(white);
                        }
                        display_kanji((unsigned short) k, e, f, script_temp_e-e, script_temp_f-f);
                        k = 0;
                        arg_address = 0;
                    }
                    else
                    {
                        // Normal image...
                        display_color(white);
                        call_address+=2;
                        display_image(e, f, script_temp_e, script_temp_f, *((unsigned int*) call_address));
                    }
                }


                // Check mouse over...
                m = 0;
                if(mouse_x > e && mouse_y > f)
                {
                    if(mouse_x < script_temp_e && mouse_y < script_temp_f)
                    {
                        mouse_current_object = current_object_data;
                        mouse_current_item = current_object_item;
                        if(mouse_last_object==current_object_data && mouse_last_item == current_object_item)
                        {
                            // Cap the alt text timer at 50...
                            if(k < 50)
                            {
                                k+=(main_frame_skip<<1);
                                if(k > 50)
                                {
                                    k = 50;
                                }
                            }
                            m++;


                            // Draw the alternate text...
                            if(k >= 50)
                            {
                                // Allow scripts to use NULL for alt text...
                                if(arg_address)
                                {
                                    // Display it later...  With cursor drawings...
                                    strcpy(mouse_text, arg_address);
                                }
                            }


                            // Handle clicks...
                            if(mouse_pressed[BUTTON0])
                            {
                                k = 255;
                                // Delay window ordering effect until after all windows have been drawn...
                                delay_promote();
                            }
                        }
                    }
                }


                // Kill alt text timer if there aren't any mice over it...
                if(m == 0)
                {
                    if(k > 250)
                    {
                        k--;
                    }
                    else
                    {
                        k = 0;
                    }
                }


                current_object_item++;
                push_int_stack(k);
                break;
            case OPCODE_WINDOWTRACKER:
                // The tracker tool for making .MUS files...
                pop_int_stack_cast(opcode, unsigned char);              // pan
                pop_int_stack(m);                                       // volume
                pop_int_stack(k);                                       // instrument
                pop_int_stack(j);                                       // seconds to display
                pop_int_stack(i);                                       // time offset
                pop_int_stack_cast(call_address, unsigned char*);       // MUS file data
                pop_float_stack(script_temp_f);                         // h
                pop_float_stack(script_temp_e);                         // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                #ifdef DEVTOOL
                    e = (e*script_window_scale)+script_window_x;            // Top left x
                    f = (f*script_window_scale)+script_window_y;            // Top left y
                    script_temp_e=(script_window_scale*script_temp_e)+e;    // Bottom right x
                    script_temp_f=(script_window_scale*script_temp_f)+f;    // Bottom right y
                    push_int_stack(tool_tracker(e, f, script_temp_e, script_temp_f, call_address, i, j, k, m, opcode));
                #else
                    push_int_stack(0);
                #endif
                current_object_item++;
                break;
            case OPCODE_WINDOWBOOK:
                // Draws the top 2/3 pages of a book and the associated text...
                pop_int_stack_cast(call_address, unsigned char*);       // filedata
                pop_int_stack(m);                                       // pageturn
                pop_int_stack(k);                                       // number of pages
                pop_int_stack(j);                                       // h (15)
                pop_int_stack(i);                                       // w (25)
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x

                opcode = (unsigned char) (m>>5);                        // The current page (must be less than number of pages...  0 if one page... )
                opcode = opcode << 1;                                   // Always an even number...  Page to display on right side...
                m = (m&31);                                             // The current frame
                e = (e*script_window_scale)+script_window_x;            // Top left x
                f = (f*script_window_scale)+script_window_y;            // Top left y
                script_temp_e = script_window_scale*.75f;
                script_temp_i = i*j;                                    // Page skip...

                                                                        // Filesize must be w*h*pages


                // Draw the pages...
                if(m != 0)
                {
                    // Left page
                    if(opcode != 0 && (opcode-1) < k) display_book_page(call_address+(script_temp_i*(opcode-1)), e, f, page_xy[PAGE_FRAME-1], script_temp_e, i, j, TRUE, (unsigned char) (opcode-1));
                    else                              display_book_page(NULL, e, f, page_xy[PAGE_FRAME-1], script_temp_e, i, j, TRUE, (unsigned char) (opcode-1));

                    // Right page
                    if((opcode+2) < k)  display_book_page(call_address+(script_temp_i*(opcode+2)), e, f, page_xy[0], script_temp_e, i, j, FALSE, (unsigned char) (opcode+2));
                    else                display_book_page(NULL, e, f, page_xy[0], script_temp_e, i, j, FALSE, (unsigned char) (opcode+2));

                    // Turning page
                    if(opcode < k)      display_book_page(call_address+(script_temp_i*opcode), e, f, page_xy[m], script_temp_e, i, j, FALSE, opcode);
                    else                display_book_page(NULL, e, f, page_xy[m], script_temp_e, i, j, FALSE, opcode);
                    if((opcode+1) < k)  display_book_page(call_address+(script_temp_i*(opcode+1)), e, f, page_xy[m], script_temp_e, i, j, TRUE, (unsigned char) (opcode+1));
                    else                display_book_page(NULL, e, f, page_xy[m], script_temp_e, i, j, TRUE, (unsigned char) (opcode+1));
                }
                else
                {
                    // Left page
                    if(opcode != 0 && (opcode-1) < k) display_book_page(call_address+(script_temp_i*(opcode-1)), e, f, page_xy[PAGE_FRAME-1], script_temp_e, i, j, TRUE, (unsigned char) (opcode-1));
                    else                              display_book_page(NULL, e, f, page_xy[PAGE_FRAME-1], script_temp_e, i, j, TRUE, (unsigned char) (opcode-1));

                    // Right page
                    if(opcode < k)  display_book_page(call_address+(script_temp_i*opcode), e, f, page_xy[0], script_temp_e, i, j, FALSE, opcode);
                    else            display_book_page(NULL, e, f, page_xy[0], script_temp_e, i, j, FALSE, opcode);
                }
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOWINPUT:
                pop_int_stack(k);                                       // next item offset
                pop_int_stack_cast(call_address, unsigned char*);       // string
                pop_int_stack(i);                                       // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x


                // Draw the box...
                e = (e*script_window_scale)+script_window_x;        // Top left x
                f = (f*script_window_scale)+script_window_y;        // Top left y


                // Is this the active input box?
                if(last_input_object == current_object_data && last_input_item == current_object_item)
                {
                    // Yup...  Draw cursor and check keyboard...
                    input_active = 3;
                    display_input(call_address, e, f, (signed char) i, script_window_scale, (signed char) last_input_cursor_pos);
                    if(last_input_cursor_pos >= 0 && last_input_cursor_pos < i)
                    {
                        opcode = input_read_key_buffer();
                        if(opcode != 0)
                        {
                            // We have input...
                            if(opcode == SDLK_BACKSPACE || opcode == SDLK_DELETE)
                            {
                                last_input_cursor_pos--;
                                if(last_input_cursor_pos < 0) last_input_cursor_pos = 0;
                                call_address[last_input_cursor_pos] = 0;
                            }
                            else
                            {
                                if(opcode == SDLK_RETURN)
                                {
                                    // Enter key...
                                    last_input_cursor_pos = 255;
                                    last_input_item+=k;  // Offset to next item
                                }
                                else if(last_input_cursor_pos < (i-1))
                                {
                                    // Some other key...
                                    call_address[last_input_cursor_pos] = opcode;
                                    last_input_cursor_pos++;
                                    call_address[last_input_cursor_pos] = 0;
                                }
                            }
                        }
                    }
                    push_int_stack(last_input_cursor_pos);
                    if(last_input_cursor_pos == 255)
                    {
                        last_input_cursor_pos = 0;
                    }
                }
                else
                {
                    // Nope... Don't draw cursor
                    display_input(call_address, e, f, (signed char) i, script_window_scale, (signed char) -1);
                    push_int_stack(-1);
                }


                // Check input handling...
                script_temp_e = e+(i*script_window_scale);
                script_temp_f = f+(script_window_scale);



                if(mouse_x > e && mouse_y > f)
                {
                    if(mouse_x < script_temp_e && mouse_y < script_temp_f)
                    {
                        mouse_current_object = current_object_data;
                        mouse_current_item = current_object_item;
                        if(mouse_pressed[BUTTON0] || mouse_pressed[1])
                        {
                            if(mouse_last_object==current_object_data && mouse_last_item == current_object_item)
                            {
                                // Only allow one input box to take from the keyboard...
                                last_input_object = current_object_data;
                                last_input_item = current_object_item;
                                input_quick_reset_key_buffer();
                                if(mouse_pressed[BUTTON0])
                                {
                                    // Keep the current string...
                                    last_input_cursor_pos = strlen(call_address);
                                }
                                else
                                {
                                    // Kill the current string...
                                    *call_address = 0;
                                    last_input_cursor_pos = 0;
                                }


                                // Delay window ordering effect until after all windows have been drawn...
                                delay_promote();
                            }
                        }
                    }
                }

                current_object_item++;
                break;
            case OPCODE_WINDOWEMACS:
#ifdef DEVTOOL
                pop_int_stack_cast(call_address, unsigned char*);       // file
                pop_int_stack(m);                                       // cursorx,cursory,scrolldown
                pop_int_stack(k);                                       // h
                pop_int_stack(i);                                       // w
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x

                // Draw the text editor...
                e = (e*script_window_scale)+script_window_x;            // Top left x
                f = (f*script_window_scale)+script_window_y;            // Top left y
                display_emacs(e, f, i, k, (unsigned char) ((m)&63), (unsigned char) ((m>>6)&63), (unsigned char) (m>>12), (unsigned short) ((m>>20)&4095), call_address, script_window_scale);


                // Handle cursor clicks
                script_temp_e = e+(script_window_scale*i);
                script_temp_f = f+(script_window_scale*k);
                script_temp_i = (i>>1)+i;
                script_temp_k = (k>>1)+k;
                j = ((m)&63);       // curx
                k = ((m>>6)&63);    // cury


                if(mouse_x > e && mouse_y > f)
                {
                    if(mouse_x < script_temp_e && mouse_y < script_temp_f)
                    {
                        mouse_current_object = current_object_data;
                        mouse_current_item = current_object_item;
                        if(mouse_pressed[BUTTON0])
                        {
                            if(mouse_last_object==current_object_data && mouse_last_item == current_object_item)
                            {
                                // Only allow one input box to take from the keyboard...
                                last_input_object = current_object_data;
                                last_input_item = current_object_item;
                                input_quick_reset_key_buffer();
                                last_input_cursor_pos = 0;

                                // Figure out the new cursor location...
                                j = (int) (((mouse_x-e)/(script_temp_e-e))*script_temp_i);
                                k = (int) (((mouse_y-f)/(script_temp_f-f))*script_temp_k);

                                // Delay window ordering effect until after all windows have been drawn...
                                delay_promote();
                            }
                        }
                    }
                }
                i = (m>>12)&255;    // xscroll
//                m = (m>>20)&2047;   // yscroll
                m = (m>>20)&4095;   // yscroll


                // Handle typematic input
                if(last_input_object == current_object_data && last_input_item == current_object_item)
                {
                    // Emacs is active...  Check cursor movement
                    input_active = 3;
                    if(key_pressed[SDLK_UP] || key_pressed[SDLK_KP8])
                    {
                        if(k > 4)  k--;
                        else if(m > 0)  m--;
                        else if(k > 0)  k--;
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_DOWN] || key_pressed[SDLK_KP2])
                    {
                        if(k < script_temp_k-4)  k++;
//                        else if(m < 2047)  m++;
                        else if(m < 4095)  m++;
                        else if(k < script_temp_k)  k++;
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_LEFT] || key_pressed[SDLK_KP4])
                    {
                        if(j > 4)  j--;
                        else if(i > 0)  i--;
                        else if(j > 0)  j--;
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_RIGHT] || key_pressed[SDLK_KP6])
                    {
                        if(j < script_temp_i-4)  j++;
                        else if(i < 255)  i++;
                        else if(j < script_temp_i)  j++;
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_HOME])
                    {
                        j = 0;
                        i = 0;
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_PAGEUP])
                    {
                        m-=script_temp_k-4;
                        if(m < 0)
                        {
                            k = 0;
                            m = 0;
                        }
                        emacs_buffer_write = 0;
                    }
                    if(key_pressed[SDLK_PAGEDOWN])
                    {
                        m+=script_temp_k-4;
//                        if(m > 2047) m = 2047;
                        if(m > 4095) m = 4095;
                        emacs_buffer_write = 0;
                    }


                    // Figure out the current cursor position in the data
                    word_temp[0] = (unsigned char) script_temp_k;
                    script_temp_k = m + k;
                    opcode = k;
                    k = 0;
                    script_temp_j = 1;
                    while(script_temp_k > 0 && k < TEXT_SIZE)
                    {
                        script_temp_j = strlen(call_address)+1;
                        k+=script_temp_j;
                        call_address+=script_temp_j;
                        script_temp_k--;
                    }
                    k = opcode;
                    script_temp_k = strlen(call_address);
                    opcode = 0;
                    if((j + i) > script_temp_k)
                    {
                        // Cursor is past end of text...
                        opcode = (j+i)-script_temp_k;
                        call_address+=script_temp_k;
                    }
                    else
                    {
                        call_address+=j+i;
                    }


                    // End key...  More difficult cursor movement...
                    if(key_pressed[SDLK_END])
                    {
                        if(opcode)
                        {
                            j+=i;
                            j-=opcode;
                        }
                        else
                        {
                            j+=i;
                            j+=strlen(call_address);
                        }
                        i = 0;
                        if(j > script_temp_i - 4)
                        {
                            i = j - (script_temp_i - 4);
                            j = script_temp_i - 4;
                        }
                        emacs_buffer_write = 0;
                    }


                    // Check normal key presses and insert/delete data...
                    script_temp_k = input_read_key_buffer();
                    if(script_temp_k != 0)
                    {
                        // We have input...
                        switch(script_temp_k)
                        {
                            case SDLK_BACKSPACE:
                                emacs_buffer_write = 0;
                                if(i == 0 && j == 0)
                                {
                                    // Go to end of last line...
                                    j+=script_temp_j-1;
                                    if(j > script_temp_i-4)
                                    {
                                        i = j - (script_temp_i - 4);
                                        j = script_temp_i - 4;
                                    }

                                    // Move the cursor...
                                    if(k > 4) { k--;  call_address--; }
                                    else if(m > 0) { m--;  call_address--; }
                                    else if(k > 0) { k--;  call_address--; }
                                    else break;  // No data to left
                                }
                                else
                                {
                                    // Move the cursor
                                    if(j > 4)  { j--; call_address--; }
                                    else if(i > 0)  { i--; call_address--; }
                                    else { j--; call_address--; }
                                }
                                // Fall through to delete...
                            case SDLK_DELETE:
                                // Delete the text
                                emacs_buffer_write = 0;
                                if(opcode == 0)
                                {
                                    sdf_insert_data(call_address, NULL, -1);
                                }
                                break;
                            case SDLK_RETURN:
                                emacs_buffer_write = 0;
                                script_temp_k = word_temp[0];
                                word_temp[0] = 0;
                                sdf_insert_data(call_address, word_temp, 1);
                                j = 0;
                                i = 0;
                                if(k < script_temp_k-4)  k++;
                                else if(m < 2047)  m++;
                                else if(k < script_temp_k)  k++;
                                break;
                            default:
                                if((key_down[SDLK_RCTRL] || key_down[SDLK_LCTRL]) && (script_temp_k == SDLK_x || script_temp_k == SDLK_k || script_temp_k == SDLK_c || script_temp_k == SDLK_v || script_temp_k == SDLK_h || script_temp_k == SDLK_b))
                                {
                                    if(script_temp_k == SDLK_v)
                                    {
                                        emacs_paste(call_address);
                                        emacs_buffer_write = 0;
                                        while(emacs_buffer_write < emacs_return_count)
                                        {
                                            if(k < word_temp[0]-4)  k++;
                                            else if(m < 2047)  m++;
                                            else if(k < word_temp[0])  k++;
                                            emacs_buffer_write++;
                                        }
                                        emacs_buffer_write = 0;
                                    }
                                    else if(script_temp_k == SDLK_c)
                                    {
                                        emacs_copy(call_address);
                                    }
                                    else if(script_temp_k == SDLK_b || script_temp_k == SDLK_h)
                                    {
                                        if(k < word_temp[0]-4)  k++;
                                        else if(m < 2047)  m++;
                                        else if(k < word_temp[0])  k++;
                                        emacs_buffer_write = 0;
                                        word_temp[0] = ' ';
                                        while(opcode > 0)
                                        {
                                            sdf_insert_data(call_address, word_temp, 1);
                                            call_address++;
                                            opcode--;
                                        }
                                        if(script_temp_k == SDLK_b)
                                        {
                                            sdf_insert_data(call_address, emacs_bad, EMACS_BAD_SIZE);
                                        }
                                        else
                                        {
                                            sdf_insert_data(call_address, emacs_horiz, EMACS_HORIZ_SIZE);
                                        }
                                    }
                                    else
                                    {
                                        emacs_copy(call_address);
                                        emacs_delete(call_address);
                                    }
                                }
                                else
                                {
                                    emacs_buffer_write = 0;
                                    word_temp[0] = ' ';
                                    while(opcode > 0)
                                    {
                                        sdf_insert_data(call_address, word_temp, 1);
                                        call_address++;
                                        opcode--;
                                    }
                                    word_temp[0] = (unsigned char) script_temp_k;
                                    sdf_insert_data(call_address, word_temp, 1);
                                    if(j < script_temp_i-4)  j++;
                                    else if(i < 255)  i++;
                                    else if(j < script_temp_i)  j++;
                                }
                                break;
                        }
                    }
                }
                else
                {
                    // Emacs is not active...
                    j = 63;
                    k = 63;
                }
                current_object_item++;


                // Throw back the new cursor data
                j = j&63;
                j|= ((k&63)<<6);
                j|= ((i&255)<<12);
//                j|= ((m&2047)<<20);
//                j|= ((m&4095)<<20);
                j = ((unsigned int) j)  |  (((unsigned int) (m&4095))<<20);
                push_int_stack(j);
#endif
                break;
            case OPCODE_WINDOWMEGAIMAGE:
                // Draw a deformable, blendable, colorable image...
                pop_int_stack_cast(call_address, unsigned char*);       // Image file
                pop_int_stack(k);                                       // Alpha + Blend mode
                pop_int_stack(i);                                       // Color

                // Texture coordinates
                pop_float_stack(f);                                     // ty
                pop_float_stack(e);                                     // tx
                script_texpoint[2][Y] = f;                              // t2y
                script_texpoint[2][X] = e;                              // t2x
                script_texpoint[3][Y] = f;                              // t3y
                script_texpoint[1][X] = e;                              // t1x
                pop_float_stack(f);                                     // ty
                pop_float_stack(e);                                     // tx
                script_texpoint[0][Y] = f;                              // t0y
                script_texpoint[0][X] = e;                              // t0x
                script_texpoint[1][Y] = f;                              // t1y
                script_texpoint[3][X] = e;                              // t3x

                // Shape coordinates
                pop_float_stack(f);                                     // p3y
                pop_float_stack(e);                                     // p3x
                script_point[3][Y] = (f*script_window_scale)+script_window_y;
                script_point[3][X] = (e*script_window_scale)+script_window_x;
                pop_float_stack(f);                                     // p3y
                pop_float_stack(e);                                     // p3x
                script_point[2][Y] = (f*script_window_scale)+script_window_y;
                script_point[2][X] = (e*script_window_scale)+script_window_x;
                pop_float_stack(f);                                     // p3y
                pop_float_stack(e);                                     // p3x
                script_point[1][Y] = (f*script_window_scale)+script_window_y;
                script_point[1][X] = (e*script_window_scale)+script_window_x;
                pop_float_stack(f);                                     // p3y
                pop_float_stack(e);                                     // p3x
                script_point[0][Y] = (f*script_window_scale)+script_window_y;
                script_point[0][X] = (e*script_window_scale)+script_window_x;



                color_temp[0] = (i>>16);
                color_temp[1] = ((i>>8)&255);
                color_temp[2] = (i&255);
                if(k)
                {
                    color_temp[3] = (unsigned char) k;
                    display_color_alpha(color_temp);
                    if(k & ALPHA_LIGHT)
                    {
                        display_blend_light();
                    }
                }
                else
                {
                    display_color(color_temp);
                }
                if(call_address == NULL)
                {
                    display_texture_off();
                    // Draw the image
                    display_start_fan();
                        display_vertex_xy(script_point[0][X], script_point[0][Y]);
                        display_vertex_xy(script_point[1][X], script_point[1][Y]);
                        display_vertex_xy(script_point[2][X], script_point[2][Y]);
                        display_vertex_xy(script_point[3][X], script_point[3][Y]);
                    display_end();
                    display_texture_on();
                }
                else
                {
                    call_address+=2;
                    display_pick_texture(*((unsigned int*) call_address))
                    // Draw the image
                    display_start_fan();
                        display_texpos_xy(script_texpoint[0][X], script_texpoint[0][Y]);  display_vertex_xy(script_point[0][X], script_point[0][Y]);
                        display_texpos_xy(script_texpoint[1][X], script_texpoint[1][Y]);  display_vertex_xy(script_point[1][X], script_point[1][Y]);
                        display_texpos_xy(script_texpoint[2][X], script_texpoint[2][Y]);  display_vertex_xy(script_point[2][X], script_point[2][Y]);
                        display_texpos_xy(script_texpoint[3][X], script_texpoint[3][Y]);  display_vertex_xy(script_point[3][X], script_point[3][Y]);
                    display_end();
                }
                if(k & ALPHA_LIGHT)
                {
                    display_blend_trans();
                }
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOW3DSTART:
                pop_int_stack_cast(opcode, unsigned char);      // Detail level override  !!!BAD!!!  Unused...
                pop_float_stack(script_temp_f);                 // H
                pop_float_stack(script_temp_e);                 // W
                pop_float_stack(f);                             // Y
                pop_float_stack(e);                             // X

                e = (e*script_window_scale)+script_window_x;            // Top left x (0 - 400)
                f = (f*script_window_scale)+script_window_y;            // Top left y (0 - 300)
                script_temp_e = script_temp_e * script_window_scale;    // W (0 - 400)
                script_temp_f = script_temp_f * script_window_scale;    // H (0 - 300)


                // Setup default lighting...
                global_render_light_color_rgb[0] = 255;
                global_render_light_color_rgb[1] = 255;
                global_render_light_color_rgb[2] = 255;
                global_render_light_offset_xy[X] = 0.75f;  // 0.25f;
                global_render_light_offset_xy[Y] = 0.60f;  // 0.38f;


// global_render_light_offset_xy[X] = ((screen_x - mouse_x) * 0.00125f);
// global_render_light_offset_xy[Y] = ((screen_y - mouse_y) * 0.0016666f);
// sprintf(DEBUG_STRING, "%f, %f", global_render_light_offset_xy[X], global_render_light_offset_xy[Y]);


                // Remember the position of the new viewport...
                window3d_x = e;
                window3d_y = f;
                window3d_w = script_temp_e;
                window3d_h = script_temp_f;


                // 400x300 notation to true screen coordinates
                return_float = screen_x / 400.0f;
                e = e * return_float;
                script_temp_e = script_temp_e * return_float;
                return_float = screen_y / 300.0f;
                f = f * return_float;
                script_temp_f = script_temp_f * return_float;


                // GL Likes lower left corner...  Cartesian coordinates
                f = screen_y - f - script_temp_f;


                // Set viewport
                display_viewport((int) e, (int) f, (int) script_temp_e, (int) script_temp_f);
                display_zbuffer_on();


                // Make depth work...
                e = window3d_order*(0.5f/((float)MAX_WINDOW));
                display_depth_overlay(e);
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOW3DEND:
                glLoadMatrixf(window_camera_matrix);
                display_viewport(0,0,screen_x,screen_y);
                display_zbuffer_off();
                display_depth_scene();
                display_blend_trans();
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOW3DPOSITION:
                pop_int_stack_cast(opcode, unsigned char);      // Position type
                pop_float_stack(script_temp_e);                 // Z
                pop_float_stack(f);                             // Y
                pop_float_stack(e);                             // X
                if(opcode == WIN_CAMERA)
                {
                    window3d_camera_xyz[X] = e;
                    window3d_camera_xyz[Y] = f;
                    window3d_camera_xyz[Z] = script_temp_e;
                    display_look_at(window3d_camera_xyz, window3d_target_xyz);
                }
                else if(opcode == WIN_TARGET)
                {
                    window3d_target_xyz[X] = e;
                    window3d_target_xyz[Y] = f;
                    window3d_target_xyz[Z] = script_temp_e;
                }
                else if(opcode == WIN_LIGHT)
                {
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
                }
                else if(opcode == WIN_ORTHO)
                {
                    // Special for modeler...
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glOrtho(-e, e, -f, f, ZNEAR, script_temp_e);
                    glMatrixMode(GL_MODELVIEW);
                }
                else if(opcode == WIN_FRUSTUM)
                {
                    // Special for modeler...
                    glMatrixMode(GL_PROJECTION);
                    glLoadIdentity();
                    glFrustum(-e, e, -f, f, ZNEAR, script_temp_e);
                    screen_frustum_x = e;
                    screen_frustum_y = f;
                    glMatrixMode(GL_MODELVIEW);
                }
                #ifdef DEVTOOL
                    else
                    {
                        // Special for modeler...
                        glScalef(e, f, script_temp_e);
                    }
                #endif
                push_int_stack(TRUE);
                break;
            case OPCODE_WINDOW3DMODEL:
                pop_int_stack(k);                                                           // Main alpha
                pop_int_stack(j);                                                           // Current frame
                pop_int_stack(i);                                                           // Character model data block offset...  First entry is RDY filestart...
                call_address = current_object_data + i;                                     // Actual position of model data block...
                pop_int_stack_cast(opcode, unsigned char);                                  // Mode
                if(*((unsigned char**) call_address) == NULL) break;
                switch(opcode & 240)
                {
                    case WIN_3D_SHADOW:
                        display_blend_trans();
                        display_cull_off();
                        render_rdy_shadow(*((unsigned char**) call_address), (unsigned short) j, 0.0f, 0.0f, 0.0f, opcode);
                        display_cull_on();
                        break;
                    case WIN_3D_MODEL:
                        // Figure out the matrix to use...
                        if(i == MODEL_BASE_FILE)
                        {
                            // Clear the base matrix...
                            script_matrix_clear();
                            render_generate_model_world_data(*((unsigned char**) call_address), (unsigned short) j, script_matrix, mainbuffer);  // Generate new bone frame in mainbuffer
                            render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, mainbuffer, 0, 0);
                        }
                        else if(i == MODEL_RIDER_FILE)
                        {
                            // Set the base matrix to attach to the saddle bone...
                            script_matrix_clear();
                            script_matrix_from_bone(SADDLE_BONE);  // Uses data in mainbuffer from last base...
                            render_generate_model_world_data(*((unsigned char**) call_address), (unsigned short) j, script_matrix, mainbuffer);  // Generate new bone frame in mainbuffer
                            render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, mainbuffer, 0, 0);
                        }
                        else if(i >= MODEL_LEFT_FILE)
                        {
                            // Use a temporary matrix to attach to the desired weapon bone...
                            script_matrix_clear();
                            script_matrix_from_bone((unsigned char) (1 + ((i - MODEL_LEFT_FILE)/24)));  // Uses data in mainbuffer from last base...
                            render_generate_model_world_data(*((unsigned char**) call_address), (unsigned short) j, script_matrix, subbuffer);  // Generate new bone frame in subbuffer
                            i = global_num_bone;
                            arg_address = global_bone_data;
                            render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, subbuffer, 0, 0);
                            global_bone_data = arg_address;
                            global_num_bone = i;
                        }
                        else
                        {
                            // Overlaps base bones...
                            render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, mainbuffer, 0, 0);
                        }
                        break;
#ifdef DEVTOOL
                    case WIN_3D_AXIS:
                        display_texture_off();
                        render_axis();
                        render_bounding_box();
                        display_texture_on();
                        break;
                    case WIN_3D_BONE:
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
                    case WIN_3D_VERTEX:
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
                    case WIN_3D_BONE_UPDATE:
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
                    case WIN_3D_TEXVERTEX:
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
                    case WIN_3D_SKIN_FROM_CAMERA:
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, opcode, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
                    case WIN_3D_MODEL_EDIT:
                        script_matrix_clear();
                        render_generate_model_world_data(*((unsigned char**) call_address), (unsigned short) j, script_matrix, mainbuffer);  // Generate new bone frame in mainbuffer (so others attach correctly)
                        render_rdy(*((unsigned char**) call_address), (unsigned short) j, WIN_3D_MODEL, (unsigned char**) (call_address+4), (unsigned char) k, NULL, 0, 0);
                        break;
#endif
                    default:
                        break;
                }
                push_int_stack(TRUE);
                break;
            case OPCODE_MODELASSIGN:
                // Set the color and textures for a given character or window...
                pop_int_stack(j);                               // Color or Texture filestart
                pop_int_stack(i);                               // Location to stick the color or texture...
                *((unsigned int*) i) = (unsigned int) j;
                break;
            case OPCODE_PARTICLEBOUNCE:
                // Reflects a particle off collision_normal_xyz, changing its velocities...
// !!!BAD!!!
// !!!BAD!!!  Should probably axe this...
// !!!BAD!!!
//                incoming_xyz = ((float*) (current_object_data+24));
//                e = 2.0f * dot_product(incoming_xyz, collision_normal_xyz);
//                incoming_xyz[X] -= collision_normal_xyz[X]*e;
//                incoming_xyz[Y] -= collision_normal_xyz[Y]*e;
//                incoming_xyz[Z] -= collision_normal_xyz[Z]*e;
//                push_int_stack(collision_normal_xyz[Z] > 0.707f)  // Return landability...
push_int_stack(TRUE);
                break;
            case OPCODE_WINDOWEDITKANJI:
                // Scripted vector font tool...
                pop_int_stack(j);                                       // Font to edit
                pop_float_stack(script_temp_f);                         // Scale
                pop_float_stack(f);                                     // y
                pop_float_stack(e);                                     // x
                pop_int_stack(i);                                       // Edit mode


                #ifdef DEVTOOL
                    script_temp_f*=script_window_scale;
                    e = (e*script_window_scale)+script_window_x;            // Top left x
                    f = (f*script_window_scale)+script_window_y;            // Top left y
                    tool_kanjiedit((unsigned short) j, e, f, script_temp_f, (unsigned short) i);
                #endif
                break;
            case OPCODE_WINDOW3DROOM:
                pop_int_stack(k);               // The ROOM_MODE_??? thing
                pop_int_stack(j);               // The rotation (0-65535)
                pop_int_stack(i);               // The RGB color
                pop_int_stack_cast(call_address, unsigned char*);  // The start of the SRF file...
                pop_float_stack(script_temp_e); // z center position
                pop_float_stack(f);             // y center position
                pop_float_stack(e);             // x center position

                color_temp[0] = (i>>16)&255;    // Red
                color_temp[1] = (i>>8)&255;     // Green
                color_temp[2] = (i)&255;        // Blue
                color_temp[3] = 255;            // Alpha
                if(k == ROOM_MODE_MINIMAP)
                {
                    display_texture_off();
                }
                room_draw_srf(e, f, script_temp_e, call_address, color_temp, (unsigned short) j, (unsigned char) k);
                if(k == ROOM_MODE_MINIMAP)
                {
                    display_texture_on();
                }
                break;
            case OPCODE_INDEXISLOCALPLAYER:
                // Return TRUE if argument is one of the player characters
                pop_int_stack(j);                                       // Character index
                i = FALSE;
                if(j < MAX_CHARACTER)
                {
                    repeat(k, MAX_LOCAL_PLAYER)
                    {
                        if(player_device_type[k])
                        {
                            if(local_player_character[k] == (unsigned short) j)
                            {
                                i = TRUE;
                                k = MAX_LOCAL_PLAYER;
                            }
                        }
                    }
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDBINDING:
                i = *((unsigned short*) (current_object_data + 220));
                if(i < MAX_CHARACTER)
                {
                    if(main_character_on[i])
                    {
                        i = (int) main_character_data[i];
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDTARGET:
                i = *((unsigned short*) (current_object_data + 62));
                if(i < MAX_CHARACTER)
                {
                    if(main_character_on[i])
                    {
                        i = (int) main_character_data[i];
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDOWNER:
                i = *((unsigned short*) (current_object_data + 76));
                if(i < MAX_CHARACTER)
                {
                    if(main_character_on[i])
                    {
                        i = (int) main_character_data[i];
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDINDEX:
                // Returns 0-MAX_CHARACTER, given a pointer to character data...
                // Returns 0-MAX_PARTICLE, given a pointer to particle data...
                // Returns 65535 if bad...
                pop_int_stack_cast(call_address, unsigned char*);
                i = 65535;
                if(call_address >= main_character_data[0] && call_address <= main_character_data[MAX_CHARACTER-1])
                {
                    i = (call_address-main_character_data[0])/CHARACTER_SIZE;
                    i = i & (MAX_CHARACTER-1);
                }
                else if(call_address >= main_particle_data[0] && call_address <= main_particle_data[MAX_PARTICLE-1])
                {
                    i = (call_address-main_particle_data[0])/PARTICLE_SIZE;
                    i = i & (MAX_PARTICLE-1);
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDBYINDEX:
                // Returns a pointer to a character, given the character's index number
                pop_int_stack(i);
                if(i < MAX_CHARACTER)
                {
                    if(main_character_on[i])
                    {
                        i = (int) main_character_data[i];
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDWINDOW:
                // Returns a pointer to a window's data, given either (or both) the window's binding character index,
                // or the window's script data start...  Returns first match, or NULL if none was found...
                pop_int_stack_cast(call_address, unsigned char*);   // Window script data start...
                pop_int_stack(k);                                   // Window's binding character index...


//log_message("INFO:   FindWindow(binding == %d, script == %d)", k, (int) call_address);
                i = 0;
                repeat(script_temp_i, main_used_window_count)
                {
                    j = main_window_order[script_temp_i];
//log_message("INFO:     Checking window %d (binding == %d, script == %d)", j, (*((unsigned short*) (main_window_data[j]+220))), (int) main_window_script_start[j]);
                    if(call_address == main_window_script_start[j] || call_address == NULL)
                    {
//log_message("INFO:     Passed script");
                        if(k == (*((unsigned short*) (main_window_data[j]+220))) || k >= MAX_CHARACTER)
                        {
//log_message("INFO:     Passed binding");
                            i = (int) main_window_data[j];
                            script_temp_i = main_used_window_count;
                        }
                    }
                }
                push_int_stack(i);
                break;
            case OPCODE_FINDPARTICLE:
                // Returns a pointer to a particle, given the particle's index number
                pop_int_stack(i);
                if(i < MAX_PARTICLE)
                {
                    if(main_particle_on[i])
                    {
                        i = (int) main_particle_data[i];
                    }
                    else
                    {
                        i = 0;
                    }
                }
                else
                {
                    i = 0;
                }
                push_int_stack(i);
                break;
            case OPCODE_ATTACHTOTARGET:
                // Attaches a particle to its target (for stickin' arrows in characters...)
                // If m is TRUE, the particle attaches to the best non-standard bone of the
                // character (for stickin' attack particles to a weapon)
                pop_int_stack(m);       // The bone type to attach to (0 for standard bones, 1 or 2 for left/right weapons, 255 for special attach-to-vertex thing...)


                i = FALSE;
                if(current_object_data >= main_particle_data[0] && current_object_data <= main_particle_data[MAX_PARTICLE-1])
                {
                    j = *((unsigned short*) (current_object_data+62));
                    if(j < MAX_CHARACTER)
                    {
                        k = (current_object_data - main_particle_data[0]) / PARTICLE_SIZE;
                        k = k & (MAX_PARTICLE-1);  // Should give particle...
                        i = particle_attach_to_character((unsigned short) k, (unsigned short) j, (unsigned char) m);
                    }
                }
                push_int_stack(i);
                break;
            case OPCODE_GETDIRECTION:
                // Returns the direction from a character to an XY spot...  Returns 0 - 3,
                // For Front, Back, Left, Right...
                pop_float_stack(f);         // Y
                pop_float_stack(e);         // X


                // Make sure we're dealin' with a character...
                i = 0;
                if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                {
                    // XY Vector to spot...
                    e -= *((float*) (current_object_data));
                    f -= *((float*) (current_object_data+4));


                    // Dot vector with front and side normals...
                    script_temp_e = (e * (*((float*) (current_object_data+120)))) + (f * (*((float*) (current_object_data+124))));  // Front dot...
                    script_temp_f = (e * (*((float*) (current_object_data+108)))) + (f * (*((float*) (current_object_data+112))));  // Side dot...
                    e = script_temp_e;
                    f = script_temp_f;
                    ABS(e);
                    ABS(f);
                    if(e > f)
                    {
                        // Forward or backwards...
                        i = (script_temp_e > 0.0f) ? DIRECTION_FRONT : DIRECTION_BACK;
                    }
                    else
                    {
                        // Left or right...
                        i = (script_temp_f > 0.0f) ? DIRECTION_LEFT : DIRECTION_RIGHT;
                    }
                }

                push_int_stack(i);
                break;
            case OPCODE_DAMAGETARGET:
                // Does some damage to the target character...
                pop_int_stack(k);           // wound_amount
                pop_int_stack(j);           // damage_amount
                pop_int_stack(i);           // damage_type


                opcode = current_object_data[78];  // The team...
                m = *((unsigned short*) (current_object_data+62));  // The target
                if(m < MAX_CHARACTER)
                {
                    if(main_character_on[((unsigned short) m)])
                    {
                        // Looks like target is valid...  Find out who the attacker is...
                        global_attacker = MAX_CHARACTER;
                        global_attack_spin = (*((unsigned short*) (main_character_data[(unsigned short) m] + 56))) + 32768;
                        script_temp_i = TRUE;
                        if(current_object_data >= main_particle_data[0] && current_object_data <= main_particle_data[MAX_PARTICLE-1])
                        {
                            // Thing doin' damage is a particle...
                            global_attacker = *((unsigned short*) (current_object_data + 76));  // particle's owner is doing attack...
                            if((*((unsigned short*) (current_object_data + 60))) & PART_ATTACK_SPIN_ON)
                            {
                                // Set global attack spin from particle's location instead of owner's...
                                script_temp_i = FALSE;
                            }
                        }
                        else if(current_object_data >= main_character_data[0] && current_object_data <= main_character_data[MAX_CHARACTER-1])
                        {
                            // Thing doin' damage is a character...
                            global_attacker = ((current_object_data - main_character_data[0]) / CHARACTER_SIZE) & (MAX_CHARACTER-1);  // character is doing attack...
                            if(global_attacker == m)
                            {
                                // Character is hurting itself...  We'll allow that as non-friendly fire...
                                if(opcode == TEAM_GOOD)
                                {
                                    opcode = TEAM_MONSTER;
                                }
                                else
                                {
                                    opcode = TEAM_GOOD;
                                }
                            }
                        }
                        if(global_attacker < MAX_CHARACTER)
                        {
                            if(main_character_on[global_attacker])
                            {
                                if(script_temp_i)
                                {
                                    // Direction from attacker's position relative to target...
                                    e = (*((float*) (main_character_data[((unsigned short) m)]))) - (*((float*) (main_character_data[global_attacker])));
                                    f = (*((float*) (main_character_data[((unsigned short) m)]+4))) - (*((float*) (main_character_data[global_attacker]+4)));
                                }
                                else
                                {
                                    // Direction from particle's position relative to target...
                                    e = (*((float*) (main_character_data[((unsigned short) m)]))) - (*((float*) (current_object_data)));
                                    f = (*((float*) (main_character_data[((unsigned short) m)]+4))) - (*((float*) (current_object_data+4)));
                                }
                                global_attack_spin = (unsigned short) (((float) atan2(f, e)) * (65535.0f / (2.0f*PI)));
                            }
                            else
                            {
                                global_attacker = MAX_CHARACTER;
                            }
                        }
                        else
                        {
                            global_attacker = MAX_CHARACTER;
                        }
                        damage_character((unsigned short) m, (unsigned char) i, (unsigned short) j, (unsigned short) k, opcode);
                    }
                }
                break;
            case OPCODE_EXPERIENCEFUNCTION:
                // Gives some experience to a character...
                pop_int_stack(k);           // affect entire team
                pop_int_stack(j);           // experience_amount
                pop_int_stack(i);           // experience_type
                pop_int_stack(m);           // target of function (character index number)
                if(k)
                {
                    // Give experience to all of target's enemies (characters on different team than target)...
                    if(((unsigned short) m) < MAX_CHARACTER)
                    {
                        if(main_character_on[m])
                        {
                            k = main_character_data[m][78];  // k is now target's team...
                            repeat(m, MAX_CHARACTER)
                            {
                                if(main_character_on[m] && main_character_data[m][78] != ((unsigned char) k))
                                {
                                    // Character is active and on different team than target...
                                    experience_function_character((unsigned short) m, (unsigned char) i, (signed short) j, FALSE);
                                }
                            }
                        }
                    }
                    i = 0;
                }
                else
                {
                    // Give experience to just target...
                    i = experience_function_character((unsigned short) m, (unsigned char) i, (signed short) j, TRUE);
                }
                push_int_stack(i);
                break;
            default:
                // If it got here, the opcode is an operand value of some type...
                // Figure out what type and push it to the appropriate stack...
                switch(opcode&224)
                {
                    case 128:
                        // It's an integer variable
                        push_int_stack(int_variable[opcode&(MAX_VARIABLE-1)]);
                        break;
                    case 160:
                        // It's a property...  Get our memory address from the variable...
                        arg_address = (unsigned char*) int_variable[opcode&(MAX_VARIABLE-1)];

                        // Then get the property number from the extension...
                        opcode = *address;
                        address++;

                        // Offset the arg_address to the correct memory location...
                        arg_address+=property_offset[opcode];
                        switch(property_type[opcode])
                        {
                            case VAR_INT:
                                // Push the integer value at arg_address...
                                push_int_stack( (*((int*) arg_address)) );
                                break;
                            case VAR_FLOAT:
                                // Push the float value at arg_address...
                                push_float_stack( (*((float*) arg_address)) );
                                break;
                            case VAR_BYTE:
                                // Push the byte value at arg_address...
                                push_int_stack( (*((unsigned char*) arg_address)) );
                                break;
                            case VAR_WORD:
                                // Push the word value at arg_address...
                                push_int_stack( (*((unsigned short*) arg_address)) );
                                break;
                            default:
                                // Strings and text are put on stack as a pointer...  Can be used with FileWriteByte...
                                push_int_stack( ((signed int) arg_address) );
                                break;
                        }
                        break;
                    case 192:
                        // It's a float variable
                        push_float_stack(float_variable[opcode&(MAX_VARIABLE-1)]);
                        break;
                    case 224:
                        // It's an extended opcode...
                        switch(opcode & 31)
                        {
                            case 0:
                                // Integer 0
                                push_int_stack(0);
                                break;
                            case 1:
                                // Integer 1
                                push_int_stack(1);
                                break;
                            case 2:
                                // Float 1.0
                                push_float_stack(1);
                                break;
                            case 3:
                                // Integer token, 1 byte signed
                                i = *((signed char*) address);  address++;
                                push_int_stack(i);
                                break;
                            case 4:
                                // Integer token, 2 byte signed
                                i = ((signed short) sdf_read_unsigned_short(address));  address+=2;
                                push_int_stack(i);
                                break;
                            case 5:
                                // Integer token, 4 byte signed
                                i = ((signed int) sdf_read_unsigned_int(address));  address+=4;
                                push_int_stack(i);
                                break;
                            case 6:
                                // Float token, 4 byte IEEE
                                f = sdf_read_float(address);  address+=4;
                                push_float_stack(f);
                                break;
                            case 7:
                                // String token, Zero terminated
                                break;
                            default:
                                // Invalid opcode
                                log_message("ERROR:  Invalid extended at 0x%04x", opcode, address-file_start);
                                break;
                        }
                        break;
                    default:
                        // Invalid opcode
                        log_message("ERROR:  Invalid opcode 0x%02x at 0x%04x", opcode, address-file_start);
                        break;
                }
                break;
        }
    }



    // Return TRUE if return_int is set, FALSE if return_float is set
    return return_int_is_set;
}

//-----------------------------------------------------------------------------------------------
