#include "fssimplewindow.h"
#include "ysglfontdata.h"
//#include "DrawingUtilNG.h"
#include "yspng.h"
#include "Item.h"
#include "Cell.h"	//for cell radius
#include "Maze.h"

//class constructor where we set the parameters such as textures, size, and location
Item::Item(Cell inputCell, itemType item, GLuint Id, Maze theMaze)
{
	thisItem = item;
	textID = Id;
	itemCell = inputCell;
	pos.x = itemCell.getCenterX();
	pos.y = itemCell.getCenterY();
	cell = inputCell.getLabel();
	collected = false;
	wid = CELL_RADIUS;
	hei = CELL_RADIUS;
	assignPointVal();
}

void Item::assignPointVal()
{
	//only assign point value bonus for getting the money bag
	//however, can easily change that with this switch case in the future
	switch (thisItem) {
	case coffee:
		pointVal = 0;
		break;
	case star:
		pointVal = 0;
		break;
	case moneyBag:
		pointVal = 200;
		break;
	}
}

void Item::draw()
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(1.0, 1.0, 1.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, textID);

	glBegin(GL_QUADS);

	//draw the item based on which item is selected (to fit geometry of the item best)
	if (thisItem == star) {
		glTexCoord2d(0.0, 0.0);
		glVertex2i(pos.x - wid / 2, pos.y - hei / 2);
		glTexCoord2d(0.0, 1.0);
		glVertex2i(pos.x + wid / 2, pos.y - hei / 2);
		glTexCoord2d(1.0, 1.0);
		glVertex2i(pos.x + wid / 2, pos.y + hei / 2);
		glTexCoord2d(1.0, 0.0);
		glVertex2i(pos.x - wid / 2, pos.y + hei / 2);
	}
	else {
		glTexCoord2d(0.0, 0.0);
		glVertex2i(pos.x - wid / 2, pos.y - hei / 2);
		glTexCoord2d(1.0, 0.0);
		glVertex2i(pos.x + wid / 2, pos.y - hei / 2);
		glTexCoord2d(1.0, 1.0);
		glVertex2i(pos.x + wid / 2, pos.y + hei / 2);
		glTexCoord2d(0.0, 1.0);
		glVertex2i(pos.x - wid / 2, pos.y + hei / 2);
	}
	glEnd();
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}

