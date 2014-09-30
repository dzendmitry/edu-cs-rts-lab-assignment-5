#include <stdlib.h>
#include <sys/mman.h>
#include <native/task.h>
#include <rtdk.h>
#include <native/mutex.h>

RT_MUTEX mutex;
volatile long int shared_resource = 0;

#define ITERATIONS 100000
#define TASKS 10
#define DELAY 10000L

void task_body(void *cookie) {

  RT_TASK *current_task;
  RT_TASK_INFO current_task_info;

  current_task = rt_task_self();
  rt_task_inquire(current_task, &current_task_info);

  rt_printf("Task name: %s started with priority %d\n",
    current_task_info.name,
    current_task_info.cprio
  );

  for (int i = 0; i < ITERATIONS; i++) {
	rt_mutex_acquire(&mutex, TM_INFINITE);
    long int r = shared_resource;
	rt_mutex_release(&mutex);
    r = r + 1;
	rt_mutex_acquire(&mutex, TM_INFINITE);
    shared_resource = r;
	rt_mutex_release(&mutex);
    rt_task_sleep(DELAY);
  }

  rt_printf("Task name: %s is shutting down\n", current_task_info.name);

}

int main(int argc, char **argv) {

  char task_name[TASKS][16];
  RT_TASK task[TASKS];

  rt_print_auto_init(1);
  
  if(rt_mutex_create(&mutex, "Mutex") != 0) {
	exit(-1);
  }

  mlockall(MCL_CURRENT|MCL_FUTURE);

  for(int i = 0; i < TASKS; i++) {
    snprintf(task_name[i], 16, "Lab5Task-%d", i);
    if (rt_task_create(&task[i], task_name[i], 0, 50 - (i*2), T_JOINABLE) != 0) {
      exit(-1);
    }
  }
  for (int i = 0; i < TASKS; i++) {
    rt_printf("Starting task %s\n", task_name[i]);
    rt_task_start(&task[i], &task_body, NULL);
  }

  rt_printf("All tasks started\n");

  for(int i = 0; i < TASKS; i++) {
    rt_task_join(&task[i]);
  }
  
  rt_mutex_delete(&mutex);

  rt_printf("All tasks stopped, shared_resource = %d\n", shared_resource);
	
}