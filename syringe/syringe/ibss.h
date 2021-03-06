/**
 * GreenPois0n Syringe - ibss.h
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

#ifndef IBSS_H_
#define IBSS_H_

#include <stdint.h>

#include "img3.h"

typedef struct ibss_t {
	char* url;
	char* path;
	img3_t* image;
} ibss_t;

ibss_t* ibss_create();
ibss_t* ibss_open(char* path);
ibss_t* ibss_download(char* url);
ibss_t* ibss_load(unsigned char* data, unsigned int size);
void ibss_free(ibss_t* ibss);

//void ibss_boot(ibss_t* ibss); // TODO: extract to parent class?
//void ibss_send(ibss_t* ibss); // TODO: extract to parent class?
//void ibss_patch(ibss_t* ibss); // TODO: extract to parent class?

#endif /* IBSS_H_ */
