#include<linux/module.h>
#include<linux/kernel.h>
#include <linux/proc_fs.h>
#include<linux/sched.h>

struct task_struct *task;

for_each_process(p) {
        printk("Task %s (pid = %d)\n",p->comm, task_pid_nr(p));
}

