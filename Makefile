V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew assimp

objects := \
	main.o \
	src/loaders/assimp.o \
	src/loader.o \
	src/scene.o \
	src/object.o \
	src/camera.o \
	src/objects/basic.o \
	src/cameras/fly.o \
	src/helpers/shaders.o \
	src/helpers/textures.o \
	src/helpers/io.o \
	src/adt/list.o \
	src/misc/stb_image.o \
	utils/files.o \
	utils/glfw_context.o \
	utils/log.o \

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
