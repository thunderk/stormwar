SUBDIRS = src data m4

if DIST_W32
EXTRA_DIST = config/config.rpath stormwar.prj TODO docs doxyconfig Makefile.w32 config_w32.h Makefile.OSX config_osx.h BUGS stormwar.spec.in StormWar.exe jpeg.dll libpng13.dll SDL.dll SDL_image.dll SDL_mixer.dll zlib1.dll
else
EXTRA_DIST = config/config.rpath stormwar.prj TODO docs doxyconfig Makefile.w32 config_w32.h Makefile.OSX config_osx.h BUGS stormwar.spec.in
endif

ACLOCAL_AMFLAGS = -I m4

CLEANFILES = docs/html/* $(PACKAGE)-*.tar.gz $(PACKAGE)-*.tar.bz2 $(PACKAGE)-*.zip

# Rule to build RPM distribution package
rpm:
	make dist && cp $(PACKAGE)-$(VERSION).tar.gz /usr/src/redhat/SOURCES && rpmbuild -ba stormwar.spec

# Rule to update the win32 exe
win32:
	make clean && rm -rf /tmp/$(PACKAGE)-buildroot && cp -r . /tmp/$(PACKAGE)-buildroot && cd /tmp/$(PACKAGE)-buildroot && make -f Makefile.w32 && cd - && cp /tmp/$(PACKAGE)-buildroot/StormWar.exe . && rm -rf /tmp/$(PACKAGE)-buildroot

# Clean for release
dist-hook:
	rm -f $(CLEANFILES)

# To make the doc
doc:
	doxygen doxyconfig
