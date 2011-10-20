/**
  * GreenPois0n Cynanide - common.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdarg.h"

#include "task.h"
#include "lock.h"
#include "common.h"
#include "framebuffer.h"


void(*_free)(void* ptr) = NULL;
void*(*_malloc)(unsigned int size, int count, int zone) = NULL;
int(*_printf)(const char *fmt, ...) = NULL;
void(*_serial_write)(int bus, char* buffer, int size) = NULL;

void* gLoadaddr = NULL;
void* gBaseaddr = NULL;
void* gRomBaseaddr = NULL;
void* gBssBaseaddr = NULL;
void* gBootBaseaddr = NULL;

char* gDeviceString = (char*) 0x200;
char* gVersionString = (char*) 0x280;
char gVersionBuf[255];
unsigned long gVersion = 0;

int cout_count = 0;

void _debug(const char* fmt, ...) {
	char* buffer[1024];
	memset(buffer, '\0', 1024);

	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 1023, fmt, args);
	va_end(args);

	serial_write(buffer);
}

void* find_printf() {
	unsigned int sp;
	int i = 0, j = 0;
	unsigned int* stack = &sp;
	void(*default_block_write)(void) = find_function("default_block_write", gBaseaddr, gBaseaddr);
	default_block_write();
	//hexdump(((unsigned char*) stack) - 0x100, 0x200);
	for(i = 0; i < 0x100; i += 4) {
		//default_block_write();
		unsigned int value = *(stack - i);
		//debug("value = 0x%08x\r\n", value);
		if((value & 0xFFF00000) == gBaseaddr) {
			for(j = 0; j < 0x100; j++) {
				//unsigned int* arm = (unsigned int*)(value + j);
				unsigned short* thumb = (unsigned short*)(value + j);
				//debug("stack[%d] = 0x%08x, value[%d] = 0x%04x\r\n", i, value, j, *thumb);
				if(*thumb == 0xB40F || *thumb == 0xB083) {
					debug("stack[%d] = 0x%08x, value[%d] = 0x%04x\r\n", i, value, j, *thumb);
					return (void*) value + (j+1);

				// Added to detect top of printf in 5.x
				//} else if(*arm == 0xB580B083) {
				//	int(*print)(const char *fmt, ...) = (void*) value + (j+1);
				//	print("ARM: stack[%d] = 0x%08x, value[%d] = 0x%08x\r\n", i, value, j, *arm);
				//	return (void*) value + (j+1);
				}
			}
		}
	}
	return 0;
}

void* find_free() {
	return find_function("free", gBaseaddr, gBaseaddr);
}

void* find_malloc() {
	//
	void* bytes = patch_find(gBaseaddr, 0x40000, "\x80\xB5\x00\xAF\x01\x21\x00\x22", 8);
	if (bytes == NULL) {
		bytes = patch_find(gBaseaddr, 0x40000, "\xB0\xB5\x01\xFB\x00\xF4", 6);
		if(bytes == NULL) return NULL;
	}
	return bytes+1;
}

int common_init() {
	memset(gVersionBuf, '\0', sizeof(gVersionBuf));
	strcpy(gVersionBuf, gVersionString);
	gVersionBuf[10] = '\0';

	gVersion = strtoul(&gVersionBuf[6], NULL, 0);
	// Setup global device and firmware variables here
	// Still need to add the framebuffer address here, or just look it up later on
	// We could also look up loadaddr later on maybe
	// m68ap = iPhone2g
	if(strstr(gDeviceString, "m68ap")) {
		gLoadaddr = 0xDEADBEEF; // Fix Me!!
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xDEADBEEF; // Fix Me!!

	// n88ap = iPhone3gs
	} else if(strstr(gDeviceString, "n88ap")) {
		if(gVersion >= 1219) {
			gLoadaddr = 0x40000000;
		} else {
			gLoadaddr = 0x41000000;
		}

		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x4FF00000;

	// n90ap, n92ap = iPhone4
	} else if(strstr(gDeviceString, "n90ap")
		 || strstr(gDeviceString, "n92ap")) {
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;

		if(gVersion >= 1219) {
			gLoadaddr = 0x40000000;
		} else {
			gLoadaddr = 0x41000000;
		}

	// n81ap = iPod4g
		//gLoadaddr = 0x41000000;
		//gRomBaseaddr = 0xBF000000;
		//gBssBaseaddr = 0x84000000;
		//gBootBaseaddr = 0x5FF00000;

	// n18ap = iPod3g
	} else if(strstr(gDeviceString, "n18ap")) {
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x4FF00000;

	// n72ap = iPod2g
	} else if(strstr(gDeviceString, "n72ap")) {
		gLoadaddr = 0x09000000;
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xFF000000; // Check Me!!

	// k48ap = iPad1
	} else if(strstr(gDeviceString, "k48ap")) {
		if(gVersion >= 1219) {
			gLoadaddr = 0x40000000;
		} else {
			gLoadaddr = 0x41000000;
		}
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;

	// k93ap, k94ap, k95ap = iPad2
	} else if(strstr(gDeviceString, "k93ap") ||
				strstr(gDeviceString, "k94ap") ||
				strstr(gDeviceString, "k95ap")) {
		gLoadaddr = 0xDEADBEEF; // Fix Me!!
		gRomBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBssBaseaddr = 0xDEADBEEF; // Fix Me!!
		gBootBaseaddr = 0xDEADBEEF; // Fix Me!!

	// k66ap = AppleTV
	} else if(strstr(gDeviceString, "k66ap")) {
		gLoadaddr = 0x41000000;
		gRomBaseaddr = 0xBF000000;
		gBssBaseaddr = 0x84000000;
		gBootBaseaddr = 0x5FF00000;

	} else {
		gLoadaddr = NULL;
		gRomBaseaddr = NULL;
		gBssBaseaddr = NULL;
		gBootBaseaddr = NULL;
	}

	if(strstr(gDeviceString, "iBSS")) {
		gBaseaddr = gBssBaseaddr;
	} else

	if(strstr(gDeviceString, "iBEC")) {
		gBaseaddr = gBootBaseaddr;
	} else

	if(strstr(gDeviceString, "iBoot")) {
		gBaseaddr = gBootBaseaddr;
	}

	_serial_write = find_function("uart_write", gBaseaddr, gBaseaddr);
	if(_serial_write == NULL) {
		return -1;
	}

	_printf = find_printf();
	if(_printf == NULL) {
		fb_print("Unable to find printf\n");
		return -1;
	} else {
		printf("Found printf at 0x%x\n", _printf);
	}
	printf("%s\n%s\n", gDeviceString, gVersionString);

	_malloc = find_malloc();
	if(_malloc == NULL) {
		puts("Unable to find malloc\n");
		return -1;

	} else {
		printf("Found malloc at 0x%x\n", _malloc);
	}
	char* test = malloc(0x20);
	if(test == NULL) {
		printf("Unable to allocate test memory\n");
	}
	printf("memory allocated at 0x%08x\n", test);

	_free = find_free();
	if(_free == NULL) {
		puts("Unable to find free\n");
		return -1;

	} else {
		printf("Found free at 0x%x\n", _free);
	}
	printf("memory free successful\n");

	return 0;
}

void _puts(const char* message) {
	printf("%s", message);
	fb_print(message);
}

void hexdump(unsigned char* buf, unsigned int len) {
	int i, j;
	enter_critical_section();
	debug("0x%08x: ", buf);
	for (i = 0; i < len; i++) {
		if (i % 16 == 0 && i != 0) {
			for (j=i-16; j < i; j++) {
				unsigned char car = buf[j];
				if (car < 0x20 || car > 0x7f) car = '.';
				debug("%c", car);
			}
			debug("\r\n0x%08x: ", buf+i);
		}
		debug("%02x ", buf[i]);
	}

	int done = (i % 16);
	int remains = 16 - done;
	if (done > 0) {
		for (j = 0; j < remains; j++) {
			debug("   ");
		}
	}

	if ((i - done) >= 0) {
		if (done == 0 && i > 0) done = 16;
		for (j = (i - done); j < i; j++) {
			unsigned char car = buf[j];
			if (car < 0x20 || car > 0x7f) car = '.';
			debug("%c", car);
		}
	}

	debug("\r\n\n");
	exit_critical_section();
}

void printf_begin() {
	cout_count = 0;
}

void printf_filler() {
	int blanks = (0x201 - (cout_count % 0x200));
	if (blanks > 100 || blanks == 0) return;
	int i;
	for (i = 0; i < blanks; i++) {
		_printf("");
	}
	printf_begin();
}

int gpprintf(const char* fmt, ...) {
	int ret;
	//va_list args;
	enter_critical_section();
	
	//va_start(args, fmt);
	//ret = vprintf(fmt, args);
	//va_end(args);
	
	cout_count += ret;
	printf_filler();
	exit_critical_section();

	return ret;
}


void panic(const char* why) {
	printf("%s", why);
	while(1){}
}
