/**
 * GreenPois0n Anthrax - immutable.c
 * Copyright (C) 2011 Chronic-Dev Team
 * Copyright (C) 2011 Joshua Hill
 * Copyright (C) 2011 Nicolas Haunold
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

#include "utils.h"
#include "immutable.h"

const char* immutable[] = { "/immutable", NULL };

int immutable_install() {
	int ret = 0;

	ret = install("/files/immutable", "/mnt/immutable", 0, 80, 0755);
	if (ret < 0) return -1;

	ret = fsexec(immutable, cache_env, true);
	if(ret < 0) return -1;

	unlink("/mnt/immutable");
	return 0;
}

int immutable_uninstall() {
	unlink("/mnt/immutable");
	return 0;
}

