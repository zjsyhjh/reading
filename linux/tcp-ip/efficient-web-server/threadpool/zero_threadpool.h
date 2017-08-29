/* $begin zero_threadpool.h */
#ifndef __ZERO_THREADPOOL_H__
#define __ZERO_THREADPOOL_H__

#include <pthread.h>
#include "zero_task.h"

struct zero_threadpool_t {
	pthread_t *threads;
	pthread_mutex_t m_lock;
	pthread_cond_t m_cond;

	int size_of_wait_queue;
	int number_of_threads;
	int used_of_threads;
};

struct zero_threadpool_t *threadpool_init(int number_of_threads);
void threadpool_add(struct zero_threadpool_t *pool, struct zero_task_t *task);
void threadpool_destroy(struct zero_threadpool_t *pool, int graceful);

#endif
/* $end zero_threadpool.h */