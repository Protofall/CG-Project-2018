#version 330

in vec4 vertex;
in vec3 normal;
in vec2 st;

out vec4 fragColour;

uniform int procTex;	//Toggle for drawing with texMap texture or generating a procedural texture
uniform sampler2D texMap;

uniform float time;	//Time, used to change the procedural pattern

//rand and noise functions were copied from an external source: https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83
float rand(vec2 n) { 
	return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

float noise(vec2 n){
	const vec2 d = vec2(0.0, 1.0);
	vec2 b = floor(n), f = smoothstep(vec2(0.0), vec2(1.0), fract(n));
	return mix(mix(rand(b), rand(b + d.yx), f.x), mix(rand(b + d.xy), rand(b + d.yy), f.x), f.y);
}

void main(void){
	vec3 source = -vertex.xyz;	//Get distance vector of fragment from camera
	float fragDistance = length(source);
	source = normalize(source);	//Turning vector into unit vector

	vec3 colour = vec3(1.0, 1.0, 1.0);
	vec3 ambient = vec3(0.5f, 0.5f, 0.5f);

	float sDotN;
	sDotN = max(dot(source, normal), 0.0);
	vec3 diffuseANDspec = colour * sDotN * pow(dot(source, vec3(0, 0, 1)), 10) / (fragDistance/2);
    
	float screenGamma = 2.2;
	float intensity = 1.0;

	//If we are looking at a transparent object, we don't want the light to hit it
	float alpha = texture(texMap, st).a;
	if(procTex == 0){
		if(alpha != 1.0){
			fragColour = texture(texMap, st) * intensity;
		}
		else{
			fragColour = vec4(ambient + diffuseANDspec, 1.0) * texture(texMap, st) * intensity;
		}
		return;
	}

	//Calculate the procedural texture and apply it here, I made it look like Sonic Adventure 2, Crazy Gadget's Green Goop stuff
	vec4 texColour = vec4(1.0, 1.0, 1.0, 1.0);

	float s = st.s;
	float t = st.t;
	texColour *= ((1 + sin( (s + noise(vec2(s * 5 , t * 5)) / 2 ) * 50 + (time * 25)) ) / 2);

	//Forcing the texture to have full alpha, only be green and that it varies from full green to 20% green
	texColour.a = 1.0;
	texColour.r = 0.0;
	texColour.b = 0.0;
	if(texColour.g < 0.2){
		texColour.g = 0.2;
	}

	fragColour = vec4(ambient + diffuseANDspec, 1.0) * texColour * intensity;
	return;
}
