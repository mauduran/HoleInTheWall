/*
 * Through the hole Game
 *
 *  Created on: 14/05/2020
 *      Author:
 *      Martín Casillas Ramos
 *      Mauricio Durán Padilla
 *      Edgar Francisco Medina Rifas
*/

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Transforms.h"
#include "Utils.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Sphere.h"
#include "Cylinder.h"

#define ROOM_WIDTH 50
#define ROOM_HEIGHT 25
#define ROOM_DEPTH  200

typedef enum { IDLE, LEFT, RIGHT, UP, DOWN, R_LEFT, R_RIGHT  } MOTION_TYPE;


typedef float vec3[3];

static Mat4   modelMatrix, projectionMatrix, viewMatrix;
static GLuint programId1, vertexPositionLoc,  vertexNormalLoc, modelMatrixLoc,  projectionMatrixLoc,  viewMatrixLoc;
static GLuint programId2, vertexPositionLoc2, vertexNormalLoc2, modelColorLoc2, vertexTexcoordLoc, modelMatrixLoc2,
projectionMatrixLoc2, viewMatrixLoc2, angleCosLoc, angleSinLoc ,centerPosition, radiusLoc, shapeLoc;

static GLuint textures[20];

static GLuint vertexColorLoc;
static GLuint ambientLightLoc, materialALoc, materialDLoc;
static GLuint materialSLoc, cameraPositionLoc;

Sphere sphere;
Cylinder square;
Cylinder cone;



static int currentTex = 0;
static int score = 0;

GLuint wallVA, roomVA;
GLuint roomBuffers[3];

static MOTION_TYPE motionType      = 0;

// 0: Sphere, 1: Cube, 2: Pyramid
static short currentShape = 0;

static float cameraZ         = ROOM_DEPTH/2-1;

static float wallZ = 0;
static float wallSpeed = 0.3;


static float shapeSpeed     = 0.35;
static float shapeX         = 0;
static float shapeY         = 0;
static float shapeRadius	= 3;
static float shapeZ         = ROOM_DEPTH/2;
static float shapeAngle     = 0;


static vec3 ambientLight  = {0.35, 0.35, 0.5};
static vec3 materialA     = {0.1, 0.1, 0.1};
static vec3 materialShape = {0.2, 0.1, 0.25};
static vec3 materialD     = {0.6, 0.6, 0.6};
static vec3 materialS     = {0.6, 0.6, 0.6};


static float centerPos[2] = {5,0};
static int   wallShape = 0;
static float wallRadius = 5;
static float wallAngle = 0;
//                          Color    Pad  Position   Exponent
static float lights[]   = { 1, 0, 0,  0,  10, 0, 70,   128,
		                    0, 1, 0,  0,   0, 0, 70,   128,
		                    0, 0, 1,  0,   -10, 0, 70,   128};

static GLuint lightsBufferId;


static vec3 shapeVertices [4];
static vec3 wallVertices  [4];

static void printResult(){
	printf("SCORE: %d\n\n\n", score);
}

