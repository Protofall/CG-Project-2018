#version 330

layout (location = 0) in vec3 a_vertex; 
layout (location = 1) in vec3 a_normal; 

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec3 TexCoords;

void main(void){
	vec4 pos = projection_matrix * view_matrix * model_matrix * vec4(a_vertex, 1.0);
	gl_Position = pos.xyww;
	TexCoords = a_vertex;
}
