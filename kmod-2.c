//#include <linux/init.h>
//#include <linux/module.h>
//#include <linux/kernel.h>
//
//#include <linux/debugfs.h>
//
//#include <linux/pid.h>
//#include <linux/sched.h>
////
//#include <linux/netdevice.h>
//#include <linux/device.h>
//#include <linux/pci.h>
////
//
//
//
//MODULE_LICENSE("Dual BSD/GPL");
//MODULE_DESCRIPTION("Stab linux module for labs");
//MODULE_VERSION("1.0");
//
//struct dentry *kmod_root;
//struct dentry *pid1_task_struct;
//struct debugfs_blob_wrapper pid1_wrapper;
//struct task_struct *ts1;
////
//struct net_device *dev;
//
//
//struct pci_dev *dev2;
//
////static int VENDOR_ID = 8086;
////static int DEVICE_ID = 4227;
//
////
//
//static int __init kmod_init(void) {
//    printk(KERN_INFO "kmod: module loading.\n");
//	kmod_root = debugfs_create_dir("kmod", NULL);
////
//	read_lock(&dev_base_lock);
//
//	dev = first_net_device(&init_net);
//	while (dev) {
//	    printk(KERN_INFO "found [%s]\n", dev->name);
//	    dev = next_net_device(dev);
//	}
//
//	read_unlock(&dev_base_lock);
////
//
//	while (dev2 = pci_get_device(PCI_ANY_ID, PCI_ANY_ID, dev2)){
////	        configure_device(dev2);
//	        printk(KERN_INFO "pci found [%d]\n", dev2->device);
//	        }
//
//    ts1 = get_pid_task(find_get_pid(1), PIDTYPE_PID);
//    pid1_wrapper.data = (void*)ts1;
//    pid1_wrapper.size = sizeof(struct task_struct);
//
//    pid1_task_struct = debugfs_create_blob("init", 0644, kmod_root, &pid1_wrapper);
//    if (pid1_task_struct == NULL) return 1;
//
//
//    printk(KERN_INFO "kmod: data size is %ld.\n", pid1_wrapper.size);
//
//    return 0;
//}
//
//
//
//static void __exit kmod_exit(void) {
//    debugfs_remove_recursive(kmod_root);
//    printk(KERN_INFO "kmod: module unloaded\n");
//}
//module_init(kmod_init);
//module_exit(kmod_exit);
