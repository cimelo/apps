#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include <stdint.h>

struct Car {
	uint8_t type;
	struct Traffic* next;
	struct Traffic* prev;
};

struct Via {
	struct Car* first;
	struct Car* last;
	uint8_t num_ambulances;
};

extern struct Via* principal;
extern struct Via* secondary;
extern struct Via* pedestrian;

// Adds car to the traffic to the beginning of the
// queue
struct Car* push();

// Removes the first element to enter the queue.
struct Car* pop();

#endif
