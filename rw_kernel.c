#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/mutex.h>

#define CYCLES 100
#define NUM_READERS 5
#define NUM_WRITERS 2

static int counter = 0;
static struct task_struct *readers[NUM_READERS];
static struct task_struct *writers[NUM_WRITERS];
static struct mutex wlock;
static int reader_ids[NUM_READERS];
static int writer_ids[NUM_WRITERS];

static int reader_fn(void *data) {
  int id = *(int *)data;

  while (!kthread_should_stop()) {
    mutex_lock(&wlock);
    if (counter >= CYCLES) {
      mutex_unlock(&wlock);
      break;
    }
    printk(KERN_INFO "Reader %d: counter = %d\n", id, counter);
    mutex_unlock(&wlock);

    msleep(1000);
  }
  return 0;
}

static int writer_fn(void *data) {
  int id = *(int *)data;

  while (!kthread_should_stop()) {
    mutex_lock(&wlock);
    if (counter >= CYCLES) {
      mutex_unlock(&wlock);
      break;
    }
    counter++;
    printk(KERN_INFO "Writer %d: counter = %d\n", id, counter);
    mutex_unlock(&wlock);

    msleep(1000);
  }
  return 0;
}

static int __init rw_init(void) {
  int i;
  mutex_init(&wlock);

  for (i = 0; i < NUM_READERS; i++) {
    reader_ids[i] = i;
    readers[i] = kthread_run(reader_fn, &reader_ids[i], "reader_%d", i);
    if (IS_ERR(readers[i]))
      return PTR_ERR(readers[i]);
  }

  for (i = 0; i < NUM_WRITERS; i++) {
    writer_ids[i] = i;
    writers[i] = kthread_run(writer_fn, &writer_ids[i], "writer_%d", i);
    if (IS_ERR(writers[i]))
      return PTR_ERR(writers[i]);
  }

  return 0;
}

static void __exit rw_exit(void) {
  int i;
  for (i = 0; i < NUM_READERS; i++)
    if (readers[i])
      kthread_stop(readers[i]);

  for (i = 0; i < NUM_WRITERS; i++)
    if (writers[i])
      kthread_stop(writers[i]);

  printk(KERN_INFO "Module unloaded. Final counter: %d\n", counter);
}

module_init(rw_init);
module_exit(rw_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anton Zubin");
MODULE_DESCRIPTION("Writer-Reader example with kthread");
