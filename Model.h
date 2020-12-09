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
#include "Item.h"
#include "soundHandler.h"

class Model {

private:
	//window width and height
	int width, height;
	ComicSansFont comicSans;

	Character theCharacter;
	Maze theMaze;
	vector<Enemy> theEnemies;
	int numEnemies = 3;
	
	vector<Item> theItems;
	chrono::system_clock::time_point prevCoffeePickupTime = chrono::system_clock::now(); //time when coffee was grabbed
	chrono::system_clock::time_point prevStarPickupTime = chrono::system_clock::now(); //time when star was grabbed
	int itemEffectTimeThresh = 5000; //how much time (in ms) that item effects will last

	//flags for whether item effects are happening
	bool coffeeSet = false;
	bool starSet = false;


	chrono::system_clock::time_point gameStartTime = chrono::system_clock::now(); //time when the game was started
	chrono::system_clock::time_point prevScoreTime = chrono::system_clock::now(); //time when the score was last incremented

	int currLevel = 1; //level the player is at...starts at 1, goes up with points

	vector<pair<string, int>> leaders;		//vector to hold the leaderboard data
	// stored for damage mechanic (bool caught())

	int prevPlayerCell;
	bool damaged;
	//int healthPercentage;

	//timestamp of the last time the character was damaged
	chrono::system_clock::time_point prevDamageTime = chrono::system_clock::now();
	double damageTimeThresh = 1000;		//how much time (in ms) to wait until dealing damage again
	double damageIncrement = 1;		//increment for dealing damage

	int soundTrig;

public:
	//class constructor
	Model();

	int getSoundTrig() { return soundTrig; }
	void setSoundTrig(int input) { soundTrig = input; }
	int getHealthPercentage() { return theCharacter.getHealth(); }
	void setHealthPercentage(int health) { theCharacter.setHealth(health); }
	void updateHealth();

	//load the leaderboard
	void loadLeaders();

	//save the leaderboard
	void saveLeaders();

	//add score to leaderboard
	void addScore(string name);

	//sort updated leaderboard
	void sortLeaders();

	//returns true if score is in top five
	bool isTopScore(int score);

	//gets the player's score
	int getPlayerScore();
	int getCurrLevel() { return currLevel; }
	void setCurrLevel(int i) { currLevel = i; }

	// Driver function to sort the vector elements 
	// by second element of pairs 
	static bool sortbysec(const pair<string, int>& a,
		const pair<string, int>& b);

	void readLeaderFile(std::ifstream& inFile);
	// reads the given file for accesing saved leaderboard

	void writeFile(std::ofstream& outFile);
	// loop through the leaders vector and writes to output file

	//display the loaded leaderboard data
	void drawLeaders();

	//clears the stored leaders
	void clearLeaders();

	//runs initialization on the Model
	void initialize();

	//assigns initial parameters to the character
	void initializeCharacter(carType car, GLuint Id);

	void initializeMaze();
	void initializeEnemy(GLuint Id);

	//adds an item to the game
	void addItem(itemType item, GLuint Id);

	//check to see if item is collected
	void checkItemCollected();
	void checkItemEffects();

	//updates the player score based on elapsed time
	void updateScore();

	//updates the level based on the player score
	void checkLevel();
	void incrementLevel();

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
