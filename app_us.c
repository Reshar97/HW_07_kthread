#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CYCLES 100
#define NUM_READERS 5
#define NUM_WRITERS 2

int counter = 0;
pthread_mutex_t wlock = PTHREAD_MUTEX_INITIALIZER;
pthread_t readers[NUM_READERS], writers[NUM_WRITERS];

void *reader_fn(void *arg) {
  int id = *(int *)arg;
  while (counter < CYCLES) {
    pthread_mutex_lock(&wlock);
    if (counter >= CYCLES) {
      pthread_mutex_unlock(&wlock);
      break;
    }
    printf("Reader %d: counter = %d\n", id, counter);
    pthread_mutex_unlock(&wlock);
    sleep(1);
  }
  return NULL;
}

void *writer_fn(void *arg) {
  int id = *(int *)arg;
  while (1) {
    pthread_mutex_lock(&wlock);
    if (counter >= CYCLES) {
      pthread_mutex_unlock(&wlock);
      break;
    }
    counter++;
    printf("Writer %d: counter = %d\n", id, counter);
    pthread_mutex_unlock(&wlock);
    sleep(1);
  }
  return NULL;
}

int main() {
  int i, ids[NUM_READERS + NUM_WRITERS];

  for (i = 0; i < NUM_READERS; i++) {
    ids[i] = i;
    if (pthread_create(&readers[i], NULL, reader_fn, &ids[i])) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < NUM_WRITERS; i++) {
    ids[NUM_READERS + i] = i;
    if (pthread_create(&writers[i], NULL, writer_fn, &ids[NUM_READERS + i])) {
      perror("pthread_create");
      exit(EXIT_FAILURE);
    }
  }

  for (i = 0; i < NUM_READERS; i++)
    pthread_join(readers[i], NULL);
  for (i = 0; i < NUM_WRITERS; i++)
    pthread_join(writers[i], NULL);

  return 0;
}
