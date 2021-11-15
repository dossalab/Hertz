V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew

objects := \
	init/main.o \
	counters/time.o \
	scene/scene.o \
	cameras/fly_camera.o \
	loaders/stb_image.o \
	loaders/obj_loader.o \
	loaders/model.o \
	utils/gl/shaders.o \
	utils/gl/textures.o \
	utils/files.o \
	utils/glfw_ctx_helper.o \
	objects/mesh.o \
	errors/errors.o \
	utils/list.o \
	logger/logger.o

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
