#include <stdlib.h>
#include <assert.h>
#include "zero_task.h"

struct zero_task_t *task_init(void (*func)(void *), void *args)
{
	struct zero_task_t *task;
	task = (struct zero_task_t*)malloc(sizeof(struct zero_task_t));
	assert(task != NULL);

	task->func = func;
	task->args = args;
	return task;
}