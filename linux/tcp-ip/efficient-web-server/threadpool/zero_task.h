/* $begin zero_task.h */
#ifndef __ZERO_TASK_H__
#define __ZERO_TASK_H__

struct zero_task_t {
	void (*func)(void *);
	void *args;
};

void task_init(void (*func)(void *), void *args);

#endif
/* $end zero_task.h */
