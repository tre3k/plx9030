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


#ifndef PLX9030C_H
#define PLX9030C_H

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>

namespace PLX9030{


#define CS0_SET_ADDR 0xfacecaf0
#define CS1_SET_ADDR CS0_SET_ADDR+1
#define CS2_SET_ADDR CS0_SET_ADDR+2
#define CS3_SET_ADDR CS0_SET_ADDR+3
#define CS0_SET_ADDR16 CS0_SET_ADDR+4
#define CS1_SET_ADDR16 CS0_SET_ADDR+5
#define CS2_SET_ADDR16 CS0_SET_ADDR+6
#define CS3_SET_ADDR16 CS0_SET_ADDR+7


	const uint32_t CS0 = CS0_SET_ADDR;
	const uint32_t CS1 = CS1_SET_ADDR;
	const uint32_t CS2 = CS2_SET_ADDR;
	const uint32_t CS3 = CS3_SET_ADDR;
	const uint32_t CS0_16 = CS0_SET_ADDR16;
	const uint32_t CS1_16 = CS1_SET_ADDR16;
	const uint32_t CS2_16 = CS2_SET_ADDR16;
	const uint32_t CS3_16 = CS3_SET_ADDR16;


	enum{
		STATUS_OK,
		STATUS_FD_ERROR,
		STATUS_WRITE_ERROR,
		STATUS_READ_ERROR,
		STATUS_IOCTL_ERROR
	};

	class plx9030{

	private:
		/* this file descriptor of device */
		int fd;
		int status;

	public:
		explicit plx9030(std::string device);
		~plx9030();

		/* return status, errors, etc. */
		int getStatus(void);
		/* just return fd variable */
		int getFileDescriptor(void);

		/* Functions for write/read CS0..3 for 8,16,24 and 32 bits */
		// base - CS0, CS1, CS2, CS3
		char read8(uint32_t base, long int offset);
		void write8(uint32_t base, long int offset, char byte);

		uint16_t read16(uint32_t base, long int offset);
		void write16(uint32_t base, long int offset, uint16_t word);

		uint16_t read_hw16(uint32_t base, long int offset);
		void write_hw16(uint32_t base, long int offset, uint16_t word);

		uint32_t read24(uint32_t base, long int offset);
		void write24(uint32_t base, long int offset, uint32_t twoword);

		uint32_t read32(uint32_t base, long int offset);
		void write32(uint32_t base, long int offset, uint32_t twoword);

		/* set mask / unset mask - selective set/unset bits
		   in register by mask */
		// mask - just or operation (bit1 | bit2 | bit3)
		void setMask(uint32_t base,
			     long int offset,
			     unsigned char mask);
		void unsetMask(uint32_t base,
			       long int offset,
			       unsigned char mask);
	};
}


#endif //PLX9030C_H

/* EOF */
