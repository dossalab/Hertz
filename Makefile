V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"
PREFIX	?= /usr/local

CFLAGS += -Wall -g -I. -Iinclude \
	-DGL_EXTENSIONS_HEADER="<GL/glew.h>"

hertz-so := libhertz.so
hertz-objects := \
	hertz/loaders/assimp.o \
	hertz/loader.o \
	hertz/scene.o \
	hertz/object.o \
	hertz/material.o \
	hertz/camera.o \
	hertz/objects/mesh.o \
	hertz/objects/light.o \
	hertz/cameras/fly.o \
	hertz/logger.o \
	hertz/built-in/shaders/simple.o \
	hertz/helpers/shaders.o \
	hertz/helpers/binders.o \
	hertz/helpers/textures.o \
	hertz/adt/list.o \
	hertz/adt/tree.o \
	hertz/misc/stb_image.o

to-remove += $(hertz-objects) $(hertz-so)

$(hertz-so): LDFLAGS += -shared
$(hertz-so): CFLAGS  += -fPIC

example-exe	:= example/example
example-libs	:= glfw3 glew assimp
example-objects := \
	example/main.o \
	example/glfw_context.o \

$(example-exe): LDFLAGS += $(shell pkg-config --libs $(example-libs)) -L. -lm -lhertz
$(example-exe): CFLAGS  += $(shell pkg-config --cflags $(example-libs))

to-remove += $(example-objects) $(example-exe)

phony += all
all: $(hertz-so)

phony += examples
examples: $(hertz-so) $(example-exe)

%.o : %.c
	$(ECHO) "CC" "$@"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(hertz-so): $(hertz-objects)
	$(ECHO) "LD" "$@"
	$(V)$(LD) $(LDFLAGS) $^ -o $@

$(example-exe): $(example-objects)
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
	rm -f $(PREFIX)/lib/$(notdir $(hertz-so))
	rm -rf $(PREFIX)/include/hz

phony += clean
clean:
	$(ECHO) "RM" "$(to-remove)"
	$(V)$(RM) $(to-remove)

.PHONY: $(phony)
