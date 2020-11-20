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
    int rowSize, colSize;

public:
    Maze();

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
};