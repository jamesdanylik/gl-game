// Colors are assigned to each vertex and then the rasterizer interpolates
//   those colors across the triangles.  We us an orthographic projection
//   as the default projetion.

#include "Squares.h"
#include "Angel.h"
#include <vector>

typedef Angel::vec4  color4;
typedef Angel::vec4  point4;

GLint program;
GLint transMat, finishMat, scaleloc, thetaloc, colorloc, 
	ambientloc, diffuseloc, specularloc, shininessloc, 
	viewloc, projloc, pickingloc;
int Button = -1;

int Width = 500;
int Height = 500;

GLfloat camX = 0;
GLfloat camY = -10;
GLfloat camZ = -10;
GLfloat LRangle = 0;
GLfloat UDangle = -30;
GLfloat fieldofView = 50; // for setting field of view

color4 lightAmbient = vec4(0.2, 0.2, 0.2, 1.0);
color4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
color4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);
point4 lightPosition = vec4(0.0,0.0, 0.0,1.0);

const int numSquares = 4;
ShapeData squareData[numSquares];

typedef struct materialStruct {
	color4 ambient;
	color4 diffuse;
	color4 specular;
	float shininess;
} materialStruct; //struct for material properties

// Utility functions

vec3 pointToVector(point4 a)
{
	return vec3(a[0],a[1],a[2]);
}

vec4 vecProduct(vec4 a, vec4 b)
{
	return vec4(a[0]*b[0],a[1]*b[1],a[2]*b[2],a[3]*b[3]);
}

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

//----------------------------------------------------------------------------

// translate creates a translation matrix based on input x, y, z

