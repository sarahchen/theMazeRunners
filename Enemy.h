#pragma once
#include <vector>
#include "Character.h"
#include "Maze.h"


class Enemy :
    public Character
{
protected:
    
    int damage;                 //how much damage does the enemy do to the player
    Cell currentLocation;           //current location of the enemy
    list<Cell> movementPlan;    //stores the plan for subsequent moves
    int size = 15;              //pixels - size of the enemy character drawing

public:
    Enemy() {};

    Cell getLocation() { return currentLocation; }

    void spawn(Cell spawnCell);
    void draw();
    void move();
    void findBestPath(Maze theMaze);
};

