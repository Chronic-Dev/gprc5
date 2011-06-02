/**
 * GreenPois0n Anthrax - loader.c
 * Copyright (C) 2011 Chronic-Dev Team
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

#include "loader.h"
#include "utils.h"

int loader_install() {
	int ret;

	puts("Creating Loader.app directory\n");
	mkdir("/mnt/Applications/Loader.app", 0755);

	puts("Installing Loader.app Bootstrap binary\n");
	ret = install("/files/Loader.app/Bootstrap", "/mnt/Applications/Loader.app/Bootstrap", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app Loader binary\n");
	ret = install("/files/Loader.app/Loader", "/mnt/Applications/Loader.app/Loader", 0, 80, 06755);
	if (ret < 0) return ret;

	puts("Installing Loader.app cydia.png image\n");
	ret = install("/files/Loader.app/cydia.png", "/mnt/Applications/Loader.app/cydia.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app cydia@2x.png image\n");
	ret = install("/files/Loader.app/cydia@2x.png", "/mnt/Applications/Loader.app/cydia@2x.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app Info.plist plist\n");
	ret = install("/files/Loader.app/Info.plist", "/mnt/Applications/Loader.app/Info.plist", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app icon.png image\n");
	ret = install("/files/Loader.app/icon.png", "/mnt/Applications/Loader.app/icon.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app icon@2x.png image\n");
	ret = install("/files/Loader.app/icon@2x.png", "/mnt/Applications/Loader.app/icon@2x.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app icon-pad.png image\n");
	ret = install("/files/Loader.app/icon-ipad.png", "/mnt/Applications/Loader.app/icon-ipad.png", 0, 80, 0755);
	if (ret < 0) return ret;

	puts("Installing Loader.app PkgInfo file\n");
	ret = install("/files/Loader.app/PkgInfo", "/mnt/Applications/Loader.app/PkgInfo", 0, 80, 0755);
	if (ret < 0) return ret;

	return 0;
}

int loader_exists() {
	if(!file_exists("/mnt/Applications/Loader.app/Loader")) {
		return 1;
	}
	return 0;
}

int loader_uninstall() {
	int ret;

	puts("Removing Loader.app Bootstrap binary\n");
	unlink("/mnt/Applications/Loader.app/Bootstrap");

	puts("Removing Loader.app Loader binary\n");
	unlink("/mnt/Applications/Loader.app/Loader");

	puts("Removing Loader.app cydia.png image\n");
	unlink("/mnt/Applications/Loader.app/cydia.png");

	puts("Removing Loader.app cydia@2x.png image\n");
	unlink("/mnt/Applications/Loader.app/cydia@2x.png");

	puts("Removing Loader.app Info.plist plist\n");
	unlink("/mnt/Applications/Loader.app/Info.plist");

	puts("Removing Loader.app icon.png image\n");
	unlink("/mnt/Applications/Loader.app/icon.png");

	puts("RemovingLoader.app icon@2x.png image\n");
	unlink("/mnt/Applications/Loader.app/icon@2x.png");

	puts("Removing Loader.app icon-pad.png image\n");
	unlink("/mnt/Applications/Loader.app/icon-ipad.png");

	puts("Removing Loader.app PkgInfo file\n");
	unlink("/mnt/Applications/Loader.app/PkgInfo");

	//puts("Removing Loader.app directory\n");
	//rmdir("/mnt/Applications/Loader.app");

	return 0;
}

