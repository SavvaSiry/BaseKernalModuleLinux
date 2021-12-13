***************************************************************************//**
*  \file       driver.c
*
*  \details    OS Lab2 (procfs: dm_dirty_log_type, device)
*
*  \author     KirillShakhov
*
*  \Tested with Linux Kernel 5.8.10-custom
*
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>                 //kmalloc()
#include <linux/uaccess.h>              //copy_to/from_user()
#include <linux/proc_fs.h>
//
#include <linux/dm-dirty-log.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/device/bus.h>
#include <linux/device/driver.h>
#include <linux/mutex.h>
#include <asm/atomic.h>
#include <linux/pm.h>
#include <linux/pci.h>
#include <linux/device.h>
//
#include <linux/types.h>
#include <linux/string.h>
#include <linux/fs.h>

static int len = 1;

static struct proc_dir_entry *parent;

struct dm_dirty_log_type *dmDirtyLogType;
struct pci_dev *dev2;

char etx_array[10]="null";
char str2[10]="null";
char str3[10]="null";


struct dm_dirty_log_type *myget_ret_log_type(const char *type_name);

/***************** Enum to Str Functions *******************/
char * module_state_to_str(enum module_state moduleState)
{
    switch(moduleState){
        case MODULE_STATE_LIVE: return "MODULE_STATE_LIVE";
        case MODULE_STATE_COMING: return "MODULE_STATE_COMING";
        case MODULE_STATE_GOING: return "MODULE_STATE_GOING";
        case MODULE_STATE_UNFORMED: return "MODULE_STATE_UNFORMED";
    }
    return "null";
}
char * rpm_status_to_str(enum rpm_status moduleState)
{
    switch(moduleState){
        case RPM_ACTIVE: return "RPM_ACTIVE";
        case RPM_RESUMING: return "RPM_RESUMING";
        case RPM_SUSPENDED: return "RPM_SUSPENDED";
        case RPM_SUSPENDING: return "RPM_SUSPENDING";
    }
    return "null";
}


