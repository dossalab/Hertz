#pragma once

/*
 * This types must match the ones from linmath.h
 * linmath.h in general should not (and at this moment can not) be exposed to
 * the end user. However we use linmath.h types and functions internally
 */

#define HZ_TYPE_DEFINE_VEC(n) \
	typedef float hz_vec##n[n]

HZ_TYPE_DEFINE_VEC(2);
HZ_TYPE_DEFINE_VEC(3);
HZ_TYPE_DEFINE_VEC(4);

typedef hz_vec4 hz_mat4x4[4];
typedef hz_vec4 hz_quat;
