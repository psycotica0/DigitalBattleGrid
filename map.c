#include <string.h>
#include "map.h"
#include "constants.h"

TileType* tileTypes;

#define NumTileTypes 3
TileType* floorTile = NULL;
TileType* holeTile = NULL;
TileType* blankTile = NULL;

#define FrontTileAlpha 1.0f
#define BackTileAlpha 0.6f

DefList* globalTileDefs;

/* The following draws a square that is at y=0 in the x-z plane with the normal in the direction of the y axis. */
void flatRect(float x1, float z1, float x2, float z2) {
	/* First I swap x's and z's so x1 and z1 are the lowest, and x2 and z2 are the higher */
	/* This allows me to guarantee which order the verticies go in, so I can guarantee which face is up */
	if (x1 > x2) {
		/* Swapy Trick */
		x1 = x1 + x2;
		x2 = x1 - x2;
		x1 = x1 - x2;
	}
	if (z1 > z2) {
		/* Swapy Trick */
		z1 = z1 + z2;
		z2 = z1 - z2;
		z1 = z1 - z2;
	}

	/* Go in counter-clockwise order. */
	glVertex3f(x1, 0.0f, z1);
	glVertex3f(x1, 0.0f, z2);
	glVertex3f(x2, 0.0f, z2);
	glVertex3f(x2, 0.0f, z1);
}

/* This function makes a square centred around the current position, also at y=0 in x-z plane. */
void flatSquare(float length) {
	flatRect(-(length/2), -(length/2), length/2, length/2);
}

