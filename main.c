/*  
 *  
 */
#include <linux/init.h>
#include <linux/module.h>   /* Needed by all modules */
#include <linux/moduleparam.h>
#include <linux/kernel.h>   /* Needed for KERN_INFO */
#include <linux/kthread.h>  // for threads
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Test driver module");
MODULE_AUTHOR("Marek");

static struct task_struct *thread1;
static struct task_struct *thread2;
static struct mutex thread_mutex;

static int thread1_fn(void *arg)
{
	unsigned long j0,j1;
	int delay = 60*HZ;

	printk(KERN_DEBUG "start thread1\n");
	j0 = jiffies;
	j1 = j0 + delay;

	while (time_before(jiffies, j1)) {
		mutex_lock(&thread_mutex);
		printk(KERN_DEBUG "thread1...\n");
		udelay(100);
		mutex_unlock(&thread_mutex);
		msleep(1);
		if (kthread_should_stop())
			break;
        	schedule();
	}

	printk(KERN_DEBUG "stop thread1\n");

	return 0;
}

static int thread2_fn(void *arg)
{
	unsigned long j0, j1;
	int delay = 80 * HZ;

	printk(KERN_DEBUG "start thread2\n");

	j0 = jiffies;
	j1 = j0 + delay;

	while(time_before(jiffies, j1)) {
		mutex_lock(&thread_mutex);
		printk(KERN_DEBUG "thread2...\n");
		udelay(100);
		mutex_unlock(&thread_mutex);

		msleep(1);
		if (kthread_should_stop())
			break;
		schedule();
	}
	printk(KERN_DEBUG "stop thread2\n");
	return 0;
}

static int __init load_module(void)
{
	struct timeval ktv;

	printk(KERN_DEBUG "load module\n");

	mutex_init(&thread_mutex);


	thread1 = kthread_create(thread1_fn, NULL, "thread1");
	thread2 = kthread_create(thread2_fn, NULL, "thread2");

	if (thread1) {
		printk(KERN_DEBUG "thread1 exist, wakeup\n");
		wake_up_process(thread1);
	}

	if (thread2) {
		printk(KERN_DEBUG "thread2 exist, wakeup\n");
		wake_up_process(thread2);
	}

	do_gettimeofday(&ktv);

	printk(KERN_DEBUG "load module end at time %d \n", ktv.tv_sec);



    /* 
     * A non 0 return means init_module failed; module can't be loaded. 
     */
    return 0;
}

void __exit unload_module(void)
{
	int ret;
	printk(KERN_DEBUG "unload module\n");
	ret = kthread_stop(thread1);
	printk(KERN_DEBUG "thread1 stop ret %d\n", ret);

	ret = kthread_stop(thread2);
	printk(KERN_DEBUG "thread2 stop ret %d\n", ret);
}


module_init(load_module);
module_exit(unload_module);

