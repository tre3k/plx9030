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

static bool device_file_registred = false;


MODULE_DEVICE_TABLE(pci,plx_ids_table);

static int __init init_plx9030(void){
  int retval;
  printk(PINFO "module start.\n");

  reg_file_device("serial",&dev,&major,&c_dev,class_dev,(struct file_operations *)&s_file_operations);
  //retval = pci_register_driver((struct pci_driver *)&s_pci_driver);

  return retval;
}

static void __exit exit_plx9030(void){

  if(device_file_registred){
    printk(PINFO "unregistred dev char file\n");
    cdev_del(&c_dev);
    device_destroy(class_dev,dev);
    class_destroy(class_dev);
    unregister_chrdev_region(dev,256);
  }
  pci_unregister_driver(&s_pci_driver);
  
  printk(PINFO "module exit.\n");
  return;
}


/* functions for proccesing pci operations */
static int plx_device_probe(struct pci_dev *pdev, const struct pci_device_id *ent){
  int retval = 0;

  u16 vendorID,deviceID,revision;
  u32 serialNM = 0;

  
  u8 vdp_flag = 0;
  char serialstring[9];

  retval = pci_enable_device(pdev);
  if(retval < 0){
    printk(PERR "Error! Can't enable pci device\n");
    return retval;
  }
  retval = pci_request_regions(pdev,MODULE_NAME);
  
  
  pci_read_config_word(pdev,PCI_VENDOR_ID,&vendorID);
  pci_read_config_word(pdev,PCI_DEVICE_ID,&deviceID);
  pci_read_config_word(pdev,PCI_REVISION_ID,&revision);
  
  /*
  pci_write_config_dword(pdev,VPD_ADDR,ADDR_SN_IN_EEPROP&0xef);

  // Need test!!!
  while(vdp_flag == 0){
    pci_read_config_byte(pdev,VPD_ADDR,&vdp_flag);
    vdp_flag &= 0x8;
  }

  pci_read_config_dword(pdev,VPD_DATA,&serialNM);
  */

  serialNM = 0x322;
  
  sprintf(serialstring,"%.8x\x00",serialNM&0xffffffff);
  
  printk(PINFO "device found, vendorID: 0x%.4x, deviceID: "
	 "0x%.4x, revision: 0x%.4x. Seral number: 0x%.8x (%d)\n",
	 vendorID,deviceID,revision,serialNM&0xffffffff,serialNM);  

   
  /*
  retval = reg_file_device(serialstring,&dev,
			   &major,&c_dev,
			   class_dev,
			   (struct file_operations *)&s_file_operations);
  */


  int bar;
  unsigned long addr_start,addr_end,len,flags;
  for(bar = 0;bar<6;bar++){
    addr_start = pci_resource_start(pdev,bar);
    addr_end = pci_resource_end(pdev,bar);
    len = pci_resource_len(pdev,bar);

    printk(PINFO "base addr%d: start 0x%x, end: 0x%x, len %d\n",
	   bar,addr_start,addr_end,len);
    printk(PINFO "flags: 0x%x\n",flags);
    
  }
  
  return retval;
}


static void plx_device_remove(struct pci_dev *pdev){
  
  pci_release_regions(pdev);
  pci_disable_device(pdev);
  printk(PINFO "pci device removed");

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
  device_file_registred = true;
  return 0;
}


module_init(init_plx9030);
module_exit(exit_plx9030);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tre3k pscyrill@mail.ru");
MODULE_VERSION("1.0");

/* EOF */

