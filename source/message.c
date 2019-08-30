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

// <ZZ> This file contains functions to handle the message buffer
//      message_get         - Macro that returns a pointer to one of the last 16 messages
//      message_reset       - Clears out the message buffer
//      message_sanitize    - Replaces "bad" words with #@$% style text
//      message_add         - Adds a message to the message buffer

#define MESSAGE_MAX 16                  // Maximum number of messages
#define MAX_MESSAGE_SIZE 128            // Maximum length of the messages
#define MAX_SANITIZE 256                // Maximum number of bad words

unsigned char message_read = 0;         // The most recent message
unsigned char message_write = 0;        // The next message to fill in
char message_buffer[MESSAGE_MAX][MAX_MESSAGE_SIZE];
unsigned char* sanitize_file = NULL;
unsigned char* sanitize_token[MAX_SANITIZE];
int sanitize_count = 0;
unsigned short message_size = 42;

//-----------------------------------------------------------------------------------------------
// <ZZ> Macro for getting a message, 0 is most recent...
#define message_get(VAL) message_buffer[(message_read-VAL) & (MESSAGE_MAX-1)]


//-----------------------------------------------------------------------------------------------
void message_reset(void)
{
    // <ZZ> This function clears out the message buffer
    repeat(message_read, MESSAGE_MAX)
    {
        message_buffer[message_read][0] = 0;
    }
    message_read = 0;
    message_write = 0;
}

//-----------------------------------------------------------------------------------------------
void message_setup(void)
{
    // <ZZ> This function reads the badwords file...
    int i;
    unsigned char* read;
    if(sanitize_file == NULL)
    {
        sanitize_file = sdf_find_filetype("BADWORDS", SDF_FILE_IS_TXT);
        if(sanitize_file)
        {
            sanitize_file = (unsigned char*) sdf_read_unsigned_int(sanitize_file);
            sscanf(sanitize_file, "%d", &sanitize_count);
            while(*sanitize_file != 0)
            {
                sanitize_file++;
            }
            sanitize_file++;

            read = sanitize_file;
            repeat(i, sanitize_count)
            {
                sanitize_token[i] = read;
//log_message("Sanitize Token %03d == %s", i, read);
                read+=strlen(read);
                read++;
            }
        }
    }
}