mat4 translateMat (GLfloat x, GLfloat y, GLfloat z)
{
	return mat4(1, 0, 0, x,
				0, 1, 0, y,
				0, 0, 1, z,
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
	return  camTransMat(x, y, z) * camRotMat(LRangle, UDangle); 
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
    glUniform4f(ambientloc,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(diffuseloc,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(specularloc, specular*r, specular*g, specular*b, 1.0f);
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
	set_colour(square.color[0], square.color[1], square.color[2]);
	mat4 translationMat = translateMat(0,0,0);
    glUniformMatrix4fv( transMat, 1, GL_TRUE, translationMat);
	glUniformMatrix4fv(finishMat, 1, GL_FALSE, translateMat(square.location[0],square.location[1],square.location[2]));
	glUniform1f(scaleloc, 2);
	glUniform1f(thetaloc, 0);
	glUniform1f(shininessloc, 5);
    glBindVertexArray( square.vao );
    glDrawArrays( GL_TRIANGLES, 0, square.numVertices );
}

// Sphere drawing functions

/*void triangle(point4 a, point4 b, point4 c, bool smooth, bool flipNorms)
{
	vec3 normal;
	if(flipNorms)
	{
		normal = -normalize(cross(b-a, c-b));
		normals[dataindex] = normal;
		points[dataindex] = a;
		dataindex++;
		normals[dataindex] = normal;
		points[dataindex] = b;
		dataindex++;
		normals[dataindex] = normal;
		points[dataindex] = c;
		dataindex++;
	}
	else if(smooth)
	{
		normal = pointToVector(a);
		normals[dataindex] = normal;
		points[dataindex] = a;
		dataindex++;
		normal = pointToVector(b);
		normals[dataindex] = normal;
		points[dataindex] = b;
		dataindex++;
		normal = pointToVector(c);
		normals[dataindex] = normal;
		points[dataindex] = c;
		dataindex++;
	}
	else
	{
		normal = normalize(cross(b-a, c-b));
		normals[dataindex] = normal;
		points[dataindex] = a;
		dataindex++;
		normals[dataindex] = normal;
		points[dataindex] = b;
		dataindex++;
		normals[dataindex] = normal;
		points[dataindex] = c;
		dataindex++;
	}
}

point4 unit(const point4 &p)
{
    float len = p.x*p.x + p.y*p.y + p.z*p.z;
    
    point4 t;
    if ( len > DivideByZeroTolerance ) {
	t = p / sqrt(len);
	t.w = 1.0;
    }

    return t;
}

void divide_triangle(point4 a, point4 b, point4 c, GLint n, bool smooth, bool flipNorms)
{
	point4 v1, v2, v3;
	if(n > 0)
	{
		v1 = unit(a+b);
		v2 = unit(a+c);
		v3 = unit(b+c);
		divide_triangle(a, v2, v1, n-1, smooth, flipNorms);
		divide_triangle(c, v3, v2, n-1, smooth, flipNorms);
		divide_triangle(b, v1, v3, n-1, smooth, flipNorms);
		divide_triangle(v1, v2, v3, n-1, smooth, flipNorms);
	}
	else triangle(a, b, c, smooth, flipNorms);
}*/

// OpenGL initialization
void
init()
{
	glViewport(0, 0, Width, Height);

    // Load shaders and use the resulting shader program
    program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    // set up vertex arrays for squares
	generateSquare(program, &squareData[0], vec3(-1.0, 0.0, -1.0));
	duplicateSquare(program, &squareData[1], vec3(1.0, 0.0, -1.0));
	duplicateSquare(program, &squareData[2], vec3(-1.0, 0.0, 1.0));
	duplicateSquare(program, &squareData[3], vec3(1.0, 0.0, 1.0));

	// get locations of shader variables and init the ones that are constant
	GLuint vlightPosition = glGetUniformLocation( program, "lightPosition" );
	glUniform4fv( vlightPosition, 1, lightPosition);

	transMat = glGetUniformLocation( program, "transMat");
	viewloc = glGetUniformLocation( program, "viewMat");
	projloc = glGetUniformLocation(program, "projMat");
	finishMat = glGetUniformLocation( program, "finishMat");
	scaleloc = glGetUniformLocation( program, "scaleAmt");
	thetaloc = glGetUniformLocation( program, "theta");
	colorloc = glGetUniformLocation( program, "pickingColor");
	ambientloc = glGetUniformLocation( program, "ambientProduct");
	diffuseloc = glGetUniformLocation( program, "diffuseProduct");
	specularloc = glGetUniformLocation( program, "specularProduct");
	shininessloc = glGetUniformLocation( program, "shininess" );
	pickingloc = glGetUniformLocation( program, "picking" );

    glEnable( GL_DEPTH_TEST );
    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // black background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// set up camera and perspective matrices

	mat4 viewMatrix = viewMat(camX, camY, camZ, LRangle, UDangle);
	glUniformMatrix4fv(viewloc, 1, GL_FALSE, viewMatrix);

	mat4 projMatrix = Perspective(fieldofView, float(Width)/float(Height), 1.0, 1000.0);
	glUniformMatrix4fv(projloc, 1, GL_FALSE, projMatrix);

	for(int i = 0; i < numSquares; i++)
		drawSquare(squareData[i]);

    glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch( key ) {
	case 033:  // Escape key
	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;

	case 'i': case 'I': // forward
		camX = camX + 0.25*sin(DegreesToRadians*LRangle);
		camZ = camZ + 0.25*cos(DegreesToRadians*LRangle);
		glutPostRedisplay();
		break;
	case 'j': case 'J': // left
		camX = camX + 0.25*cos(DegreesToRadians*LRangle);
		camZ = camZ - 0.25*sin(DegreesToRadians*LRangle);
		glutPostRedisplay();
		break;
	case 'k': case 'K': // right
		camX = camX - 0.25*cos(DegreesToRadians*LRangle);
		camZ = camZ + 0.25*sin(DegreesToRadians*LRangle);
		glutPostRedisplay();
		break;
	case 'm': case 'M': // backward
		camX = camX - 0.25*sin(DegreesToRadians*LRangle);
		camZ = camZ - 0.25*cos(DegreesToRadians*LRangle);
		glutPostRedisplay();
		break;

	//KEY FOR RESET-----
	case '1':
		camX = 0;
		camY = -40;
		camZ = -110;
		UDangle = -30;
		LRangle = 0;
		fieldofView = 100;
		glutPostRedisplay();
		break;
	//------------------

	case 'n': case 'N': //narrow, max at 10
		if(fieldofView > 10)
		{
			fieldofView--;
			glutPostRedisplay();
		}
		break;
	case 'w': case 'W': //widen, max at 150
		if(fieldofView < 150)
		{
			fieldofView++;
			glutPostRedisplay();
		}
		break;
	}
}

//----------------------------------------------------------------------------
// arrow key function
void arrows(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_UP:
		UDangle++;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		UDangle--;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		LRangle++;
		glutPostRedisplay();
		break;
	case GLUT_KEY_RIGHT:
		LRangle--;
		glutPostRedisplay();
		break;
	}
}

void mouse(int button, int state, int x, int y)
{
	Button = button;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1i(pickingloc, 1);

		color4 pickingColor;
		for(int i = 0; i < numSquares; i++)
		{
			pickingColor = getColorByIndex(i);
			glUniform4fv(colorloc, 1, pickingColor);
			drawSquare(squareData[i]);
		}

		glUniform1i(pickingloc, 0);

		glFlush();
		glFinish();

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		unsigned char data[4];
		glReadPixels(x, Height-y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);

		int pickedID = getIndexByColor(data[0], data[1], data[2]);

		if(pickedID != 0x00FFFFFF)
		{
			for(int j = 0; j < numSquares; j++)
			{
				if(j == pickedID)
				{
					squareData[j].color = vec3(1.0, 1.0, 1.0);
					drawSquare(squareData[j]);
				}
				else
					drawSquare(squareData[j]);
			}
		}
		glutSwapBuffers();
		glutPostRedisplay();
        Button = -1;
	}
}

void reshape(int w, int h)
{
	Width = w;
	Height = h;

	glViewport(0, 0, w, h);

	mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( projloc, 1, GL_TRUE, projection );
}

//----------------------------------------------------------------------------
// idle rotation function
/*
void rotatemenow()
{
	thetaicy++; // for setting icy orbit
	thetaswamp += 0.5; // for setting swamp orbit
	thetawater += 0.33; // for setting water orbit
	thetamud -= 0.2; // for setting mud orbit
	thetamoon += 1.0; // for setting moon orbit
	glutPostRedisplay();
}
*/
//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );
	glutInitWindowPosition (0, 0);
    glutInitWindowSize( Width, Height );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Picking" );

	glewExperimental = GL_TRUE;

    glewInit();

    init();

	glutReshapeFunc( reshape );
    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
	glutSpecialFunc( arrows );
	glutMouseFunc( mouse );
	//glutIdleFunc( rotatemenow );
    glutMainLoop();
    return 0;
}
