#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <openssl/evp.h>

#include "queue.h"

void enqueue (char * data, struct producer_consumer_queue * aq)
{
  volatile struct producer_consumer_queue * q = aq;
  // упакуем задание в новую структуру
  struct producer_consumer_queue_item * p = (typeof(p))malloc(sizeof(*p));

  p->data = malloc(strlen(data) + 1);
  strcpy(p->data, data);
  p->next = 0;

  // получим "эксклюзивный" доступ к очереди заданий
  pthread_mutex_lock(&aq->lock);
  // ... и добавим новое задание туда:

  if (q->tail)
  {
      q->tail->next = p;
  }
  else
  {
    q->head = p;
    // очередь была пуста, а теперь нет -- надо разбудить потребителей
    pthread_cond_broadcast(&aq->cond);
  }
  q->tail = p;

  asm volatile ("" : : : "memory");
  // зафиксируем изменения очереди в памяти

  // разрешаем доступ всем снова
  pthread_mutex_unlock(&aq->lock);
}

char * dequeue(struct producer_consumer_queue *aq)
{
  volatile struct producer_consumer_queue *q = aq;
  // получаем эксклюзивный доступ к очереди:
  pthread_mutex_lock(&aq->lock);


  while (!q->head && q->is_alive)
  {
    // очередь пуста, делать нечего, ждем...
    pthread_cond_wait(&aq->cond, &aq->lock);
    // wait разрешает доступ другим на время ожидания
  }

  // запоминаем текущий элемент или 0, если очередь умерла
  struct producer_consumer_queue_item * p = q->head;

  if (p)
  {
    // и удаляем его из очереди
    q->head = q->head->next;

    if (!q->head)
    {
      q->tail = q->head;
    }
    asm volatile ("" : : : "memory");
    // зафиксируем изменения очереди в памяти
  }

  // возвращаем эксклюзивный доступ другим участникам
  pthread_mutex_unlock(&aq->lock);

  // отдаём данные

  char * data = NULL;

  if (p)
  {
      data = malloc(strlen(p->data) + 1);
      strcpy(data, p->data);

      free(p->data);
      free(p);
  }
  return data;
}

struct producer_consumer_queue * producer_consumer_queue_create()
{
  struct producer_consumer_queue *q = (typeof(q))malloc(sizeof(*q));
  q->head = q->tail = 0;
  q->is_alive = 1;
  pthread_mutex_init(&q->lock, 0);
  pthread_cond_init(&q->cond, 0);

  return q;
}

// И процедура для закрытия очереди:

void producer_consumer_queue_stop(struct producer_consumer_queue *aq)
{
  volatile struct producer_consumer_queue *q = aq;
  // для обращения к разделяемым переменным необходим эксклюзивный доступ
  pthread_mutex_lock(&aq->lock);
  q->is_alive = 0;
  asm volatile ("" : : : "memory");
  // зафиксируем изменения очереди в памяти
  pthread_cond_broadcast(&aq->cond); // !!!!! avp
  pthread_mutex_unlock(&aq->lock);
}