void drawFloorTile(Room* room, Tile* tile, float alpha) {
	glBegin(GL_QUADS);

	/* The Tile body */
	glColor4f(1.0f,1.0f,1.0f,alpha);
	flatSquare(tileSize);

	/* The Edges */
	glColor4f(0.0f, 0.0f, 0.0f, alpha);
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

void drawFloorTileFront(Room* room, Tile* tile) {
	/* Only draw the front */
	glPolygonMode(GL_BACK, GL_POINT);
	glPolygonMode(GL_FRONT, GL_FILL);
	drawFloorTile(room, tile, FrontTileAlpha);
}

void drawFloorTileRear(Room* room, Tile* tile) {
	/* Only draw the back */
	glPolygonMode(GL_FRONT, GL_POINT);
	glPolygonMode(GL_BACK, GL_FILL);
	drawFloorTile(room, tile, BackTileAlpha);
}

void drawBlankTile(Room* room, Tile* tile, float alpha) {
	/* If the tile is NULL (an edge) or is of a type that starts with '.' then don't draw the wall */
	if (tile->Left != NULL && tile->Left->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(-((tileSize/2) + edgeSize), (tileSize/2) + edgeSize, 0);
		glRotatef(90, 0, 0, 1);
		drawFloorTile(room, tile, alpha);
		glPopMatrix();
	}

	if (tile->Right != NULL && tile->Right->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(((tileSize/2) + edgeSize), (tileSize/2) + edgeSize, 0);
		glRotatef(90, 0, 0, -1);
		drawFloorTile(room, tile, alpha);
		glPopMatrix();
	}

	if (tile->Up != NULL && tile->Up->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(0, (tileSize/2) + edgeSize, -((tileSize/2) + edgeSize));
		glRotatef(90, -1, 0, 0);
		drawFloorTile(room, tile, alpha);
		glPopMatrix();
	}

	if (tile->Down != NULL && tile->Down->def->type->code[0] != '.') {
		glPushMatrix();
		glTranslatef(0, (tileSize/2) + edgeSize, ((tileSize/2) + edgeSize));
		glRotatef(90, 1, 0, 0);
		drawFloorTile(room, tile, alpha);
		glPopMatrix();
	}

	glPushMatrix();
	glTranslatef(0.0f, tileSize + (2*edgeSize), 0.0f);
	glBegin(GL_QUADS);
	glColor4f(0.0f, 0.0f, 0.0f, alpha);
	flatSquare(tileSize + (2*edgeSize));
	glEnd();
	glPopMatrix();
}

void drawBlankTileFront(Room* room, Tile* tile) {
	/* Only draw the front */
	glPolygonMode(GL_BACK, GL_POINT);
	glPolygonMode(GL_FRONT, GL_FILL);
	drawBlankTile(room, tile, FrontTileAlpha);
}

void drawBlankTileRear(Room* room, Tile* tile) {
	/* Only draw the back */
	glPolygonMode(GL_FRONT, GL_POINT);
	glPolygonMode(GL_BACK, GL_FILL);
	drawBlankTile(room, tile, BackTileAlpha);
}

void initMap() {
	tileTypes = malloc(sizeof(TileType) * NumTileTypes);
	if (tileTypes == NULL) exit(1);

	floorTile = tileTypes;
	holeTile = tileTypes + 1;
	blankTile = tileTypes + 2;

	memcpy(floorTile->code, "01", 2);
	memcpy(holeTile->code, "00", 2);
	memcpy(blankTile->code, "..", 2);

	floorTile->drawTileFront = &drawFloorTileFront;
	floorTile->drawTileRear = &drawFloorTileRear;
	floorTile->useCallList = 0;

	/* The holeTile is blank */
	holeTile->useCallList = 0;
	holeTile->drawTileFront = NULL;
	holeTile->drawTileRear = NULL;

	blankTile->useCallList = 0;
	blankTile->drawTileFront = &drawBlankTileFront;
	blankTile->drawTileRear = &drawBlankTileRear;

	/* Initialize Global Tile Definitions */
	{
		TileType* curType;
		DefList* curDef;
		int i;

		globalTileDefs = malloc(sizeof(DefList) * NumTileTypes);
		for (i=0; i < NumTileTypes; i++) {
			curType = tileTypes + i;
			curDef = globalTileDefs + i;

			curDef->def = malloc(sizeof(Def));
			memcpy(curDef->def->code ,curType->code, 2);
			curDef->def->type = curType;
			curDef->def->def = NULL;

			if (i+1 < NumTileTypes) {
				curDef->next = curDef+1;
			}
		}
	}
}

void renderTile(Room* room, Tile* tile, int direction) {
	/* Store the current state */
	glPushMatrix();

	/* Move into position for this tile */
	glTranslatef(squaresToUnits(tile->pos.x), 0, squaresToUnits(tile->pos.y));

	if (tile->def->def && tile->def->def->defAction) {
		if (tile->def->def->defAction(room, tile) == 0) {
			/* The function returned 0, it's assumed that any rendering has been done by that function */
			/* Restore the matrix from before */
			glPopMatrix();
			return;
		}
	}

	if (tile->def->type->useCallList) {
		glCallList(tile->def->type->callList);
	} else {
		/* If drawTiles are NULL, then just don't draw */
		if (direction == Render_Front) {
			if (tile->def->type->drawTileFront) {
				tile->def->type->drawTileFront(room, tile);
			}
		} else {
			if (tile->def->type->drawTileRear) {
				tile->def->type->drawTileRear(room, tile);
			}
		}
	}

	/* Restore the matrix from before */
	glPopMatrix();
}

void renderRoom(Room* room, int direction) {
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
		renderTile(room, current, direction);
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

void renderWorld(World* world, int direction) {
	RoomList* current = world->rooms;

	while (current != NULL) {
		renderRoom(current->value, direction);
		current = current->next;
	}
}

/* This function take in a two char array and returns the tile type with that code */
TileType* getTileType(char* code) {
	int x;
	for (x = 0; x < NumTileTypes; x++) {
		if (memcmp(tileTypes[x].code, code, 2) == 0) {
			return(tileTypes+x);
		}
	}
	return NULL;
}

/* This searches a def list to find the right def */
Def* findFromDefList(DefList* list, char* code) {
	while (list != NULL) {
		if (strcmp(code, list->def->code) == 0) {
			return list->def;
		}
		list = list->next;
	}

	return NULL;
}

/* This function takes in a two char array and returns the tile definition with that code from the room, or globally */
Def* getTileDef(Room* room, char* code) {
	DefList* newList;
	Def* temp;
	/* First check the global array */
	temp = findFromDefList(globalTileDefs, code);

	if (temp != NULL) {
		return temp;
	}

	/* Now the room */
	temp = findFromDefList(room->defList, code);
	if (temp != NULL) {
		return temp;
	}

	/* If we don't find one, we should create one for the local room and return it */
	temp = malloc(sizeof(Def));
	memcpy(temp->code, code, 2);
	temp->def = NULL;
	/* By default everything is a normal floor tile */
	temp->type = floorTile;

	/* Add the def to the room's list */
	newList = malloc(sizeof(DefList));
	newList->def = temp;
	newList->next = room->defList;

	room->defList = newList;

	/* And return the new one */
	return temp;
}

World* newWorld() {
	World* world;

	world = malloc(sizeof(World));
	world->rooms = NULL;

	return world;
}

Room* newRoom(World* world, char* ID, int x, int y, int elevation, Metadata* metadata) {
	RoomList* list;

	/* Since it doesn't really matter, I've decided that since I'll only be adding rooms, I'll add to the front */
	list = malloc(sizeof(RoomList));
	list->next = world->rooms;
	world->rooms = list;

	list->value = malloc(sizeof(Room));
	list->value->ID = ID;
	list->value->pos.x = x;
	list->value->pos.y = y;
	list->value->pos.elevation = elevation;
	list->value->metadata = metadata;

	list->value->defList = NULL;

	list->value->map = malloc(sizeof(Map));
	list->value->map->TopLeft = NULL;

	list->value->undefinedTiles = NULL;

	return list->value;
}

Tile* getTile(Room* room, int x, int y) {
	/* TODO: Use some kind of caching to speed this up huge for consecutive access */
	Tile* tile = room->map->TopLeft;

	for (; x > 0; x--) {
		if (tile == NULL) {
			return NULL;
		}
		tile = tile->Right;
	}

	for (; y > 0; y--) {
		if (tile == NULL) {
			return NULL;
		}
		tile = tile->Down;
	}

	return tile;
}

/* This function is used for when we mass allocate tiles to interconnect the blob */
/* It uses sX and sY to use as starting co-ordinates */
Tile* makeTileBlob(int width, int height, int sX, int sY) {
	Tile* blob;
	Tile* cur;
	int i;

	if (width <= 0 || height <= 0) {
		return NULL;
	}

	blob = malloc(sizeof(Tile) * width * height);

	for (i=0; i < width * height; i++) {
		cur = blob + i;
		/* All of these check if the given other thing exists, and otherwise it's NULL */
		cur->Left = i%width == 0? NULL: blob + (i-1);
		cur->Right = (i+1)%width == 0? NULL: blob + (i+1);
		cur->Up = i < width? NULL: blob + (i-width);
		cur->Down = i >= width*(height-1)? NULL: blob + (i+width);
		/* Set the position */
		cur->pos.x = (i % width) + sX;
		cur->pos.y = (i / width) + sY;
	}

	return blob;
}

/* Join tile blobs left to right */
/* Should have same height */
/* Returns the new top left corner */
Tile* joinTileLeftRight(Tile* left, Tile* right) {
	Tile* curLeft = left;
	Tile* curRight = right;

	/* Edge Cases */
	if (curLeft == NULL) return right;
	if (curRight == NULL) return left;

	/* Find right edge of left */
	while (curLeft->Right != NULL) {
		curLeft = curLeft->Right;
	}

	while (curLeft != NULL && curRight != NULL) {
		curLeft->Right = curRight;
		curRight->Left = curLeft;

		curLeft = curLeft->Down;
		curRight = curRight->Down;
	}

	return left;
}

/* Join tile blobs top to bottom */
/* Should have same width */
/* Returns new top left corner */
Tile* joinTileTopBottom(Tile* top, Tile* bottom) {
	Tile* curTop = top;
	Tile* curBottom = bottom;

	/* Edge Cases */
	if (curTop == NULL) return bottom;
	if (curBottom == NULL) return top;

	/* Find bottom edge of top */
	while (curTop->Down != NULL) {
		curTop = curTop->Down;
	}

	while (curTop != NULL && curBottom != NULL) {
		curTop->Down = curBottom;
		curBottom->Up = curTop;

		curTop = curTop->Right;
		curBottom = curBottom->Right;
	}

	return top;
}

/* This gets the size of a blob. Just pass in addresses of the variables to store the values in */
void getTileBlobSize(Tile* blob, int* width, int* height) {
	Tile* cur = blob;
	(*width) = 0;
	(*height) = 0;

	if (cur == NULL) return;

	/* Compensate for the fact that we're comparing nexts, not currents */
	(*width) = 1;
	(*height) = 1;

	while (!(cur->Down == NULL && cur->Right == NULL)) {
		if (cur->Down != NULL) {
			(*height)++;
			cur = cur->Down;
		}
		if (cur->Right != NULL) {
			(*width)++;
			cur = cur->Right;
		}
	}
}


/* This function makes a room at least the given width, filling with '..' as needed */
/* If the given width is less than or equal to the current width, nothing is done */
/* Returns the top left */
Tile* expandRoomWidth(Room* room, int width) {
	Tile* tile = room->map->TopLeft;
	int curWidth;
	int curHeight;

	getTileBlobSize(tile, &curWidth, &curHeight);

	/* Special case for empty rooms */
	curHeight = (curHeight == 0)? 1: curHeight;

	return room->map->TopLeft = joinTileLeftRight(tile, makeTileBlob(width - curWidth, curHeight, curWidth, 0));
}

/* This function does the same thing as expandRoomWidth, but for height */
Tile* expandRoomHeight(Room* room, int height) {
	Tile* tile = room->map->TopLeft;
	int curWidth;
	int curHeight;

	getTileBlobSize(tile, &curWidth, &curHeight);

	/* Special case for empty rooms */
	curWidth = (curWidth == 0)? 1: curWidth;

	return room->map->TopLeft = joinTileTopBottom(tile, makeTileBlob(curWidth, height - curHeight, 0, curHeight));
}


/* This function makes a room the given size, filling it with ".." tiles as needed  */
/* Returns the new bottom right tile */
Tile* expandRoom(Room* room, int width, int height) {
	expandRoomWidth(room, width);
	return expandRoomHeight(room, height);
}

/* Definitions */
Tile* findPreCreatedTile(Room*, char*);
Tile* insertPreCreatedTile(Room*, Tile*);
Tile* removePreCreatedTile(Room*, Tile*);
Tile* preCreateTile(Room*, char*);

/* This function replaces one Tile object with another */
/* It doesn't copy over the tile's def, but does stitch it into the map, and copies over it's position */
/* It returns the replacement tile. The original tile could be discarded, but might not be safe to free */
Tile* replaceTile(Room* room, Tile* original, Tile* replacement) {

	replacement->Up = original->Up;
	replacement->Right = original->Right;
	replacement->Down = original->Down;
	replacement->Left = original->Left;

	if (replacement->Up != NULL) replacement->Up->Down = replacement;
	if (replacement->Right != NULL) replacement->Right->Left = replacement;
	if (replacement->Down != NULL) replacement->Down->Up = replacement;
	if (replacement->Left != NULL) replacement->Left->Right = replacement;

	if (replacement->Up == NULL && replacement->Left == NULL) {
		/* Top Left corner: Update the Map */
		room->map->TopLeft = replacement;
	}

	replacement->pos.x = original->pos.x;
	replacement->pos.y = original->pos.y;

	return replacement;
}

/* Wow, this ended up being really inefficient, I iterate over this room, like, 6 times, if I'm lucky */
Tile* setTile(Room* room, int x, int y, char* code) {
	Tile* tile;
	Tile* preCreated;

	expandRoom(room, x+1, y+1);
	tile = getTile(room, x, y);

	preCreated = findPreCreatedTile(room, code);
	if (preCreated != NULL) {
		/* We've got this tile created already, so we'll just patch it in. */
		/* First, take it out of the undefinedTiles list */
		removePreCreatedTile(room, preCreated);
		/* So far as I know, I can't free this tile, and instead must leak it, because I may have allocated it in a bulk block */
		tile = replaceTile(room, tile, preCreated);
	} else {
		tile->def = getTileDef(room, code);
	}

	return tile;
}

/* This function finds the room with the given ID in the World */
/* If no room exists with that ID, it returns NULL */
Room* findRoom(World* world, char* ID) {
	RoomList* list;
	for (list = world->rooms; list != NULL; list = list->next) {
		if (strcmp(list->value->ID, ID) == 0) {
			return list->value;
		}
	}

	return NULL;
}

/* This function gets the room with the given ID from the World, and if it doesn't exist, returns a new one */
Room* getRoom(World* world, char* ID) {
	Room* temp = findRoom(world, ID);
	if (temp != NULL) {
		return temp;
	}

	return newRoom(world, ID, 0, 0, 0, NULL);
}

/* This function adds metadata to the room */
/* If data with the given name exists, the value is changed to this value */
Metadata* setMetadata(Room* room, char* name, char* value) {
	Metadata* temp;

	if (strcmp(name, "elevation") == 0) {
		/* This is the elevation, it's stored with the position */
		room->pos.elevation = strtol(value, NULL, 10);
	}

	/* Find the metadata, if it exists */
	temp = getMetadata(room, name);
	if (temp != NULL) {
		temp->value = value;
		return temp;
	}

	/* Make a new entry */
	temp = malloc(sizeof(Metadata));
	temp->name = name;
	temp->value = value;
	temp->next = room->metadata;

	room->metadata = temp->next;

	return temp;
}

/* This function returns the metadata from the given room */
/* If the metadata doesn't exist, this returns NULL. */
Metadata* getMetadata(Room* room, char* name) {
	Metadata* temp;

	/* Find the metadata */
	for (temp = room->metadata; temp != NULL; temp = temp->next) {
		if (strcmp(temp->name, name) == 0) {
			return temp;
		}
	}
}

/* This function finds, in the given room, the tile with the given code */
/* It will return the first one it finds */
/* If it can't find a tile of that type, it will pre-create one and return it to you */
Tile* getTileByCode(Room* room, char code[2]) {
	Tile* current = room->map->TopLeft;
	int left = 0;

	while (current != NULL) {
		if (strncmp(current->def->code, code, 2) == 0) {
			return current;
		}

		if (left) {
			/* We're moving to the left */
			if (current->Left == NULL) {
				current = current->Down;
				left = !left;
			} else {
				current = current->Left;
			}
		} else {
			/* We're moving to the right */
			if (current->Right == NULL) {
				current = current->Down;
				left = !left;
			} else {
				current = current->Right;
			}
		}
	}

	current = findPreCreatedTile(room, code);
	if (current == NULL) {
		return preCreateTile(room, code);
	}

	return current;
}

/* This function is the action to take for a Stitch tile on render */
/* It expects the DefType->arg to be a room and tile */
int stitchDefAction(Room* room, Tile* tile) {
	Room* otherRoom;
	Tile* otherTile;
	int newX;
	int newY;

	otherRoom = tile->def->def->arg->roomTile.room;
	otherTile = tile->def->def->arg->roomTile.tile;

	newX = tile->pos.x - otherTile->pos.x;
	newY = tile->pos.y - otherTile->pos.y;

	if (tile->Left == NULL) {
		/* Left Edge, subtract 1 more from X */
		newX--;
	} else if (tile->Right == NULL) {
		/* Right Edge, add more to X */
		newX++;
	} else if (tile->Up == NULL) {
		/* Top Edge, subtract 1 more from Y */
		newY--;
	} else if (tile->Down == NULL) {
		newY++;
	} else {
		fputs("Must use Stitch only on an Edge\n", stderr);
		return 1;
	}

	newX = room->pos.x + newX;
	newY = room->pos.y + newY;

	otherRoom->pos.x = newX;
	otherRoom->pos.y = newY;

	/* Return 1 to tell the function that we haven't done any rendering */
	return 1;
}

/* This function inspects the given room's undefinedTiles list for the given tile, and if it exist, returns it. */
/* If the tile doesn't exist, this returns NULL */
Tile* findPreCreatedTile(Room* room, char* code) {
	Tile* current = room->undefinedTiles;

	while (current != NULL) {
		if (strncmp(current->def->code, code, 2) == 0) {
			return current;
		}

		current = current->Right;
	}

	return NULL;
}

/* This function inserts a Tile into the undefinedTiles list */
Tile* insertPreCreatedTile(Room* room, Tile* tile) {
	if (room->undefinedTiles == NULL) {
		tile->Right = NULL;
		tile->Left = NULL;
	} else {
		tile->Right = room->undefinedTiles;
		tile->Left = tile->Right->Left;
	}

	room->undefinedTiles = tile;
	return tile;
}

/* This function removes a Tile from the undefinedTiles list */
Tile* removePreCreatedTile(Room* room, Tile* tile) {

	if (tile->Left == NULL && tile->Right == NULL) {
		/* Only item in the list */
		room->undefinedTiles = NULL;
	} else if (tile->Left == NULL) {
		/* First item in the list */
		tile->Right->Left = NULL;
		room->undefinedTiles = tile->Right;
	} else if (tile->Right == NULL) {
		/* Last item in list */
		tile->Left->Right = NULL;
	} else {
		tile->Right->Left = tile->Left;
		tile->Left->Right = tile->Right;
	}

	return tile;
}

/* This function "pre-creates" a tile from an otherwise empty room */
/* That is, it makes a stub tile containing just the tile code and adds it to the undefinedTile list. */
/* Then, when a tile is added to a room, it is expected that this list will be consulted first for pre-created tiles */
/* This allows tiles to be referenced before they are created in a map */
Tile* preCreateTile(Room* room, char* code) {
	Tile* temp;

	temp = findPreCreatedTile(room, code);
	if (temp != NULL) {
		return temp;
	}

	temp = malloc(sizeof(Tile));
	temp->def = getTileDef(room, code);
	insertPreCreatedTile(room, temp);

	return temp;
}

/* This function sets a tile def for a room */
Def* setTileDef(World* world, Room* room, char* code, char* function, char* arg, char* typeCode) {
	Def* temp = getTileDef(room, code);

	temp->type = getTileType(typeCode);

	/* Don't really know how I'm going to handle functions yet, so ignore them */
	if (strcmp(function, "Stitch") == 0) {
		char* roomCode;
		char* tileCode;

		temp->def = malloc(sizeof(DefType));
		temp->def->defAction = &stitchDefAction;

		roomCode = arg + strspn(arg, " ");
		tileCode = strpbrk(roomCode, " ,");
		(*tileCode) ='\0';
		tileCode++;
		tileCode = tileCode + strspn(tileCode, " ,");

		temp->def->arg = malloc(sizeof(IntOrTile));
		temp->def->arg->roomTile.room = getRoom(world, roomCode);
		temp->def->arg->roomTile.tile = getTileByCode(temp->def->arg->roomTile.room, tileCode);
	}

	return temp;
}

