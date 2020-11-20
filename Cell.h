#pragma once
#include <vector>
#include <iostream>
#include <stdlib.h>
#include "fssimplewindow.h"

#define PI 3.1415926

class Cell {
	// do we need "nodes" for each cell to draw it? 
private:
	struct node {
		double x;
		double y;
	};
	int rowSize, colSize;
	double radius;
	double centerX, centerY; // we might be able to get these from the nodes label and maze dimensions
	bool seen;		// visited if true;	
	int label;									
	int neighbors[6];// array of neighbor labels --> index 0 is above neighbor and go around to the right
	bool walls[6];	// if true, there is a wall
	std::vector<int> availableNeighbors; // individual cells don't have access to other cells states only their labels...?
	std::vector<node> cellNodes;

public:
	Cell() {};
	// initialize cell --> as full walls?
	Cell(int alabel, int mazeRowSize, int mazeColSize, int cX, int cY);

	/*
	* corresponding node location to vector index values;
   0__1
  5/  \2
   \__/
   4  3
	*/

	bool isSeen() { return seen; }
	void setSeen() { seen = true; }
	int getCenterX() { return centerX; }
	int getCenterY() { return centerY; }

	void setNodes(double centerX, double centerY);
	// return desired node's x and y coordinates
	int getNodeX(int nodeIdx) { return cellNodes[nodeIdx].x; } 
	int getNodeY(int nodeIdx) { return cellNodes[nodeIdx].y; }

	/*
	// choose an unseen neighbor and remove the wall between them -- return label of neighbor
	int searchNext();

	// check if cell has any unseen neighbors  --> I think we have to do this in the maze class
	bool hasUnseenNeighbors();
	*/

	// sets list to the list of all neighbor indices
	void getAllNeighbors(int list[6]) { list = neighbors; }

	// returns the label of the neighbor at the desired index
	int getNeighbor(int idx);
	// returns a vector of labels of available cells to move to
	std::vector<int> getAvailableNeighbors(); 

	// returns the array of walls
	bool getWalls() { return walls; }
	// sets wall at location index
	void setWall(int index, bool set) { walls[index] = set; }

	// draws the cell with corresponding walls --> local coordinates
	void drawCell(double centerX, double centerY);
};