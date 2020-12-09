#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>    // std::all_of

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "external_files/Shader.hpp"
#include "myModels.hpp"
#include "collision.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external_files/stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "external_files/tiny_obj_loader.h"

//Array to store program IDs
unsigned int ProgramIDs[2];

//Skybox stuff
unsigned int skyTexHandle;

// Maze and player variables
int** maze;
int mazeSize;
static float exitSpin = 0.0f;

float playerXPos;
float playerYPos;
float playerZPos;
static float playerVerticalRot = 0.0f;
static float playerHorizontalRot = 0.0f;
static float playerRunSpeed = 0.045f;

bool heldButtons[5];
bool controllerPressButtons[2];
bool inputMode = true;  //true = controller, false = keyboard/mouse
int joystickPresent;
bool firing = false;    //Is true when doing the recoil animation for the mega buster
float fireStart = 0.0f; //When the shot was fired

int count;
const float* axes;
const unsigned char* buttons;

//The myModels object
myModels my_models;

//.obj model vectors and handles
std::vector<tinyobj::shape_t> shapesToiletp0;
std::vector<tinyobj::material_t> materialsToiletp0;
std::vector<tinyobj::shape_t> shapesToiletp1;
std::vector<tinyobj::material_t> materialsToiletp1;
std::vector<tinyobj::shape_t> shapesToiletp2;
std::vector<tinyobj::material_t> materialsToiletp2;

std::vector<tinyobj::shape_t> shapesMegaBuster;
std::vector<tinyobj::material_t> materialsMegaBuster;
std::vector<tinyobj::shape_t> shapesSpaceCraft;
std::vector<tinyobj::material_t> materialsSpaceCraft;

unsigned int toilet0VaoHandle;
unsigned int toilet1VaoHandle;
unsigned int toilet2VaoHandle;
unsigned int megaBusterVaoHandle;
unsigned int spaceCraftVaoHandle;

//Collision boxes
std::vector<float> boxes;

static double FOV = 1.0;
static int winX = 640;  //Width
static int winY = 480;  //Height

#define NUM_TEXTURES 10 //Update this when adding new models
GLuint TexID[NUM_TEXTURES];

