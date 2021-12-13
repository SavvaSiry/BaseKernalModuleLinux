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
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/proc_fs.h>
#include <linux/pci.h>
#include <linux/pid.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/string.h>

#define LINUX_KERNEL_VERSION  510
/*
** Function Prototypes
*/
static int      __init etx_driver_init(void);
static void     __exit etx_driver_exit(void);

/***************** Procfs Functions *******************/
static ssize_t  read_pci(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  read_mult(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  write_pci(struct file *filp, const char *buf, size_t len, loff_t * off);
static ssize_t  write_mult(struct file *filp, const char *buf, size_t len, loff_t * off);


/************ procfs operation sturcture **************/
static struct proc_ops proc_fops = {
        .proc_read = read_pci,
        .proc_write =  write_pci
};

static struct proc_ops proc_fops2 = {
        .proc_read = read_mult,
        .proc_write =  write_mult
};

/***************** struct/variables *******************/
static struct class *dev_class;
static struct cdev etx_cdev;
static struct proc_dir_entry *parent;
struct task_struct *g, *p;
struct pci_dev *dev2;
static char *arr;
static char *str;
static int len = 1;
unsigned int sigHandlersAddr[64];
unsigned int maxSig;
int k = 0;
int i;
dev_t dev = 0;
char pciarr[10]="";
char proc_pid[10]="";
char multarr[10]="\0\0\0\0\0\0\0\0\0\0";


////Multiprocess
static ssize_t read_mult(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    pr_info("Start read_mult");
    if (len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    k = 0;
    if (strcmp("\0\0\0\0\0\0\0\0\0\0", multarr) == 0) { //Если массив multarr, в который мы записываем равен \0... тогда выполняем общий вывод
        do_each_thread(g, p) {
            maxSig = p->sighand->action[0].sa.sa_flags; //Находим максимальный сигнал
            for (i = 1; i<64; i++)
                if (p->sighand->action[i].sa.sa_flags > maxSig) maxSig = p->sighand->action[i].sa.sa_flags;
            sprintf(str,"Pid = %d \tNr_th = %d\tSF=%x\tMAX_SIGNAL=%x  \t\tName = %s\n",
                    task_pid_nr(p), p->signal->nr_threads, p->signal->flags, maxSig, p->comm);
            for (i = k; i < k + 100; i++){ //производим запись в большой масиив arr, str буффера
                arr[i] = str[i - k];
            }
            for (i = 0; i < 100; i++){ //очищаем буффер, чтобы не осталось мусора в массиве
                str[i] = '\0';
            }
            k += 100; //добавляем к индексу значение длины буффера str
            if (k >= 90000) break; //проверяем на переполнение буффера arr
        } while_each_thread(g, p);
    copy_to_user(buffer, arr, k);
    } else { //Если мы изменяли буффер multarr, то у нас вызывается слудующий код
        do_each_thread(g, p) {
            for (i = 0; i < 1; i++){ //очищаем массив в которй мы записываем значение id процесса
                proc_pid[i] = '\0';
            }
            sprintf(proc_pid, "%d\n", task_pid_nr(p)); //копируем значение id процесса в буффер proc_pid
            //сравниваем значения multarr, в котором получаем значения через echo, с id процесса
            //если значния равны, то выполняем вывод информациидля конкретного id, переданного в multarr
            if (strcmp(multarr, proc_pid) == 0){
                maxSig = p->sighand->action[0].sa.sa_flags;
                for (i = 1; i<64; i++)
                    if (p->sighand->action[i].sa.sa_flags > maxSig) maxSig = p->sighand->action[i].sa.sa_flags;
                sprintf(str, "Pid = %d \tNr_th = %d\tSF=%x\tMAX_SIGNAL=%x\t\tName = %s\n",
                task_pid_nr(p), p->signal->nr_threads, p->signal->flags, maxSig, p->comm);
                for (i = k; i < k + 100; i++){
                    arr[i] = str[i - k];
                }
                k += 100;
                copy_to_user(buffer, arr, k); //вывод в юзер спэйс информации по id процесса
                for (i = 0; i < 10; i++){ //очищаем multarr, что позволяет при повторном вызове cat, получить вывод для всех процессов
                multarr[i] = '\0';
                }
                return length;
            }
        }  while_each_thread(g, p);
            //код ниже выполняется в случе, если значение переданной в write_mult не соответствует не одному id процессу
            for (i = 0; i < 10; i++){ //снова очищаем буффер, чтобы повторный вызов cat выводил информацию по всем процессам
                multarr[i] = '\0';
            }
        copy_to_user(buffer, "Pid is incorrect\n", 17);//Вывод в юзер спэйс инфомации о том, что нет ниодного процесса с тким id
    }
    return length;
}

////write_mult
static ssize_t write_mult(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    for (i = 0; i < 10; i++){ //предварительная очистка буффера, чтобы избавиться от мусора при повторном вызове функции
        pciarr[i] = '\0';
    }
    if(copy_from_user(multarr, buff, len)) //копируем значение переданное юзером в локальную переменную multarr
    {
        pr_err("Data Write : Err!\n");
    }
    return len;
}

////PCI_dev
static ssize_t read_pci(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
    pr_info("Start read_pci");
    if(len) {
        len=0;
    }
    else {
        len=1;
        return 0;
    }
    while ((dev2 = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev2))){
        sprintf(str,"Dev_ID= %d\t\tClass= %x\t\tBUS_MSP= %x\tBN= %x\n",
        dev2->devfn, dev2->class, dev2->bus->max_bus_speed, dev2->bus->number);
        for (i = k; i < k + 120; i++){
            arr[i] = str[i - k];
        }
        k += 120;
        if (k >= 90000) break;
    }
    if(copy_to_user(buffer, arr, k)){
    pr_err("Data Send : Err!\n");
    }
    return length;
}

////PCI_write
static ssize_t write_pci(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    for (i = 0; i < 1; i++){
        pciarr[i] = '\0';
    }
    if(copy_from_user(pciarr, buff, len) )
    {
        pr_err("Data Write : Err!\n");
    }

    return len;
}

static int __init etx_driver_init(void)
{
    arr = kmalloc(100000, GFP_KERNEL);
    str = kmalloc(120, GFP_KERNEL);

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_info("Cannot create the struct class\n");
        goto r_class;
    }

    /*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("pci_mult", NULL);

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

static void __exit etx_driver_exit(void)
{
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