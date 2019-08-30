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

unsigned int game_seed;

//-----------------------------------------------------------------------------------------------
void generate_game_seed(void)
{
    // <ZZ> This function generates a new game seed...
    srand(time(0));
    game_seed = (rand()&255);  game_seed<<=8;
    game_seed |= (rand()&255);  game_seed<<=8;
    game_seed |= (rand()&255);  game_seed<<=8;
    game_seed |= (rand()&255);
    log_message("INFO:   Set game seed to %d", game_seed);
}

//-----------------------------------------------------------------------------------------------
