/**
  * GreenPois0n Medicine - animate.mm
  * Copyright (C) 2011 Chronic-Dev Team
  * Copyright (C) 2011 Nicolas Haunold
  * Copyright (C) 2011 Justin Williams
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

#include <Foundation/Foundation.h>
#include <CoreGraphics/CoreGraphics.h>
#include <sys/sysctl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <IOKit/IOKitLib.h>
#include <mach/mach_traps.h>
#include "animate_frames.h"

int screenWidth, screenHeight;

CGContextRef fb_open() {
	io_connect_t conn = NULL;
	int bytesPerRow;
	void *surfaceBuffer;
	void *frameBuffer;
	CGContextRef context = NULL;
	CGColorSpaceRef colorSpace;

	void *mfb_lib = dlopen("/System/Library/PrivateFrameworks/IOMobileFramebuffer.framework/IOMobileFramebuffer", 2);
	void *cs_lib = dlopen("/System/Library/PrivateFrameworks/CoreSurface.framework/CoreSurface", 2);

	int (*mfb_open)(io_service_t, task_port_t, uint32_t, io_connect_t*);
	int (*mfb_layr)(io_service_t, int, void *);
	int (*cs_lock)(void *, unsigned int);
	int (*cs_unlock)(void *);
	int (*cs_height)(void *);
	int (*cs_width)(void *);
	int (*cs_bytes)(void *);
	void *(*cs_addr)(void *);

	*(void **)(&mfb_open) = dlsym(mfb_lib, "IOMobileFramebufferOpen");
	*(void **)(&mfb_layr) = dlsym(mfb_lib, "IOMobileFramebufferGetLayerDefaultSurface");
	*(void **)(&cs_lock) = dlsym(cs_lib, "CoreSurfaceBufferLock");
	*(void **)(&cs_unlock) = dlsym(cs_lib, "CoreSurfaceBufferUnlock");
	*(void **)(&cs_height) = dlsym(cs_lib, "CoreSurfaceBufferGetHeight");
	*(void **)(&cs_width) = dlsym(cs_lib, "CoreSurfaceBufferGetWidth");
	*(void **)(&cs_bytes) = dlsym(cs_lib, "CoreSurfaceBufferGetBytesPerRow");
	*(void **)(&cs_addr) = dlsym(cs_lib, "CoreSurfaceBufferGetBaseAddress");

	io_service_t fb_service = IOServiceGetMatchingService(kIOMasterPortDefault, IOServiceMatching("AppleCLCD"));
	if(!fb_service) {
		printf("Couldn't find framebuffer.\n");
		return NULL;
	}

	(*mfb_open)(fb_service, mach_task_self(), 0, &conn);
	(*mfb_layr)(conn, 0, &surfaceBuffer);

	screenHeight = (*cs_height)(surfaceBuffer);
	screenWidth = (*cs_width)(surfaceBuffer);
	bytesPerRow = (*cs_bytes)(surfaceBuffer);

	(*cs_lock)(surfaceBuffer, 3);
	frameBuffer = (*cs_addr)(surfaceBuffer);
	(*cs_unlock)(surfaceBuffer);

	// create bitmap context
	colorSpace = CGColorSpaceCreateDeviceRGB();
	context = CGBitmapContextCreate(frameBuffer, screenWidth, screenHeight, 8, bytesPerRow, colorSpace, kCGImageAlphaPremultipliedLast);
	if(context == NULL) {
		printf("Couldn't create screen context!");
		return NULL;
	}

	CGColorSpaceRelease(colorSpace);

	return context;
}

int main(int argc, char **argv, char **envp) {
	NSAutoreleasePool *p = [[NSAutoreleasePool alloc] init];
	NSLog(@"Welcome to animate...");
	NSMutableArray *arr = [[NSMutableArray alloc] init];

	anim_sequence *sp = seq;
	while (sp->data != NULL) {
		printf("Adding Image...");
		CGDataProviderRef dpr = CGDataProviderCreateWithData(NULL, sp->data, sp->size, NULL);
		CGImageRef img = CGImageCreateWithPNGDataProvider(dpr, NULL, true, kCGRenderingIntentDefault);
		[arr addObject:(id)img];
		CGDataProviderRelease(dpr);
		sp++;
	}

	printf("Sleeping...");
	sleep(1);

 	CGContextRef c = fb_open();
	int i;
	for(i = 0; i < [arr count]; i++) {
		printf("Displaying Image...");
		CGImageRef bootimg = (CGImageRef)[arr objectAtIndex:i];
		CGContextDrawImage(c, CGRectMake(0, 0, screenWidth, screenHeight), bootimg);
	}
	[arr release];
	[p drain];
}

// vim:ft=objc
