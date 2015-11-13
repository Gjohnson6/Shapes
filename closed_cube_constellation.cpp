#include "closed_cube_constellation.h"

using namespace glm;

ClosedCubeConstellation::ClosedCubeConstellation()
{
}



bool ClosedCubeConstellation::Initialize(int number_of_objects)
{
	int size = cbrt(number_of_objects);

	this->Clear();
	this->Resize(size * size * size);
	float delta = 2.0f / (size - 1.0f);

	int positionIndex = 0;
	vec3 position(-1.0f, -1.0f, -1.0f);

	for (int z = 0; z < size; z++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				this->positions[positionIndex] = PositionData(position, vec3(sin(position.x * 2.0f * glm::pi<float>()), 0.0f, cos(position.x * 2.0f * glm::pi<float>())));
				positionIndex++;

				position.x += delta;
			}
			position.x = -1.0f;
			position.y += delta;
		}
		position.y = -1.0f;
		position.z += delta;
	}
	return true;
}

