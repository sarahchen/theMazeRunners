/*
DEMO
ViewManager.cpp
Created by Ryder Babik
11/1/20
Engineering Computation 24780-B
file that orchestrates the model and view
*/

#include "ViewManager2.h"
#include "ysglfontdata.h"
#include <chrono>
#include <iostream>

using namespace std;

ViewManager::ViewManager()
{

	//window size and simulation time
	FsGetWindowSize(width, height);
	
}

void ViewManager::manage()
{

	//FsGetWindowSize(width, height);
	int key;

	//creates all the textIds for the graphics
	prepareTheTextIds();

	int mouseEvent, lb, mb, rb;
	int locX, locY;
	Character theCharacter(width / 6 + (2.5 / 6 * width),(3 * height / 5), textIds[0]);

	//theCharacter.draw(*this);
	//FsSwapBuffers();
	//FsSleep(2000);

	//FsPollDevice();
	//while (FSKEY_ESC != (key = FsInkey())) {
	while(!exitDesired) { // this makes the exit buttons be what quits the game
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		mouseEvent = FsGetMouseEvent(lb, mb, rb, locX, locY);
		if (!isPlaying)
			startScreen(locX, locY, lb);
		else {
			playScreen(locX, locY, lb);
			key = FsInkey();
			switch (key) {
			case FSKEY_UP:
				//theCharacter.adjustAcceleration(.2);
				theCharacter.setForce(7000);
				break;
			case FSKEY_DOWN:
				//theCharacter.adjustAcceleration(-.2);
				theCharacter.setForce(-10000);
				break;
			case FSKEY_LEFT:
				theCharacter.adjustOrientation(.05);
				break;
			case FSKEY_RIGHT:
				theCharacter.adjustOrientation(-.05);
				break;
			default: 
				theCharacter.setForce(0);
			//	theCharacter.adjustAcceleration(-.005);
			//	if (!(abs(double(theCharacter.getVel())) < 2.0)){theCharacter.adjustForce(-20);}
				break;
			}
			theCharacter.updateKinematics(.1);
			theCharacter.draw(*this);
		}
		FsSwapBuffers();
		FsSleep(60);
		FsPollDevice();
	}
}

void ViewManager::createTextId(YsRawPngDecoder& png, const char* fileName, vector<GLuint>& textIds)
{

	GLuint textId; //texture identified for the png
	png.Decode(fileName);

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)width - 1, (float)height - 1, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &textId); //reserve a texture identifier for me
	glBindTexture(GL_TEXTURE_2D, textId); // Making the texture identifier current (or bring it to the deck)

	// set up parameters for the current texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D
	(GL_TEXTURE_2D,
		0,    // Level of detail
		GL_RGBA,       // the "A" in RGBA will include the transparency
		png.wid,    // the width and height
		png.hei,
		0,    // Border width, but not supported and needs to be 0.
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		png.rgba);

	// turn off texture 
	glDisable(GL_TEXTURE_2D);

	textIds.push_back(textId);
}

void ViewManager::prepareTheTextIds()
{
	//assign png file names
	const char* car = "car.png";

	//add all the text ids to the texture vector
	createTextId(carPic, car, textIds);
}

