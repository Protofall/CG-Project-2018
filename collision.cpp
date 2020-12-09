#include "collision.hpp"

//Used to calculate falling time, not sure if checking time or distance is better
float fallTime = 0.0f;
bool falling = false;
bool onWall = false;

//This is to help with moving from block to block and also when walking on corner (But mostly neighbouring blocks)
//It returns true or false depending if an x or z coordinate was reset. Used for hitting wall sound effects
extern bool cornerCollision(float pX, float pZ, float &playerXPos, float &playerZPos, float playerYRot, std::vector<float> &boxes, int i, float playerRadius){
	float niceYRot = fmod(playerYRot, 2 * M_PI);
	bool ret = false;
	if(pX < boxes[i + 3] && pZ > boxes[i + 2]){	//Top right
		if(niceYRot > M_PI * (5/4)){	//Looking more west wards
			//Reset Z
			playerZPos = boxes[i + 2] + playerRadius;
			ret = true;
		}
		else{	//South
			playerXPos = boxes[i + 3] - playerRadius;
			ret = true;
		}
	}
	else if(pX < boxes[i + 3] && pZ < boxes[i + 2]){	//Bottom right
		if(niceYRot > M_PI * (7/4)){	//Looking more North wards
			//Reset X
			playerXPos = boxes[i + 3] - playerRadius;
			ret = true;
		}
		else{	//West
			playerZPos = boxes[i + 5] - playerRadius;
			ret = true;
		}
	}
	else if(pX > boxes[i + 3] && pZ > boxes[i + 2]){	//Bottom left
		if(niceYRot > M_PI * (1/4)){	//Looking more East wards
			//Reset Z
			playerZPos = boxes[i + 5] - playerRadius;
			ret = true;
		}
		else{	//North
			playerXPos = boxes[i] + playerRadius;
			ret = true;
		}
	}
	else if(pX > boxes[i + 3] && pZ < boxes[i + 2]){	//Top left
		if(niceYRot > M_PI * (3/4)){	//Looking more south wards
			//Reset X
			playerXPos = boxes[i] + playerRadius;
			ret = true;
		}
		else{	//East
			playerZPos = boxes[i + 2] + playerRadius;
			ret = true;
		}
	}
	return ret;
}

//Note, this only works for meshes that are at 90 degree angles, examples like below can't be detected
/*

\    /
 \  /
  \/X

*/
extern void checkCollision(float pX, float pY, float pZ, float &playerXPos, float &playerYPos, float &playerZPos, float playerYRot, std::vector<float> &boxes){
	float playerRadius = 0.22;
	float playerHeight = 0.80;	//Note that the y is +0.5 what it is on in the collision realm, we account for this in calculations
	bool onGround = false;
	bool hitWall = false;

	for(int i = 0; i < boxes.size(); i = i + 6){
		//This detects if you're inside the box with the new pos (X and Z)
		bool checkX = playerXPos - playerRadius < boxes[i] && playerXPos + playerRadius > boxes[i + 3];	//Checks if you're in the X band ( || )
		bool checkZ = playerZPos - playerRadius < boxes[i + 2] && playerZPos + playerRadius > boxes[i + 5];	//Checks if you're in the Z band ( == )

		//Note: I'm using checkY = y12 >= y21 && y22 > y11 because I want you to be able to stand on a block
		bool checkY = (playerYPos - 0.5 + playerHeight) >= boxes[i + 4] && boxes[i + 1] > (playerYPos - 0.5);
		bool checkOldY = (pY - 0.5 + playerHeight) >= boxes[i + 4] && boxes[i + 1] > (pY - 0.5);	//Check if you were in the same Y as the box

		//Allow you to step over blocks that are 0.25 heigher than your feet
		if(checkY && checkX && checkZ && boxes[i + 1] - (playerYPos - 0.5) <= 0.25){
			onGround = true;
			playerYPos = 0.5 + boxes[i + 1];
			continue;
		}

		//If we're inside a wall
		if(checkX && checkY && checkZ){
			bool checkOldX = pX - playerRadius < boxes[i] && pX + playerRadius > boxes[i + 3];
			bool checkOldZ = pZ - playerRadius < boxes[i + 2] && pZ + playerRadius > boxes[i + 5];

			if(!checkOldX && !checkOldZ){
				if(cornerCollision(pX, pZ, playerXPos, playerZPos, playerYRot, boxes, i, playerRadius)){
					hitWall = true;
				}
				continue;	//This continue is important
			}

			if(!checkOldX){
				if(playerXPos > boxes[i]){	//Push the player outside the wall, this way even if the player is stuck in a wall, they should hopefully be pushed out
					playerXPos = boxes[i] + playerRadius;
					hitWall = true;
				}
				else{
					playerXPos = boxes[i + 3] - playerRadius;
					hitWall = true;
				}
			}
			if(!checkOldZ){
				if(playerZPos > boxes[i + 2]){
					playerZPos = boxes[i + 2] + playerRadius;
					hitWall = true;
				}
				else{
					playerZPos = boxes[i + 5] - playerRadius;
					hitWall = true;
				}
			}

			//If we just sunk into the floor, then update Y
			if(!(checkOldX && checkOldY && checkOldZ) && (checkX && checkY && checkZ) && pY < boxes[i + 4] && (pX != playerXPos && pZ != playerZPos)){
				onGround = true;
				playerYPos = boxes[i + 1];
			}
		}
	}

	if(hitWall && !onWall){
		onWall = true;
		system("aplay -q external_files/wallCollision.wav &");
	}

	if(!hitWall){
		onWall = false;
	}

	//Triggered the second we start falling
	if(!onGround && !falling){
		falling = true;
		fallTime = glfwGetTime();
	}

	//This ends the falling state
	if(onGround && falling){
		falling = false;
		float totalFallTime = glfwGetTime() - fallTime;

		//If you fall for more than 0.12 seconds (Which is around one block), play fall sound
		if(totalFallTime > 0.12){
			system("aplay -q external_files/landingHurt.wav &");
		}
	}

	return;
}

