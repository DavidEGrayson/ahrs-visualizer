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
};

/** Texture coordinates for the quad. */
static GLfloat texCoords[] = {
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
   1.f,  1.f,

   // x,y,z axis: hopefully ignored
   0,0, 0,0, 0,0,
   0,0, 0,0, 0,0,
};


static const GLfloat colors[] = {
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
   0.5f,  0.f, 0.5f,  1.f,

   // red for X axis
   1, 0, 0, 1,
   1, 0, 0, 1,

   // green for Y axis
   0, 1, 0, 1,
   0, 1, 0, 1,
   
   // blue for Z axis
   0, 0, 1, 1,
   0, 0, 1, 1,
};
