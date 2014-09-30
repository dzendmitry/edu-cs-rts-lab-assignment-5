#include <sys/mman.h>
#include <native/task.h>
#include <rtdk.h>

void task_body(void *cookie) {

  RT_TASK *current_task;
  RT_TASK_INFO current_task_info;

  rt_printf("Hello, world\n");
  // Получить информацию о текущей задаче
  current_task = rt_task_self();
  // Получить информацию о состоянии текущей задачи
  rt_task_inquire(current_task, &current_task_info);
  // Напечатать имя задачи
  rt_printf("Task name: %s\n", current_task_info.name);

}

int main(int argc, char **argv) {
  // Структура данных с информацией о задаче
  RT_TASK task;
  int err;
  // Инициализировать подсистему печати xenomai
  rt_print_auto_init(1);
  // Залочить текущие и будущие страницы памяти процесса
  mlockall(MCL_CURRENT|MCL_FUTURE);
  // Печатать при помощи функции из real-time printing library
  rt_printf("Starting task\n");
  // Создать новую real-time задачу La5Task-1 с приоритетом 50
  err = rt_task_create(&task, "Lab5Task-1", 0, 50, 0);
  // Запустить задачу в случае успешного создания
  if (!err) {
    rt_task_start(&task, &task_body, NULL);
  }
  // Дождаться завершения работы задачи
  rt_task_join(&task);

}
