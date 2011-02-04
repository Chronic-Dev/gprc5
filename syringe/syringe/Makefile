# /syringe/Makefile

SUBDIRS = exploits

MKSTATICLIB = 1

INCLUDES = ../include ../include/resources

SOURCES = \
	common.c		\
	libirecovery.c	\
	libpartial.c	\
	libpois0n.c		\

PREMADE_OBJECTS = exploits/limera1n.o exploits/steaks4uce.o

LIBRARIES = usb-1.0 curl z
LDFLAGS = -L/opt/local/lib  -mmacosx-version-min=10.5 -arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk
CFLAGS = -mmacosx-version-min=10.5 -arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk

TARGET = syringe

include ../common.mk
