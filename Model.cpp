#pragma once

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
	//healthPercentage = 100;
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

void Model::loadLeaders()
{
	string inFileName = "leaderboard.txt";
	ifstream inFile;

	inFile.open(inFileName);

	if (inFile.is_open()) {
		readFile(inFile);
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

void Model::initializeCharacter(carType car, GLuint Id)
{
	theCharacter.setCarInfo(width / 6 + (2.5 / 6 * width), (3 * height / 5), car, Id);
}

void Model::initializeMaze()
{
	theMaze.generateMaze();
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

void Model::update(ViewManager& theManager)
{
	// Testing of validCharMove
	// If it is a validCharMove, there is no obstacle

	// Test Front
	if (theMaze.validCharMove(theCharacter, 0)) {
		theCharacter.setObstacleFront(false);
	}
	else {
		theCharacter.setObstacleFront(true);
	}
	// Test Back
	if (theMaze.validCharMove(theCharacter, 1)) {
		theCharacter.setObstacleBack(false);
	}
	else {
		theCharacter.setObstacleBack(true);
	}

	this->updateHealth();
	theCharacter.updateKinematics(.1);
	theCharacter.draw();
	theMaze.drawMaze();

	if (fabs(theCharacter.getVel()) > 0.3) {
		theMaze.setPlayerCell(theCharacter);
	}

	for (int i = 0; i < numEnemies; i++) {
		theEnemies[i].findBestPath(theMaze);
		theEnemies[i].move();
		theEnemies[i].draw();
	}
}

bool Model::caught()
{
	for (int i = 0; i < numEnemies; i++) {

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

void Model::readFile(ifstream& inFile)
{
	using namespace std;

	string currLine;
	stringstream currStream;

	string name;
	int score;

	vector<pair<string, int>> temp;

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
