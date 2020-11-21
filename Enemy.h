#pragma once
#include <vector>
#include "Character.h"
#include "Maze.h"

class Enemy :
    public Character
{
protected:
    vector<pair<int, int>> movementPlan; //stores the movement plan for the enemy
    int damage; //how much damage does the enemy do to the player

public:
    Enemy() {};
    void spawn(int locX, int locY);
    void draw();
    void move();
    void findBestPath(Maze theMaze);
};

