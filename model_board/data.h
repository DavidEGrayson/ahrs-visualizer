#define EX 9   // X dimension
#define EY 6   // Y dimension
#define EZ 1   // Thickness

#define AL 10  // axes length

static GLfloat vertices[] = {
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

   /* X AXIS */
    0,  0,  0,
   AL,  0,  0,

   /* Y AXIS */
    0,  0,  0,
    0, AL,  0,

   /* Z AXIS */
    0,  0,  0,
    0,  0, AL,

   // Acceleration
   0, 0, 0,
   0, 0, 0,

   // Magnetic field
   0, 0, 0,
   0, 0, 0,
};

/** Texture coordinates for the quad. */
static GLfloat texCoords[] = {
   0,  0,
   1,  0,
   0,  1,
   1,  1,

   0,  0,
   0,  1,
   1,  0,
   1,  1,

   0,  0,
   0,  1,
   1,  0,
   1,  1,

   0,  0,
   0,  1,
   1,  0,
   1,  1,

   0,  0,
   0,  1,
   1,  0,
   1,  1,

   0,  0,
   0,  1,
   1,  0,
   1,  1,

   // These objects don't use textures:
   0,0, 0,0, // x axis
   0,0, 0,0, // y axis
   0,0, 0,0, // z axis
   0,0, 0,0, // acceleration line
   0,0, 0,0, // magnetic field lines
};


static const GLfloat colors[] = {
   1,  0,  0,  1,  //red
   1,  0,  0,  1,
   1,  0,  0,  1,
   1,  0,  0,  1,

   0,  1,  0,  1,  // blue
   0,  1,  0,  1,
   0,  1,  0,  1,
   0,  1,  0,  1,

   0,  0,  1,  1, // green
   0,  0,  1,  1,
   0,  0,  1,  1,
   0,  0,  1,  1,

   0, 0.5, 0.5,  1, // teal
   0, 0.5, 0.5,  1,
   0, 0.5, 0.5,  1,
   0, 0.5, 0.5,  1,

   0.5, 0.5,  0,  1, // yellow
   0.5, 0.5,  0,  1,
   0.5, 0.5,  0,  1,
   0.5, 0.5,  0,  1,

   0.5,  0, 0.5,  1, // purple
   0.5,  0, 0.5,  1,
   0.5,  0, 0.5,  1,
   0.5,  0, 0.5,  1,

   // X axis: red
   1, 0, 0, 1,
   1, 0, 0, 1,

   // Y axis: green
   0, 1, 0, 1,
   0, 1, 0, 1,
   
   // Z axis: blue
   0, 0, 1, 1,
   0, 0, 1, 1,

   // Acceleration: cyan
   0, 1, 1, 0.3,
   0, 1, 1, 1,

   // Magnetic field: yellow
   1, 1, 0, 0.3,
   1, 1, 0, 1,
};
