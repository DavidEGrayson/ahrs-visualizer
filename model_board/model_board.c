#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "cube_texture_and_coords.h"
#include "../png_texture.h"

static GLuint texture_top;

void model_board_init(void)
{
   int width, height;
   texture_top = png_texture_load(PATH "top.png", &width, &height);

   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(3, GL_BYTE, 0, quadx);

   glEnableClientState( GL_COLOR_ARRAY );
   glColorPointer(4, GL_FLOAT, 0, colorsf);

   glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void model_board_redraw(void)
{
   glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Top
   glBindTexture(GL_TEXTURE_2D, texture_top);
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

   // same pattern for other 5 faces - rotation chosen to make image orientation 'nice'
   //glBindTexture(GL_TEXTURE_2D, 0);
   glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
   glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
   glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
   glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
   glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
}
