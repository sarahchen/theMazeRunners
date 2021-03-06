/*
Maze Mania
ViewManager2.cpp
Created by Nate Winneg, Ryder Babik
11/19/20
Engineering Computation 24780-B
file that orchestrates the model and view
*/


#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <iterator> // for iterators 
#include <vector> // for vectors 
//#include <bits/stdc++.h> 
#include <filesystem>
#include <time.h>       /* time */
#include "ViewManager2.h"
#include "ysglfontdata.h"
#include "soundHandler.h"

using namespace std;

ViewManager::ViewManager()
{
	//window size and simulation time
	FsGetWindowSize(width, height);
	
	//character selection tracking
	selectedCar = lambo;
	selectionIndex = 0;
	lamboselected = true;
	F1selected = false;
	truckSelected = false;
	carSelected = false;
	newVehicleChosen = false;
	transpR = 1.0, transpG = 1.0, transpB = 1.0;
	
	//level monitoring for changing difficulty
	prevLevel = 1;

}

void ViewManager::initialize() {

	theModel.initialize();

	theModel.initializeCharacter(selectedCar, textIds[selectionIndex]); 
	theModel.setHealthPercentage(100);

	theModel.initializeMaze();
	theModel.initializeEnemy(textIds[3]);
	prevLevel = 1;
	theModel.setCurrLevel(1);
	
}

