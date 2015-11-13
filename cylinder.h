#pragma once
#include "shape.h"

#if 0

class Cylinder : public Shape
{
public:
	Cylinder(int slices, int stacks, float span, float back_radius = 1.0f, float front_radius = 1.0f);

protected:
	bool PreGLInitialize();
	void NonGLTakeDown();
	void RecomputeNormals();

	int slices;
	int stacks;
	float span;
	float fr;
	float br;
	bool is_partial_span;
};

#endif