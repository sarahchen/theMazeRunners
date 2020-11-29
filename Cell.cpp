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
	for (int i = 0; i < 6; i++) // init all walls as true
		walls[i] = true;
	for (int i = 0; i < 6; i++)
		neighbors[i] = getNeighbor(i); // finds each neighbor
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

int Cell::getNeighbor(int label)
{
	/*
	* this is what I'm basing neighbor calculations on where this is a 10 row case
	* #'s in the boxes are cell labels?
col 1
 | col 2
 |   | col 3
 |   |  |
 __  | __    __    __    __
/01\__/03\__/05\__/07\__/09\__
\__/02\__/04\__/06\__/08\__/10\--- row 1
/11\__/13\__/15\__/17\__/19\__/
\__/12\__/16\__/16\__/18\__/20\--- row 2
/21\__/23\__/25\__/27\__/29\__/				<---- This is the model the current getNeighbor is based on
\__/22\__/24\__/26\__/28\__/30\--- row 3
/31\__/33\__/35\__/37\__/39\__/
\__/32\__/34\__/36\__/38\__/40\--- row 4
/  \__/  \__/  \__/  \__/  \__/
alternative row and col convention --> doesnt effect drawing but will effect determining neighbors
col 1
 | col 2
 |   | col 3
 |   |  |
 __  | __    __    __    __
/01\__/02\__/03\__/04\__/05\__	--- row 1
\__/06\__/07\__/08\__/09\__/10\ --- row 2
/11\__/12\__/13\__/14\__/15\__/ --- row 3
\__/16\__/17\__/18\__/19\__/20\ --- row 4
/  \__/  \__/  \__/  \__/  \__/
// neighbor index cases --> same convention for walls
 __/00\__
/05\__/01\
\__/cu\__/
/04\rr/02\
\__/03\__/
   \__/
	*/
	// math to figure out neighbors
	switch (label) {
	case 0:
		if (label - rowSize <= 0)
			return 0; // return 0 for no neighbor
		else
			return label - rowSize;
	case 1:
		if (label - rowSize <= 0 && label % 2 != 0)
			return 0;
		else if (label % rowSize == 0)
			return 0;
		else if (label % 2 == 0)
			return label + 1;
		else
			return label - rowSize - 1;
	case 2:
		if (label + rowSize > rowSize * colSize && label % 2 == 0)
			return 0;
		else if (label % rowSize == 0)
			return 0;
		else if (label % 2 != 0)
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
		else if (label + rowSize > rowSize * colSize && label % 2 == 0)
			return 0;
		else if (label % 2 != 0)
			return label - 1;
		else
			return label + rowSize - 1;
	case 5:
		if ((label - 1) % rowSize == 0)
			return 0;
		else if (label - rowSize <= 0 && label % 2 != 0)
			return 0;
		else if (label % 2 == 0)
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

void Cell::drawCell()
{
	// set up local coordinate system in the Maze class when drawing each node
	// set wall color
	glColor3ub(255,0,0);
	// draw the walls in 
	//glLoadIdentity();
	//glTranslatef(centerX, centerY, 0);

	glBegin(GL_LINE_STRIP);
	if (walls[0]) {
		glVertex2d(cellNodes[0].x, cellNodes[0].y);
		glVertex2d(cellNodes[1].x, cellNodes[1].y);
	}
	if (walls[1]) {
		glVertex2d(cellNodes[1].x, cellNodes[1].y);
		glVertex2d(cellNodes[2].x, cellNodes[2].y);
	}
	if (walls[2]) {
		glVertex2d(cellNodes[2].x, cellNodes[2].y);
		glVertex2d(cellNodes[3].x, cellNodes[3].y);
	}
	if (walls[3]) {
		glVertex2d(cellNodes[3].x, cellNodes[3].y);
		glVertex2d(cellNodes[4].x, cellNodes[4].y);
	}
	if (walls[4]) {
		glVertex2d(cellNodes[4].x, cellNodes[4].y);
		glVertex2d(cellNodes[5].x, cellNodes[5].y);
	}
	if (walls[5]) {
		glVertex2d(cellNodes[5].x, cellNodes[5].y);
		glVertex2d(cellNodes[0].x, cellNodes[0].y);
	}
	glEnd();
	glLoadIdentity();
}
