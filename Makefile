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
	utils/gl/shaders.o \
	utils/gl/textures.o \
	utils/gl/io.o \
	utils/files.o \
	utils/glfw_context.o \
	utils/list.o \
	utils/log.o \
	utils/stb_image.o

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
