#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

#include <linux/debugfs.h>

#include <linux/pid.h>
#include <linux/sched.h>
//
#include <linux/netdevice.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/types.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Stab linux module for labs");
MODULE_VERSION("1.0");

struct dentry *kmod_root;
struct dentry *pid1_task_struct;
struct debugfs_blob_wrapper pid1_wrapper;
struct task_struct *ts1;
//
struct net_device *dev;

struct pci_dev *dev2;


//static int VENDOR_ID = 8086;
//static int DEVICE_ID = 4227;

//

static int __init kmod_init(void) {
    printk(KERN_INFO "kmod: module loading.\n");
	kmod_root = debugfs_create_dir("kmod", NULL);
//
//	read_lock(&dev_base_lock);
//
	dev = first_net_device(&init_net);
	while (dev) {
	    printk(KERN_INFO "111found [%s]\n", dev->name);
	    dev = next_net_device(dev);
	}
//
//	read_unlock(&dev_base_lock);
        ////ТУТ НУЖНЫЙ КОТ
//    printk("PCI_SLOT\t\tCLASS\t\tBUS_MAX_SP\tCAP\tREG\n");
//	  while (dev2 = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev2)){
//        printk("%d\t\t%d\t\t%c\t\t%d\t%i",
//               dev2->devfn, /*dev2->device,*/ dev2->class, dev2->bus->max_bus_speed, pci_pcie_cap(dev2), dev2->hdr_type);
//	      }

//    Мультипроцессы
    struct task_struct *g, *p;

    printk("pid\t\tnr_th\t\tflags\t\tss_flag\tName");
    do_each_thread(g, p) {
        printk("%d\t\t%d\t\t%x\t\t%X\t%s", task_pid_nr(p), p->signal->nr_threads, p->signal->flags, p->sas_ss_flags, p->comm);

    } while_each_thread(g, p);

    ts1 = get_pid_task(find_get_pid(1), PIDTYPE_PID);
    pid1_wrapper.data = (void*)ts1;
    pid1_wrapper.size = sizeof(struct task_struct);

    pid1_task_struct = debugfs_create_blob("init", 0644, kmod_root, &pid1_wrapper);
    if (pid1_task_struct == NULL) return 1;


    printk(KERN_INFO "kmod: data size is %ld.\n", pid1_wrapper.size);

    return 0;
}



static void __exit kmod_exit(void) {
    debugfs_remove_recursive(kmod_root);
    printk(KERN_INFO "kmod: module unloaded\n");
}
module_init(kmod_init);
module_exit(kmod_exit);
