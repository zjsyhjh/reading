#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "zero_threadpool.h"
#include "zero_task.h"
#include "../base/zero_debug.h"

static void *zero_thread_main(void *arg);

struct zero_threadpool_t *threadpool_init(int number_of_threads)
{
	if (number_of_threads <= 0) {
		log_err("number_of_threads must greater than 0");
		return NULL;
	}

	struct zero_threadpool_t *pool;
	pool = (struct zero_threadpool_t*)malloc(sizeof(struct zero_threadpool_t));
	if (pool == NULL) {
		log_err("threadpool_init: malloc error");
		return NULL;
	}

	pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * number_of_threads);
	if (pool == NULL) {
		log_err("threadpool_init: malloc error");
		return NULL;
	}

	int rc;
	if ((rc = pthread_mutex_init(&(pool->m_lock), NULL))  != 0) {
		log_err("threadpool_init: pthread_mutex_init() error");
		return NULL;
	}

	if ((rc = pthread_cond_init(&(pool->m_cond), NULL)) != 0) {
		log_err("threadpool_init: pthread_cond_init() error");
		return NULL;
	}
	
	for (int i = 0; i < number_of_threads; i++) {
		if ((rc = pthread_create(&(pool->threads[i]), NULL, zero_thread_main, (void *)pool)) != 0) {
			log_err("threadpool_init: pthread_create() error");
			return NULL;
		}
		log_info("thread: %d started", i);
	}
	pool->wait_queue = (struct zero_task_t *)malloc(sizeof(struct zero_task_t));
	if (pool->wait_queue == NULL) {
		log_err("threadpool_init: malloc error");
		return NULL;
	}
	pool->wait_queue->func = NULL;
	pool->wait_queue->args = NULL;
	pool->wait_queue->next_task = NULL;

	pool->size_of_wait_queue = 0;
	pool->used_of_threads = 0;
	pool->number_of_threads = number_of_threads;
	pool->shutdown = 0;

	return pool;
}


void threadpool_add(struct zero_threadpool_t *pool, struct zero_task_t *task)
{
	if (task == NULL) {
		log_err("threadpool_add: task == NULL");
		exit(1);
	}

	int rc;
	if ((rc = pthread_mutex_lock(&(pool->m_lock))) != 0) {
		log_err("threadpool_add: pthread_mutex_lock() error");
		exit(1);
	}
	
	task->next_task = pool->wait_queue->next_task;
	pool->wait_queue->next_task = task;

	pool->size_of_wait_queue++;
	/*
	 * 有任务加入，发送一个信号给另外一个正处于阻塞等待状态的线程，使其脱离阻塞状态，继续执行
	 * 如果没有线程处于阻塞等待状态，pthread_cond_signal也会成功返回
	 * 使用pthread_cond_signal一般不会有“惊群”现象，它最多只给一个线程发信号
	 * 加入有多个线程正在阻塞等待这个条件变量的话，那么是根据各等待线程的优先级的高低确定
	 * 如果各线程优先级相同，则根据等待时间的长短来确定哪个线程获得信号
	 * 但无论如何，一个pthread_cond_signal调用最多发送信号一次
	 */
	if ((rc = pthread_cond_signal(&(pool->m_cond))) != 0) {
		log_err("threadpool_add: pthread_cond_signal() error");
		exit(1);
	}

	return;
}

/*
 * 线程工作函数
 */
static void *zero_thread_main(void *arg)
{
	if (arg == NULL) {
		log_err("zero_thread_main: arg == NULL");
		return NULL;
	}

	struct zero_threadpool_t *pool = (struct zero_threadpool_t*)arg;

	while (1) {
		pthread_mutex_lock(&(pool->m_lock));
		/*
		 * 等待队列为空，说明没有task要做
		 * pthread_cond_wait将互斥锁解锁，再将条件变量加锁
		 * pthread_cond_signal会唤醒某个线程
		 * 被唤醒的线程首先会再次锁互斥锁（m_lock)，如果成功，再进行条件的判断（while循环条件）
		 */
		while (pool->size_of_wait_queue == 0 && !(pool->shutdown)) {
			pthread_cond_wait(&(pool->m_cond), &(pool->m_lock));
		}

		if (pool->shutdown) {
			break;
		}
		struct zero_task_t *task = pool->wait_queue->next_task;
		pool->wait_queue->next_task = task->next_task;
		pool->size_of_wait_queue--;
		pthread_mutex_unlock(&(pool->m_lock));

		//run task
		(*(task->func))(task->args);
		free(task);
	}
	pthread_mutex_unlock(&(pool->m_lock));
	pthread_exit(NULL);
	return NULL;
}


void threadpool_free(struct zero_threadpool_t *pool) {
	if (pool->threads) free(pool->threads);
	struct zero_task_t *task;
	while(pool->wait_queue->next_task != NULL) {
		task = pool->wait_queue->next_task;
		pool->wait_queue->next_task = pool->wait_queue->next_task->next_task;
		free(task);
	}
}

void threadpool_destroy(struct zero_threadpool_t *pool) {
	pthread_mutex_lock(&(pool->m_lock));
	do {
		if (pool->shutdown) break;
		pool->shutdown = 1;
		/* 唤醒所有线程 */
		pthread_cond_broadcast(&(pool->m_cond));
		
		pthread_mutex_unlock(&(pool->m_lock));

		for (int i = 0; i < pool->number_of_threads; i++) {
			pthread_join(pool->threads[i], NULL);
			log_info("thread %d exit", i);
		}
	} while(0);

	pthread_mutex_destroy(&(pool->m_lock));
	pthread_cond_destroy(&(pool->m_cond));
	threadpool_free(pool);
}


