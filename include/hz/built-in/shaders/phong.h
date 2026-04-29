#pragma once

#include <hz/program.h>

#define HZ_PHONG_SHADER_NUM_LIGHTS	10

hz_program *hz_phong_program_new(hz_arena *arena);
