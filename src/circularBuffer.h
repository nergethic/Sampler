#pragma once
#include <stdint.h>

struct CircularBuffer {
	float* data;
	int32_t head;
	int32_t tail;
	int max; //of the buffer
	bool full;
};

void circular_buf_reset(CircularBuffer* cbuf);

void circular_buf_free(CircularBuffer* cbuf);

bool is_circular_buf_full(CircularBuffer* cbuf);

bool is_circular_buf_empty(CircularBuffer* cbuf);

CircularBuffer* circular_buf_init(float* buffer, size_t size);

size_t circular_buf_size(CircularBuffer* cbuf);

static void advance_pointer(CircularBuffer* cbuf, int32_t bytes);

static void retreat_pointer(CircularBuffer* cbuf, int32_t bytes);

void circular_buf_put(CircularBuffer* cbuf, float data); // OVERRITE

int circular_buf_get(CircularBuffer* cbuf, float* data);