void ViewManager::manage()
{

	//FsGetWindowSize(width, height);
	int key;

	//creates all the textIds for the graphics
	prepareTheTextIds();


	int mouseEvent, lb, mb, rb;
	int locX, locY;
	GLuint ID;	
	initialize();

	Sound coffee("engineRev.wav");
	Sound cha_ching("cash_register.wav");
	Sound starSound("star.wav");
	Sound background("backgroundMusic.wav");
	background.setVolume(0.01);

	itemType theItem;
	GLuint itemId;
	getRandomItem(theItem, itemId);
	theModel.addItem(theItem, itemId);

	while (!exitDesired) { // this makes the exit buttons be what quits the game
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		mouseEvent = FsGetMouseEvent(lb, mb, rb, locX, locY);
		
		//if the game hasnt begun yet and are on the start screen
		if (!isPlaying) {
			startScreen(locX, locY, lb, mouseEvent);
			if (background.isPlaying())
				background.stop();
			//monitor vehicle selection switches
			if (newVehicleChosen) {
				switch (selectedCar) {
				case truck:
					ID = textIdsTruck[color];
					break;
				case lambo:
					ID = textIdsCar[color];
					break;
				case F1:
					ID = textIdsF1[color];
					break;
				case regCar:
					ID = textIdsRegCar[color];
					break;
				}
				theModel.updateCharId(selectedCar, ID);
				newVehicleChosen = false;
			}
		}
		//otherwise, they want to play the game so begin the game
		else {
			if (theModel.getHealthPercentage() > 0) {
				playScreen(locX, locY, lb);
				key = FsInkey();

				// TESTING
				if (!background.isPlaying())

					background.playOnce();

				switch (theModel.getSoundTrig()) {
				case 0:
					coffee.playOnce();
					theModel.setSoundTrig(5);
					break;
				case 1:
					starSound.playOnce();
					theModel.setSoundTrig(5);
					break;
				case 2:
					cha_ching.playOnce();
					theModel.setSoundTrig(5);
					break;
				}
				
				//monitor level transitions in the game
				if (theModel.getCurrLevel() != prevLevel) {
					levelUp();
					prevLevel = theModel.getCurrLevel();
					theModel.initializeEnemy(textIds[3]);
				}

				//handle user inputs on keys to control the character's motion
				if (FsGetKeyState(FSKEY_LEFT) && FsGetKeyState(FSKEY_UP)) {
					/*		theCharacter.goForward();
							theCharacter.adjustOrientation(.05);*/
					theModel.moveCharForward();
					theModel.rotateChar(.2);
				}
				else if (FsGetKeyState(FSKEY_RIGHT) && FsGetKeyState(FSKEY_UP)) {
					/*		theCharacter.goForward();
							theCharacter.adjustOrientation(-.05);*/
					theModel.moveCharForward();
					theModel.rotateChar(-.2);
				}
				else if (FsGetKeyState(FSKEY_LEFT) && FsGetKeyState(FSKEY_DOWN)) {
					/*theCharacter.goBackward();
					theCharacter.adjustOrientation(-.05);*/
					theModel.moveCharBackward();
					theModel.rotateChar(.2);
				}
				else if (FsGetKeyState(FSKEY_RIGHT) && FsGetKeyState(FSKEY_DOWN)) {
					/*		theCharacter.goBackward();
							theCharacter.adjustOrientation(.05);*/
					theModel.moveCharBackward();
					theModel.rotateChar(-.2);
				}

				else if (FsGetKeyState(FSKEY_UP)) { theModel.moveCharForward(); } // { theCharacter.goForward(); }
				else if (FsGetKeyState(FSKEY_RIGHT)) {
					/*		theCharacter.adjustOrientation(-.1);
							theCharacter.setForce(0);*/
					theModel.rotateChar(-.2);
					theModel.zeroCharForce();
				}
				else if (FsGetKeyState(FSKEY_LEFT)) {
					/*	theCharacter.adjustOrientation(.1);
						theCharacter.setForce(0);*/
					theModel.rotateChar(.2);
					theModel.zeroCharForce();
				}
				else if (FsGetKeyState(FSKEY_DOWN)) { theModel.moveCharBackward(); } // { theCharacter.goBackward(); }

				//if the user isn't pressing any key then let the vehicle dynamics slow it down
				else { theModel.zeroCharForce(); } 
				theModel.update(*this);
			}
			else {
				if (background.isPlaying())
					background.stop();
				if (theModel.isTopScore(theModel.getPlayerScore()))
					saveScreen();
				else
					gameOverScreen();
				isPlaying = false;
				GLuint ID;
				switch (selectedCar) {
				case truck:
					ID = textIdsTruck[color];
					break;
				case lambo:
					ID = textIdsCar[color];
					break;
				case F1:
					ID = textIdsF1[color];
					break;
				case regCar:
					ID = textIdsRegCar[color];
					break;
				}
				theModel.saveLeaders();
				initialize();
			}
		}
		FsSwapBuffers();
		FsSleep(60);
		FsPollDevice();
	}
	theModel.saveLeaders();
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
	const char* F1 = "F1.png";
	const char* truckOrange = "truckOrange.png";
	const char* basicCar = "basicCar.png";
	const char* keyboard = "keyboard.png";
	const char* truckBlue = "truckBlue.png";
	const char* truckGreen = "truckGreen.png";
	const char* carYellow = "carYellow.png";
	const char* carBlue = "carBlue.png";
	const char* carRose = "carRose.png";
	const char* F1Red = "F1Red.png";
	const char* F1Blue = "F1Blue.png";
	const char* F1Green = "F1Green.png";
	const char* regCarBlue = "regCarBlue.png";
	const char* regCarPurple = "regCarPurple.png";
	const char* regCarGray = "regCarGray.png";
	const char* coffee = "coffee.png";
	const char* star = "star.png";
	const char* moneyBag = "moneyBag.png";
	const char* enemyText = "enemy.png";


	//add all the text ids to the texture vector
	createTextId(F1Pic, F1, textIds);
	createTextId(basicCarPic, basicCar, textIds);
	createTextId(keyboardPic, keyboard, textIds);
	createTextId(enemyPic, enemyText, textIds);

	createTextId(carPicYellow, carYellow, textIdsCar);
	createTextId(carPicBlue, carBlue, textIdsCar);
	createTextId(carPicRose, carRose, textIdsCar);

	createTextId(truckPicOrange, truckOrange, textIdsTruck);
	createTextId(truckPicBlue, truckBlue, textIdsTruck);
	createTextId(truckPicGreen, truckGreen, textIdsTruck);

	createTextId(F1PicRed, F1Red, textIdsF1);
	createTextId(F1PicBlue, F1Blue, textIdsF1);
	createTextId(F1PicGreen, F1Green, textIdsF1);

	createTextId(regCarPicPurple, regCarPurple, textIdsRegCar);
	createTextId(regCarPicBlue, regCarBlue, textIdsRegCar);
	createTextId(regCarPicGray, regCarGray, textIdsRegCar);

	createTextId(coffeePic, coffee, textIdsItems);
	createTextId(starPic, star, textIdsItems);
	createTextId(moneyBagPic, moneyBag, textIdsItems);
}

void ViewManager::getRandomItem(itemType& item, GLuint& Id)
{
	int randItem = rand() % textIdsItems.size();
	switch (randItem) {
	case 0:
		item = coffee;
		Id = textIdsItems[0];
		break;
	case 1:
		item = star;
		Id = textIdsItems[1];
		break;
	case 2:
		item = moneyBag;
		Id = textIdsItems[2];
		break;
	}

}

