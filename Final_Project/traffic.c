#include "traffic.h"

struct Via* principal = NULL;
struct Via* secondary = NULL;
struct Via* pedestrian = NULL;

struct Via* init_via() {
	struct Via* via = (struct Via*) malloc(sizeof(struct Via));

	via->first = NULL;
	via->last = NULL;
	via->n_ambulances = 0;

	return via;
}

void push(struct Via* via, uint8_t type) {
	struct Car* new = (struct Car*) malloc(sizeof(struct Car));
	new->prev = NULL;
	new->type = type;

	if (!via->first) {
		via->first = new;
		via->last = new;
		if (type == 'A') {
			++via->n_ambulances;
		}
		new->next = NULL;
	}
	else {
		new->next = via->first;
		via->first->prev = new;
		via->first = new;
	}
}

void pop(struct Via* via) {
	// Verifies if the queue is not empty
	if (via->first) {
		if (via->first->type == 'A') {
			--via->n_ambulances;
		}

		// Verifies if there's only one element
		if (via->first == via->last) {
			free(via->first);
			via->first = NULL;
			via->last = NULL;
		}
		else {
			via->last->prev->next = NULL;
			free(via->last);
		}
	}
}
