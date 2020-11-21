#include "Maze.h"

// initialize Grid for the maze
Maze::Maze() 
{
    // For now 10x10 hexagonal Grid
    // 6 10x10 Grids make up 1 cube
    // Every 100 Cells make up 1 Grid
    // 600 Cells total
    
    rowSize = 10;
    colSize = 10;

    for(int i = 1; i < 600; i++) {
        Cell tmp(i, rowSize, colSize, i*rowSize+10, i*colSize+10);
        Grid[i] = tmp;
    }
}
bool Maze::searchNext(int currCell, int& nextCell)
{
    Cell* current = &Grid[currCell];

    std::vector<int> unseenNeighbors;
    int allNeighbors[6];
    current->getAllNeighbors(allNeighbors);
    getUnseenNeighbors(currCell, unseenNeighbors);

    if (unseenNeighbors.size() != 0)
    {
        // Randomly pick an available neighbor
        nextCell = unseenNeighbors[rand() % unseenNeighbors.size()];

        // Set Wall in that direction to false
        int* idx = std::find(allNeighbors, allNeighbors+sizeof(allNeighbors), nextCell);
        if (idx != allNeighbors + sizeof(allNeighbors))
        {
            current->setWall(*idx, false);
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
    int allNeighbors[6];
    current->getAllNeighbors(allNeighbors);

    // iterate through them and check if seen
    // if not seen, add to unseenList
    for (int i = 0; i < 6; i++)
    {
        int label = allNeighbors[i];
        if (Grid[label].isSeen() == false) {
            unseenList.push_back(label);
        }
    }
}

void Maze::generateMaze()
{
    // Initialize the Starting Cell
    Cell* startCell = &Grid[1];
    // Mark starting cell as visited and push it to the stack
    startCell->setSeen();
    VisitedCells.push(1);

    Cell* currentCell;
    int lbl, nextlbl;

    // while the stack is not empty...
    while(!VisitedCells.empty()) {

        // pop the current cell from the stack 
        lbl = VisitedCells.top();
        VisitedCells.pop();

        currentCell = &Grid[lbl];

        // if the current cell has any unseen neighbors
        // choose one as the next cell, mark it seen, and push to stack
        std::vector<int> unseenNeighbors;
        if(searchNext(lbl, nextlbl)) {
            Grid[nextlbl].setSeen();
            VisitedCells.push(nextlbl);
        }

        // if the current cell has no unseen neighbors, pop visited cells
        // until we find a cell with unseen neighbors
    }
}

void Maze::drawMaze(ViewManager& theManager)
{
    // draw maze
    // for each side of the cube, just draw that side?
    // side goes from 1-6
    
    for (int side = 1; side < 7; side++) {
        int cellIdx = 0;
        for (int i = 1; i < 100; i++) {
            cellIdx = i * side;
            Grid[cellIdx].drawCell(Grid[cellIdx].getCenterX(),
                Grid[cellIdx].getCenterY());
        }
    }
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