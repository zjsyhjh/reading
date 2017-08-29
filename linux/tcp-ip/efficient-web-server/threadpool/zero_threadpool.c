#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "zero_threadpool.h"
#include "zero_task.h"
#include "../debug/debug.h"

static void *zero_threadpool_main(void *arg);

struct zero_threadpool_t *threadpool_init(int number_of_threads)
{
	assert(number_of_threads > 0);

	struct zero_threadpool_t *pool;
	pool = (struct zero_threadpool_t*)malloc(sizeof(struct zero_threadpool_t));

	assert(pool != NULL);

	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * number_of_threads);

	assert(pool->threads != NULL);

	int rc;
	if ((rc = pthread_mutex_init(&(pool->m_lock), NULL))  != 0) {
		posix_error(rc, "threadpool_init: pthread_mutex_init() error");
	}

	if ((rc = pthread_cond_init(&(pool->m_cond), NULL)) != 0) {
		posix_error(rc, "threadpool_init: pthread_cond_init() error");
	}

	pool->used_of_threads = 0;
	pool->size_of_wait_queue = 0;
	pool->number_of_threads = number_of_threads;
	
	for (int i = 0; i < number_of_threads; i++) {
		if ((rc = pthread_create(&(pool->threads[i]), NULL, zero_threadpool_main, (void *)pool)) != 0) {
			posix_error(rc, "threadpool_init: pthread_create() error");
		}

		fprintf(stdout, "[INFO]: thread-%i started\n", i);
	}
	return pool;
}


void threadpool_add(struct zero_threadpool_t *pool, struct zero_task_t *task)
{
	assert(task != NULL);

	int rc;
	if ((rc = pthread_mutex_lock(&(pool->m_lock))) != 0) {
		posix_error(rc, "threadpool_add: pthread_mutex_lock() error");
	}
	//todo ...
	pool->size_of_wait_queue++;

	if ((rc = pthread_cond_signal(&(pool->m_cond))) != 0) {
		posix_error(rc, "threadpool_add: pthread_cond_signal() error");
	}

	return;
}



static void *zero_threadpool_main(void *arg)
{
	assert(arg != NULL);

	struct zero_threadpool_t *pool = (struct zero_threadpool_t*)arg;

	while (1) {
		pthread_mutex_lock(&(pool->m_lock));

		while (pool->size_of_wait_queue == 0) {
			pthread_cond_wait(&(pool->m_cond), &(pool->m_lock));
		}

		pool->size_of_wait_queue--;
		pthread_mutex_unlock(&(pool->m_lock));

		//todo run task

	}
	pthread_exit(NULL);
	return NULL;
}
