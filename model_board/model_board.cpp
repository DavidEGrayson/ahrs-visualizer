#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <stdexcept>

#include <GLES/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include "data.h"
#include "../png_texture.h"
#include "../asset_dir.h"

static GLuint texture_top, texture_bottom;

void model_board_init(void)
{
    std::string asset_dir = asset_dir_get();
    texture_top = png_texture_load((asset_dir + "/top2.png").c_str(), NULL, NULL);
    texture_bottom = png_texture_load((asset_dir + "/bottom2.png").c_str(), NULL, NULL);

    if (texture_top == 0 || texture_bottom == 0)
    {
        throw std::runtime_error("Loading textures failed.");
    }

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, colors);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, texCoords);
}

void model_board_redraw(float * acceleration, float * magnetic_field)
{
    glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    // Top
    glBindTexture(GL_TEXTURE_2D, texture_top);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Bottom
    glBindTexture(GL_TEXTURE_2D, texture_bottom);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);

    glBindTexture(GL_TEXTURE_2D, 0);  // Turn off textures

    // Other sides
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);


    // X, Y, Z axes
    glDrawArrays(GL_LINES, 24, 2);
    glDrawArrays(GL_LINES, 26, 2);
    glDrawArrays(GL_LINES, 28, 2);

    // Acceleration
    const float accel_scale = 3;
    vertices[3*31+0] = acceleration[0] * accel_scale;
    vertices[3*31+1] = acceleration[1] * accel_scale;
    vertices[3*31+2] = acceleration[2] * accel_scale;
    glDrawArrays(GL_LINES, 30, 2);

    // Magnetic field
    const float mag_scale = 3;
    vertices[3*33+0] = magnetic_field[0] * mag_scale;
    vertices[3*33+1] = magnetic_field[1] * mag_scale;
    vertices[3*33+2] = magnetic_field[2] * mag_scale;
    glDrawArrays(GL_LINES, 32, 2);
}
