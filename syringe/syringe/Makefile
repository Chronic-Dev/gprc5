# /syringe/Makefile

SUBDIRS = exploits

MKSTATICLIB = 1

INCLUDES = ../include ../include/resources

SOURCES = \
	common.c		\
	libirecovery.c	\
	libpartial.c	\
	libpois0n.c		\
	libdioxin.c		\
	patch.c \
	functions.c \

XPLAT_CFLAGS = -O3 -DCURL_STATICLIB=1
PLAT_CFLAGS = 
XPLAT_LIBRARIES = curl z bz2
PLAT_LIBRARIES =
XPLAT_PREMADE_OBJECTS = exploits/limera1n.o exploits/steaks4uce.o
PLAT_PREMADE_OBJECTS =

include ../platform.mk
ifeq ($(UNAME),Darwin)
	# OS X
	PLAT_CFLAGS = -I/usr/include -I/opt/local/include -arch i386 -mmacosx-version-min=10.7 -isysroot /Developer/SDKs/MacOSX10.7.sdk -framework IOKit
	PLAT_LIBRARIES =
	LDFLAGS = -L/usr/lib -framework CoreFoundation -framework IOKit -mmacosx-version-min=10.7 -arch i386 -isysroot /Developer/SDKs/MacOSX10.7.sdk -fnested-functions
	ADDOBJ = ../lib-static/macosx/libusb-1.0.a ../lib-static/macosx/libplist.a ../lib-static/macosx/libimobiledevice.a ../lib-static/macosx/libgnutls.a ../lib-static/macosx/libgnutlsxx.a ../lib-static/macosx/libgpg-error.a ../lib-static/macosx/libusbmuxd.a ../lib-static/macosx/libgcrypt.a ../lib-static/macosx/libcurl.a
else
	ifneq (,$(findstring MINGW32_NT,$(UNAME)))
		# Win32
		LDFLAGS =
		ADDOBJ = /mingw/lib/libcurl.a /mingw/lib/libwsock32.a /mingw/lib/libwldap32.a /mingw/lib/libpenwin32.a /mingw/lib/libz.a /mingw/lib/libsetupapi.a
	else
		# Linux
		PLAT_LIBRARIES = usb-1.0
		LDFLAGS = -L/opt/local/lib
	endif
endif
PREMADE_OBJECTS = $(XPLAT_PREMADE_OBJECTS) $(PLAT_PREMADE_OBJECTS)
LIBRARIES = $(XPLAT_LIBRARIES) $(PLAT_LIBRARIES)
CFLAGS = $(XPLAT_CFLAGS) $(PLAT_CFLAGS)

TARGET = syringe

include ../common.mk
