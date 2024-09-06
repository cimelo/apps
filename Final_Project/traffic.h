#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include "hal.h"
#include "ch.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "liquidcrystal.h"

#define N_LANES 3
#define MAX_CARS 7
#define BUFF_LEN 16

// Times of the Semaphores in seconds
#define L1_G 10
#define L1_Y 4

#define L2_G 5
#define L2_Y 3

#define LP_G 5
#define LP_Y 2

#define GRN 0
#define YLW 1
#define RED 2

// Pins for the LCD
#define RS 6
#define EN 7
#define D4 2
#define D5 3
#define D6 4
#define D7 5

// Port and pins for the semaphores
#define PIN_G1	8
#define PIN_R1	9
#define PIN_G2	10
#define PIN_R2	11
#define PIN_GP	12
#define PIN_RP	13

#define AMB_WAIT 2

struct Lane {
	char cars[MAX_CARS];
	uint8_t n;
	uint8_t n_ambs;
	uint8_t sem_times[3];
};

struct Sem_handler {
	uint8_t id_green;
	uint8_t interr;
};

struct Timer_cfg {
	uint8_t pins[2];
	uint8_t periods[2];
};

extern mutex_t mtx_sem;
extern condition_variable_t cond_sem;
extern uint8_t is_timeout;


extern struct Lane lanes[];
extern struct Sem_handler sem_handler;
extern uint8_t buffer_cmd[BUFF_LEN];
// Mutex used to read command sent into to the
// serial input and processing it's data
extern mutex_t mtx_print;
extern condition_variable_t cond_msg;
extern uint8_t has_msg;
extern char queues_str[3][MAX_CARS+1];

// Instanciates a lane
void init_lane(struct Lane* lane);

// Sets the programs default initial configurations
void init_program(void);

// Init the matrix with NULL chars
void init_queues_str(void);

// Adds car to the traffic to the beginning of the
// queue
void push(uint8_t lane, char type);

// Removes the first element that entered the queue
void pop(struct Lane* lane);

// Helper function prepares the queue to print
// Remember to free the str after the use
char* prepare_str(struct Lane* lane);

// Send the lanes to print into the LCD
void print_lanes(void);

// Writes the buffer with '\0'
void flush_buffer_cmd(void);

// Receives the command buffer and prepares the lanes
void process_cmd(void);

void update_lane(uint8_t id_lane);

void moviment(uint8_t id_lane);

void start_sem_timer(thread_t *tp, struct Timer_cfg *cfg);

#endif
