#ifndef _MAP_H_
#define _MAP_H_
#include <gl.h>
#include "chunk_string.h"

struct Tile;
struct Room;

/* The Tile Type is meant to be globally unique and static */
/* There should be a single Tile Type for every Built In Tile Code */
typedef struct TileType {
	char code[2]; /* This is the two letter code used to make this tile */
	/* This is the function used to draw the tile of this type */
	void (*drawTile)(struct Room*, struct Tile*);
	/* This is a boolean that stores whether or not the callList should be used for this tile instead of calling drawTile */
	/* This is because some tiles are always drawn the same, and others are different depending on the circumstance */
	int useCallList;
	/* The call list is a storred model used to draw the tile so it doesn't have to be computed every time */
	GLuint callList;
} TileType;

/* I've decided that for what I can see the DefTypes take in either an integer or a specific tile from a room */
/* Thus, until I can come up with something more elegant I will give each def type a reference to this union as input */
typedef union IntOrTile {
	int integer;
	struct {
		struct Room* room;
		struct Tile* tile;
	} roomTile;
} IntOrTile;

/* This is the special properties exhibited by the tile */
/* The function held here is intended to be called when rendering the tile */
/* It will take in a reference to the Tile that it is to represent, it can find its arguments in tile.def.def.arg */
/* If it returns a 0 then it's assumed the function has drawn the tile and the TileType is not to be used */
/* If it returns a 1 then it's done something else and the TypeType should still be used to render */
typedef struct DefType {
	union IntOrTile* arg;
	int (*defAction)(struct Room*, struct Tile*);
} DefType;

/* This is a definition of a given tile */
/* It contains the code that was used to represent this tile */
/* It also contains a link to the type of tile tile to represent this tile with */
/* Lastly it contains a link to the type of tile this is */
typedef struct Def {
	char code[2];
	struct TileType* type;
	struct DefType* def;
} Def;

/* This is a list of tile definitions for a given room */
typedef struct DefList {
	struct Def* def;
	struct DefList* next;
} DefList;

/* This is the position of a tile in the room */
/* (In Squares) */
typedef struct TilePos {
	int x;
	int y;
} TypePos;

/* This is the 2D Tile item in the map */
/* This is implemented as a 2D linked list, each one linking to the surrounding tiles */
/* I chose to have each one link in all directions rather than just going right and down because I feel it will be often that tiles will have to check their surroundings */
/* In these cases it will be much faster if each tile can inspect all its neighbours */
typedef struct Tile {
	struct Def* def;
	struct TilePos pos;

	struct Tile* Left;
	struct Tile* Right;
	struct Tile* Up;
	struct Tile* Down;
} Tile;

/* This is the link to the map */
typedef struct Map {
	struct Tile* TopLeft;
} Map;

/* This is a linked list of metadata name value pairs */
typedef struct Metadata {
	char* name;
	char* value;

	struct Metadata* next;
} Metadata;

/* This is the position of the top left of a room in the world */
/* (x and y are in sqaures, elevation is in feet) */
typedef struct RoomPos {
	int x;
	int y;
	int elevation;
} RoomPos;

/* This is a room */
typedef struct Room {
	struct Map* map;
	struct Metadata* metadata;
	struct RoomPos pos;
	struct DefList* defList;
	char* ID;
	/* Like with TileDef, the call list is where we'll store the room prebuilt so we don't have to render it every time */
	GLuint callList;
	/* This is a list (Going Left to Right) of all tiles which have been referenced, but not yet placed in the map */
	/* This is so that one can reference a tile in a definition before that room's map is read */
	struct Tile* undefinedTiles;
} Room;

/* This is a linked list for rooms */
typedef struct RoomList {
	struct Room* value;
	struct RoomList* next;
} RoomList;

/* Whole world (So far as the file is concerned) */
typedef struct World {
	struct RoomList* rooms;
	/* Like with TileDef and Room, the call list is where we'll store the world prebuilt so we don't have to render it every time */
	GLuint callList;
} World;

/* This function initializes a lot of the constants used here */
void initMap();

/* This function is used to render the world */
void renderWorld(World*);

/* This function takes in the two letter code and returns the TileType you should use.*/
TileType* getTileType(char*);

/* This function allocates and initializes the world structures */
World* newWorld();

/* This function allocates and initializes the room structure in the given world */
Room* newRoom(World* world, char* ID, int x, int y, int elevation, Metadata* metadata);

/* This function sets the tile at the given position in the room to that which is described by the given code */
Tile* setTile(Room* room, int x, int y, char* code);

/* This function finds the room with the given ID in the World */
/* If no room exists with that ID, it returns NULL */
Room* findRoom(World* world, char* ID);

/* This function gets the room with the given ID from the World, and if it doesn't exist, returns a new one */
Room* getRoom(World* world, char* ID);

/* This function adds metadata to the room */
/* If data with the given name exists, the value is changed to this value */
Metadata* setMetadata(Room* room, char* name, char* value);

/* This function returns the value of some metadata from the given room */
/* If the metadata doesn't exist, this returns NULL. */
/* No distinction is made between a value which is saved as NULL, and a non-existant name */
Metadata* getMetadata(Room* room, char* name);

/* This function sets a tile def for a room */
Def* setTileDef(World*, Room* room, char* code, char* function, char* arg, char* typeCode);

#endif
