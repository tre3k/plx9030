/*
 *  Copyright (c) 2019-2021 NRC KI PNPI, Gatchina, LO, 188300 Russia
 *
 *  this file is part of kernel module plx9030.
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
 *  Library for working with PSD neutron detector and 4-channel counter,
 *  devices use PCI controller plx9030.
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

	/**
	 * @brief Neutron PSD detector class
	 * @code
	 * auto psd = new Plx9030PSD("/dev/plxdev0");
	 * @endcode
	 * @param [in] device Path to file of char device of plx9030
	 */
	class Plx9030PSD {
	public:
		explicit Plx9030PSD(std::string device);
		~Plx9030PSD();
		/**
		 * @brief Initialisation function
		 * You need use this before working with PSD detector
		 * @code
		 * psd->init();
		 * @endcode
		 */
		void init(void);
		/**
		 * @brief Start count function
		 * @code
		 * psd->start();
		 * @endcode
		 */
		void start(void);
		/**
		 * @brief Stop count function
		 * @code
		 * psd->stop();
		 * @endcode
		 */
		void stop(void);
		/**
		 * @brief Read memory with time delay information
		 * This function read 2 bytes from PSD memory (just one pop
		 * from FIFO memroy)
		 * @code
		 * auto rd = psd->readMem();
		 * @endcode
		 * @param out rd.raw uint16_t type of raw value of memroy \
		 * [ 4 bit code ][ 12 bit value ]
		 * @param out rd.code determine of x0, x1, y0 or y1
		 * @param out rd.value value of time delay between anode \
		 * signal and x0, x1, y0, y1 signal
		 */
		raw_data readMem(void);
		/**
		 * @brief Read all memory
		 * read memory (call readMem() function) until the whole
		 * @code
		 * auto mem = psd->getAllMemory();
		 * for(auto rd : mem) {
		 *     std::cout << rd.code << ":" << rd.value << std::endl;
		 * }
		 * @endcode
		 */
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


	/**
	 * @brief Neutron counter class
	 * @code
	 * auto counter = Plx9030Counter("/dev/plxdev0");
	 * @endcode
	 * @param in chrdev Path to char device file of plx9030
	 */
	class Plx9030Counter{
	public:
		Plx9030Counter(std::string chrdev);
		~Plx9030Counter();

		/**
		 * @brief Status values
		 */
		const int STATUS_OK {0};
		const int STATUS_ERROR_FILE_DESCRIPTOR {-1};
		const int STATUS_NOT_COUNTER_DEVICE {-2};

		/**
		 * @brief Control Register Bits
		 */
		const uint8_t CONTROL_REGISTER {2};
		const uint8_t CR_RG_GATE {0x80};
		const uint8_t CR_LOAD_CRM {0x40};
		const uint8_t CR_END_CRM {0x20};
		const uint8_t CR_RGCLR {0x10};

		/**
		 * @brief Time control register bits
		 */
		const uint8_t TIMER_REGISTER {3};
		const uint8_t TR_GATE_SEL {0x80};
		const uint8_t TR_LOAD_GATE {0x40};
		const uint8_t TR_EN_GATE {0x20};
		const uint8_t TR_CLR_GATE {0x10};

                /**
		 * @brief timer value registers
		 */
		const uint8_t TIMER_STATUS_REGISTER {4};
		const uint8_t TIMER_BUFFER_REGISTER {8};
		const uint8_t TIMER_VALUE_REGISTER {12};

		int getStatus(void);
		uint32_t readValue(short channel);
		void writeValue(uint32_t value, short channel);
		void setTimeInterval(unsigned int time);
		void startTimer(void);
		void stopTimer(void);
		bool isFinish(void);
		void resetCounter(void);
		uint32_t readTimer(void);
		void writeTimer(uint32_t value);
		void resetTimer(void);

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
		// auto values = convertToFourValue(fullMem);
		// for(auto var : values) { .... } etc...

		/* VIRTUAL TEST */
		int coord;
		for(int i = 0; i < 10; i++){
			coord = rand() % (size * size);
			data[coord] += 1;
		}
	}

	PLX9030Detector::Plx9030Counter *plx9030Counter(char *chrdev) {
		return new PLX9030Detector::
			Plx9030Counter(std::string(chrdev));
	}

	bool isFinishCounter(PLX9030Detector::Plx9030Counter *f) {
		// return f->isFinish();
		return true;
	}

	int readValueCounter(PLX9030Detector::Plx9030Counter *f,
			     int channel) {
		// return f->readValue((short)(channel & 0xffff));
		/* VIRTUAL TEST */
		return rand()%100;
	}

	void startCounter(PLX9030Detector::Plx9030Counter *f) {
		f->startTimer();
	}

	void stopCounter(PLX9030Detector::Plx9030Counter *f) {
		f->stopTimer();
	}

	void resetCounter(PLX9030Detector::Plx9030Counter *f) {
		f->resetCounter();
	}
}


#endif // PLX9030DETECTOR_H
