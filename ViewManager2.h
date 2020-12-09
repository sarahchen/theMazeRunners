/*
Maze Mania
ViewManager2.cpp
Created by Nate Winneg, Ryder Babik
11/19/20
Engineering Computation 24780-B
Header file that orchestrates the model and view
*/

#pragma once
#include <chrono>
#include <vector>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include "fssimplewindow.h"
#include "yspng.h"
#include "yssimplesound.h"
#include "GraphicFont.h"
#include "Character.h"
#include "Model.h"
#include "soundHandler.h"

//enum carType { lambo, F1, truck, regCar };

class ViewManager {

private:
	//window width and height
	int width, height;
	bool isPlaying = false;		//user is playing game
	bool exitDesired = false;		//user has clicked on the exit button

	////model object
	Model theModel;
	
	carType selectedCar;		//enum for the type of car user chose
	bool lamboselected;			//booleans to know if car was the selected one
	bool F1selected;
	bool truckSelected;
	bool carSelected;

	bool newVehicleChosen;		//bool to register event of switching selected car
	int selectionIndex;			//"index" or car number of car selected
	int color = 0;
	int prevLevel;

	vector<pair<string, int>> leaders;		//vector to hold the leaderboard data

	vector<Sound> theSounds;

	//graphics to be displayed
	vector<GLuint> textIds, textIdsTruck, textIdsCar, textIdsF1, textIdsRegCar, textIdsItems;
	YsRawPngDecoder F1Pic, basicCarPic;
	YsRawPngDecoder truckPicOrange, truckPicBlue, truckPicGreen;
	YsRawPngDecoder carPicYellow, carPicBlue, carPicRose;
	YsRawPngDecoder F1PicRed, F1PicBlue, F1PicGreen;
	YsRawPngDecoder regCarPicBlue, regCarPicPurple, regCarPicGray;
	YsRawPngDecoder coffeePic, starPic, moneyBagPic;
	YsRawPngDecoder enemyPic;
	YsRawPngDecoder keyboardPic;
	double transpR, transpG, transpB;

	////texts to be used for strings
	ImpactFont impact; 
	OldEnglishFont oldEnglish;
	GaramondFont garamond;
	ComicSansFont comicSans;

	////sounds to be played
	//YsSoundPlayer soundPlayer;
	//YsSoundPlayer::SoundData EyeTiger, Sword, Splat, Round1, Round2, FinalRound, EvilLaugh, scream, celebration, error;


public:
	//class constructor
	ViewManager();

	void initialize();

	//basically runs the entire program
	void manage();

	//pass it the decoder name, the png file name, and whether you want to add it to the text id vector for the fruits or robot
	void createTextId(YsRawPngDecoder& png, const char* fileName, vector<GLuint>& textIds);

	//gets all the text IDs ready to then be called as a parameter in the createTextId function
	void prepareTheTextIds();

	void getRandomItem(itemType& item, GLuint& Id);

	////load the leaderboard
	//void load();

	////display the loaded  leaderboard data
	//void drawLeaders();

	////save the leaderboard
	//void save();

	//// Driver function to sort the vector elements 
	//// by second element of pairs 
	//bool sortbysec(const pair<int, int>& a,
	//	const pair<int, int>& b);

	//void readFile(std::ifstream& inFile);
	//// reads the given file for accesing saved leaderboard

	//void writeFile(std::ofstream& outFile);
	//// loop through the leaders vector and writes to output file

	//get methods so that each class can be in charge of assigning its own text ID to graphics
	GLuint getCarID() { return textIds[0]; }

	// displays the start screen
		// locX and locY are mouse locations and lb is the left button part of mouse event
	void startScreen(int locX, int locY, int lb, int mouseEvent);

	// displays the screen during gameplay
		// pass it the same variables as startScreen for the exit button
	void playScreen(int locX, int locY, int lb);

	void levelUp();

	//screen for at end of game to save score
	void saveScreen();

	void gameOverScreen();

	//draws the static elements that are present in both start and play screens (such as game title)
	void drawStaticElements();

	//draws the four cars to choose from
	void drawCars(int locX, int locY, int lb);

	//draws a blinking outline around the car name being hovered over
	void drawHoverOutline(int leftX, int topY, int rightX, int bottomY);

	//when a new car is selected, first clear previous selection
	void clearCarSelection();

	//draws new car selection
	void drawSelection(int leftX, int rightX, int topY, int bottomY);

	//outlines color section button
	void drawColorSelect(int locX, int locY, int lb);
	void monitorColorSelect(int locX, int locY, int lb, int mouseEvent);

	//adjusts the transparency of the selected car
	void colorSelect(int locX, int locY, int lb);
	//void setCarSelection(bool &selection, bool *selectionOptions);
	//void drawHoverOutline(int leftX, int topY, int rightX, int bottomY);
};


//public:
//	//class constructor
//	ViewManager();
//
//	//basically runs the entire program
//	void manage();
//
//
//	//pass it the decoder name, the png file name, and whether you want to add it to the text id vector for the fruits or robot
//	void createTextId(YsRawPngDecoder& png, const char* fileName, vector<GLuint>& textIds);
//
//	//gets all the text IDs ready to then be called as a parameter in the createTextId function
//	void prepareTheTextIds();
//
//
//	//get methods so that each class can be in charge of assigning its own text ID to graphics
//	GLuint getCarID() { return textIds[0]; }
//
//	// displays the start screen
//		// locX and locY are mouse locations and lb is the left button part of mouse event
//	void startScreen(int locX, int locY, int lb);
//	// displays the screen during gameplay
//		// pass it the same variables as startScreen for the exit button
//	void playScreen(int locX, int locY, int lb);
//};
