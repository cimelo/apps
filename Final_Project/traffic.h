#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include <stdlib.h>
#include <stdint.h>

struct Car {
	uint8_t type;
	struct Car* next;
	struct Car* prev;
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
struct Via* init_via(void);

// Adds car to the traffic to the beginning of the
// queue
void push(struct Via* via, uint8_t type);

// Removes the first element that entered the queue.
void pop(struct Via* via);

#endif
