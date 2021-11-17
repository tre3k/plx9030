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

bool Plx9030PSD::is_runing = false;
bool Plx9030PSD::is_mem_end = false;
bool Plx9030PSD::is_half_mem_end = false;
unsigned int Plx9030PSD::mem_count = 0;

Plx9030PSD::Plx9030PSD(std::string device){
	plx = new PLX9030::plx9030(device);
	status = plx->getStatus();
	if(status != 0){
		delete plx;
	}
}

Plx9030PSD::~Plx9030PSD(){
	delete plx;
}

void Plx9030PSD::init(){
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
	status = plx->getStatus();
}

void Plx9030PSD::start(){
	plx->write8(PLX9030::CS0, 0, 0x80);
	plx->write8(PLX9030::CS0, 2, 0x40);
	plx->write_hw16(PLX9030::CS3, 31, 0xf001);
	plx->write8(PLX9030::CS0, 2, 0x60);
	plx->write_hw16(PLX9030::CS3, 31, 0xfc03);
	status = plx->getStatus();
}

void Plx9030PSD::stop(){
	plx->write_hw16(PLX9030::CS3,  31,  0xf803);
	status = plx->getStatus();
}

raw_data Plx9030PSD::readMem(){
	raw_data retval;
	uint16_t tmp;

	tmp = (uint16_t)(plx->read_hw16(PLX9030::CS3, 256) & 0xffff);
	status = plx->getStatus();

	retval.raw = tmp;
	retval.code = (tmp & 0xe000) >> 13;
	retval.value = tmp & 0x1fff;

	return retval;
}

std::vector<raw_data> Plx9030PSD::getAllMemory(void){
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

std::vector<four_value> Plx9030PSD::convertToFourValue(
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

		for(int j = 0; j < 4; j++) if(value[j] < 0 || value[j] > 4800)
						   four.correct = false;
		retval.push_back(four);
	}

	return retval;
}


unsigned char Plx9030PSD::checkMem() {
	unsigned char byte = 0x00;
	byte = plx->read8(PLX9030::CS0, 3);
	status = plx->getStatus();
	byte &= 0x0f;
	return byte;
}

int Plx9030PSD::fromCode(int code) {
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

int Plx9030PSD::getStatus() {
	return status;
}


/* COUNTER */
Plx9030Counter::Plx9030Counter(std::string chrdev) {
	_chrdev = chrdev;
	plx = new PLX9030::plx9030(chrdev);
	status = plx->getStatus();
	if(status != 0){
		delete plx;
	}
	/* read 4 ofsset addr from cs0 */
	char test = (char)(
		plx->read8(PLX9030::CS0, TIMER_STATUS_REGISTER) & 0x7f
		);
	if(test != 0x55){
		//this is not counter device!
		status = STATUS_NOT_COUNTER_DEVICE;
		return;
	}

	plx->write8(PLX9030::CS0, CONTROL_REGISTER, 0x00);
	plx->write8(PLX9030::CS0, TIMER_REGISTER, 0x00);
}

Plx9030Counter::~Plx9030Counter() {
	if(plx != nullptr){
		delete plx;
	}
}

int Plx9030Counter::getStatus() {
	return status;
}

uint32_t Plx9030Counter::readValue(short channel) {
	if(status != 0) return 0xffffffff;
	return plx->read32(PLX9030::CS0, 16 + channel * 4);
}

void Plx9030Counter::writeValue(uint32_t value, short channel) {
	if(status != 0) return;
	plx->write32(PLX9030::CS0, 16 + channel*4, value);
}

void Plx9030Counter::setTimeInterval(unsigned int time) {
	uint32_t buff_reg = 0xffffffff - time;
	plx->write32(PLX9030::CS0, TIMER_BUFFER_REGISTER, buff_reg);
}

void Plx9030Counter::startTimer() {
	plx->write8(PLX9030::CS0, CONTROL_REGISTER, 0x00);
	/* stop and reset timer */
	char control_timer = plx->read8(PLX9030::CS0, TIMER_REGISTER);
	control_timer |= (TR_GATE_SEL | TR_CLR_GATE);
	plx->write8(PLX9030::CS0, TIMER_REGISTER, control_timer);

	/* load value from preset value register (buffer register) to
	   timer value register (timer read register) */
	control_timer = plx->read8(PLX9030::CS0, TIMER_REGISTER);
	control_timer |= TR_LOAD_GATE;
	control_timer &=~ TR_CLR_GATE;
	plx->write8(PLX9030::CS0, TIMER_REGISTER, control_timer);

	/* write 0b1111 to D0..D3 in CONTROL REGISTER
	   (operation "not" for all inputs) */
	plx->write8(PLX9030::CS0, CONTROL_REGISTER, 0x0f);

	char control_register = plx->read8(PLX9030::CS0, CONTROL_REGISTER);
	control_register |= (CR_RGCLR | CR_END_CRM);
	plx->write8(PLX9030::CS0, CONTROL_REGISTER, control_register);

	/* start timer */
	control_timer = TR_GATE_SEL | TR_EN_GATE;
	plx->write8(PLX9030::CS0, TIMER_REGISTER, control_timer);
}

void Plx9030Counter::stopTimer() {
	char control_timer = plx->read8(PLX9030::CS0, TIMER_REGISTER);
	control_timer &=~ (TR_EN_GATE | TR_CLR_GATE);
	plx->write8(PLX9030::CS0, TIMER_REGISTER, control_timer);

	char control_register = plx->read8(PLX9030::CS0, CONTROL_REGISTER);
	control_register |= CR_END_CRM;
	plx->write8(PLX9030::CS0, CONTROL_REGISTER, control_register);
}

bool Plx9030Counter::isFinish() {
	/* test GATE_STATE bit */
        return (plx->read8(PLX9030::CS0, TIMER_STATUS_REGISTER) & 0x80) ?
		true : false;
}

void Plx9030Counter::resetCounter() {
	char control_register = plx->read8(PLX9030::CS0,
					   CONTROL_REGISTER);
	control_register &= ~CR_RGCLR;  // clear counters
	plx->write8(PLX9030::CS0, CONTROL_REGISTER, control_register);
}

uint32_t Plx9030Counter::readTimer(){
	uint32_t retval = plx->read32(PLX9030::CS0, TIMER_VALUE_REGISTER);
	return retval;
}

void Plx9030Counter::writeTimer(uint32_t value) {
	plx->write32(PLX9030::CS0, TIMER_BUFFER_REGISTER, value);
}

void Plx9030Counter::resetTimer() {
	char control_timer = plx->read8(PLX9030::CS0, TIMER_REGISTER);
	control_timer |= TR_CLR_GATE;
	plx->write8(PLX9030::CS0, TIMER_REGISTER, control_timer);
}
