#include "map.h"
#include "buildMap.h"
#include "outputMap.h"

int main() {
	World* world;

	initMap();
	world = buildWorld(stdin);

	outputWorld(world);
	return 0;
}
