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

//keep track of which time of item this is
enum itemType { coffee, star, moneyBag };

class Item {
private:
	//basic parameters for the item
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
	Item(Cell inputCell, itemType item, GLuint Id, Maze theMaze);

	//get and sets for the item
	void assignPointVal();
	int getCell() { return cell; }
	int getPointVal() { return pointVal; }
	void setPos(int newX, int newY) { pos.x = newX, pos.y = newY; }

	//draw the item in the game
	void draw();

	//get the item enum type
	int getItemType() { return thisItem; }

};
