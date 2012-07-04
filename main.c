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

#include "model_board/model_board.h"

/* OpenGL rotation matrix that converts ground coordinates
 * (x=easy, y=north, z=up) into board coordinates.  It is
 * column-major so do matrix[COL][ROW]. */
float matrix[4][4];

// Acceleration vector in units of g (9.8 m/s^2).
float acceleration[3];

// Magnetic field vector where each component is approximately between -1 and 1.
float magnetic_field[3];

// 0 = Screen faces south, 90 = West, 180 = North, 270 = East
// TODO: read screen_orientation from environment
float screen_orientation = 0;

uint32_t screen_width, screen_height;
EGLDisplay display;
EGLSurface surface;
EGLContext context;

// Sets the display, OpenGL|ES context and screen stuff
static void init_opengl(void)
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
static void init_projection()
{
    float nearp = 1, farp = 500.0f, hht, hwd;

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    hht = nearp * tan(45.0 / 2.0 / 180.0 * M_PI);
    hwd = hht * screen_width / screen_height;

    glFrustumf(-hwd, hwd, -hht, hht, nearp, farp);
}

static void init_textures(void)
{
    glEnable(GL_TEXTURE_2D);
    model_board_init();
}

static void redraw_scene()
{
    // Start with a clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Move the camera back so we can see the board.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -30);

    // Convert screen coords (x=right y=up z=out) to ground coords (x=east y=north z=up).
    glRotatef(-90, 1, 0, 0);
    glRotatef(screen_orientation, 0, 0, 1);

    // Convert ground coords to board coordinates.
    glMultMatrixf(matrix[0]);

    model_board_redraw(acceleration, magnetic_field);

    eglSwapBuffers(display, surface);
}

static void exit_func(void)
{
    // clear screen
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(display, surface);

    // Release OpenGL resources
    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroySurface(display, surface);
    eglDestroyContext(display, context);
    eglTerminate(display);
}

/* The matrix expected on the standrd input is a 
 * ROW-major matrix that converts a vector from board coordinates
 * to ground coordinates. */
static void read_input(void)
{
    // Set the translation part to be the identity.
    matrix[3][0] = matrix[3][1] = matrix[3][2] = 0;
    matrix[0][3] = matrix[1][3] = matrix[2][3] = 0;
    matrix[3][3] = 1;

    while(1)
    {
        // Read the rotation matrix from the standard input.
        // The input values are ROW-major but we need to make a
        // COLUMN-major matrix for OpenGL.
        int result = scanf("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
                           &matrix[0][0], &matrix[1][0], &matrix[2][0],
                           &matrix[0][1], &matrix[1][1], &matrix[2][1],
                           &matrix[0][2], &matrix[1][2], &matrix[2][2],
                           &acceleration[0], &acceleration[1], &acceleration[2],
                           &magnetic_field[0], &magnetic_field[1], &magnetic_field[2]);

        // Read to the end of the line so that we don't get stuck forever on one invalid line.
        while(getc(stdin) != '\n');

        if (result >= 9)
        {
            break; // Success
        }

        fprintf(stderr, "unrecognized input: only regonized %d items.\n", result);
    }
}

int main()
{
    bcm_host_init();
    init_opengl();
    init_projection();
    init_textures();

    while(1)
    {
        read_input();
        redraw_scene();
    }

    exit_func();
    return 0;
}

