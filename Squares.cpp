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

//----------------------------------------------------------------------------
// Cone

const int numConeDivisions = 32;
const int numConeVertices = numConeDivisions * 6;

point4 conePoints [numConeVertices];
point3 coneNormals[numConeVertices];

point2 circlePoints[numConeDivisions];

void makeCircle(point2* dest, int numDivisions)
{
    for (int i = 0; i < numDivisions; i++)
    {
        float a = i * 2.0f * M_PI / numDivisions;
        dest[i] = point2(cosf(a), sinf(a));
    }
}

void makeConeWall(point4* destp, point3* destn, int numDivisions, float z1, float z2, int& Index, int dir)
{
    for (int i = 0; i < numDivisions; i++)
    {
        point3 p1(circlePoints[i].x, circlePoints[i].y, z1);
        point3 p2(0.0f, 0.0f, z2);
        point3 p3(circlePoints[(i+1)%numDivisions].x, circlePoints[(i+1)%numDivisions].y, z1);
        if (dir == -1)
        {
            point3 temp = p1;
            p1 = p3;
            p3 = temp;
        }
        point3 n = cross(p1-p2, p3-p2);
        destp[Index] = p1; destn[Index] = n; Index++;
        destp[Index] = p2; destn[Index] = n; Index++;
        destp[Index] = p3; destn[Index] = n; Index++;
    }    
}

//----------------------------------------------------------------------------
// Cylinder

const int numCylDivisions = 32;
const int numCylVertices = numCylDivisions * 12;

point4 cylPoints [numCylVertices];
point3 cylNormals[numCylVertices];
point3 cylLocations[numCylVertices];

void generateCylinder(GLuint program, ShapeData* cylData, vec3 loc, vec3 cylColor)
{
    makeCircle(circlePoints, numCylDivisions);
    int Index = 0;
    makeConeWall(cylPoints, cylNormals, numCylDivisions, 0.5f, 0.5f, Index, 1);
    makeConeWall(cylPoints, cylNormals, numCylDivisions, -0.5f, -0.5f, Index, -1);
    
    for (int i = 0; i < numCylDivisions; i++)
    {
        int i2 = (i+1)%numCylDivisions;
        point3 p1(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        point3 p2(circlePoints[i2].x, circlePoints[i2].y, 1.0f);
        point3 p3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        //point3 n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
        p1 = point3(circlePoints[i2].x, circlePoints[i2].y, -1.0f);
        p2 = point3(circlePoints[i].x,  circlePoints[i].y,  1.0f);
        p3 = point3(circlePoints[i].x,  circlePoints[i].y,  -1.0f);
        //n = cross(p3-p2, p1-p2);
        cylPoints[Index] = p1; cylNormals[Index] = point3(p1.x, p1.y, 0.0f); Index++;
        cylPoints[Index] = p2; cylNormals[Index] = point3(p2.x, p2.y, 0.0f); Index++;
        cylPoints[Index] = p3; cylNormals[Index] = point3(p3.x, p3.y, 0.0f); Index++;
    }
    
    cylData->numVertices = numCylVertices;
    cylData->location = loc;
	cylData->color = cylColor;

    // Create a vertex array object
    glGenVertexArrays( 1, &cylData->vao );
    glBindVertexArray( cylData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)cylPoints,  sizeof(cylPoints),
                    (float*)cylNormals, sizeof(cylNormals),
                    0, 0);
}

void duplicateCylinder(GLuint program, ShapeData* cylData, vec3 loc, vec3 cylColor)
{
	cylData->numVertices = numCylVertices;
    cylData->location = loc;
	cylData->color = cylColor;

    // Create a vertex array object
    glGenVertexArrays( 1, &cylData->vao );
    glBindVertexArray( cylData->vao );
    
    // Set vertex attributes
    setVertexAttrib(program,
                    (float*)cylPoints,  sizeof(cylPoints),
                    (float*)cylNormals, sizeof(cylNormals),
                    0, 0);
}