int createObjVAO(std::vector<float> vertex_positions, std::vector<float> vertex_normals,
            std::vector<float> vertex_texture_coordinates, std::vector<unsigned int> indexed_triangles, unsigned int &handle){

    // The usual code to send data to GPU and link it to shader variables.    
    glGenVertexArrays(1, &handle);
    glBindVertexArray(handle);

    unsigned int buffer[4];
    glGenBuffers(4, buffer);

    // Set vertex attribute
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_positions.size(), &vertex_positions[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Normals (This block MUST be done before the texture block)
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_normals.size(), &vertex_normals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Texture attributes
    glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_texture_coordinates.size(), &vertex_texture_coordinates[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    // Array of vertex indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indexed_triangles.size(), &indexed_triangles[0], GL_STATIC_DRAW);

    // Un-bind
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return 0;
}

// Load the 6 skymap textures for the cube map and set parameters
void createSkyMap(){
    glGenTextures(1, &skyTexHandle);
    glActiveTexture(GL_TEXTURE0);

    int width, height, nrChannels;

    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexHandle);

    std::vector<std::string> faces;

    faces.push_back("external_files/skybox/totality_rt.png");
    faces.push_back("external_files/skybox/totality_lf.png");
    faces.push_back("external_files/skybox/totality_up.png");
    faces.push_back("external_files/skybox/totality_dn.png");
    faces.push_back("external_files/skybox/totality_bk.png");
    faces.push_back("external_files/skybox/totality_ft.png");

    for(unsigned int i = 0; i < faces.size(); i++){
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if(data){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else{
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return;
}

// Load an image from file as texture "ID"
int loadTexture(std::string fileName, int ID){
    glActiveTexture(GL_TEXTURE0);

    int x, y, n;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fileName.c_str(), &x, &y, &n, 0);
    if(!data){
        printf("Texture not loaded\n");
        return 1;
    }

    glBindTexture(GL_TEXTURE_2D, TexID[ID]);
    if(n == 3){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }
    else if(n == 4){    //For pictures with an alpha channel
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    else {
        printf("Channels: %d\n", n);
        fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.\n");
    }
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    return 0;
}

//Load in the obj models
void loadObjModels(){
    std::string toiletp0 = "external_files/objModels/toilet/Toilet.obj";
    std::string toiletp1 = "external_files/objModels/toilet/ToiletLid1.obj";
    std::string toiletp2 = "external_files/objModels/toilet/ToiletLid2.obj";
    std::string megaBuster = "external_files/objModels/megaBuster/blaster.obj";
    std::string spaceCraft = "external_files/objModels/spaceCraft/Craft2.obj";

    std::string toiletBasePath = "external_files/objModels/toilet/";
    std::string megaBusterBasePath = "external_files/objModels/megaBuster/";
    std::string spaceCraftBasePath = "external_files/objModels/spaceCraft/";

    std::string loadErr;
    if(!LoadObj(shapesToiletp0, materialsToiletp0, loadErr, toiletp0.c_str(), toiletBasePath.c_str(), true)){
        printf("Failed to load first toilet obj file\n");
        exit(1);
    }
    if(!loadErr.empty()){
        std::cerr << loadErr << std::endl;
        loadErr = "";
    }
    if(!LoadObj(shapesToiletp1, materialsToiletp1, loadErr, toiletp1.c_str(), toiletBasePath.c_str(), true)){
        printf("Failed to load second toilet obj file\n");
        exit(1);
    }
    if(!loadErr.empty()){
        std::cerr << loadErr << std::endl;
        loadErr = "";
    }
    if(!LoadObj(shapesToiletp2, materialsToiletp2, loadErr, toiletp2.c_str(), toiletBasePath.c_str(), true)){
        printf("Failed to load third toilet obj file\n");
        exit(1);
    }
    if(!loadErr.empty()){
        std::cerr << loadErr << std::endl;
        loadErr = "";
    }
    if(!LoadObj(shapesMegaBuster, materialsMegaBuster, loadErr, megaBuster.c_str(), megaBusterBasePath.c_str(), true)){
        printf("Failed to load the megaBuster obj file\n");
        exit(1);
    }
    if(!loadErr.empty()){
        std::cerr << loadErr << std::endl;
        loadErr = "";
    }
    if(!LoadObj(shapesSpaceCraft, materialsSpaceCraft, loadErr, spaceCraft.c_str(), spaceCraftBasePath.c_str(), true)){
        printf("Failed to load the space craft obj file\n");
        exit(1);
    }
    if(!loadErr.empty()){
        std::cerr << loadErr << std::endl;
        loadErr = "";
    }

    //In our case each model there is only 1 shape and 1 material, so we can reduce the amount of code with this assumption
    createObjVAO(shapesToiletp0[0].mesh.positions, shapesToiletp0[0].mesh.normals, shapesToiletp0[0].mesh.texcoords, shapesToiletp0[0].mesh.indices, toilet0VaoHandle);
    createObjVAO(shapesToiletp1[0].mesh.positions, shapesToiletp1[0].mesh.normals, shapesToiletp1[0].mesh.texcoords, shapesToiletp1[0].mesh.indices, toilet1VaoHandle);
    createObjVAO(shapesToiletp2[0].mesh.positions, shapesToiletp2[0].mesh.normals, shapesToiletp2[0].mesh.texcoords, shapesToiletp2[0].mesh.indices, toilet2VaoHandle);
    createObjVAO(shapesMegaBuster[0].mesh.positions, shapesMegaBuster[0].mesh.normals, shapesMegaBuster[0].mesh.texcoords, shapesMegaBuster[0].mesh.indices, megaBusterVaoHandle);
    createObjVAO(shapesSpaceCraft[0].mesh.positions, shapesSpaceCraft[0].mesh.normals, shapesSpaceCraft[0].mesh.texcoords, shapesSpaceCraft[0].mesh.indices, spaceCraftVaoHandle);

    //Our transparent spinning cube defaults to the last texture for some reason, so we must set these all before it
    int retSum = 0;
    retSum += loadTexture(toiletBasePath + materialsToiletp0[0].diffuse_texname, 5);
    retSum += loadTexture(toiletBasePath + materialsToiletp1[0].diffuse_texname, 6);
    retSum += loadTexture(toiletBasePath + materialsToiletp2[0].diffuse_texname, 7);
    retSum += loadTexture(megaBusterBasePath + materialsMegaBuster[0].diffuse_texname, 8);
    retSum += loadTexture(spaceCraftBasePath + materialsSpaceCraft[0].diffuse_texname, 9);
    if(retSum != 0){
        printf("RetSum isn't zero: %d\n", retSum);
        exit(1);
    }
}

int createSlot(){
	glActiveTexture(GL_TEXTURE0);  //Texture slot zero

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glGenTextures(NUM_TEXTURES, TexID); //Basically telling it we'll use 10 textures and our array of unsigned ints (The textures)
    
    return 0;
}

int setProjection(){
    // Perspective projection matrix
    glm::mat4 projection;
    projection = glm::perspective(FOV, double(winX) / double(winY), 0.001, 30.0);

	// Load it to the shader program
    int projHandleMain = glGetUniformLocation(ProgramIDs[0], "projection_matrix");
    int projHandleSkybox = glGetUniformLocation(ProgramIDs[1], "projection_matrix");
	if(projHandleMain == -1 || projHandleSkybox == -1){
        fprintf(stderr, "Error updating proj matrix\n");
        return 1;
    }
    glUseProgram(ProgramIDs[0]);
    glUniformMatrix4fv(projHandleMain, 1, false, glm::value_ptr(projection));
    glUseProgram(ProgramIDs[1]);
    glUniformMatrix4fv(projHandleSkybox, 1, false, glm::value_ptr(projection));

    return 0;
}

//The recoil animation. Two movements, recoil & reposition. Both are linear, but recoil has larger gradient than reposition. Takes "speed" seconds
float sawtoothWave(){
    float res = 0.0f;
    float speed = 0.5f;
    float currentTime = glfwGetTime() - fireStart;  //This will give us the time since the shot was fired in seconds
    currentTime *= 10;

    //End of the animation
    if(currentTime > speed * 10 || !firing){
        firing = false;
        return 0;
    }

    //First 1/5 of the animation is recoiling back, last 4/5 of it is moving back into position
    if(currentTime <= speed * 2){
        //calculate quick recoil based on currentTime, remember backwards is negative
        res = -speed * 2 * currentTime; //Not perfect for different speeds, but we don't change speed so thats fine
    }
    else{
        //calculate the slower return motion based on currentTime, remember forwards is positive
        res = currentTime / (speed * 10) - (10/9);
        
        //Shouldn't need to be triggered, but here just incase
        if(res > 0){
            res = 0;
        }
    }
    return res;
}

void renderMegaBuster(){
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(ProgramIDs[0]);

    glBindTexture(GL_TEXTURE_2D, TexID[8]);

    int texHandle = glGetUniformLocation(ProgramIDs[0], "texMap");
    int modelHandle = glGetUniformLocation(ProgramIDs[0], "model_matrix");
    int viewHandle = glGetUniformLocation(ProgramIDs[0], "view_matrix");
    int procHandle = glGetUniformLocation(ProgramIDs[0], "procTex");

    if(viewHandle == -1 || modelHandle == -1 || texHandle == -1 || procHandle == -1){
        fprintf(stderr, "Could not find texMap, model_matrix or view_matrix variables\n");
        exit(1);
    }

    glUniform1i(procHandle, 0);

    glm::mat4 viewMat = glm::mat4();
    glm::mat4 modelMat = glm::mat4();

    viewMat = glm::translate(viewMat, glm::vec3(1, -1.5, -2.5));    //Move it into position (x to the right, y moves it away and z moves it down)
    modelMat = glm::rotate(modelMat, (float)(-0.2), glm::vec3(0, 0, 1));    //These x and z rots make it face inwards (Kinda like a torch light)
    modelMat = glm::rotate(modelMat, (float)(0.2), glm::vec3(1, 0, 0));
    modelMat = glm::rotate(modelMat, (float)(M_PI * 11 / 16), glm::vec3(0, 1, 0));  //Make the blaster face left and to the foreground
    modelMat = glm::translate(modelMat, glm::vec3(sawtoothWave(), 0, 0));    //The recoil animation
    modelMat = glm::scale(modelMat, glm::vec3(20, 20, 20));
    
    // Set VAO to the model and draw
    glBindVertexArray(megaBusterVaoHandle);

    // We are using texture unit 0 (the default). This tells the fragment shader which texture we are using
    glUniform1i(texHandle, 0);

    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));   // Set the view matrix
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));   // Set the model matrix
    glDrawElements(GL_TRIANGLES, shapesMegaBuster[0].mesh.indices.size() * 3, GL_UNSIGNED_INT, 0);  // Draw the object
}