static float euclideanDist(float x1, float y1, float x2, float y2){
	return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

static void genSquareVertices(vec3 target[4], float shX, float shY, float angle, float radius){

	float x = (shX + radius);
	float y = (shY + radius);

	//Superior Derecha (x, y, z)
	target[0][0] = cos(angle)*(x-shX) - sin(angle)*(y-shY)+ shX;
	target[0][1] = sin(angle)*(x-shX) + cos(angle)*(y-shY) + shY;
	target[0][2] = 0;

	//Inferior Derecha (x, y, z)
	y = (shY - radius);
	target[1][0] = cos(angle)*(x-shX) - sin(angle)*(y-shY)+ shX;
	target[1][1] = sin(angle)*(x-shX) + cos(angle)*(y-shY) + shY;
	target[1][2] = 0;


	//Inferior izq (x, y, z)
	x = (shX - radius);
	y = (shY - radius);
	target[2][0] = cos(angle)*(x-shX) - sin(angle)*(y-shY)+ shX;
	target[2][1] =	sin(angle)*(x-shX) + cos(angle)*(y-shY) + shY;
	target[2][2] = 0;

	//Superior izq (x, y, z)
	y = (shY + radius);
	target[3][0] = cos(angle)*(x-shX) - sin(angle)*(y-shY)+ shX;
	target[3][1] = sin(angle)*(x-shX) + cos(angle)*(y-shY) + shY;
	target[3][2] = 0;



}

static void genTrianglesVertices(vec3 target[4], float shX, float shY,  float angle, float radius){
	float tempY = radius;


	// (0, radio)

	//Se rota radio segun el angulo
	float y = cos(angle)*(tempY) + shY;
	float x = - sin(angle)*(tempY) + shX;


	target[0][0] = x ;
	target[0][1] = y;
	target[0][2] = 0;


	//Derecha (x, y, z)
	target[1][0] = -0.5*(x-shX) -0.8660*(y-shY)+ shX;
	target[1][1] = 0.8660*(x-shX) + -0.5*(y-shY) + shY;
	target[1][2] = 0;

	//inferior izq (x, y, z)
	target[2][0] = -0.5*(x-shX) + 0.8660*(y-shY)+ shX;
	target[2][1] = -0.8660*(x-shX) + -0.5*(y-shY) + shY;
	target[2][2] = 0;


}

static float calcTriangleArea(vec3 p1, vec3 p2, vec3 p3){
	return fabs((p1[0]*(p2[1]-p3[1])+p2[0]*(p3[1]-p1[1])+p3[0]*(p1[1]-p2[1]))/2.0);
}

static bool isPointInsideSquare(vec3 point, vec3 wall[4]){
	float sqArea = pow(2*wallRadius,2);
	vec3 newPoint;
	newPoint[0] = -point[0];
	newPoint[1] = point[1];
	newPoint[2] = point[2];

	float partialAreas = calcTriangleArea(newPoint, wall[0], wall[1]);
	partialAreas += calcTriangleArea(newPoint, wall[1], wall[2]);
	partialAreas += calcTriangleArea(newPoint, wall[2], wall[3]);
	partialAreas += calcTriangleArea(newPoint, wall[3], wall[0]);


	return (fabs(sqArea-partialAreas) < 1);
}

static bool isPointInsideTriangle(vec3 point, vec3 wall[4]){

	float trArea = calcTriangleArea(wall[0], wall[1], wall[2]);

	vec3 newPoint;
	newPoint[0] = -point[0];
	newPoint[1] = point[1];
	newPoint[2] = point[2];

	float partialAreas = calcTriangleArea(newPoint, wall[0], wall[1]);
	partialAreas += calcTriangleArea(newPoint, wall[1], wall[2]);
	partialAreas += calcTriangleArea(newPoint, wall[2], wall[0]);

	return (fabs(trArea-partialAreas) < 1);
}


 static void initTexture(const char* filename, GLuint textureId) {
 	unsigned char* data;
 	unsigned int width, height;
 	glBindTexture(GL_TEXTURE_2D, textureId);
 	loadBMP(filename, &data, &width, &height);
 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 }

 static void initTextures() {
 	glGenTextures(18, textures);

 	initTexture("textures/1.bmp" , textures[0]);
 	initTexture("textures/2.bmp" , textures[1]);
 	initTexture("textures/3.bmp" , textures[2]);
 	initTexture("textures/4.bmp" , textures[3]);
 	initTexture("textures/5.bmp" , textures[4]);
 	initTexture("textures/6.bmp" , textures[5]);
 	initTexture("textures/7.bmp" , textures[6]);
 	initTexture("textures/8.bmp" , textures[7]);
 	initTexture("textures/9.bmp" , textures[8]);
 	initTexture("textures/10.bmp" , textures[9]);
 	initTexture("textures/11.bmp" , textures[10]);
 	initTexture("textures/12.bmp" , textures[11]);
 	initTexture("textures/13.bmp" , textures[12]);
 	initTexture("textures/14.bmp" , textures[13]);
 	initTexture("textures/15.bmp" , textures[14]);
 	initTexture("textures/16.bmp" , textures[15]);
 	initTexture("textures/17.bmp" , textures[16]);
 	initTexture("textures/18.bmp" , textures[17]);
 	initTexture("textures/19.bmp" , textures[18]);
 	initTexture("textures/20.bmp" , textures[19]);

 }

static void initShaders() {
	shapeZ = ROOM_DEPTH/2-(shapeRadius*15);
	GLuint vShader = compileShader("shaders/phong.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	GLuint fShader = compileShader("shaders/phong.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;
	programId1 = glCreateProgram();
	glAttachShader(programId1, vShader);
	glAttachShader(programId1, fShader);
	glLinkProgram(programId1);

	vertexPositionLoc   = glGetAttribLocation(programId1, "vertexPosition");
	vertexNormalLoc     = glGetAttribLocation(programId1, "vertexNormal");
	modelMatrixLoc      = glGetUniformLocation(programId1, "modelMatrix");
	viewMatrixLoc       = glGetUniformLocation(programId1, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId1, "projMatrix");
	ambientLightLoc     = glGetUniformLocation(programId1, "ambientLight");
	materialALoc        = glGetUniformLocation(programId1, "materialA");
	materialDLoc        = glGetUniformLocation(programId1, "materialD");
	materialSLoc        = glGetUniformLocation(programId1, "materialS");
	cameraPositionLoc   = glGetUniformLocation(programId1, "cameraPosition");

	vertexColorLoc 		= glGetUniformLocation(programId1, "vertexColor");


	vShader = compileShader("shaders/position_mvp.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	fShader = compileShader("shaders/modelColor.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;
	programId2 = glCreateProgram();
	glAttachShader(programId2, vShader);
	glAttachShader(programId2, fShader);
	glLinkProgram(programId2);

	vertexPositionLoc2   = glGetAttribLocation(programId2, "vertexPosition");
	vertexNormalLoc2		 = glGetAttribLocation(programId2, "vertexNormal");
	vertexTexcoordLoc   = glGetAttribLocation(programId2, "vertexTexcoord");
	modelMatrixLoc2      = glGetUniformLocation(programId2, "modelMatrix");
	viewMatrixLoc2       = glGetUniformLocation(programId2, "viewMatrix");
	projectionMatrixLoc2 = glGetUniformLocation(programId2, "projectionMatrix");
	modelColorLoc2       = glGetUniformLocation(programId2, "modelColor");
	centerPosition       = glGetUniformLocation(programId2, "centerPosition");
	radiusLoc       = glGetUniformLocation(programId2, "radius");
	shapeLoc       = glGetUniformLocation(programId2, "shape");
	angleCosLoc			 = glGetUniformLocation(programId2, "angleCos");
	angleSinLoc 		 = glGetUniformLocation(programId2, "angleSin");


}

static void initLights() {
	glUseProgram(programId1);
	glUniform3fv(ambientLightLoc,  1, ambientLight);

	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);

	glGenBuffers(1, &lightsBufferId);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW);

	GLuint uniformBlockIndex = glGetUniformBlockIndex(programId1, "LightBlock");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
	glUniformBlockBinding(programId1, uniformBlockIndex, 0);
}

static void initWall(){
	float w1 = -ROOM_WIDTH  / 2, w2 = ROOM_WIDTH  / 2;
	float h1 = -ROOM_HEIGHT / 2, h2 = ROOM_HEIGHT / 2;
	float d2 = -ROOM_DEPTH  / 2 +0.1;

	float positions[] = {
			w2, h2, d2, w2, h1, d2, w1, h1, d2,   w1, h1, d2	, w1, h2, d2	, w2, h2, d2,
	};

	float normals[] = {0,  0, 1,  0,  0, 1,  0,  0, 1,  0,  0, 1,  0,  0, 1,  0,  0, 1};  // Atrás


	glUseProgram(programId2);
	glGenVertexArrays(1, &wallVA);
	glBindVertexArray(wallVA);
	GLuint buffers[3];
	glGenBuffers(3, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc2, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc2);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc2, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc2);

	float wh = (float) ROOM_WIDTH / ROOM_HEIGHT;

	float texcoords[] = { 0.65 * wh, 0.65,  0.65 * wh, 0,       0, 0,        0, 0,        0, 0.65,  0.65 * wh, 0.65 };


	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexTexcoordLoc, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexTexcoordLoc);

}

