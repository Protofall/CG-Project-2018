#ifndef COLLISION_HP
#define COLLISION_HPP

#include <vector>
#include <iostream>
#include <math.h>       // fmod
#include <GLFW/glfw3.h>

extern bool cornerCollision(float pX, float pZ, float &playerXPos, float &playerZPos, float playerYRot, std::vector<float> &boxes, int i, float playerRadius);
extern void checkCollision(float pX, float pY, float pZ, float &playerXPos, float &playerYPos, float &playerZPos, float playerYRot ,std::vector<float> &boxes);
extern void updateCollisionBox(float x1, float x2, float y1, float y2, float z1, float z2, std::vector<float> &boxes, int i);
extern void addCollisionBoxes(std::vector<float> &boxes);

#endif
