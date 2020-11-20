/*
FINAL PROJECT
Character.cpp
Created by Ryder Babik
11/9/20
Engineering Computation 24780-B
class for creating a character object in the game
*/

#define _USE_MATH_DEFINES

#include <iostream>
#include <cstdlib>
#include <string>
#include <math.h>
#include <cmath>
#include <algorithm>
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "ViewManager2.h"
#include "DrawingUtilNG.h"
//#include "StringPlus.h"
#include "GraphicFont.h"
#include "yspng.h"
#include "Character.h"

const double GRAVITY = 9.80665;
const int STARTLIVES = 3;
const int FULLENERGY = 100;


Character::Character(int xpos, int ypos, GLuint Id)
{
	textID = Id;
	gameOver = false;
	livesRemaining = STARTLIVES;
	score = 0;

	Force = 0;
	mass = 800;
	accel = 0;
	mu = 0.4;

	headingAngle = M_PI_2;
	initPos.x = xpos;
	initPos.y = ypos;
	thisCharacter.hei = 60;
	thisCharacter.wid = 30;
	thisCharacter.dep = 0;

	FsGetWindowSize(width, height);

	initialize();
}

void Character::initialize()
{
	energyLevel = FULLENERGY;
	setX(initPos.x);
	setY(initPos.y);
	setZ(initPos.z);
	setVel(0);
}

void Character::reset()
{
	initialize();
}

void Character::updateKinematics(double deltaT)
{
	cout << "Force: " << Force << "    Acceleration: " << accel << "    Velocity:  "  << velocity << endl;

	if (isValidMove()) {
		posCenter.x += velocity * cos(headingAngle) * deltaT;
		posCenter.y -= velocity * sin(headingAngle) * deltaT;
		velocity += accel * deltaT;
		//if (velocity > 10) {
		//	velocity = 10;
		//	Force = 0;
		//}
		//if (velocity < -10) {
		//	velocity = -10;
		//	Force = 0;
		//}
		if (velocity > .5) {
			accel = -mu * GRAVITY + (Force / mass); 
		}
		else if (velocity < -0.5) { 
			accel = mu * GRAVITY + (Force / mass); 
		}   //change direction of friction!!
		else if (abs(velocity) <= 0.5) { 
			accel = (Force / mass); 
		}

	//	if (abs(accel) < .01) { accel = 0; }
	}

	else {
		velocity = 0;
		accel = 0;
		Force = 0;
	}
}

void Character::adjustOrientation(double angleAdjust)
{
	headingAngle += angleAdjust;
	if (!(isValidOrientation())) { headingAngle -= angleAdjust; }
}

bool Character::isValidMove()
{
	int myCornersX[] = { ax,dx, bx,cx };
	int myCornersY[] = { ay,dy, by,cy };
	//first check limits of screen
	if ((*std::min_element(myCornersX, myCornersX + 2) <= width/6 && velocity > 0) || (*std::min_element(myCornersX + 2, myCornersX + 4) <= width/6 && velocity < 0))
	{
		return false;}
	if ((*std::min_element(myCornersY, myCornersY + 2) <= height/5 && velocity > 0) || (*std::min_element(myCornersY + 2, myCornersY + 4) <= height/5 && velocity < 0))
	{return false;}

	if ((*std::max_element(myCornersX, myCornersX + 2) >= width && velocity > 0) || (*std::max_element(myCornersX + 2, myCornersX + 4) >= width && velocity < 0))
	{
		return false;
	}
	if ((*std::max_element(myCornersY, myCornersY + 2) >= height && velocity > 0) || (*std::max_element(myCornersY + 2, myCornersY + 4) >= height && velocity < 0))
	{
		return false;
	}

	else { return true; }
}

bool Character::isValidOrientation()
{
	calcEdges();
	int myCornersX[] = { ax,dx, bx,cx };
	int myCornersY[] = { ay,dy, by,cy };
	//first check limits of screen
	if ((*std::min_element(myCornersX, myCornersX + 2) <= width / 6) || (*std::min_element(myCornersX + 2, myCornersX + 4) <= width / 6))
	{
		return false;
	}
	if ((*std::min_element(myCornersY, myCornersY + 2) <= height / 5) || (*std::min_element(myCornersY + 2, myCornersY + 4) <= height / 5))
	{
		return false;
	}
	if ((*std::max_element(myCornersX, myCornersX + 2) >= width) || (*std::max_element(myCornersX + 2, myCornersX + 4) >= width))
	{
		return false;
	}
	if ((*std::max_element(myCornersY, myCornersY + 2) >= height) || (*std::max_element(myCornersY + 2, myCornersY + 4) >= height))
	{
		return false;
	}

	else { return true; }
}

bool Character::isLastLife()
{
	if (livesRemaining > 1) { return false; }
	else { return true; }
}

void Character::decrementLives()
{
	if (!isLastLife()) {
		livesRemaining -= 1;
	}
	else { gameOver = true; }
}

void Character::calcEdges()
{
	//a is front left, b is back left, c is back right, d is front right
	ax = posCenter.x + thisCharacter.wid / 2 * cos(headingAngle) - thisCharacter.wid / 2 * sin(headingAngle);
	ay = posCenter.y - thisCharacter.wid / 2 * sin(headingAngle) - thisCharacter.hei / 2 * cos(headingAngle);;
	bx = ax - thisCharacter.hei * cos(headingAngle);
	by = ay + thisCharacter.hei * sin(headingAngle);
	cx = bx + thisCharacter.wid * sin(headingAngle);
	cy = by + thisCharacter.wid * cos(headingAngle);
	dx = cx + thisCharacter.hei * cos(headingAngle);
	dy = cy - thisCharacter.hei * sin(headingAngle);
}

void Character::draw(ViewManager& theManager)
{
	//a is front left, b is back left, c is back right, d is front right
	//ax = posCenter.x + thisCharacter.wid / 2 * cos(headingAngle) - thisCharacter.wid / 2 * sin(headingAngle);
	//ay = posCenter.y - thisCharacter.wid / 2 * sin(headingAngle) - thisCharacter.hei / 2 * cos(headingAngle);;
	//bx = ax - thisCharacter.hei * cos(headingAngle);
	//by = ay + thisCharacter.hei * sin(headingAngle);
	//cx = bx + thisCharacter.wid * sin(headingAngle);
	//cy = by + thisCharacter.wid * cos(headingAngle);
	//dx = cx + thisCharacter.hei * cos(headingAngle);
	//dy = cy - thisCharacter.hei * sin(headingAngle);
	calcEdges();
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textID);

	glBegin(GL_QUADS);

	glTexCoord2d(1.0, 0.0);
	glVertex2i(ax, ay);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(bx, by);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(cx, cy);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(dx, dy);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}