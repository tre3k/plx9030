/*
 *  Copyright (c) 2019-2021 NRC KI PNPI, Gatchina, LO, 188300 Russia
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
 *     Authors:  2019-2021 Kirill Pshenichnyi <pshcyrill@mail.ru>
 */


#include <plx9030c.h>

using namespace PLX9030;

plx9030::plx9030(std::string device) {
	fd = open(device.c_str(), O_RDWR);
	if(fd < 0){
		status = STATUS_FD_ERROR;
		return;
	}
	status = STATUS_OK;
}

plx9030::~plx9030() {
	close(fd);
}

int plx9030::getStatus() {
	return status;
}

int plx9030::getFileDescriptor() {
	return fd;
}

/* Function for write/read CS0..3 */
char plx9030::read8(uint32_t base, long int offset) {
	unsigned char byte = 0x00;
	if(ioctl(fd, base, offset) < 0) status = STATUS_IOCTL_ERROR;
	if(read(fd, &byte, 1) < 0) status = STATUS_READ_ERROR;
	return byte;
}

void plx9030::write8(uint32_t base, long int offset, char byte) {
	if(ioctl(fd, base, offset) < 0) status = STATUS_IOCTL_ERROR;
	if(write(fd, &byte, 1) <0 ) status = STATUS_WRITE_ERROR;
}

uint16_t plx9030::read_hw16(uint32_t base, long int offset) {
	unsigned short int word;
	switch(base){
	case CS0:
		base = CS0_16;
		break;
	case CS1:
		base = CS1_16;
		break;
	case CS2:
		base = CS2_16;
		break;
	case CS3:
		base = CS3_16;
		break;

	}

	if(ioctl(fd, base, offset) < 0) status = STATUS_IOCTL_ERROR;
	if(read(fd, &word, 2) < 0) status = STATUS_READ_ERROR;
	return word;
}

void plx9030::write_hw16(uint32_t base, long int offset, uint16_t word) {
	switch(base) {
	case CS0:
		base = CS0_16;
		break;
	case CS1:
		base = CS1_16;
		break;
	case CS2:
		base = CS2_16;
		break;
	case CS3:
		base = CS3_16;
		break;
	}

	if(ioctl(fd, base, offset) < 0) status = STATUS_IOCTL_ERROR;
	if(write(fd, &word, 2) <0 ) status = STATUS_WRITE_ERROR;
}

uint16_t plx9030::read16(uint32_t base, long int offset) {
	uint16_t retval = 0;
	// 16 bit = 2 bytes
	for(int i=0;i<2;i++){
		retval |= ((read8(base, offset+i)&0xff) << 8*i);
	}
	return retval;
}

void plx9030::write16(uint32_t base, long int offset, uint16_t word) {
	char byte = 0;
	for(int i=0;i<2;i++){
		byte = (char)((word>>8*i)&0xff);
		write8(base, offset+i, byte);
	}
}

uint32_t plx9030::read24(uint32_t base, long int offset) {
	uint32_t retval = 0;
	// 24 bit = 3 bytes
	for(int i=0;i<3;i++){
		retval |= ((read8(base, offset+i)&0xff) << 8*i);
	}
	return retval;
}

void plx9030::write24(uint32_t base,  long int offset, uint32_t twoword) {
	char byte = 0;
	for(int i=0;i<3;i++){
		byte = (char)((twoword>>8*i)&0xff);
		write8(base, offset+i, byte);
	}
}

uint32_t plx9030::read32(uint32_t base, long int offset) {
	uint32_t retval = 0;
	for(int i=0;i<4;i++){
		retval |= ((read8(base, offset+i)&0xff) << 8*i);
	}
	return retval;
}

void plx9030::write32(uint32_t base, long int offset, uint32_t twoword) {
	char byte = 0;
	for(int i=0;i<4;i++){
		byte = (char)((twoword>>8*i)&0xff);
		write8(base, offset+i, byte);
	}
}

void plx9030::setMask(uint32_t base, long int offset, unsigned char mask) {
	char byte = read8(base, offset);
	byte |= mask;
	write8(base, offset, byte);
}

void plx9030::unsetMask(uint32_t base, long int offset, unsigned char mask) {
	char byte = read8(base, offset);
	byte &= ~mask;
	write8(base, offset, byte);
}
