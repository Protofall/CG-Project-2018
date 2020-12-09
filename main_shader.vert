#version 330

uniform mat4 projection_matrix;

uniform mat4 model_matrix;
uniform mat4 view_matrix;

layout (location = 0) in vec3 a_vertex;	//Defined in model
layout (location = 1) in vec3 a_vert_normal;
layout (location = 2) in vec2 a_tex_coord;

out vec4 vertex;
out vec3 normal;
out vec2 st;	//My texture vector

void main(void){
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(a_vertex, 1.0);

    st = a_tex_coord;	//Pass texture coords to the fragment shader
	normal = normalize(mat3(view_matrix * model_matrix) * a_vert_normal);
	vertex = view_matrix * model_matrix * vec4(a_vertex, 1.0);
}