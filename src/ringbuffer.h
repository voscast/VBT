// ringbuffer functions for VBT
//
// Copyright 2007-2018 by Daniel Noethen.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
//
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <pthread.h>

struct ringbuf {
	char *buf;
	char *w_ptr;
	char *r_ptr;
	unsigned int size;
	unsigned int full;
    pthread_mutex_t mutex;
}; 


int rb_init(struct ringbuf *rb, unsigned int size);
int rb_filled(struct ringbuf *rb);
int rb_space(struct ringbuf *rb);
unsigned int rb_read(struct ringbuf *rb, char *dest);
unsigned int rb_read_len(struct ringbuf *rb, char *dest, unsigned int len);

int rb_write(struct ringbuf *rb, char* src, unsigned int size);
int rb_free(struct ringbuf *rb);

#endif /*RINGBUFFER_H*/

