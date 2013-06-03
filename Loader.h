#ifndef LOADER_H
#define LOADER_H
#include <vector>
#include "angel.h"
bool loadOBJ(
    const char * path,
    std::vector < vec3 > & out_vertices,
    std::vector < vec2 > & out_uvs,
    std::vector < vec3 > & out_normals
);
#endif