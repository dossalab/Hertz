V ?= @

CC	:= gcc
LD	:= gcc
ECHO	:= @ printf "[%s]\t%s\n"

exe	:= opengl
libs	:= glfw3 glew

objects := \
	app/main.o \
	engine/scene.o \
	engine/mesh.o \
	loaders/stb_image.o \
	loaders/obj_loader.o \
	gl/utils/shaders.o \
	utils/files.o \
	errors/errors.o \
	utils/list.o \
	logger/logger.o

LDFLAGS	:= $(shell pkg-config --libs $(libs)) -lm -O3
CFLAGS	:= $(shell pkg-config --cflags $(libs)) -Wall -I. -g -O3

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
