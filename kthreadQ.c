#include <linux/version.h>

#include <linux/module.h>
#if defined(MODVERSIONS)
#include <linux/modversions.h>
#endif

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/completion.h>
#include "kthread_extended.h"

static int counter=0;
struct task_struct *mythread[10];
xthread_mutex_t lock;
long t=1;
int i;



void *Print(void *threadid)
{
  
  printk("it's me %d\n",(int)threadid);

  
  xthread_mutex_lock(&lock);  
  for(i=0;i<10000;i++)
    counter++;
  printk("counter equals:%d\n",counter);
 
  xthread_mutex_unlock(&lock);
  xthread_self();
//  xthread_exit(&mythread[(int) threadid]);
  
}


int init_module(void)
{
  xthread_mutex_init(&lock);
  printk("Main(): before initializing.\n");
  for(t=0;t<10;t++){
    printk("creating thread %ld\n",t);
    xthread_create(&mythread[t],NULL,Print,(void *)t); 
    
  }
    //xthread_exit(&mythread[1]);
    xthread_self();
    return(0);

}

/* remove the module */
void cleanup_module(void)
{
    printk("cleanup_module\n");
    
    //xthread_mutex_destroy(&lock);

}

MODULE_LICENSE("GPL");