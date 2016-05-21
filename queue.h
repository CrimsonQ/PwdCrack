#ifndef QUEUE_H
#define QUEUE_H

#include "pthread.h"

// Структура данных для одного задания
struct producer_consumer_queue_item
{
  struct producer_consumer_queue_item *next;
  // здесь идут собственно данные. вы можете поменять этот кусок,
  // использовав структуру, более специфичную для вашей задачи
  char * data;
};

// Очередь с дополнительными структурами для синхронизации.
// в ней будут храниться произведённые, но ещё не потреблённые задания.
struct producer_consumer_queue
{
  struct producer_consumer_queue_item *head;
  struct producer_consumer_queue_item * tail;
                              // head == tail == 0, если очередь пуста
  pthread_mutex_t lock;       // мьютекс для всех манипуляций с очередью
  pthread_cond_t cond;        // этот cond "сигналим", когда очередь стала НЕ ПУСТОЙ
  int is_alive;               // показывает, не закончила ли очередь свою работу
};

void enqueue (char * data, struct producer_consumer_queue * aq);

char * dequeue(struct producer_consumer_queue * aq);

struct producer_consumer_queue * producer_consumer_queue_create();

void producer_consumer_queue_stop(struct producer_consumer_queue * aq);

#endif // QUEUE_H