static void initRoom() {
	float w1 = -ROOM_WIDTH  / 2, w2 = ROOM_WIDTH  / 2;
	float h1 = -ROOM_HEIGHT / 2, h2 = ROOM_HEIGHT / 2;
	float d1 = -ROOM_DEPTH  / 2, d2 = ROOM_DEPTH  / 2;

	float positions[] = {w1, h2, d1, w1, h1, d1, w2, h1, d1,   w2, h1, d1, w2, h2, d1, w1, h2, d1,  // Frente
			             w2, h2, d2, w2, h1, d2, w1, h1, d2,   w1, h1, d2, w1, h2, d2, w2, h2, d2,  // Atrás
			             w1, h2, d2, w1, h1, d2, w1, h1, d1,   w1, h1, d1, w1, h2, d1, w1, h2, d2,  // Izquierda
			             w2, h2, d1, w2, h1, d1, w2, h1, d2,   w2, h1, d2, w2, h2, d2, w2, h2, d1,  // Derecha
			             w1, h1, d1, w1, h1, d2, w2, h1, d2,   w2, h1, d2, w2, h1, d1, w1, h1, d1,  // Abajo
						 w1, h2, d2, w1, h2, d1, w2, h2, d1,   w2, h2, d1, w2, h2, d2, w1, h2, d2   // Arriba
	};

	float normals[] = { 0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  // Frente
						0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  // Atrás
					    1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  // Izquierda
					   -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0,  // Derecha
					    0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  // Abajo
					    0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  // Arriba
	};

	glUseProgram(programId1);
	glGenVertexArrays(1, &roomVA);
	glBindVertexArray(roomVA);
	GLuint buffers[2];
	glGenBuffers(2, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	sphere_bind(sphere,vertexPositionLoc, vertexColorLoc, vertexNormalLoc);
	cylinder_bind(square, vertexPositionLoc, vertexColorLoc, vertexNormalLoc);
	cylinder_bind(cone, vertexPositionLoc, vertexColorLoc, vertexNormalLoc);

}

static void displayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float dist = 0.2;
	float w1 = -ROOM_WIDTH  / 2 + dist, w2 = ROOM_WIDTH  / 2 - dist;
	float h1 = -ROOM_HEIGHT / 2 + dist, h2 = ROOM_HEIGHT / 2 - dist;

	switch(motionType) {
  		case  RIGHT  :  if(shapeX - shapeSpeed >=  w1 +shapeRadius) shapeX -= shapeSpeed; break;
  		case  LEFT :  if(shapeX + shapeSpeed <=  w2 -shapeRadius) shapeX += shapeSpeed; break;
		case  DOWN :  if(shapeY - shapeSpeed >=  h1 +shapeRadius) shapeY -= shapeSpeed; break;
		case  UP  :  if(shapeY + shapeSpeed <=  h2 -shapeRadius) shapeY += shapeSpeed; break;
		case  R_LEFT : shapeAngle += 1; break;
		case  R_RIGHT: shapeAngle -= 1; break;
		case  IDLE  :  ;
	}


	//Creates new wall and updates score
	//Calculate centerPos of new shape and shape type.
	if(wallZ> ROOM_DEPTH){
		currentTex = rand()%20;
		score++;
		wallZ = 0;
		wallShape = rand()%3;

		wallAngle = rand()*2*M_PI/RAND_MAX;


		if(wallShape!=0){
			float squareH = wallRadius*sqrt(2);
			centerPos[0]= rand()%(ROOM_WIDTH-(int)squareH*2) - (ROOM_WIDTH/2-squareH);
			centerPos[1] = rand()%(ROOM_HEIGHT-(int)squareH*2) - (ROOM_HEIGHT/2-squareH);
		} else{
			centerPos[0]= rand()%(ROOM_WIDTH-(int)wallRadius*2) - (ROOM_WIDTH/2-wallRadius);
			centerPos[1] = rand()%(ROOM_HEIGHT-(int)wallRadius*2) - (ROOM_HEIGHT/2-wallRadius);
		}

		if(wallShape==1){
			genSquareVertices(wallVertices, centerPos[0], centerPos[1], wallAngle, wallRadius);
		} else if(wallShape==2){
			genTrianglesVertices(wallVertices, centerPos[0], centerPos[1], wallAngle,  wallRadius*sqrt(2));

		}

		wallSpeed *= 1.05;

	}

	int dR = (currentShape==2)? sqrt(2)*shapeRadius: shapeRadius;

	if(wallZ>=155-(dR) && wallZ<=155+(dR)){

		float angle = shapeAngle*M_PI/180.0;

		if (currentShape==1){

			//Cuadrado

			genSquareVertices(shapeVertices, shapeX, shapeY, angle,shapeRadius);



		} else if (currentShape == 2){
			//Triangulo

			genTrianglesVertices(shapeVertices, shapeX, shapeY, angle, shapeRadius);

		}

		int num = 3;
		if(currentShape==1) num=4;

		//Circulo en Muro
		if(currentShape==0 || wallShape == 0){
			if(euclideanDist(-centerPos[0], centerPos[1], shapeX, shapeY)>wallRadius-dR){
				printResult();
				exit(0);
			}
		} else if(wallShape == 1){
			//Cuadrado

			for(int i=0; i<num; i++){
				if(!isPointInsideSquare(shapeVertices[i], wallVertices)) {
					printResult();
					exit(0);
				}
			}
		} else {
			for(int i=0; i<num; i++){
				if(!isPointInsideTriangle(shapeVertices[i], wallVertices)){
					printResult();
					exit(0);
				}
			}
		}
	}

	glUseProgram(programId1);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
	mIdentity(&viewMatrix);
	rotateY(&viewMatrix, 0);
	translate(&viewMatrix, shapeX,  -shapeY, -cameraZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);

	glUniform3f(cameraPositionLoc, 0, 0, cameraZ);


//	Dibujar el cuarto
	glUniform3fv(materialALoc,     1, materialA);
	mIdentity(&modelMatrix);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	glBindVertexArray(roomVA);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	//Dibujar Figura
	glUniform3fv(materialALoc,     1, materialShape);
	mIdentity(&modelMatrix);
	translate(&modelMatrix, -shapeX, shapeY, shapeZ);
	rotateZ(&modelMatrix, shapeAngle);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);

	if(currentShape==0){
		sphere_draw(sphere);
	} else if(currentShape==1){
		cylinder_draw(square);
	} else {
		cylinder_draw(cone);
	}

	//Dibujar muro
	glUseProgram(programId2);
	glUniformMatrix4fv(projectionMatrixLoc2, 1, true, projectionMatrix.values);
	mIdentity(&modelMatrix);

	glUniformMatrix4fv(viewMatrixLoc2, 1, true, viewMatrix.values);

	translate(&modelMatrix, 0, 0, wallZ);

	glUniform1i(shapeLoc, wallShape);
	glUniform2f(centerPosition, centerPos[0], centerPos[1]);
	glUniform1f(radiusLoc, wallRadius);
	glUniform1f(angleCosLoc, cos(wallAngle));
	glUniform1f(angleSinLoc, sin(wallAngle));

	glUniform3f(modelColorLoc2, .4, .35, .45);
	glUniformMatrix4fv(modelMatrixLoc2, 1, true, modelMatrix.values);
	glUniform3f(cameraPositionLoc, 0, 0, cameraZ);
	glBindVertexArray(wallVA);
	glBindTexture(GL_TEXTURE_2D, textures[currentTex]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	wallZ+=wallSpeed;

	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
}

