
Skyboxingsystem for Soulfu (2008 MiR)

The skyboxing system is very basic.
The files need are in this zip, consisting of for Sourcefiles (Skybox.c, Skybox.h, modified: display.c and soulfu.c) and 6 cubemap pictures

The Skybox is actually not a quadric box, but is somewhat flat. (400x400x200)
To change this just update the Values for 
long SKYBOX_X=-200;
long SKYBOX_DX= 400;
long SKYBOX_Y=-200;
long SKYBOX_DY= 400;
long SKYBOX_Z=-100;
long SKYBOX_DZ= 200;

in the file skybox.h

The necessary cubemap pictures must be imported into your Soulfu datafile.sdf.
The pictures are enumerated by their name. SKYBOX11.PCX etc. In general, the system is expandable to use different skyboxes in different rooms, but actually that is not supported. It now takes the MAP_ROOM_FLAG_OUTSIDE, wich gets set in the MAPGEN.SRF file for TOWN000.SRF level only and replaces the blue sky by the skybox. Thats all.

For future levels you will have to set the Outside-flag manually in the same manner.


The basic skyboxcode is taken from University of North Carolina at Chapel Hill, see the copyrightnotice in skybox.h

