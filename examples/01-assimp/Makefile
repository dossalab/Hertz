this-dir := $(call my-dir)

assimp-example-exe := $(this-dir)assimp
assimp-example-objects := \
	$(this-dir)main.o \
	$(this-dir)../utils/glfw_context.o \
	$(this-dir)assimp.o \
	$(this-dir)../utils/stb_image.o

assimp-example-libs := glfw3 glew assimp

$(assimp-example-exe): LDFLAGS += \
	$(shell $(PKG_CONFIG) -libs $(assimp-example-libs)) -lm -lhertz
$(assimp-example-exe): CFLAGS += \
	$(shell $(PKG_CONFIG) -cflags $(assimp-example-libs))

$(assimp-example-exe): $(assimp-example-objects)

rebuild-deps += $(assimp-example-objects)
to-remove += $(assimp-example-objects)
example-programs += $(assimp-example-exe)
