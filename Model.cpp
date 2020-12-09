#pragma once

#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <algorithm>
#include <functional>
#include <string>
#include <time.h>       /* time */
#include <iterator> // for iterators 
#include <vector> // for vectors 
//#include <bits/stdc++.h> 
#include <filesystem>
#include "Model.h"
#include "ViewManager2.h"
#include "ysglfontdata.h"


using namespace std;

const int LEADERCOUNT = 5; //number of top scores we want to track

Model::Model()
{
	//window size and simulation time
	FsGetWindowSize(width, height);
	damaged = false;
	soundTrig = 5;
	currLevel = 1;
	//healthPercentage = 100;
	 /* initialize random seed: */
	//srand(time(NULL));
}



void Model::loadLeaders()
{
	string inFileName = "leaderboard.txt";
	ifstream inFile;

	inFile.open(inFileName);

	if (inFile.is_open()) {
		readLeaderFile(inFile);
		inFile.close();
	}
	else
		cout << "Was not able to open " << inFileName << " for input. " << endl;
}


void Model::drawLeaders()
{
	if (leaders.size() != 0) {
		//draw background for leaderboard
		glColor3ub(230, 230, 255); // black for the exit box...? maybe red?
		glBegin(GL_QUADS);
		glVertex2d(3, height / 2 + 5);
		glVertex2d(width/8, height / 2 + 5);
		glVertex2d(width/8, height / 2 + leaders.size() * 30 + 10);  //200
		glVertex2d(3, height / 2 + leaders.size() * 30 + 10);
		glEnd();
	}

	// Declaring iterator to a vector 
	vector<pair<string, int>>::iterator ptr;

	string name, score;
	int hei = height / 2 + 30;

	int i = 0;
	for (auto iter : leaders) {
		comicSans.setColorRGB(0, 0, 0);
		name = iter.first;
		comicSans.drawText(name, 5, hei, 0.18);
		score = to_string(iter.second);
		comicSans.drawText(score, 115, hei, 0.18);
		hei += 30;
	}
}

void Model::clearLeaders()
{
	leaders.clear();
	saveLeaders();
	loadLeaders();
}

void Model::initialize() 
{
	theEnemies.clear();			//clear out enemies
	theItems.clear();			//clear out items
	theCharacter.setScore(0);	//clear score

	loadLeaders();  // load the leaderboard file

}

void Model::initializeCharacter(carType car, GLuint Id)
{
	theCharacter.setCarInfo(width / 6 + (2.5 / 6 * width), (3 * height / 5), car, Id);
}

void Model::initializeMaze()
{
	theMaze.initializeGrid();
	theMaze.generateMaze(currLevel);
	theMaze.setPlayerCell(theCharacter);
}

void Model::initializeEnemy(GLuint Id)
{
	for (int i = 0; i < numEnemies; i++) {
		Enemy newEnemy;
		theEnemies.push_back(newEnemy);
		theEnemies[i].spawn(theMaze.getRandomCell(), Id);

		//set enemy speeds randomly (between 1500-3000ms)
		int randRate = rand() % 1500 + 1500;
		theEnemies[i].setMoveTimeThresh(randRate);
	}
}

void Model::addItem(itemType item, GLuint Id)
{
	//int numCells = theMaze.getNumRows() * theMaze.getNumCols();
	//int cell = rand() % numCells;

	//int xPos = width / 8 + rand() % int(width * 7/8);
	//int yPos = height / 5.5 + rand() % int(height * 4.5/5.5);

	Item newItem(theMaze.getRandomCell(), item, Id, theMaze);
	theItems.push_back(newItem);
}

void Model::checkItemCollected()
{
	//vector<bool> temp;
	//for (int i = 0; i < theItems.size(); i++) {
	//	temp.push_back(false);
	//}

	for (int i = 0; i < theItems.size(); i++) {
		if (theMaze.getPlayerCell() == theItems[i].getCell()) {
			prevPickupTime = chrono::system_clock::now();
			//temp[i] = true;
			theCharacter.addScore(theItems[i].getPointVal());
			soundTrig = theItems[i].getItemType();
			theItems.erase(theItems.begin() + i);
			break;
		}
	}

	//for (int i = 0; i < theItems.size(); i++) {
	//	if (temp[i] == true) {
	//		
	//	}
	//	else
	//		soundTrig = 5;
	//}
}

void Model::updateHealth()
{
	if (caught()) {
		auto currentTime = chrono::system_clock::now();
		double elapsedTime = chrono::duration_cast<std::chrono::milliseconds> (currentTime - prevDamageTime).count();

		if (elapsedTime > damageTimeThresh) {
			theCharacter.setHealth(theCharacter.getHealth() - damageIncrement);
			prevDamageTime = currentTime;
		}
	}
}

void Model::updateScore()
{
	auto currentTime = chrono::system_clock::now();
	double elapsedTime = chrono::duration_cast<std::chrono::milliseconds> (currentTime - prevScoreTime).count();
	if (elapsedTime > 10) {	//one point per 10ms

		theCharacter.setScore(theCharacter.getScore() + 1);
		prevScoreTime = currentTime;
	}
	//cout << "current score = " << theCharacter.getScore() << '\n';
}

