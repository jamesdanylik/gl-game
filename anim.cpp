////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef _WIN32
#include <windows.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Squares.h"
#include "tga.h"

#include "Angel/Angel.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif
#include "Loader.h"
FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

//constants I added
const int NO_OBJECT = 999;
float distance = -15.0;
float DTIME = 0.0;
float TIME_LAST = 0.0;
int Rotate = 0; //turn off and on rotation
float theta1 = 0.0; //degree of rotation of left cube
float theta2 = 0.0; //degree of rotation of right cube
GLint texloc;
GLuint g_textureID = 0;
GLuint g_normalMapID = 1;
int currentObject = NO_OBJECT;


int Animate = 0 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//texture

GLuint texture_cube;
GLuint texture_earth;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
const int numSquares = 20;
ShapeData squareData[numSquares];
vec3 squareLocations[numSquares] = {
	vec3(-5.0, 0.0, -3.0),
	vec3(-3.0, 0.0, -3.0),
	vec3(-1.0, 0.0, -3.0),
	vec3(1.0, 0.0, -3.0),
	vec3(3.0, 0.0, -3.0),
	vec3(5.0, 0.0, -3.0), //row 1
	vec3(-5.0, 0.0, -1.0),
	vec3(-1.0, 0.0, -1.0),
	vec3(1.0, 0.0, -1.0),
	vec3(5.0, 0.0, -1.0), //row 2
	vec3(-5.0, 0.0, 1.0),
	vec3(-1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(5.0, 0.0, 1.0), //row 3
	vec3(-5.0, 0.0, 3.0),
	vec3(-3.0, 0.0, 3.0),
	vec3(-1.0, 0.0, 3.0),
	vec3(1.0, 0.0, 3.0),
	vec3(3.0, 0.0, 3.0),
	vec3(5.0, 0.0, 3.0), //row 4
};

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view, view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uBump, uEnableTex, uEnablePicking, uPickColor;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye(0, 0.0, 50.0,1.0);
Angel::vec4 ref(0.0, 0.0, 0.0,1.0);
Angel::vec4 up(0.0,1.0,0.0,0.0);

// Used to modify camera location and orientation
GLfloat camX = 0;
GLfloat camY = 0;
GLfloat camZ = -15;
GLfloat LRangle = 0;
GLfloat UDangle = -30;
GLfloat fieldofView = 50;

double TIME = 0.0 ;

void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
			distance = -15.0;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
		case 'i':
			if(camZ < -0.5)
				camZ += 0.5;
			break;
		case 'o':
			camZ -= 0.5;
			break;
		case 't':
			if(Rotate == 0)
				Rotate = 1;
			else
				Rotate = 0;
			break;
    }
    glutPostRedisplay() ;

}

//////////////////////////////////////////////////////
//    PROC: rotateY()
//    DOES: this function returns a matrix for a rotataion
//    about the y axis of theta degrees
// 
//////////////////////////////////////////////////////
mat4 rotateY(float theta)
{
	float psi = theta*DegreesToRadians;
	return mat4(cos(psi), 0, sin(psi), 0,
				0, 1, 0, 0,
				-sin(psi), 0, cos(psi), 0,
				0, 0, 0, 1);
}

//////////////////////////////////////////////////////
//    PROC: rotateX()
//    DOES: this function returns a matrix for a rotataion
//    about the x axis of theta degrees
// 
//////////////////////////////////////////////////////
mat4 rotateX(float theta)
{
	float psi = theta*DegreesToRadians;
	return mat4(1, 0, 0, 0,
				0, cos(psi), -sin(psi), 0,
				0, sin(psi), cos(psi), 0,
				0, 0, 0, 1);
}

// viewMat creates a view matrix based on location values

mat4 camTransMat (GLfloat eyex, GLfloat eyey, GLfloat eyez)
{
	return mat4(1, 0, 0, eyex,
				0, 1, 0, eyey,
				0, 0, 1, eyez,
				0, 0, 0, 1);
}

mat4 camRotMat (GLfloat leftright, GLfloat updown)
{
	mat4 leftrightMat = RotateY(leftright);
	mat4 updownMat = RotateX(updown);
	return leftrightMat * updownMat;
}

mat4 viewMat (GLfloat x, GLfloat y, GLfloat z, GLfloat LRangle, GLfloat UDangle)
{
	return camTransMat(x, y, z)*camRotMat(LRangle, UDangle); 
}

//Color Picking Utility Functions

vec4 getColorByIndex(int index)
{
	int r = index&0xFF;
	int g = (index>>8)&0xFF;
	int b = (index>>16)&0xFF;

	return vec4(float(r)/255.0f, float(g)/255.0f, float(b)/255.0f, 1.0f);
}

int getIndexByColor(int r, int g, int b)
{
	return (r) | (g<<8) | (b<<16);
}

// Square Drawing function
//////////////////////////////////////////////////////
//    PROC: drawSquare()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
//////////////////////////////////////////////////////

