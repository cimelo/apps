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
	uint8_t n_ambulances;
};

extern struct Via* principal;
extern struct Via* secondary;
extern struct Via* pedestrian;

// Instanciates a via
struct Via* init_via();

// Adds car to the traffic to the beginning of the
// queue
struct Car* push(struct Via* via);

// Removes the first element to enter the queue.
struct Car* pop(struct* Via via);

#endif
