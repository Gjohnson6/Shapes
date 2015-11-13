#pragma once
#include "shape.h"

class Torus : public Shape
{
public:
	Torus(float innerRadius, float outerRadius, int sides, int rings);
protected:
	bool PreGLInitialize();
	void NonGLTakeDown();
	void RecomputeNormals();

	float inner_radius;
	float outer_radius;
	int sides;
	int rings;
};

