#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>

#include <bcm_host.h>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "cube_texture_and_coords.h"

#define PATH "./"

#define IMAGE_SIZE 128

#ifndef M_PI
   #define M_PI 3.141592654
#endif

uint32_t screen_width, screen_height;
EGLDisplay display;
EGLSurface surface;
EGLContext context;

typedef struct
{
   GLuint tex[6];

   // model rotation vector and direction
   GLfloat rot_angle_x_inc, rot_angle_y_inc, rot_angle_z_inc;

   // current model rotation angles
   GLfloat rot_angle_x, rot_angle_y, rot_angle_z;

   // current distance from camera
   GLfloat distance;

} CUBE_STATE_T;

static void init_ogl();
static void init_model_proj(CUBE_STATE_T *state);
static void reset_model(CUBE_STATE_T *state);
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc);
static void redraw_scene(CUBE_STATE_T *state);
static void update_model(CUBE_STATE_T *state);
static void init_textures(CUBE_STATE_T *state);
static void exit_func(void);
static CUBE_STATE_T _state, *state=&_state;


// Sets the display, OpenGL|ES context and screen stuff
static void init_ogl()
{
   EGLBoolean result;
   EGLint num_config;

   static EGL_DISPMANX_WINDOW_T nativewindow;

   DISPMANX_ELEMENT_HANDLE_T dispman_element;
   DISPMANX_DISPLAY_HANDLE_T dispman_display;
   DISPMANX_UPDATE_HANDLE_T dispman_update;

   static const EGLint attribute_list[] =
   {
      EGL_RED_SIZE, 8,
      EGL_GREEN_SIZE, 8,
      EGL_BLUE_SIZE, 8,
      EGL_ALPHA_SIZE, 8,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
   };
   
   EGLConfig config;

   // get an EGL display connection
   display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
   assert(display!=EGL_NO_DISPLAY);

   // initialize the EGL display connection
   result = eglInitialize(display, NULL, NULL);
   assert(EGL_FALSE != result);

   // get an appropriate EGL frame buffer configuration
   result = eglChooseConfig(display, attribute_list, &config, 1, &num_config);
   assert(EGL_FALSE != result);

   // create an EGL rendering context
   context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
   assert(context!=EGL_NO_CONTEXT);

   // create an EGL window surface
   int32_t success = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);
   assert( success >= 0 );

   VC_RECT_T dst_rect = {.x = 0, .y = 0, .width = screen_width, .height = screen_height};
   VC_RECT_T src_rect = {.x = 0, .y = 0, .width = screen_width<<16, .height = screen_height<<16};

   dispman_display = vc_dispmanx_display_open(0 /* LCD */);
   dispman_update = vc_dispmanx_update_start(0);

   dispman_element = vc_dispmanx_element_add (dispman_update, dispman_display,
      0/*layer*/, &dst_rect, 0/*src*/,
      &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, 0/*transform*/);

   nativewindow.element = dispman_element;
   nativewindow.width = screen_width;
   nativewindow.height = screen_height;
   vc_dispmanx_update_submit_sync(dispman_update);

   surface = eglCreateWindowSurface(display, config, &nativewindow, NULL);
   if(surface == EGL_NO_SURFACE)
   {
      fprintf(stderr, "eglCreateWindowSurface returned ELG_NO_SURFACE.  "
        "Try closing other OpenGL programs.\n");
      exit(1);
   }

   // connect the context to the surface
   result = eglMakeCurrent(display, surface, surface, context);
   assert(EGL_FALSE != result);

   glClearColor(0, 0, 0, 0.5);   // set background colors
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers
   glShadeModel(GL_FLAT);

   // Enable back face culling.
   glEnable(GL_CULL_FACE);
}

// Description: Sets the OpenGL|ES model to default values
static void init_model_proj(CUBE_STATE_T *state)
{
   float nearp = 1.0f;
   float farp = 500.0f;
   float hht;
   float hwd;

   glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

   glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
      
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();

   hht = nearp * (float)tan(45.0 / 2.0 / 180.0 * M_PI);
   hwd = hht * (float)screen_width / (float)screen_height;

   glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
   
   glEnableClientState( GL_VERTEX_ARRAY );
   glVertexPointer( 3, GL_BYTE, 0, quadx );

   glEnableClientState( GL_COLOR_ARRAY );
   glColorPointer(4, GL_FLOAT, 0, colorsf);

   reset_model(state);
}

static void reset_model(CUBE_STATE_T *state)
{
   state->rot_angle_x = 45.f;
   state->rot_angle_y = 30.f;
   state->rot_angle_z = 0.0f;
   state->rot_angle_x_inc = 0.5f;
   state->rot_angle_y_inc = 0.6f;
   state->rot_angle_z_inc = 0.0f;
   state->distance = 60.0f;
}

