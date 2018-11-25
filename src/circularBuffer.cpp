#pragma once;
#include "circularBuffer.h"
#include <stdlib.h>
#include <assert.h>

void circular_buf_reset(CircularBuffer* cbuf)
{
	assert(cbuf);

	cbuf->head = 0;
	cbuf->tail = 0;
	cbuf->full = false;
}

void circular_buf_free(CircularBuffer* cbuf)
{
	assert(cbuf);
	free(cbuf);
}

bool is_circular_buf_full(CircularBuffer* cbuf)
{
	assert(cbuf);

	return cbuf->full;
}

bool is_circular_buf_empty(CircularBuffer* cbuf)
{
	assert(cbuf);

	return (!cbuf->full && (cbuf->head == cbuf->tail));
}

CircularBuffer* circular_buf_init(float* buffer, size_t size)
{
	assert(buffer && size);

	CircularBuffer* cbuf = (CircularBuffer*)malloc(sizeof(CircularBuffer));
	assert(cbuf);

	cbuf->data = buffer;
	cbuf->max = size;
	circular_buf_reset(cbuf);

	assert(is_circular_buf_empty(cbuf));

	return cbuf;
}

size_t circular_buf_size(CircularBuffer* cbuf)
{
	assert(cbuf);

	size_t size = cbuf->max;

	if (!cbuf->full)
	{
		if (cbuf->head >= cbuf->tail)
		{
			size = (cbuf->head - cbuf->tail);
		}
		else
		{
			size = (cbuf->max + cbuf->head - cbuf->tail);
		}
	}

	return size;
}

static void advance_pointer(CircularBuffer* cbuf, int32_t bytes)
{
	assert(cbuf);

	if (cbuf->full)
	{
		cbuf->tail = (cbuf->tail + 1) % cbuf->max;
	}

	cbuf->head = (cbuf->head + 1) % cbuf->max;
	cbuf->full = (cbuf->head == cbuf->tail);
}

static void retreat_pointer(CircularBuffer* cbuf, int32_t bytes)
{
	assert(cbuf);

	cbuf->full = false;
	cbuf->tail = (cbuf->tail + 1) % cbuf->max;
}

void circular_buf_put(CircularBuffer* cbuf, float data) // OVERRITE
{
	assert(cbuf && cbuf->data);

	cbuf->data[cbuf->head] = data;

	advance_pointer(cbuf, sizeof(float));
}

int circular_buf_get(CircularBuffer* cbuf, float* data)
{
	assert(cbuf && data && cbuf->data);

	int r = -1;

	if (!is_circular_buf_empty(cbuf))
	{
		*data = cbuf->data[cbuf->tail];
		retreat_pointer(cbuf, sizeof(float));

		r = 0;
	}

	return r;
}