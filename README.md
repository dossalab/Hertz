## Hertz, the embeddable OpenGL renderer (WIP)

This started as a small project to play with OpenGL, and over time it was separated into a drop-in library, allowing to add 3D rendering to any OpenGL-enabled environment.

### Features (some are implemented, some are goals to be achieved):

- Low boilerplate - built-in shaders, objects, cameras, scene graph, allowing to quickly start application development;
- Low overhead and mostly static memory allocation - most primitives are intended to be stack-allocated, eliminating leak possibilities and providing flexibility;
- Easy to drop-in - library has simple API, so it should be compatible with existing projects;
- No external dependencies - library is context-agnostic, meaning that with a couple of tweaks it could be used with any context. So far we tried GLFW + GLEW and GTK3.
