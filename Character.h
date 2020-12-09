
/*
Maze Mania
Character.h
Created by Ryder Babik
11/19/20
Engineering Computation 24780-B
Header file for character object for game
*/
#pragma once
#include <string>
#include "fssimplewindow.h"

using namespace std;

class ViewManager;

struct vertex {
	double x, y, z;
};

struct characterFeatures {
	double hei, wid, dep;
	string label;
};

enum carType { lambo, F1, truck, regCar };

class Character {
protected:
	//window width and height
	int width, height;

	carType thisCar;

	bool willHitObstacleFront = false;
	bool willHitObstacleBack = false;

	characterFeatures thisCharacter;
	vertex initPos, posCenter;
	double velocity;

	double headingAngle;
	bool gameOver;
	double health;
	int score, livesRemaining;
	double energyLevel;

	int Force, mass, maxVel;
	double maxVelMult = 1;
	double accel, mu;
	double ax, ay, bx, by, cx, cy, dx, dy;  //car edges

	GLuint textID;

public:
	//class constructor
	Character();

	Character(int xpos, int ypos, carType car, GLuint Id);

	void setCarInfo(int xpos, int ypos, carType car, GLuint Id);
	void vehicleParams();
	void assignId(carType car, GLuint Id);

	void assignId(GLuint Id);

	// get and set for all member variables
	string getLabel() { return thisCharacter.label; }
	double getX() { return posCenter.x; }
	double getY() { return posCenter.y; }
	double getZ() { return posCenter.z; }
	double getVel() { return velocity; }
	double getHei() { return thisCharacter.hei; }
	double getWid() { return thisCharacter.wid; }
	double getDep() { return thisCharacter.dep; }
	double getAng() { return headingAngle; }
	bool characterIsAlive() { return !gameOver; }
	int getScore() { return score; }

	double getAx() { return ax; }
	double getAy() { return ay; }
	double getBx() { return bx; }
	double getBy() { return by; }
	double getCx() { return cx; }
	double getCy() { return cy; }
	double getDx() { return dx; }
	double getDy() { return dy; }
	double getHealth() { return health; }

	void setLabel(const std::string& newLabel) { thisCharacter.label = newLabel; }
	void setX(double xCoord) { posCenter.x = xCoord; }
	void setY(double yCoord) { posCenter.y = yCoord; }
	void setZ(double zCoord) { posCenter.z = zCoord; }
	void setVel(double vel) { velocity = vel; }
	void setMaxVelMult(double mult) { maxVelMult = mult; }
	void setHei(double height) { thisCharacter.hei = height; }
	void setWid(double width) { thisCharacter.wid = width; }
	void setDep(double depth) { thisCharacter.dep = depth; }
	void setScore(int newScore) { score = newScore; }
	void setHealth(int newHealth) { health = newHealth; }
	void addScore(int amount) { score += amount; }
	
	void setForce(double force) { Force = force; }
	void adjustOrientation(double angleAdjust); 
	void adjustForce(int forceAdjust) { Force += forceAdjust; }
	void adjustAcceleration(double acceleration) { accel += acceleration; }

	//these set force magnitude based on if you are trying to accelerate or brake!!
	void goForward();
	void goBackward();

	//checks to see if your move puts you in an obstacle or not
	bool isValidMove();
	bool isValidOrientation();
	void checkMove();
	
	void setObstacleFront(bool val) { willHitObstacleFront = val; }
	void setObstacleBack(bool val) { willHitObstacleBack = val; }

	//calculates the four corners of the texture ID
	void calcEdges();

	//manages life count of character
	bool isLastLife();
	void decrementLives();

	void initialize();
	void reset();

	//updates pos, vel, accel
	void updateKinematics(double deltaT);

	//draw character in new location and orientation
	void draw();
};

//public:
//	//class constructor
//	Character() {};
//	Character(int xpos, int ypos, GLuint Id);
//
//	// get and set for all member variables
//	string getLabel() { return thisCharacter.label; }
//	double getX() { return posCenter.x; }
//	double getY() { return posCenter.y; }
//	double getZ() { return posCenter.z; }
//	double getVel() { return velocity; }
//	double getHei() { return thisCharacter.hei; }
//	double getWid() { return thisCharacter.wid; }
//	double getDep() { return thisCharacter.dep; }
//	bool characterIsAlive() { return !gameOver; }
//
//	void setLabel(const std::string& newLabel) { thisCharacter.label = newLabel; }
//	void setX(double xCoord) { posCenter.x = xCoord; }
//	void setY(double yCoord) { posCenter.y = yCoord; }
//	void setZ(double zCoord) { posCenter.z = zCoord; }
//	void setVel(double vel) { velocity = vel; }
//	void setHei(double height) { thisCharacter.hei = height; }
//	void setWid(double width) { thisCharacter.wid = width; }
//	void setDep(double depth) { thisCharacter.dep = depth; }
//	
//	void setForce(double force) { Force = force; }
//	void adjustOrientation(double angleAdjust); 
//	void adjustForce(int forceAdjust) { Force += forceAdjust; }
//	void adjustAcceleration(double acceleration) { accel += acceleration; }
//
//	bool isValidMove();
//	bool isValidOrientation();
//	void checkMove();
//
//	void calcEdges();
//
//	bool isLastLife();
//	void decrementLives();
//
//	void initialize();
//	void reset();
//	void updateKinematics(double deltaT);
//
//	//draw character in new location and orientation
//	void draw(ViewManager& theManager);
//};
