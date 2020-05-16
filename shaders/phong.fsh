#version 400

in vec3 worldVertexPosition;
in vec3 worldNormalNormal;

uniform vec3 cameraPosition;
uniform vec3 ambientLight;
uniform vec3 materialA;
uniform vec3 materialD;
uniform vec3 materialS;

const int N = 3;

struct Light {
	vec3 lightColor;
	vec3 ligthPosition;
	float exponent;	
};

uniform LightBlock {
	Light lights[N];
};


out vec4 pixelColor;

void main() {
	
	vec3 n = normalize(worldNormalNormal);
	vec3 v = normalize(cameraPosition - worldVertexPosition);
	int i;
	vec3 l, r;
	float factorD, factorS;
	vec3 tempPixelColor = ambientLight * materialA;
	for(i = 0; i < N; i++){
		l = normalize(lights[i].ligthPosition - worldVertexPosition);
		factorD = clamp(dot(n, l), 0, 1);
		r = normalize((2 * n) * dot(n, l) - l);
		factorS = clamp(pow(dot(r, v), lights[i].exponent), 0, 1);
		tempPixelColor += lights[i].lightColor * (materialD * factorD + materialS * factorS);
	}
	
	pixelColor = vec4(clamp(tempPixelColor, 0, 1), 1);
}

