#pragma once
#include <vector>
#include "Character.h"
#include "Maze.h"


class Enemy :
    public Character
{
protected:
    
    int damage;                 //how much damage does the enemy do to the player
    Cell currentLocation;       //current location of the enemy
    list<Cell> movementPlan;    //stores the plan for subsequent moves
    int size = 37;              //pixels - size of the enemy character drawing

    //timestamp of the last time the enemy moved
    chrono::system_clock::time_point prevMoveTime = chrono::system_clock::now(); 

    double moveTimeThresh = 1000;   //how much time (in ms) the enemy will wait until next move

public:
    Enemy() {};

    Cell getLocation() { return currentLocation; }

    //sets the amount of time (in ms) that the enemy waits before making next move
    void setMoveTimeThresh(double inTime) { moveTimeThresh = inTime; }

    void spawn(Cell spawnCell, GLuint Id);
    void draw();
    void move();
    void findBestPath(Maze theMaze);
};

