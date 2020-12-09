#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>
#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class myModels{
public:
	int initSky();
	int initCube(GLuint ProgramID);
	int initFrame(GLuint ProgramID);
	unsigned int getVaoHandle(std::string name);
	unsigned int getIndexTriSize(std::string name);
private:
	unsigned int skyVaoHandle;
	
	unsigned int cubeIndexTriSize;
	unsigned int cubeVaoHandle;

	unsigned int frameIndexTriSize;
	unsigned int frameVaoHandle;
};

#endif