//Note blah1 >= blah2 and i is the ith collision box starting at index 0
extern void updateCollisionBox(float x1, float x2, float y1, float y2, float z1, float z2, std::vector<float> &boxes, int i){
	boxes[(6 * i) + 0] = x1;
	boxes[(6 * i) + 1] = y1;
	boxes[(6 * i) + 2] = z1;
	boxes[(6 * i) + 3] = x2;
	boxes[(6 * i) + 4] = y2;
	boxes[(6 * i) + 5] = z2;
	return;
}

//Boxes will be in format "x1, y1, z1, x2, y2, z2" and repeat (World coords) (Player starts at 1,0,1)
//Note due to how the collision code is done, the 1 set must all be greater than the 2 set
extern void addCollisionBoxes(std::vector<float> &boxes){

	//Moving platform
	boxes.push_back(9.5);
	boxes.push_back(-0.5);
	boxes.push_back(2.5);
	boxes.push_back(8.5);
	boxes.push_back(-1.5);
	boxes.push_back(1.5);

	//Maze Southern wall
	boxes.push_back(8.5);
	boxes.push_back(0.5);
	boxes.push_back(0.5);
	boxes.push_back(0.5);
	boxes.push_back(-1.5);
	boxes.push_back(-0.5);

	//Maze Western wall
	boxes.push_back(8.5);
	boxes.push_back(0.5);
	boxes.push_back(7.5);
	boxes.push_back(7.5);
	boxes.push_back(-1.5);
	boxes.push_back(2.5);

	//Maze Northern wall
	boxes.push_back(8.5);
	boxes.push_back(0.5);
	boxes.push_back(8.5);
	boxes.push_back(-0.5);
	boxes.push_back(-1.5);
	boxes.push_back(7.5);

	//Maze Eastern wall
	boxes.push_back(0.5);
	boxes.push_back(0.5);
	boxes.push_back(8.5);
	boxes.push_back(-0.5);
	boxes.push_back(-1.5);
	boxes.push_back(-0.5);

	//The exit hole's frame (0.1 units high)
	boxes.push_back(1.5);
	boxes.push_back(-0.4);
	boxes.push_back(7.5);
	boxes.push_back(0.5);
	boxes.push_back(-0.5);
	boxes.push_back(6.5);

	/*
	//The floors (Simplified)

	4   2 2 2 X
	4   1 1 1 1
	L   1 1 1 1
	3 3 1 1 1 1

	//Where X is the exit hole, 1 is the 1st floor, 2 is the 2nd floor and 3 is the exit to the maze floor,
	//4 is the floor beyond the lift and L is the lift (Lift collision is the first box in this vector)
	//Note: Depending on maze layout we might be able to use less blocks
	*/

	//Floor 1
	boxes.push_back(7.5);
	boxes.push_back(-0.5);
	boxes.push_back(6.5);
	boxes.push_back(0.5);
	boxes.push_back(-1.5);
	boxes.push_back(0.5);

	//Floor 2
	boxes.push_back(7.5);
	boxes.push_back(-0.5);
	boxes.push_back(7.5);
	boxes.push_back(1.5);
	boxes.push_back(-1.5);
	boxes.push_back(6.5);

	//Floor 3
	boxes.push_back(9.5);
	boxes.push_back(-0.5);
	boxes.push_back(1.5);
	boxes.push_back(7.5);
	boxes.push_back(-1.5);
	boxes.push_back(0.5);

	//Floor 4
	boxes.push_back(9.5);
	boxes.push_back(-0.5);
	boxes.push_back(8.5);
	boxes.push_back(8.5);
	boxes.push_back(-1.5);
	boxes.push_back(2.5);

	//Internal wall 1
	boxes.push_back(2.5);
	boxes.push_back(0.5);
	boxes.push_back(2.5);
	boxes.push_back(1.5);
	boxes.push_back(-0.5);
	boxes.push_back(1.5);

	//Internal wall 2
	boxes.push_back(2.5);
	boxes.push_back(0.5);
	boxes.push_back(4.5);
	boxes.push_back(0.5);
	boxes.push_back(-0.5);
	boxes.push_back(3.5);

	//Internal wall 3
	boxes.push_back(3.5);
	boxes.push_back(0.5);
	boxes.push_back(1.5);
	boxes.push_back(2.5);
	boxes.push_back(-0.5);
	boxes.push_back(0.5);

	//Internal wall 4
	boxes.push_back(4.5);
	boxes.push_back(0.5);
	boxes.push_back(6.5);
	boxes.push_back(3.5);
	boxes.push_back(-0.5);
	boxes.push_back(1.5);

	//Internal wall 5
	boxes.push_back(3.5);
	boxes.push_back(0.5);
	boxes.push_back(6.5);
	boxes.push_back(2.5);
	boxes.push_back(-0.5);
	boxes.push_back(5.5);

	//Internal wall 6
	boxes.push_back(5.5);
	boxes.push_back(0.5);
	boxes.push_back(4.5);
	boxes.push_back(4.5);
	boxes.push_back(-0.5);
	boxes.push_back(3.5);

	//Internal wall 7
	boxes.push_back(6.5);
	boxes.push_back(0.5);
	boxes.push_back(6.5);
	boxes.push_back(5.5);
	boxes.push_back(-0.5);
	boxes.push_back(5.5);

	//Internal wall 8
	boxes.push_back(7.5);
	boxes.push_back(0.5);
	boxes.push_back(5.5);
	boxes.push_back(6.5);
	boxes.push_back(-0.5);
	boxes.push_back(4.5);

	//Internal wall 9
	boxes.push_back(7.5);
	boxes.push_back(0.5);
	boxes.push_back(3.5);
	boxes.push_back(6.5);
	boxes.push_back(-0.5);
	boxes.push_back(2.5);

	//Internal wall 10
	boxes.push_back(8.5);
	boxes.push_back(0.5);
	boxes.push_back(2.5);
	boxes.push_back(5.5);
	boxes.push_back(-0.5);
	boxes.push_back(1.5);

	//Toilet Main
	boxes.push_back(9.05);
	boxes.push_back(0.0);
	boxes.push_back(8.5);
	boxes.push_back(8.95);
	boxes.push_back(-0.5);
	boxes.push_back(8.3);

	//Toilet Back
	boxes.push_back(9.05);
	boxes.push_back(-0.25);
	boxes.push_back(8.3);
	boxes.push_back(8.95);
	boxes.push_back(-0.5);
	boxes.push_back(8.2);
}
