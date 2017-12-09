/* 
 * Testbed for GLSL fragment noise() replacement.
 *
 * Shaders are loaded from two external files:
 * "GLSLnoisetest.vert" and "GLSLnoisetest.frag".
 * The program itself draws a spinning sphere
 * with a noise-generated fragment color.
 *
 * This program uses GLFW for convenience, to handle the OS-specific
 * window management stuff. Some Windows-specific stuff is still there,
 * though, so this code is not portable to other platforms.
 * The non-portable parts of the code is the MessageBox() function
 * call, which is easily replaced by a fprintf(stderr) call,
 * and probably most of the stupid loading of function pointers that
 * Windows requires of you to access anything above OpenGL 1.1.
 *
 * Author: Stefan Gustavson (stegu@itn.liu.se) 2004
 */

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glfw.h>

// The default include file for GL extensions might not be up to date.
// #include <GL/glext.h>
#include "glext.h"

// The ugly "windows.h" is required only for the message boxes.
#include "windows.h" 

/* Global variables for all the nice ARB extensions we need */
PFNGLACTIVETEXTUREPROC           glActiveTexture           = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC  glCreateProgramObjectARB  = NULL;
PFNGLDELETEOBJECTARBPROC         glDeleteObjectARB         = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC     glUseProgramObjectARB     = NULL;
PFNGLCREATESHADEROBJECTARBPROC   glCreateShaderObjectARB   = NULL;
PFNGLSHADERSOURCEARBPROC         glShaderSourceARB         = NULL;
PFNGLCOMPILESHADERARBPROC        glCompileShaderARB        = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB = NULL;
PFNGLATTACHOBJECTARBPROC         glAttachObjectARB         = NULL;
PFNGLGETINFOLOGARBPROC           glGetInfoLogARB           = NULL;
PFNGLLINKPROGRAMARBPROC          glLinkProgramARB          = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC   glGetUniformLocationARB   = NULL;
PFNGLUNIFORM4FARBPROC            glUniform4fARB            = NULL;
PFNGLUNIFORM1FARBPROC            glUniform1fARB            = NULL;
PFNGLUNIFORM1IARBPROC            glUniform1iARB            = NULL;

/* Some more global variables for convenience. */
double t0 = 0.0;
int frames = 0;
char titlestring[200];

GLuint permTextureID;
GLuint simplexTextureID;
GLuint gradTextureID;
GLuint sphereList;
GLboolean updateTime = GL_TRUE;
GLboolean animateObject = GL_TRUE;

GLhandleARB programObj;
GLhandleARB vertexShader;
GLhandleARB fragmentShader;
GLint      location_permTexture = -1; 
GLint      location_simplexTexture = -1; 
GLint      location_gradTexture = -1; 
GLint      location_time = -1;

const char *vertexShaderStrings[1];
const char *fragmentShaderStrings[1];
GLint vertexCompiled;
GLint fragmentCompiled;
GLint shadersLinked;
char str[4096]; // For error messages from the GLSL compiler and linker

