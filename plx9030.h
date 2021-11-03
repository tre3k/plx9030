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


#ifndef PLX9030_H
#define PLX9030_H

#define DEBUG_MODE

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/pci.h>
#include <linux/delay.h>

#define MODULE_NAME "plx9030"
#define DEVICE_FILE_NAME "plxdev"
#define MAX_DEVICES 8

#define ADDR_SN_IN_EEPROP 0x00a00003
#define VPD_DATA 0x50
#define VPD_ADDR 0x4c

#define CS0_SET_ADDR 0xfacecaf0
#define CS1_SET_ADDR CS0_SET_ADDR+1
#define CS2_SET_ADDR CS0_SET_ADDR+2
#define CS3_SET_ADDR CS0_SET_ADDR+3
#define CS0_SET_ADDR16 CS0_SET_ADDR+4
#define CS1_SET_ADDR16 CS0_SET_ADDR+5
#define CS2_SET_ADDR16 CS0_SET_ADDR+6
#define CS3_SET_ADDR16 CS0_SET_ADDR+7


static unsigned int gCount = 0;
static dev_t tdev;
static unsigned int gMajor;


/* function of prototypes for device file */
static ssize_t device_file_read(struct file *f,
				char __user *buff,
				size_t count,
				loff_t *offset);
static ssize_t device_file_write(struct file *f,
				 const char __user *buff,
				 size_t count,
				 loff_t *offset);
static long device_file_ioctl(struct file *f,
			      unsigned int ioctl_num,
			      unsigned long ioctl_param);
static int device_file_release(struct inode *inode,
			       struct file *f);
static int device_file_open(struct inode *inode,
			    struct file *f);

struct file_operations s_file_operations = {
	.owner = THIS_MODULE,
	.open = device_file_open,
	.release = device_file_release,
	.read = device_file_read,
	.write = device_file_write,
	.unlocked_ioctl = device_file_ioctl
};


/* function of prototypes for PCI */
static int plx_device_probe(struct pci_dev *pdev,
			    const struct pci_device_id *ent);
static void plx_device_remove(struct pci_dev *pdev);

static struct pci_device_id plx_ids_table[] =  {
	{PCI_DEVICE(0x10b5,0x90f1)},
	{0,}
};


static struct pci_driver s_pci_driver = {
	.name = MODULE_NAME,
	.id_table = plx_ids_table,
	.probe    = plx_device_probe,
	.remove   = plx_device_remove
};

struct my_chrdevice_data{
	struct cdev cdev;
	struct class *dev_class;

	dev_t mkdev;

	unsigned long cs0_port;
	unsigned long cs1_port;
	void __iomem *cs2_mem;
	void __iomem *cs3_mem;

	long lencs0, lencs1, lencs2, lencs3;

	unsigned int major;
	unsigned int number;

	int cs_flag;
	long offset;
};

struct my_chrdevice_data devs[MAX_DEVICES];

static int init_chrdev(void);
static void remove_chrdev(void);


#endif
