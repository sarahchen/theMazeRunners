#pragma once

#include <stdio.h>
#include <unordered_map>
#include <stack>
#include "Cell.h"

class Cell;
class ViewManager;

class Maze {
private:
    std::unordered_map<int, Cell> Grid;
    std::stack<int> VisitedCells;
    
public:
    
    int rowSize, colSize;
    
    Maze();

    //returns the unordered map of cells that compose the maze grid
    std::unordered_map<int, Cell> getGrid() { return Grid; }

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
    void drawMaze(ViewManager& theManager);
    
    //placeholder for now, needs to have access to player position, somehow
    //returns euclidean distance from desired point to the player position
    double calcHeuristic(double posX, double posY);
};