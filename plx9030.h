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


#define MODULE_NAME "plx9030"
#define DEVICE_FILE_NAME "plx"

#define ADDR_SN_IN_EEPROP 0x00a00003
#define VPD_DATA 0x50
#define VPD_ADDR 0x4c


#define PMOD "plx9010: "
#define PINFO KERN_INFO PMOD
#define PERR KERN_ERR PMOD
#define PALERT KERN_ALERT PMOD
#define PWARN KERN_WARNING PMOD


/* function of prototypes for device file */
static ssize_t device_file_read(struct file *f, char __user *buff, size_t count, loff_t *offset);
static ssize_t device_file_write(struct file *f, const char __user *buff, size_t count, loff_t *offset);
static long device_file_ioctl(struct file *f, unsigned int ioctl_num, unsigned long ioctl_param);
static int device_file_release(struct inode *inode, struct file *f);
static int device_file_open(struct inode *inode, struct file *f);

struct file_operations s_file_operations = {
						  .owner = THIS_MODULE,
						  .open = device_file_open,
						  .release = device_file_release,
						  .read = device_file_read,
						  .write = device_file_write,
						  .unlocked_ioctl = device_file_ioctl
};


/* function of prototypes for PCI */
static int plx_device_probe(struct pci_dev *pdev, const struct pci_device_id *ent);
static void plx_device_remove(struct pci_dev *pdev);

static struct pci_device_id plx_ids_table[] =  {
					       { PCI_DEVICE(0x10b5,0x90f1) },
					       { PCI_DEVICE(0x8086,0x9d21) }, //For test!
					       {0,}
};


static struct pci_driver s_pci_driver = {
					      .name = MODULE_NAME,
					      .id_table = plx_ids_table,
					      .probe    = plx_device_probe,
					      .remove   = plx_device_remove
};



/* other prototypes */
static int reg_file_device(char *prefix,
			   dev_t *ldev,
			   int *lmajor,
			   struct cdev *lc_dev,
			   struct class *lclass_dev,
			   struct file_operations *
			   );

#endif //PLX9030_H

/* EOF */