void renderToilet(){
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(ProgramIDs[0]);
    int texHandle = glGetUniformLocation(ProgramIDs[0], "texMap");
    int modelHandle = glGetUniformLocation(ProgramIDs[0], "model_matrix");
    int viewHandle = glGetUniformLocation(ProgramIDs[0], "view_matrix");
    int procHandle = glGetUniformLocation(ProgramIDs[0], "procTex");

    if(viewHandle == -1 || modelHandle == -1 || texHandle == -1 || procHandle == -1){
        fprintf(stderr, "Could not find texMap, model_matrix or view_matrix variables\n");
        exit(1);
    }

    glUniform1i(procHandle, 0);

    glm::mat4 viewMat = glm::mat4();
    glm::mat4 modelMat = glm::mat4();

    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f)); //Adjust for player viewing
    viewMat = glm::rotate(viewMat, playerHorizontalRot , glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - 9, -0.5 - playerYPos, playerZPos - 8.5));
    modelMat = glm::rotate(modelMat, (float)(M_PI / 2), glm::vec3(0, 1, 0));
    modelMat = glm::scale(modelMat, glm::vec3(0.15, 0.15, 0.15));
    
    glBindTexture(GL_TEXTURE_2D, TexID[5]);
    glBindVertexArray(toilet0VaoHandle);
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));   // Set the view matrix
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));   // Set the model matrix
    glDrawElements(GL_TRIANGLES, shapesToiletp0[0].mesh.indices.size() * 3, GL_UNSIGNED_INT, 0);  // Draw the toilet

    //Since we only need to change the view matrix, we can recycle the old model matrix
    viewMat = glm::mat4();
    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMat = glm::rotate(viewMat, playerHorizontalRot , glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - 9, -0.275 -playerYPos, playerZPos - 8.3));

    glBindTexture(GL_TEXTURE_2D, TexID[6]);
    glBindVertexArray(toilet1VaoHandle);
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, shapesToiletp1[0].mesh.indices.size() * 3, GL_UNSIGNED_INT, 0);  // Draw the toilet part 1

    viewMat = glm::mat4();
    viewMat = glm::mat4();
    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMat = glm::rotate(viewMat, playerHorizontalRot , glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - 9, -0.27 -playerYPos, playerZPos - 8.3));

    glBindTexture(GL_TEXTURE_2D, TexID[7]);
    glBindVertexArray(toilet2VaoHandle);
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, shapesToiletp2[0].mesh.indices.size() * 3, GL_UNSIGNED_INT, 0);  // Draw the toilet part 2
}

