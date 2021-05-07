#include "kthread_extended.h"


int xthread_create(struct task_struct *thread,int Null,void *fn,void *arg){
    thread= kthread_run(fn,(void*) arg,"Create thread");
    return 0;
}

void xthread_exit(struct task_struct *thread){
  /*  
  set_current_state(TASK_INTERRUPTIBLE);
      while (!kthread_should_stop()) {
    schedule();
    set_current_state(TASK_INTERRUPTIBLE);
  }
  */
    printk("before stops\n");
    kthread_stop(thread);//stops a thrad created by kthread_create
}

pid_t xthread_self(){
      pid_t tid= current->pid;
      printk("%ld\n", tid);
      return  tid; 
}

void xthread_init_barrier(xthread_barrier_t *br){
	br->count=0;
	xthread_mutex_init(&(br->b_lock));
	xthread_cond_init(&(br->b_proceed));	  
}

void xthread_barrier(xthread_barrier_t *br,int num_threads){
	xthread_mutex_lock(&(br->b_lock));
	br->count++;
	if (br -> count == num_threads) { 
	br -> count = 0; 
	xthread_cond_broadcast(&(br -> b_proceed)); 
	    } 
 else 
	while (xthread_cond_wait(&(br -> b_proceed),&(br -> b_lock)) != 0); 
	xthread_mutex_unlock(&(br -> b_lock)); 
      } 


void xthread_mutex_init(xthread_mutex_t *mx)
{
        spin_lock_init(&mx->spin);
        mx->owner = -1;

}

void xthread_mutex_lock(xthread_mutex_t *mx)
{
        long flags;

        if (mx->owner == smp_processor_id()) {
            printk(KERN_ERR "xthread_mutex recursive deadlock\n");
            dump_stack();
            BUG();
        }

        spin_lock_irqsave(&mx->spin, flags);
        mx->flags = flags;
        mx->owner = smp_processor_id();

}

int xthread_mutex_trylock(xthread_mutex_t *mx)
{
        unsigned long flags;
        int owner;

        if (mx->owner == (owner = smp_processor_id())) {
            printk(KERN_ERR "xthread_mutex recursive deadlock\n");
            dump_stack();
            BUG();
        }

        local_irq_save(flags);

        if (spin_trylock(&mx->spin)) {
                mx->flags = flags;
                mx->owner = owner;
                return 1;
        }

        local_irq_restore(flags);

        return 0;
}

void xthread_mutex_unlock(xthread_mutex_t *mx)
{
        unsigned long flags = mx->flags;
        mx->owner = -1;
        spin_unlock_irqrestore(&mx->spin, flags);
}

void xthread_mutex_destroy(xthread_mutex_t *mx)
{
	kfree(mx);
}

void xthread_cond_init(xthread_cond_t *cv)
{
        init_waitqueue_head(&cv->queue);
}

int xthread_cond_wait(xthread_cond_t *cv, xthread_mutex_t *mx)
{
        wait_queue_t wait;
        int ret;

        init_waitqueue_entry(&wait, current);
        wait.flags |= WQ_FLAG_EXCLUSIVE;
        add_wait_queue(&cv->queue, &wait);
        current->state = TASK_INTERRUPTIBLE;

        xthread_mutex_unlock(mx);

        schedule();
        ret = signal_pending(current) ? XTHREAD_COND_WAIT_SIGNAL : XTHREAD_COND_WAIT_SUCCESS;
        current->state = TASK_RUNNING;
        remove_wait_queue(&cv->queue, &wait);

        xthread_mutex_lock(mx);

        return ret;
}

void xthread_cond_signal(xthread_cond_t *cv)
{
        /* Wakes up only one condition waiter, because of WQ_FLAG_EXCLUSIVE */
        wake_up_interruptible(&cv->queue);
}

void xthread_cond_broadcast(xthread_cond_t *cv)
{
        /* Wakes up all waiters */
        wake_up_interruptible_all(&cv->queue);
}


void  xthread_cond_destroy(xthread_cond_t *cv){
    kfree(cv);
}