int perm[256]= {151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

/* These are Ken Perlin's proposed gradients for 3D noise. I kept them for
   better consistency with the reference implementation, but there is really
   no need to pad this to 16 gradients for this particular implementation.
   If only the "proper" first 12 gradients are used, they can be extracted
   from the grad4[][] array: grad3[i][j] == grad4[i*2][j], 0<=i<=11, j=0,1,2
*/
int grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
                   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
                   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
                   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16

/* These are my own proposed gradients for 4D noise. They are the coordinates
   of the midpoints of each of the 32 edges of a tesseract, just like the 3D
   noise gradients are the midpoints of the 12 edges of a cube.
*/
int grad4[32][4]= {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
                   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
                   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
                   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
                   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
                   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
                   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
                   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};

/* This is a look-up table to speed up the decision on which simplex we
   are in inside a cube or hypercube "cell" for 3D and 4D simplex noise.
   It is used to avoid complicated nested conditionals in the GLSL code.
   The table is indexed in GLSL with the results of six pair-wise
   comparisons beween the components of the P=(x,y,z,w) coordinates
   within a hypercube cell.
   c1 = x>=y ? 32 : 0;
   c2 = x>=z ? 16 : 0;
   c3 = y>=z ? 8 : 0;
   c4 = x>=w ? 4 : 0;
   c5 = y>=w ? 2 : 0;
   c6 = z>=w ? 1 : 0;
   offsets = simplex[c1+c2+c3+c4+c5+c6];
   o1 = step(160,offsets);
   o2 = step(96,offsets);
   o3 = step(32,offsets);
   (For the 3D case, c4, c5, c6 and o3 are not needed.)
*/
  unsigned char simplex4[][4] = {{0,64,128,192},{0,64,192,128},{0,0,0,0},
  {0,128,192,64},{0,0,0,0},{0,0,0,0},{0,0,0,0},{64,128,192,0},
  {0,128,64,192},{0,0,0,0},{0,192,64,128},{0,192,128,64},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{64,192,128,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {64,128,0,192},{0,0,0,0},{64,192,0,128},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{128,192,0,64},{128,192,64,0},
  {64,0,128,192},{64,0,192,128},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{128,0,192,64},{0,0,0,0},{128,64,192,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {128,0,64,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,0,64,128},{192,0,128,64},{0,0,0,0},{192,64,128,0},
  {128,64,0,192},{0,0,0,0},{0,0,0,0},{0,0,0,0},
  {192,64,0,128},{0,0,0,0},{192,128,0,64},{192,128,64,0}};

/*
 * printError() - Signal an error. MessageBox() is a Windows-specific function,
 * rewrite this to print the error message to the console to make it portable!
 */
void printError(const char *errtype, const char *errmsg) {
  // fprintf(stderr, "%s: %s\n", errtype, errmsg);
  MessageBox(NULL, errmsg, errtype, MB_OK|MB_ICONEXCLAMATION);
}


/*
 * loadExtensions() - Load OpenGL extensions for anything above OpenGL
 * version 1.1. (This is a requirement from Windows, not from OpenGL.)
 */
void loadExtensions() {
    //These extension strings indicate that the OpenGL Shading Language,
    // version 1.00, and GLSL shader objects are supported.
    if(!glfwExtensionSupported("GL_ARB_shading_language_100"))
    {
        printError("GL init error", "GL_ARB_shading_language_100 extension was not found");
        return;
    }
    if(!glfwExtensionSupported("GL_ARB_shader_objects"))
    {
        printError("GL init error", "GL_ARB_shader_objects extension was not found");
        return;
    }
    else
    {
        glActiveTexture           = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
        glCreateProgramObjectARB  = (PFNGLCREATEPROGRAMOBJECTARBPROC)wglGetProcAddress("glCreateProgramObjectARB");
        glDeleteObjectARB         = (PFNGLDELETEOBJECTARBPROC)wglGetProcAddress("glDeleteObjectARB");
        glUseProgramObjectARB     = (PFNGLUSEPROGRAMOBJECTARBPROC)wglGetProcAddress("glUseProgramObjectARB");
        glCreateShaderObjectARB   = (PFNGLCREATESHADEROBJECTARBPROC)wglGetProcAddress("glCreateShaderObjectARB");
        glShaderSourceARB         = (PFNGLSHADERSOURCEARBPROC)wglGetProcAddress("glShaderSourceARB");
        glCompileShaderARB        = (PFNGLCOMPILESHADERARBPROC)wglGetProcAddress("glCompileShaderARB");
        glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)wglGetProcAddress("glGetObjectParameterivARB");
        glAttachObjectARB         = (PFNGLATTACHOBJECTARBPROC)wglGetProcAddress("glAttachObjectARB");
        glGetInfoLogARB           = (PFNGLGETINFOLOGARBPROC)wglGetProcAddress("glGetInfoLogARB");
        glLinkProgramARB          = (PFNGLLINKPROGRAMARBPROC)wglGetProcAddress("glLinkProgramARB");
        glGetUniformLocationARB   = (PFNGLGETUNIFORMLOCATIONARBPROC)wglGetProcAddress("glGetUniformLocationARB");
        glUniform4fARB            = (PFNGLUNIFORM4FARBPROC)wglGetProcAddress("glUniform4fARB");
        glUniform1fARB            = (PFNGLUNIFORM1FARBPROC)wglGetProcAddress("glUniform1fARB");
        glUniform1iARB            = (PFNGLUNIFORM1IARBPROC)wglGetProcAddress("glUniform1iARB");

        if( !glCreateProgramObjectARB || !glDeleteObjectARB || !glUseProgramObjectARB ||
            !glCreateShaderObjectARB || !glCreateShaderObjectARB || !glCompileShaderARB || 
            !glGetObjectParameterivARB || !glAttachObjectARB || !glGetInfoLogARB || 
            !glLinkProgramARB || !glGetUniformLocationARB || !glUniform4fARB ||
      			!glUniform1fARB || !glUniform1iARB )
        {
            printError("GL init error", "One or more GL_ARB_shader_objects functions were not found");
            return;
        }
    }
}


/*
 * readShaderFile(filename) - read a shader source string from a file
 */
unsigned char* readShaderFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if(file == NULL)
    {
        printError("ERROR", "Cannot open shader file!");
  		  return 0;
    }
    int bytesinfile = filelength(fileno(file));
    unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
    int bytesread = fread( buffer, 1, bytesinfile, file);
    buffer[bytesread] = 0; // Terminate the string with 0
    fclose(file);
    
    return buffer;
}


/*
 * createShaders() - create, load, compile and link the GLSL shader objects.
 */
void createShaders() {
	  // Create the vertex shader.
    vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

	  unsigned char *vertexShaderAssembly = readShaderFile("GLSLnoisetest4.vert");
    vertexShaderStrings[0] = (char*)vertexShaderAssembly;
    glShaderSourceARB( vertexShader, 1, vertexShaderStrings, NULL );
    glCompileShaderARB( vertexShader);
    free((void *)vertexShaderAssembly);

    glGetObjectParameterivARB( vertexShader, GL_OBJECT_COMPILE_STATUS_ARB, 
                               &vertexCompiled );
    if(vertexCompiled  == GL_FALSE)
  	{
	  	glGetInfoLogARB(vertexShader, sizeof(str), NULL, str);
		  printError("Vertex shader compile error", str);
  	}

  	// Create the fragment shader.
    fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

    unsigned char *fragmentShaderAssembly = readShaderFile( "GLSLnoisetest4.frag" );
    fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
    glShaderSourceARB( fragmentShader, 1, fragmentShaderStrings, NULL );
    glCompileShaderARB( fragmentShader );
    free((void *)fragmentShaderAssembly);

    glGetObjectParameterivARB( fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB, 
                               &fragmentCompiled );
    if(fragmentCompiled == GL_FALSE)
   	{
		  glGetInfoLogARB( fragmentShader, sizeof(str), NULL, str );
		  printError("Fragment shader compile error", str);
	  }

    // Create a program object and attach the two compiled shaders.
    programObj = glCreateProgramObjectARB();
    glAttachObjectARB( programObj, vertexShader );
    glAttachObjectARB( programObj, fragmentShader );

    // Link the program object and print out the info log.
    glLinkProgramARB( programObj );
    glGetObjectParameterivARB( programObj, GL_OBJECT_LINK_STATUS_ARB, &shadersLinked );

    if( shadersLinked == GL_FALSE )
	{
		glGetInfoLogARB( programObj, sizeof(str), NULL, str );
		printError("Program object linking error", str);
	}
	// Locate the uniform shader variables so we can set them later:
  // a texture ID ("permTexture") and a float ("time").
	location_permTexture = glGetUniformLocationARB( programObj, "permTexture" );
	if(location_permTexture == -1)
    printError("Binding error","Failed to locate uniform variable 'permTexture'.");
	location_simplexTexture = glGetUniformLocationARB( programObj, "simplexTexture" );
	if(location_simplexTexture == -1)
    printError("Binding error","Failed to locate uniform variable 'simplexTexture'.");
	location_gradTexture = glGetUniformLocationARB( programObj, "gradTexture" );
	if(location_gradTexture == -1)
    printError("Binding error","Failed to locate uniform variable 'gradTexture'.");
 	location_time = glGetUniformLocationARB( programObj, "time" );
	if(location_time == -1)
    printError("Binding error", "Failed to locate uniform variable 'time'.");
}


/*
 * showFPS() - Calculate and report frames per second
 * (updated once per second) in the window title bar
 */
void showFPS() {

    double t, fps;
    
    // Get current time
    t = glfwGetTime();  // Gets number of seconds since glfwInit()
    // If one second has passed, or if this is the very first frame
    if( (t-t0) > 1.0 || frames == 0 )
    {
        fps = (double)frames / (t-t0);
        sprintf(titlestring, "GLSL Perlin noise (%.1f FPS)", fps);
        glfwSetWindowTitle(titlestring);
        t0 = t;
        frames = 0;
    }
    frames ++;
}


/*
 * setupCamera() - set up the OpenGL projection and (model)view matrices
 */
void setupCamera() {

    int width, height;
    
    // Get window size. It may start out different from the requested
    // size, and will change if the user resizes the window.
    glfwGetWindowSize( &width, &height );
    if(height<=0) height=1; // Safeguard against iconified/closed window

    // Set viewport. This is the pixel rectangle we want to draw into.
    glViewport( 0, 0, width, height ); // The entire window

    // Select and setup the projection matrix.
    glMatrixMode(GL_PROJECTION); // "We want to edit the projection matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // 45 degrees FOV, same aspect ratio as viewport, depth range 1 to 100
    gluPerspective( 45.0f, (GLfloat)width/(GLfloat)height, 1.0f, 100.0f );

    // Select and setup the modelview matrix.
    glMatrixMode( GL_MODELVIEW ); // "We want to edit the modelview matrix"
    glLoadIdentity(); // Reset the matrix to identity
    // Look from 0,-4,0 towards 0,0,0 with Z as "up" in the image
    gluLookAt( 0.0f, -4.0f, 0.0f,  // Eye position
               0.0f, 0.0f, 0.0f,   // View point
               0.0f, 0.0f, 1.0f ); // Up vector
}


/*
 * drawTexturedSphere(r, segs) - Draw a sphere centered on the local
 * origin, with radius "r" and approximated by "segs" polygon segments,
 * with texture coordinates in a latitude-longitude mapping.
 */
void drawTexturedSphere(float r, int segs) {
  int i, j;
  float x, y, z, z1, z2, R, R1, R2;

  // Top cap
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,0,1);
  glTexCoord2f(0.5f,1.0f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,r);
  z = cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 0; i <= 2*segs; i++) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f((float)i/(2*segs), 1.0f-1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();  

  // Height segments
  for(j = 1; j < segs-1; j++) {
    z1 = cos(j*M_PI/segs);
    R1 = sin(j*M_PI/segs);
    z2 = cos((j+1)*M_PI/segs);
    R2 = sin((j+1)*M_PI/segs);
    glBegin(GL_TRIANGLE_STRIP);
    for(i = 0; i <= 2*segs; i++) {
      x = R1*cos(i*2.0*M_PI/(2*segs));
      y = R1*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z1);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)j/segs);
      glVertex3f(r*x, r*y, r*z1);
      x = R2*cos(i*2.0*M_PI/(2*segs));
      y = R2*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z2);
      glTexCoord2f((float)i/(2*segs), 1.0f-(float)(j+1)/segs);
      glVertex3f(r*x, r*y, r*z2);
    }
    glEnd();
  }

  // Bottom cap
  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0,0,-1);
  glTexCoord2f(0.5f, 1.0f); // This is an ugly (u,v)-mapping singularity
  glVertex3f(0,0,-r);
  z = -cos(M_PI/segs);
  R = sin(M_PI/segs);
    for(i = 2*segs; i >= 0; i--) {
      x = R*cos(i*2.0*M_PI/(2*segs));
      y = R*sin(i*2.0*M_PI/(2*segs));
      glNormal3f(x, y, z);
      glTexCoord2f(1.0f-(float)i/(2*segs), 1.0f/segs);
      glVertex3f(r*x, r*y, r*z);
    }
  glEnd();
}


