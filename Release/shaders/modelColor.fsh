#version 400

uniform vec3 modelColor; 

uniform float radius;
uniform vec2 centerPosition;
uniform int shape;
uniform float angleCos;
uniform float angleSin;

in vec2 vertexPosToFS;
out vec4 pixelColor; 

in vec2 vertexTexcoordToFS;
uniform sampler2D myTexture;



float area(vec2 A, vec2 B, vec2 C){
	return abs((A.x*(B.y-C.y)+B.x*(C.y-A.y)+C.x*(A.y-B.y))/2.0);
}

void main() { 

	//CIRCULO	
	if(shape == 0){
		float d = distance(vertexPosToFS, centerPosition);
	
		if(d < radius) discard;  		
	}
	
	//CUADRADO
	if(shape == 1){
		float rotatedX = angleCos*(vertexPosToFS.x-centerPosition.x) - angleSin*(vertexPosToFS.y-centerPosition.y)+centerPosition.x;
		
		float rotatedY = angleSin*(vertexPosToFS.x-centerPosition.x) + angleCos*(vertexPosToFS.y-centerPosition.y) + centerPosition.y;
		
		if(rotatedX>(centerPosition.x-radius) && rotatedX<(centerPosition.x+radius) && rotatedY<(centerPosition.y+radius) && rotatedY>(centerPosition.y-radius)) discard;
	}
	
	//TRIANGULO
	if(shape == 2){
		//VERTICE ARRIBA
		vec2 C = vec2(0,0) + vec2(0, sqrt(2)*radius);
		
		mat2 rotMatrix = mat2(vec2(angleCos, -angleSin), vec2(angleSin, angleCos));
		C = rotMatrix * C;
		
		mat2 rotB = mat2(vec2(-0.5, -0.8660), vec2(0.8660, -0.5));
		
		
		vec2 B = rotB * C;
		
		mat2 rotA = mat2(vec2(-0.5, 0.8660), vec2(-0.8660, -0.5));
		
		vec2 A = rotA * C;
		
		C += centerPosition;
		B += centerPosition;
		A += centerPosition;
		
		
		if(abs(area(C,B,A)-(area(vertexPosToFS.xy, A, B) + area(vertexPosToFS.xy, A, C) + area(vertexPosToFS.xy, B, C)))<0.005){
			discard;
		}
			
	}
	
	
	pixelColor = texture(myTexture, vertexTexcoordToFS); 
	//pixelColor = vec4(vertexTexcoordToFS.x, vertexTexcoordToFS.y,0, 0); 


} 
