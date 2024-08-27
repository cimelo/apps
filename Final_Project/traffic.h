#ifndef TRAFFIC_H_
#define TRAFFIC_H_

#include "hal.h"
#include "ch.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "liquidcrystal.h"

#define N_VIAS 3
#define MAX_CARS 7
#define BUFF_LEN 16

#define V1_G 10
#define V1_Y 2
#define V1_R 5

#define V2_G 8
#define V2_Y 2
#define V2_R 5

#define VP_G 8
#define VP_R 5

#define GRN 0
#define YLW 1
#define RED 2

struct Via {
	char cars[MAX_CARS];
	uint8_t n;
	uint8_t sem_state;
	uint8_t sem_times[3];
};

extern struct Via vias[];
extern uint8_t buffer_cmd[16];
// Mutex used to read command sent into to the
// serial input and processing it's data
extern mutex_t mtx_print;
extern condition_variable_t cond_msg;
extern uint8_t has_msg;
extern char queues_str[3][MAX_CARS+1];

// Instanciates a via
void init_via(struct Via* via);

// Sets the programs default initial configurations
void init_program(void);

// Init the matrix with NULL chars
void init_queues_str(void);

// Adds car to the traffic to the beginning of the
// queue
void push(struct Via* via, char type);

// Removes the first element that entered the queue
void pop(struct Via* via);

// Helper function prepares the queue to print
// Remember to free the str after the use
char* prepare_str(struct Via* via);

// Send the vias to print into the LCD
void print_vias(void);

// Writes the buffer with '\0'
void flush_buffer_cmd(void);

// Receives the command buffer and prepares the vias
void process_cmd(void);

void update_via(uint8_t id_via);

void moviment(uint8_t id_via);

#endif
