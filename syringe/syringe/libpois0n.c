/**
 * GreenPois0n Syringe - libpois0n.c
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
#include <sys/stat.h>

#include "libdioxin.h"
#include "libpois0n.h"
#include "libpartial.h"
#include "libirecovery.h"

#include "common.h"
//#include "ramdisk.h"
#include "exploits.h"
#include "payloads.h"
//#include "firmware.h"

#define GET_BUILD_VERSION		"go memory search $loadaddr 0x400 3C6B65793E50726F6475637456657273696F6E3C2F6B65793E"
#define GET_PRODUCT_BUILD_VERSION "go memory search $loadaddr 0x400 3C6B65793E50726F647563744275696C6456657273696F6E3C2F6B65793E"

#define LIMERA1N
#define STEAKS4UCE
//#define PWNAGE2

#define DEBUG_SERIAL

int upload_new_ibss();
int upload_new_ibec();
int upload_new_iboot();
int upload_new_devicetree();

int upload_old_ibss();
int upload_old_ibec();
int upload_old_iboot();
int upload_old_devicetree();

int fetch_new_image(const char* path, const char* output);
int fetch_new_dfu_image(const char* type, const char* output);
int fetch_new_firmware_image(const char* type, const char* output);

int fetch_old_image(const char* path, const char* output);
int fetch_old_dfu_image(const char* type, const char* output);
int fetch_old_firmware_image(const char* type, const char* output);

static pois0n_callback progress_callback = NULL;
static void* user_object = NULL;

void pois0n_set_error(const char* message) {
	strncpy(libpois0n_error, message, 255);
}

int recovery_callback(irecv_client_t client, const irecv_event_t* event) {
	if(progress_callback) {
		progress_callback(event->progress, user_object);
	}
	return 0;
}

void download_callback(ZipInfo* info, CDFile* file, size_t progress) {
	if(progress_callback)
		progress_callback(progress, user_object);
}

char* send_command(char* command) {
	char* ret = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;
	error = irecv_send_command(client, command);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to send command\n");
		return -1;
	}

	error = irecv_getret(client, &ret);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to send command\n");
		return -1;
	}

	return ret;
}

int fetch_new_image(const char* path, const char* output) {
	debug("Fetching %s...\n", path);
	if (download_file_from_zip(device->url_new, path, output, &download_callback) != 0) {
		pois0n_set_error("Unable to connect to the network");
		return -1;
	}

	return 0;
}

int fetch_old_image(const char* path, const char* output) {
	debug("Fetching %s...\n", path);
	if (download_file_from_zip(device->url_old, path, output, &download_callback) != 0) {
		pois0n_set_error("Unable to connect to the network");
		return -1;
	}

	return 0;
}

int fetch_new_dfu_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 254, "Firmware/dfu/%s", name);

	debug("Preparing to fetch DFU image from Apple's servers\n");
	if (fetch_new_image(path, output) < 0) {
		error("Unable to fetch DFU image from Apple's servers\n");
		return -1;
	}

	return 0;
}

int fetch_old_dfu_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.RELEASE.dfu", type, device->model);
	snprintf(path, 254, "Firmware/dfu/%s", name);

	debug("Preparing to fetch DFU image from Apple's servers\n");
	if (fetch_old_image(path, output) < 0) {
		error("Unable to fetch DFU image from Apple's servers\n");
		return -1;
	}

	return 0;
}

int fetch_new_firmware_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.img3", type, device->model);
	snprintf(path, 254, "Firmware/all_flash/all_flash.%s.production/%s", device->model, name);

	debug("Preparing to fetch firmware image from Apple's servers\n");
	if (fetch_new_image(path, output) < 0) {
		error("Unable to fetch firmware image from Apple's servers\n");
	}

	return 0;
}

int fetch_old_firmware_image(const char* type, const char* output) {
	char name[64];
	char path[255];

	memset(name, '\0', 64);
	memset(path, '\0', 255);
	snprintf(name, 63, "%s.%s.img3", type, device->model);
	snprintf(path, 254, "Firmware/all_flash/all_flash.%s.production/%s", device->model, name);

	debug("Preparing to fetch firmware image from Apple's servers\n");
	if (fetch_old_image(path, output) < 0) {
		error("Unable to fetch firmware image from Apple's servers\n");
	}

	return 0;
}

int upload_new_dfu_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s.new", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_new_dfu_image(type, image) < 0) {
			error("Unable to upload DFU image\n");
			return -1;
		}
	}

	if (client->mode != kDfuMode) {
		debug("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			return -1;
		}
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_old_dfu_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s.old", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_old_dfu_image(type, image) < 0) {
			error("Unable to upload DFU image\n");
			return -1;
		}
	}

	if (client->mode != kDfuMode) {
		debug("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			return -1;
		}
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_new_firmware_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s.new", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_new_firmware_image(type, image) < 0) {
			error("Unable to upload firmware image\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_old_firmware_image(const char* type) {
	char image[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(image, '\0', 255);
	snprintf(image, 254, "%s.%s.old", type, device->model);

	debug("Checking if %s already exists\n", image);
	if (stat(image, &buf) != 0) {
		if (fetch_old_firmware_image(type, image) < 0) {
			error("Unable to upload firmware image\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s to device\n", image);
	error = irecv_send_file(client, image, 1);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload firmware image\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int upload_firmware_payload(const char* type) {
	int size = 0;
	const unsigned char* payload = NULL;
	irecv_error_t error = IRECV_E_SUCCESS;

	if(!strcmp(type, "iBSS")) {
		payload = iBSS_payload;
		size = sizeof(iBSS_payload);
		debug("Loaded payload for iBSS\n");

	} else if(!strcmp(type, "iBEC")) {
		payload = iBEC_payload;
		size = sizeof(iBEC_payload);
		debug("Loaded payload for iBEC\n");

	} else if(!strcmp(type, "iBoot")) {
		payload = iBoot_payload;
		size = sizeof(iBoot_payload);
		debug("Loaded payload for iBoot\n");

	} else {
		return -1;
	}

	if (payload == NULL) {
		pois0n_set_error("Unable to upload firmware payload\n");
		return -1;
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload firmware payload\n");
		debug("%s\n", irecv_strerror(error));
		return -1;
	}

	debug("Uploading %s payload\n", type);
	error = irecv_send_buffer(client, (unsigned char*) payload, size, 1);
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to upload payload\n");
		return -1;
	}

	return 0;
}

int upload_new_ibss() {
	if (upload_new_dfu_image("iBSS") < 0) {
		error("Unable upload iBSS\n");
		return -1;
	}
	return 0;
}

int upload_old_ibss() {
	if (upload_old_dfu_image("iBSS") < 0) {
		error("Unable upload iBSS\n");
		return -1;
	}
	return 0;
}

int upload_new_ibec() {
	if (upload_new_dfu_image("iBEC") < 0) {
		error("Unable upload iBEC\n");
		return -1;
	}
	return 0;
}

int upload_old_ibec() {
	if (upload_old_dfu_image("iBEC") < 0) {
		error("Unable upload iBEC\n");
		return -1;
	}
	return 0;
}


int upload_new_iboot() {
	if (upload_new_firmware_image("iBoot") < 0) {
		error("Unable upload iBoot\n");
		return -1;
	}
	return 0;
}

int upload_old_iboot() {
	if (upload_old_firmware_image("iBoot") < 0) {
		error("Unable upload iBoot\n");
		return -1;
	}
	return 0;
}

int upload_new_devicetree() {
	if (upload_new_firmware_image("DeviceTree") < 0) {
		error("Unable upload DeviceTree\n");
		return -1;
	}
	return 0;
}

int upload_old_devicetree() {
	if (upload_old_firmware_image("DeviceTree") < 0) {
		error("Unable upload DeviceTree\n");
		return -1;
	}
	return 0;
}

int upload_ramdisk() {
	unsigned int ramdisk_dmg_len = 0;
	unsigned char* ramdisk_dmg = NULL;
	file_read("ramdisk.dmg", &ramdisk_dmg, &ramdisk_dmg_len);
	if(ramdisk_dmg && ramdisk_dmg_len) {
		irecv_send_buffer(client, ramdisk_dmg, ramdisk_dmg_len, 0);
	} else {
		error("Unable to open ramdisk.dmg\n");
		return -1;
	}
	free(ramdisk_dmg);
	return 0;
}

int upload_new_kernelcache() {
	struct stat buf;
	char kernelcache[255];
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(kernelcache, '\0', 255);
	memset(&buf, '\0', sizeof(buf));
	snprintf(kernelcache, 254, "kernelcache.release.%c%c%c", device->model[0], device->model[1], device->model[2]);
	debug("Checking if kernelcache already exists\n");
	if (stat(kernelcache, &buf) != 0) {
		if (fetch_new_image(kernelcache, kernelcache) < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		pois0n_set_error("Unable upload kernelcache\n");
		return -1;
	}

	error = irecv_send_file(client, kernelcache, 0);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		pois0n_set_error("Unable upload kernelcache\n");
		return -1;
	}

	return 0;
}

int upload_old_kernelcache() {
	struct stat buf;
	char kernelcache[255];
	irecv_error_t error = IRECV_E_SUCCESS;

	memset(kernelcache, '\0', 255);
	memset(&buf, '\0', sizeof(buf));
	snprintf(kernelcache, 254, "kernelcache.release.%c%c%c", device->model[0], device->model[1], device->model[2]);
	debug("Checking if kernelcache already exists\n");
	if (stat(kernelcache, &buf) != 0) {
		if (fetch_old_image(kernelcache, kernelcache) < 0) {
			error("Unable to upload kernelcache\n");
			return -1;
		}
	}

	debug("Resetting device counters\n");
	error = irecv_reset_counters(client);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		pois0n_set_error("Unable upload kernelcache\n");
		return -1;
	}

	error = irecv_send_file(client, kernelcache, 0);
	if (error != IRECV_E_SUCCESS) {
		debug("%s\n", irecv_strerror(error));
		pois0n_set_error("Unable upload kernelcache\n");
		return -1;
	}

	return 0;
}

int upload_ibss_payload() {
	if (upload_firmware_payload("iBSS") < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}
	return 0;
}

int upload_ibec_payload() {
	if (upload_firmware_payload("iBEC") < 0) {
		error("Unable to upload iBEC payload\n");
		return -1;
	}
	return 0;
}

int fetchVersionURL()
{
//	
//	char* x = NULL;
//	irecv_error_t error = IRECV_E_SUCCESS;
//	
//	if(boot_iboot() < 0) {
//		debug("Unable to load iBoot\n");
//		return -1;
//	}
//	
//	debug("Mounting root filesystem\n");
//	x = send_command("go fs mount nand0a /boot");
//	if (error != IRECV_E_SUCCESS) {
//		pois0n_set_error("Unable to mount root filesystem\n");
//		return -1;
//	}
//	
//	debug("Reading system version\n");
//	x = send_command("go fs load /boot/System/Library/CoreServices/SystemVersion.plist $loadaddr");
//	if (error != IRECV_E_SUCCESS) {
//		pois0n_set_error("Unable to read system version\n");
//		return -1;
//	}
//	x = send_command(GET_BUILD_VERSION);
//	x = send_command("go memory search $_ 0x40 3C737472696E673E");
//	x = send_command("go string print $_");
//	
//	unsigned char* start = strstr(x, "<string>") + sizeof("<string>") - 1;
//	unsigned char* stop = strstr(start, "</string>");
//	unsigned int length = stop - start;
//	char version[0x10];
//	memset(version, '\0', sizeof(version));
//	memcpy(version, (void*) start, length < sizeof(version) ? length : sizeof(version));
//	
//		//printf("Found version %s\n", version);
//	x = send_command("go fs load /boot/System/Library/CoreServices/SystemVersion.plist $loadaddr");
//	x = send_command(GET_PRODUCT_BUILD_VERSION);
//	x = send_command("go memory search $_ 0x40 3C737472696E673E");
//	x = send_command("go string print $_");
//	
//	char buildVersion[0x10];
//	unsigned char*start2 = strstr(x, "<string>") + sizeof("<string>") - 1;
//	unsigned char*stop2 = strstr(start2, "</string>");
//	unsigned int length2 = stop2 - start2;
//	memset(buildVersion, '\0', sizeof(buildVersion));
//	memcpy(buildVersion, (void*) start2, length2 < sizeof(buildVersion) ? length2 : sizeof(buildVersion));
//	printf("Found build version %s\n", buildVersion);
//	
//	plist_t thePlist = loadFirmwareList();
//	char *product = device->product;
//	char *urlString = NULL;
//
//	plist_t productNode = plist_dict_get_item(thePlist, product);
//	plist_t versionNode = plist_dict_get_item(productNode, version);
//	plist_t buildNode = plist_dict_get_item(versionNode, buildVersion);
//	plist_t urlNode = plist_dict_get_item(buildNode, "URL");
//	plist_get_string_val(urlNode, &urlString);
//	printf("device->url: %s\n", device->url);
//	printf("found URL %s for %s (%s)\n", urlString, version, buildVersion);
//	device->url = urlString;
//	printf("device->url: %s\n", device->url);
//	
//	plist_free(urlNode);
//	plist_free(buildNode);
//	plist_free(versionNode);
//	plist_free(productNode);
//	plist_free(thePlist);
//	
//	//printf("DONE!!!\n");
//	//return urlString;
	return 0;
	
}

int boot_ramdisk() {
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Preparing to upload old iBEC\n");
	if(upload_old_ibec() < 0) {
		error("Unable to upload iBEC to device\n");
		return -1;
	}

	debug("Decrypting old iBEC\n");
    error = irecv_send_command(client, "go image decrypt $loadaddr");
    if(error != IRECV_E_SUCCESS) {
		error("Unable to upload iBEC\n");
		return -1;
	}

    debug("Moving old iBEC into place\n");
    if(device->chip_id == 8720) {
        error = irecv_send_command(client, "go memory move 0x09000040 $loadaddr 0x48000");
    } else {
        error = irecv_send_command(client, "go memory move 0x41000040 $loadaddr 0x48000");
    }
    if(error != IRECV_E_SUCCESS) {
        pois0n_set_error("Unable to append old iBEC\n");
        return -1;
    }

	debug("Patching old iBEC\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000");

	debug("Jumping into old iBEC\n");
	error = irecv_send_command(client, "go jump $loadaddr");
	if(error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to hook jump_to\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload old iBEC payload\n");
	upload_ibec_payload();
	error = irecv_send_command(client, "go");
	debug("Preparing to upload new iBEC\n");
	if(upload_new_ibec() < 0) {
		error("Unable to upload iBEC to device\n");
		return -1;
	}

	debug("Decrypting new iBEC\n");
    error = irecv_send_command(client, "go image decrypt $loadaddr");
    if(error != IRECV_E_SUCCESS) {
		error("Unable to upload iBEC\n");
		return -1;
	}

    debug("Moving new iBEC into place\n");
    if(device->chip_id == 8720) {
        error = irecv_send_command(client, "go memory move 0x09000040 $loadaddr 0x48000");
    } else {
        error = irecv_send_command(client, "go memory move 0x41000040 $loadaddr 0x48000");
    }
    if(error != IRECV_E_SUCCESS) {
        pois0n_set_error("Unable to append iBEC\n");
        return -1;
    }

	debug("Patching iBEC\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000");

	//int offset = send_command("go memory search 0x41000000 0x48000 80B56F4684B04FF0");
	error = irecv_send_command(client, "go mw 0x41000F40 0x47184b00");
	error = irecv_send_command(client, "go mw 0x41000F44 0x40000000");
	//offset = send_command("go memory search 0x41000000 0x48000 004B184784B04FF0");
	//offset = send_command("go memory search $_ F0");
	//send_command(go mw)

	debug("Jumping back into new iBEC\n");
	error = irecv_send_command(client, "go jump $loadaddr");
	if(error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to hook jump_to\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload old iBEC payload\n");
	upload_ibec_payload();
	send_command("go");

	debug("Preparing to upload ramdisk\n");
	if (upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}

	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "ramdisk");
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to execute ramdisk command\n");
		return -1;
	}

	debug("Setting kernel bootargs\n");
	#ifdef DEBUG_SERIAL
	error = irecv_send_command(client, "go kernel bootargs rd=md0 -v serial=1 debug=0xa");
	#endif

	#ifndef DEBUG_SERIAL
	error = irecv_send_command(client, "go kernel bootargs rd=md0 -v amfi_allow_any_signature=1");
	#endif

	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to set kernel bootargs\n");
		return -1;
	}

	//4.3 bug - it seems if the devicetree is not loaded the one from nand is used and this causes bad things to happen...
	//semaphore: Added for 4.3 ramdisk booting - devicetree seems to be needed for the ramdisk to run on 4.3
	debug("Preparing to upload devicetree\n");
	if (upload_new_devicetree() < 0) {
		error("Unable to upload devicetree\n");
		return -1;
	}

	debug("Loading devicetree\n");
	error = irecv_send_command(client, "devicetree");
	if(error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to load devicetree\n");
		return -1;
	}
	//End 4.3 bug

	debug("Preparing to upload kernelcache\n");
	if (upload_new_kernelcache() < 0) {
		error("Unable to upload kernelcache\n");
		return -1;
	}

	error = irecv_send_command(client, "go rdboot");
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to boot kernelcache\n");
		return -1;
	}

	error = irecv_send_command(client, "bootx");
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to boot kernelcache\n");
		return -1;
	}
	return 0;
}

int boot_tethered() {
	
		//	fetchVersionURL();
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("TETHERED: Preparing to upload ramdisk\n");
	if (upload_ramdisk() < 0) {
		error("Unable to upload ramdisk\n");
		return -1;
	}
	
	debug("Executing ramdisk\n");
	error = irecv_send_command(client, "ramdisk");
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to execute ramdisk command\n");
		return -1;
	}
	
	debug("Setting kernel bootargs\n");
	#ifdef DEBUG_SERIAL
	error = irecv_send_command(client, "go kernel bootargs -v serial=1 debug=0xa amfi_allow_any_signature=1");
	#endif
	#ifndef DEBUG_SERIAL
	error = irecv_send_command(client, "go kernel bootargs -v amfi_allow_any_signature=1");
	#endif

	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to set kernel bootargs\n");
		return -1;
	}

//4.3 bug - it seems if the devicetree is not loaded the one from nand is used and this causes bad things to happen...
//semaphore: Added for 4.3 ramdisk booting - devicetree seems to be needed for the ramdisk to run on 4.3
    debug("Preparing to upload devicetree\n");
    if (upload_new_devicetree() < 0) {
        error("Unable to upload devicetree\n");
        return -1;
    }

    debug("Loading devicetree\n");
    error = irecv_send_command(client, "go devicetree");
    if(error != IRECV_E_SUCCESS) {
        pois0n_set_error("Unable to load devicetree\n");
        return -1;
    }
//End 4.3 bug

	debug("Preparing to upload kernelcache\n");
	if (upload_new_kernelcache() < 0) {
		error("Unable to upload kernelcache\n");
		return -1;
	}

	debug("Hooking jump_to command\n");
	error = irecv_send_command(client, "go rdboot");
	if(error != IRECV_E_SUCCESS) {
		error("Unable to hook jump_to\n");
		return -1;
	}

	error = irecv_send_command(client, "bootx");
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to boot kernelcache\n");
		return -1;
	}

	return 0;
}


int boot_ibec() {
	irecv_error_t error = IRECV_E_SUCCESS;
	
	debug("Loading iBEC\n");
	error = irecv_send_command(client, "go image load 0x69626F74 $loadaddr"); //FIXME: don't know how to find these addresses
	
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable load iBEC to memory\n");
		return -1;
	}
	
	debug("Shifting iBEC\n");
	if (device->chip_id == 8720) {
		error = irecv_send_command(client,"go memory move 0x9000040 0x9000000 0x48000"); //FIXME: don't know how to find these addresses
	} else {
		error = irecv_send_command(client,
								   "go memory move 0x41000040 0x41000000 0x48000"); //FIXME: don't know how to find these addresses
	}
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to move iBEC into memory\n");
		return -1;
	}
	
	debug("Patching iBoot\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000"); //FIXME: don't know how to find these addresses
	
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to patch iBEC\n");
		return -1;
	}
	
	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);
	
	debug("Jumping into iBEC\n");
	
	error = irecv_send_command(client, "go jump $loadaddr"); //FIXME: don't know how to find these addresses
	
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to jump into iBEC\n");
		return -1;
	}
	
	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		pois0n_set_error("Unable to boot the device tethered\n");
		return -1;
	}
	
	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);
	
	if (upload_firmware_payload("iBEC") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}
	
	debug("Initializing greenpois0n in iBEC\n");
	irecv_send_command(client, "go");
	
	return 0;
}

int boot_iboot() {
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Loading iBoot\n");
		error = irecv_send_command(client, "go image load 0x69626F74 $loadaddr");

	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable load iBoot to memory\n");
		return -1;
	}

	debug("Shifting iBoot\n");
	if (device->chip_id == 8720) {
		error = irecv_send_command(client,"go memory move 0x9000040 0x9000000 0x48000");
	} else {
		error = irecv_send_command(client,
				"go memory move 0x41000040 0x41000000 0x48000");
	}
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to move iBoot into memory\n");
		return -1;
	}

	debug("Patching iBoot\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000");
	
	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to patch iBoot\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "false");
	irecv_saveenv(client);

	debug("Jumping into iBoot\n");

		error = irecv_send_command(client, "go jump $loadaddr");

	if (error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to jump into iBoot\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		pois0n_set_error("Unable to boot the device tethered\n");
		return -1;
	}

	irecv_setenv(client, "auto-boot", "true");
	irecv_saveenv(client);

	if (upload_firmware_payload("iBoot") < 0) {
		error("Unable to boot the device tethered\n");
		return -1;
	}

	debug("Initializing greenpois0n in iBoot\n");
	irecv_send_command(client, "go");

	return 0;
}

//bootarg should only be passed IF we want to specify.. otherwise, it MUST BE NULL
int execute_ibss_payload(char *bootarg) {
	//int i = 0;
	char* bootargs = bootarg;
	irecv_error_t error = IRECV_E_SUCCESS;

	debug("Initializing greenpois0n in iBSS\n");
	irecv_send_command(client, "go");

	if (bootargs == NULL) {
		// Code to detect whether to boot ramdisk or filesystem
		debug("Checking if device is already jailbroken\n");
		error = irecv_getenv(client, "boot-args", &bootargs);
		if (error != IRECV_E_SUCCESS) {
			debug("%s\n", irecv_strerror(error));
			pois0n_set_error("Unable to read env var\n");
			return -1;
		}
	}
	
	
		/* TODO: FIXME: right now it properly grabs the version but it never gets back into a state where the ramdisk can properly run
		 
		 it will get to the greenpois0n init screen and no further.
		 
		 */
		//printf("Fetching version URL...\n");
	
		//fetchVersionURL();
	
		//printf("Re-Injecting...\n");
	
		//	boot_ibec(); //still doesnt work, nor does putting in boot_ibec();
	
		// If boot-args hasn't been set then we've never been jailbroken
	if (!strcmp(bootargs, "") || !strcmp(bootargs, "0")) {
		debug("Booting jailbreak ramdisk\n");
		if (boot_ramdisk() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 1 then boot device into tethered mode
	else if (!strcmp(bootargs, "1")) {
		debug("Booting tethered device\n");
		if (boot_tethered() < 0) {
			error("Unable to boot device into tethered mode\n");
			return -1;
		}
	}
	// If boot-args is 2, then don't boot kernel, just load iBSS payload
	else if (!strcmp(bootargs, "2")) {
		debug("Booting iBSS in payload mode\n");
		return 0;
	}
	// If boot-args is 3, then don't boot kernel, just load iBoot payload
	else if (!strcmp(bootargs, "3")) {
		debug("Booting device in verbose mode\n");
		if (boot_iboot() < 0) {
			error("Unable to boot device into verbose mode\n");
			return -1;
		}
	}

	return 0;
	
}

void pois0n_init() {
	irecv_init();
	irecv_set_debug_level(libpois0n_debug);
	debug("Initializing libpois0n\n");
	pois0n_set_error("An unknown error has occured");

#ifdef __APPLE__
	system("killall -9 iTunesHelper");
#endif

#ifdef _WIN32
	system("TASKKILL /F /IM iTunes.exe > NUL");
	system("TASKKILL /F /IM iTunesHelper.exe > NUL");
#endif
}

void pois0n_set_callback(pois0n_callback callback, void* object) {
	progress_callback = callback;
	user_object = object;
}

int pois0n_is_ready_with_ecid(uint64_t ecid) {
	irecv_error_t error = IRECV_E_SUCCESS;
	
		//////////////////////////////////////
		// Begin
		// debug("Connecting to device\n");
	error = irecv_open_with_ECID(ecid,&client);
	if (error != IRECV_E_SUCCESS) {
		debug("Device must be in DFU mode to continue\n");
		return -1;
	}
	irecv_event_subscribe(client, IRECV_PROGRESS, &recovery_callback, NULL);
	
		//////////////////////////////////////
		// Check device
		// debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		pois0n_set_error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}
	
	return 0;
}

int pois0n_is_ready() {
	irecv_error_t error = IRECV_E_SUCCESS;

	//////////////////////////////////////
	// Begin
	// debug("Connecting to device\n");
	error = irecv_open(&client);
	if (error != IRECV_E_SUCCESS) {
		debug("Device must be in DFU mode to continue\n");
		return -1;
	}
	irecv_event_subscribe(client, IRECV_PROGRESS, &recovery_callback, NULL);

	//////////////////////////////////////
	// Check device
	// debug("Checking the device mode\n");
	if (client->mode != kDfuMode) {
		pois0n_set_error("Device must be in DFU mode to continue\n");
		irecv_close(client);
		return -1;
	}

	return 0;
}


plist_t loadFirmwareList() {
//	int err = 0;
//	char path[512];
//	uint32_t size = 0;
//	plist_t plist = NULL;
//	unsigned char* data = NULL;
//	
//	memset(path, '\0', sizeof(path));
//		//path = "firmware_2.plist";
//	snprintf(path, sizeof(path)-1, "firmware_2.plist"); //TODO: FIXME hardcoded to checking for firmware_2.plist in / folder only works with injectpois0n
//	err = file_read(path, &data, &size);
//	if (err < 0) {
//		fprintf(stderr, "Unable to open firmware_2.plist\n");
//		return NULL;
//	}
	return NULL;
	//plist_t plist = NULL;
//	plist_from_xml(firmware_2_plist, firmware_2_plist_len, &plist);
//		//free(data);
//	
//	return plist;
}


int pois0n_is_compatible_using_url(char *url) {
	irecv_error_t error = IRECV_E_SUCCESS;
	info("Checking if device is compatible with this jailbreak\n");
	
	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		pois0n_set_error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	if (url != NULL)
	{
		device->url_new = url;
	}
	
	
	
	
	if (device->chip_id != 8930
#ifdef LIMERA1N
		&& device->chip_id != 8922 && device->chip_id != 8920
#endif
#ifdef STEAKS4UCE
		&& device->chip_id != 8720
#endif
		) {
		pois0n_set_error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}
	
	return 0;
}

int pois0n_is_compatible() {
	irecv_error_t error = IRECV_E_SUCCESS;
	info("Checking if device is compatible with this jailbreak\n");

	debug("Checking the device type\n");
	error = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		pois0n_set_error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}
	info("Identified device as %s\n", device->product);

	if (device->chip_id != 8930
#ifdef LIMERA1N
			&& device->chip_id != 8922 && device->chip_id != 8920
#endif
#ifdef STEAKS4UCE
			&& device->chip_id != 8720
#endif
	) {
		pois0n_set_error("Sorry device is not compatible with this jailbreak\n");
		return -1;
	}

	return 0;
}

