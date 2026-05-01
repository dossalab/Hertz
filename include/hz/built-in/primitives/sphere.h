#pragma once

#include <hz/node.h>
#include <hz/material.h>
#include <hz/program.h>

hz_node *hz_sphere_new(hz_arena *arena, hz_program *program, hz_material *material, unsigned quality);
