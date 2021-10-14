V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew

objects := \
	app/main.o \
	app/time.o \
	engine/scene.o \
	loaders/stb_image.o \
	loaders/obj_loader.o \
	loaders/model.o \
	gl/mesh.o \
	gl/shaders.o \
	gl/textures.o \
	utils/files.o \
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
