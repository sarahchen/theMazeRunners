
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
#include "ViewManager2.h"
#include "ysglfontdata.h"

using namespace std;

ViewManager::ViewManager()
{
	//window size and simulation time
	FsGetWindowSize(width, height);
	selectedCar = lambo;
	selectionIndex = 0;
	lamboselected = true;
	F1selected = false;
	truckSelected = false;
	carSelected = false;

	newVehicleChosen = false;
	transpR = 1.0, transpG = 1.0, transpB = 1.0;
}

void ViewManager::manage()
{

	//FsGetWindowSize(width, height);
	int key;

	//creates all the textIds for the graphics
	prepareTheTextIds();

	theModel.load();  // load the leaderboard file

	int mouseEvent, lb, mb, rb;
	int locX, locY;

	theModel.initializeCharacter(selectedCar, textIds[selectionIndex]);
	theModel.initializeMaze();
	theModel.initializeEnemy();

	while (!exitDesired) { // this makes the exit buttons be what quits the game
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		mouseEvent = FsGetMouseEvent(lb, mb, rb, locX, locY);

		if (!isPlaying) {
			startScreen(locX, locY, lb);
			if (newVehicleChosen) {
				theModel.updateCharId(selectedCar, textIds[selectionIndex]);
				//theCharacter.assignId(selectedCar, textIds[selectionIndex]);
				newVehicleChosen = false;
			}
		}

		else {

			playScreen(locX, locY, lb);
			key = FsInkey();

			if (FsGetKeyState(FSKEY_LEFT) && FsGetKeyState(FSKEY_UP)) {
		/*		theCharacter.goForward();
				theCharacter.adjustOrientation(.05);*/
				theModel.moveCharForward();
				theModel.rotateChar(.05);
			}
			else if (FsGetKeyState(FSKEY_RIGHT) && FsGetKeyState(FSKEY_UP)) {
		/*		theCharacter.goForward();
				theCharacter.adjustOrientation(-.05);*/
				theModel.moveCharForward();
				theModel.rotateChar(-.05);
			}
			else if (FsGetKeyState(FSKEY_LEFT) && FsGetKeyState(FSKEY_DOWN)) {
				/*theCharacter.goBackward();
				theCharacter.adjustOrientation(-.05);*/
				theModel.moveCharBackward();
				theModel.rotateChar(.05);
			}
			else if (FsGetKeyState(FSKEY_RIGHT) && FsGetKeyState(FSKEY_DOWN)) {
		/*		theCharacter.goBackward();
				theCharacter.adjustOrientation(.05);*/
				theModel.moveCharBackward();
				theModel.rotateChar(-.05);
			}

			else if (FsGetKeyState(FSKEY_UP)) { theModel.moveCharForward(); } // { theCharacter.goForward(); }
			else if (FsGetKeyState(FSKEY_RIGHT)) {
		/*		theCharacter.adjustOrientation(-.1);
				theCharacter.setForce(0);*/
				theModel.rotateChar(-.1);
				theModel.zeroCharForce();
			}
			else if (FsGetKeyState(FSKEY_LEFT)) {
			/*	theCharacter.adjustOrientation(.1);
				theCharacter.setForce(0);*/
				theModel.rotateChar(.1);
				theModel.zeroCharForce();
			}
			else if (FsGetKeyState(FSKEY_DOWN)) { theModel.moveCharBackward(); } // { theCharacter.goBackward(); }

			else { theModel.zeroCharForce(); } //{ theCharacter.setForce(0); }

			//else {
			//	switch (key) {
			//	case FSKEY_UP:
			//		//theCharacter.adjustAcceleration(.2);
			//		theCharacter.goForward();
			//		break;
			//	case FSKEY_DOWN:
			//		//theCharacter.adjustAcceleration(-.2);
			//		theCharacter.goBackward();
			//		break;
			//	case FSKEY_LEFT:
			//		theCharacter.adjustOrientation(.05);
			//		break;
			//	case FSKEY_RIGHT:
			//		theCharacter.adjustOrientation(-.05);
			//		break;
			//	default:
			//		theCharacter.setForce(0);
			//		//	theCharacter.adjustAcceleration(-.005);
			//		//	if (!(abs(double(theCharacter.getVel())) < 2.0)){theCharacter.adjustForce(-20);}
			//		break;
			//	}
			//}

			/*theCharacter.updateKinematics(.1);
			theCharacter.draw(*this);*/
			theModel.update(*this);
		}
		FsSwapBuffers();
		FsSleep(60);
		FsPollDevice();
	}
	theModel.save();
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
	const char* truck = "truck.png";
	const char* basicCar = "basicCar.png";
	const char* keyboard = "keyboard.png";

	//add all the text ids to the texture vector
	createTextId(carPic, car, textIds);
	createTextId(F1Pic, F1, textIds);
	createTextId(truckPic, truck, textIds);
	createTextId(basicCarPic, basicCar, textIds);
	createTextId(keyboardPic, keyboard, textIds);
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

	drawStaticElements();

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

	drawCars(locX, locY, lb);
	drawColorSelect(locX, locY, lb);

	// text for choose player texture ^^
	//glColor3ub(100, 100, 100);

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

//void ViewManager::load()
//{
//	string inFileName = "leaderboard.txt";
//	ifstream inFile;
//
//	inFile.open(inFileName);
//
//	if (inFile.is_open()) {
//		readFile(inFile);
//		inFile.close();
//	}
//	else
//		cout << "Was not able to open " << inFileName << " for input. " << endl;
//}
//
//void ViewManager::drawLeaders()
//{
//
//	//draw background for leaderboard
//	glColor3ub(230, 230, 255); // black for the exit box...? maybe red?
//	glBegin(GL_QUADS);
//	glVertex2d(2, height/2 + 5);
//	glVertex2d(130, height / 2 + 5);
//	glVertex2d(130, height / 2 + leaders.size() * 40 + 10);  //200
//	glVertex2d(2, height / 2 + leaders.size() * 40 + 10);
//	glEnd();
//
//	// Declaring iterator to a vector 
//	vector<pair<string, int>>::iterator ptr;
//
//	string display;
//	int hei = height/2;
//	int i = 0;
//
//	for (auto iter : leaders) {
//		if (i < 7) {
//			display = iter.first + "   " + to_string(iter.second);
//			hei += 40;
//			i++;
//			comicSans.setColorRGB(0, 0, 0);
//			comicSans.drawText(display, 5, hei, 0.2);
//		}
//		i++;
//	}
//}
//
//void ViewManager::save()
//{
//	string outFileName = "leaderboard.txt";
//	ifstream inFile;
//	ofstream outFile;
//
//	outFile.open(outFileName);
//
//	if (outFile.is_open()) {
//		writeFile(outFile);
//		outFile.close();
//	}
//}
//
////sort by score in descending order
//bool ViewManager::sortbysec(const pair<int, int> &a, const pair<int, int> &b)
//{
//	return (a.second > b.second);
//}
//
//void ViewManager::readFile(ifstream& inFile)
//{
//	using namespace std;
//
//	string currLine;
//	stringstream currStream;
//
//	string name;
//	int score;
//
//	while (!inFile.eof()) //&& not '\n'
//	{
//		getline(inFile, currLine);
//		if (!(currLine.empty())) {
//			currStream.str(currLine);
//			currStream >> name >> score;
//			currStream.clear();
//			leaders.push_back(make_pair(name, score));
//		}
//	}
//
//	// Using sort() function to sort by 2nd element 
//	// of pair 
//	sort(leaders.begin(), leaders.end()); //, sortbysec);
//		/*if (currLine.find(":") != string::npos) {
//			string name = StringPlus::trim(0, currLine.substr(currLine.find(":") - 1));
//			int score = StringPlus::trim(currLine.substr(currLine.find(":") + 1));
//
//		}*/
//
//}
//
//void ViewManager::writeFile(ofstream& outFile)
//{
//	// Declaring iterator to a vector 
//	vector<pair<string, int>>::iterator ptr;
//
//	//for (ptr = leaders.begin(); ptr < leaders.end(); ptr++) {
//	//	while (int i = 0 < 8)
//	//		cout << *ptr << endl;
//	//}
//	int i = 0;
//	for (auto iter : leaders) {
//		outFile << iter.first << "\t" << iter.second << endl;
//	}
//
//}

void ViewManager::playScreen(int locX, int locY, int lb)
{
	bool onExit = false;
	if (locX > 0 && locX < width / 6)
		if (locY > 4.5 * height / 5 && locY < height)
			onExit = true;

	if (onExit && lb)
		exitDesired = true;

	drawStaticElements();

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

void ViewManager::drawStaticElements()
{
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
	//impact.drawText("^", 55, height / 5 + 90, 0.5);
	//impact.drawText("<-  v  ->", 10, height / 5 + 110, 0.5);

	glColor3ub(192, 192, 192);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textIds[4]);

	glBegin(GL_QUADS);

	glTexCoord2d(1.0, 0.0);
	glVertex2i(0, 170);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(140, 170);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(140, 270);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(0, 270);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	// leaderboard text >>>> will need to tweak location <<<<
	// will need access to some kind of array of previous scores
		// likely another function that reads from a text file and creates a vector or array
	glColor3ub(130, 160, 255);
	glBegin(GL_QUADS);

	glVertex2i(2, height / 2 - 25);
	glVertex2i(130, height / 2 - 25);
	glVertex2i(130, height / 2);
	glVertex2i(2, height / 2);
	glEnd();

	impact.drawText("LEADERBOARD", 10, height / 2, 0.35);

	theModel.drawLeaders(); //draw leaderboard
}

void ViewManager::drawCars(int locX, int locY, int lb)
{

	double selected = 1.0;
	double notSelected = 0.3;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (selectedCar == lambo) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

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

	if (selectedCar == F1) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIds[1]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(2 * width / 5, 2.25 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(5 * width / 10, 2.25 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(5 * width / 10, 2.75 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(2 * width / 5, 2.75 * height / 5);
	glEnd();

	if (selectedCar == truck) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIds[2]);
	glBegin(GL_QUADS);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(5 * width / 9, 2.2 * height / 5);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(3 * width / 4, 2.2 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(3 * width / 4, 2.85 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(5 * width / 9, 2.85 * height / 5);
	glEnd();

	if (selectedCar == regCar) { glColor4d(transpR, transpG, transpB, selected); }
	else { glColor4d(transpR, transpG, transpB, notSelected); }

	glBindTexture(GL_TEXTURE_2D, textIds[3]);
	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2d(4 * width / 5, 2.1 * height / 5);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(11.2 * width / 12, 2.1 * height / 5);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(11.2 * width / 12, 2.85 * height / 5);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(4 * width / 5, 2.85 * height / 5);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	//bool onExit = false;
	//if (locX > width / 5 && locX < width)
	//	if (locY > height / 5 && locY < 2 * height / 5)
	//		onPlay = true;
	//	else if (locY > 4 * height / 5 && locY < height)
	//		onExit = true;

	//if (onPlay && lb) {
	//	isPlaying = true;
	//}
	//if (onExit && lb) {
	//	exitDesired = true;
	//}

	bool onlambo = false;
	bool onF1 = false;
	bool onTruck = false;
	bool onCar = false;

	//static bool lamboselected = false;
	//static bool F1selected = false;
	//static bool truckSelected = false;
	//static bool carSelected = false;
	bool selectionOptions[4] = { lamboselected, F1selected, truckSelected, carSelected };
	carType carSelections[4] = { lambo, F1, truck, regCar };

	//for lambo
	int lambo_leftX = width / 4.5 - 8;
	int lambo_rightX = width / 4.35 + 70;
	int lambo_topY = 3 * height / 5 - 30;
	int lambo_bottomY = 3.25 * height / 5 + 10;

	//for F1
	int F1_leftX = 2 * width / 4.7 - 10;
	int F1_rightX = 2 * width / 4.7 + 55;
	int F1_topY = 3 * height / 5 - 30;
	int F1_bottomY = 3.25 * height / 5 + 10;

	//for truck
	int truck_leftX = 5 * width / 7.95 - 20;
	int truck_rightX = 5 * width / 7.95 + 45;
	int truck_topY = 3 * height / 5 - 30;
	int truck_bottomY = 3.25 * height / 5 + 10;

	//for car
	int car_leftX = 8.5 * width / 10 - 20;
	int car_rightX = 8.5 * width / 10 + 55;
	int car_topY = 3 * height / 5 - 30;
	int car_bottomY = 3.25 * height / 5 + 10;


	if (locX > lambo_leftX&& locX < lambo_rightX) {
		if (locY > lambo_topY / 5 && locY < lambo_bottomY) {
			drawHoverOutline(lambo_leftX, lambo_topY, lambo_rightX, lambo_bottomY);
			onlambo = true;
		}
	}
	if (locX > F1_leftX&& locX < F1_rightX) {
		if (locY > F1_topY / 5 && locY < F1_bottomY) {
			drawHoverOutline(F1_leftX, F1_topY, F1_rightX, F1_bottomY);
			onF1 = true;
		}
	}
	if (locX > truck_leftX&& locX < truck_rightX) {
		if (locY > truck_topY / 5 && locY < truck_bottomY) {
			drawHoverOutline(truck_leftX, truck_topY, truck_rightX, truck_bottomY);
			onTruck = true;
		}
	}
	if (locX > car_leftX&& locX < car_rightX) {
		if (locY > car_topY / 5 && locY < car_bottomY) {
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
	impact.drawText("ROAD", 2 * width / 4.7, 3 * height / 5, 0.3);
	impact.drawText("RUNNER", 2 * width / 4.77, 3.25 * height / 5, 0.3);

	// label for third car
	impact.setColorRGB(0.95, .5, 0);	//burnt orange
	impact.drawText("BIG", 5 * width / 7.95, 3 * height / 5, 0.3);
	impact.drawText("BOY", 5 * width / 7.97, 3.25 * height / 5, 0.3);

	// label for fourth car
	impact.setColorRGB(0, .9, 0.9);	//cyan
	impact.drawText("COOL", 8.5 * width / 10, 3 * height / 5, 0.3);
	impact.drawText("CRUISER", 8.35 * width / 10, 3.25 * height / 5, 0.3);
}

void ViewManager::drawHoverOutline(int leftX, int topY, int rightX, int bottomY)
{
	static int i = 0;
	if (i < 5) {
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

	glColor3ub(255, 255, 150); //background for color select box
	glBegin(GL_QUADS);
	glVertex2d(2.3 * width / 5, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glVertex2d(2.3 * width / 5, 3.8 * height / 5);
	glEnd();

	glColor3ub(207, 181, 59); //outline for color select box
	glBegin(GL_LINE_LOOP);
	glVertex2d(2.3 * width / 5, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glVertex2d(2.3 * width / 5, 3.8 * height / 5);
	glEnd();

	garamond.setColorRGB(0, 0, 0);
	garamond.drawText("COLOR SELECT", width / 2.1, 3.76 * height / 5, 0.4);

	glColor3ub(200, 200, 200); // filled in right triangle
	glBegin(GL_TRIANGLES);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.35 * width / 3, 3.65 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glEnd();

	glColor3ub(100, 100, 100); // outline right triangle
	glBegin(GL_LINE_LOOP);
	glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	glVertex2d(2.35 * width / 3, 3.65 * height / 5);
	glVertex2d(2.2 * width / 3, 3.8 * height / 5);
	glEnd();

	glColor3ub(200, 200, 200); // filled in left triangle
	glBegin(GL_TRIANGLES);
	glVertex2d(2.3 * width / 5, 3.5 * height / 5);
	glVertex2d(2.07 * width / 5, 3.65 * height / 5);
	glVertex2d(2.3 * width / 5, 3.8 * height / 5);
	glEnd();

	glColor3ub(100, 100, 100); // outline left triangle
	glBegin(GL_LINE_LOOP);
	glVertex2d(2.3 * width / 5, 3.5 * height / 5);
	glVertex2d(2.07 * width / 5, 3.65 * height / 5);
	glVertex2d(2.3 * width / 5, 3.8 * height / 5);
	glEnd();

	double slopeLeftLower, slopeLeftUpper, slopeRightLower, slopeRightUpper;

	slopeRightLower = ((3.65 * height / 5) - 3.8 * height / 5) / ((2.35 * width / 3) - (2.2 * width / 3));
	slopeRightUpper = ((3.65 * height / 5) - 3.8 * height / 5) / ((2.35 * width / 3) - (2.2 * width / 3));
	//check for button click!!
	//if (locX > 2.2 * width / 3 && locY > slopeLeftLower)

	//glVertex2d(2.2 * width / 3, 3.5 * height / 5);
	//glVertex2d(2.35 * width / 3, 3.65 * height / 5);
	//glVertex2d(2.2 * width / 3, 3.8 * height / 5);

	//if (locX > 2.2 * width / 3 && locX < 3.65 * height / 5)
	//	if (locY > slopeRightLower && locY < slopeRightUpper)

	//if (locX > 2.2 * width / 3 && locX < 3.65 * height / 5)

}

void ViewManager::setTransparency(int locX, int locY, int lb)
{
	

}

//void ViewManager::setCarSelection(bool &selection, bool *selectionOptions)
//{	
//	for (int i = 0; i < sizeof(selectionOptions) / sizeof(selectionOptions[0]); i++) 
//		selectionOptions[i] = false;
//
//	selection = true;
//}

//		mouseEvent = FsGetMouseEvent(lb, mb, rb, locX, locY);
//		if (!isPlaying)
//			startScreen(locX, locY, lb);
//		else {
//			playScreen(locX, locY, lb);
//			key = FsInkey();
//			switch (key) {
//			case FSKEY_UP:
//				//theCharacter.adjustAcceleration(.2);
//				theCharacter.setForce(7000);
//				break;
//			case FSKEY_DOWN:
//				//theCharacter.adjustAcceleration(-.2);
//				theCharacter.setForce(-10000);
//				break;
//			case FSKEY_LEFT:
//				theCharacter.adjustOrientation(.05);
//				break;
//			case FSKEY_RIGHT:
//				theCharacter.adjustOrientation(-.05);
//				break;
//			default: 
//				theCharacter.setForce(0);
//			//	theCharacter.adjustAcceleration(-.005);
//			//	if (!(abs(double(theCharacter.getVel())) < 2.0)){theCharacter.adjustForce(-20);}
//				break;
//			}
//			theCharacter.updateKinematics(.1);
//			theCharacter.draw(*this);
//		}
//		FsSwapBuffers();
//		FsSleep(60);
//		FsPollDevice();
//	}
//}

//void ViewManager::createTextId(YsRawPngDecoder& png, const char* fileName, vector<GLuint>& textIds)
//{
//
//	GLuint textId; //texture identified for the png
//	png.Decode(fileName);
//
//	glViewport(0, 0, width, height);
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0, (float)width - 1, (float)height - 1, 0, -1, 1);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	
//	glEnable(GL_TEXTURE_2D);
//	glGenTextures(1, &textId); //reserve a texture identifier for me
//	glBindTexture(GL_TEXTURE_2D, textId); // Making the texture identifier current (or bring it to the deck)
//
//	// set up parameters for the current texture
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//
//	glTexImage2D
//	(GL_TEXTURE_2D,
//		0,    // Level of detail
//		GL_RGBA,       // the "A" in RGBA will include the transparency
//		png.wid,    // the width and height
//		png.hei,
//		0,    // Border width, but not supported and needs to be 0.
//		GL_RGBA,
//		GL_UNSIGNED_BYTE,
//		png.rgba);
//
//	// turn off texture 
//	glDisable(GL_TEXTURE_2D);
//
//	textIds.push_back(textId);
//}

//void ViewManager::prepareTheTextIds()
//{
//	//assign png file names
//	const char* car = "car.png";
//
//	//add all the text ids to the texture vector
//	createTextId(carPic, car, textIds);
//}
//
//void ViewManager::startScreen(int locX, int locY, int lb)
//{
//	bool onPlay = false;
//	bool onExit = false;
//	if (locX > width / 5 && locX < width)
//		if (locY > height / 5 && locY < 2 * height / 5)
//			onPlay = true;
//		else if (locY > 4 * height / 5 && locY < height)
//			onExit = true;
//
//	if (onPlay && lb) {
//		isPlaying = true;
//	}
//	if (onExit && lb) {
//		exitDesired = true;
//	}
//
//	// title box at the top of the screen
//	glColor3ub(192, 192, 192);
//	glBegin(GL_QUADS);
//	glVertex2d(0, 0);
//	glVertex2d(width, 0);
//	glVertex2d(width, height / 5);
//	glVertex2d(0, height / 5);
//	glEnd();
//	// draw the game title  >>>>>> will need to tweak the location <<<<<<
//	oldEnglish.setColorRGB(255, 255, 255);
//	oldEnglish.drawText("Maze Mania", 60, height / 5, 1.3);
//	// box at the size of the screen for controls and leaderboard
//	glColor3ub(192, 192, 192);
//	glBegin(GL_QUADS);
//	glVertex2d(0, height / 5);
//	glVertex2d(width / 6, height / 5);
//	glVertex2d(width / 6, height);
//	glVertex2d(0, height);
//	glEnd();
//	// controls text >>>>> will need to tweak the location <<<<< 
//		// this could be a user preference change (wasd or arrows)
//	impact.setColorRGB(0, 0, 0);
//	impact.drawText("CONTROLS", 5, height / 5 + 50, 0.5);
//	impact.drawText("^", 55, height / 5 + 90, 0.5);
//	impact.drawText("<-  v  ->", 10, height / 5 + 110, 0.5);
//	// leaderboard text >>>> will need to tweak location <<<<
//	// will need access to some kind of array of previous scores
//		// likely another function that reads from a text file and creates a vector or array
//	impact.drawText("LEADERBOARD", 10, height / 2, 0.35);
//	/*
//	impact.drawText(firstPlace.c_str(), 10, height / 2 + 20, 1.0);
//	impact.drawText(secondPlace.c_str(), 10, height / 2 + 40, 1.0);
//	impact.drawText(thirdPlace.c_str(), 10, height / 2 + 60, 1.0);
//	*/
//	// play button box
//	glColor3ub(0, 255, 255); // color: cyan
//	glBegin(GL_QUADS);
//	glVertex2d(width / 6, height / 5);
//	glVertex2d(width, height / 5);
//	glVertex2d(width, 2 * height / 5);
//	glVertex2d(width / 6, 2 * height / 5);
//	glEnd();
//	// text for play ^^
//	glColor3ub(0, 0, 0);
//	glRasterPos2d(width / 2 + 30, 3.25 * height / 10);
//	YsGlDrawFontBitmap20x32("PLAY");
//
//	//glColor3ub(250, 100, 100);
//	//// choose player color box
//	//glBegin(GL_QUADS);
//	//glVertex2d(width / 6, 2 * height / 5);
//	//glVertex2d(width, 2 * height / 5);
//	//glVertex2d(width, 3 * height / 5);
//	//glVertex2d(width / 6, 3 * height / 5);
//	//glEnd();
//
//	// text for choose player texture ^^
//	glColor3ub(100, 100, 100);
//	glEnable(GL_TEXTURE_2D);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glColor4d(1.0, 1.0, 1.0, 1.0);
//	glBindTexture(GL_TEXTURE_2D, textIds[0]);
//	glBegin(GL_QUADS);
//	glTexCoord2d(1.0, 0.0);
//	glVertex2d(width / 5, 2.25 * height / 5);
//	glTexCoord2d(0.0, 0.0);
//	glVertex2d(width / 3, 2.25 * height / 5);
//	glTexCoord2d(0.0, 1.0);
//	glVertex2d(width / 3, 2.75 * height / 5);
//	glTexCoord2d(1.0, 1.0);
//	glVertex2d(width / 5, 2.75 * height / 5);
//	glEnd();
//
//	glDisable(GL_BLEND);
//	glDisable(GL_TEXTURE_2D);
//
//	// label for first car
//	impact.setColorRGB(1.0, .85, 0); //(197, 179, 88);		//vegas gold
//	impact.drawText("GOLDEN", width / 4.35, 3 * height / 5, 0.3);
//	impact.drawText("STALLION", width / 4.5, 3.25 * height / 5, 0.3);
//
//	// leave height/5 space for textID player options
//	glColor3ub(0, 0, 0); // black for the exit box...? maybe red?
//	glBegin(GL_QUADS);
//	glVertex2d(width / 6, 4 * height / 5);
//	glVertex2d(width, 4 * height / 5);
//	glVertex2d(width, height);
//	glVertex2d(width / 6, height);
//	glEnd();
//	// text for end box
//	glColor3ub(255, 255, 255);
//	glRasterPos2d(width / 2 - 20, 4.65 * height / 5);
//	YsGlDrawFontBitmap20x32("EXIT GAME");
//	// outline for hovering over buttons
//	glColor3ub(255, 0, 0);
//	glLineWidth(5);
//
//	if (onPlay) {
//		glBegin(GL_LINE_LOOP);
//		glVertex2d(width / 6, height / 5);
//		glVertex2d(width, height / 5);
//		glVertex2d(width, 2 * height / 5);
//		glVertex2d(width / 6, 2 * height / 5);
//		glEnd();
//	}
//	if (onExit) {
//		glBegin(GL_LINE_LOOP);
//		glVertex2d(width / 6, 4 * height / 5);
//		glVertex2d(width, 4 * height / 5);
//		glVertex2d(width, height);
//		glVertex2d(width / 6, height);
//		glEnd();
//	}
//}
//
//void ViewManager::playScreen(int locX, int locY, int lb)
//{
//	bool onExit = false;
//	if (locX > 0 && locX < width / 6)
//		if (locY > 4.5 * height / 5 && locY < height)
//			onExit = true;
//
//	if (onExit && lb)
//		exitDesired = true;
//
//	glColor3ub(192, 192, 192);
//	glBegin(GL_QUADS);
//	glVertex2d(0, 0);
//	glVertex2d(width, 0);
//	glVertex2d(width, height / 5);
//	glVertex2d(0, height / 5);
//	glEnd();
//	// draw the game title  >>>>>> will need to tweak the location <<<<<<
//	oldEnglish.setColorRGB(255, 255, 255);
//	oldEnglish.drawText("Maze Mania", 40, height / 5, 1.3);
//	// box at the size of the screen for controls and leaderboard
//	glColor3ub(192, 192, 192);
//	glBegin(GL_QUADS);
//	glVertex2d(0, height / 5);
//	glVertex2d(width / 6, height / 5);
//	glVertex2d(width / 6, height);
//	glVertex2d(0, height);
//	glEnd();
//	// controls text >>>>> will need to tweak the location <<<<< 
//		// this could be a user preference change (wasd or arrows)
//	impact.setColorRGB(0, 0, 0);
//	impact.drawText("CONTROLS", 5, height / 5 + 50, 0.5);
//	impact.drawText("^", 55, height / 5 + 90, 0.5);
//	impact.drawText("<-  v  ->", 10, height / 5 + 110, 0.5);
//	// leaderboard text >>>> will need to tweak location <<<<
//	// will need access to some kind of array of previous scores
//		// likely another function that reads from a text file and creates a vector or array
//	impact.drawText("LEADERBOARD", 10, height / 2, 0.35);
//	// exit button on game screen
//	glColor3ub(0, 0, 0);
//	glBegin(GL_QUADS);
//	glVertex2d(0, 4.5 * height / 5);
//	glVertex2d(width / 6, 4.5 * height / 5);
//	glVertex2d(width / 6, height);
//	glVertex2d(0, height);
//	glEnd();
//	glColor3ub(255, 255, 255);
//	glRasterPos2d(15, 4.8 * height / 5);
//	YsGlDrawFontBitmap12x16("EXIT GAME");
//
//	if (onExit) {
//		glColor3ub(255, 0, 0);
//		glBegin(GL_LINE_LOOP);
//		glVertex2d(0, 4.5 * height / 5);
//		glVertex2d(width / 6, 4.5 * height / 5);
//		glVertex2d(width / 6, height - 2);
//		glVertex2d(0, height - 2);
//		glEnd();
//	}
//
//	// placeholder for where the game map will go
//	glColor3ub(130, 200, 235);
//	glBegin(GL_QUADS);
//	glVertex2d(width / 6, height / 5);
//	glVertex2d(width, height / 5);
//	glVertex2d(width, height);
//	glVertex2d(width / 6, height);
//	glEnd();
//	/*glColor3ub(0, 0, 0);
//	glRasterPos2d(width / 3 + 20, 3 * height / 5);
//	YsGlDrawFontBitmap20x32("THE GAME GOES HERE");*/
//}
