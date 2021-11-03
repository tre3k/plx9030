/*
 *  Copyright (c) 2018-2021 NRC KI PNPI, Gatchina, LO, 188300 Russia
 *
 *  This file is part of kernel module plx9030.
 *
 *  plx9030 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  plx9030 is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with plx9030.  If not, see <https://www.gnu.org/licenses/>.
 *
 *     Authors:  2018      Mikhail Golubev <mgolubev86@gmail.com>,
 *               2018-2021 Kirill Pshenichnyi <pshcyrill@mail.ru>
 */


#include "plx9030.h"

static int __init init_plx9030(void){
	int retval = 0;
	printk(KERN_INFO MODULE_NAME ": module init.\n");

	retval = alloc_chrdev_region(&tdev, 0, MAX_DEVICES, "plx9030_dev");
	gMajor = MAJOR(tdev);
	printk(KERN_INFO MODULE_NAME ": char device major: %d\n", gMajor);
	retval = pci_register_driver((struct pci_driver *) &s_pci_driver);

	return retval;
}

static void __exit exit_plx9030(void){
	gCount=0;
	pci_unregister_driver(&s_pci_driver);
	unregister_chrdev_region(tdev, MAX_DEVICES);
	printk(KERN_INFO MODULE_NAME ": module exit.\n");
	return;
}


/* PCI DEVICES */
static int plx_device_probe(struct pci_dev *pdev,
			    const struct pci_device_id *ent) {
	int retval = 0;

	u16 vendorID,deviceID,revision;
	u32 serialNM = 0;
	char serialstring[9];

	int bar;
	unsigned long addr_start,addr_end, len;

	unsigned long cs0_port;
	unsigned long cs1_port;
	void __iomem *cs2_mem;
	void __iomem *cs3_mem;

	retval = pci_enable_device(pdev);
	if(retval < 0){
		printk(KERN_ERR MODULE_NAME
		       ": ERROR! Can't enable pci device.\n");
		return retval;
	}

	printk(KERN_INFO MODULE_NAME
	       ": ************ Device number %d ************\n",
	       gCount);

	retval = pci_request_regions(pdev,MODULE_NAME);
	pci_enable_device_mem(pdev);
	pci_set_master(pdev);

	pci_read_config_word(pdev,PCI_VENDOR_ID,&vendorID);
	pci_read_config_word(pdev,PCI_DEVICE_ID,&deviceID);
	pci_read_config_word(pdev,PCI_REVISION_ID,&revision);

	pci_write_config_dword(pdev,VPD_ADDR,ADDR_SN_IN_EEPROP & 0x7fffffff);
	mdelay(100);
	pci_read_config_dword(pdev,VPD_DATA,&serialNM);

	sprintf(serialstring,"%.8x\x00",serialNM&0xffffffff);

	printk(KERN_INFO MODULE_NAME
	       ": device found, vendorID: 0x%.4x, deviceID: "
	       "0x%.4x, revision: 0x%.4x. Seral number: 0x%.8x (%d)\n",
	       vendorID, deviceID, revision, serialNM&0xffffffff, serialNM);


	for(bar=0; bar<6; bar++){
		addr_start = pci_resource_start(pdev,bar);
		addr_end = pci_resource_end(pdev,bar);
		len = pci_resource_len(pdev,bar);

		printk(KERN_INFO MODULE_NAME ": "
		       "base addr%d: start 0x%x, end: 0x%x, len %d\n",
		       bar,addr_start,addr_end,len);

	}

	devs[gCount].cs0_port = pci_resource_start(pdev,2);
	devs[gCount].cs1_port = pci_resource_start(pdev,3);
	devs[gCount].cs2_mem = pci_ioremap_bar(pdev,4);
	devs[gCount].cs3_mem = pci_ioremap_bar(pdev,5);

	devs[gCount].lencs0 = pci_resource_len(pdev,2);
	devs[gCount].lencs1 = pci_resource_len(pdev,3);
	devs[gCount].lencs2 = pci_resource_len(pdev,4);
	devs[gCount].lencs3 = pci_resource_len(pdev,5);


	devs[gCount].number = gCount;

	retval = init_chrdev();
	gCount++;
	printk(KERN_INFO MODULE_NAME ": \n");
	return retval;
}


static void plx_device_remove(struct pci_dev *pdev){
	pci_release_regions(pdev);
	pci_disable_device(pdev);
	remove_chrdev();
	printk(KERN_INFO MODULE_NAME
	       ": remove device: %s%d\n",
	       DEVICE_FILE_NAME, gCount);
	gCount++;
	return;
}