void renderSpaceCraft(){
    glActiveTexture(GL_TEXTURE0);

    glUseProgram(ProgramIDs[0]);

    glBindTexture(GL_TEXTURE_2D, TexID[9]);

    int texHandle = glGetUniformLocation(ProgramIDs[0], "texMap");
    int modelHandle = glGetUniformLocation(ProgramIDs[0], "model_matrix");
    int viewHandle = glGetUniformLocation(ProgramIDs[0], "view_matrix");
    int procHandle = glGetUniformLocation(ProgramIDs[0], "procTex");

    if(viewHandle == -1 || modelHandle == -1 || texHandle == -1 || procHandle == -1){
        fprintf(stderr, "Could not find texMap, model_matrix or view_matrix variables\n");
        exit(1);
    }

    glUniform1i(procHandle, 0);

    glm::mat4 viewMat = glm::mat4();
    glm::mat4 modelMat = glm::mat4();

    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f)); //Adjust for player viewing
    viewMat = glm::rotate(viewMat, playerHorizontalRot + (float)(0.1 * exitSpin), glm::vec3(0.0f, 1.0f, 0.0f));  //Make it orbit around the center of the maze and adjust for playering viewing
    viewMat = glm::translate(viewMat, glm::vec3(4.5f, 10.0f - playerYPos, -15.5f));  //Move it about 20 units from center of maze
    modelMat = glm::rotate(modelMat, (float)(9 * M_PI / 4), glm::vec3(0.0f, 1.0f, 0.0f));   //Rotates the model, this helps for orbitting around the center of the map
    modelMat = glm::rotate(modelMat, (float)((M_PI/5) + glm::sin(0.7 * exitSpin) * 0.2), glm::vec3(0.0f, 0.0f, 1.0f));  //Ship rolls from side to side abit
    modelMat = glm::rotate(modelMat, (float)(0.1), glm::vec3(1.0f, 0.0f, 0.0f));    //Tilt the nose up abit cause it looked weird
    
    glBindVertexArray(spaceCraftVaoHandle);
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, shapesSpaceCraft[0].mesh.indices.size() * 3, GL_UNSIGNED_INT, 0);
}

void renderExitCube(int texHandle, int modelHandle, int viewHandle, int i, int j, int mode){
    if(mode == 1){
        glBindTexture(GL_TEXTURE_2D, TexID[2]);
    }
    else{
        glBindTexture(GL_TEXTURE_2D, TexID[4]);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glm::mat4 viewMat = glm::mat4();
    glm::mat4 modelMat = glm::mat4();

    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(i), 1.0f + (0.5 * sin(exitSpin)) - playerYPos, playerZPos - (float)(j)));
    modelMat = glm::rotate(modelMat, exitSpin, glm::vec3(0.0f, 1.0f, 0.0f));   //Makes the exit signs spin around the point between the 2 signs
    modelMat = glm::translate(modelMat, glm::vec3(0.3f * -mode, 0.0f, 0.0f));  //Split the 2 cubes apart
    modelMat = glm::rotate(modelMat, exitSpin, glm::vec3(0.0f, 1.0f, 0.0f));   //Makes the exit sign spin on the spot (model matrix)
    modelMat = glm::scale(modelMat, glm::vec3(0.25f, 0.25f, 0.25f));    //This is in model (View stuff done first, model stuff done after)
    
    glBindVertexArray(my_models.getVaoHandle("cube"));
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("cube") * 3, GL_UNSIGNED_INT, 0);
}

void renderProcTex(int texHandle, int modelHandle, int viewHandle, int procHandle, int timeHandle, int i, int j){
    glUniform1i(procHandle, 1);
    glm::mat4 viewMat = glm::mat4();
    glm::mat4 modelMat = glm::mat4();

    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(i), 0.0f - playerYPos, playerZPos - (float)(j)));
    
    glBindVertexArray(my_models.getVaoHandle("frame"));
    glUniform1f(timeHandle, glfwGetTime());
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("frame") * 3, GL_UNSIGNED_INT, 0);
    glUniform1i(procHandle, 0);
}

