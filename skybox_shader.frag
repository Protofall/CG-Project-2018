#version 330

out vec4 fragColor;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate

uniform samplerCube skybox;

void main(void){
    fragColor = texture(skybox, TexCoords);
}