//-----------------------------------------------------------------------------------------------
char qbert[] = "@#$&";
void message_sanitize(char* message_text)
{
    // <ZZ> This function takes a string and replaces any bad words with qbert style text...
    int read, token, read_ahead, token_read;
    unsigned char letter, token_letter, last_token_letter, match, keep_going;


    // Go through each letter of the message string...
    read = 0;
    letter = message_text[read];
    while(letter!=0)
    {
        // Go through all tokens in the sanitize file looking for matches...
//log_message("INFO:   SANITIZE: Working on letter %d of message (%c)", read, letter);
        token = 0;
        while(token < sanitize_count)
        {
            // Go through this particular token, while at the same time scanning ahead in the message
            read_ahead = read;
            letter = message_text[read_ahead];
            token_read = 0;
            token_letter = sanitize_token[token][token_read];
            last_token_letter = 0;
            match = FALSE;
            keep_going = TRUE;
            while(keep_going)
            {
                if(token_letter == 0 || letter == 0)
                {
                    // We're at the end of the token or the message...
                    if(token_letter == 0)
                    {
                        // Looks like we matched a token...
                        match = TRUE;
                    }
                    keep_going = FALSE;
                }
                else
                {
                    // Is this letter of the message actually a letter, or is it a symbolic character?
                    if(letter >= 'A' && letter <= 'Z') { letter = letter - 'A' + 'a'; }  // Make lowercase
                    if(letter >= 'a' && letter <= 'z')
                    {
                        // The message letter is actually a letter...  Does it match the current letter of the token?
                        if(letter == token_letter)
                        {
                            // Yup...  That means we can proceed to the next letter of the token...
//log_message("INFO:   SANITIZE:   Matched letter %d (%c) with token %d (%s) letter %d (%c)", read_ahead, letter, token, sanitize_token[token], token_read, token_letter);
                            token_read++;
                            last_token_letter = token_letter;
                            token_letter = sanitize_token[token][token_read];
                        }
                        else
                        {
                            // Nope...  Well does it match the last token letter?
                            if(letter == last_token_letter)
                            {
                                // Yes it does...  That means we might have something like DDDAAARRRNNN...
//log_message("INFO:   SANITIZE:   Matched letter %d (%c) with token %d (%s) letter %d (%c)", read_ahead, letter, token, sanitize_token[token], token_read-1, last_token_letter);
                            }
                            else
                            {
                                // Doesn't match the last one either...  That means it's not a match...
//log_message("INFO:   SANITIZE:   Match failed with token %d (%s) (%c or %c)", token, sanitize_token[token], token_letter, last_token_letter);
                                keep_going = FALSE;
                            }
                        }

                    }
                    read_ahead++;
                    letter = message_text[read_ahead];
                }
            }
            token++;
            if(match)
            {
                letter = random_number;
                while(read_ahead > read)
                {
                    read_ahead--;
                    if((message_text[read_ahead] >= 'A' && message_text[read_ahead] <= 'Z') || (message_text[read_ahead] >= 'a' && message_text[read_ahead] <= 'z'))
                    {
                        message_text[read_ahead] = qbert[(letter&3)];
                        letter++;
                        match = random_number;
                        if(match > 192)
                        {
                            letter++;
                        }
                    }
                }
                token = sanitize_count;
            }
        }


        read++;
        letter = message_text[read];
    }

}


//-----------------------------------------------------------------------------------------------
/*
char qbert[] = "@#$&";
char message_temp[256];
char token_temp[256];
void message_sanitize(char* message_text)
{
    // <ZZ> This function fills in message_temp with a sanitized copy of message_text
    int read;
    int master_read;
    int write;
    int bad_word;
    int check_pos;
    int length;
    int place;
    char capital_token;
    unsigned char keep_going;
    unsigned char keep_searching;



    write = 0;
    keep_going = TRUE;
    while(keep_going)
    {
        // Non-token characters...
        while(*message_text != 0 && !((*message_text >= 'A' && *message_text <= 'Z') || (*message_text >= 'a' && *message_text <= 'z')) && write < 256)
        {
            message_temp[write] = *message_text;
            message_text++;
            write++;
        }



        // Token characters
        if(*message_text == 0)
        {
            keep_going = FALSE;
        }
        else
        {
            // Read in the token...
            place = 0;
            while(*message_text != 0 && ((*message_text >= 'A' && *message_text <= 'Z') || (*message_text >= 'a' && *message_text <= 'z')) && place < SRC_MAX_TOKEN_SIZE-1)
            {
                token_temp[place] = *message_text;
                message_text++;
                place++;
            }
            token_temp[place] = 0;
            if(*message_text == 0)  keep_going = FALSE;


            // Make it lowercase
            place = 0;
            capital_token = (token_temp[0] >= 'A' && token_temp[0] <= 'Z');
            while(token_temp[place] != 0)
            {
                if(token_temp[place] <= 'Z')  token_temp[place] += ('a'-'A');
                place++;
            }


            // Sanitize the token, if need be...
            length = strlen(token_temp) - 2;
            master_read = 0;
            repeat(bad_word, sanitize_count)
            {
                // Check against every word in "BADWORDS.TXT"
                check_pos = 0;
                repeat(check_pos, length)
                {
                    // Check every position in the token
                    place = check_pos;
                    read = master_read;
                    keep_searching = TRUE;
                    while(sanitize_file[read] != 0 && token_temp[place] != 0 && keep_searching)
                    {
                        if(token_temp[place] == sanitize_file[read])
                        {
                            place++;
                        }
                        else if(token_temp[place] == sanitize_file[read+1])
                        {
                            read++;
                            place++;
                        }
                        else
                        {
                            read++;
                            if(sanitize_file[read] != 0)
                            {
                                keep_searching = FALSE;
                            }
                        }
                    }
                    if(keep_searching)
                    {
                        if(sanitize_file[read] == 0) keep_searching = FALSE;
                        else
                        {
                            if(sanitize_file[read+1] == 0)  keep_searching = FALSE;
                        }
                        if(keep_searching == FALSE)
                        {
                            // Found one...  Replace with @#$%, skip rest...
                            while(place > check_pos)
                            {
                                place--;
                                token_temp[place] = qbert[(message_write + place)&3];
                            }
                            bad_word = sanitize_count;
                        }
                    }
                }

                // Finished checking this word, skip ahead to next
                while(sanitize_file[master_read] != 0)
                {
                    master_read++;
                }
                master_read++;
            }




            // Keep the first letter capitalized...
            if(capital_token && token_temp[0] >= 'a' && token_temp[0] <= 'z') token_temp[0] -= ('a'-'A');



            // Write the token into the new string...
            read = 0;
            while(write < 256 && token_temp[read] != 0)
            {
                message_temp[write] = token_temp[read];
                read++;
                write++;
            }
        }
    }
    if(write > 255) write = 255;
    message_temp[write] = 0;
}
*/

