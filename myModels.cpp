#include "myModels.hpp"

#define VALS_PER_VERT 3
#define VALS_PER_TEX 2
#define VALS_PER_NORM 3

int myModels::initSky(){
    // Positions and normals combined in one array
    // Could equally use 2 separate arrays as before 
    GLfloat cubeVertices[] = {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 

        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f,

        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
    };
    
    glGenVertexArrays(1, &skyVaoHandle);
    glBindVertexArray(skyVaoHandle);
  
    // Buffers to store position, colour and index data
    unsigned int buffer[2];
    glGenBuffers(2, buffer);

    // Set vertex position
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);

    // Set vertex normal
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),(GLvoid*)(3 * sizeof(GLfloat)));
    
    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    return 0;
}

int myModels::initCube(GLuint ProgramID){
	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec2> vertex_texture_coordinates;
	std::vector<glm::uvec3> indexed_triangles;

	vertex_positions.emplace_back(-0.5f, 0.5f, 0.5f);	//Face 1	(Vertexes 0, 3)
	vertex_positions.emplace_back(0.5f, 0.5f, 0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);

	vertex_positions.emplace_back(0.5f, 0.5f, 0.5f);	//Face 2	(Vertexes 4, 7)
	vertex_positions.emplace_back(0.5f, 0.5f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);

	vertex_positions.emplace_back(0.5f, 0.5f, -0.5f);	//Face 3	(Vertexes 8, 11)
	vertex_positions.emplace_back(-0.5f, 0.5f, -0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, 0.5f, -0.5f);	//Face 4	(Vertexes 12, 15)
	vertex_positions.emplace_back(-0.5f, 0.5f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);	//Face 5 (Bottom face)	(Vertexes 16, 19)
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, 0.5f, 0.5f);	//Face 6 (Top face)	(Vertexes 20, 23)
	vertex_positions.emplace_back(0.5f, 0.5f, 0.5f);
	vertex_positions.emplace_back(0.5f, 0.5f, -0.5f);
	vertex_positions.emplace_back(-0.5f, 0.5f, -0.5f);

	vertex_normals.emplace_back(0, 0, 1);	//Z is +ve
	vertex_normals.emplace_back(0, 0, 1);
	vertex_normals.emplace_back(0, 0, 1);
	vertex_normals.emplace_back(0, 0, 1);

	vertex_normals.emplace_back(1, 0, 0);	//X is +ve
	vertex_normals.emplace_back(1, 0, 0);
	vertex_normals.emplace_back(1, 0, 0);
	vertex_normals.emplace_back(1, 0, 0);

	vertex_normals.emplace_back(0, 0, -1);	//Z is -ve
	vertex_normals.emplace_back(0, 0, -1);
	vertex_normals.emplace_back(0, 0, -1);
	vertex_normals.emplace_back(0, 0, -1);

	vertex_normals.emplace_back(-1, 0, 0);	//X is -ve
	vertex_normals.emplace_back(-1, 0, 0);
	vertex_normals.emplace_back(-1, 0, 0);
	vertex_normals.emplace_back(-1, 0, 0);

	vertex_normals.emplace_back(0, -1, 0);	//Y is -ve
	vertex_normals.emplace_back(0, -1, 0);
	vertex_normals.emplace_back(0, -1, 0);
	vertex_normals.emplace_back(0, -1, 0);

	vertex_normals.emplace_back(0, 1, 0);	//Y is +ve
	vertex_normals.emplace_back(0, 1, 0);
	vertex_normals.emplace_back(0, 1, 0);
	vertex_normals.emplace_back(0, 1, 0);

	for(int i = 0; i < 6; i++){
		vertex_texture_coordinates.emplace_back(0.0f, 0.0f);
		vertex_texture_coordinates.emplace_back(1.0f, 0.0f);
		vertex_texture_coordinates.emplace_back(1.0f, 1.0f);
		vertex_texture_coordinates.emplace_back(0.0f, 1.0f);

		indexed_triangles.emplace_back(0 + (4*i), 1 + (4*i), 2 + (4*i));
		indexed_triangles.emplace_back(2 + (4*i), 3 + (4*i), 0 + (4*i));
	}

	cubeIndexTriSize = indexed_triangles.size();

    // The usual code to send data to GPU and link it to shader variables
	glGenVertexArrays(1, &cubeVaoHandle);
	glBindVertexArray(cubeVaoHandle);

	unsigned int buffer[4];
	glGenBuffers(4, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_positions.size(), &vertex_positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_normals.size(), &vertex_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_NORM, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_texture_coordinates.size(), &vertex_texture_coordinates[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

    // Array of vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indexed_triangles.size(), &indexed_triangles[0], GL_STATIC_DRAW);

    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;
}

int myModels::initFrame(GLuint ProgramID){
	std::vector<glm::vec3> vertex_positions;
	std::vector<glm::vec3> vertex_normals;
	std::vector<glm::vec2> vertex_texture_coordinates;
	std::vector<glm::uvec3> indexed_triangles;

	vertex_positions.emplace_back(-0.5f, -0.4f, 0.5f);	//Face 1	(Vertexes 0, 3)
	vertex_positions.emplace_back(0.5f, -0.4f, 0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);

	vertex_positions.emplace_back(0.5f, -0.4f, 0.5f);	//Face 2	(Vertexes 4, 7)
	vertex_positions.emplace_back(0.5f, -0.4f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);

	vertex_positions.emplace_back(0.5f, -0.4f, -0.5f);	//Face 3	(Vertexes 8, 11)
	vertex_positions.emplace_back(-0.5f, -0.4f, -0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, -0.4f, -0.5f);	//Face 4	(Vertexes 12, 15)
	vertex_positions.emplace_back(-0.5f, -0.4f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, -0.5f, 0.5f);	//Face 5 (Bottom face)	(Vertexes 16, 19)
	vertex_positions.emplace_back(0.5f, -0.5f, 0.5f);
	vertex_positions.emplace_back(0.5f, -0.5f, -0.5f);
	vertex_positions.emplace_back(-0.5f, -0.5f, -0.5f);

	vertex_positions.emplace_back(-0.5f, -0.4f, 0.5f);	//Face 6 (Top face)	(Vertexes 20, 23)
	vertex_positions.emplace_back(0.5f, -0.4f, 0.5f);
	vertex_positions.emplace_back(0.5f, -0.4f, -0.5f);
	vertex_positions.emplace_back(-0.5f, -0.4f, -0.5f);

	vertex_normals.emplace_back(0, 0, 1);	//Z is +ve
	vertex_normals.emplace_back(0, 0, 1);
	vertex_normals.emplace_back(0, 0, 1);
	vertex_normals.emplace_back(0, 0, 1);

	vertex_normals.emplace_back(1, 0, 0);	//X is +ve
	vertex_normals.emplace_back(1, 0, 0);
	vertex_normals.emplace_back(1, 0, 0);
	vertex_normals.emplace_back(1, 0, 0);

	vertex_normals.emplace_back(0, 0, -1);	//Z is -ve
	vertex_normals.emplace_back(0, 0, -1);
	vertex_normals.emplace_back(0, 0, -1);
	vertex_normals.emplace_back(0, 0, -1);

	vertex_normals.emplace_back(-1, 0, 0);	//X is -ve
	vertex_normals.emplace_back(-1, 0, 0);
	vertex_normals.emplace_back(-1, 0, 0);
	vertex_normals.emplace_back(-1, 0, 0);

	vertex_normals.emplace_back(0, -1, 0);	//Y is -ve
	vertex_normals.emplace_back(0, -1, 0);
	vertex_normals.emplace_back(0, -1, 0);
	vertex_normals.emplace_back(0, -1, 0);

	vertex_normals.emplace_back(0, 1, 0);	//Y is +ve
	vertex_normals.emplace_back(0, 1, 0);
	vertex_normals.emplace_back(0, 1, 0);
	vertex_normals.emplace_back(0, 1, 0);

	for(int i = 0; i < 6; i++){
		if(i < 4){
			vertex_texture_coordinates.emplace_back(0.0f, 0.9f);
			vertex_texture_coordinates.emplace_back(1.0f, 0.9f);
			vertex_texture_coordinates.emplace_back(1.0f, 1.0f);
			vertex_texture_coordinates.emplace_back(0.0f, 1.0f);
		}
		else{
			vertex_texture_coordinates.emplace_back(0.0f, 0.0f);
			vertex_texture_coordinates.emplace_back(1.0f, 0.0f);
			vertex_texture_coordinates.emplace_back(1.0f, 1.0f);
			vertex_texture_coordinates.emplace_back(0.0f, 1.0f);
		}

		indexed_triangles.emplace_back(0 + (4*i), 1 + (4*i), 2 + (4*i));
		indexed_triangles.emplace_back(2 + (4*i), 3 + (4*i), 0 + (4*i));
	}

	frameIndexTriSize = indexed_triangles.size();

    // The usual code to send data to GPU and link it to shader variables
	glGenVertexArrays(1, &frameVaoHandle);
	glBindVertexArray(frameVaoHandle);

	unsigned int buffer[4];
	glGenBuffers(4, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_positions.size(), &vertex_positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertex_normals.size(), &vertex_normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_NORM, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * vertex_texture_coordinates.size(), &vertex_texture_coordinates[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

    // Array of vertex indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indexed_triangles.size(), &indexed_triangles[0], GL_STATIC_DRAW);

    // Un-bind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return 0;
}

unsigned int myModels::getIndexTriSize(std::string name){
	if(name.compare("cube") == 0){
		return cubeIndexTriSize;
	}
	if(name.compare("frame") == 0){
		return frameIndexTriSize;
	}
	if(name.compare("sky") == 0){
		std::cout << "There is no sky IndexTriSize\n";
		return -1;
	}
	return 0;
}

unsigned int myModels::getVaoHandle(std::string name){
	if(name.compare("cube") == 0){
		return cubeVaoHandle;
	}
	if(name.compare("frame") == 0){
		return frameVaoHandle;
	}
	if(name.compare("sky") == 0){
		return skyVaoHandle;
	}
	return 0;
}