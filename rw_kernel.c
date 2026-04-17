#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>

#define NUM_READERS 5
#define NUM_WRITERS 2
#define CYCLES 1000000

static struct task_struct *readers[NUM_READERS];
static struct task_struct *writers[NUM_WRITERS];
static struct rw_semaphore rwsem;
static unsigned long long counter = 0;

int reader_fn(void *data) {
    int id = (int)(long)data;
    while (!kthread_should_stop()) {
        down_read(&rwsem);
        counter++;
        up_read(&rwsem);
        if (counter >= CYCLES) break;
    }
    return 0;
}

int writer_fn(void *data) {
    int id = (int)(long)data;
    while (!kthread_should_stop()) {
        down_write(&rwsem);
        counter++;
        up_write(&rwsem);
        if (counter >= CYCLES) break;
    }
    return 0;
}

static int __init rw_init(void) {
    int i;
    init_rwsem(&rwsem);
    for (i = 0; i < NUM_READERS; i++)
        readers[i] = kthread_run(reader_fn, (void *)(long)i, "reader_%d", i);
    for (i = 0; i < NUM_WRITERS; i++)
        writers[i] = kthread_run(writer_fn, (void *)(long)i, "writer_%d", i);
    return 0;
}

static void __exit rw_exit(void) {
    int i;
    for (i = 0; i < NUM_READERS; i++)
        kthread_stop(readers[i]);
    for (i = 0; i < NUM_WRITERS; i++)
        kthread_stop(writers[i]);
}

module_init(rw_init);
module_exit(rw_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Zubin");
MODULE_DESCRIPTION("Binary Search Example");
