#ifndef PLX9030_H
#define PLX9030_H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/pci.h>

#define PMOD "plx9010: "
#define PINFO KERN_INFO PMOD
#define PERR KERN_ERR PMOD


/* function prototips for device file */
static ssize_t device_file_read(struct file *f, char __user *buff, size_t count, loff_t *offset);
static ssize_t device_file_write(struct file *f, const char __user *buff, size_t count, loff_t *offset);
static long device_file_ioctl(struct file *f, unsigned int ioctl_num, unsigned long ioctl_param);
static int device_file_release(struct inode *inode, struct file *f);
static int device_file_open(struct inode *inode, struct file *f);


static int plx_device_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void plx_device_remove(struct pci_dev *pdev);


#endif //PLX9030_H

/* EOF */