void ViewManager::startScreen(int locX, int locY, int lb)
{
	bool onPlay = false;
	bool onExit = false;
	if (locX > width / 5 && locX < width)
		if (locY > height / 5 && locY < 2 * height / 5)
			onPlay = true;
		else if (locY > 4 * height / 5 && locY < height)
			onExit = true;

	if (onPlay && lb) {
		isPlaying = true;
	}
	if (onExit && lb) {
		exitDesired = true;
	}

	// title box at the top of the screen
	glColor3ub(192, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, 0);
	glVertex2d(width, 0);
	glVertex2d(width, height / 5);
	glVertex2d(0, height / 5);
	glEnd();
	// draw the game title  >>>>>> will need to tweak the location <<<<<<
	oldEnglish.setColorRGB(255, 255, 255);
	oldEnglish.drawText("Maze Mania", 60, height / 5, 1.3);
	// box at the size of the screen for controls and leaderboard
	glColor3ub(192, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, height / 5);
	glVertex2d(width / 6, height / 5);
	glVertex2d(width / 6, height);
	glVertex2d(0, height);
	glEnd();
	// controls text >>>>> will need to tweak the location <<<<< 
		// this could be a user preference change (wasd or arrows)
	impact.setColorRGB(0, 0, 0);
	impact.drawText("CONTROLS", 5, height / 5 + 50, 0.5);
	impact.drawText("^", 55, height / 5 + 90, 0.5);
	impact.drawText("<-  v  ->", 10, height / 5 + 110, 0.5);
	// leaderboard text >>>> will need to tweak location <<<<
	// will need access to some kind of array of previous scores
		// likely another function that reads from a text file and creates a vector or array
	impact.drawText("LEADERBOARD", 10, height / 2, 0.35);
	/*
	impact.drawText(firstPlace.c_str(), 10, height / 2 + 20, 1.0);
	impact.drawText(secondPlace.c_str(), 10, height / 2 + 40, 1.0);
	impact.drawText(thirdPlace.c_str(), 10, height / 2 + 60, 1.0);
	*/
	// play button box
	glColor3ub(0, 255, 255); // color: cyan
	glBegin(GL_QUADS);
	glVertex2d(width / 6, height / 5);
	glVertex2d(width, height / 5);
	glVertex2d(width, 2 * height / 5);
	glVertex2d(width / 6, 2 * height / 5);
	glEnd();
	// text for play ^^
	glColor3ub(0, 0, 0);
	glRasterPos2d(width / 2 + 30, 3.25 * height / 10);
	YsGlDrawFontBitmap20x32("PLAY");

	//glColor3ub(250, 100, 100);
	//// choose player color box
	//glBegin(GL_QUADS);
	//glVertex2d(width / 6, 2 * height / 5);
	//glVertex2d(width, 2 * height / 5);
	//glVertex2d(width, 3 * height / 5);
	//glVertex2d(width / 6, 3 * height / 5);
	//glEnd();

	// text for choose player texture ^^
	glColor3ub(100, 100, 100);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textIds[0]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(width / 5, 2.25 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(width / 3, 2.25 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(width / 3, 2.75 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(width / 5, 2.75 * height / 5);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// label for first car
	impact.setColorRGB(1.0, .85, 0); //(197, 179, 88);		//vegas gold
	impact.drawText("GOLDEN", width / 4.35, 3 * height / 5, 0.3);
	impact.drawText("STALLION", width / 4.5, 3.25 * height / 5, 0.3);

	// leave height/5 space for textID player options
	glColor3ub(0, 0, 0); // black for the exit box...? maybe red?
	glBegin(GL_QUADS);
	glVertex2d(width / 6, 4 * height / 5);
	glVertex2d(width, 4 * height / 5);
	glVertex2d(width, height);
	glVertex2d(width / 6, height);
	glEnd();
	// text for end box
	glColor3ub(255, 255, 255);
	glRasterPos2d(width / 2 - 20, 4.65 * height / 5);
	YsGlDrawFontBitmap20x32("EXIT GAME");
	// outline for hovering over buttons
	glColor3ub(255, 0, 0);
	glLineWidth(5);

	if (onPlay) {
		glBegin(GL_LINE_LOOP);
		glVertex2d(width / 6, height / 5);
		glVertex2d(width, height / 5);
		glVertex2d(width, 2 * height / 5);
		glVertex2d(width / 6, 2 * height / 5);
		glEnd();
	}
	if (onExit) {
		glBegin(GL_LINE_LOOP);
		glVertex2d(width / 6, 4 * height / 5);
		glVertex2d(width, 4 * height / 5);
		glVertex2d(width, height);
		glVertex2d(width / 6, height);
		glEnd();
	}
}

void ViewManager::playScreen(int locX, int locY, int lb)
{
	bool onExit = false;
	if (locX > 0 && locX < width / 6)
		if (locY > 4.5 * height / 5 && locY < height)
			onExit = true;

	if (onExit && lb)
		exitDesired = true;

	glColor3ub(192, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, 0);
	glVertex2d(width, 0);
	glVertex2d(width, height / 5);
	glVertex2d(0, height / 5);
	glEnd();
	// draw the game title  >>>>>> will need to tweak the location <<<<<<
	oldEnglish.setColorRGB(255, 255, 255);
	oldEnglish.drawText("Maze Mania", 40, height / 5, 1.3);
	// box at the size of the screen for controls and leaderboard
	glColor3ub(192, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, height / 5);
	glVertex2d(width / 6, height / 5);
	glVertex2d(width / 6, height);
	glVertex2d(0, height);
	glEnd();
	// controls text >>>>> will need to tweak the location <<<<< 
		// this could be a user preference change (wasd or arrows)
	impact.setColorRGB(0, 0, 0);
	impact.drawText("CONTROLS", 5, height / 5 + 50, 0.5);
	impact.drawText("^", 55, height / 5 + 90, 0.5);
	impact.drawText("<-  v  ->", 10, height / 5 + 110, 0.5);
	// leaderboard text >>>> will need to tweak location <<<<
	// will need access to some kind of array of previous scores
		// likely another function that reads from a text file and creates a vector or array
	impact.drawText("LEADERBOARD", 10, height / 2, 0.35);
	// exit button on game screen
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2d(0, 4.5 * height / 5);
	glVertex2d(width / 6, 4.5 * height / 5);
	glVertex2d(width / 6, height);
	glVertex2d(0, height);
	glEnd();
	glColor3ub(255, 255, 255);
	glRasterPos2d(15, 4.8 * height / 5);
	YsGlDrawFontBitmap12x16("EXIT GAME");

	if (onExit) {
		glColor3ub(255, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 4.5 * height / 5);
		glVertex2d(width / 6, 4.5 * height / 5);
		glVertex2d(width / 6, height - 2);
		glVertex2d(0, height - 2);
		glEnd();
	}

	// placeholder for where the game map will go
	glColor3ub(130, 200, 235);
	glBegin(GL_QUADS);
	glVertex2d(width / 6, height / 5);
	glVertex2d(width, height / 5);
	glVertex2d(width, height);
	glVertex2d(width / 6, height);
	glEnd();
	/*glColor3ub(0, 0, 0);
	glRasterPos2d(width / 3 + 20, 3 * height / 5);
	YsGlDrawFontBitmap20x32("THE GAME GOES HERE");*/
}
