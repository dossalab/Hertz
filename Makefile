CC = gcc
LD = gcc

exe = opengl
webasm = opengl.js

obj += $(addprefix core/, main.o)
obj += $(addprefix engine/, engine.o loader.o camera.o scene.o mesh.o shader.o util.o stb_image.o events.o)

CFLAGS += -I. -Wall -g
LDFLAGS += -lglfw -lGL -lm -g

# common targets
emscripten_files = $(webasm:.js=.wasm) $(webasm:.js=.data)

all: $(exe)
webasm: $(webasm)

clean:
	$(RM) $(exe) $(page) $(emscripten_files) $(obj)

%.o : %.c
	@ echo -e "[CC]\t$@"
	@ $(CC) $(CFLAGS) -c $^ -o $@

# regular gcc build
$(exe): $(obj)
	@ echo -e "[LD]\t$@"
	@ $(LD) $(LDFLAGS) $^ -o $@

# emscripten build
resources := shaders res

$(webasm): CC = emcc
$(webasm): LD = emcc
$(webasm): CFLAGS += -DBUILD_WEBGL
$(webasm): LDFLAGS += -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1
$(webasm): LDFLAGS += $(addprefix --embed-file , $(resources))
$(webasm): LDFLAGS += -s ALLOW_MEMORY_GROWTH=1

$(webasm): $(obj)
	@ echo -e "[PAGE]\t$@"
	@ $(LD) $(LDFLAGS) $^ -o $@


.PHONY: all webasm clean