void Model::updateLevel()
{
	int score = theCharacter.getScore();
	int levelIncrementThresh = 1000; //increment the level every 1000 points

	double levelCheck = floor((double)score / (double)levelIncrementThresh) + 1;
	if (levelCheck > currLevel) incrementLevel();

	//cout << "current level = " << currLevel << '\n';
	//cout << " level check = " << levelCheck << '\n';
}

void Model::incrementLevel()
{
	//do anything else here that will happen with an extra level increment
	currLevel++;
}

void Model::update(ViewManager &theManager)
{
	 //testing of validcharmove
	// if it is a validcharmove, there is no obstacle

	// check front
	if (theMaze.validCharMove(theCharacter, 0)) {
		theCharacter.setObstacleFront(false);
	}
	else {
		theCharacter.setObstacleFront(true);
	}
	// check back
	if (theMaze.validCharMove(theCharacter, 1)) {
		theCharacter.setObstacleBack(false);
	}
	else {
		theCharacter.setObstacleBack(true);
	}

	theMaze.drawMaze();

	if (fabs(theCharacter.getVel()) > 0.3) {
		theMaze.setPlayerCell(theCharacter);
	}

	for (int i = 0; i < theEnemies.size(); i++) {
		theEnemies[i].findBestPath(theMaze);
		theEnemies[i].move();
		theEnemies[i].draw();
	}

	checkItemCollected();
	for (int i = 0; i < theItems.size(); i++) theItems[i].draw();

	//add item to the model!!
	static int counter = 0;
	if (counter == 100) {
		//cout << "New item!!";
		itemType theItem;
		GLuint itemId;
		theManager.getRandomItem(theItem, itemId);
		addItem(theItem, itemId);
		counter = 0;
	}
	else counter++;

	updateHealth();
	updateScore();
	updateLevel();
	theCharacter.updateKinematics(.1);
	
	theCharacter.draw();

}

bool Model::caught()
{
	for (int i = 0; i < theEnemies.size(); i++) {

		int enemyCell = theEnemies[i].getLocation().getLabel();
		int playerCell = theMaze.getPlayerCell();
		//chrono::time_point<chrono::steady_clock> start;

		if (enemyCell == playerCell /*&& playerCell != prevPlayerCell*/) {
			//prevPlayerCell = playerCell;
			//start = chrono::steady_clock::now();
			return true;
		}
		//else if (enemyCell == playerCell && playerCell == prevPlayerCell) {
		//	auto current = chrono::duration_cast<chrono::seconds>(std::chrono::steady_clock::now() - start).count();
		//	if (fmod(current, 3.0) == 0.0 && damaged == false) {
		//		damaged = true;
		//		return true;
		//	}
		//	else if (fmod(current, 3.0) == 0.0 && damaged == true) {
		//		//return false;
		//	}
		//	else {
		//		damaged = false;
		//		//return false;
		//	}
		//}
		//else {
		//	damaged = false;
		//	//return false;
		//}
	}
	return false;
}

void Model::saveLeaders()
{
	string outFileName = "leaderboard.txt";
	ifstream inFile;
	ofstream outFile;

	outFile.open(outFileName);

	if (outFile.is_open()) {
		writeFile(outFile);
		outFile.close();
	}
}

void Model::addScore(string name)
{
	int score = getPlayerScore();
	leaders.push_back(make_pair(name, score));
	sortLeaders();
	while (leaders.size() > 5)
		leaders.pop_back();
}

void Model::sortLeaders()
{
	sort(leaders.begin(), leaders.end(), Model::sortbysec);
}

bool Model::isTopScore(int score)
{
	if (leaders.size() < 5) return true;
	pair<string, int> lastLeader = leaders[LEADERCOUNT - 1];
	return score >= lastLeader.second;
}

int Model::getPlayerScore()
{
	return theCharacter.getScore();
}

//sort by score in descending order
bool Model::sortbysec(const pair<string, int>& a, const pair<string, int>& b)
{
	return a.second > b.second;
}

void Model::readLeaderFile(ifstream& inFile)
{
	using namespace std;

	string currLine;
	stringstream currStream;

	string name;
	int score;

	vector<pair<string, int>> temp;

	leaders.clear();

	while (!inFile.eof()) //&& not '\n'
	{

		getline(inFile, currLine);
		if (!(currLine.empty())) {
			currStream.str(currLine);
			currStream >> name >> score;
			currStream.clear();
			temp.push_back(make_pair(name, score));
		}
	}

	// Using sort() function to sort by 2nd element 
	// of pair 
	sort(temp.begin(), temp.end(), Model::sortbysec); //comparePairs);
		/*if (currLine.find(":") != string::npos) {
			string name = StringPlus::trim(0, currLine.substr(currLine.find(":") - 1));
			int score = StringPlus::trim(currLine.substr(currLine.find(":") + 1));

		}*/

	for (int i = 0; i < min(int(LEADERCOUNT), int(temp.size())); i++) {  				// SET HOW MANY LEADERS WE WANT TO SHOW
		leaders.push_back(temp[i]);
	}

}

void Model::writeFile(ofstream& outFile)
{
	// Declaring iterator to a vector 
	vector<pair<string, int>>::iterator ptr;

	//for (ptr = leaders.begin(); ptr < leaders.end(); ptr++) {
	//	while (int i = 0 < 8)
	//		cout << *ptr << endl;
	//}
	int i = 0;
	for (auto iter : leaders) {
		outFile << iter.first << "\t" << iter.second << endl;
	}

}
