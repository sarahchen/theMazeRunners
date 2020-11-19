/*
FINAL PROJECT
Character.h
Created by Ryder Babik
11/9/20
Engineering Computation 24780-B
header file for creating a character object in the game
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

class Character {
private:
	//window width and height
	int width, height;

	characterFeatures thisCharacter;
	vertex initPos, posCenter;
	double velocity;

	double headingAngle;
	bool gameOver;
	int livesRemaining, score;
	double energyLevel;

	int Force, mass;
	double accel, mu;
	double ax, ay, bx, by, cx, cy, dx, dy;  //car edges

	GLuint textID;

public:
	//class constructor
	Character(int xpos, int ypos, GLuint Id);

	// get and set for all member variables
	string getLabel() { return thisCharacter.label; }
	double getX() { return posCenter.x; }
	double getY() { return posCenter.y; }
	double getZ() { return posCenter.z; }
	double getVel() { return velocity; }
	double getHei() { return thisCharacter.hei; }
	double getWid() { return thisCharacter.wid; }
	double getDep() { return thisCharacter.dep; }
	bool characterIsAlive() { return !gameOver; }

	void setLabel(const std::string& newLabel) { thisCharacter.label = newLabel; }
	void setX(double xCoord) { posCenter.x = xCoord; }
	void setY(double yCoord) { posCenter.y = yCoord; }
	void setZ(double zCoord) { posCenter.z = zCoord; }
	void setVel(double vel) { velocity = vel; }
	void setHei(double height) { thisCharacter.hei = height; }
	void setWid(double width) { thisCharacter.wid = width; }
	void setDep(double depth) { thisCharacter.dep = depth; }
	
	void setForce(double force) { Force = force; }
	void adjustOrientation(double angleAdjust); 
	void adjustForce(int forceAdjust) { Force += forceAdjust; }
	void adjustAcceleration(double acceleration) { accel += acceleration; }

	bool isValidMove();
	bool isValidOrientation();
	void checkMove();

	void calcEdges();

	bool isLastLife();
	void decrementLives();

	void initialize();
	void reset();
	void updateKinematics(double deltaT);

	//draw character in new location and orientation
	void draw(ViewManager& theManager);
};