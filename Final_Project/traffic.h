#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include <stdlib.h>
#include <stdint.h>

struct Car {
	char type;
	struct Car* next;
	struct Car* prev;
};

struct Via {
	struct Car* first;
	struct Car* last;
	uint8_t n_ambulances;
	uint8_t n_carros;
};

extern struct Via* principal;
extern struct Via* secondary;
extern struct Via* pedestrian;

// Instanciates a via
struct Via* init_via(void);

// Adds car to the traffic to the beginning of the
// queue
void push(struct Via* via, char type);

// Removes the first element that entered the queue.
void pop(struct Via* via);

// Helper function prepares the queue to print
char* prepare_print(struct Via* via);

#endif