static void timerFunc(int id) {
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
}


static void keyReleasedFunc(unsigned char key,int x, int y) {
	motionType = IDLE;
	shapeSpeed = 0.5;
}



static void keyPressedFunc(unsigned char key, int x, int y) {
	switch(key) {
		case 'a':
		case 'A': motionType = LEFT; break;
		case 'd':
		case 'D': motionType = RIGHT; break;
		case 'w':
		case 'W': motionType = UP; break;
		case 's':
		case 'S': motionType = DOWN; break;
		case 'q':
		case 'Q': motionType = R_LEFT; break;
		case 'e':
		case 'E': motionType = R_RIGHT; break;
		case 32 : currentShape = (currentShape+1)%3; break;
		case 13 : shapeSpeed = 1; break;
		//13 es el enter. Para aumentar velocidad de movimiento
		case 27 : exit(0);
	}
 }




int main(int argc, char **argv) {
	vec3 sphereColor = {0.8, 0.3, 0.8};
	square = cylinder_create(shapeRadius*2, shapeRadius, shapeRadius, 20, 20, sphereColor, sphereColor);
	cone = cylinder_create(shapeRadius*2, shapeRadius, 0.1, 20, 20, sphereColor, sphereColor);
	sphere=sphere_create(shapeRadius, 100, 100, sphereColor);
	srand(time(0));
	setbuf(stdout, NULL);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Proyecto de Grafica. CDM");

    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);
    glutKeyboardUpFunc(keyReleasedFunc);

    glewInit();
    glEnable(GL_DEPTH_TEST);

    initTextures();
    initShaders();
    initLights();
    initWall();
    initRoom();

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glutMainLoop();

	return 0;
}
