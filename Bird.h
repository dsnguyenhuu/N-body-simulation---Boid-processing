//Dong-Son Nguyen-Huu - 40014054
//COMP 426 
//October 8, 2017
//Assignment 1

#pragma once

#include <Eigen/Dense>

class Bird {
private:
	float length = 0.005;

	static int birdID;
	int ID;
	
	float r;
	float g;
	float b;
	
	float posX;
	float posY;

	Eigen::Vector2f position;
	Eigen::Vector2f velocity;

	bool moveLeft = true;
	bool moveDown = true;
	
	Eigen::Vector2f nextPosition;
public:
	Bird(float re, float gr, float bl);

	const Eigen::Vector2f &getPosition() const;
	void setPosition(const Eigen::Vector2f &position);
	
	const Eigen::Vector2f &getVelocity() const;
	void setVelocity(const Eigen::Vector2f &velocity);

	const Eigen::Vector2f &getNextPosition() const;
	void setNextPosition(const Eigen::Vector2f &nextPosition);

	
	void drawBird();
};


