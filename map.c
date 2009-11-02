#include "map.h"
#include "constants.h"

TileType* floorTile = NULL;
TileType* holeTile = NULL;
TileType* blankTile = NULL;

/* The following draws a square that is at y=0 in the x-z plane. */
void flatRect(float x1, float z1, float x2, float z2) {
	glVertex3f(x1, 0.0f, z1);
	glVertex3f(x2, 0.0f, z1);
	glVertex3f(x2, 0.0f, z2);
	glVertex3f(x1, 0.0f, z2);
}

/* This function makes a square centred around the current position, also at y=0 in x-z plane. */
void flatSquare(float length) {
	flatRect(-(length/2), -(length/2), length/2, length/2);
}

void drawFloorTile(Tile* tile) {
	glBegin(GL_QUADS);

	/* The Tile body */
	glColor3f(1.0f,1.0f,1.0f);
	flatSquare(tileSize);

	/* The Edges */
	glColor3f(0.0f, 0.0f, 0.0f);
	/* Back Edge */
	flatRect(-(tileSize/2), -(tileSize/2), ((tileSize/2) + edgeSize), -((tileSize/2) + edgeSize));
	/* Right Edge */
	flatRect(tileSize/2, -(tileSize/2), (tileSize/2) + edgeSize, (tileSize/2) + edgeSize);
	/* Front Edge */
	flatRect(tileSize/2, tileSize/2, -((tileSize/2) + edgeSize), (tileSize/2) + edgeSize);
	/* Left Edge */
	flatRect(-(tileSize/2), tileSize/2, -((tileSize/2) + edgeSize), -((tileSize/2) + edgeSize));
	glEnd();
}

void drawBlankTile(Tile* tile) {
	/* If the tile is NULL (an edge) or is of a type that starts with '.' then don't draw the wall */
	if (tile->Left != NULL || tile->Left->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(-((tileSize/2) + edgeSize), (tileSize/2) + edgeSize, 0);
		glRotatef(90, 0, 0, -1);
		drawFloorTile(tile);
		glPopMatrix();
	}

	if (tile->Right != NULL || tile->Right->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(((tileSize/2) + edgeSize), (tileSize/2) + edgeSize, 0);
		glRotatef(90, 0, 0, 1);
		drawFloorTile(tile);
		glPopMatrix();
	}

	if (tile->Up != NULL || tile->Up->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(0, (tileSize/2) + edgeSize, -((tileSize/2) + edgeSize));
		glRotatef(90, -1, 0, 0);
		drawFloorTile(tile);
		glPopMatrix();
	}

	if (tile->Down != NULL || tile->Down->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(0, (tileSize/2) + edgeSize, ((tileSize/2) + edgeSize));
		glRotatef(90, 1, 0, 0);
		drawFloorTile(tile);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.0f, tileSize + (2*edgeSize), 0.0f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 0.0f);
	flatSquare(tileSize + (2*edgeSize));
	glEnd();
	glPopMatrix();
}

void initMap() {
	floorTile = malloc(sizeof(TileType));
	if (floorTile == NULL) exit(1);
	holeTile = malloc(sizeof(TileType));
	if (holeTile == NULL) exit(1);
	blankTile = malloc(sizeof(TileType));
	if (blankTile == NULL) exit(1);

	floorTile->code[0] = '0';
	floorTile->code[1] = '1';

	holeTile->code[0] = '0';
	holeTile->code[1] = '1';

	blankTile->code[0] = '.';
	blankTile->code[1] = '.';

	floorTile->callList = glGenLists(1);
	glNewList(floorTile->callList, GL_COMPILE);
	floorTile->drawTile = &drawFloorTile;
	floorTile->drawTile(NULL);
	floorTile->useCallList = 1;
	glEndList();

	/* The holeTile is blank */
	holeTile->useCallList = 0;
	holeTile->drawTile = NULL;

	blankTile->useCallList = 0;
	blankTile->drawTile = &drawBlankTile;
}

