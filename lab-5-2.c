#include <stdlib.h>
#include <sys/mman.h>
#include <native/task.h>
#include <rtdk.h>
#include <native/sem.h>

#define TASKS 10
#define DELAY 2 // SECONDS

RT_SEM sem;

void task_body(void *cookie) {

  rt_sem_p(&sem, TM_INFINITE);

  RT_TASK *current_task;
  RT_TASK_INFO current_task_info;

  current_task = rt_task_self();
  rt_task_inquire(current_task, &current_task_info);

  rt_printf("Task name: %s started with priority %d\n",
    current_task_info.name,
    current_task_info.cprio
  );

  rt_task_sleep(DELAY * 1000000L); // delay for 1 second

  rt_printf("Task name: %s is shutting down\n", current_task_info.name);

}

int main(int argc, char **argv) {

  char task_name[TASKS][16];
  RT_TASK task[TASKS];

  rt_print_auto_init(1);

  if(rt_sem_create(&sem, "Semaphore", 0, S_PRIO) != 0) {
	exit(-1);
  }
  
  mlockall(MCL_CURRENT|MCL_FUTURE);

  for(int i = 0; i < TASKS; i++) {
    snprintf(task_name[i], 16, "Lab5Task-%d", i);
    if (rt_task_create(&task[i], task_name[i], 0, 10 + (i * 2), T_JOINABLE) != 0) {
      exit(-1);
    }
  }

  for (int i = 0; i < TASKS; i++) {
    rt_printf("Starting task %s\n", task_name[i]);
    rt_task_start(&task[i], &task_body, NULL);
  }

  rt_printf("All tasks started\n");

  rt_sem_broadcast(&sem);
  
  for(int i = 0; i < TASKS; i++) {
    rt_task_join(&task[i]);
  }

  rt_printf("All tasks shutted down\n");
  
  rt_sem_delete(&sem);
}
