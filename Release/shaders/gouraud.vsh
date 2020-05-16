#version 330
 
in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 vertexColor;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
uniform vec3 ambientLight;
uniform vec3 materialA;
uniform vec3 diffuseLight;
uniform vec3 lightPosition;
uniform vec3 materialD;
 
uniform vec3 materialS;
uniform float exponent;
uniform vec3 camera;
 
out vec3 vertexColorToFS;
 
void main() {
    vec4 worldPosition = modelMatrix * vec4(vertexPosition, 1);
    gl_Position = projMatrix * viewMatrix * worldPosition;
    
    // Modelo de Phong implementado a nivel vrtice (Gouraud)
    vec3 vertexToLight = normalize(lightPosition - worldPosition.xyz);
 
    // Clculo del factor difuso
    mat4 G = transpose(inverse(modelMatrix));
    vec4 worldNormal = G * vec4(vertexNormal, 0);
    vec3 unitaryNormal = normalize(worldNormal.xyz);
    float factorD = clamp(dot(vertexToLight, unitaryNormal), 0, 1);
    
    // Clculo del factor especular
    vec3 vertexToCamera = normalize(camera - worldPosition.xyz);
    vec3 reflected = normalize(2 * unitaryNormal * dot(unitaryNormal, vertexToLight) - vertexToLight);
    float factorS = clamp(pow(dot(reflected, vertexToCamera), exponent), 0, 1);
    
    vertexColorToFS = ambientLight * materialA *vertexColor + diffuseLight * (materialD * factorD + materialS * factorS);
    vertexColorToFS = clamp(vertexColorToFS, 0, 1);
}