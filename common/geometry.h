#ifndef GEOMETRY_H
#define GEOMETRY_H

struct 3dpoint {
	float x, y, z;
};

struct vertex {
	struct 3dpoint points[3];
};

#endif
