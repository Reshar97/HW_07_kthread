#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define CYCLES 1000000

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
unsigned long long counter = 0;

void* reader(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_rwlock_rdlock(&rwlock);
        counter++;
        pthread_rwlock_unlock(&rwlock);
        if (counter >= CYCLES) break;
    }
    return NULL;
}

void* writer(void* arg) {
    int id = *(int*)arg;
    while (1) {
        pthread_rwlock_wrlock(&rwlock);
        counter++;
        pthread_rwlock_unlock(&rwlock);
        if (counter >= CYCLES) break;
    }
    return NULL;
}

int main() {
    pthread_t readers[NUM_READERS], writers[NUM_WRITERS];
    int ids[NUM_READERS + NUM_WRITERS], i;

    for (i = 0; i < NUM_READERS + NUM_WRITERS; i++) ids[i] = i;

    for (i = 0; i < NUM_READERS; i++)
        pthread_create(&readers[i], NULL, reader, &ids[i]);
    for (i = 0; i < NUM_WRITERS; i++)
        pthread_create(&writers[i], NULL, writer, &ids[NUM_READERS + i]);

    for (i = 0; i < NUM_READERS; i++) pthread_join(readers[i], NULL);
    for (i = 0; i < NUM_WRITERS; i++) pthread_join(writers[i], NULL);

    printf("Counter: %llu\n", counter);
    return 0;
}