void pois0n_exit() {
	debug("Exiting libpois0n\n");
	irecv_close(client);
	irecv_exit();
}

char* pois0n_get_error() {
	return strdup(libpois0n_error);
}

int pois0n_injectonly() {
	//////////////////////////////////////
	// Send exploit
	if (device->chip_id == 8930) {

#ifdef LIMERA1N

		debug("Preparing to upload limera1n exploit\n");
		if (limera1n_exploit() < 0) {
			pois0n_set_error("Unable to upload exploit data\n");
			return -1;
		}

#else

		pois0n_set_error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8920 || device->chip_id == 8922) {

#ifdef LIMERA1N

		debug("Preparing to upload limera1n exploit\n");
		if (limera1n_exploit() < 0) {
			pois0n_set_error("Unable to upload exploit data\n");
			return -1;
		}

#else

		pois0n_set_error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8720) {

#ifdef STEAKS4UCE

		debug("Preparing to upload steaks4uce exploit\n");
		if (steaks4uce_exploit() < 0) {
			pois0n_set_error("Unable to upload exploit data\n");
			return -1;
		}

#else

		pois0n_set_error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else if (device->chip_id == 8900) {

#ifdef PWNAGE2

		debug("Preparing to upload pwnage2 exploit\n");
		if(pwnage2_exploit() < 0) {
			pois0n_set_error("Unable to upload exploit data\n");
			return -1;
		}

#else

		pois0n_set_error("Sorry, this device is not currently supported\n");
		return -1;

#endif

	}

	else {
		pois0n_set_error("Sorry, this device is not currently supported\n");
		return -1;
	}
	return 0;
}

int reinject_take_deux()
{
	irecv_error_t error = IRECV_E_SUCCESS;
	debug("Preparing to upload iBEC\n");
	if (upload_new_ibec() < 0) {
			// This sometimes returns failure even if we were successful... oh well...
			//error("Unable to upload iBSS\n");
			//return -1;
	}
	
	debug("Decrypting iBEC\n");
    error = irecv_send_command(client, "go image decrypt $loadaddr");
    if(error != IRECV_E_SUCCESS) {
		error("Unable to upload iBEC\n");
		return -1;
	}
	
    debug("Moving iBEC into place\n");
    if(device->chip_id == 8720) {
        error = irecv_send_command(client, "go memory move 0x09000040 $loadaddr 0x48000");
    } else {
        error = irecv_send_command(client, "go memory move 0x41000040 $loadaddr 0x48000");
    }
    if(error != IRECV_E_SUCCESS) {
        pois0n_set_error("Unable to append iBEC\n");
        return -1;
    }
	
	debug("Patching iBoot\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000");
	
	debug("Jumping back into iBEC\n");
	error = irecv_send_command(client, "go jump $loadaddr");
	if(error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to hook jump_to\n");
		return -1;
	}
	
	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}
	
	debug("Preparing to upload iBEC payload\n");
	if (upload_ibec_payload() < 0) {
		error("Unable to upload iBEC payload\n");
		return -1;
	}
	
	irecv_send_command(client, "go");
	
	return 0;
}

