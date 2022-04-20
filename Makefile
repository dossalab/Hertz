V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew assimp

objects := \
	example/main.o \
	example/glfw_context.o \
	hertz/loaders/assimp.o \
	hertz/loader.o \
	hertz/scene.o \
	hertz/object.o \
	hertz/camera.o \
	hertz/objects/basic.o \
	hertz/cameras/fly.o \
	hertz/logger.o \
	hertz/built-in/shaders/simple.o \
	hertz/helpers/shaders.o \
	hertz/helpers/textures.o \
	hertz/adt/list.o \
	hertz/misc/stb_image.o \

LDFLAGS	:= $(shell pkg-config --libs $(libs)) -lm
CFLAGS	:= $(shell pkg-config --cflags $(libs)) -Wall -I. -g -Iinclude \
	-DGL_EXTENSIONS_HEADER="<GL/glew.h>"

all: $(exe)

%.o : %.c
	$(ECHO) "CC" "$@"
	$(V)$(CC) $(CFLAGS) -c $< -o $@

$(exe): $(objects)
	$(ECHO) "LD" "$@"
	$(V)$(LD) $(LDFLAGS) $^ -o $@

clean:
	$(ECHO) "RM" "$(objects) $(exe)"
	$(V)$(RM) $(objects) $(exe)

.PHONY: all clean
