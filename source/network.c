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

// <ZZ> This file contains functions to handle networking
//      network_blah			- Blah

// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
#define ALLOW_LOCAL_PACKETS
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!


#define UDP_PORT 17859          // Orangeville, PA
#define TCPIP_PORT 30628        // Colbert, GA
#ifdef LIL_ENDIAN
    #define LOCALHOST ((127) | (0<<8) | (0<<16) | (1<<24))
#else
    #define LOCALHOST ((127<<24) | (0<<16) | (0<<8) | (1))
#endif
#define MAX_REMOTE       1024   // Maximum number of network'd computers...





unsigned char network_on;
unsigned char* netlist = NULL;


IPaddress       local_address;
IPaddress       main_server_address;
TCPsocket       main_server_socket;
unsigned char   main_server_on = FALSE;
//#define MAIN_SERVER_NAME "www.aaronbishopgames.com"
#define MAIN_SERVER_NAME "FooFoo"


UDPsocket       remote_socket;
IPaddress	    remote_address[MAX_REMOTE];
unsigned short	remote_room_number[MAX_REMOTE];
unsigned char	remote_is_neighbor[MAX_REMOTE];
unsigned char	remote_on[MAX_REMOTE];
unsigned short  num_remote = 0;




#define MAX_PACKET_SIZE 8192
#define PACKET_HEADER_SIZE  3
#define PACKET_TYPE_CHAT                    0
#define PACKET_TYPE_I_AM_HERE               1
#define PACKET_TYPE_ROOM_UPDATE             2
#define PACKET_TYPE_I_WANNA_PLAY            3
#define PACKET_TYPE_OKAY_YOU_CAN_PLAY       4
unsigned char packet_buffer[MAX_PACKET_SIZE];
unsigned short packet_length;
unsigned short packet_counter;
unsigned short packet_readpos;
unsigned char packet_seed;
unsigned char packet_checksum;


unsigned char global_version_error = FALSE;
unsigned short required_executable_version = 65535;
unsigned short required_data_version = 65535;


unsigned char  network_script_newly_spawned;
unsigned char  network_script_extra_data;
unsigned char  network_script_remote_index;
unsigned char  network_script_netlist_index;
unsigned short network_script_x;
unsigned short network_script_y;
unsigned char  network_script_z;
unsigned char  network_script_facing;
unsigned char  network_script_action;
unsigned char  network_script_team;
unsigned char  network_script_poison;
unsigned char  network_script_petrify;
unsigned char  network_script_alpha;
unsigned char  network_script_deflect;
unsigned char  network_script_haste;
unsigned char  network_script_other_enchant;
unsigned char  network_script_eqleft;
unsigned char  network_script_eqright;
unsigned char  network_script_eqcol01;
unsigned char  network_script_eqcol23;              // high-data only
unsigned char  network_script_eqspec1;              // high-data only
unsigned char  network_script_eqspec2;              // high-data only
unsigned char  network_script_eqhelm;               // high-data only
unsigned char  network_script_eqbody;               // high-data only
unsigned char  network_script_eqlegs;               // high-data only
unsigned char  network_script_class;                // high-data only
unsigned short network_script_mount_index;          // high-data only





//-----------------------------------------------------------------------------------------------
// Packet macros...
//-----------------------------------------------------------------------------------------------
#define packet_begin(type)                                                  \
{                                                                           \
    packet_length = PACKET_HEADER_SIZE;                                     \
    packet_buffer[0] = (unsigned char) type;                                \
    packet_buffer[1] = 0;                                                   \
    packet_buffer[2] = 0;                                                   \
}
// packet_buffer[0] is the packet type...
// packet_buffer[1] is the checksum
// packet_buffer[2] is the random seed


//-----------------------------------------------------------------------------------------------
#define packet_add_string(string)                                           \
{                                                                           \
    packet_counter = 0;                                                     \
    while(string[packet_counter] != 0)                                      \
    {                                                                       \
        packet_buffer[packet_length] = string[packet_counter];              \
        packet_length++;                                                    \
        packet_counter++;                                                   \
    }                                                                       \
    packet_buffer[packet_length] = 0;                                       \
    packet_length++;                                                        \
}

