/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (procfs)
*
*  \author     EmbeTronicX
* 
*  \Tested with Linux raspberrypi 5.10.27-v7l-embetronicx-custom+
*
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#include <linux/pci.h>
//мое
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/device.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/string.h>

/*
** I am using the kernel 5.10.27-v7l. So I have set this as 510.
** If you are using the kernel 3.10, then set this as 310,
** and for kernel 5.1, set this as 501. Because the API proc_create()
** changed in kernel above v5.5.
**
*/
#define LINUX_KERNEL_VERSION  510
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int32_t value = 0;
static int len = 1;


dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;
struct pci_dev *dev2;

/*
** Function Prototypes
*/
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);

/***************** Procfs Functions *******************/
static ssize_t  read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  read_proc2(struct file *filp, char __user *buffer, size_t length,loff_t * offset);

/*
** procfs operation sturcture
*/
static struct proc_ops proc_fops = {
        .proc_read = read_proc,
};

static struct proc_ops proc_fops2 = {
        .proc_read = read_proc2
};

// Мои переменные
struct task_struct *g, *p;
/*
** This function will be called when we read the procfs file
*/
static ssize_t read_proc2(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    if (len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    if(copy_to_user(buffer, "multiprocess\n", 13)){
    pr_err("Data Send : Err!\n");
    }
    return length;
}

static ssize_t read_proc(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    pr_info("Start read_proc");
    if(len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    char arr[4000];
    char str[40];
    int i;
    int k = 0;
    ////pci_dev
    while ((dev2 = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev2))){
        sprintf(str, "pci found [%d]\tflags[%d]\n", (dev2->device), dev2->dev_flags);
        for (i = k; i < k + 40; i++){
            arr[i] = str[i - k];
        }
        k += 40;
        if (k >= 4000) break;
    }
    ////multiprocess
//    do_each_thread(g, p) {
//        sprintf(str, "Task (pid = %d)\n", task_pid_nr(p));
//        for (i = k; i < k + 20; i++){
//            arr[i] = str[i - k];
//        }
//        k += 20;
//    } while_each_thread(g, p);
//    unsigned int sigHandlersAddr[64];
//    for_each_process(p) {
//            pr_info("Hi");
//        for (i = 0; i<64; i++)
//        sigHandlersAddr[i] =
//            printk("Get %d", (unsigned int) p->sighand->action[i].sa.sa_handler);

//        sprintf(str, "Task %s (pid = %d) sig[%s]\n",p->comm, task_pid_nr(p), sigHandlersAddr);
//            for (i = k; i < k + 100; i++){
//                arr[i] = str[i - k];
//            }
//            k += 100;
//            if (k >= 4000) break;


//    while ((ts1 = get_pid_task(find_get_pid(pid), PIDTYPE_PID))){
//        sprintf(str, "Task (pid = %d)\n", task_pid_nr(p));
//        for (i = k; i < k + 25; i++){
//            arr[i] = str[i - k];
//        }
//        k += 25;
//    }
//    do_each_thread(g, p) {
//
//        printk("Task %s (pid = %d) ",p->comm, task_pid_nr(p));
//        printk(" %d", p->signal->nr_threads);
//        printk(" %u \n", p->signal->flags);
//        printk("Task %d\n", p->signal.multiprocess);
//    } while_each_thread(g, p);

    //print
    if(copy_to_user(buffer, arr, k)){
    pr_err("Data Send : Err!\n");
    }
    return length;
}

/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
        pr_info("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    //Закоментил и заработало
//    cdev_init(&etx_cdev,&proc_fops);

    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        pr_info("Cannot add the device to the system\n");
        goto r_class;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_info("Cannot create the struct class\n");
        goto r_class;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_info("Cannot create the Device 1\n");
        goto r_device;
    }

    /*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("etx",NULL);

    if( parent == NULL )
    {
        pr_info("Error creating proc entry");
        goto r_device;
    }

    /*Creating Proc entry under "/proc/etx/" */
    proc_create("pci_dev", 0666, parent, &proc_fops);
    proc_create("multiprocess", 0666, parent, &proc_fops2);

    pr_info("Device Driver Insert...Done!!!\n");
    return 0;

    r_device:
    class_destroy(dev_class);
    r_class:
    unregister_chrdev_region(dev,1);
    return -1;
}

/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
    /* Removes single proc entry */
    //remove_proc_entry("etx/etx_proc", parent);

    /* remove complete /proc/etx */
    proc_remove(parent);

    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple Linux device driver (procfs)");
MODULE_VERSION("1.6");