void renderTile(Tile* tile) {
	/* Store the current state */
	glPushMatrix();

	/* Move into position for this tile */
	glTranslatef(squaresToUnits(tile->pos.x), 0, squaresToUnits(tile->pos.y));

	if (tile->def->def && tile->def->def->defAction) {
		if (tile->def->def->defAction(tile) == 0) {
			/* The function returned 0, it's assumed that any rendering has been done by that function */
			/* Restore the matrix from before */
			glPopMatrix();
			return;
		}
	}

	if (tile->def->type->useCallList) {
		glCallList(tile->def->type->callList);
	} else {
		/* If drawTile is NULL, then just don't draw */
		if (tile->def->type->drawTile) {
			tile->def->type->drawTile(tile);
		}
	}

	/* Restore the matrix from before */
	glPopMatrix();
}

void renderRoom(Room* room) {
	Tile* current = room->map->TopLeft;
	/* This marks if we're moving left or right */
	/* If left == 1 then we're moving left */
	int left = 0; /* Start off moving to the right */

	/* Store the current state */
	glPushMatrix();

	/* Position us at the top left of the room */
	glTranslatef(squaresToUnits(room->pos.x), feetToUnits(room->pos.elevation), squaresToUnits(room->pos.y));

	/* Go through all tiles */
	while (current != NULL) {
		renderTile(current);
		if (left) {
			if (current->Left == NULL) {
				current = current->Down;
				left = 0;
			} else {
				current = current->Left;
			}
		} else {
			if (current->Right == NULL) {
				current = current->Down;
				left = 1;
			} else {
				current = current->Right;
			}
		}
	}

	/* Restore the state we had when we started drawing the room */
	glPopMatrix();
}

void renderWorld(World* world) {
	RoomList* current = world->rooms;

	while (current != NULL) {
		renderRoom(current->value);
		current = current->next;
	}
}

World* buildTestWorld() {
	World* world;
	Tile* tileArray;
	int x,y;

	world = malloc(sizeof(World));
	world->rooms = malloc(sizeof(RoomList));
	world->rooms->next = NULL;

	world->rooms->value = malloc(sizeof(Room));
	world->rooms->value->pos.x = 0;
	world->rooms->value->pos.y = 0;
	world->rooms->value->pos.elevation = 0;
	world->rooms->value->metadata = NULL;

	world->rooms->value->map = malloc(sizeof(Map));

	tileArray = malloc(sizeof(Tile)*9);

	world->rooms->value->map->TopLeft=tileArray;

	for (x=0; x<3; x++) {
		for (y=0; y<3; y++) {
			(tileArray+x+(3*y))->pos.x = x;
			(tileArray+x+(3*y))->pos.y = y;

			(tileArray+x+(3*y))->def = malloc(sizeof(Def));
			(tileArray+x+(3*y))->def->def = NULL;
			if (x == 1 && y == 1) {
				/* We're in the middle tile */
				(tileArray+x+(3*y))->def->code[0] = '.';
				(tileArray+x+(3*y))->def->code[1] = '.';

				(tileArray+x+(3*y))->def->type = blankTile;

				/* The middle is the only one that has a tile on all sides */
				(tileArray+x+(3*y))->Left = tileArray+(x-1)+(3*y);
				(tileArray+x+(3*y))->Right = tileArray+(x+2)+(3*(y));
				(tileArray+x+(3*y))->Up = tileArray+(x)+(3*(y-1));
				(tileArray+x+(3*y))->Down = tileArray+(x)+(3*(y+2));
			} else {
				/* We're not the middle */
				(tileArray+x+(3*y))->def->code[0] = '0';
				(tileArray+x+(3*y))->def->code[1] = '1';

				(tileArray+x+(3*y))->def->type = floorTile;

				(tileArray+x+(3*y))->Up = y==0? NULL: tileArray+(x)+(3*(y-1));
				(tileArray+x+(3*y))->Down = y==2? NULL: tileArray+(x)+(3*(y+1));
				(tileArray+x+(3*y))->Left = x==0? NULL: tileArray+(x-1)+(3*(y));
				(tileArray+x+(3*y))->Right = x==2? NULL: tileArray+(x+1)+(3*(y));
			}
		}
	}

	return world;
}
