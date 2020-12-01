#include "Maze.h"
#include <time.h>

// initialize Grid for the maze
Maze::Maze() 
{
    // for now, 10x16 Grid of Hexagons
    numRows = 10;
    numCols = GRID_SIZE / numRows;
    
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

Cell Maze::getRandomCell()
{
    // Initialize random seed
    srand(time(0));

    //find random number in the range of generated cells
    int randIdx = rand() % (numRows * numCols) + 1;
    auto out = Grid.find(randIdx);

    return out->second;

}

Cell Maze::getCell(int label)
{
    auto out = Grid.find(label);
    return out->second;
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

void Maze::drawMaze()
{
    // setMazeColor
    glColor3ub(255, 0, 0);
    int side = 1;
    int cellIdx = 0;
    for (int i = 1; i < GRID_SIZE+1; i++) {
        cellIdx = i * side;
        Grid[cellIdx].drawCell();
    }

    // Highlight player cell
    glColor3ub(0, 0, 255);
    playerCell.drawCell();
}

//finds the cell that the player is currently in and sets that variable in Maze
void Maze::setPlayerCell(Character thePlayer)
{
    double playerX = thePlayer.getX();
    double playerY = thePlayer.getY();

    //start by checking the cell that the player was previously in
    double cellY = playerCell.getCenterY();

    double cellX = playerCell.getCenterX();
    double dX = playerX - cellX;
    double dY = playerY - cellY;

    double dist = sqrt(dX * dX + dY * dY);
    //cout << "short search, player cell = " << playerCell.getLabel() << '\n';
    //only check all other cells if player is outside the current cell
    if (dist > playerCell.getRadius()) {
        for (auto check : Grid) {

            Cell checkCell = check.second;

            cellX = checkCell.getCenterX();
            cellY = checkCell.getCenterY();

            dX = playerX - cellX;
            dY = playerY - cellY;
            dist = sqrt(dX * dX + dY * dY);

            if (dist <= checkCell.getRadius()) {
                playerCell = checkCell;
                //cout << "long search, player cell = " << playerCell.getLabel() << '\n';
                break;
            }
        }
    }
}

//returns euclidean distance from desired point to the player position
double Maze::calcHeuristic(Cell searchCell) {
    
    double goalX = playerCell.getCenterX();
    double goalY = playerCell.getCenterY();

    double startX = searchCell.getCenterX();
    double startY = searchCell.getCenterY();
    
    double dX = startX - goalX;
    double dY = startY - goalY;

    double dist = sqrt(dX * dX + dY * dY)/ (2*searchCell.getRadius());
    
    return dist;
}

// Checks if the line between cellNode1 and cellNode2 intersects the line from p1 to p2
// uses this line intersection method:
// https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
// returns true if in contact
bool Maze::checkWallContact(node cellNode1, node cellNode2, node p1, node p2)
{
    if ((checkOrientation(cellNode1, cellNode2, p1) != checkOrientation(cellNode1, cellNode2, p2))
        && (checkOrientation(p1, p2, cellNode1) != checkOrientation(p1, p2, cellNode2))) {
        return true;
    }

    return false;
}

// Checks orientation of three points
// returns 0 if colinear, 1 cw, 2 ccw
int Maze::checkOrientation(node one, node two, node three)
{
    int val = (two.y - one.y) * (three.x - two.x) -
        (two.x - one.x) * (three.y - two.y);

    if (val == 0) {
        return 0;
    }
    else if (val > 0) {
        return 1;
    }
    else {
        return 2;
    }

}

// Check if front or back of player intersects with cell walls
// uses playerCell
// returns true if character does NOT contact
bool Maze::validCharMove(Character thePlayer)
{
    // sets cell that the player is in
    setPlayerCell(thePlayer);

    // gets 4 corners of car and check if each side is moveable
    node a = { thePlayer.getAx(), thePlayer.getAy() };
    node b = { thePlayer.getBx(), thePlayer.getBy() };
    node c = { thePlayer.getCx(), thePlayer.getCy() };
    node d = { thePlayer.getDx(), thePlayer.getDy() };

    // iterate through every wall and check
    for (int i = 0; i < 6; i++) {

        // if wall exists, check if it intersects with player
        if (playerCell.walls[i]) {
            // if the wall intersects with top or bottom of player, return false
            if (checkWallContact(playerCell.getNode(i), playerCell.getNode((i+1)%6), a, d) ||
                checkWallContact(playerCell.getNode(i), playerCell.getNode((i+1)%6), b, c)) {
                std::cout << "In Cell: " << playerCell.getLabel() << " Failed at Wall: " << i << std::endl;
                return false;
            }
        }
    }

    return true;
}