void renderMaze(){
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(ProgramIDs[0]);

    int texHandle = glGetUniformLocation(ProgramIDs[0], "texMap");
    int modelHandle = glGetUniformLocation(ProgramIDs[0], "model_matrix");
    int viewHandle = glGetUniformLocation(ProgramIDs[0], "view_matrix");
    int procHandle = glGetUniformLocation(ProgramIDs[0], "procTex");
    int timeHandle = glGetUniformLocation(ProgramIDs[0], "time");

    if(viewHandle == -1 || modelHandle == -1 || texHandle == -1 || procHandle == -1 || timeHandle == -1){
        fprintf(stderr, "Could not find texMap, model_matrix or view_matrix variables\n");
        exit(1);
    }

    glUniform1i(procHandle, 0);
    
    // We reset the camera for this frame
    glm::mat4 viewMat; //Change to view matrix
    glm::mat4 modelMat;
    
    //Loops over all of maze drawing each box/wall
    for(int j = 0; j < mazeSize; j++){
        for(int i = 0; i < mazeSize; i++){
            if(maze[i][j] == 0){    //Draw the wall
                glBindTexture(GL_TEXTURE_2D, TexID[0]);

                viewMat = glm::mat4();
                modelMat = glm::mat4();

                viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
                viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
                viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(i), 0.0f - playerYPos, playerZPos - (float)(j))); //z is column(i), x is row(j)
                
                glBindVertexArray(my_models.getVaoHandle("cube"));
                glUniform1i(texHandle, 0);
                glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
                glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
                glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("cube") * 3, GL_UNSIGNED_INT, 0);
            }
            if(maze[i][j] == 0 || maze[i][j] == 4){ //Draw the metal floor
                glBindTexture(GL_TEXTURE_2D, TexID[3]);

                viewMat = glm::mat4();
                modelMat = glm::mat4();

                viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
                viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
                viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(i), - 1.0f - playerYPos, playerZPos - (float)(j)));
                
                glBindVertexArray(my_models.getVaoHandle("cube"));
                glUniform1i(texHandle, 0);
                glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
                glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
                glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("cube") * 3, GL_UNSIGNED_INT, 0);
            }
            else if(maze[i][j] == 1){   //Draw main floor
                glBindTexture(GL_TEXTURE_2D, TexID[1]);

                viewMat = glm::mat4();
                modelMat = glm::mat4();

                viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
                viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
                viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(i), - 1.0f - playerYPos, playerZPos - (float)(j)));
                
                glBindVertexArray(my_models.getVaoHandle("cube"));
                glUniform1i(texHandle, 0);
                glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
                glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
                glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("cube") * 3, GL_UNSIGNED_INT, 0);
            }
        }
    }

    //The block that moves on Y axis
    glBindTexture(GL_TEXTURE_2D, TexID[3]);

    viewMat = glm::mat4();
    modelMat = glm::mat4();

    viewMat = glm::rotate(viewMat, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMat = glm::rotate(viewMat, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
    viewMat = glm::translate(viewMat, glm::vec3(playerXPos - (float)(9), 0.5 * sin(exitSpin)- playerYPos - 0.5f, playerZPos - (float)(2)));
    
    glBindVertexArray(my_models.getVaoHandle("cube"));
    glUniform1i(texHandle, 0);
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMat));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMat));
    glDrawElements(GL_TRIANGLES, my_models.getIndexTriSize("cube") * 3, GL_UNSIGNED_INT, 0);

    //Render the procedural texture that changes with time
    renderProcTex(texHandle, modelHandle, viewHandle, procHandle, timeHandle, 1, 7);

    //In my code, the transparent objects MUST be done last because they need other object info
    renderExitCube(texHandle, modelHandle, viewHandle, 1, 7, 1);
    renderExitCube(texHandle, modelHandle, viewHandle, 1, 7, -1);
}

