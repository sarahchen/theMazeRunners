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

Model::Model()
{
	//window size and simulation time
	FsGetWindowSize(width, height);
	theMaze.generateMaze();
}

void Model::load()
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

	//draw background for leaderboard
	glColor3ub(230, 230, 255); // black for the exit box...? maybe red?
	glBegin(GL_QUADS);
	glVertex2d(2, height / 2 + 5);
	glVertex2d(130, height / 2 + 5);
	glVertex2d(130, height / 2 + leaders.size() * 40 + 10);  //200
	glVertex2d(2, height / 2 + leaders.size() * 40 + 10);
	glEnd();

	// Declaring iterator to a vector 
	vector<pair<string, int>>::iterator ptr;

	string display;
	int hei = height / 2;
	int i = 0;

	for (auto iter : leaders) {
		if (i < 7) {
			display = iter.first + "   " + to_string(iter.second);
			hei += 40;
			i++;
			comicSans.setColorRGB(0, 0, 0);
			comicSans.drawText(display, 5, hei, 0.2);
		}
		i++;
	}
}

void Model::initializeCharacter(carType car, GLuint Id)
{
	theCharacter.setCarInfo(width / 6 + (2.5 / 6 * width), (3 * height / 5), car, Id);
}

void Model::update(ViewManager& theManager)
{

	//Sarah this is for you to complete! The front looks at the two two front corners of the car, and the back one looks at the back two corners
	//here are the four corner positions! Return true if neither corner is hitting an obstacle, else return false

	double ax = theCharacter.getAx(), ay = theCharacter.getAy(), bx = theCharacter.getBx(), by = theCharacter.getBy();
	double cx = theCharacter.getCx(), cy = theCharacter.getCy(), dx = theCharacter.getDx(), dy = theCharacter.getDy();

	// if (theMaze.validCharMove(ax,ay,dx,dy)) {willHitObstacleFront = false;}
	// else { willHitObstacleFront = true; }

	// if (theMaze.validCharMove(bx,by,cx,cy)) {willHitObstacleback = false;}
	// else { willHitObstacleBack = true; }

	// theCharacter.updateKinematics(.1);
	// theCharacter.draw(theManager);
	theMaze.drawMaze(theManager);
}

void Model::save()
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

//sort by score in descending order
bool Model::sortbysec(const pair<int, int>& a, const pair<int, int>& b)
{
	return (a.second > b.second);
}

void Model::readFile(ifstream& inFile)
{
	using namespace std;

	string currLine;
	stringstream currStream;

	string name;
	int score;

	while (!inFile.eof()) //&& not '\n'
	{
		getline(inFile, currLine);
		if (!(currLine.empty())) {
			currStream.str(currLine);
			currStream >> name >> score;
			currStream.clear();
			leaders.push_back(make_pair(name, score));
		}
	}

	// Using sort() function to sort by 2nd element 
	// of pair 
	sort(leaders.begin(), leaders.end()); //, sortbysec);
		/*if (currLine.find(":") != string::npos) {
			string name = StringPlus::trim(0, currLine.substr(currLine.find(":") - 1));
			int score = StringPlus::trim(currLine.substr(currLine.find(":") + 1));

		}*/

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