/***************** Procfs Functions *******************/
static ssize_t  show_dm_dirty_log_type(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  show_device(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t *off);
/***************** fops Functions *******************/
static struct proc_ops ddlt_fops = { .proc_read = show_dm_dirty_log_type };
static struct proc_ops device_fops = {
        .proc_read = show_device,
        .proc_write = write_proc,
};



/***************** Buffer Functions *******************/
static char *arr;
static char *str;
int k = 0;
int i;
static void clean_line(void){
    for (i = k; i < k + 60; i++){
        arr[i] = '\0';
    }
    for (i = 0; i < 60; i++){
        str[i] = '\0';
    }
}
static void clean_buffer(void){
    for (i = 0; i < 100000; i++){
        arr[i] = '\0';
    }
}
static void go_to_new_line(void){
    int ll = 0;
    for (i = k; i < k + 60; i++){
        arr[i] = str[i - k];
        ll++;
    }
    k += ll;
}

/***************** Read Functions *******************/
static ssize_t show_dm_dirty_log_type(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
if (len) {
len=0;
}
else {
len=1;
return 0;
}

clean_buffer();
//get ddlt
dmDirtyLogType = myget_ret_log_type("core");


clean_line();
sprintf(str, "///////////////////\n");
go_to_new_line();

clean_line();
sprintf(str, "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
go_to_new_line();

clean_line();
sprintf(str, "module name:%s\n", dmDirtyLogType->module->name);
go_to_new_line();

clean_line();
sprintf(str, "module state:%s\n", module_state_to_str(dmDirtyLogType->module->state));
go_to_new_line();

clean_line();
sprintf(str, "module version:%s\n", dmDirtyLogType->module->version);
go_to_new_line();

clean_line();
sprintf(str, "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
go_to_new_line();

clean_line();
sprintf(str, "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);
go_to_new_line();
///////////////////////////////////////////////
//get ddlt
dmDirtyLogType = myget_ret_log_type("disk");

clean_line();
sprintf(str, "///////////////////\n");
go_to_new_line();

clean_line();
sprintf(str, "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
go_to_new_line();

clean_line();
sprintf(str, "module name:%s\n", dmDirtyLogType->module->name);
go_to_new_line();

clean_line();
sprintf(str, "module state:%s\n", module_state_to_str(dmDirtyLogType->module->state));
go_to_new_line();

clean_line();
sprintf(str, "module version:%s\n", dmDirtyLogType->module->version);
go_to_new_line();

clean_line();
sprintf(str, "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
go_to_new_line();

clean_line();
sprintf(str, "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);
go_to_new_line();
//////////////////////////////////////////////////////////////////
//get ddlt
dmDirtyLogType = myget_ret_log_type("userspace");

clean_line();
sprintf(str, "///////////////////\n");
go_to_new_line();

clean_line();
sprintf(str, "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
go_to_new_line();

clean_line();
sprintf(str, "module name:%s\n", dmDirtyLogType->module->name);
go_to_new_line();

clean_line();
sprintf(str, "module state:%s\n", module_state_to_str(dmDirtyLogType->module->state));
go_to_new_line();

clean_line();
sprintf(str, "module version:%s\n", dmDirtyLogType->module->version);
go_to_new_line();

clean_line();
sprintf(str, "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
go_to_new_line();

clean_line();
sprintf(str, "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);
go_to_new_line();



//    printk(KERN_INFO "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
//    printk(KERN_INFO "module name:%s\n", dmDirtyLogType->module->name);
//    enum module_state ms = dmDirtyLogType->module->state;
//    char *state = module_state_to_str(ms);
//    printk(KERN_INFO "module state:%s\n", state);
//    printk(KERN_INFO "module version:%s\n", dmDirtyLogType->module->version);
//    printk(KERN_INFO "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
//    printk(KERN_INFO "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);

//    dmDirtyLogType = myget_ret_log_type("disk");
//    printk(KERN_INFO "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
//    printk(KERN_INFO "module name:%s\n", dmDirtyLogType->module->name);
//    enum module_state ms2 = dmDirtyLogType->module->state;
//    char *state2 = module_state_to_str(ms2);
//    printk(KERN_INFO "module state:%s\n", state2);
//    printk(KERN_INFO "module version:%s\n", dmDirtyLogType->module->version);
//    printk(KERN_INFO "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
//    printk(KERN_INFO "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);

//    dmDirtyLogType = myget_ret_log_type("userspace");
//    printk(KERN_INFO "dm_dirty_log_type name:%s\n", dmDirtyLogType->name);
//    printk(KERN_INFO "module name:%s\n", dmDirtyLogType->module->name);
//    enum module_state ms3 = dmDirtyLogType->module->state;
//    char *state3 = module_state_to_str(ms3);
//    printk(KERN_INFO "module state:%s\n", state3);
//    printk(KERN_INFO "module version:%s\n", dmDirtyLogType->module->version);
//    printk(KERN_INFO "module srcversion:%s\n", dmDirtyLogType->module->srcversion);
//    printk(KERN_INFO "module num kernal param:%d\n", dmDirtyLogType->module->num_kp);


if(copy_to_user(buffer, arr, k)){
pr_err("Data Send : Err!\n");
}
return length;
}

/*
** This function will be called when we write the procfs file
*/
static ssize_t write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    pr_info("proc file wrote.....\n");
    for (i = 0; i < 10; i++){
        etx_array[i] = '\0';
    }
    if( copy_from_user(etx_array,buff,len) )
    {
        pr_err("Data Write : Err!\n");
    }

    return len;
}

static ssize_t show_device(struct file *filp, char __user *buffer, size_t length, loff_t * offset){
pr_info("Start read_proc");
if(len) {
len=0;
}
else {
len=1;
return 0;
}

if(strcmp (etx_array, str2)==0){
clean_buffer();

//pci_dev
for_each_pci_dev(dev2){
        clean_line();
        sprintf(str, "///////////////////\n");
        go_to_new_line();

        clean_line();
        sprintf(str, "pci found [%d]\n", (dev2->device));
        go_to_new_line();

        clean_line();
        sprintf(str, "init name:%s\n", (dev2->dev.init_name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(parent) init name:%s\n", (dev2->dev.parent->init_name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(kobject) name:%s\n", (dev2->dev.kobj.name));
        go_to_new_line();

        clean_line();
        sprintf(str, "    parent name:%s\n", (dev2->dev.kobj.parent->name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(type) name:%s\n", (dev2->dev.type->name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(bus) name:%s\n", (dev2->dev.bus->name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(bus) dev_name:%s\n", (dev2->dev.bus->dev_name));
        go_to_new_line();

        clean_line();
        sprintf(str, "(mutex) owner:%ld\n", atomic_long_read(&dev2->dev.mutex.owner));
        go_to_new_line();

        clean_line();
        sprintf(str, "(power) active_time:%llu\n", dev2->dev.power.active_time);
        go_to_new_line();

        clean_line();
        sprintf(str, "(power) runtime_status:%s\n", rpm_status_to_str(dev2->dev.power.runtime_status));
        go_to_new_line();

        clean_line();
        sprintf(str, "(power) usage_count:%d\n", atomic_read(&dev2->dev.power.usage_count));
        go_to_new_line();


        //        printk(KERN_INFO "pci found device:%d\n", dev2->device);
        //        printk(KERN_INFO "init name:%s\n", dev2->dev.init_name);
        //        printk(KERN_INFO "(parent) init name:%s\n", dev2->dev.parent->init_name);
        //        printk(KERN_INFO "(kobject) name:%s\n", dev2->dev.kobj.name);
        //        printk(KERN_INFO "      parent name:%s\n", dev2->dev.kobj.parent->name);
        //        printk(KERN_INFO "(type) name:%s\n", dev2->dev.type->name);
        //        printk(KERN_INFO "(bus) name:%s\n", dev2->dev.bus->name);
        //        printk(KERN_INFO "(bus) dev_name:%s\n", dev2->dev.bus->dev_name);
        //        printk(KERN_INFO "(mutex) owner:%ld\n", atomic_long_read(&dev2->dev.mutex.owner));
        //        printk(KERN_INFO "(power) active_time:%llu\n", dev2->dev.power.active_time);
        //        printk(KERN_INFO "(power) runtime_status:%s\n", rpm_status_to_str(dev2->dev.power.runtime_status));
        //        printk(KERN_INFO "(power) usage_count:%d\n", atomic_read(&dev2->dev.power.usage_count));
        if (k >= 90000) break;
}
}
else{
clean_line();
sprintf(str, "%s", etx_array);
go_to_new_line();

clean_line();
sprintf(str, "%s\n", etx_array);
go_to_new_line();

for_each_pci_dev(dev2){
//            strcpy(dst, src);


        for (i = 0; i < 10; i++){
            str3[i] = '\0';
        }
        sprintf(str3, "%d\n", (dev2->device));

        clean_line();
        sprintf(str, "%s\n", str3);
        go_to_new_line();

        if (strcmp (etx_array, str3)==0){

            clean_line();
            sprintf(str, "pci found [%d]\n", (dev2->device));
            go_to_new_line();
            clean_line();
            sprintf(str, "init name:%s\n", (dev2->dev.init_name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(parent) init name:%s\n", (dev2->dev.parent->init_name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(kobject) name:%s\n", (dev2->dev.kobj.name));
            go_to_new_line();
            clean_line();
            sprintf(str, "    parent name:%s\n", (dev2->dev.kobj.parent->name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(type) name:%s\n", (dev2->dev.type->name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(bus) name:%s\n", (dev2->dev.bus->name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(bus) dev_name:%s\n", (dev2->dev.bus->dev_name));
            go_to_new_line();
            clean_line();
            sprintf(str, "(mutex) owner:%ld\n", atomic_long_read(&dev2->dev.mutex.owner));
            go_to_new_line();
            clean_line();
            sprintf(str, "(power) active_time:%llu\n", dev2->dev.power.active_time);
            go_to_new_line();
            clean_line();
            sprintf(str, "(power) runtime_status:%s\n", rpm_status_to_str(dev2->dev.power.runtime_status));
            go_to_new_line();
            clean_line();
            sprintf(str, "(power) usage_count:%d\n", atomic_read(&dev2->dev.power.usage_count));
            go_to_new_line();
            return length;
        }
}
}

if(copy_to_user(buffer, arr, k)){
pr_err("Data Send : Err!\n");
}
return length;
}

/***************** Driver init *******************/
static int __init etx_driver_init(void)
{
    arr = kmalloc(100000, GFP_KERNEL);
    str = kmalloc(60, GFP_KERNEL);
    /*Create proc directory. It will create a directory under "/proc" */
    parent = proc_mkdir("etx",NULL);
    if( parent == NULL )
    {
        pr_info("Error creating proc entry");
        return -1;
    }

    /*Creating Proc entry under "/proc/etx/" */
    proc_create("dm_dirty_log_type", 0666, parent, &ddlt_fops);
    proc_create("device", 0666, parent, &device_fops);

    pr_info("Device Driver Insert...Done!!!\n");
    return 0;
}

/***************** Driver exit *******************/
static void __exit etx_driver_exit(void)
{
    /* remove complete /proc/etx */
    proc_remove(parent);
    pr_info("Device Driver Remove...Done!!!\n");
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KirillShakhov <kirill.shakhov14@gmail.com>");
MODULE_DESCRIPTION("OS Lab2 (procfs: dm_dirty_log_type, device)");
MODULE_VERSION("1.8");