#include "Cylinder.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <GL/glew.h>

struct strCylinder{
	float length;
	int slices;

	float * positions;
	float * colors;
	float * normals;
	GLushort * indices;

	int vertexNum;
	int indicesNum;
	GLuint cylinderBuffers[4];
	GLuint cylinderVA;
};

static void crossProduct(vec3 p1, vec3 p2, vec3 p3, vec3 res) {
	vec3 u = { p2[0]-p1[0], p2[1]-p1[1], p2[2]-p1[2]};
	vec3 v = {p3[0]-p1[0], p3[1]-p1[1], p3[2]-p1[2]};

	res[0] = u[1]*v[2] - u[2]*v[1]; //i
	res[1] =  u[2]*v[0] - u[0]*v[2];
	res[2] = u[0]*v[1] - u[1]*v[0];
}


Cylinder cylinder_create(float length, float bottomRadius, float topRadius, int slices, int stacks, vec3 bottomColor, vec3 topColor){
    Cylinder cylinder = (Cylinder) malloc(sizeof(struct strCylinder));
    cylinder->slices = slices;
    int vertexNum = (2*(slices+1)*stacks)+2;

    cylinder->indicesNum = vertexNum + 2*(slices+1) + (stacks);

    cylinder->positions = (float *) calloc(vertexNum*3, sizeof(float));
    cylinder->colors = (float *) calloc(vertexNum*3, sizeof(float));
    cylinder->normals = (float *) calloc(vertexNum*3, sizeof(float));
    cylinder->indices = (unsigned short * ) calloc((cylinder->indicesNum), sizeof(unsigned short) );

    cylinder->vertexNum = vertexNum;

    float currentTheta = 0;
    float incTheta = 2*M_PI/(slices);
    float currentRadius = topRadius;
    float incRadius = (bottomRadius-topRadius)/stacks;
    float currentY = length/2;
    float decY = (length)/stacks;

    float r = topColor[0];
    float incR = (bottomColor[0]-topColor[0])/(stacks-1);
    float g = topColor[1];
    float incG = (bottomColor[1]-topColor[1])/(stacks-1);
    float b = topColor[2];
    float incB = (bottomColor[2]-topColor[2])/(stacks-1);
	float randR = r*0.8 + 0.2*rand()/RAND_MAX;
	float randG = g*0.8 + 0.2*rand()/RAND_MAX;
	float randB = b*0.8 + 0.2*rand()/RAND_MAX;

    int count = 0;
    int indexCountTop = 0;
    int indexCount = slices+2;
    int indexCountBottom = stacks-1+slices+vertexNum ;
    int indexValueCount = 1;


    int negRadDiff = bottomRadius-topRadius;

    cylinder->positions[count] = 0;
    cylinder->colors[count]= r;
    cylinder->normals[count++] = 0;
    cylinder->positions[count] = currentY;
    cylinder->colors[count]= g;
    cylinder->normals[count++] = 1;
    cylinder->positions[count] = 0;
    cylinder->colors[count]= b;
    cylinder->normals[count++] = 0;
    cylinder->indices[indexCountTop++] = 0;

    vec3 tempPoint = {0,0,0};
    float repeatedNormalX = 0;
    float repeatedNormalY = 0;
    float repeatedNormalZ = 0;

    float x, z;

    for (int i = 0; i < stacks; i++) {
        currentTheta = 0;
        if(i==(stacks-1)){
            cylinder->positions[count] = 0;
            cylinder->colors[count]= r;
            cylinder->normals[count++] = 0;
            cylinder->positions[count] = currentY-decY;
            cylinder->colors[count]= g;
            cylinder->normals[count++] = -1;
            cylinder->positions[count] = 0;
            cylinder->colors[count]= b;
            cylinder->normals[count++] = 0;
            cylinder->indices[indexCountBottom++] = (short) indexValueCount++;
        }
        for(int j=0; j<slices+1;j++){
            //upper layer

            vec3 nextPoint = {topRadius*cos(currentTheta+incTheta), length/2, topRadius*sin(currentTheta+incTheta)};
            vec3 pointBelow = {(topRadius+incRadius)*cos(currentTheta), length/2-decY, (topRadius+incRadius)*sin(currentTheta)};
            vec3 currPoint = {topRadius*cos(currentTheta), length/2, topRadius*sin(currentTheta)};
            crossProduct(nextPoint, pointBelow, currPoint, tempPoint);

            if((negRadDiff>0 && tempPoint[1]<0) ||(negRadDiff<0 && tempPoint[1]>0)){
            	tempPoint[1] = -tempPoint[1];
            	tempPoint[0] = -tempPoint[0];
            	tempPoint[2] = -tempPoint[2];
            }

            if(j==0){
            	repeatedNormalX = tempPoint[0];
            	repeatedNormalY = tempPoint[1];
            	repeatedNormalZ = tempPoint[2];
            }

        	x = currentRadius*cos(currentTheta);

        	z = currentRadius*sin(currentTheta);

            cylinder->positions[count] = x;
//            cylinder->normals[count] = topRadius*cos(currentTheta);
            cylinder->normals[count] = (j==slices)?repeatedNormalX: tempPoint[0];
            cylinder->colors[count++]= randR;


            cylinder->positions[count] = currentY;
            cylinder->normals[count] = (j==slices)?repeatedNormalY: tempPoint[1];
            cylinder->colors[count++]= randG;

            cylinder->positions[count] = z;
            cylinder->normals[count] = topRadius*sin(currentTheta);
            cylinder->normals[count] = (j==slices)?repeatedNormalZ: tempPoint[2];
            cylinder->colors[count++]= randB;

            if(i==0){
                cylinder->indices[indexCountTop++] = (short) indexValueCount;
            }

            cylinder->indices[indexCount++] = (short)indexValueCount++;

            x = (currentRadius+incRadius)*cos(currentTheta);
            z =  (currentRadius+incRadius)*sin(currentTheta);
            cylinder->positions[count] = x;
//            cylinder->normals[count] = topRadius*cos(currentTheta);
            cylinder->normals[count] = (j==slices)?repeatedNormalX: tempPoint[0];
            cylinder->colors[count++]= randR;

            cylinder->positions[count] = currentY-decY;
            cylinder->normals[count] = (j==slices)?repeatedNormalY: tempPoint[1];
            cylinder->colors[count++]= randG;

            cylinder->positions[count] =z;
//            cylinder->normals[count] = topRadius*sin(currentTheta);
            cylinder->normals[count] = (j==slices)?repeatedNormalZ: tempPoint[2];
            cylinder->colors[count++]= randB;
            if(i==(stacks-1)){
                cylinder->indices[indexCountBottom++] = (short)indexValueCount;
            }

            cylinder->indices[indexCount++] = indexValueCount++;
            currentTheta+= incTheta;
        }
        r += incR;
        g += incG;
        b += incB;
        randR = r*0.8 + 0.2*rand()/RAND_MAX;
        randG = g*0.8 + 0.2*rand()/RAND_MAX;
        randB = b*0.8 + 0.2*rand()/RAND_MAX;
        currentY -= decY;
        currentRadius += incRadius;
        cylinder->indices[indexCount++] = (short)0xFFFF;
    }



    return cylinder;
}


