#include "traffic.h"

struct Via vias[N_VIAS];
mutex_t mtx_io;

void init_via(struct Via* via) {
	via->first = NULL;
	via->last = NULL;
	via->n_ambulances = 0;
	via->n = 0;
}

void init_program(void) {
	lcd_init_4bits(2, 16, 6, 7, 2, 3, 4, 5);

	for (uint8_t i = 0; i < N_VIAS; ++i) {
		init_via(&vias[i]);
	}

	vias[0].sem_times[0] = V1_G;
	vias[0].sem_times[1] = V1_Y;
	vias[0].sem_times[2] = V1_R;
	vias[1].sem_times[0] = V2_G;
	vias[1].sem_times[1] = V2_Y;
	vias[1].sem_times[2] = V2_R;
	vias[2].sem_times[0] = VP_G;
	vias[2].sem_times[1] = 0;
	vias[2].sem_times[2] = VP_R;

	vias[0].sem_state = GRN;
	vias[1].sem_state = RED;
	vias[2].sem_state = RED;
}

void push(struct Via* via, char type) {
	if (via->n < MAX_CARS) {
		struct Car *new = (struct Car*) malloc(sizeof(struct Car));
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

char* prepare_str(struct Via* via) {
	char *queue_str = NULL;

	if (via->first != NULL) {
		queue_str = (char*) malloc(sizeof(char) * 17);
		struct Car* now = NULL;
		uint8_t i = 0;

		for (now = via->first; now != NULL; now = now->next, ++i) {
			queue_str[i] = now->type;
		}
		queue_str[i] = '\0';
	}

	return queue_str;
}

void print_vias(void) {
	char *str_vias[3];
	uint8_t i;

	for (i = 0; i < N_VIAS; ++i) {
		str_vias[i] = prepare_str(&vias[i]);
	}

	lcd_print(str_vias[0]);
	lcd_cursor_position(1, 0);
	lcd_print(str_vias[1]);
	lcd_cursor_position(1, 7);
	lcd_print(str_vias[2]);
	
	for (i = 0; i < 3; i++) {
		free(str_vias[i]);
	}
}

uint8_t buffer_cmd[BUFF_LEN];

void flush_buffer(void) {
	for (uint8_t i = 0; i < BUFF_LEN; ++i) {
		buffer_cmd[i] = 0;
	}
}

void process_cmd(void) {
	lcd_cursor_position(0, 1);
	lcd_print( (char*) buffer_cmd );
}
