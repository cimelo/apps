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

void push(struct Via* via) {
	struct Car* new = (struct Car*) malloc(sizeof(struct Car));

	if (!via->first) {

	}
	else {

	}
}

void pop(struct Via* via) {

}
