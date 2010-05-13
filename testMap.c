#include <string.h>
#include "buildMap.h"

World* buildWorld(FILE* file) {
	World* world;
	Room* room;
	Tile* tileArray;
	int x,y;

	world = newWorld();
	room = newRoom(world, "Test", 0, 0, 0, NULL);

	for (x=0; x<3; x++) {
		for (y=0; y<3; y++) {

			if (x == 1 && y == 1) {
				/* We're in the middle tile */
				setTile(room, x, y, "..");
			} else {
				/* We're not the middle */
				setTile(room, x, y, "01");
			}
		}
	}

	return world;
}
