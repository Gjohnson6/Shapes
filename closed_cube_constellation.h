#pragma once
#include "constellation.h"

class ClosedCubeConstellation : public Constellation
{
public:
	ClosedCubeConstellation();

	bool Initialize(int number_of_objects);
};