/* CHAR DEVICES */
static int init_chrdev(void){
	int retval = 0;

	char device_name[128];
	char device_name_class[128];

	sprintf(device_name,"plxdev%d",gCount);
	sprintf(device_name_class,"plxdev%d",gCount);

	devs[gCount].dev_class = class_create(THIS_MODULE, device_name_class);
	if(devs[gCount].dev_class == NULL) goto err_class;
	devs[gCount].mkdev = MKDEV(gMajor, gCount);
	if(device_create(devs[gCount].dev_class,
			 NULL,
			 devs[gCount].mkdev,
			 NULL,
			 device_name) == NULL) goto err_device;
	cdev_init(&devs[gCount].cdev, &s_file_operations);
	devs[gCount].cdev.owner = THIS_MODULE;
	retval = cdev_add(&devs[gCount].cdev, devs[gCount].mkdev, 1);
	if(retval < 0) goto err_cdevadd;

	printk(KERN_INFO MODULE_NAME ": register device: %s\n", device_name);

	return retval;

err_cdevadd:
	device_destroy(devs[gCount].dev_class,tdev);
err_device:
	class_destroy(devs[gCount].dev_class);
	printk(KERN_ERR MODULE_NAME ": Error create device!\n");
err_class:
	printk(KERN_ERR MODULE_NAME ": Error create class for char device!\n");
	return -1;
}

static void remove_chrdev(void){
	cdev_del(&(devs[gCount].cdev));
	device_destroy(devs[gCount].dev_class, devs[gCount].mkdev);
	class_destroy(devs[gCount].dev_class);
	return;
}


static int device_file_open(struct inode *inode, struct file *f){
	int retval;
	struct my_chrdevice_data *mydata;

	mydata = container_of(inode->i_cdev, struct my_chrdevice_data, cdev);
	f->private_data = mydata;

	return retval;
}

static long device_file_ioctl(struct file *f,
			      unsigned int ioctl_num,
			      unsigned long ioctl_param) {
	long retval = 0;
	struct my_chrdevice_data *mydata;
	mydata = (struct my_chrdevice_data *) f->private_data;

	switch(ioctl_num) {
	case CS0_SET_ADDR:
		if(ioctl_param >= mydata->lencs0){
			printk(KERN_ERR MODULE_NAME ": ofsset >= len CS0!\n");
			return retval;
		}
	case CS1_SET_ADDR:
		if(ioctl_param >= mydata->lencs1){
			printk(KERN_ERR MODULE_NAME ": ofsset >= len CS1!\n");
			return retval;
		}
	case CS2_SET_ADDR:
		if(ioctl_param >= mydata->lencs2){
			printk(KERN_ERR MODULE_NAME ": ofsset >= len CS2!\n");
			return retval;
		}
	case CS3_SET_ADDR:
		if(ioctl_param >= mydata->lencs3){
			printk(KERN_ERR MODULE_NAME ": ofsset >= len CS3!\n");
			return retval;
		}
		break;
	}

	mydata->cs_flag = ioctl_num;
	mydata->offset = ioctl_param;

#ifdef DEBUG_MODE
	printk(KERN_INFO MODULE_NAME
	       "ioctl: num: %d, param: %d\n",
	       ioctl_num, ioctl_param);
	printk(KERN_INFO MODULE_NAME
	       "ioctl: cs: %d, offset: %d\n",
	       mydata->cs_flag, mydata->offset);
#endif

	return retval;
}


static ssize_t device_file_read(struct file *f,
				char __user *buff,
				size_t count,
				loff_t *offset) {
	struct my_chrdevice_data *mydata;
	mydata = (struct my_chrdevice_data *) f->private_data;

#ifdef DEBUG_MODE
	printk(KERN_INFO MODULE_NAME
	       "read: cs: %d, offset: %d\n",
	       mydata->cs_flag,mydata->offset);
#endif
	unsigned char iobuff = 0x00;
	unsigned short int iobuff16 = 0x0000;


	switch(mydata->cs_flag) {
	case CS0_SET_ADDR:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS0, offset: %d\n",
		       mydata->offset);
#endif

		iobuff = inb(mydata->cs0_port+mydata->offset);

#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.2x\n",
		       iobuff & 0xff);
