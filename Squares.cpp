#include "angel/angel.h"
#include "Squares.h"
#include <cmath>

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;
typedef Angel::vec3 point3;
typedef Angel::vec2 point2;

void setVertexAttrib(GLuint program, 
                     GLfloat* points,    GLsizeiptr psize, 
                     GLfloat* normals,   GLsizeiptr nsize,
                     GLfloat* texcoords, GLsizeiptr tsize)
{
    GLuint buffer[2];
    glGenBuffers( 2, buffer );

    glBindBuffer( GL_ARRAY_BUFFER, buffer[0] );
    glBufferData( GL_ARRAY_BUFFER, psize, points, GL_STATIC_DRAW );
    GLuint vPosition = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( vPosition );
    glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    glBindBuffer( GL_ARRAY_BUFFER, buffer[1] );
    glBufferData( GL_ARRAY_BUFFER, nsize, normals, GL_STATIC_DRAW );
    GLuint vNormal = glGetAttribLocation( program, "vNormal" );
    glEnableVertexAttribArray( vNormal );
    glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

    if (texcoords)
    {
        GLuint tbuffer;
        glGenBuffers( 1, &tbuffer );
        glBindBuffer( GL_ARRAY_BUFFER, tbuffer );
        glBufferData( GL_ARRAY_BUFFER, tsize, texcoords, GL_STATIC_DRAW );
        GLuint vTexCoords = glGetAttribLocation( program, "vTexCoords" );
        glEnableVertexAttribArray( vTexCoords );
        glVertexAttribPointer( vTexCoords, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );
    }
    
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray(0);
}

//Square

const int numSquareVertices = 6;
const int maxNumSquares = 1000;

point4 squarePoints[numSquareVertices];
point3 squareNormals[numSquareVertices];
point2 squareUV[numSquareVertices];
point3 squareLocation[maxNumSquares];

//vertices of a square centered at origin in x-z plane
point4 vertices[4] = {
	point4(-0.5, 0.0, -0.5, 1.0), //topleft
	point4(0.5, 0.0, -0.5, 1.0),  //topright
	point4(-0.5, 0.0, 0.5, 1.0),  //bottomleft
	point4(0.5, 0.0, 0.5, 1.0)    //bottomright
};

int Index = 0;
int locIndex = 0;
void square( int a, int b, int c, int d, const point3& normal)
{
	if(Index == numSquareVertices)
		Index = 0;
    squarePoints[Index] = vertices[a]; squareNormals[Index] = normal; 
    squareUV[Index] = point2(0.0f, 1.0f); Index++;
    squarePoints[Index] = vertices[b]; squareNormals[Index] = normal;
    squareUV[Index] = point2(0.0f, 0.0f); Index++;
    squarePoints[Index] = vertices[c]; squareNormals[Index] = normal;
    squareUV[Index] = point2(1.0f, 0.0f); Index++;
    squarePoints[Index] = vertices[a]; squareNormals[Index] = normal;
    squareUV[Index] = point2(0.0f, 1.0f); Index++;
    squarePoints[Index] = vertices[c]; squareNormals[Index] = normal;
    squareUV[Index] = point2(1.0f, 0.0f); Index++;
    squarePoints[Index] = vertices[d]; squareNormals[Index] = normal;
    squareUV[Index] = point2(1.0f, 1.0f); Index++;
}

void generateSquare(GLuint program, ShapeData* squareData, vec3 loc)
{
	square(0, 2, 3, 1, point3(0.0, 1.0, 0.0));
	squareData->numVertices = numSquareVertices;
	squareData->location = loc;
	squareData->color = vec3(1.0);

    // Create a vertex array object
    glGenVertexArrays( 1, &squareData->vao );
    glBindVertexArray( squareData->vao );

    // Set vertex attributes
    setVertexAttrib(program, 
        (float*)squarePoints,  sizeof(squarePoints), 
        (float*)squareNormals, sizeof(squareNormals),
        (float*)squareUV,      sizeof(squareUV));
}

void duplicateSquare(GLuint program, ShapeData* squareData, vec3 loc)
{
	squareData->numVertices = numSquareVertices;
	squareData->location = loc;
	squareData->color = vec3(1.0);

	//Create a vertex array object
	glGenVertexArrays( 1, &squareData->vao );
	glBindVertexArray( squareData->vao );

    // Set vertex attributes
    setVertexAttrib(program, 
        (float*)squarePoints,  sizeof(squarePoints), 
        (float*)squareNormals, sizeof(squareNormals),
        (float*)squareUV,      sizeof(squareUV));
}