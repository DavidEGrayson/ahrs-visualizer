#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <wordexp.h>
#include <fcntl.h>

#include <bcm_host.h>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <boost/program_options.hpp>
namespace opts = boost::program_options;

#include "asset_dir.h"
#include "model_board/model_board.h"
#include "version.h"

/* OpenGL rotation matrix that converts board coordinates
 * into ground coordinates (x=north, y=east, z=down).  It is
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

static inline VC_RECT_T rect_width_height(int width, int height)
{
    VC_RECT_T r;
    r.x = r.y = 0;
    r.width = width;
    r.height = height;
    return r;
}

static void nice_bcm_host_init()
{
    int fd = open("/dev/vchiq", O_RDWR | O_LARGEFILE);
    if (fd == -1)
    {
        char buffer[256];
        char * msg = strerror_r(errno, buffer, sizeof(buffer) - 1);
        if (msg)
        {
            std::cerr << "Warning: Could not open /dev/vchiq: "
              << msg << "." << std::endl;
        }
        else
        {
            std::cerr << "Warning: Could not open /dev/vchiq." << std::endl;
        }
    }
    else
    {
        close(fd);
    }

    bcm_host_init();
}

// Sets the display, OpenGL|ES context and screen stuff
static void opengl_init(void)
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
    if (display == EGL_NO_DISPLAY)
    {
        throw std::runtime_error("Failed to get display.  eglGetDisplay failed.");
    }

    // initialize the EGL display connection
    result = eglInitialize(display, NULL, NULL);
    if (result == EGL_FALSE)
    {
        throw std::runtime_error("Failed to initialize display.  eglInitialize failed.");
    }

    // get an appropriate EGL frame buffer configuration
    result = eglChooseConfig(display, attribute_list, &config, 1, &num_config);
    if (result == EGL_FALSE)
    {
        throw std::runtime_error("Failed to choose config.  eglChooseConfig failed.");
    }

    // create an EGL rendering context
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, NULL);
    if (context == EGL_NO_CONTEXT)
    {
        throw std::runtime_error("Failed to create context.  eglCreateContext failed.");
    }

    // create an EGL window surface
    int32_t success = graphics_get_display_size(0 /* LCD */, &screen_width, &screen_height);
    if (success < 0)
    {
        throw std::runtime_error("Failed to get display size.");
    }

    VC_RECT_T dst_rect = rect_width_height(screen_width, screen_height);
    VC_RECT_T src_rect = rect_width_height(screen_width<<16, screen_height<<16);

    dispman_display = vc_dispmanx_display_open(0 /* LCD */);
    dispman_update = vc_dispmanx_update_start(0);

    dispman_element = vc_dispmanx_element_add (dispman_update, dispman_display,
                                               0/*layer*/, &dst_rect, 0/*src*/,
                                               &src_rect, DISPMANX_PROTECTION_NONE, 0 /*alpha*/, 0/*clamp*/, (DISPMANX_TRANSFORM_T)0/*transform*/);

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
    if (result == EGL_FALSE)
    {
        throw std::runtime_error("Failed to connect to the surface.");
    }

    glClearColor(0, 0, 0, 0.5);   // set background colors
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffers
    glShadeModel(GL_FLAT);

    // Enable back face culling.
    glEnable(GL_CULL_FACE);
}

// Description: Sets the OpenGL|ES model to default values
static void projection_init()
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

static void textures_init(void)
{
    // Enable alpha blending so what we see through transparent
    // parts of the model is the same as the background.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_TEXTURE_2D);
}


static void redraw_scene()
{
    // Start with a clear screen
    glClear(GL_COLOR_BUFFER_BIT);

    // Move the camera back so we can see the board.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -30);

    // Convert screen coords (right, up, out) to ground coords (north, east, down).
    glRotatef(90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);
    glRotatef(-screen_orientation, 0, 0, 1);

    // Convert ground coords to board coordinates.
    glMultMatrixf(matrix[0]);

    model_board_redraw(acceleration, magnetic_field);

    eglSwapBuffers(display, surface);
}

static void opengl_deinit(void)
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

static void read_args(int argc, char *argv[])
{
    try
    {
        opts::options_description generic("Generic options");
        generic.add_options()
            ("help,h", "produce help message")
            ("version,v", "print version number")
            ;

        opts::options_description config("Configuration");
        config.add_options()
            ("screen-angle,a", opts::value<float>(&screen_orientation)->default_value(0),
             "specifies your screen orientation.  "
             "0 = screen faces South, 90 = West, 180 = North, 270 = East\n")
            ;

        opts::options_description cmdline_options;
        cmdline_options.add(generic);
        cmdline_options.add(config);

        // Read options from command line.
        opts::variables_map options;
        opts::store(opts::command_line_parser(argc, argv).options(cmdline_options).run(), options);

        // Read options form config file, ~/.ahrs-visualizer
        {
            wordexp_t expansion_result;
            wordexp("~/.ahrs-visualizer", &expansion_result, 0);
            std::ifstream file(expansion_result.we_wordv[0]);
            opts::store(opts::parse_config_file(file, config), options);
        }

        opts::notify(options);

        if(options.count("help"))
        {
            std::cout << cmdline_options;
            std::cout << "For more information, run: man ahrs-visualizer" << std::endl;
            exit(0);
        }

        if (options.count("version"))
        {
            std::cout << VERSION << std::endl;
            exit(0);
        }
    }
    catch(const opts::multiple_occurrences & error)
    {
        std::cerr << "Error: " << error.what() << " of " << error.get_option_name() << " option." << std::endl;
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    try
    {
        read_args(argc, argv);

        asset_dir_init();
        nice_bcm_host_init();
        opengl_init();
        projection_init();
        textures_init();
        model_board_init();

        while(1)
        {
            read_input();
            redraw_scene();
        }

        opengl_deinit();
        bcm_host_deinit();
        return 0;
    }
    catch(const std::exception & error)
    {
        std::cerr << "Error: " << error.what() << std::endl;
        exit(9);
    }
}

