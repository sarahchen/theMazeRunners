/*
Nate Winneg -- Project Component Code

This class is resposible for creating the
cell object and the assiciated variables
and functionalities. It isn't fully fleshed out
yet and the row column scheme isn't fully
finalized so there are some sketches
commented in to show what I was going for.
*/

#include "Cell.h"

using namespace std;

Cell::Cell(int alabel, int mazeRowSize, int mazeColSize, double cx, double cy)
{
	radius = CELL_RADIUS; // distance from center to a corner node -- this can be changed to whatever is appropriate
	rowSize = mazeRowSize; colSize = mazeColSize;
	centerX = cx; centerY = cy;
	seen = false;
	label = alabel;
	for (int i = 0; i < 6; i++) {
		walls[i] = true; // init all walls as true
		neighbors[i] = getNeighbor(i); // finds each neighbor
	}
	setNodes(); // init all nodes
}

void Cell::setNodes()
{
	cellNodes.push_back({ centerX - radius * sin(PI / 6), centerY - radius * cos(PI / 6) });
	cellNodes.push_back({ centerX + radius * sin(PI / 6), centerY - radius * cos(PI / 6) });
	cellNodes.push_back({ centerX + radius, centerY });
	cellNodes.push_back({ centerX + radius * sin(PI / 6), centerY + radius * cos(PI / 6) });
	cellNodes.push_back({ centerX - radius * sin(PI / 6), centerY + radius * cos(PI / 6) });
	cellNodes.push_back({ centerX - radius, centerY });
}

int Cell::getNeighbor(int idx)
{
	/*
	* this is what I'm basing neighbor calculations on where this is a 10 row case
	* #'s in the boxes are cell labels?

		__    __    __    __	__
	 __/02\__/04\__/06\__/08\__/10\
	/01\__/03\__/05\__/07\__/09\__/--- row 1
	\__/12\__/14\__/16\__/18\__/20\
	/11\__/13\__/15\__/17\__/19\__/--- row 2
	\__/22\__/24\__/26\__/28\__/30\				<---- This is the model the current getNeighbor is based on
	/21\__/23\__/25\__/27\__/29\__/--- row 3
	\__/32\__/34\__/36\__/38\__/40\
	/31\__/33\__/35\__/37\__/39\__/--- row 4
	\__/  \__/  \__/  \__/  \__/

	// neighbor index cases --> same convention for walls
	 __/00\__
	/05\__/01\
	\__/cu\__/
	/04\rr/02\
	\__/03\__/
	   \__/
	*/
	// math to figure out neighbors

	switch (idx) {
	case 0:
		if (label - rowSize <= 0)
			return 0; // return 0 for no neighbor
		else
			return label - rowSize;
	case 1:
		if (label - rowSize <= 0 && label % 2 == 0)
			return 0;
		else if (label % rowSize == 0)
			return 0;
		else if (label % 2 != 0)
			return label + 1;
		else
			return label - rowSize + 1;
	case 2:
		if (label + rowSize > rowSize * colSize && label % 2 != 0)
			return 0;
		else if (label % rowSize == 0)
			return 0;
		else if (label % 2 == 0)
			return label + 1;
		else
			return label + rowSize + 1;
	case 3:
		if (label + rowSize > rowSize * colSize)
			return 0;
		else
			return label + rowSize;
	case 4:
		if ((label - 1) % rowSize == 0)
			return 0;
		else if (label + rowSize > rowSize * colSize && label % 2 != 0)
			return 0;
		else if (label % 2 == 0)
			return label - 1;
		else
			return label + rowSize - 1;
	case 5:
		if ((label - 1) % rowSize == 0)
			return 0;
		else if (label - rowSize <= 0 && label % 2 == 0)
			return 0;
		else if (label % 2 != 0)
			return label - 1;
		else
			return label - rowSize - 1;
	}
	return 0;
}

// This could be simpler and not reset everytime probably
std::vector<int> Cell::getAvailableNeighbors()
{
	availableNeighbors.clear();
	for (int i = 0; i < 6; i++) {
		if (walls[i] == false)
			availableNeighbors.push_back(neighbors[i]);
	}
	return availableNeighbors;
}

std::vector<int> Cell::getAllNeighbors()
{
	std::vector<int> list;
	for (int i = 0; i < 6; i++) {
		list.push_back(neighbors[i]);
	}
	return list;
}

void Cell::drawCell()
{
	// set up local coordinate system in the Maze class when drawing each node

	if (walls[0]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[0].x, cellNodes[0].y);
		glVertex2d(cellNodes[1].x, cellNodes[1].y);
		glEnd();
	}
	if (walls[1]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[1].x, cellNodes[1].y);
		glVertex2d(cellNodes[2].x, cellNodes[2].y);
		glEnd();
	}
	if (walls[2]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[2].x, cellNodes[2].y);
		glVertex2d(cellNodes[3].x, cellNodes[3].y);
		glEnd();
	}
	if (walls[3]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[3].x, cellNodes[3].y);
		glVertex2d(cellNodes[4].x, cellNodes[4].y);
		glEnd();
	}
	if (walls[4]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[4].x, cellNodes[4].y);
		glVertex2d(cellNodes[5].x, cellNodes[5].y);
		glEnd();
	}
	if (walls[5]) {
		glBegin(GL_LINE_STRIP);
		glVertex2d(cellNodes[5].x, cellNodes[5].y);
		glVertex2d(cellNodes[0].x, cellNodes[0].y);
		glEnd();
	}

	glLoadIdentity();
}

void Cell::highlightCell()
{
	// set up local coordinate system in the Maze class when drawing each node

	glBegin(GL_POLYGON);
	for (int i = 5; i >= 0; i--) {
		glVertex2d(cellNodes[(i + 1) % 6].x, cellNodes[(i + 1) % 6].y);
		glVertex2d(cellNodes[i].x, cellNodes[i].y);
	}
	glEnd();
	glLoadIdentity();
}
