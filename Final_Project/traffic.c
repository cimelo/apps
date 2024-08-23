#include "traffic.h"

struct Via* principal = (struct Via*) malloc(sizeof(Via));
struct Via* secondary = (struct Via*) malloc(sizeof(Via));
struct Via* pedestrian = (struct Via*) malloc(sizeof(Via));

struct Via* init_via() {
	struct Via* via = (struct Via*) malloc(sizeof(Via));

	via.first = NULL;
	via.last = NULL;
	via.n_ambulances = 0;

	return via;
}

struct Car* push(struct Via* via) {
	if (!via) {
		via = (struct Via*) malloc(sizeof(Via));
	}
	else {

	}
}

struct Car* pop(struct* Via via) {

}
