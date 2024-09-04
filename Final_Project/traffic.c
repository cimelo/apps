#include "traffic.h"

struct Lane lanes[N_LANES];
struct Sem_handler sem_handler = {
	.id_green = 0,
	.sem_state = GRN
};

uint8_t has_msg = 0;
mutex_t mtx_print;
condition_variable_t cond_msg;
uint8_t buffer_cmd[BUFF_LEN];
char queues_str[3][MAX_CARS+1];

mutex_t mtx_sem;
condition_variable_t cond_sem;
uint8_t is_timeout = 0;


void init_lane(struct Lane* lane) {
	lane->n = 0;
}

void init_queues_str(void) {
	uint8_t i, j;

	for (i = 0; i < N_LANES; ++i) {
		for (j = 0; j <= MAX_CARS; ++j) {
			queues_str[i][j] = '\0';
		}
	}
}

void init_program(void) {
	lcd_init_4bits(2, 16, 6, 7, 2, 3, 4, 5);
	uint8_t i;

	for (i = 0; i < N_LANES; ++i) {
		init_lane(&lanes[i]);
	}

	for (i = 0; i < 6; ++i) {
		palSetPadMode(IOPORT2, i, PAL_MODE_OUTPUT_PUSHPULL);
		palClearPad(IOPORT2, i);
	}

	init_queues_str();

	lanes[0].sem_times[0] = L1_G;
	lanes[0].sem_times[1] = L1_Y;
	lanes[1].sem_times[0] = L2_G;
	lanes[1].sem_times[1] = L2_Y;
	lanes[2].sem_times[0] = LP_G;
	lanes[2].sem_times[1] = 0;

	lanes[0].sem_state = GRN;
	lanes[1].sem_state = RED;
	lanes[2].sem_state = RED;

	flush_buffer_cmd();
}

void push(struct Lane* lane, char type) {
	if (lane->n < MAX_CARS) {
		lane->cars[lane->n] = type;
		++lane->n;

		if (type == 'A') {
			++lane->n_ambs;
		}
	}
}

void pop(struct Lane* lane) {
	if (lane->n) {
		if (lane->cars[0] == 'A') {
			--lane->n_ambs;
		}

		for (uint8_t i = 0; i < lane->n; ++i) {
			lane->cars[i] = lane->cars[i+1];
		}

		--lane->n;
	}
}

void print_lanes(void) {
	uint8_t i, j;

	lcd_clear();

	for (i = 0; i < N_LANES; ++i) {
		for (j = 0; j < lanes[i].n; ++j) {
			queues_str[i][j] = lanes[i].cars[lanes[i].n-j-1];
		}
		queues_str[i][j] = '\0';
	}

	if (lanes[0].n) {
		lcd_cursor_position(0, 0);
		lcd_print(queues_str[0]);
	}
	if (lanes[1].n) {
		lcd_cursor_position(1, 0);
		lcd_print(queues_str[1]);
	}
	if(lanes[2].n) {
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
	uint8_t lane = 0, i = 0;
	char *tmp = (char*)buffer_cmd;

	for (; i < BUFF_LEN && tmp[i] != ';'; ++i) {
		if (tmp[i] >= '0' && tmp[i] <= '2') {
			lane = tmp[i]-'0';
		}
		else {
			push(&lanes[lane], tmp[i]);
		}
	}
}

void update_lane(uint8_t id_lane) {
	for (int i = 0; i < lanes[id_lane].n; ++i) {
		if (lanes[id_lane].cars[i] == 'A') {
			if (i && lanes[id_lane].cars[i-1] == 'C') {
				lanes[id_lane].cars[i] = 'C';
				lanes[id_lane].cars[i-1] = 'A';
			}
		}
	}
}

void moviment(uint8_t id_lane) {
	if (lanes[id_lane].n) {
		update_lane(id_lane);
		if (sem_handler.id_green == id_lane) {
			pop(&lanes[id_lane]);
		}
	}
}
