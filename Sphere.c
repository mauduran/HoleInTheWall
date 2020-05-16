#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <GL/glew.h>

#include "Utils.h"
#include "Sphere.h"

struct strSphere{
	float radius;
	int parallel, meridian;
//	vec3 sphereColor;

	float * positions;
	float * colors;
//	float * normals;
	GLushort * indices;

	int vertexNum;

	GLuint sphereBuffers[4];
	GLuint sphereVA;
//	GLuint programId;
};


Sphere sphere_create(float radius, int parallel, int meridian, vec3 sphereColor){
	Sphere sphere = (Sphere) malloc(sizeof(struct strSphere));
	meridian++;
	sphere->parallel = parallel;
	sphere->meridian = meridian;

	//Definir tamaños de los arreglos

	//x meridianos,  y paralelos
	//x*y*3

	int vertexNum = (2*meridian*parallel);

	sphere->positions = (float *) malloc(sizeof(float)*vertexNum*3);
	sphere->colors = (float *) malloc(sizeof(float)*vertexNum*3);
//	sphere->normals = (float *) malloc(sizeof(float)*vertexNum*3);
	sphere->indices = (GLushort *) malloc(sizeof(GLushort)*(vertexNum + parallel));

	sphere->vertexNum = vertexNum;

	//Llenado de arreglos

	float currentTheta = 0;
	float incTheta = 2*M_PI/(meridian-1);

	float currentPhi = 0;
	float incPhi = M_PI/(parallel-1);


	int count = 0;
	int indexCount = 0;
	int indexValueCount = 0;

	for (int i = 0; i < parallel; i++) {
		float r = sphereColor[0]*0.8 + 0.2*rand()/RAND_MAX;
		float g = sphereColor[1]*0.8 + 0.2*rand()/RAND_MAX;
		float b = sphereColor[2]*0.8 + 0.2*rand()/RAND_MAX;
		currentTheta = 0;
		//calculo de paralelo
		for(int j=0; j<meridian;j++){
			//upper layer
			sphere->positions[count] = radius*sin(currentPhi)*cos(currentTheta);
			sphere->colors[count++]= r;
//			sphere->normals[count++] = sin(currentPhi)*cos(currentTheta);

			sphere->positions[count] = radius*sin(currentPhi)*sin(currentTheta);
			sphere->colors[count++]= g;
//			sphere->normals[count++] = sin(currentPhi)*sin(currentTheta);

			sphere->positions[count] = radius*cos(currentPhi);
			sphere->colors[count++]= b;
//			sphere->normals[count++] = cos(currentPhi);


			//lower layer
			sphere->positions[count] = radius*sin(currentPhi+incPhi)*cos(currentTheta);
			sphere->colors[count++]= r;
//			sphere->normals[count++] = sin(currentPhi+incPhi)*cos(currentTheta);

			sphere->positions[count] = radius*sin(currentPhi+incPhi)*sin(currentTheta);
			sphere->colors[count++]= g;
//			sphere->normals[count++] = sin(currentPhi+incPhi)*sin(currentTheta);

			sphere->positions[count] = radius*cos(currentPhi+incPhi);
			sphere->colors[count++]= b;
//			sphere->normals[count++] = cos(currentPhi+incPhi);

			sphere->indices[indexCount++] = indexValueCount++;
			sphere->indices[indexCount++] = indexValueCount++;
			currentTheta+= incTheta;
		}
		sphere->indices[indexCount++] = 0xFFFF;
		currentPhi += incPhi;
	}

	return sphere;
}

void sphere_bind(Sphere sphere, GLuint vertexPosLoc, GLuint vertexColorLoc, GLuint vertexNormalLoc){
//	sphere->programId = glCreateProgram();

	GLuint sphereVA;
	glGenVertexArrays(1, &sphereVA);
	sphere->sphereVA =sphereVA;

	GLuint sphereBuffers[4];
	glGenBuffers(4, sphereBuffers);

	glBindVertexArray(sphereVA);
	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*sphere->vertexNum,
					sphere->positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPosLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPosLoc);

	//Colors
	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*sphere->vertexNum,
					sphere->colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexColorLoc);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, sphereBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*sphere->vertexNum,
					sphere->positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	//Indices
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereBuffers[3]);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*(sphere->vertexNum+sphere->parallel),
	    		sphere->indices, GL_STATIC_DRAW);
	    glPrimitiveRestartIndex(0xFFFF);
	    glEnable(GL_PRIMITIVE_RESTART);



	sphere->sphereBuffers[0] = sphereBuffers[0];
	sphere->sphereBuffers[1] = sphereBuffers[1];
	sphere->sphereBuffers[2] = sphereBuffers[2];
	sphere->sphereBuffers[3] = sphereBuffers[3];
}

void sphere_draw(Sphere sphere){
//		glUseProgram(sphere->programId);
	    glBindVertexArray(sphere->sphereVA);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->sphereBuffers[3]);
	    glDrawElements(GL_TRIANGLE_STRIP, sphere->vertexNum+sphere->parallel,GL_UNSIGNED_SHORT,0);

}

void print_sphere_positions(Sphere sphere){
	for(int i=0; i<sphere->vertexNum; i++){
		printf("%.3f, %.3f, %.3f\n", sphere->positions[3*i], sphere->positions[3*i+1], sphere->positions[3*i+2]);
	}
}

void print_sphere_colors(Sphere sphere){
	for(int i=0; i<sphere->vertexNum; i++){
		printf("x: %f y: %f z: %f\n", sphere->colors[3*i], sphere->colors[3*i+1], sphere->colors[3*i+2]);
	}
}

void print_sphere_indices(Sphere sphere){
	for(int i=0; i<sphere->vertexNum+sphere->parallel; i++){
		printf("i: %d\n", sphere->indices[i]);
	}
}
