#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "liquidcrystal.h"

#define N_VIAS 3

struct Car {
	char type;
	struct Car* next;
	struct Car* prev;
};

struct Via {
	struct Car* first;
	struct Car* last;
	uint8_t n_ambulances;
	uint8_t n;
};

extern struct Via vias[];

// Instanciates a via
void init_via(struct Via* via);

// Adds car to the traffic to the beginning of the
// queue
void push(struct Via* via, char type);

// Removes the first element that entered the queue.
void pop(struct Via* via);

// Helper function prepares the queue to print
// Remember to always free the return from this
// function!
void send_print(struct Via* via);

#endif
