#ifndef __ZERO_PRIORITY_QUEUE_H__
#define __ZERO_PRIORITY_QUEUE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/* pq_ptr[1] ... pq_ptr[16] */
#define PQ_DEFAULT_CAPACITY (16 + 1)

/* function pointer */
typedef int (*zero_pq_cmp_ptr)(void *ptr_i, void *ptr_j);

struct zero_pq_t {
	size_t size;
	size_t capacity;
	zero_pq_cmp_ptr cmp;
	void **pq_ptr;
};

static void resize(struct zero_pq_t *zero_pq, size_t new_capacity) {
	void **new_ptr = (void **)malloc(sizeof(void *) * new_capacity);
	assert(new_ptr != NULL);

	memcpy(new_ptr, zero_pq->pq_ptr, sizeof(void *) * (zero_pq->capacity));
	free(zero_pq->pq_ptr);

	zero_pq->pq_ptr = new_ptr;
	zero_pq->capacity = new_capacity;
}

static void exch(void *item1, void *item2) {
	void *tmp = item1;
	item1 = item2;
	item2 = tmp;
}

static void swim(struct zero_pq_t *zero_pq, size_t k) {
	while (k > 1) {
		if (zero_pq->cmp(zero_pq->pq_ptr[k], zero_pq->pq_ptr[k >> 1]) >= 0) break;
		exch(zero_pq->pq_ptr[k], zero_pq->pq_ptr[k >> 1]);
		k >>= 1;
	}
}

static void sink(struct zero_pq_t *zero_pq, size_t k) {
	while (2 * k <= zero_pq->size) {
		size_t j = 2 * k;
		if (j < zero_pq->size && zero_pq->cmp(zero_pq->pq_ptr[j + 1], zero_pq->pq_ptr[j]) < 0) j++;
		if (zero_pq->cmp(zero_pq->pq_ptr[j], zero_pq->pq_ptr[k]) > 0) break;
		exch(zero_pq->pq_ptr[j], zero_pq->pq_ptr[k]);
		k = j;
	}
}


void zero_pq_insert(struct zero_pq_t *zero_pq, void *item) {
	if (zero_pq->size + 1 == zero_pq->capacity) {
		resize(zero_pq, ((zero_pq->capacity - 1) << 1) + 1); 
	}

	zero_pq->pq_ptr[++zero_pq->size] = item;
	swim(zero_pq, zero_pq->size);
}


void zero_pq_delmin(struct zero_pq_t *zero_pq) {
	assert(zero_pq->size > 0);

	exch(zero_pq->pq_ptr[1], zero_pq->pq_ptr[zero_pq->size]);
	zero_pq->size--;

	sink(zero_pq, 1);
	if (zero_pq->size > 0 && zero_pq->size <= (zero_pq->capacity - 1) / 4) {
		resize(zero_pq, (zero_pq->capacity - 1) / 2 + 1);
	}
}

size_t zero_pq_size(struct zero_pq_t *zero_pq) {
	return zero_pq->size;
}

bool zero_pq_empty(struct zero_pq_t *zero_pq) {
	return zero_pq->size == 0;
}

void *zero_pq_min(struct zero_pq_t *zero_pq) {
	if (zero_pq_empty(zero_pq)) return NULL;
	return zero_pq->pq_ptr[1];
}




#endif