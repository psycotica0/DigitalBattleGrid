#include "map.h"
#include "outputMap.h"

void outputWorld(World* world) {
	RoomList* roomList;
	Metadata* meta;
	DefList* dList;
	Tile* curTile;
	Tile* curRow;
	for (roomList = world->rooms; roomList != NULL; roomList = roomList->next) {
		if (roomList->value != NULL) {
			printf("[Room %s]\n", roomList->value->ID);
			for (meta = roomList->value->metadata; meta != NULL; meta = meta->next) {
				printf("%s=%s\n", meta->name, meta->value);
			}
			printf("[Defs %s]\n", roomList->value->ID);
			for (dList = roomList->value->defList; dList != NULL; dList = dList->next) {
				printf("%.2s=BROKEN", dList->def->code);
				if (dList->def->type != NULL) {
					printf(":%.2s", dList->def->type->code);
				}
				puts("");
			}
			printf("[Map %s]\n", roomList->value->ID);
			for (curRow = roomList->value->map->TopLeft; curRow != NULL; curRow = curRow->Down) {
				for (curTile = curRow; curTile != NULL; curTile = curTile->Right) {
					printf("%.2s ", curTile->def->code);
				}
				puts("");
			}
		}
	}
}

