V ?= @

include scripts/build-utils.mk

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

hertz-so	:= libhertz.so
hertz-so-name	:= $(notdir $(hertz-so))

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

to-linker += $(hertz-so)
to-remove += $(hertz-objects) $(hertz-so)
rebuild-deps += $(hertz-objects)

$(hertz-so): LDFLAGS += -shared -Wl,-soname,$(hertz-so-name)
$(hertz-so): CFLAGS  += -fPIC

phony += all
all: $(hertz-so)

$(hertz-so): $(hertz-objects)

include examples/01-assimp/Makefile

$(rebuild-deps): $(MAKEFILE_LIST)

to-remove += $(example-programs)
to-linker += $(example-programs)

phony += examples
examples: $(hertz-so) $(example-programs)

%.o : %.c
	$(ECHO) "CC" "$@"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(to-linker):
	$(ECHO) "LD" "$@"
	$(V)$(LD) $(LDFLAGS) $^ -o $@

phony += install
install: $(hertz)
	install -d $(PREFIX)/include
	install -d $(PREFIX)/lib
	install -m 644 $(hertz-so) $(PREFIX)/lib/
	cp -r include/hz $(PREFIX)/include/

phony += uninstall
uninstall:
	rm -f $(PREFIX)/lib/$(hertz-so-name)
	rm -rf $(PREFIX)/include/hz

phony += clean
clean:
	$(ECHO) "RM" "$(to-remove)"
	$(V)$(RM) $(to-remove)

.PHONY: $(phony)
