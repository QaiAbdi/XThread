#ifndef __kthread_extended_H__
#define __kthread_extended_H__

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
//#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/fs.h>
#include <linux/net.h>
#include <linux/poll.h>

typedef struct {
        spinlock_t spin;
        long flags;
        int owner;
} xthread_mutex_t;

#define MUTEX_INITIALIZER { SPIN_LOCK_UNLOCKED, 0, -1 }

typedef struct {
        wait_queue_head_t queue;
} xthread_cond_t;

typedef struct{
	xthread_mutex_t b_lock;
	xthread_cond_t  b_proceed;
	int count;
} xthread_barrier_t;

typedef enum { 
        XTHREAD_COND_POLL_NONE, XTHREAD_COND_POLL_FILE, XTHREAD_COND_POLL_SOCK
} xthread_cond_poll_type_t;

typedef struct {
        xthread_cond_poll_type_t type;  /* Must set this. */
        union {                 /* Must set union field according to type. */
                struct file *file;      
                struct socket *sock;
        } obj;
        short events;           /* And this. */
        short revents;          /* Check response in this. */
        wait_queue_t wait;          /* Internal, don't set. */
        wait_queue_head_t *queue;   /* Internal, don't set */
} xthread_cond_poll_t;

typedef struct {
        poll_table poll_table_base;
        xthread_cond_poll_t *poll_array;
        int poll_array_size;
        int poll_array_index;
} xthread_cond_poll_table_t;

#define XTHREAD_COND_INITIALIZER(COND_NAME) { \
        __WAIT_QUEUE_HEAD_INITIALIZER(COND_NAME.queue) \
}

enum { XTHREAD_COND_WAIT_SUCCESS, XTHREAD_COND_WAIT_SIGNAL, XTHREAD_COND_WAIT_TIMEOUT };


int  xthread_create(struct task_struct *,int ,void *,void *);
void xthread_exit(struct task_struct *);

pid_t xthread_self();

void xthread_init_barrier(xthread_barrier_t *);
void xthread_barrier(xthread_barrier_t *,int );

void xthread_mutex_init(xthread_mutex_t *);
void xthread_mutex_lock(xthread_mutex_t *);
int  xthread_mutex_trylock(xthread_mutex_t *);
void xthread_mutex_unlock(xthread_mutex_t *);
void xthread_mutex_destroy(xthread_mutex_t *);

void xthread_cond_init(xthread_cond_t *);
int  xthread_cond_wait(xthread_cond_t *, xthread_mutex_t *);
void xthread_cond_signal(xthread_cond_t *);
void xthread_cond_broadcast(xthread_cond_t *);
void xthread_cond_destroy(xthread_cond_t *);


#endif // __kthread_extended_H__
