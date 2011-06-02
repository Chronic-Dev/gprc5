/**
 * GreenPois0n Anthrax - recoveryagent.c
 * Copyright (C) 2011 Chronic-Dev Team
 * Copyright (C) 2011 Joshua Hill
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
#include "device.h"
#include "recoveryagent.h"


int recoveryagent_install() {
	int ret = 0;

	if(!recoveryagent_exists()) {
		puts("Installing RecoveryAgent...\n");
		ret = install("/files/RecoveryAgent", "/mnt/RecoveryAgent", 0, 80, 0755);
		if (ret < 0) return -1;

		ret = install("/files/com.zdziarski.liverecovery.plist", "/mnt/System/Library/LaunchDaemons/com.zdziarski.liverecovery.plist", 0, 80, 0644);
		if (ret < 0) return -1;
	} else {
		puts("RecoveryAgent already installed, skipping...\n");
	}
	return 0;
}

int recoveryagent_exists() {
	if(!file_exists("/mnt/RecoveryAgent")) {
		return 1;
	}
	return 0;
}

int recoveryagent_uninstall() {
	puts("Removing RecoveryAgent...\n");
	unlink("/mnt/RecoveryAgent");
	unlink("/mnt/System/Library/LaunchDaemons/com.zdziarski.liverecovery.plist");
	return 0;
}