int reinject() //TODO: FIXME!! this doesn't work! :(
{
	irecv_error_t error = IRECV_E_SUCCESS;
	debug("Preparing to upload iBSS\n");
	if (upload_old_ibss() < 0) {
			// This sometimes returns failure even if we were successful... oh well...
			//error("Unable to upload iBSS\n");
			//return -1;
	}

	debug("Decrypting iBSS\n");
    error = irecv_send_command(client, "go image decrypt $loadaddr");
    if(error != IRECV_E_SUCCESS) {
		error("Unable to upload kernelcache\n");
		return -1;
	}

    debug("Moving iBSS into place\n");
    if(device->chip_id == 8720) {
        error = irecv_send_command(client, "go memory move 0x09000040 $loadaddr 0x48000");
    } else {
        error = irecv_send_command(client, "go memory move 0x41000040 $loadaddr 0x48000");
    }
    if(error != IRECV_E_SUCCESS) {
        pois0n_set_error("Unable to append kernelcache\n");
        return -1;
    }

	debug("Patching iBoot\n");
	error = irecv_send_command(client, "go patch $loadaddr 0x48000");

	debug("Jumping back into iBSS\n");
	error = irecv_send_command(client, "go jump $loadaddr");
	if(error != IRECV_E_SUCCESS) {
		pois0n_set_error("Unable to hook jump_to\n");
		return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}
	
	debug("Preparing to upload iBSS payload\n");
	if (upload_ibss_payload() < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}
	
	irecv_send_command(client, "go");
	
	return 0;
}

//Bootargs must be NULL unless we want to specify the boot type
int pois0n_inject(char *bootargs) {
	if (pois0n_injectonly() < 0) {
		error("Unable to inject exploit");
		return -1;
	}

	//////////////////////////////////////
	// Send iBSS
	debug("Preparing to upload iBSS\n");
	if (upload_old_ibss() < 0) {
		// This sometimes returns failure even if we were successful... oh well...
		//error("Unable to upload iBSS\n");
		//return -1;
	}

	debug("Reconnecting to device\n");
	client = irecv_reconnect(client, 10);
	if (client == NULL) {
		error("Unable to reconnect\n");
		return -1;
	}

	debug("Preparing to upload iBSS payload\n");
	if (upload_ibss_payload() < 0) {
		error("Unable to upload iBSS payload\n");
		return -1;
	}

	debug("Executing iBSS payload\n");
	if (execute_ibss_payload(bootargs) < 0) {
		error("Unable to execute iBSS payload\n");
		return -1;
	}

	return 0;
}
