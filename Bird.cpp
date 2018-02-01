//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1

#include "Bird.h"
#include <Eigen/Dense>
#include <GLFW/glfw3.h>
#include <random>

//Generating a random device to make random numbers
std::random_device random;
std::mt19937 ran(random());

//Defining the static variable birdID, to be used to keep track of the bird
int Bird::birdID = 0;

//Parametrized constructor, takes 3 float values as arguments which will color the birds
Bird::Bird(float re, float gr, float bl) {
	r = re;
	g = gr;
	b = bl;

	//creating distribution objects that will return a 'float'
	std::uniform_real_distribution<float> xPos(-1, 1);
	std::uniform_real_distribution<float> yPos(-1, 1);

	//initializing the positions to the position vector
	position(0) = xPos(ran);
	position(1) = yPos(ran);

	//initializing the positions of the position values, to be used for movement of the birds
	posX = position(0);
	posY = position(1);

	//creating distribution objects that will return a 'float' 
	std::uniform_real_distribution<float> xVel(-0.1, 0.1);
	std::uniform_real_distribution<float> yVel(-0.1, 0.1);

	//initializing the velocity vector
	velocity(0) = xVel(ran);
	velocity(1) = yVel(ran);

	//initializing the bird's ID and then incrementing it for the next bird. ID's are only assigned if the bird is able to be generated successfully
	ID = birdID;
	birdID++;


}
//Getters for position, velocity, and next position
const Eigen::Vector2f &Bird::getPosition() const {
	return position;
}
const Eigen::Vector2f &Bird::getVelocity() const {
	return velocity;
}
const Eigen::Vector2f &Bird::getNextPosition() const {
	return getNextPosition();
}


//Setters for position, velocity, and next position
void Bird::setPosition(const Eigen::Vector2f &otherPosition) {
	posX = otherPosition(0);
	posY = otherPosition(1);
	position = otherPosition;
}
void Bird::setVelocity(const Eigen::Vector2f &newVelocity) {
	velocity = newVelocity;
}
void Bird::setNextPosition(const Eigen::Vector2f &oFuturePosition) {
	nextPosition = oFuturePosition;
}

//drawBird will determine if the bird needs to change direction, as well as using GLFW to draw them onto the screen
void Bird::drawBird() {
	if (moveLeft) {
		if (posX > (-1 + (length ))) {		//indicates that the bird can still move left
			posX -= velocity.norm();		//decreasing its X position
		}
		else {
			moveLeft = false;				//can't move left because the x value is smaller than -1 + the length of the bird
		}
	}
	else {
		if (posX < (1 - (length ))) {		//indicates that the bird can still move right
			posX += velocity.norm();		//increasing its Y position
		}
		else {
			moveLeft = true;				//Tells it to move left
		}
	}

	if (moveDown) {							
		if (posY > (-1 + (length ))) {		//indicates that the bird can still move down
			posY -= velocity.norm();		//decreasing its Y position
		}
		else {
			moveDown = false;				//can't move left because the Y value is smaller than -1 + the length of the bird
		}
	}
	else {
		if (posY < (1 - (length ))) {		//indicates that the bird can still move up
			posY += velocity.norm();		//increases the Y position
		}
		else {
			moveDown = true;				//signals to move down
		}
	}

	//Update the position vector
	position(0) = posX;						
	position(1) = posY;

	//Setting the colors
	glColor3f(r, g, b);
	
	//Initializing the triangle shapes (which are filled)
	glBegin(GL_TRIANGLES);

	//Initializing the 3 points of the triangle
	glVertex2f(posX, posY + (length));
	glVertex2f(posX - (length), posY - (length));
	glVertex2f(posX + (length), posY - (length));

	glEnd();
		
}


