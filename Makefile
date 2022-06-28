include scripts/build-utils.mk

CROSS_COMPILE ?=

$(call allow-override,CC,gcc)
$(call allow-override,LD,gcc)

CC	:= $(CROSS_COMPILE)$(CC)
LD	:= $(CROSS_COMPILE)$(LD)
PKG_CONFIG ?= $(CROSS_COMPILE)pkg-config

ECHO	:= @ printf "[%s]\t%s\n"
PREFIX	?= /usr/local

GL_HEADER ?= <GL/gl.h>
GL_EXTENSIONS_HEADER ?= <GL/glew.h>

CFLAGS += -Wall -g -I. -Iinclude \
	-I$(PREFIX)/include \
	-DHZ_GL_HEADER="$(GL_HEADER)" \
	-DHZ_GL_EXTENSIONS_HEADER="$(GL_EXTENSIONS_HEADER)"

hertz-abi	:= 1
hertz-version	:= $(hertz-abi).0.0
hertz-ldname	:= libhertz.so
hertz-realname	:= $(hertz-ldname).$(hertz-version)
hertz-soname	:= $(hertz-ldname).$(hertz-abi)

hertz-target	:= $(hertz-realname)

hertz-objects := \
	hertz/object.o \
	hertz/material.o \
	hertz/objects/camera.o \
	hertz/objects/mesh.o \
	hertz/objects/light.o \
	hertz/objects/root.o \
	hertz/actors/fly.o \
	hertz/logger.o \
	hertz/built-in/shaders/simple.o \
	hertz/helpers/shaders.o \
	hertz/helpers/binders.o \
	hertz/helpers/textures.o \
	hertz/adt/list.o \
	hertz/adt/tree.o

to-linker += $(hertz-target)
to-remove += $(hertz-objects) $(hertz-target)
rebuild-deps += $(hertz-objects)

$(hertz-target): LDFLAGS += -shared -Wl,-soname,$(hertz-soname)
$(hertz-target): CFLAGS  += -fPIC

phony += all
all: $(hertz-target)

$(hertz-target): $(hertz-objects)

include examples/01-assimp/Makefile

$(rebuild-deps): $(MAKEFILE_LIST)

to-remove += $(example-programs)
to-linker += $(example-programs)

phony += examples
examples: $(hertz-target) $(example-programs)

%.o : %.c
	$(ECHO) "CC" "$@"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(to-linker):
	$(ECHO) "LD" "$@"
	$(V)$(LD) $^ $(LDFLAGS) -o $@

phony += install
install: $(hertz-target)
	install -d $(PREFIX)/include
	install -d $(PREFIX)/lib
	install -m 644 $(hertz-target) $(PREFIX)/lib/
	ln -sf $(PREFIX)/lib/$(hertz-realname) $(PREFIX)/lib/$(hertz-soname)
	ln -sf $(PREFIX)/lib/$(hertz-realname) $(PREFIX)/lib/$(hertz-ldname)
	cp -r include/hz $(PREFIX)/include/

phony += uninstall
uninstall:
	rm -f $(PREFIX)/lib/$(hertz-realname)
	rm -f $(PREFIX)/lib/$(hertz-soname)
	rm -f $(PREFIX)/lib/$(hertz-ldname)
	rm -rf $(PREFIX)/include/hz

phony += clean
clean:
	$(ECHO) "RM" "$(to-remove)"
	$(V)$(RM) $(to-remove)

.PHONY: $(phony)