/*
 * initSphereList(GLuint *listID, GLdouble scale) - create a display list
 * to render the sphere more efficently than calling lots of trigonometric
 * functions for each frame.
 * (A vertex array would be even faster, but I'm a bit lazy here.)
 */
void initSphereList(GLuint *listID, GLdouble scale)
{
  *listID = glGenLists(1);
  
  glNewList(*listID, GL_COMPILE);
  drawTexturedSphere(scale, 20);
  glEndList();
}


/*
 * initPermTexture(GLuint *texID) - create and load a 2D texture for
 * a combined index permutation and gradient lookup table.
 * This texture is used for 2D and 3D noise, both classic and simplex.
 */
void initPermTexture(GLuint *texID)
{
  char *pixels;
  int i,j;
  
  glGenTextures(1, texID); // Generate a unique texture ID
  glBindTexture(GL_TEXTURE_2D, *texID); // Bind the texture to texture unit 0

  pixels = (char*)malloc( 256*256*4 );
  for(i = 0; i<256; i++)
    for(j = 0; j<256; j++) {
      int offset = (i*256+j)*4;
      char value = perm[(j+perm[i]) & 0xFF];
      pixels[offset] = grad3[value & 0x0F][0] * 64 + 64;   // Gradient x
      pixels[offset+1] = grad3[value & 0x0F][1] * 64 + 64; // Gradient y
      pixels[offset+2] = grad3[value & 0x0F][2] * 64 + 64; // Gradient z
      pixels[offset+3] = value;                     // Permuted index
    }
  
  // GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

/*
 * initSimplexTexture(GLuint *texID) - create and load a 1D texture for a
 * simplex traversal order lookup table. This is used for simplex noise only,
 * and only for 3D and 4D noise where there are more than 2 simplices.
 * (3D simplex noise has 6 cases to sort out, 4D simplex noise has 24 cases.)
 */
void initSimplexTexture(GLuint *texID)
{
  glActiveTexture(GL_TEXTURE1); // Activate a different texture unit (unit 1)

  glGenTextures(1, texID); // Generate a unique texture ID
  glBindTexture(GL_TEXTURE_1D, *texID); // Bind the texture to texture unit 1

  // GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
  glTexImage1D( GL_TEXTURE_1D, 0, GL_RGBA, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, simplex4 );
  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glActiveTexture(GL_TEXTURE0); // Switch active texture unit back to 0 again
}

/*
 * initGradTexture(GLuint *texID) - create and load a 2D texture
 * for a 4D gradient lookup table. This is used for 4D noise only.
 */
void initGradTexture(GLuint *texID)
{
  char *pixels;
  int i,j;
  
  glActiveTexture(GL_TEXTURE2); // Activate a different texture unit (unit 2)

  glGenTextures(1, texID); // Generate a unique texture ID
  glBindTexture(GL_TEXTURE_2D, *texID); // Bind the texture to texture unit 2

  pixels = (char*)malloc( 256*256*4 );
  for(i = 0; i<256; i++)
    for(j = 0; j<256; j++) {
      int offset = (i*256+j)*4;
      char value = perm[(j+perm[i]) & 0xFF];
      pixels[offset] = grad4[value & 0x1F][0] * 64 + 64;   // Gradient x
      pixels[offset+1] = grad4[value & 0x1F][1] * 64 + 64; // Gradient y
      pixels[offset+2] = grad4[value & 0x1F][2] * 64 + 64; // Gradient z
      pixels[offset+3] = grad4[value & 0x1F][3] * 64 + 64; // Gradient z
    }
  
  // GLFW texture loading functions won't work here - we need GL_NEAREST lookup.
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  glActiveTexture(GL_TEXTURE0); // Switch active texture unit back to 0 again
}


/*
 * drawScene(float t) - the actual drawing commands to render our scene.
 */
void drawScene(float t) {
    glRotatef(30.0f, 1.0f, 0.0f, 0.0f); // Rotate the view somewhat
    glPushMatrix(); // Transforms to animate the light:
      glRotatef(30.0f*t, 0.0f, 0.0f, 1.0f);
      glTranslatef(5.0f, 0.0f, 0.0f); // Orbit around Z, 5 units from origin
      float lightpos0[4]={0.0f, 0.0f, 0.0f, 1.0f}; // Origin, in hom. coords
      glLightfv(GL_LIGHT0, GL_POSITION, lightpos0); // Set light position
    glPopMatrix(); // Revert to initial transform
    glPushMatrix(); // Transforms to animate the object:
      glRotatef(45.0f*t, 0.0f, 0.0f, 1.0f); // Spin around Z
      glColor3f(1.0f, 1.0f, 1.0f); // White base color
      // Enable lighting and the LIGHT0 we placed before
      glEnable(GL_LIGHTING);
      glEnable(GL_LIGHT0);
      // We have now enabled lighting, so this object is lit.
      glCallList(sphereList); // Draw a sphere using the display list
    glPopMatrix(); // Revert to initial transform
    // Disable lighting again, to prepare for next frame.
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
}


/*
 * renderScene() - a wrapper to drawScene() to switch shaders on and off
 */
void renderScene( void )
{
  static float t;
  if (animateObject) t = (float)glfwGetTime(); // Get elapsed time
	if(GL_TRUE)
	{
  	// Use vertex and fragment shaders.
 		glUseProgramObjectARB( programObj );
	  // Update the uniform time variable.
    if(( updateTime ) && ( location_time != -1 ))
    		glUniform1fARB( location_time, (float)glfwGetTime() );
	  // Identify the textures to use.
 	  if( location_permTexture != -1 )
  		glUniform1iARB( location_permTexture, 0 ); // Texture unit 0
 	  if( location_simplexTexture != -1 )
  		glUniform1iARB( location_simplexTexture, 1 ); // Texture unit 1
 	  if( location_gradTexture != -1 )
  		glUniform1iARB( location_gradTexture, 2 ); // Texture unit 2
 		// Render with the shaders active.
 		drawScene(t);
 		// Deactivate the shaders.
    glUseProgramObjectARB(0);
	}
	else
	{
		// Render the normal way.
		drawScene(t);
	}
}


/*
 * main(argc, argv) - the standard C entry point for the program
 */
int main(int argc, char *argv[]) {

    int running = GL_TRUE; // Main loop exits when this is set to GL_FALSE
    
    // Initialise GLFW
    glfwInit();

    // Open the OpenGL window
    if( !glfwOpenWindow(640, 480, 8,8,8,8, 32,0, GLFW_WINDOW) )
    {
        glfwTerminate(); // glfwOpenWindow failed, quit the program.
        return 1;
    }
    
    // Load the extensions for GLSL - note that this has to be done
    // *after* the window has been opened, or we won't have a GL context
    // to query for those extensions and connect to instances of them.
    loadExtensions();

    // Create the two shaders
    createShaders();

    // Enable back face culling and Z buffering
    glEnable(GL_CULL_FACE); // Cull away all back facing polygons
    glEnable(GL_DEPTH_TEST); // Use the Z buffer

    // Use a dark blue color with A=1 for the background color
    glClearColor(0.0f, 0.1f, 0.3f, 1.0f);

    glEnable(GL_TEXTURE_1D); // Enable 1D texturing
    glEnable(GL_TEXTURE_2D); // Enable 2D texturing

    // Create and load three textures (do not read them from a file)
    initPermTexture(&permTextureID);
    initSimplexTexture(&simplexTextureID);
    initGradTexture(&gradTextureID);
    
    glfwSwapInterval(0); // Do not wait for screen refresh between frames

    // Compile a display list for the teapot, to render it more quickly
    initSphereList(&sphereList, 1.0);
    
    // Main loop
    while(running)
    {
        // Calculate and update the frames per second (FPS) display
        showFPS();

        // Clear the color buffer and the depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
        // Set up the camera projection.
        setupCamera();
        
        // Draw the scene.
        renderScene();

        // Swap buffers, i.e. display the image and prepare for next frame.
        glfwSwapBuffers();

        // Decide whether to update the shader "time" variable or not
        if(glfwGetKey('A')) updateTime = GL_TRUE;
        if(glfwGetKey('S')) updateTime = GL_FALSE;
        // Decide whether to animate the rotation for the scene or not
        if(glfwGetKey('Z')) animateObject = GL_TRUE;
        if(glfwGetKey('X')) animateObject = GL_FALSE;

        // Check if the ESC key was pressed or the window was closed.
        if(glfwGetKey(GLFW_KEY_ESC) || !glfwGetWindowParam(GLFW_OPENED))
          running = GL_FALSE;
    }

    // Close the OpenGL window and terminate GLFW.
    glfwTerminate();

    return 0;
}

