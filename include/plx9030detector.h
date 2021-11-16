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


#ifndef PLX9030DETECTOR_H
#define PLX9030DETECTOR_H

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdbool.h>
#include <vector>

#include <plx9030c.h>


namespace PLX9030Detector{

#define MEMORY_SIZE 524288 //bytes

#define MEMORY_EMPTY 0
#define MEMORY_HALF 2
#define MEMORY_FULL 3

#define Y2 3
#define Y1 7
#define X2 1
#define X1 5

	struct raw_data {
		int code;
		int value;
		uint16_t raw;
	};

	struct four_value {
		int x1, x2, y1, y2;
		bool correct;
	};


	/* PSD Detector */
	class Plx9030PSD {
	public:
		explicit Plx9030PSD(std::string device);
		~Plx9030PSD();
		void init(void);
		void start(void);
		void stop(void);
		raw_data readMem(void);
		std::vector<raw_data> getAllMemory(void);
		std::vector<four_value> convertToFourValue(
			std::vector<raw_data>
			);
		unsigned char checkMem(void);
		int getStatus(void);

	public:

		static bool is_runing;
		static bool is_mem_end;
		static bool is_half_mem_end;
		static unsigned int mem_count;

	private:
		PLX9030::plx9030 *plx = nullptr;
		int fromCode(int code);
		int status {0};
	};


	/* COUNTER */
	class Plx9030Counter{
	public:
		Plx9030Counter(std::string chrdev);
		~Plx9030Counter();
		int getStatus(void);

	protected:
		PLX9030::plx9030 *plx = nullptr;
		std::string _chrdev;
		int status {0};

	};
}


extern "C"
{
	PLX9030Detector::Plx9030PSD *
	plx9030Psd(char *chrdev) {
		return new PLX9030Detector::
			Plx9030PSD(std::string(chrdev));
	}

	unsigned int checkMemPsd(PLX9030Detector::Plx9030PSD *f) {
		return f->checkMem();
	}

	int getStatusPsd(PLX9030Detector::Plx9030PSD *f) {
		return f->getStatus();
	}

	void initPsd(PLX9030Detector::Plx9030PSD *f) {
		f->init();
	}

	void startPsd(PLX9030Detector::Plx9030PSD *f) {
		f->start();
	}

	void stopPsd(PLX9030Detector::Plx9030PSD *f) {
		f->stop();
	}

	void delete_plx9030Psd(PLX9030Detector::Plx9030PSD *f) {
		delete f;
	}

	void readDataPsd(PLX9030Detector::Plx9030PSD *f,
		       unsigned int *data,
		       int size) {

		// auto fullMem = f->getAllMemory();
		// for(auto var : fullMem) { .... } etc...

		int coord;
		for(int i = 0; i < 10; i++){
			coord = rand() % (size * size);
			data[coord] += 1;
		}
	}
}


#endif // PLX9030DETECTOR_H
