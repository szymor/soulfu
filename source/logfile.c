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

// <ZZ> This file contains functions for handling the logfile.txt file
//      log_message             - Writes a message to the log file using printf style formatting
//      close_logfile           - Closes the logfile, automatically called via atexit()
//      open_logfile            - Opens the logfile

FILE* logfile = NULL;
int log_error_count;

//-----------------------------------------------------------------------------------------------
void log_message(char *format, ...)
{
    // <ZZ> This function spits out a message to the logfile.txt file, following standard printf
    //      style formatting.  It also logs any errors to the message buffer (for ingame tools).
    va_list ap;
    char log_buffer[256];

    if(logfile)
    {
        va_start(ap, format);
        vsprintf(log_buffer, format, ap);
        va_end(ap);
        fprintf(logfile, "%s\n", log_buffer);
        fflush(logfile);


        #ifdef DEVTOOL
            if(log_buffer[0] == 'E' && log_error_count < 1)
            {
                log_buffer[64] = 0;
                message_add(log_buffer, NULL, FALSE);
                log_error_count++;
            }
            if(log_buffer[0] == 'F' && log_error_count == 1)
            {
                log_buffer[64] = 0;
                message_add(log_buffer, NULL, FALSE);
                log_error_count++;
            }
        #endif
    }
}

//-----------------------------------------------------------------------------------------------
void close_logfile(void)
{
    // <ZZ> This function closes the logfile, and should be run automatically on program
    //      termination...
    if(logfile)
    {
        log_message("INFO:   Log file closed");
        fclose(logfile);
    }
}

//-----------------------------------------------------------------------------------------------
signed char open_logfile(void)
{
    // <ZZ> This function opens up the LOGFILE.TXT file and registers close_logfile() to run
    //      on program termination.  It returns TRUE if it worked okay, FALSE if there was a
    //      problem.
    logfile = fopen("LOGFILE.TXT", "w");
    log_error_count = 0;
    if(logfile)
    {
        log_message("INFO:   Log file started");
        atexit(close_logfile);
        return TRUE;
    }
    return FALSE;
}

//-----------------------------------------------------------------------------------------------