void drawSquare(ShapeData square)
{
	/*glUseProgramObjectARB( program );
	int texture2ID;
	texture2ID = glGetUniformLocationARB(program, "NormalMap");
	glUniform1i(texture2ID, 1);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_normalMapID);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_textureID);*/

	set_colour(square.color[0], square.color[1], square.color[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, g_textureID );
	glActiveTexture(GL_TEXTURE1);
	glBindTexture( GL_TEXTURE_2D, g_normalMapID );
	glUniform1i( uEnableTex, 1 );
	//glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	model_view = view;
	model_view *= Translate(square.location[0], square.location[1], square.location[2]);
	model_view *= Scale(2.0f, 2.0f, 2.0f);
	model_view *= rotateY(theta1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
	glBindVertexArray( square.vao );
    glDrawArrays( GL_TRIANGLES, 0, square.numVertices );
	glUniform1i( uEnableTex, 0 );
}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
	glViewport(0, 0, Width, Height);

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes
    generateSquare(program, &squareData[0], squareLocations[0]);
	for(int i = 1; i < numSquares; i++)
		duplicateSquare(program, &squareData[i], squareLocations[i]);
	//duplicateSquare(program, &squareData[2], vec3(-1.0, 0.0, 1.0));
	//duplicateSquare(program, &squareData[3], vec3(1.0, 0.0, 1.0));

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
	uBump      = glGetUniformLocation( program, "Bump"			  );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );
	uEnablePicking = glGetUniformLocation( program, "Picking" );
	uPickColor = glGetUniformLocation( program, "PickingColor" );

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);

    glEnable(GL_DEPTH_TEST);

	TgaImage textureImage;
    if (!textureImage.loadTGA("stone_wall.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }

	TgaImage normalImage;
    if (!normalImage.loadTGA("stone_wall_normal_map.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }

	//NORMAL TEXTURE MAP
	glActiveTexture(GL_TEXTURE0);
    glGenTextures( 1, &g_textureID );
    glBindTexture( GL_TEXTURE_2D, g_textureID );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage.width, textureImage.height, 0,
                 (textureImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, textureImage.data );

    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
	//NORMAL BUMP MAP
    glActiveTexture(GL_TEXTURE1);
    glGenTextures( 1, &g_normalMapID );
    glBindTexture( GL_TEXTURE_2D, g_normalMapID );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, normalImage.width, normalImage.height, 0,
                 (normalImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, normalImage.data );
    
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    //glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    // Set texture sampler variable to texture unit 0
    // (set in glActiveTexture(GL_TEXTURE0))
    
    glUniform1i( uTex, 0);
    glUniform1i( uBump, 1);

    Arcball = new BallData ;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
    model_view *= Translate(camX, camY, camZ);
	model_view *= rotateY(LRangle);
	model_view *= rotateX(UDangle);

    /*model_view *= Translate(0.0f, -3.0f, distance);
    HMatrix r;
    Ball_Value(Arcball,r);

    mat4 mat_arcball_rot(
        r[0][0], r[0][1], r[0][2], r[0][3],
        r[1][0], r[1][1], r[1][2], r[1][3],
        r[2][0], r[2][1], r[2][2], r[2][3],
        r[3][0], r[3][1], r[3][2], r[3][3]);
    model_view *= mat_arcball_rot;*/
    
    view = model_view;
    
    
    //model_view = Angel::LookAt(eye, ref, up);//just the view matrix;

    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

    // Previously glScalef(Zoom, Zoom, Zoom);
    model_view *= Scale(Zoom);
	view = model_view;

    // Draw Cubes
	for(int i = 0; i < numSquares; i++)
	{
		//model_view = view;
		//model_view *= Scale(3.0f, 3.0f, 3.0f);
		//model_view *= Translate(squareData[i].location[0], squareData[i].location[1], squareData[i].location[2]);
		//model_view *= rotateY(theta1);
		drawSquare(squareData[i]);
	}

    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height) ;
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(uEnablePicking, 1);

		vec4 pickingColor;
		for(int i = 0; i < numSquares; i++)
		{
			pickingColor = getColorByIndex(i);
			glUniform4fv(uPickColor, 1, pickingColor);
			drawSquare(squareData[i]);
		}

		glUniform1i(uEnablePicking, 0);

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned char data[4];
		glReadPixels(x, Height-9-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		int pickedID = getIndexByColor(data[0], data[1], data[2]);

		if(pickedID < numSquares || pickedID == 0x00FFFFFF)
		{
			if(pickedID != 0x00FFFFFF && pickedID != currentObject && currentObject != NO_OBJECT)
			{
				squareData[currentObject].color = vec3(1.0, 1.0, 1.0);
				squareData[pickedID].color = vec3(0.0, 0.0, 0.0);
				currentObject = pickedID;
			}
			else if(pickedID != 0x00FFFFFF && pickedID != currentObject && currentObject == NO_OBJECT)
			{
				squareData[pickedID].color = vec3(0.0, 0.0, 0.0);
				currentObject = pickedID;
			}
			else if(pickedID != 0x00FFFFFF && pickedID == currentObject)
			{
				squareData[pickedID].color = vec3(1.0, 1.0, 1.0);
				currentObject = NO_OBJECT;
			}
			else if(pickedID == 0x00FFFFFF && currentObject != NO_OBJECT)
			{
				squareData[currentObject].color = vec3(1.0, 1.0, 1.0);
				currentObject = NO_OBJECT;
			}
		}
		glutPostRedisplay();
        Button = -1;
	}
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.
        
        eye.x = 20*sin(TIME);
        eye.z = 20*cos(TIME);
        
        printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ; 
    }
	else if( Rotate == 1 )
	{
		TIME = TM.GetElapsedTime();

		DTIME = TIME-TIME_LAST;
		TIME_LAST = TIME;

		theta1 += DTIME*360;
		if(theta1 > 360)
			theta1 -= 360;
		
		theta2 += DTIME*180;
		if(theta2 > 360)
			theta2 -= 360;

		glutPostRedisplay() ;
	}
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
	
std::vector< vec3 > vertices;
std::vector< vec2 > uvs;
std::vector< vec3 > normals; // Won't be used at the moment.
bool res = loadOBJ("CHARMODSTEX/YoungRanger_obj.obj", vertices, uvs, normals);
//end of loader test


    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




