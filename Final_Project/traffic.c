#include "traffic.h"

struct Via vias[N_VIAS];
mutex_t mtx_print;
uint8_t has_msg = 0;
condition_variable_t cond_msg;
uint8_t buffer_cmd[BUFF_LEN];
char queues_str[3][MAX_CARS+1];

void init_via(struct Via* via) {
	via->n = 0;
}

void init_queues_str(void) {
	uint8_t i, j;

	for (i = 0; i < N_VIAS; ++i) {
		for (j = 0; j <= MAX_CARS; ++j) {
			queues_str[i][j] = '\0';
		}
	}
}

void init_program(void) {
	lcd_init_4bits(2, 16, 6, 7, 2, 3, 4, 5);

	for (uint8_t i = 0; i < N_VIAS; ++i) {
		init_via(&vias[i]);
	}

	init_queues_str();

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

	flush_buffer_cmd();
}

void push(struct Via* via, char type) {
	if (via->n < MAX_CARS) {
		via->cars[via->n] = type;
		++via->n;
	}
}

void pop(struct Via* via) {
	if (via->n) {
		for (uint8_t i = 0; i < via->n; ++i) {
			via->cars[i] = via->cars[i+1];
		}

		--via->n;
	}
}

void print_vias(void) {
	uint8_t i, j;

	lcd_clear();

	for (i = 0; i < N_VIAS; ++i) {
		for (j = 0; j < vias[i].n; ++j) {
			queues_str[i][j] = vias[i].cars[vias[i].n-j-1];
		}
		queues_str[i][j] = '\0';
	}

	if (vias[0].n) {
		lcd_cursor_position(0, 0);
		lcd_print(queues_str[0]);
	}
	if (vias[1].n) {
		lcd_cursor_position(1, 0);
		lcd_print(queues_str[1]);
	}
	if(vias[2].n) {
		lcd_cursor_position(1, 8);
		lcd_print(queues_str[2]);
	}
}

void flush_buffer_cmd(void) {
	for (uint8_t i = 0; i < BUFF_LEN; ++i) {
		buffer_cmd[i] = ' ';
	}
}

void process_cmd(void) {
	uint8_t via = 0, i = 0;
	char *tmp = (char*)buffer_cmd;

	for (; i < BUFF_LEN && tmp[i] != ';'; ++i) {
		if (tmp[i] >= '0' && tmp[i] <= '2') {
			via = tmp[i]-'0';
		}
		else {
			push(&vias[via], tmp[i]);
		}
	}
}

void update_via(uint8_t id_via) {
	//uint8_t tmp = 0;

	for (int i = 0; i < vias[id_via].n; ++i) {
		if (vias[id_via].cars[i] == 'A') {
			if (i && vias[id_via].cars[i-1] == 'C') {
				vias[id_via].cars[i] = 'C';
				vias[id_via].cars[i-1] = 'A';
			}
		}
	}
}

void moviment(uint8_t id_via) {
	if (vias[id_via].n) {
		update_via(id_via);
		if (vias[id_via].sem_state != RED || vias[id_via].cars[0] == 'A') {
			pop(&vias[id_via]);
		}
	}
}
