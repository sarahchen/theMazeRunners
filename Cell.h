/*
Nate Winneg - Project Component Code

This class is resposible for creating the
cell object and the assiciated variables
and functionalities. It isn't fully fleshed out
yet and the row column scheme isn't fully
finalized so there are some sketches 
commented in to show what I was going for.
*/

#pragma once
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "fssimplewindow.h"

#define PI 3.1415926
#define CELL_RADIUS 25

class Cell {

private:
	struct node {
		double x;
		double y;
	};

	int rowSize, colSize;
	double radius;
	double centerX, centerY;
	bool seen;		// visited if true;	
	int label;

	// list of neighbors that do not have a wall in between them
	std::vector<int> availableNeighbors;

	// Nodes that make up the hexagon corners
	/*
	* corresponding node location to vector index values;
		   0__1
		  5/  \2
		   \__/
		   4  3
	*/
	std::vector<node> cellNodes;

public:

	// array of neighbor labels --> 
	// index 0 is above neighbor and go around to the right
	int neighbors[6];
	bool walls[6];	// if true, there is a wall

	Cell() {};

	// Initialize Cell
	Cell(int alabel, int mazeRowSize, int mazeColSize, double cx, double cy);

	void setNodes();

	bool isSeen() { return seen; }
	void setSeen() { seen = true; }
	// return desired node's x and y coordinates
	int getNodeX(int nodeIdx) { return cellNodes[nodeIdx].x; }
	int getNodeY(int nodeIdx) { return cellNodes[nodeIdx].y; }
	// return centerX and centerY of the cell
	int getCenterX() { return centerX; }
	int getCenterY() { return centerY; }

	// returns the label of the neighbor at the desired location (top bottom etc)
	int getNeighbor(int idx);
	// returns a vector of labels of available cells to move to
	std::vector<int> getAvailableNeighbors();
	// returns a vector of all neighbor labels
	std::vector<int> getAllNeighbors();

	// returns the array of walls
	bool getWalls() { return walls; }
	// sets wall at location index
	void setWall(int index, bool set) { 
		walls[index] = set; 
	}

	// draws the cell with corresponding walls
	void drawCell();
};
