V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew assimp

objects := \
	assets/assimp.o \
	assets/loader.o \
	assets/stb_image.o \
	main.o \
	render/render.o \
	render/scene.o \
	cameras/fly_camera.o \
	utils/gl/shaders.o \
	utils/gl/textures.o \
	utils/gl/io.o \
	utils/files.o \
	utils/glfw_context.o \
	objects/mesh.o \
	utils/list.o \
	utils/log.o

LDFLAGS	:= $(shell pkg-config --libs $(libs)) -lm
CFLAGS	:= $(shell pkg-config --cflags $(libs)) -Wall -I. -g

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