void cylinder_bind(Cylinder cylinder, GLuint vertexPosLoc, GLuint vertexColorLoc, GLuint vertexNormalLoc){
	GLuint cylinderVA;
	glGenVertexArrays(1, &cylinderVA);
	cylinder->cylinderVA = cylinderVA;

	GLuint cylinderBuffers[4];
	glGenBuffers(4, cylinderBuffers);

	glBindVertexArray(cylinderVA);
	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*cylinder->vertexNum,
					cylinder->positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPosLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPosLoc);

	//Colors
	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffers[1]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*cylinder->vertexNum,
					cylinder->colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexColorLoc);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffers[2]);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(float)*cylinder->vertexNum,
					cylinder->normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderBuffers[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*(cylinder->indicesNum),
			cylinder->indices, GL_STATIC_DRAW);
	glPrimitiveRestartIndex(0xFFFF);
	glEnable(GL_PRIMITIVE_RESTART);

	cylinder->cylinderBuffers[0] = cylinderBuffers[0];
	cylinder->cylinderBuffers[1] = cylinderBuffers[1];
	cylinder->cylinderBuffers[2] = cylinderBuffers[2];
	cylinder->cylinderBuffers[3] = cylinderBuffers[3];
}

void cylinder_draw(Cylinder cylinder){
	    glBindVertexArray(cylinder->cylinderVA);

	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder->cylinderBuffers[3]);

	    glDrawElements(GL_TRIANGLE_FAN, cylinder->slices+2,GL_UNSIGNED_SHORT,0);
	    glDrawElements(GL_TRIANGLE_STRIP, cylinder->vertexNum+cylinder->slices,GL_UNSIGNED_SHORT,  (void*) ((cylinder->slices+1)*sizeof(GLushort)) );

	    glDrawElements(GL_TRIANGLE_FAN, cylinder->slices+2,GL_UNSIGNED_SHORT, (void*) (sizeof(GLushort)*(cylinder->indicesNum-cylinder->slices-2)) );

}

void cylinder_drawLines(Cylinder cylinder){
	    glBindVertexArray(cylinder->cylinderVA);
	    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinder->cylinderBuffers[3]);
	    glDrawElements(GL_LINE_STRIP, cylinder->slices+1,GL_UNSIGNED_SHORT, (void*)2);
	    glDrawElements(GL_LINE_STRIP, cylinder->vertexNum+cylinder->slices,GL_UNSIGNED_SHORT,  (void*) ((cylinder->slices+1)*sizeof(GLushort)) );

	    glDrawElements(GL_LINE_STRIP, cylinder->slices+1,GL_UNSIGNED_SHORT,(void*) (sizeof(GLushort)*(cylinder->indicesNum-cylinder->slices-2)));
}

void print_cylinder_positions(Cylinder cylinder){
	for(int i=0; i<cylinder->vertexNum; i++){
		printf("%d: %.3f, %.3f, %.3f\n", i, cylinder->positions[3*i], cylinder->positions[3*i+1], cylinder->positions[3*i+2]);
	}
}

void print_cylinder_colors(Cylinder cylinder){
	for(int i=0; i<cylinder->vertexNum; i++){
		printf("r: %f g: %f b: %f\n", cylinder->colors[3*i], cylinder->colors[3*i+1], cylinder->colors[3*i+2]);
	}
}

void print_cylinder_normals(Cylinder cylinder){
	for(int i=0; i<cylinder->vertexNum; i++){
		printf("%f, %f, %f\n", cylinder->normals[3*i], cylinder->normals[3*i+1], cylinder->normals[3*i+2]);
	}
}

void print_cylinder_indices(Cylinder cylinder){
	for(int i=0; i<cylinder->indicesNum; i++){
		printf("%d: %d\n", i, cylinder->indices[i]);
	}
}






