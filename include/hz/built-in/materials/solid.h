#pragma once

#include <hz/material.h>
#include <hz/util/arena.h>

hz_material *hz_solid_material_new(hz_arena *arena, float r, float g, float b, float shininess);
