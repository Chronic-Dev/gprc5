CFLAGS=-I../../syringe/include -I/opt/local/include -mmacosx-version-min=10.6 -arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk
LDFLAGS=-L../../syringe/syringe -L/opt/local/lib -lsyringe -lcurl -lz -lxml2 -framework CoreFoundation -framework IOKit -framework Cocoa -mmacosx-version-min=10.6 -arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk

all:
	@echo "Building Mac OS X GUI"
	mkdir -p greenpois0n.app/Contents/MacOS
	$(CC) $(CFLAGS) $(LDFLAGS) main.m appFunctions.m GPUSLDeviceManager.m -o greenpois0n.app/Contents/MacOS/greenpois0n ../../syringe/lib-static/macosx/libusb-1.0.a ../../syringe/lib-static/macosx/libplist.a ../../syringe/lib-static/macosx/libimobiledevice.a ../../syringe/lib-static/macosx/libimobiledevice.a ../../syringe/lib-static/macosx/libgnutls.a ../../syringe/lib-static/macosx/libgnutlsxx.a ../../syringe/lib-static/macosx/libgpg-error.a ../../syringe/lib-static/macosx/libusbmuxd.a ../../syringe/lib-static/macosx/libgcrypt.a
	
clean:
	@echo "Cleaning Mac OS X GUI"
	rm -rf greenpois0n.app/Contents/MacOS/greenpois0n
