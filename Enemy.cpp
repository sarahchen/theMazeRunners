#include <queue>
#include <unordered_set>
#include "Enemy.h"

#define LINEAR_INDEX(X, Y, XSIZE, YSIZE) ((Y-1)*XSIZE + (X-1))

using namespace std;

void Enemy::spawn(Cell spawnCell, GLuint Id)
{
	textID = Id;
	currentLocation = spawnCell;
}

//draw enemy about center
void Enemy::draw()
{
	double centerX = currentLocation.getCenterX();
	double centerY = currentLocation.getCenterY();
	
	//glColor3ub(200, 0, 0);
	//glBegin(GL_QUADS);
	//glVertex2d(centerX - size / 2, centerY + size / 2);
	//glVertex2d(centerX + size / 2, centerY + size / 2);
	//glVertex2d(centerX + size / 2, centerY - size / 2);
	//glVertex2d(centerX - size / 2, centerY - size / 2);
	//glEnd();
	//glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textID);

	glBegin(GL_QUADS);

	glTexCoord2d(0.0, 0.0);
	glVertex2d(centerX - size / 2, centerY + size / 2);
	glTexCoord2d(0.0, 1.0);
	glVertex2d(centerX + size / 2, centerY + size / 2);
	glTexCoord2d(1.0, 1.0);
	glVertex2d(centerX + size / 2, centerY - size / 2);
	glTexCoord2d(1.0, 0.0);
	glVertex2d(centerX - size / 2, centerY - size / 2);
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

}

//moves enemy to the next position from the movement plan
void Enemy::move()
{
	if (!movementPlan.empty()) {
		
		currentLocation = movementPlan.back();
		movementPlan.pop_back();
	}
}

//finds shortest path to the player through the maze, using A*
//mostly borrowed from PS09
void Enemy::findBestPath(Maze theMaze)
{
	int numRows = theMaze.getNumRows();
	int numCols = theMaze.getNumCols();
	
	//make my own struct to hold coordinate info
	struct Node {
		int cell_label;
		double g, h;
	};
	struct NodeCompareF {
		bool operator()(Node const& n1, Node const& n2) {
			// return "true" if "n1" is ordered after "n2”:
			return (n1.g + n1.h) > (n2.g + n2.h);
		}
	};

	priority_queue<Node, vector<Node>, NodeCompareF> open_list_q;
	unordered_set<int> open_list_m;
	unordered_set<int> closed_list;
	unordered_map<int, int> parent_list; //first int is child, second int is parent

	//set start location
	Node startNode = { currentLocation.getLabel(), 0, theMaze.calcHeuristic(currentLocation)};
	parent_list[currentLocation.getLabel()] = -1;

	//run the A* search while open list is empty
	Node currNode = startNode;
	Cell currCell = theMaze.getCell(currNode.cell_label);
	open_list_q.push(currNode);
	open_list_m.insert(currNode.cell_label);

	bool foundGoal = false;
	while (!foundGoal && !open_list_q.empty()) {

		//grab node with smallest f value
		currNode = open_list_q.top();
		currCell = theMaze.getCell(currNode.cell_label);
		open_list_q.pop();
		open_list_m.erase(currNode.cell_label);

		//add node to closed list
		closed_list.insert(currNode.cell_label);

		//if current node is the goal, stop the search
		//cout << "h val = " << theMaze.calcHeuristic(currCell) << '\n';
		if (theMaze.calcHeuristic(currCell) == 0) {
			foundGoal = true;
			//cout << "found goal\n";
			break;
		}

		//evaluate successors of current node
		vector<int> neighborLabels = currCell.getAvailableNeighbors();

		for (int idx = 0; idx < neighborLabels.size(); idx++) {

			int neighborLabel = neighborLabels[idx];

			//check if node is not already on the closed list
			if (closed_list.find(neighborLabel) == closed_list.end()) {

				Cell succCell = theMaze.getCell(neighborLabel);
				Node succNode = { neighborLabel, currNode.g + 1, theMaze.calcHeuristic(succCell)};
				parent_list[succNode.cell_label] = currNode.cell_label;

				//if not already on the open list, put it on there
				if (open_list_m.find(neighborLabel) == open_list_m.end()) {
					open_list_q.push(succNode);
					open_list_m.insert(succNode.cell_label);
				}
			}
		}
	}
	//cout << "ended search " << closed_list.size() << '\n';

	////if player was found, populate the movement plan
	if (foundGoal) {
		
		movementPlan.clear();

		movementPlan.push_back(currCell);
		auto out = parent_list.find(currCell.getLabel());
		int nextParentLabel = out->second;

		while (nextParentLabel > 0) {
			
			currCell = theMaze.getCell(nextParentLabel);

			movementPlan.push_back(currCell);
			auto out = parent_list.find(currCell.getLabel());
			nextParentLabel = out->second;
			//cout << "parent label = " << nextParentLabel << '\n';
		}
		movementPlan.pop_back();
		//cout << "movement plan completed\n";
	}
}
