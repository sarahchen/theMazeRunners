#pragma once

#include <stdio.h>
#include <unordered_map>
#include <stack>
#include "Cell.h"
#include "Character.h"

#define GRID_SIZE 160

class Cell;
class ViewManager;

class Maze {
private:

    // Number of rows and columns in the Grid
    int numRows, numCols;

    std::unordered_map<int, Cell> Grid;
    std::stack<int> VisitedCells;

    Cell playerCell; //cell where the player is currently located
    
public:
    Maze();

    //returns the unordered map of cells that compose the maze grid
    std::unordered_map<int, Cell> getGrid() { return Grid; }

    int getNumRows() { return numRows; }
    int getNumCols() { return numCols; }

    Cell getRandomCell();
    Cell getCell(int label);

    // iterates through all currCell's neighbors
    // populates unseenList w lables of unseen neighbors
    void getUnseenNeighbors(int currCell, std::vector<int>& unseenList);

    // gets unseen neighbors and randomly chooses a label
    // assigns chosen label to nextCell and returns true
    // if no unseen neighbors returns false
    bool searchNext(int currCell, int& nextCell);

    // depth-first search algorithm
    void generateMaze();

    // draws the maze
    void drawMaze();

    // finds what cell the player is currently in
    //NEEDS TO BE WRITTEN
    void setPlayerCell(Character thePlayer);
    
    //placeholder for now, needs to have access to player position, somehow
    //returns euclidean distance from desired point to the player position
    double calcHeuristic(Cell searchCell);
};