#endif
		copy_to_user(buff, (const void *)&iobuff, 1);
		break;


	case CS1_SET_ADDR:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS1, offset: %d\n",
		       mydata->offset);
#endif
		iobuff = inb(mydata->cs1_port+mydata->offset);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.2x\n",
		       iobuff & 0xff);
#endif
		copy_to_user(buff, (const void *)&iobuff, 1);
		break;

	case CS2_SET_ADDR:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS2, offset: %d\n",
		       mydata->offset);
#endif
		iobuff = (unsigned char) ioread8(
			mydata->cs2_mem+mydata->offset
			);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.2x\n",
		       iobuff & 0xff);
#endif
		copy_to_user(buff, (const void *)&iobuff, 1);
		break;

	case CS3_SET_ADDR:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS3, offset: %d\n",
		       mydata->offset);
#endif
		iobuff = (unsigned char) ioread8(
			mydata->cs3_mem+mydata->offset
			);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.2x\n",
		       iobuff & 0xff);
#endif
		copy_to_user(buff, (const void *)&iobuff, 1);
		break;

	case CS2_SET_ADDR16:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS2 for 16bit, offset: %d\n",
		       mydata->offset);
#endif
		iobuff16 = (unsigned short int) ioread16(
			mydata->cs2_mem+mydata->offset*2
			);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.4x\n",
		       iobuff16 & 0xffff);
#endif
		copy_to_user(buff, (const void *)&iobuff16, 2);
		break;


	case CS3_SET_ADDR16:
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS3 for 16bit, offset: %d\n",
		       mydata->offset);
#endif
		iobuff16 = (unsigned short int) ioread16(
			mydata->cs3_mem+mydata->offset*2
			);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": read data: 0x%.4x\n",
		       iobuff16 & 0xffff);
#endif
		copy_to_user(buff,(const void *)&iobuff16,2);
		break;
	}

	return count;
}

static ssize_t device_file_write(struct file *f,
				 const char __user *buff,
				 size_t count, loff_t *offset){
	struct my_chrdevice_data *mydata;
	mydata = (struct my_chrdevice_data *) f->private_data;
#ifdef DEBUG_MODE
	printk(KERN_INFO MODULE_NAME
	       ": write to device: plxdev%d\n",
	       mydata->number);
#endif

	unsigned char iobuff = 0x00;
	unsigned short int iobuff16 = 0x0000;

	copy_from_user(&iobuff,buff,1);

	switch(mydata->cs_flag){
	case CS0_SET_ADDR:
		copy_from_user(&iobuff,buff,1);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS0, write data: 0x%.2x\n",
		       iobuff);
#endif
		outb(iobuff, mydata->cs0_port+mydata->offset);
		break;

	case CS1_SET_ADDR:
		copy_from_user(&iobuff, buff, 1);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS1, write data: 0x%.2x\n",
		       iobuff);
#endif
		outb(iobuff, mydata->cs1_port+mydata->offset);
		break;

	case CS2_SET_ADDR:
		copy_from_user(&iobuff, buff, 1);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS2, write data: 0x%.2x\n",
		       iobuff);
#endif
		iowrite8(iobuff, mydata->cs2_mem+mydata->offset);
		break;

	case CS3_SET_ADDR:
		copy_from_user(&iobuff, buff, 1);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS3, write data: 0x%.2x\n",
		       iobuff);
#endif
		iowrite8(iobuff, mydata->cs3_mem+mydata->offset);
		break;

	case CS2_SET_ADDR16:
		copy_from_user(&iobuff16, buff, 2);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS2 for 16 bit, write data: 0x%.4x\n",
		       iobuff16);
#endif
		iowrite16(iobuff16,
			  mydata->cs2_mem+mydata->offset*2);
		break;

	case CS3_SET_ADDR16:
		copy_from_user(&iobuff16, buff, 2);
#ifdef DEBUG_MODE
		printk(KERN_INFO MODULE_NAME
		       ": SET CS3 for 16 bit, write data: 0x%.4x\n",
		       iobuff16);
#endif
		iowrite16(iobuff16, mydata->cs3_mem+mydata->offset*2);
		break;
	}

	return count;
}

static int device_file_release(struct inode *inode, struct file *f){
	int retval = 0;
	/* reservate for next futures */
	return retval;
}


module_init(init_plx9030);
module_exit(exit_plx9030);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Pshenichnyi <pscyrill@mail.ru>");
MODULE_VERSION("3.0");
