#include "traffic.h"

struct Via vias[N_VIAS];

void init_via(struct Via* via) {
	via->first = NULL;
	via->last = NULL;
	via->n_ambulances = 0;
	via->n = 0;
}

void push(struct Via* via, char type) {
	if (via->n < 17) {
		struct Car* new = (struct Car*) malloc(sizeof(struct Car));
		new->prev = NULL;
		new->type = type;
		++via->n;

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

		--via->n;
	}
}

void send_print(struct Via* via) {
	char* queue_str = (char*) malloc(sizeof(char) * 17);
	struct Car* now = NULL;
	uint8_t i = 0;

	for (now = via->first; now != NULL; now = now->next, ++i) {
		queue_str[i] = now->type;
	}
	queue_str[i] = '\0';

	char str_num[5];
	sprintf(str_num, "%d", vias->n);

	lcd_print(queue_str);
	lcd_cursor_position(0, 14);
	lcd_print(str_num);

	// chprintf(chp, queue_str);

	free(queue_str);
}