void ViewManager::startScreen(int locX, int locY, int lb, int mouseEvent)
{
	bool onPlay = false;
	bool onExit = false;
	if (locX > width / 8 && locX < width)
		if (locY > height / 7 && locY < 2 * height / 6)
			onPlay = true;
		else if (locY > 4 * height / 5 && locY < height)
			onExit = true;

	if (onPlay && lb) {
		isPlaying = true;
	}
	if (onExit && lb) {
		exitDesired = true;
	}

	drawStaticElements();

	// play button box
	glColor3ub(0, 255, 255); // color: cyan
	glBegin(GL_QUADS);
	glVertex2d(width / 8, height / 7);
	glVertex2d(width, height / 7);
	glVertex2d(width, 2 * height / 6);
	glVertex2d(width / 8, 2 * height / 6);
	glEnd();
	// text for play ^^
	glColor3ub(0, 0, 0);
	glRasterPos2d(width / 2 + 30, 3.25 * height / 12);
	YsGlDrawFontBitmap20x32("PLAY");

	drawCars(locX, locY, lb);
	drawColorSelect(locX, locY, lb);
	monitorColorSelect(locX, locY, lb, mouseEvent);

	// leave height/5 space for textID player options
	glColor3ub(0, 0, 0); // black for the exit box...? maybe red?
	glBegin(GL_QUADS);
	glVertex2d(width / 8, 4 * height / 5);
	glVertex2d(width, 4 * height / 5);
	glVertex2d(width, height);
	glVertex2d(width / 8, height);
	glEnd();
	// text for end box
	glColor3ub(255, 255, 255);
	glRasterPos2d(width / 2 - 20, 4.65 * height / 5);
	YsGlDrawFontBitmap20x32("EXIT GAME");
	// outline for hovering over buttons
	glColor3ub(255, 0, 0);
	glLineWidth(5);

	if (onPlay) {
		glColor3ub(0, 255, 0);
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		glVertex2d(width / 8, height / 7);
		glVertex2d(width, height / 7);
		glVertex2d(width, 2 * height / 6);
		glVertex2d(width / 8, 2 * height / 6);
		glEnd();
	}
	if (onExit) {
		glColor3ub(255, 0, 0);
		glLineWidth(5);
		glBegin(GL_LINE_LOOP);
		glVertex2d(width / 8, 4 * height / 5);
		glVertex2d(width, 4 * height / 5);
		glVertex2d(width, height);
		glVertex2d(width / 8, height);
		glEnd();
	}
}

