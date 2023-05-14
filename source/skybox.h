//============================================================================
// SKYBOX.H -- CODE FOR DRAWING SKYBOX BACKGROUND TEXTURES IN OpenGL
// (C) 1999,2004   Bill Baxter
//============================================================================
// HINT:  Get your sky textures from http://www.planethalflife.com/crinity
//============================================================================
// There's a much easier way to do this if you have cube map support
// in your hardware.  If not then this will just have to do.
//============================================================================
// Permission to use, copy, modify, distribute and sell this software
// and its documentation for any purpose is hereby granted without
// fee, provided that the above copyright notice appear in all copies
// and that both that copyright notice and this permission notice
// appear in supporting documentation.  Binaries may be compiled with
// this software without any royalties or restrictions.
//
// The University of North Carolina at Chapel Hill makes no representations 
// about the suitability of this software for any purpose. It is provided 
// "as is" without express or implied warranty.
//============================================================================
enum { SKY_BACK, SKY_DOWN, SKY_FRONT, SKY_LEFT, SKY_RIGHT, SKY_UP };
long SKYBOX_ON=0;
long SKYBOX_X=-200;
long SKYBOX_DX= 400;
long SKYBOX_Y=-200;
long SKYBOX_DY= 400;
long SKYBOX_Z=-100;
long SKYBOX_DZ= 200;

typedef struct  {
  unsigned char *data;
  int W, H;
} SimpImg;

//void UploadSkyTextures(unsigned int texIDs[6], SimpImg images[6]);

unsigned int g_skyTexID[] = {0,0,0,0,0,0};
SimpImg g_skyImageData[] = {{0},{0},{0},{0},{0},{0}};

unsigned int g_skyTexIDs[10][6]; 
 
unsigned int texture_skybox_0  = 0;
unsigned int texture_skybox_1  = 0;
unsigned int texture_skybox_2  = 0;
unsigned int texture_skybox_3  = 0;
unsigned int texture_skybox_4  = 0;
unsigned int texture_skybox_5  = 0;

//void LoadSkyBoxTextures(unsigned int texIds[6]);
//void DrawSkyBox(unsigned int texIds[6]);

