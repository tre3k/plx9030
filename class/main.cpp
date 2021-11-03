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
 *
 *  This is example file for use PNPI neutron detector
 */


#include <iostream>
#include "plx9030c.h"
#include "plx9030detector.h"

int main(int argc,char **argv){
	PLX9030Detector::plx9030Detector *det =
		new PLX9030Detector::plx9030Detector("/dev/plxdev0");
	det->init();
	det->start();
	sleep(1);
	det->stop();

	std::cout << std::hex <<
		"Check: 0x" << (int) det->checkMem() << std::dec << std::endl;

	std::vector<PLX9030Detector::raw_data> values;
	std::vector<PLX9030Detector::four_value> values4;

	values = det->getAllMemory();
	values4 = det->convertToFourValue(values);

	for(auto val: values4){
		std::cout << " x1 = " << val.x1
			  << ", x2 = " << val.x2
			  << ", y1 = " << val.y1
			  << ", y2 = " << val.y2;
		if(!val.correct) std::cout << " NOT CORRECT!";
		std::cout << std::endl;
	}


	/*
	for(auto val : values){
		std::cout << std::dec <<
		val.code << " " << val.value <<
		std::hex << "  <- 0x" << val.raw  << std::dec << std::endl;
	}
	std::cout << "size : " << values.size() << std::endl;
	*/

	/*
	for(int i=0;i<999;i++){
		mem_val = det->readMem();
		std::cout << std::dec << i <<  "\r\t" <<
		mem_val.code << " " << mem_val.value <<
		std::hex << "  <- 0x" << mem_val.raw << std::endl;

		if(i>1) std::cout << std::hex << "0x" <<
		(int) det->checkMem() << std::dec << std::endl;

	}
	*/

	delete det;
	std::cout << "exit." << std::endl;
}