void ViewManager::playScreen(int locX, int locY, int lb)
{
	bool onExit = false;
	bool onMenu = false;
	if (locX > 0 && locX < width / 6)
		if (locY > 4.5 * height / 5 && locY < height)
			onExit = true;

	if (onExit && lb)
		exitDesired = true;

	if (locX > 0 && locX < width / 6)
		if (locY > 4 * height / 5 && locY < 4.5 * height / 5)
			onMenu = true;

	if (onMenu && lb) {
		isPlaying = false;
		// re-init everything when return to menu
		GLuint ID;
		switch (selectedCar) {
		case truck:
			ID = textIdsTruck[color];
			break;
		case lambo:
			ID = textIdsCar[color];
			break;
		case F1:
			ID = textIdsF1[color];
			break;
		case regCar:
			ID = textIdsRegCar[color];
			break;
		}
		initialize();
	}
	drawStaticElements();

	// exit button on game screen
	glColor3ub(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2d(0, 4.5 * height / 5);
	glVertex2d(width / 8, 4.5 * height / 5);
	glVertex2d(width / 8, height);
	glVertex2d(0, height);
	glEnd();
	glColor3ub(255, 255, 255);
	glRasterPos2d(15, 4.8 * height / 5);
	YsGlDrawFontBitmap12x16("EXIT GAME");

	// exit to menu button on game screen
	glColor3ub(0, 100, 100);
	glBegin(GL_QUADS);
	glVertex2d(0, 4 * height / 5);
	glVertex2d(width / 8, 4 * height / 5);
	glVertex2d(width / 8, 4.5 * height / 5);
	glVertex2d(0, 4.5 * height / 5);
	glEnd();
	glColor3ub(255, 255, 255);
	glRasterPos2d(width / 20, 4.3 * height / 5);
	YsGlDrawFontBitmap12x16("MENU");

	if (onExit) {
		glColor3ub(255, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 4.5 * height / 5);
		glVertex2d(width / 8, 4.5 * height / 5);
		glVertex2d(width / 8, height - 2);
		glVertex2d(0, height - 2);
		glEnd();
	}

	if (onMenu) {
		glColor3ub(255, 0, 0);
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 4 * height / 5);
		glVertex2d(width / 8, 4 * height / 5);
		glVertex2d(width / 8, 4.5 * height / 5);
		glVertex2d(0, 4.5 * height / 5);
		glEnd();
	}

	// placeholder for where the game map will go
	glColor3ub(130, 200, 235);
	glBegin(GL_QUADS);
	glVertex2d(width / 8, height / 6);
	glVertex2d(width, height / 6);
	glVertex2d(width, height);
	glVertex2d(width / 8, height);
	glEnd();

	// health percentage
	//green
	if (theModel.getHealthPercentage() > 66)
		impact.setColorRGB(0, 255, 0);
	// yellow
	else if (theModel.getHealthPercentage() < 66 && theModel.getHealthPercentage() > 33)
		impact.setColorRGB(255, 255, 0);
	// red 
	else
		impact.setColorRGB(255, 0, 0);
	impact.drawText((std::to_string(theModel.getHealthPercentage()) + "%").c_str(), 9.1 * width / 10, height / 6, 0.7);
	impact.drawText("Health", 9 * width / 10, height / 10, 0.6);

	impact.setColorRGB(0, 0, 0);
	string levelString = "Level " + to_string(theModel.getCurrLevel());
	impact.drawText(levelString.c_str(), 160, height / 10, 0.7);
	string scoreString = "Score: " + to_string(theModel.getPlayerScore());
	impact.drawText(scoreString.c_str(), 160, height / 6, 0.7);

}

void ViewManager::levelUp()
{
	auto start = chrono::high_resolution_clock::now();
	while (chrono::duration<double>(chrono::high_resolution_clock::now() - start).count() < 1) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		glColor3ub(0, 100, 100);
		glBegin(GL_QUADS);
		glVertex2d(0, 0);
		glVertex2d(width, 0);
		glVertex2d(width, height);
		glVertex2d(0, height);
		glEnd();

		impact.setColorRGB(255, 255, 255);
		string levelString = "Level " + to_string(theModel.getCurrLevel());
		impact.drawText(levelString.c_str(), width/3, height/2, 2.0);

		FsSwapBuffers();
		FsSleep(20);
	}
}

void ViewManager::drawStaticElements()
{
	// title box at the top of the screen
	glColor3ub(0, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, 0);
	glVertex2d(width, 0);
	glVertex2d(width, height / 5);
	glVertex2d(0, height / 5);
	glEnd();
	// draw the game title  >>>>>> will need to tweak the location <<<<<<
	impact.setColorRGB(0, .98, .98);
	impact.drawText("Maze Mania", width / 2.46, height / 6.9, 1.3);
	// box at the size of the screen for controls and leaderboard
	glColor3ub(0, 192, 192);
	glBegin(GL_QUADS);
	glVertex2d(0, height / 5);
	glVertex2d(width / 8, height / 5);
	glVertex2d(width / 8, height);
	glVertex2d(0, height);
	glEnd();
	// controls text >>>>> will need to tweak the location <<<<< 
		// this could be a user preference change (wasd or arrows)
	impact.setColorRGB(0, 0, 0);
	impact.drawText("CONTROLS", 13, height / 5 + 50, 0.5);
	glColor3ub(0, 192, 192);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textIds[2]);

	glBegin(GL_QUADS);

	glTexCoord2d(1.0, 0.0);
	glVertex2i(0, height / 3.8);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(width / 8, height / 3.8);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(width / 8, height / 2.35);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(0, height / 2.35);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// leaderboard text >>>> will need to tweak location <<<<
	// will need access to some kind of array of previous scores
		// likely another function that reads from a text file and creates a vector or array
	glColor3ub(0, 230, 230);
	glBegin(GL_QUADS);

	glVertex2i(3, height / 2.2);
	glVertex2i(width / 8, height / 2.2);
	glVertex2i(width / 8, height / 2);
	glVertex2i(3, height / 2);
	glEnd();

	impact.drawText("LEADERBOARD", width / 80, height / 2.02, 0.35);

	theModel.drawLeaders(); //draw leaderboard
}

