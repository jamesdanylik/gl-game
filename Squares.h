#ifndef _SQUARES_H_
#define _SQUARES_H_

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

#include "Angel/Angel.h"

struct ShapeData
{
    GLuint vao;
    int numVertices;
	vec3 location;
	vec3 color;
};

void generateSquare(GLuint program, ShapeData* squareData, vec3 loc);
void duplicateSquare(GLuint program, ShapeData* squareData, vec3 loc);

void generateCylinder(GLuint program, ShapeData* cylData, vec3 loc, vec3 cylColor);
void duplicateCylinder(GLuint program, ShapeData* cylData, vec3 loc, vec3 cylColor);

#endif