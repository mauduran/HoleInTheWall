#ifndef CYLINDER_H_
#define CYLINDER_H_

typedef struct strCylinder* Cylinder;
#include <GL/glew.h>

typedef float vec3[3];

Cylinder cylinder_create(float length, float bottomRadius, float topRadius, int slices, int stacks, vec3 bottomColor, vec3 topColor);

Cylinder cylinder_create(float length, float bottomRadius, float topRadius, int slices, int stacks, vec3 bottomColor, vec3 topColor);

void cylinder_bind(Cylinder cylinder, GLuint vertexPosLoc, GLuint vertexColorLoc, GLuint vertexNormalLoc);

void cylinder_draw(Cylinder cylinder);

void cylinder_drawLines(Cylinder cylinder);

void print_cylinder_positions(Cylinder cylinder);

void print_cylinder_colors(Cylinder cylinder);

void print_cylinder_indices(Cylinder cylinder);

void print_cylinder_normals(Cylinder cylinder);


#endif