void ViewManager::saveScreen()
{
	int key;
	std::string playerName;

	FsPollDevice();
	key = FsInkey();
	while (key != FSKEY_ENTER) {
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		string aScore = to_string(theModel.getPlayerScore());
		// backdrop
		glColor3ub(192, 192, 192);
		DrawingUtilNG::drawRectangle(0, 0, width, height, true);

		// Show score
		impact.setColorHSV(360, 1, 0.5);
		impact.drawText("You made the top 5!", width / 3 - 25, height / 10 + 20, 0.8);
		impact.drawText("Your score was: " + aScore + " points!", width / 6 + 100, 2 * height / 10 + 20, 0.8);


		// ask player to give a name
		impact.setColorHSV(200, 1, 0.7);
		impact.drawText("Enter your name to save your score.", width / 6, 4.25 * height / 10, 0.7);
		impact.drawText("Press ENTER when done.", width / 6, 5 * height / 10, 0.5);

		DrawingUtilNG::drawRectangle(width / 6, 5 * height / 10 + 5, 2 * width / 3, height / 10, false);

		// maybe show the existing leaderboard and the score being saved here

		DrawingUtilNG::buildStringFromFsInkey(key, playerName);

		impact.setColorHSV(185, 1, 0.5);
		impact.drawText(playerName.c_str(), width / 6 + 5, 6 * height / 10, 0.8);

		FsSwapBuffers();
		FsSleep(20);
		FsPollDevice();
		key = FsInkey();
	}
	if (theModel.isTopScore(theModel.getPlayerScore())) theModel.addScore(playerName);
}

void ViewManager::gameOverScreen()
{
	bool goToMenu = false;
	bool exitNow = false;
	int mouseEvent, lb, mb, rb, locX, locY;
	string aScore = to_string(theModel.getPlayerScore());
	FsPollDevice();
	while (!goToMenu && !exitNow) {
		mouseEvent = FsGetMouseEvent(lb, mb, rb, locX, locY);
		bool onMenu = false;
		bool onExit = false;
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		// backdrop
		glColor3ub(0, 0, 0);
		DrawingUtilNG::drawRectangle(0, 0, width, height, true);

		// score counter
		impact.setColorRGB(255, 0, 0);
		impact.drawText("GAME OVER", 3 * width / 9, 2 * height / 5, 1.25);
		impact.drawText("Your score was: " + aScore + " points!", width / 6 + 100, 2 * height / 10 + 20, 0.8);

		glColor3b(0, 100, 100);
		glBegin(GL_QUADS);
		glVertex2i(0, 3 * height / 5);
		glVertex2i(width, 3 * height / 5);
		glVertex2i(width, 4*height/5);
		glVertex2i(0, 4*height/5);
		glEnd();
		comicSans.setColorRGB(0, 0, 0);
		comicSans.drawText("MENU", width / 2 - 125, 3.8 * height / 5, 0.8);

		glColor3b(100, 100, 0);
		glBegin(GL_QUADS);
		glVertex2i(0, 4 * height / 5);
		glVertex2i(width, 4 * height / 5);
		glVertex2i(width, height);
		glVertex2i(0, height);
		glEnd();
		comicSans.setColorRGB(0, 0, 0);
		comicSans.drawText("RAGE QUIT", width / 2 - 250, 4.8 * height / 5, 0.8);

		if (locY < 4*height/5 && locY > 3 * height/5) {
			if (locX > 0 && locX < width)
				onMenu = true;
			else
				onMenu = false;
		}
		if (locY < height && locY > 4* height/5) {
			if (locX > 0 && locX < width)
				onExit = true;
			else
				onExit = false;
		}
		glColor3ub(255, 0, 0);
		glLineWidth(10);
		if (onMenu) {
			glBegin(GL_LINE_LOOP);
			glVertex2i(0, 3 * height / 5);
			glVertex2i(width, 3 * height / 5);
			glVertex2i(width, 4 * height / 5);
			glVertex2i(0, 4 * height / 5);
			glEnd();
		}
		if (onExit) {
			glBegin(GL_LINE_LOOP);
			glVertex2i(0, 4 * height / 5);
			glVertex2i(width, 4 * height / 5);
			glVertex2i(width, height);
			glVertex2i(0, height);
			glEnd();
		}

		if (onMenu && lb)
			goToMenu = true;
		else if (onExit && lb)
			exitNow = true;
		//cout << onExit << endl;
		FsSwapBuffers();
		FsSleep(20);
		FsPollDevice();
	}
	glLineWidth(1);
	if (goToMenu)
		exitDesired = false;
	else if (exitNow)
		exitDesired = true;
}

