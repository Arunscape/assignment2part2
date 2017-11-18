#include "restaurant.h"

/*
	Sets *ptr to the i'th restaurant. If this restaurant is already in the cache,
	it just copies it directly from the cache to *ptr. Otherwise, it fetches
	the block containing the i'th restaurant and stores it in the cache before
	setting *ptr to it.
*/
void getRestaurant(restaurant* ptr, int i, Sd2Card* card, RestCache* cache) {
	uint32_t block = REST_START_BLOCK + i/8;
	if (block != cache->cachedBlock) {
		while (!card->readBlock(block, (uint8_t*) cache->block)) {
			Serial.print("readblock failed, try again");
		}
		cache->cachedBlock = block;
	}
	*ptr = cache->block[i%8];
}

// Swaps the two restaurants (which is why they are pass by reference).
void swap(RestDist& r1, RestDist& r2) {
	RestDist tmp = r1;
	r1 = r2;
	r2 = tmp;
}

// Selection sort to sort the restaurants.
void ssort(RestDist restaurants[]) {
	for (int i = NUM_RESTAURANTS-1; i >= 1; --i) {
		int maxId = 0;
		for (int j = 1; j <= i; ++j)
			if (restaurants[j].dist > restaurants[maxId].dist) {
				maxId = j;
			}
		swap(restaurants[i], restaurants[maxId]);
	}
}

// Computes the manhattan distance between two points (x1, y1) and (x2, y2).
int16_t manhattan(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
	return abs(x1-x2) + abs(y1-y2);
}

/*
	Fetches all restaurants from the card, saves their RestDist information
	in restaurants[], and then sorts them based on their distance to the
	point on the map represented by the MapView.
*/
void getAndSortRestaurants(const MapView& mv, RestDist restaurants[], Sd2Card* card, RestCache* cache,int currentRating) {
	restaurant r;
	//Serial.println(currentRating);

	// first get all the restaurants and store their corresponding RestDist information.
	//and only choose restaurants with appropriate rating
	int counter=0;
	Serial.print("Current Rating: ");
	Serial.println(currentRating);

	for (int i = 0; i < NUM_RESTAURANTS; ++i) {

		getRestaurant(&r, i, card, cache);



		// Serial.print("restaurant rating: ");
		// Serial.print(r.rating);
		// Serial.print(" ");
		// Serial.println((int)max(floor((r.rating+1)/2), 1));

		if(currentRating<= max((int)floor((r.rating+1)/2), 1)){
			// Serial.println("true");
			restaurants[counter].index = i;
			restaurants[counter].dist = manhattan(lat_to_y(r.lat), lon_to_x(r.lon),
																			mv.mapY + mv.cursorY, mv.mapX + mv.cursorX);
			counter++;
		}
	}

	// Now sort them.
	ssort(restaurants);
}
