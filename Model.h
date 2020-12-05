/*
Maze Mania
Model.cpp
Created by Ryder Babik
11/19/20
Engineering Computation 24780-B
Header file that creates the model
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
#include "Maze.h"
#include "Enemy.h"

class Model {

private:
	//window width and height
	int width, height;
	ComicSansFont comicSans;

	Character theCharacter;
	Maze theMaze;
	Enemy theEnemy;

	vector<pair<string, int>> leaders;		//vector to hold the leaderboard data
	// stored for damage mechanic (bool caught())
	int prevPlayerCell;
	bool damaged;
	//int healthPercentage;

public:
	//class constructor
	Model();

	int getHealthPercentage() { return theCharacter.getHealth(); }
	void setHealthPercentage(int health) { theCharacter.setHealth(health); }
	void updateHealth();

	//load the leaderboard
	void load();

	//save the leaderboard
	void save();

	//add score to leaderboard
	void addScore(string name);

	//sort updated leaderboard
	void sortLeaders();

	//returns true if score is in top five
	bool isTopScore(int score);

	//gets the player's score
	int getPlayerScore();

	// Driver function to sort the vector elements 
	// by second element of pairs 
	static bool sortbysec(const pair<string, int>& a,
		const pair<string, int>& b);

	void readFile(std::ifstream& inFile);
	// reads the given file for accesing saved leaderboard

	void writeFile(std::ofstream& outFile);
	// loop through the leaders vector and writes to output file

	//display the loaded leaderboard data
	void drawLeaders();

	//clears the stored leaders
	void clearLeaders();

	//assigns initial parameters to the character
	void initializeCharacter(carType car, GLuint Id);

	void initializeMaze();
	void initializeEnemy();

	//updates all aspects of the gameplay
	void update(ViewManager& theManager);

	void moveCharForward() { theCharacter.goForward(); }
	void moveCharBackward() { theCharacter.goBackward(); }
	void rotateChar(double angleAdjust) { theCharacter.adjustOrientation(angleAdjust); }
	void zeroCharForce() { theCharacter.setForce(0); }
	void updateCharId(carType car, GLuint Id) { theCharacter.assignId(car, Id); }

	// returns true if damage required
	// damage hit if enemy catches the player and again for every 3 seconds they are on the same cell
	bool caught();
};