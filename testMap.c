#include <string.h>
#include "buildMap.h"

World* buildWorld(FILE* file) {
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
				memcpy((tileArray+x+(3*y))->def->code, "..", 2);

				(tileArray+x+(3*y))->def->type = getTileType("..");

				/* The middle is the only one that has a tile on all sides */
				(tileArray+x+(3*y))->Left = tileArray+(x-1)+(3*y);
				(tileArray+x+(3*y))->Right = tileArray+(x+2)+(3*(y));
				(tileArray+x+(3*y))->Up = tileArray+(x)+(3*(y-1));
				(tileArray+x+(3*y))->Down = tileArray+(x)+(3*(y+2));
			} else {
				/* We're not the middle */
				memcpy((tileArray+x+(3*y))->def->code, "01", 2);

				(tileArray+x+(3*y))->def->type = getTileType("01");

				(tileArray+x+(3*y))->Up = y==0? NULL: tileArray+(x)+(3*(y-1));
				(tileArray+x+(3*y))->Down = y==2? NULL: tileArray+(x)+(3*(y+1));
				(tileArray+x+(3*y))->Left = x==0? NULL: tileArray+(x-1)+(3*(y));
				(tileArray+x+(3*y))->Right = x==2? NULL: tileArray+(x+1)+(3*(y));
			}
		}
	}

	return world;
}