void ViewManager::drawCars(int locX, int locY, int lb)
{

	double selected = 1.0;
	double notSelected = 0.3;

	bool onlambo = false;
	bool onF1 = false;
	bool onTruck = false;
	bool onCar = false;
	bool selectionOptions[4] = { lamboselected, F1selected, truckSelected, carSelected };
	carType carSelections[4] = { lambo, F1, truck, regCar };

	//for lambo
	int lambo_leftX = width / 5.2 - 8;
	int lambo_rightX = width / 3.2 + 8;
	int lambo_topY = 2.1 * height / 5 - 8;
	int lambo_bottomY = 3.25 * height / 5 + 10;

	//for F1
	int F1_leftX = 2 * width / 5 - 8;
	int F1_rightX = 1 * width / 2 + 8;
	int F1_topY = 2.1 * height / 5 - 8;
	int F1_bottomY = 3.25 * height / 5 + 10;

	//for truck
	int truck_leftX = 5 * width / 9 - 8;
	int truck_rightX = 3 * width / 4 + 8;
	int truck_topY = 2.15 * height / 5 - 8;
	int truck_bottomY = 3.25 * height / 5 + 10;

	//for car
	int car_leftX = 4.05 * width / 5 - 8;
	int car_rightX = 11. * width / 12 + 8;
	int car_topY = 2.15 * height / 5 - 8;
	int car_bottomY = 3.25 * height / 5 + 10;


	if (locX > lambo_leftX&& locX < lambo_rightX) {
		if (locY > lambo_topY&& locY < lambo_bottomY) {
			drawHoverOutline(lambo_leftX, lambo_topY, lambo_rightX, lambo_bottomY);
			onlambo = true;
		}
	}
	if (locX > F1_leftX&& locX < F1_rightX) {
		if (locY > F1_topY&& locY < F1_bottomY) {
			drawHoverOutline(F1_leftX, F1_topY, F1_rightX, F1_bottomY);
			onF1 = true;
		}
	}
	if (locX > truck_leftX&& locX < truck_rightX) {
		if (locY > truck_topY&& locY < truck_bottomY) {
			drawHoverOutline(truck_leftX, truck_topY, truck_rightX, truck_bottomY);
			onTruck = true;
		}
	}
	if (locX > car_leftX&& locX < car_rightX) {
		if (locY > car_topY&& locY < car_bottomY) {
			drawHoverOutline(car_leftX, car_topY, car_rightX, car_bottomY);
			onCar = true;
		}
	}

	//if (onlambo && lb) {setCarSelection(lamboselected, selectionOptions);}
	if ((onlambo || onF1 || onTruck || onCar) && lb) { clearCarSelection(); }
	if (onlambo && lb) { lamboselected = true; }
	if (onF1 && lb) { F1selected = true; }
	if (onTruck && lb) { truckSelected = true; }
	if (onCar && lb) { carSelected = true; }

	if (lamboselected) { drawSelection(lambo_leftX, lambo_rightX, lambo_topY, lambo_bottomY); }
	if (F1selected) { drawSelection(F1_leftX, F1_rightX, F1_topY, F1_bottomY); }
	if (truckSelected) { drawSelection(truck_leftX, truck_rightX, truck_topY, truck_bottomY); }
	if (carSelected) { drawSelection(car_leftX, car_rightX, car_topY, car_bottomY); }

	for (int i = 0; i < 4; i++) {
		if (selectionOptions[i] == true) {
			newVehicleChosen = true;
			selectedCar = carSelections[i];
			selectionIndex = i;
		}
	}

	// label for first car
	impact.setColorRGB(1.0, .85, 0); //(197, 179, 88);		//vegas gold
	impact.drawText("GOLDEN", width / 4.4, 3 * height / 5, 0.3);
	impact.drawText("STALLION", width / 4.5, 3.25 * height / 5, 0.3);

	// label for second car
	impact.setColorRGB(0.95, .2, 0);	//hot red
	impact.drawText("ROAD", 2.01 * width / 4.7, 3 * height / 5, 0.3);
	impact.drawText("RUNNER", 2.01 * width / 4.77, 3.25 * height / 5, 0.3);

	// label for third car
	impact.setColorRGB(0.95, .5, 0);	//burnt orange
	impact.drawText("BIG", 5 * width / 7.95, 3 * height / 5, 0.3);
	impact.drawText("BOY", 5 * width / 7.97, 3.25 * height / 5, 0.3);

	// label for fourth car
	impact.setColorRGB(0, .9, 0.9);	//cyan
	impact.drawText("COOL", 8.5 * width / 10, 3 * height / 5, 0.3);
	impact.drawText("CRUISER", 8.4 * width / 10, 3.25 * height / 5, 0.3);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (selectedCar == lambo) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIdsCar[color]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(width / 5.2, 2.1 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(width / 3.2, 2.1 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(width / 3.2, 2.6 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(width / 5.2, 2.6 * height / 5);
	glEnd();

	if (selectedCar == F1) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIdsF1[color]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(2 * width / 5, 2.1 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(5 * width / 10, 2.1 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(5 * width / 10, 2.6 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(2 * width / 5, 2.6 * height / 5);
	glEnd();

	if (selectedCar == truck) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIdsTruck[color]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(5 * width / 9, 2.15 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(3 * width / 4, 2.15 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(3 * width / 4, 2.7 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(5 * width / 9, 2.7 * height / 5);
	glEnd();

	if (selectedCar == regCar) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIdsRegCar[color]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(4.05 * width / 5, 2.15 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(11. * width / 12, 2.15 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(11. * width / 12, 2.55 * height / 5);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(4.05 * width / 5, 2.55 * height / 5);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

void ViewManager::drawHoverOutline(int leftX, int topY, int rightX, int bottomY)
{
	static int i = 0;
	if (true/*i < 5*/) {
		glColor3ub(0, 0, 255);
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
		glVertex2d(leftX, topY);
		glVertex2d(rightX, topY);
		glVertex2d(rightX, bottomY);
		glVertex2d(leftX, bottomY);
		glEnd();
		i++;
	}
	else if (i < 10) { i++; }
	else i = 0;
	//cout << i << endl;
}

void ViewManager::clearCarSelection()
{
	lamboselected = false;
	F1selected = false;
	truckSelected = false;
	carSelected = false;
}

void ViewManager::drawSelection(int leftX, int rightX, int topY, int bottomY)
{
	glColor3ub(130, 200, 240); //draw filled in selection
	glBegin(GL_QUADS);
	glVertex2d(leftX, topY);
	glVertex2d(rightX, topY);
	glVertex2d(rightX, bottomY);
	glVertex2d(leftX, bottomY);
	glEnd();

	glColor3ub(0, 0, 255);	//draw selection outline
	glLineWidth(3);
	glBegin(GL_LINE_LOOP);
	glVertex2d(leftX, topY);
	glVertex2d(rightX, topY);
	glVertex2d(rightX, bottomY);
	glVertex2d(leftX, bottomY);
	glEnd();


}

void ViewManager::drawColorSelect(int locX, int locY, int lb)
{

	glColor3ub(150, 200, 250); //background for color select box
	glBegin(GL_QUADS);
	glVertex2d(1.9 * width / 5, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glVertex2d(1.9 * width / 5, 3.8 * height / 5);
	glEnd();

	glColor3ub(100, 150, 200); //outline for color select box
	glBegin(GL_LINE_LOOP);
	glVertex2d(1.9 * width / 5, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glVertex2d(1.9 * width / 5, 3.8 * height / 5);
	glEnd();

	impact.setColorRGB(0, 0, 0);
	impact.drawText("COLOR SELECT", width / 2.03, 3.76 * height / 5, 0.45);


	glColor3ub(220, 220, 220); //background box for right triangle
	glBegin(GL_QUADS);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.33 * width / 3, 3.5 * height / 5);
	glVertex2d(2.33 * width / 3, 3.8 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glEnd();

	glColor3ub(100, 150, 200); //outline for right color select box
	glBegin(GL_LINE_LOOP);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.33 * width / 3, 3.5 * height / 5);
	glVertex2d(2.33 * width / 3, 3.8 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glEnd();

	glColor3ub(0, 0, 200); // filled in right triangle
	glBegin(GL_TRIANGLES);
	glVertex2d(2.22 * width / 3, 3.55 * height / 5);
	glVertex2d(2.31 * width / 3, 3.65 * height / 5);
	glVertex2d(2.22 * width / 3, 3.75 * height / 5);
	glEnd();

	glColor3ub(0, 0, 100); // outline right triangle
	glBegin(GL_LINE_LOOP);
	glVertex2d(2.22 * width / 3, 3.55 * height / 5);
	glVertex2d(2.31 * width / 3, 3.65 * height / 5);
	glVertex2d(2.22 * width / 3, 3.75 * height / 5);
	glEnd();


	glColor3ub(220, 220, 220); //background box for left triangle
	glBegin(GL_QUADS);
	glVertex2d(1.9 * width / 5, 3.5 * height / 5);
	glVertex2d(1.7 * width / 5, 3.5 * height / 5);
	glVertex2d(1.7 * width / 5, 3.8 * height / 5);
	glVertex2d(1.9 * width / 5, 3.8 * height / 5);
	glEnd();

	glColor3ub(100, 150, 200); //outline for left color select box
	glBegin(GL_LINE_LOOP);
	glVertex2d(1.9 * width / 5, 3.5 * height / 5);
	glVertex2d(1.7 * width / 5, 3.5 * height / 5);
	glVertex2d(1.7 * width / 5, 3.8 * height / 5);
	glVertex2d(1.9 * width / 5, 3.8 * height / 5);
	glEnd();

	glColor3ub(0, 0, 200); // filled in left triangle
	glBegin(GL_TRIANGLES);
	glVertex2d(1.87 * width / 5, 3.55 * height / 5);
	glVertex2d(1.75 * width / 5, 3.65 * height / 5);
	glVertex2d(1.87 * width / 5, 3.75 * height / 5);
	glEnd();

	glColor3ub(0, 0, 100); // outline left triangle
	glBegin(GL_LINE_LOOP);
	glVertex2d(1.87 * width / 5, 3.55 * height / 5);
	glVertex2d(1.73 * width / 5, 3.65 * height / 5);
	glVertex2d(1.87 * width / 5, 3.75 * height / 5);
	glEnd();

}

void ViewManager::monitorColorSelect(int locX, int locY, int lb, int mouseEvent)
{

	bool onLeftColor = false, onRightColor = false;
	//on left box check
	if (locX > 1.7 * width / 5 && locX < 1.9 * width / 5)
		if (locY > 3.5 * height / 5 && locY < 3.8 * height / 5) {
			onLeftColor = true;
			glColor3ub(0, 250, 0);
			glBegin(GL_LINE_LOOP);
			glVertex2d(1.9 * width / 5, 3.5 * height / 5);
			glVertex2d(1.7 * width / 5, 3.5 * height / 5);
			glVertex2d(1.7 * width / 5, 3.8 * height / 5);
			glVertex2d(1.9 * width / 5, 3.8 * height / 5);
			glEnd();
		}

	//on right box check
	if (locX > 2.2 * width / 3 && locX < 2.33 * width / 3)
		if (locY > 3.5 * height / 5 && locY < 3.8 * height / 5) {
			onRightColor = true;
			glColor3ub(0, 250, 0); //outline for right color select box
			glBegin(GL_LINE_LOOP);
			glVertex2d(2.2 * width / 3, 3.5 * height / 5);
			glVertex2d(2.33 * width / 3, 3.5 * height / 5);
			glVertex2d(2.33 * width / 3, 3.8 * height / 5);
			glVertex2d(2.2 * width / 3, 3.8 * height / 5);
			glEnd();
		}

	if (onLeftColor && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		if (color == 0) color = 2;
		else color -= 1;
		newVehicleChosen = true;
	}
	if (onRightColor && mouseEvent == FSMOUSEEVENT_LBUTTONDOWN) {
		color = (color + 1) % 3;
		newVehicleChosen = true;
	}

}

void ViewManager::colorSelect(int locX, int locY, int lb)
{
	if (locX > 2.3 * width / 5 && locX < 2.2 * width / 3) {
		if (locY > 3.5 * height / 5 && locY < 3.8 * height / 5) {
			if (color == 0) color = 3;
			else color -= 1;
			newVehicleChosen = true;
		}
	}
	if (locX > 2.3 * width / 5 && locX < 2.2 * width / 3) {
		if (locY > 3.5 * height / 5 && locY < 3.8 * height / 5) {
			if (color == 0) color = 3;
			else color -= 1;
			newVehicleChosen = true;

		}
	}

}
