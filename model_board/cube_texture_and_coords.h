#define EZ 1

static const GLbyte quadx[6*4*3] = {
   /* FRONT */
   -10, -10,  EZ,
   10, -10,  EZ,
   -10,  10,  EZ,
   10,  10,  EZ,

   /* BACK */
   -10, -10, -EZ,
   -10,  10, -EZ,
   10, -10, -EZ,
   10,  10, -EZ,

   /* LEFT */
   -10, -10,  EZ,
   -10,  10,  EZ,
   -10, -10, -EZ,
   -10,  10, -EZ,

   /* RIGHT */
   10, -10, -EZ,
   10,  10, -EZ,
   10, -10,  EZ,
   10,  10,  EZ,

   /* TOP */
   -10,  10,  EZ,
   10,  10,  EZ,
   -10,  10, -EZ,
   10,  10, -EZ,

   /* BOTTOM */
   -10, -10,  EZ,
   -10, -10, -EZ,
   10, -10,  EZ,
   10, -10, -EZ,
};

/** Texture coordinates for the quad. */
static const GLfloat texCoords[6 * 4 * 2] = {
   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,

   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,

   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,

   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,

   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f,

   0.f,  0.f,
   0.f,  1.f,
   1.f,  0.f,
   1.f,  1.f
};

// Colors are invisible when textures appear on all 6 faces.
// If textures are disabled, e.g. by commenting out glEnable(GL_TEXTURE_2D),
// the colours will appear.

static const GLfloat colorsf[6*4*4] = {
   1.f,  0.f,  0.f,  1.f,  //red
   1.f,  0.f,  0.f,  1.f,
   1.f,  0.f,  0.f,  1.f,
   1.f,  0.f,  0.f,  1.f,

   0.f,  1.f,  0.f,  1.f,  // blue
   0.f,  1.f,  0.f,  1.f,
   0.f,  1.f,  0.f,  1.f,
   0.f,  1.f,  0.f,  1.f,

   0.f,  0.f,  1.f,  1.f, // green
   0.f,  0.f,  1.f,  1.f,
   0.f,  0.f,  1.f,  1.f,
   0.f,  0.f,  1.f,  1.f,

   0.f, 0.5f, 0.5f,  1.f, // teal
   0.f, 0.5f, 0.5f,  1.f,
   0.f, 0.5f, 0.5f,  1.f,
   0.f, 0.5f, 0.5f,  1.f,

   0.5f, 0.5f,  0.f,  1.f, // yellow
   0.5f, 0.5f,  0.f,  1.f,
   0.5f, 0.5f,  0.f,  1.f,
   0.5f, 0.5f,  0.f,  1.f,

   0.5f,  0.f, 0.5f,  1.f, // purple
   0.5f,  0.f, 0.5f,  1.f,
   0.5f,  0.f, 0.5f,  1.f,
   0.5f,  0.f, 0.5f,  1.f
};
