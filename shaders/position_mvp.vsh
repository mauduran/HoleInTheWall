#version 400

in vec3 vertexPosition;
uniform mat4 modelMatrix;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec2 vertexPosToFS;

in vec2 vertexTexcoord;

out vec2 vertexTexcoordToFS;


void main() {
    vertexTexcoordToFS = vertexTexcoord;
	vertexPosToFS = vec2(vertexPosition);
  gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(vertexPosition, 1);
}
