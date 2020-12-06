/*
Maze Mania
Character.h
Created by Ryder Babik
11/19/20
Engineering Computation 24780-B
Header file for item object for game
*/


#pragma once

#include <string>
#include "fssimplewindow.h"
#include "Character.h"
#include "Maze.h"

using namespace std;

//struct vertex {
//	double x, y, z;
//};

enum itemType { coffee, star, moneyBag };

class Item {
private:
	vertex pos;
	itemType thisItem;
	bool collected;
	GLuint textID;
	int wid, hei;
	int pointVal;
	int cell;

	Cell itemCell;

public:
	//class constructor
	Item(int cellNum, itemType item, GLuint Id, Maze theMaze);

	void assignPointVal();

	int getCell() { return cell; }
	//int getXpos() { return pos.x; }
	//int getYpos() { return pos.y; }
	int getPointVal() { return pointVal; }

	void setPos(int newX, int newY) { pos.x = newX, pos.y = newY; }

	void draw();

};