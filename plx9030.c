/* 2018
 * PNPI
 */

#include "plx9030.h"
#include "plx9030c.h"


static dev_t dev;
static int major = 0;
static int minor_count = 0;
static struct cdev c_dev;
static struct class *class_dev = NULL;


MODULE_DEVICE_TABLE(pci,plx_ids_table);

static int __init init_plx9030(void){
  int retval;
  printk(PINFO "module start.\n");

  //reg_file_device("serial",&dev,&major,&c_dev,class_dev,(struct file_operations *)&s_file_operations);
  retval = pci_register_driver((struct pci_driver *)&s_pci_driver);

  return retval;
}

static void __exit exit_plx9030(void){

  
  device_destroy(class_dev,dev);
  cdev_del(&c_dev);
  class_destroy(class_dev);
  unregister_chrdev_region(dev,1);

  
  printk(PINFO "module exit.\n");
  return;
}


/* functions for processng operation of char device file */ 
static ssize_t device_file_read(struct file *f, char __user *buff, size_t count, loff_t *offset){
  ssize_t retval = 0;
  

  return retval;
}

static ssize_t device_file_write(struct file *f, const char __user *buff, size_t count, loff_t *offset){

  
  return count;
}

static long device_file_ioctl(struct file *f, unsigned int ioctl_num, unsigned long ioctl_param){
  switch(ioctl_num){
  case IOCTL_SET_ADDR:
    break;

  case IOCTL_SET_GPIO:
    break;
  }

  return 0;
}

static int device_file_release(struct inode *inode, struct file *f){
  
  return 0;
}

static int device_file_open(struct inode *inode, struct file *f){
  return 0;
}


/* register and create char file of device */
static int reg_file_device(char *prefix,dev_t *ldev,int *lmajor,
			   struct cdev *lc_dev, struct class *lclass_dev,
			   struct file_operations *sfo){
  int retval = 0;

  int lminor;
  
  char namebuffer[32];
  sprintf(namebuffer,DEVICE_FILE_NAME "_%s_%d",prefix,minor_count);

  /* proc */
  retval = alloc_chrdev_region(ldev,0,1,namebuffer);
  if(retval < 0){
    printk(PERR "Error! Failed alloc char device region :(\n");
    return retval;
  }
  lminor = MINOR(*ldev);
  lmajor = MAJOR(*ldev);
  printk(PINFO "alloc char file region, major = %d, minor = %d\n",lmajor,lminor);
  if(minor_count != lminor) printk(PWARN "minor count not eq current minor!\n"); 

  /* sys */
  lclass_dev = class_create(THIS_MODULE,namebuffer);
  if(lclass_dev == NULL){
    printk(PERR "Error! Failed create class for device! :(\n");
    unregister_chrdev_region(*ldev,1);
    return -1;
  }

  /* dev */
  if(device_create(lclass_dev, NULL, *ldev, NULL, namebuffer) == NULL){
    printk(PERR "Error! Can't create device :(\n");
    unregister_chrdev_region(*ldev,1);
    class_destroy(lclass_dev);
    return -1;
  }

  cdev_init(lc_dev,sfo);
  retval = cdev_add(lc_dev,*ldev,1);
  if(retval < 0){
    printk(PERR "Error! Fail cdev add :(\n");
    unregister_chrdev_region(*ldev,1);
    class_destroy(lclass_dev);
    return -1;
  }


  printk(PINFO "device char file: %s\n",namebuffer);
  
  minor_count++;
  return 0;
}


module_init(init_plx9030);
module_exit(exit_plx9030);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tre3k pscyrill@mail.ru");
MODULE_VERSION("1.0");

/* EOF */

