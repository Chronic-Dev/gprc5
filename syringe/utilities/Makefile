# /syringe/Makefile

SUBPROJS = injectpois0n tetheredboot loadibec
LDFLAGS = -L/usr/lib -L. -I/usr/include -lz -lcurl -framework CoreFoundation  -mmacosx-version-min=10.5 -arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk
CFLAGS =  -mmacosx-version-min=10.5 -arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk
	
include ../common.mk
