// All the units are inch/10

#define EX 9   // X dimension
#define EY 6   // Y dimension
#define EZ 1   // Thickness

static const GLbyte quadx[6*4*3] = {
   /* TOP */
   -EX, -EY,  EZ,
    EX, -EY,  EZ,
   -EX,  EY,  EZ,
    EX,  EY,  EZ,

   /* BOTTOM */
   -EX, -EY,   0,
   -EX,  EY,   0,
    EX, -EY,   0,
    EX,  EY,   0,

   /* LEFT EDGE */
   -EX, -EY,  EZ,
   -EX,  EY,  EZ,
   -EX, -EY,   0,
   -EX,  EY,   0,

   /* RIGHT EDGE */
   EX, -EY,   0,
   EX,  EY,   0,
   EX, -EY,  EZ,
   EX,  EY,  EZ,

   /* FAR EDGE */
   -EX,  EY,  EZ,
    EX,  EY,  EZ,
   -EX,  EY,   0,
    EX,  EY,   0,

   /* CLOSE EDGE */
   -EX, -EY,  EZ,
   -EX, -EY,   0,
    EX, -EY,  EZ,
    EX, -EY,   0,
};

/** Texture coordinates for the quad. */
static const GLfloat texCoords[6 * 4 * 2] = {
   0.f,  0.f,
   1.f,  0.f,
   0.f,  1.f,
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