//-----------------------------------------------------------------------------------------------
#define packet_add_unsigned_int(number)                                     \
{                                                                           \
    packet_buffer[packet_length] = (unsigned char) (packet_counter>>24);    \
    packet_buffer[packet_length+1] = (unsigned char) (packet_counter>>16);  \
    packet_buffer[packet_length+2] = (unsigned char) (packet_counter>>8);   \
    packet_buffer[packet_length+3] = (unsigned char) packet_counter;        \
    packet_length+=4;                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_add_unsigned_short(number)                                   \
{                                                                           \
    packet_buffer[packet_length] = (unsigned char) (number>>8);             \
    packet_buffer[packet_length+1] = (unsigned char) number;                \
    packet_length+=2;                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_add_unsigned_char(number)                                    \
{                                                                           \
    packet_buffer[packet_length] = (unsigned char) number;                  \
    packet_length++;                                                        \
}

//-----------------------------------------------------------------------------------------------
#define packet_read_string(string)                                          \
{                                                                           \
    packet_counter = 0;                                                     \
    while(packet_buffer[packet_readpos] != 0 && packet_readpos < MAX_PACKET_SIZE && packet_counter < 250)    \
    {                                                                       \
        string[packet_counter] = packet_buffer[packet_readpos];             \
        packet_counter++;                                                   \
        packet_readpos++;                                                   \
    }                                                                       \
    string[packet_counter] = 0;                                             \
    packet_readpos++;                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_read_unsigned_int(to_set)                                    \
{                                                                           \
    to_set = packet_buffer[packet_readpos];                                 \
    to_set = to_set << 8;                                                   \
    to_set |= packet_buffer[packet_readpos+1];                              \
    to_set = to_set << 8;                                                   \
    to_set |= packet_buffer[packet_readpos+2];                              \
    to_set = to_set << 8;                                                   \
    to_set |= packet_buffer[packet_readpos+3];                              \
    packet_readpos+=4;                                                      \
}

//-----------------------------------------------------------------------------------------------
#define packet_read_unsigned_short(to_set)                                  \
{                                                                           \
    to_set = packet_buffer[packet_readpos];                                 \
    to_set = to_set << 8;                                                   \
    to_set |= packet_buffer[packet_readpos+1];                              \
    packet_readpos+=2;                                                      \
}

//-----------------------------------------------------------------------------------------------
#define packet_read_unsigned_char(to_set)                                   \
{                                                                           \
    to_set = packet_buffer[packet_readpos];                                 \
    packet_readpos++;                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_encrypt()                                                    \
{                                                                           \
    packet_seed = random_number;                                            \
    packet_buffer[1] = packet_seed;                                         \
    packet_counter = PACKET_HEADER_SIZE;                                    \
    while(packet_counter < packet_length)                                   \
    {                                                                       \
        packet_buffer[packet_counter] += random_table[(packet_seed+2173-packet_counter)&and_random];         \
        packet_counter++;                                                   \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_decrypt()                                                    \
{                                                                           \
    packet_counter = PACKET_HEADER_SIZE;                                    \
    packet_seed = packet_buffer[1];                                         \
    while(packet_counter < packet_length)                                   \
    {                                                                       \
        packet_buffer[packet_counter] -= random_table[(packet_seed+2173-packet_counter)&and_random];         \
        packet_counter++;                                                   \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
#define calculate_packet_checksum()                                         \
{                                                                           \
    packet_checksum = 0;                                                    \
    packet_counter = PACKET_HEADER_SIZE;                                    \
    while(packet_counter < packet_length)                                   \
    {                                                                       \
        packet_checksum += packet_buffer[packet_counter];                   \
        packet_counter++;                                                   \
    }                                                                       \
}

//-----------------------------------------------------------------------------------------------
#define packet_end()                                                        \
{                                                                           \
    calculate_packet_checksum();                                            \
    packet_buffer[2] = packet_checksum;                                     \
    packet_encrypt();                                                       \
}

//-----------------------------------------------------------------------------------------------
unsigned char packet_valid()
{
    calculate_packet_checksum();
    return (packet_buffer[2] == packet_checksum);
}

//-----------------------------------------------------------------------------------------------





//-----------------------------------------------------------------------------------------------
void network_clear_remote_list()
{
    // <ZZ> This function clears the list of other network players...
    unsigned short i;
    num_remote = 0;
    repeat(i, MAX_REMOTE)
    {
        remote_on[i] = FALSE;
    }
}

//-----------------------------------------------------------------------------------------------
unsigned char network_add_remote(unsigned char* remote_name)
{
    // <ZZ> This function adds a new network player by either address ("192.168.0.12") or
    //      name ("Frizzlesnitz")...  Returns TRUE if it worked (usually does)...
    unsigned short i;
    IPaddress temp_address;


    if(remote_name)
    {
        log_message("INFO:   Trying to add %s as a new remote...", remote_name);
        if(SDLNet_ResolveHost(&temp_address, remote_name, UDP_PORT) == 0)
        {
            log_message("INFO:   Found IP...  It's %d.%d.%d.%d", ((unsigned char*)&temp_address.host)[0], ((unsigned char*)&temp_address.host)[1], ((unsigned char*)&temp_address.host)[2], ((unsigned char*)&temp_address.host)[3]);
        }
        else
        {
            log_message("ERROR:  Couldn't find the IP address...  Oh, well...");
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }


    if(num_remote < MAX_REMOTE)
    {
        repeat(i, MAX_REMOTE)
        {
            if(remote_on[i])
            {
                if(remote_address[i].host == temp_address.host)
                {
                    log_message("ERROR:  That IP address is already used by remote %d...", i);
                    return FALSE;
                }
            }
        }


        repeat(i, MAX_REMOTE)
        {
            if(remote_on[i] == FALSE)
            {
                log_message("INFO:   Added new remote as remote number %d", i);
                remote_address[i].host = temp_address.host;
                remote_on[i] = TRUE;
                remote_room_number[i] = 65535;
                remote_is_neighbor[i] = FALSE;
                num_remote++;
                return TRUE;
            }
        }
    }
    log_message("ERROR:  Too many remotes all ready...  Oh, well...");
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
void network_delete_remote(unsigned short remote)
{
    if(remote < MAX_REMOTE)
    {
        if(remote_on[remote])
        {
            remote_on[remote] = FALSE;
            num_remote--;
        }
    }
}


//-----------------------------------------------------------------------------------------------
void network_close(void)
{
    // <ZZ> This function shuts down the network...
    log_message("INFO:   Shutting down the network");
	SDLNet_Quit();
}

//-----------------------------------------------------------------------------------------------
unsigned char network_setup(void)
{
    // <ZZ> This function initializes all the networking stuff.  Returns TRUE if networking is
    //      available, FALSE if not.



    // Turn all of our ports on and stuff
    network_on = FALSE;


    log_message("INFO:   ------------------------------------------");
    log_message("INFO:   Looking for NETLIST.DAT...");
    netlist = sdf_find_filetype("NETLIST", SDF_FILE_IS_DAT);
    if(netlist)
    {
        netlist = (unsigned char*) sdf_read_unsigned_int(netlist);
        log_message("INFO:   Found NETLIST.DAT...");
    }



    main_server_on = FALSE;
    network_clear_remote_list();
    log_message("INFO:   ------------------------------------------");
    log_message("INFO:   Trying to turn on networking...");
    if(SDLNet_Init()==0)
    {
        // Network started up okay...
        log_message("INFO:   Network started okay!");


        // Now try to open a UDP socket for talking to other remotes...
        log_message("INFO:   Trying to open up port %d for UDP networking...", UDP_PORT);
        remote_socket=SDLNet_UDP_Open(UDP_PORT);
        if(!remote_socket)
        {
            log_message("ERROR:  Uh, oh...  We couldn't open the port for some reason...  No networking for you...");
        }
        else
        {
            log_message("INFO:   Trying to find the IP address for %s...", MAIN_SERVER_NAME);
            if(SDLNet_ResolveHost(&main_server_address, MAIN_SERVER_NAME, TCPIP_PORT) == 0)
            {
                log_message("INFO:   Found IP...  It's %d.%d.%d.%d", ((unsigned char*)&main_server_address.host)[0], ((unsigned char*)&main_server_address.host)[1], ((unsigned char*)&main_server_address.host)[2], ((unsigned char*)&main_server_address.host)[3]);
                main_server_socket=SDLNet_TCP_Open(&main_server_address);
                if(!main_server_socket)
                {
                    log_message("ERROR:  Main server could not be contacted...  You can still use the IP Typer Inner though...");
                }
                else
                {
                    log_message("INFO:   Socket to main server open'd correctly...");
                    main_server_on = TRUE;
                }
            }
            else
            {
                log_message("ERROR   Main server was not found...  You can still use the IP Typer Inner though...");
            }



            // Round about way of finding our local address...
            log_message("INFO:   Looking for IP Address of local machine...");
            local_address.host = LOCALHOST;
            sprintf(run_string[0], "%s", SDLNet_ResolveIP(&local_address));
            log_message("INFO:   Said that LOCALHOST is %s", run_string[0]);
            if(SDLNet_ResolveHost(&local_address, run_string[0], UDP_PORT) == 0)
            {
                log_message("INFO:   Found IP...  It's %d.%d.%d.%d", ((unsigned char*)&local_address.host)[0], ((unsigned char*)&local_address.host)[1], ((unsigned char*)&local_address.host)[2], ((unsigned char*)&local_address.host)[3]);
            }


            // !!!BAD!!!
            // !!!BAD!!!
            network_add_remote("FooFoo");
            network_add_remote("Frizzlesnitz");
            // !!!BAD!!!
            // !!!BAD!!!

            // Remember to turn it off
            network_on = TRUE;
            atexit(network_close);
        }
    }
    else
    {
        // No network this time...
        log_message("ERROR:  Network failed!");
        log_message("ERROR:  SDLNet told us...  %s", SDLNet_GetError());
    }
    log_message("INFO:   ------------------------------------------");
    return network_on;
}

//-----------------------------------------------------------------------------------------------
#define NETWORK_ALL_REMOTES_IN_GAME             0
#define NETWORK_ALL_REMOTES_IN_ROOM             1
#define NETWORK_ALL_REMOTES_IN_NEARBY_ROOMS     2
#define NETWORK_SERVER_VIA_TCPIP                3
void network_send(unsigned char send_code)
{
    // <ZZ> This function sends a packet to the specified computer...
    unsigned short i;
    UDPpacket udp_packet;


// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
if(send_code == NETWORK_ALL_REMOTES_IN_ROOM)
{
    send_code = NETWORK_ALL_REMOTES_IN_GAME;
}
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!



    log_message("INFO:   network_send() called");
    if(network_on)
    {
        log_message("INFO:     network_on");

        // Let's figger out who we're sending it to...
        if(send_code == NETWORK_ALL_REMOTES_IN_GAME || send_code == NETWORK_ALL_REMOTES_IN_ROOM || send_code == NETWORK_ALL_REMOTES_IN_NEARBY_ROOMS)
        {
            // Now let's organize our packet into SDLNet's little format...
            log_message("INFO:     send via UDP");


            // Send the packet to all players who need to get it...
            repeat(i, MAX_REMOTE)
            {
                if(remote_on[i])
                {
                    if(send_code == NETWORK_ALL_REMOTES_IN_GAME || remote_room_number[i] == map_current_room || (remote_is_neighbor[i] && send_code == NETWORK_ALL_REMOTES_IN_NEARBY_ROOMS))
                    {
#ifdef ALLOW_LOCAL_PACKETS
                        if(TRUE)
#else
                        if(remote_address[i].host != LOCALHOST && remote_address[i].host != local_address.host)
#endif
                        {
                            log_message("INFO:     Sending to remote %d (%d.%d.%d.%d)", i, ((unsigned char*)&remote_address[i].host)[0], ((unsigned char*)&remote_address[i].host)[1], ((unsigned char*)&remote_address[i].host)[2], ((unsigned char*)&remote_address[i].host)[3]);
                            udp_packet.channel = -1;
                            udp_packet.data = packet_buffer;
                            udp_packet.len = packet_length;
                            udp_packet.maxlen = MAX_PACKET_SIZE;
                            udp_packet.address.host = remote_address[i].host;
                            #ifdef LIL_ENDIAN
                                udp_packet.address.port = (UDP_PORT>>8) | ((UDP_PORT&255)<<8);  // I want to cry...
                            #else
                                udp_packet.address.port = UDP_PORT;
                            #endif
                            if(!SDLNet_UDP_Send(remote_socket, -1, &udp_packet))
                            {
                                log_message("INFO:     Got error from SDLNet...  %s", SDLNet_GetError());
                            }
                        }
                        else
                        {
                            log_message("INFO:     Skipping remote %d because it's the local machine", i);
                        }
                    }
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
unsigned short network_find_remote_character(unsigned int ip_address_of_remote, unsigned char local_index_on_remote)
{
    // <ZZ> This function attempts to find the index of a character on the local computer, who is
    //      hosted on the given remote - with the given local index number on that remote...
    //      This lets me find something like character number 23 on Bob's computer, which is
    //      handled as character 42 on my computer...  It returns the index on my computer, or
    //      MAX_CHARACTER if a match can't be found...
    unsigned short i;
    unsigned char* character_data;

    repeat(i, MAX_CHARACTER)
    {
        if(main_character_on[i])
        {
            character_data = main_character_data[i];
            if(*((unsigned int*)(character_data+252)) == ip_address_of_remote)
            {
                if(character_data[250] == local_index_on_remote)
                {
                    return i;
                }
            }
        }
    }
    return MAX_CHARACTER;
}


//-----------------------------------------------------------------------------------------------
void network_listen(void)
{
    // <ZZ> This function checks for incoming packets, and handles 'em all
    UDPpacket udp_packet;
    unsigned char character_class;
    unsigned short room_number;
    unsigned short seed;
    unsigned char door_flags;
    unsigned short i, num_char;
    signed short length;
    unsigned char* script_file_start;
    float x, y, z;
    unsigned char found;
    unsigned char* character_data;
    unsigned char filename[9];



    if(network_on)
    {
        // Let's try to follow SDLNet's little format...
        udp_packet.channel = -1;
        udp_packet.data = packet_buffer;
        udp_packet.len = MAX_PACKET_SIZE;
        udp_packet.maxlen = MAX_PACKET_SIZE;


        if(SDLNet_UDP_Recv(remote_socket, &udp_packet))
        {
            // We've got a new packet...
            log_message("INFO:   Got a UDP packet from %d.%d.%d.%d:%d...  Length = %d, Type = %d", ((unsigned char*)&udp_packet.address.host)[0], ((unsigned char*)&udp_packet.address.host)[1], ((unsigned char*)&udp_packet.address.host)[2], ((unsigned char*)&udp_packet.address.host)[3], (((unsigned char*)&udp_packet.address.port)[0]<<8) | ((unsigned char*)&udp_packet.address.port)[1], udp_packet.len, packet_buffer[0]);
            packet_length = udp_packet.len;
            packet_decrypt();
            if(packet_valid())
            {
                // Screen out any packets we accidentally sent to ourself...
                log_message("INFO:     Checksum is okay...");
#ifdef ALLOW_LOCAL_PACKETS
                if(TRUE)
#else
                if(udp_packet.address.host != LOCALHOST && udp_packet.address.host != local_address.host)
#endif
                {
                    packet_readpos = PACKET_HEADER_SIZE;
                    log_message("INFO:     Packet isn't from local machine...");
                    if(packet_buffer[0] == PACKET_TYPE_CHAT)
                    {
                        packet_read_unsigned_char(character_class);     // Speaker class
                        packet_read_string(run_string[0]);              // Speaker name
                        packet_read_string(run_string[1]);              // Message
                        message_add(run_string[1], run_string[0], TRUE);
                    }
                    if(packet_buffer[0] == PACKET_TYPE_ROOM_UPDATE)
                    {
                        packet_read_unsigned_short(room_number);        // The room number this sender is in...
                        packet_read_unsigned_short(seed);               // The map seed this sender is using...
// !!!BAD!!!
// !!!BAD!!!  Seed should be checked properly...
// !!!BAD!!!
                        if(room_number == map_current_room && seed == 0 && netlist)
                        {
                            // Start to kill off any of this host's characters...
                            repeat(i, MAX_CHARACTER)
                            {
                                if(main_character_on[i])
                                {
                                    character_data = main_character_data[i];
                                    if(*((unsigned int*)(character_data+252)) == udp_packet.address.host)
                                    {
                                        character_data[82] = 0;  // Give 'em 0 hits...
                                    }
                                }
                            }



                            packet_read_unsigned_char(door_flags);      // The door flags for this room...
                            packet_read_unsigned_char(num_char);        // The number of characters in this packet...  (each character should have 11 or 19 bytes of data...)
                            length = packet_length - packet_readpos;    // The number of bytes remaining...
                            while(num_char > 0 && length >= 11)
                            {
                                network_script_newly_spawned = FALSE;
                                packet_read_unsigned_char(network_script_remote_index);
                                packet_read_unsigned_char(network_script_netlist_index);
                                packet_read_unsigned_char(network_script_z);
                                packet_read_unsigned_char(network_script_x);
                                packet_read_unsigned_char(network_script_y);
                                network_script_x = network_script_x | ((network_script_z&192)<<2);
                                network_script_y = network_script_y | ((network_script_z&48)<<4);
                                network_script_z = network_script_z & 15;
                                packet_read_unsigned_char(network_script_facing);
                                packet_read_unsigned_char(network_script_action);
                                network_script_extra_data = network_script_action>>7;
                                network_script_action = network_script_action&127;
                                packet_read_unsigned_char(network_script_team);
                                network_script_poison = (network_script_team >> 5) & 1;
                                network_script_petrify = (network_script_team >> 4) & 1;
                                network_script_alpha = (network_script_team & 8) ? (64) : (255);
                                network_script_deflect = (network_script_team >> 2) & 1;
                                network_script_haste = (network_script_team >> 1) & 1;
                                network_script_other_enchant = (network_script_team & 1);
                                network_script_team = network_script_team >> 6;
                                packet_read_unsigned_char(network_script_eqleft);
                                packet_read_unsigned_char(network_script_eqright);
                                packet_read_unsigned_char(network_script_eqcol01);
                                if(network_script_extra_data && length >= 19)
                                {
                                    // We've got more data coming...
                                    packet_read_unsigned_char(network_script_eqcol23);
                                    packet_read_unsigned_char(network_script_eqspec1);
                                    packet_read_unsigned_char(network_script_eqspec2);
                                    packet_read_unsigned_char(network_script_eqhelm);
                                    packet_read_unsigned_char(network_script_eqbody);
                                    packet_read_unsigned_char(network_script_eqlegs);
                                    packet_read_unsigned_char(network_script_class);
                                    packet_read_unsigned_char(network_script_mount_index);
                                    if(network_script_mount_index != network_script_remote_index)
                                    {
                                        // Character is riding a mount...
                                        network_script_mount_index = network_find_remote_character(udp_packet.address.host, (unsigned char) network_script_mount_index);
                                    }
                                    else
                                    {
                                        network_script_mount_index = MAX_CHARACTER;
                                    }
                                }
                                else
                                {
                                    // Script shouldn't ask for these, but just in case...
                                    network_script_eqcol23 = 0;
                                    network_script_eqspec1 = 0;
                                    network_script_eqspec2 = 0;
                                    network_script_eqhelm = 0;
                                    network_script_eqbody = 0;
                                    network_script_eqlegs = 0;
                                    network_script_class = 0;
                                    network_script_mount_index = MAX_CHARACTER;
                                }


                                // Okay, we've read all of the data for this character, now let's see if we need to spawn it...
                                i = network_find_remote_character(udp_packet.address.host, network_script_remote_index);
                                found = FALSE;
                                if(i < MAX_CHARACTER)
                                {
                                    if(main_character_on[i])
                                    {
                                        // Looks like we've found the character...
                                        found = TRUE;
                                    }
                                }
                                if(!found)
                                {
                                    // We didn't find this character - that means we'll have to try to spawn a new one of the appropriate type...
                                    x = (network_script_x - 512.0f) * 0.25f;
                                    y = (network_script_y - 512.0f) * 0.25f;
                                    z = room_heightmap_height(roombuffer, x, y);
                                    z = z + (network_script_z*2.0f);


                                    script_file_start = netlist + (network_script_netlist_index<<3);
                                    repeat(i, 8)
                                    {
                                        filename[i] = script_file_start[i];
                                    }
                                    filename[8] = 0;
                                    i = MAX_CHARACTER;
                                    script_file_start = sdf_find_filetype(filename, SDF_FILE_IS_RUN);
                                    if(script_file_start)
                                    {
                                        script_file_start = (unsigned char*) sdf_read_unsigned_int(script_file_start);
                                        character_data = obj_spawn(CHARACTER, x, y, z, script_file_start, 65535);
                                        if(character_data)
                                        {
                                            if(character_data >= main_character_data[0] && character_data <= main_character_data[MAX_CHARACTER-1])
                                            {
                                                network_script_newly_spawned = TRUE;
                                                i = (character_data-main_character_data[0])/CHARACTER_SIZE;
                                                i = i & (MAX_CHARACTER-1);
                                                *((unsigned int*)(character_data+252)) = udp_packet.address.host;
                                                character_data[250] = network_script_remote_index;
                                            }
                                        }
                                    }
                                }
                                // Now let's do this again and give the character a script function call, so we can handle the network data more precisely...
                                if(i < MAX_CHARACTER)
                                {
                                    if(main_character_on[i])
                                    {
                                        character_data = main_character_data[i];
                                        character_data[67] = EVENT_NETWORK_UPDATE;
                                        fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
                                    }
                                }
                                





                                num_char--;
                                length = packet_length - packet_readpos;
                            }




                            // Finish killing off any character whose hits haven't been reset...
                            repeat(i, MAX_CHARACTER)
                            {
                                if(main_character_on[i])
                                {
                                    character_data = main_character_data[i];
                                    if(*((unsigned int*)(character_data+252)) == udp_packet.address.host)
                                    {
                                        if(character_data[82] == 0)
                                        {
                                            character_data[67] = EVENT_DAMAGED;
                                            global_attacker = i;
                                            global_attack_spin = (*((unsigned short*) (character_data + 56))) + 32768;
                                            fast_run_script(main_character_script_start[i], FAST_FUNCTION_EVENT, character_data);
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

//-----------------------------------------------------------------------------------------------
unsigned char network_find_script_index(unsigned char* filename)
{
    // <ZZ> This function finds a character script filename in the NETLIST.DAT file...  So
    //      we don't have to send the whole thing over the network...  Returns 0 if it didn't
    //      find a match...
    unsigned short i, j;
    unsigned char found;
    unsigned char* checkname;
    if(netlist)
    {
        checkname = netlist+8;
        i = 1;
        while(i < 256)
        {
            found = TRUE;
            repeat(j, 8)
            {
                if(checkname[j] == filename[j])
                {
                    if(checkname[j] == 0)
                    {
                        return ((unsigned char) i);
                    }
                }
                else
                {
                    found = FALSE;
                    j = 8;
                }
            }
            if(found)
            {
                return ((unsigned char) i);
            }
            checkname+=8;
            i++;
        }
    }
    return 0;
}

//-----------------------------------------------------------------------------------------------












//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
// Special functions to send certain types of packets...
//-----------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------
void network_send_chat(unsigned char speaker_class, unsigned char* speaker_name, unsigned char* message)
{
    // <ZZ> This function sends a chat message to all the players in the room (or if message starts with
    //      <ALL> it goes to all in the game)...
    unsigned char send_to_all;

    send_to_all = FALSE;
    if(message)
    {
        if(message[0] == '<')
        {
            if(message[1] == 'A')
            {
                if(message[2] == 'L')
                {
                    if(message[3] == 'L')
                    {
                        if(message[4] == '>')
                        {
                            send_to_all = TRUE;
                            message+=5;
                            if(message[0] == ' ')
                            {
                                message++;
                            }
                        }
                    }
                }
            }
        }
    }
    packet_begin(PACKET_TYPE_CHAT);
        packet_add_unsigned_char(speaker_class);
        packet_add_string(speaker_name);
        packet_add_string(message);
    packet_end();


    // Spit out the message on the local computer...
    message_add(message, speaker_name, TRUE);


    if(send_to_all)
    {
        log_message("INFO:   Sending chat message %s to all in game from speaker %s", message, speaker_name);
        network_send(NETWORK_ALL_REMOTES_IN_GAME);
    }
    else
    {
        log_message("INFO:   Sending chat message %s to all in room from speaker %s", message, speaker_name);
        network_send(NETWORK_ALL_REMOTES_IN_ROOM);
    }
}

//-----------------------------------------------------------------------------------------------
void network_send_room_update()
{
    // <ZZ> This function sends a room update packet to all the players in the room...
    //      The packet should include all characters that are hosted on the local machine...
    unsigned short local_character_count, i, facing;
    unsigned char* character_data;
    unsigned short mount;
    float* character_xyz;
    unsigned short x, y;
    float fz;
    unsigned char z;
    unsigned char pmod;
    unsigned char action;
    unsigned char misc;

    // Make sure we're in a valid room...
    if(map_current_room < MAX_MAP_ROOM)
    {
        // Count how many characters we need to send over network...
        local_character_count = 0;
        repeat(i, MAX_CHARACTER)
        {
            // Only need to send characters that are used...
            if(main_character_on[i])
            {
                // Only need to send characters that are hosted locally...
                if(main_character_data[i][252] == 0 && main_character_data[i][253] == 0 && main_character_data[i][254] == 0 && main_character_data[i][255] == 0) 
                {
                    // Is this character's script in NETLIST.DAT?
                    if(main_character_data[i][251])
                    {
                        // Looks like we've got one to send...
                        local_character_count++;
                    }
                }
            }
        }


        if(local_character_count > 255)
        {
            local_character_count = 255;
        }
        log_message("INFO:   Sending room update packet to all in room (%d characters)", local_character_count);
        message_add("Sending room update packet to remotes...", "NETWORK", FALSE);


        packet_begin(PACKET_TYPE_ROOM_UPDATE);
            packet_add_unsigned_short(map_current_room);
            packet_add_unsigned_short(0);       // !!!BAD!!!  map_seed
            packet_add_unsigned_char(map_room_data[map_current_room][29]);  // Door flags
            packet_add_unsigned_char(local_character_count);
            i = 0;
            while(i < MAX_CHARACTER && local_character_count > 0)
            {
                if(main_character_on[i])
                {
                    if(main_character_data[i][252] == 0 && main_character_data[i][253] == 0 && main_character_data[i][254] == 0 && main_character_data[i][255] == 0) 
                    {
                        if(main_character_data[i][251])
                        {
                            // This character is hosted on the local machine, so let's send it on over...
                            character_data = main_character_data[i];
                            character_xyz = (float*) character_data;
                            packet_add_unsigned_char(i);                        // Local index number
                            packet_add_unsigned_char(character_data[251]);      // Script index (in NETLIST.DAT)
                            x = ((unsigned short) ((character_xyz[X]*ROOM_HEIGHTMAP_PRECISION) + 512.0f))&1023;
                            y = ((unsigned short) ((character_xyz[Y]*ROOM_HEIGHTMAP_PRECISION) + 512.0f))&1023;
                            fz = (character_xyz[Z] - character_xyz[11])*0.5f;  clip(0.0f, fz, 15.0f);  z = (unsigned char) fz;
                            pmod = ((x>>8)<<6) | ((y>>8)<<4) | z;
                            packet_add_unsigned_char(pmod);                     // Position modifiers (top 2 bits for x) (mid 2 bits for y) (low 4 bits are z above floor)
                            packet_add_unsigned_char(x);                        // X position (with modifier should range from 0-1023)
                            packet_add_unsigned_char(y);                        // Y position (with modifier should range from 0-1023)
                            facing = *((unsigned short*) (character_data+56));
                            facing = facing>>8;
                            packet_add_unsigned_char(facing);                   // Facing (should range from 0-255)
                            action = character_data[65];
                            if(CHAR_FLAGS & CHAR_FULL_NETWORK)
                            {
                                action = action | 128;
                            }
                            mount = *((unsigned short*) (character_data+164));
                            if(mount < MAX_CHARACTER)
                            {
                                action = action | 128;
                            }
                            packet_add_unsigned_char(action);                   // Action (high bit used if extra character data is to be sent...)
                            misc = character_data[78]<<6;
                            if((*((unsigned short*) (character_data+40))) > 0)
                            {
                                misc = misc | 32;
                            }
                            if((*((unsigned short*) (character_data+42))) > 0)
                            {
                                misc = misc | 16;
                            }
                            if(character_data[79] < 128)
                            {
                                misc = misc | 8;
                            }
                            if(character_data[216] & ENCHANT_FLAG_DEFLECT)
                            {
                                misc = misc | 4;
                            }
                            if(character_data[216] & ENCHANT_FLAG_HASTE)
                            {
                                misc = misc | 2;
                            }
                            if(character_data[216] & (ENCHANT_FLAG_SUMMON_3 | ENCHANT_FLAG_LEVITATE | ENCHANT_FLAG_INVISIBLE | ENCHANT_FLAG_MORPH))
                            {
                                misc = misc | 1;
                            }
                            packet_add_unsigned_char(misc);                     // Miscellaneous (top 2 bits are team) (then 1 bit for poison) (then 1 bit for petrify) (then 1 bit for low alpha) (then 1 bit for enchant_deflect) (then 1 bit for enchant_haste) (then 1 bit if enchanted in any way other than deflect & haste)
                            packet_add_unsigned_char(character_data[242]);      // EqLeft
                            packet_add_unsigned_char(character_data[243]);      // EqRight
                            packet_add_unsigned_char(character_data[240]);      // EqCol01
                            if(action & 128)
                            {
                                // Character is a high-data character...  Extra character data is to be sent...
                                packet_add_unsigned_char(character_data[241]);      // EqCol23
                                packet_add_unsigned_char(character_data[244]);      // EqSpec1
                                packet_add_unsigned_char(character_data[245]);      // EqSpec2
                                packet_add_unsigned_char(character_data[246]);      // EqHelm
                                packet_add_unsigned_char(character_data[247]);      // EqBody
                                packet_add_unsigned_char(character_data[248]);      // EqLegs
                                packet_add_unsigned_char(character_data[204]);      // Character class
                                if(mount < MAX_CHARACTER)
                                {
                                    packet_add_unsigned_char(mount);                    // Local index number of mount
                                }
                                else
                                {
                                    packet_add_unsigned_char(i);                        // Mount is not valid, so send our own local index number again (since we're obviously not riding ourself)
                                }
                            }
                            local_character_count--;
                        }
                    }
                }
                i++;
            }
        packet_end();



        network_send(NETWORK_ALL_REMOTES_IN_ROOM);
    }
}

//-----------------------------------------------------------------------------------------------
