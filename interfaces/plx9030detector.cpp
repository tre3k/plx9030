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


#include <plx9030detector.h>

using namespace PLX9030Detector;

bool plx9030Detector::is_runing = false;
bool plx9030Detector::is_mem_end = false;
bool plx9030Detector::is_half_mem_end = false;
unsigned int plx9030Detector::mem_count = 0;

plx9030Detector::plx9030Detector(std::string device){
	plx = new PLX9030::plx9030(device);
	status = plx->getStatus();
	if(status != 0){
		delete plx;
	}
}

plx9030Detector::~plx9030Detector(){
	delete plx;
}

void plx9030Detector::init(){
	plx->write8(PLX9030::CS0, 0, 0);                // RESET
	plx->write8(PLX9030::CS0, 2, 0);                // RESET TDC & FIFO
	plx->write_hw16(PLX9030::CS3, 31, 0);           // Disable start & stop
	plx->write8(PLX9030::CS0, 2, 0x40);             // RESET TDC & FIFO
	plx->write_hw16(PLX9030::CS3, 31, 3);           // Inable INIT

	usleep(300);

	plx->write_hw16(PLX9030::CS3, 1, 0x0007);
	plx->write_hw16(PLX9030::CS3, 0, 0xfc81);

	plx->write_hw16(PLX9030::CS3, 3, 0);
	plx->write_hw16(PLX9030::CS3, 2, 0);

	plx->write_hw16(PLX9030::CS3, 5, 0);
	plx->write_hw16(PLX9030::CS3, 4, 2);

	plx->write_hw16(PLX9030::CS3, 7, 0);
	plx->write_hw16(PLX9030::CS3, 6, 0);

	plx->write_hw16(PLX9030::CS3, 9, 0x600);
	plx->write_hw16(PLX9030::CS3, 8, 2);

	plx->write_hw16(PLX9030::CS3, 11, 0x0e0);
	plx->write_hw16(PLX9030::CS3, 10, 0);

	plx->write_hw16(PLX9030::CS3, 13, 0);
	plx->write_hw16(PLX9030::CS3, 12, 0);

	plx->write_hw16(PLX9030::CS3, 15, 0x014);
	plx->write_hw16(PLX9030::CS3, 14, 0x1fb4);

	plx->write_hw16(PLX9030::CS3, 23, 0x7ff);
	plx->write_hw16(PLX9030::CS3, 22, 0);

	plx->write_hw16(PLX9030::CS3, 25, 0x200);
	plx->write_hw16(PLX9030::CS3, 24, 0);

	plx->write_hw16(PLX9030::CS3, 29, 0);
	plx->write_hw16(PLX9030::CS3, 28, 0);

	plx->write_hw16(PLX9030::CS3, 9, 0x640);
	plx->write_hw16(PLX9030::CS3, 8, 0);

	usleep(100000);
}

void plx9030Detector::start(){
	plx->write8(PLX9030::CS0, 0, 0x80);
	plx->write8(PLX9030::CS0, 2, 0x40);
	plx->write_hw16(PLX9030::CS3, 31, 0xf001);
	plx->write8(PLX9030::CS0, 2, 0x60);
	plx->write_hw16(PLX9030::CS3, 31, 0xfc03);
}

void plx9030Detector::stop(){
	plx->write_hw16(PLX9030::CS3,  31,  0xf803);
}

raw_data plx9030Detector::readMem(){
	raw_data retval;
	uint16_t tmp;

	tmp = (unsigned short int)(plx->read_hw16(PLX9030::CS3, 256)&0xffff);
	retval.raw = tmp;
	retval.code = (tmp & 0xe000) >> 13;
	retval.value = tmp & 0x1fff;

	return retval;
}

std::vector<raw_data> plx9030Detector::getAllMemory(void){
	std::vector<raw_data> retval;

	raw_data mem_val;
	int count = 0;
	int watchdog = 0;
	while(1){
		if(count > 1) if(checkMem() & 0x04) break;
		mem_val = readMem();
		if(mem_val.raw != 0) retval.push_back(mem_val);
		count ++;
		// WatchDog:
		if(count > 5) if(retval.at(retval.size() - 1).code ==
				 retval.at(retval.size() - 2).code)
				      watchdog ++;
		if(watchdog > 10) break;
	}

	return retval;
}

std::vector<four_value> plx9030Detector::convertToFourValue(
	std::vector<raw_data> raw_values) {
	std::vector<four_value> retval;
	four_value four;
	int value[4] = {-1, -1, -1, -1};


	for(int i=3;i<raw_values.size();i+=4){
		four.correct = true;
		value[fromCode(raw_values.at(i - 3).code)] =
			raw_values.at(i - 3).value;
		value[fromCode(raw_values.at(i - 2).code)] =
			raw_values.at(i-2).value;
		value[fromCode(raw_values.at(i - 1).code)] =
			raw_values.at(i - 1).value;
		value[fromCode(raw_values.at(i).code)] =
			raw_values.at(i).value;

		four.y1 = value[fromCode(Y1)];
		four.x1 = value[fromCode(X1)];
		four.y2 = value[fromCode(Y2)];
		four.x2 = value[fromCode(X2)];

		for(int j=0;j<4;j++) if(value[j]<0 || value[j]>4800)
					     four.correct = false;

		retval.push_back(four);
	}

	return retval;
}

unsigned char plx9030Detector::checkMem() {
	unsigned char byte = 0x00;
	byte = plx->read8(PLX9030::CS0, 3);
	byte &= 0x0f;
	return byte;
}

int plx9030Detector::fromCode(int code) {
	switch(code){
	case X1:
		return 0;
	case X2:
		return 1;
	case Y1:
		return 2;
	case Y2:
		return 3;
	}
	return -1;
}

int plx9030Detector::getStatus() {
	return status;
}
