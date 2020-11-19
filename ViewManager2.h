/*
DEMO
ViewManager.h
Created by Ryder Babik
11/1/20
Engineering Computation 24780-B
header file for the view manager class
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

class ViewManager {

private:
	//window width and height
	int width, height;
	bool isPlaying = false;
	bool exitDesired = false;

	////model object
	//Model theModel;

	//graphics to be displayed
	vector<GLuint> textIds;
	YsRawPngDecoder carPic;

	////texts to be used for strings
	ImpactFont impact; 
	OldEnglishFont oldEnglish;
	//ComicSansFont comicSans;
	//GaramondFont garamond;

	////sounds to be played
	//YsSoundPlayer soundPlayer;
	//YsSoundPlayer::SoundData EyeTiger, Sword, Splat, Round1, Round2, FinalRound, EvilLaugh, scream, celebration, error;


public:
	//class constructor
	ViewManager();

	//basically runs the entire program
	void manage();


	//pass it the decoder name, the png file name, and whether you want to add it to the text id vector for the fruits or robot
	void createTextId(YsRawPngDecoder& png, const char* fileName, vector<GLuint>& textIds);

	//gets all the text IDs ready to then be called as a parameter in the createTextId function
	void prepareTheTextIds();


	//get methods so that each class can be in charge of assigning its own text ID to graphics
	GLuint getCarID() { return textIds[0]; }

	// displays the start screen
		// locX and locY are mouse locations and lb is the left button part of mouse event
	void startScreen(int locX, int locY, int lb);
	// displays the screen during gameplay
		// pass it the same variables as startScreen for the exit button
	void playScreen(int locX, int locY, int lb);
};