// Updates model variables to current position/rotation
static void update_model(CUBE_STATE_T *state)
{
   state->rot_angle_x = inc_and_wrap_angle(state->rot_angle_x, state->rot_angle_x_inc);
   state->rot_angle_y = inc_and_wrap_angle(state->rot_angle_y, state->rot_angle_y_inc);
   state->rot_angle_z = inc_and_wrap_angle(state->rot_angle_z, state->rot_angle_z_inc);
}

/***********************************************************
 * Arguments:
 *       GLfloat angle     current angle
 *       GLfloat angle_inc angle increment
 *
 * Description:   Increments or decrements angle by angle_inc degrees
 *                Wraps to 0 at 360 deg.
 *
 * Returns: new value of angle
 ***********************************************************/
static GLfloat inc_and_wrap_angle(GLfloat angle, GLfloat angle_inc)
{
   angle += angle_inc;

   if (angle >= 360)
      angle -= 360;
   else if (angle <= 0)
      angle += 360;

   return angle;
}

/***********************************************************
 *       CUBE_STATE_T *state - holds OGLES model info
 *
 * Description:   Draws the model and calls eglSwapBuffers
 *                to render to screen
 ***********************************************************/
static void redraw_scene(CUBE_STATE_T *state)
{
   // Start with a clear screen
   glClear(GL_COLOR_BUFFER_BIT);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   // move camera back to see the cube
   glTranslatef(0, 0, -state->distance);

   glRotatef(state->rot_angle_x, 1, 0, 0);
   glRotatef(state->rot_angle_y, 0, 1, 0);
   glRotatef(state->rot_angle_z, 0, 0, 1);

   glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

   // Draw first (front) face:
   // Bind texture surface to current vertices
   glBindTexture(GL_TEXTURE_2D, state->tex[0]);

   // Need to rotate textures - do this by rotating each cube face
   glRotatef(270, 0, 0, 1); // front face normal along z axis

   // draw first 4 vertices
   glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

   // same pattern for other 5 faces - rotation chosen to make image orientation 'nice'
   glBindTexture(GL_TEXTURE_2D, state->tex[1]);
   glRotatef(90, 0, 0, 1); // back face normal along z axis
   glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[2]);
   glRotatef(90, 1, 0, 0); // left face normal along x axis
   glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[3]);
   glRotatef(90, 1, 0, 0); // right face normal along x axis
   glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);

   glBindTexture(GL_TEXTURE_2D, state->tex[4]);
   glRotatef(-90, 0, 1, 0); // top face normal along y axis
   glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);

   glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

   glBindTexture(GL_TEXTURE_2D, state->tex[5]);
   glRotatef(90, 0, 1, 0); // bottom face normal along y axis
   glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);

   eglSwapBuffers(display, surface);
}

static GLuint init_texture(const char * file_name, int width, int height)
{
   int size = width*height*3;
   unsigned char * data = malloc(size);
   if (data == 0)
   {
      fprintf(stderr, "Unable to allocate %d bytes for %s texture.\n", size, file_name);
      exit(2);
   }

   FILE * file = fopen(file_name, "rb");
   if (file == 0)
   {
      perror(file_name);
      exit(3);
   }

   size_t bytes_read = fread(data, 1, size, file);
   fclose(file);
   if (bytes_read != size)
   {
      fprintf(stderr, "Expected to read %d bytes from %s but read %d.\n", size, file_name, bytes_read);
      free(data);
      exit(4);
   }

   GLuint texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   free(data);
   return texture;
}

static void init_textures(CUBE_STATE_T *state)
{
   glEnable(GL_TEXTURE_2D);
   glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   state->tex[1] = state->tex[0] = init_texture(PATH "Lucca_128_128.raw", 128, 128);
   state->tex[3] = state->tex[2] = init_texture(PATH "Djenne_128_128.raw", 128, 128);
   state->tex[5] = state->tex[4] = init_texture(PATH "Gaudi_128_128.raw", 128, 128);
}

static void exit_func(void)
{
   // clear screen
   glClear( GL_COLOR_BUFFER_BIT );
   eglSwapBuffers(display, surface);

   // Release OpenGL resources
   eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
   eglDestroySurface(display, surface);
   eglDestroyContext(display, context);
   eglTerminate(display);
}

int main()
{
   bcm_host_init();

   // Start OGLES
   init_ogl();

   // Setup the model world
   init_model_proj(state);

   // initialise the OGLES texture(s)
   init_textures(state);

   while(1)
   {
      usleep(5*1000);
      update_model(state);
      redraw_scene(state);
   }
   exit_func();
   return 0;
}