//-----------------------------------------------------------------------------------------------
void message_add(char* message_text, char* speaker_name, unsigned char sanitize)
{
    // <ZZ> This function adds a message to the message buffer
    char c;
    int read;
    int write;


    // Sanitize the message, if it's flagged as needing it...
    if(sanitize)
    {
        message_sanitize(message_text);
    }


    // Copy the message into the message_buffer
    write = 0;
    if(speaker_name)
    {
        c = speaker_name[0];
        while(write < 16 && c != 0)
        {
            message_buffer[message_write][write] = c;
            write++;
            c = speaker_name[write];
        }
        message_buffer[message_write][write] = ':';
        write++;
        message_buffer[message_write][write] = ' ';
        write++;
    }


    // Now copy in the message...
    read = 0;
    c = message_text[0];
    while(c != 0)
    {
        while(write < (message_size-1) && c != 0)
        {
            message_buffer[message_write][write] = c;
            write++;
            read++;
            c = message_text[read];
        }
        message_buffer[message_write][write] = 0;
        message_read = message_write;
        message_write = (message_write + 1) & (MESSAGE_MAX-1);

        if(c != 0)
        {
            // Need to use multiple lines...  Back track up to 10 letters...
            c = 0;
            while(message_text[read] != ' ' && c < 10)
            {
                read--;
                c++;
            }
            if(message_text[read] == ' ')
            {
                message_buffer[message_read][write-c] = 0;
                read++;                
            }
            else
            {
                read+=c;
                while(message_text[read] == ' ')
                {
                    read++;
                }
            }
            c = message_text[read];


            // Then indent the new line by 5 letters...
            repeat(write, 5)
            {
                message_buffer[message_write][write] = ' ';
            }
        }
    }
    message_buffer[message_write][write] = 0;
}

//-----------------------------------------------------------------------------------------------
void language_message_add(unsigned short message_index)
{
    // <ZZ> This function is a toned down version of message_add for doing language file
    //      strings...
    unsigned char* message_string;
    unsigned int offset;

    if(message_index <= user_language_phrases && message_index > 0)
    {
        message_string = language_file[user_language] + (message_index<<2) + 1;
        offset = (*message_string);  offset = offset<<8;  message_string++;
        offset+= (*message_string);  offset = offset<<8;  message_string++;
        offset+= (*message_string);  
        message_string = language_file[user_language] + offset;
        message_add(message_string, NULL, FALSE);
    }
}

//-----------------------------------------------------------------------------------------------
