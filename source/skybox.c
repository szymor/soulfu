//============================================================================
// Skybox drawing code for OpenGL
// (C) 1999,2004   Bill Baxter
//============================================================================
// HINT:  Get your sky textures from http://www.planethalflife.com/crinity
//============================================================================
// This code relies on some simple functionality in GLVU
//   http://www.cs.unc.edu/~walk/software/glvu,
// but you could easily replace the GLVU stuff with straight OpenGL and 
// other utility libraries.
// GLVU is only used for setting up the view matrix and for some image
// loading.
//-----------------------------------------------------------------------------
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
//-----------------------------------------------------------------------------

#include "skybox.h"
/* 
void UploadSkyTextures(unsigned int texIDs[6], SimpImg images[6])
{
  glGenTextures(6, texIDs);
  glEnable(GL_TEXTURE_2D);
  for (int i=0; i<6; i++)
  {
    glBindTexture(GL_TEXTURE_2D, texIDs[i]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
                 images[i].W, images[i].H, 
                 0, GL_RGB, GL_UNSIGNED_BYTE, images[i].data);
  }
  GLerror("Error after binding textures");
} */

void LoadSkyBoxTextures(unsigned int texIds[6],unsigned int texSrc[6]) {
    //texture_skybox_0
    texIds[0]=texSrc[4];//ok top
    texIds[1]=texSrc[3];
    texIds[2]=texSrc[5];//ok bottom
    texIds[3]=texSrc[2];// ok
    texIds[4]=texSrc[0];
    texIds[5]=texSrc[1];
}

void DrawSkyBox(unsigned int  texIds[6])
{
  // RIPPED FROM GLUT 3.7 glut_shapes.c AND CHANGED SLIGHTLY
  static const GLint faces[6][4] =
  {
    /*
    {0, 1, 2, 3}, // left
    {3, 2, 6, 7}, // top
    {7, 6, 5, 4}, // right
    {4, 5, 1, 0}, // bottom
    {5, 6, 2, 1}, // back
    {7, 4, 0, 3}  // front
    */
    {1, 2, 6, 5}, // back
    {5, 4, 0, 1}, // bottom
    {3, 0, 4, 7}, // front
    {6, 7, 4, 5}, // right ( 'left' in crinity's labeling )
    {1, 0, 3, 2}, // left  ( 'right' in crinity's labeling )
    {2, 3, 7, 6}  // top
    
/*    {5, 1, 2, 6}, // back
    {5, 4, 0, 1}, // bottom
    {0, 4, 7, 3}, // front
    {4, 5, 6, 7}, // right ( 'left' in crinity's labeling )
    {1, 0, 3, 2}, // left  ( 'right' in crinity's labeling )
    {2, 3, 7, 6}  // top*/
    
  };
  GLfloat v[8][3];
  GLint i;

  v[0][0] = v[1][0] = v[2][0] = v[3][0] =  SKYBOX_X;  // min x
  v[4][0] = v[5][0] = v[6][0] = v[7][0] =  SKYBOX_X+SKYBOX_DX;  // max x
  v[0][1] = v[1][1] = v[4][1] = v[5][1] =  SKYBOX_Y;  // min y
  v[2][1] = v[3][1] = v[6][1] = v[7][1] =  SKYBOX_Y+SKYBOX_DY;  // max y
  v[0][2] = v[3][2] = v[4][2] = v[7][2] =  SKYBOX_Z;  // min z
  v[1][2] = v[2][2] = v[5][2] = v[6][2] =  SKYBOX_Z+SKYBOX_DZ;  // max z

  for (i = 5; i >= 0; i--) 
  {
    glBindTexture(GL_TEXTURE_2D, texIds[i]);
    glBegin(GL_QUADS);
    {
      /*glTexCoord2f(0,1);  glVertex3fv(&v[faces[i][0]][0]);
      glTexCoord2f(1,1);  glVertex3fv(&v[faces[i][1]][0]);
      glTexCoord2f(1,0);  glVertex3fv(&v[faces[i][2]][0]);
      glTexCoord2f(0,0);  glVertex3fv(&v[faces[i][3]][0]);*/

      glTexCoord2f(0,0);  glVertex3fv(&v[faces[i][0]][0]);
      glTexCoord2f(0,1);  glVertex3fv(&v[faces[i][1]][0]);
      glTexCoord2f(1,1);  glVertex3fv(&v[faces[i][2]][0]);
      glTexCoord2f(1,0);  glVertex3fv(&v[faces[i][3]][0]);
      
    }
    glEnd();
  }
}

