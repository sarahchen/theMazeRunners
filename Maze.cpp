#include "Maze.h"
#include <time.h>

// initialize Grid for the maze
Maze::Maze() 
{
    // for now, 10x16 Grid of Hexagons
    numRows = 10;
    numCols = 16;
    
    // rowSize: size of each row
    // colSize: size of each column
    int rowSize = numCols;
    int colSize = numRows;
    
    int gridX, gridY;
    double centerX, centerY;

    for(int i = 1; i < GRID_SIZE + 1; i++) {

        // Determine centerX and centerY for current Cell
        // Using Convention stated in Cell.cpp

        gridX = i % numCols;
        gridY = (i / numCols) + 1;
        if (gridX == 0) { 
            gridX = numCols;
            gridY = gridY - 1;
        }
        
        centerX = 150. + CELL_RADIUS + (CELL_RADIUS + CELL_RADIUS*sin(PI / 6))*(gridX-1.);
        centerY = 125. + ((CELL_RADIUS/2)*sqrt(3)) + ((CELL_RADIUS / 2) * sqrt(3)) * (gridX % 2)
            + (CELL_RADIUS*sqrt(3))*(gridY-1.);

        Cell tmp(i, rowSize, colSize, centerX, centerY);
        Grid[i] = tmp;
    }
}

bool Maze::searchNext(int currCell, int& nextCell)
{
    Cell* current = &Grid[currCell];

    std::vector<int> unseenNeighbors;
    std::vector<int> allNeighbors;
    allNeighbors = current->getAllNeighbors();

    getUnseenNeighbors(currCell, unseenNeighbors);

    if (unseenNeighbors.size() != 0)
    {
        // Randomly pick an available neighbor
        nextCell = unseenNeighbors[rand() % unseenNeighbors.size()];
        Cell* next = &Grid[nextCell];

        // Set Wall in that direction to false
        auto it = std::find(allNeighbors.begin(), allNeighbors.end(), nextCell);
        if (it != allNeighbors.end()) {
            int index = std::distance(allNeighbors.begin(), it);
            current->setWall(index, false);
        }

        // Set Wall in of nextCell to false
        std::vector<int> nextNeighbors;
        nextNeighbors = next->getAllNeighbors();
        it = std::find(nextNeighbors.begin(), nextNeighbors.end(), currCell);
        if (it != nextNeighbors.end()) {
            int index = std::distance(nextNeighbors.begin(), it);
            next->setWall(index, false);
        }

        return true;
    }

    return false;
}

void Maze::getUnseenNeighbors(int currCell, std::vector<int>& unseenList)
{
    Cell* current = &Grid[currCell];

    /*--- Create list of available neighbors ---*/
    // Get all neighbors of the cell
    std::vector<int> allNeighbors;
    allNeighbors = current->getAllNeighbors();

    // iterate through them and check if seen
    // if not seen, add to unseenList
    int label;
    for (int i = 0; i < 6; i++)
    {
        label = allNeighbors[i];
        if (label != 0 && Grid[label].isSeen() == false) {
            unseenList.push_back(label);
        }
    }
}

void Maze::generateMaze()
{
    // Initialize random seed
    srand(time(0));

    // Initialize the Starting Cell
    Cell* startCell = &Grid[1];
    // Mark starting cell as visited and push it to the stack
    startCell->setSeen();
    VisitedCells.push(1);

    Cell* currentCell;
    int lbl, nextlbl;

    // while the stack is not empty...
    while(!VisitedCells.empty()) {
        // get the current cell from the stack 
        lbl = VisitedCells.top();
        currentCell = &Grid[lbl];

        // if the current cell has any unseen neighbors
        // choose one as the next cell, mark it seen, and push to stack
        std::vector<int> unseenNeighbors;
        if(searchNext(lbl, nextlbl)) {
            std::cout << lbl << std::endl;
            Grid[nextlbl].setSeen();
            VisitedCells.push(nextlbl);
        }
        // if the current cell has no unseen neighbors, pop visited cells
        // until we find a cell with unseen neighbors
        else {
            VisitedCells.pop();
        }
    }
}

void Maze::drawMaze(ViewManager& theManager)
{
    // draw maze
    // for each side of the cube, just draw that side?
    // side goes from 1-6
    
    // for (int side = 1; side < 7; side++) {
        int side = 1;
        int cellIdx = 0;
        for (int i = 1; i < GRID_SIZE+1; i++) {
            cellIdx = i * side;
            Grid[cellIdx].drawCell();
        }
    // }
}

//placeholder for now, needs to have access to player position, somehow
//returns euclidean distance from desired point to the player position
double Maze::calcHeuristic(double posX, double posY) {
    
    double playerPosX = 10; //obviously needs to be changed
    double playerPosY = 10; //obviously needs to be changed
    
    double dX = posX - playerPosX; //sign doesn't matter because it will be squared later
    double dY = posY - playerPosY; //sign doesn't matter because it will be squared later

    double dist = sqrt(dX * dX + dY * dY);
    
    return dist;
}