//RenderDoc can help with debugging
void renderSkybox(){
    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix;

    glActiveTexture(GL_TEXTURE0);

    // Draw a big cube surrounding us, textured with cube map
    glDepthFunc(GL_LEQUAL);
    glUseProgram(ProgramIDs[1]);

    int viewHandle = glGetUniformLocation(ProgramIDs[1], "view_matrix");
    int modelHandle = glGetUniformLocation(ProgramIDs[1], "model_matrix");
    if(viewHandle == -1 || modelHandle == -1){
        std::cout << "Uniform: model or view is not an active uniform label\n";
        exit(1);
    }

    viewMatrix = glm::rotate(viewMatrix, playerVerticalRot, glm::vec3(1.0f, 0.0f, 0.0f));
    viewMatrix = glm::rotate(viewMatrix, playerHorizontalRot, glm::vec3(0.0f, 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(10.0f, 10.0f, 10.0f));  //Scale the cube to be outside the camera 
        
    glUniformMatrix4fv(viewHandle, 1, false, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(modelHandle, 1, false, glm::value_ptr(modelMatrix));

    // Draw the skybox
    glBindVertexArray(my_models.getVaoHandle("sky"));
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexHandle);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

//Load and store the maze file 0 is wall, 1 is bump-mapped floor, 3 is void, 4 is exterior/metal floor
void loadMazeFile(){
    mazeSize = 10;

    // Allocating space for the maze
    maze = new int*[mazeSize];
    for(int i = 0; i < mazeSize; ++i){
        maze[i] = new int[mazeSize];
    }

    for(int i = 0; i < 10; i++){
        for(int j = 0; j < 10; j++){
            if(i == 9 || j == 9){
                maze[j][i] = 3;
                continue;
            }
            if((i != 9 && j != 9) && (i == 0 || j == 8 || i == 8 || j == 0)){
                maze[j][i] = 0;
            }
            else{
                maze[j][i] = 1;
            }
        }
    }

    //Add the exit hole and the exit path and the bit beyond the lift
    maze[8][1] = 4;
    maze[9][1] = 4;
    for(int i = 0; i < 6; i++){
        maze[9][3 + i] = 4;
    }

    //Add the mazy part of the maze
    maze[1][4] = 0;
    maze[2][2] = 0;
    maze[2][4] = 0;
    maze[3][1] = 0;
    maze[3][6] = 0;
    maze[4][2] = 0;
    maze[4][3] = 0;
    maze[4][4] = 0;
    maze[4][5] = 0;
    maze[4][6] = 0;
    maze[5][4] = 0;
    maze[6][2] = 0;
    maze[6][6] = 0;
    maze[7][2] = 0;
    maze[7][3] = 0;
    maze[7][5] = 0;
}

//Sets the resolution to either the size of the primary screen or whats described in the config file
void setResolution(){
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    std::ifstream config;
    config.open("config");
    if(!config){
        printf("Config file not found\n");
        winX = mode->width;
        winY = mode->height;
        config.close();
        return;
    }
    else{   //No config file, just fullscreen instead
        printf("Config file found\n");
    }
    std::string name;
    std::string value;
    while(config >> name >> value){
        if(name.compare("Width:")){
            if(!std::all_of(value.begin(), value.end(), ::isdigit)){
                winX = mode->width;
                continue;
            }
            winX = atoi(value.c_str());
            if(winX < 1){
                winX = mode->width;
            }
        }
        else if(name.compare("Height:")){
            if(!std::all_of(value.begin(), value.end(), ::isdigit)){
                winY = mode->height;
                continue;
            }
            winY = atoi(value.c_str());
            if(winY < 1){
                winY = mode->height;
            }
        }
    }
    config.close();
    return;
}

void move(){
    float pX = playerXPos;
    float pY = playerYPos;
    float pZ = playerZPos;
    if(joystickPresent && inputMode){
        float controlX = axes[0];
        float controlY = axes[1];
        controlY *= -1;

        float angle;
        angle = glm::acos((controlY) / (sqrt((controlX * controlX) + (controlY * controlY))));
        if(pow(controlX,2) == pow(controlY,2)){ //Divide by zero errors from calculating the ratio
            if(controlX > 0 && controlY > 0){
                angle = M_PI / 4;
            }
            if(controlX > 0 && controlY < 0){
                angle = 3 * M_PI / 4;
            }
            if(controlX < 0 && controlY < 0){
                angle = 5 * M_PI / 4;
            }
            if(controlX < 0 && controlY > 0){
                angle = 7 * M_PI / 4;
            }
        }
        if(controlX < 0){   //Obtuse angles
            angle = (2 * M_PI) - angle;
        }

        if(angle != angle){ //Detect nan and correct it
            if(controlY < 0){
                angle = M_PI;
            }
            if(controlY > 0){
                angle = 0;
            }
        }

        //If the joystick isn't in the deadspace, then calculate new x and z
        if(!(controlX < 0.4 && controlX > -0.4 && controlY < 0.4 && controlY > -0.4)){
            playerXPos += playerRunSpeed * glm::cos(playerHorizontalRot + M_PI/2 + angle);  //X depends on both axes
            playerZPos += playerRunSpeed * glm::sin(playerHorizontalRot + M_PI/2 + angle);
        }
    }
    else{
        if(heldButtons[0] && !heldButtons[2]){ //W
            playerXPos += playerRunSpeed * glm::cos(playerHorizontalRot + M_PI/2);
            playerZPos += playerRunSpeed * glm::sin(playerHorizontalRot + M_PI/2);
        }
        if(heldButtons[1] && !heldButtons[3]){ //A
            playerXPos += playerRunSpeed * glm::cos(playerHorizontalRot);
            playerZPos += playerRunSpeed * glm::sin(playerHorizontalRot);
        }
        if(heldButtons[2] && !heldButtons[0]){ //S
            playerXPos -= playerRunSpeed * glm::cos(playerHorizontalRot + M_PI/2);
            playerZPos -= playerRunSpeed * glm::sin(playerHorizontalRot + M_PI/2);
        }
        if(heldButtons[3] && !heldButtons[1]){ //D
            playerXPos -= playerRunSpeed * glm::cos(playerHorizontalRot);
            playerZPos -= playerRunSpeed * glm::sin(playerHorizontalRot);
        }
    }
    playerYPos -= 0.1;  //This is gravity, its very basic
    checkCollision(pX, pY, pZ, playerXPos, playerYPos, playerZPos, playerHorizontalRot, boxes);
    return;
}

void controller_buttons(GLFWwindow* window){
    if(buttons[6] == GLFW_PRESS){   //Back is pressed
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if(buttons[3] == GLFW_PRESS && inputMode){   //Press Y to reset
        if(!controllerPressButtons[1]){
            playerXPos = 1.0;
            playerYPos = 0.0;
            playerZPos = 1.0;
        }
        controllerPressButtons[1] = true;
    }
    else{
        controllerPressButtons[1] = false;
    }
    if(buttons[7] == GLFW_PRESS){  //Start is pressed
        if(!controllerPressButtons[0]){
            inputMode = !inputMode;
        }
        controllerPressButtons[0] = true;
    }
    else{
        controllerPressButtons[0] = false;
    }
    if(buttons[2] == GLFW_PRESS && inputMode){   //Press X to fire and do recoil animation
        if(!firing){
            firing = true;
            fireStart = glfwGetTime();
            system("aplay -q external_files/blaster.wav &");
        }
    }
}

void controller_look(){
    float centreX = winX/2;
    float centreY = winY/2;

    if(axes[4] > 0.4 || axes[4] < -0.4){
        playerVerticalRot += 0.04 * (axes[4]);
    }
    if(axes[3] > 0.4 || axes[3] < -0.4){
        playerHorizontalRot += 0.04 * (axes[3]);
    }

    //Backflip prevention code
    if(playerVerticalRot > M_PI/2){    //If beyond looking straight down, undo changes
        playerVerticalRot = M_PI/2;
    }

    if(playerVerticalRot < -M_PI/2){    //If beyond looking straight down, undo changes
        playerVerticalRot = -M_PI/2;
    }
    return;
}

void reshape_callback(GLFWwindow *window, int x, int y){
    winX = x;
    winY = y;

    setProjection();
    glViewport(0, 0, (GLsizei)x, (GLsizei)y); 
}

//This code seems right, but for some reason the keyboard behaves very strangely
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;
            case GLFW_KEY_G:    //Press G to switch between keyboard/mouse and controller controls
                inputMode = !inputMode;
                break;
            case GLFW_KEY_Y:
                if(!inputMode || !joystickPresent){  //Press Y to reset
                    playerXPos = 1.0;
                    playerYPos = 0.0;
                    playerZPos = 1.0;
                }
                break;
            default:
                break;
        }
    }

    int state = glfwGetKey(window, GLFW_KEY_X);
    if(state == GLFW_PRESS || state == GLFW_REPEAT){    //Why is it bugged? Sometimes it holds longer and fires an extra shot. The keyboard is weird
        if((!inputMode || !joystickPresent) && !firing){  //Press X to fire and do recoil animation
            firing = true;
            fireStart = glfwGetTime();
            system("aplay -q external_files/blaster.wav &");
        }
    }

    if(action == GLFW_PRESS && key == GLFW_KEY_W){
        heldButtons[0] = true;
    }
    if(action == GLFW_PRESS && key == GLFW_KEY_A){
        heldButtons[1] = true;
    }
    if(action == GLFW_PRESS && key == GLFW_KEY_S){
        heldButtons[2] = true;
    }
    if(action == GLFW_PRESS && key == GLFW_KEY_D){  //For some reason the D key is buggy
        heldButtons[3] = true;
    }

    //And releasing keys doesn't update them all at once...
    if(action == GLFW_RELEASE && key == GLFW_KEY_W){
        heldButtons[0] = false;
    }
    if(action == GLFW_RELEASE && key == GLFW_KEY_A){
        heldButtons[1] = false;
    }
    if(action == GLFW_RELEASE && key == GLFW_KEY_S){
        heldButtons[2] = false;
    }
    if(action == GLFW_RELEASE && key == GLFW_KEY_D){
        heldButtons[3] = false;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos){
    float centreX = winX/2;
    float centreY = winY/2;

    if(!joystickPresent || !inputMode){
        //Use the distance between xpos/ypos and the origin to modify the rotation

        playerVerticalRot += 0.005 * (ypos - centreY);
        playerHorizontalRot += 0.005 * (xpos - centreX);

        if(playerVerticalRot > M_PI/2){    //If beyond looking straight down, undo changes
            playerVerticalRot = M_PI/2;
        }

        if(playerVerticalRot < -M_PI/2){    //If beyond looking straight down, undo changes
            playerVerticalRot = -M_PI/2;
        }
    }

    glfwSetCursorPos(window, centreX, centreY); //Reset the cursor to the centre of the screen so we don't hit the boarders of the window
}

static void error_callback(int error, const char* description){
    fputs(description, stderr);
}

int main(int argC, char *argV[]){

	std::cout << "Controls (Keyboard/Mouse mode):" << std::endl;
    std::cout << "- Use the mouse to look around (When cursor is focused on the window)" << std::endl;
    std::cout << "- Use WASD to move forward, left, back and right respectively" << std::endl;
    std::cout << "- Press G to swap between keyboard/mouse and XBOX 360 controls" << std::endl;
    std::cout << "- Press ESC to end the program\n" << std::endl;

    std::cout << "Controls (XBOX 360 controller):" << std::endl;
    std::cout << "- Use the left thumbstick to move around" << std::endl;
    std::cout << "- Use the right thumbstick to look around" << std::endl;
    std::cout << "- Press the Start Button to swap between XBOX 360 and keyboard/mouse controls" << std::endl;
    std::cout << "- Press the Back button to end the program\n" << std::endl;

    std::cout << "Shared controls (Same for keyboard and controller):" << std::endl;
    std::cout << "- Press Y to reset the player's position back to the start" << std::endl;
    std::cout << "- Press X to fire the blue cannon (Which also has a sound effect)\n" << std::endl;

    std::cout << "Note that mouse/keyboard controls are locked when in XBOX 360 mode and vica versa.";
    std::cout << " However Pressing G or ESC on keyboard or Back or Start on the XBOX 360 controller always works.";
    std::cout << " Note that the controller support might not work on your system if you don't have the latest drivers\n\n";

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()){
        exit(1);
    }

    // Specify that we want OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    loadMazeFile();

    joystickPresent = glfwJoystickPresent(GLFW_JOYSTICK_1); //Detects if a Joystick is plugged in (Controls change if true)

    // Create the window and OpenGL context
    setResolution();    //Set the resolution to the screen size
    window = glfwCreateWindow(winX, winY, "Megaman and the Legend of the Dark and Dank Toilet", NULL, NULL);
    if(!window){
        glfwTerminate();
        exit(1);
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if(glewInit() != GLEW_OK){
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

    // These shaders use supplied tex coords to texture surface
	ProgramIDs[0] = LoadShaders("main_shader.vert", "main_shader.frag");
    ProgramIDs[1] = LoadShaders("skybox_shader.vert", "skybox_shader.frag");

	if(ProgramIDs[0] == 0 || ProgramIDs[1] == 0){
        fprintf(stderr, "Error loading shaders\n");        
		exit(1);
    }

    // Set up OpenGL parameters and data
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    my_models.initSky(); //Creates the skybox mesh
    createSkyMap();
    setProjection();
    my_models.initCube(ProgramIDs[0]); //Creates the cube mesh
    my_models.initFrame(ProgramIDs[0]); //Creates the frame mesh

    // Load the 3 textures to change between
    createSlot();   //Slot created after skymap is made
    loadTexture("external_files/textures/wall.png", 0);
    loadTexture("external_files/textures/floor.png", 1);
    loadTexture("myTextures/exit.png", 2);
    loadTexture("external_files/textures/metalCreate.png", 3);
    loadTexture("myTextures/exitTrans.png", 4);

    loadObjModels();

    //Set all the held/pressed buttons to zero
    for(int i = 0; i < 2; i++){
        controllerPressButtons[i] = false;
        heldButtons[2 * i] = false;
        heldButtons[2 * i + 1] = false;
    }
    heldButtons[4] = false;

    // Define callback functions and start main loop
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, reshape_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);  //This hides the mouse cursor

    //Starting coords
    playerXPos = 1;
    playerYPos = 0;
    playerZPos = 1;

    addCollisionBoxes(boxes);

    while(!glfwWindowShouldClose(window)){
        joystickPresent = glfwJoystickPresent(GLFW_JOYSTICK_1); //Detects if a Joystick is plugged in (Controls change if true)
        if(joystickPresent){
            axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
        }

        exitSpin = glfwGetTime();
        updateCollisionBox(9.5, 8.5, 0.5 * glm::sin(exitSpin), 0.5 * glm::sin(exitSpin) - 1.0, 2.5, 1.5, boxes, 0); //Update the moving block's collision
        move();

        if(joystickPresent){
            if(inputMode){
                controller_look();
            }
            buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
            controller_buttons(window);
        }

        //Render code
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderSpaceCraft();
        renderToilet();
        renderMaze();
        renderSkybox();

        glClear(GL_DEPTH_BUFFER_BIT);   //Need to reset the depth buffer for HUD stuff like the blaster
        renderMegaBuster();

        glBindVertexArray(0);
        glFlush();

        //Swap buffers and get inputs
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();

    //Delete the store matrix
    for(int i = 0; i < mazeSize; ++i){
        delete [] maze[i];
    }
    delete [] maze;
    
	return 0;
}
