#ifndef SPHERE_H_
#define SPHERE_H_

typedef struct strSphere* Sphere;

typedef float vec3[3];

Sphere sphere_create(float, int, int, vec3);

void sphere_bind(Sphere, GLuint,  GLuint, GLuint);

void sphere_draw(Sphere);

void print_sphere_positions(Sphere sphere);

void print_sphere_indices(Sphere sphere);

void print_sphere_colors(Sphere sphere);

#endif
