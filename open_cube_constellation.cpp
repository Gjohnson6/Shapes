#include "open_cube_constellation.h"

using namespace glm;

OpenCubeConstellation::OpenCubeConstellation()
{
}


bool OpenCubeConstellation::Initialize(int number_of_objects)
{
	//First the number_of_objects needs to be converted into the size of the cube.
	//In closed_cube_constellation this is simply done by finding the cube root of number_of_objects
	//but because open root doesn't have points in the center of the cube, that allows the size to be larger than the cube root.
	//For example, if number_of_objects is 511, then the size of closed_cube_constellation is 7,
	//which creates a 7x7x7 cube that consists of points and 168 points are "lost" because adding them anywhere would create a non-uniform cube.
	//The size of open_cube_constellation, however, is 10, which creates a 10x10x10 cube with nothing in the center, which consists of
	//488 points and 23 points are "lost".

	//The equation for finding how many points an open_cube_constellation for a given size is 6n^2 - 12n + 8
	//This doesn't work for 1, so there needs to be a special case.
	int size = 1;//Size will be the height, width, and depth of the cube
	int pointCount = 0;//pointCount is the amount of points in the cube
	if (number_of_objects != 1)//size will stay 1 if number_of_objects is 1.
	{
		for (int i = 2; i <= 11; i++)//Goes up to 11 because 10 is the highest possible size assuming the limit is 512
		{
			pointCount = (6 * pow(i, 2.0) - (12 * i) + 8);
			if (pointCount > number_of_objects)//If amount of objects in a cube with size i is greater than the number of objects passed in, then the size is i - 1. If it's equal, then size is i.
			{
				size = i - 1;
				pointCount = (6 * pow(i - 1, 2.0) - (12 * (i - 1)) + 8);	
				break;
			}
			else if (pointCount == number_of_objects)
			{
				size = i;
				break;
			}
		}
	}

	this->Clear();
	this->Resize(pointCount);
	float delta = 2.0f / (size - 1.0f);

	int positionIndex = 0;
	vec3 position(-1.0f, -1.0f, -1.0f);

	for (int z = 0; z < size; z++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int x = 0; x < size; x++)
			{
				if (!(z > 0 && z < size - 1 && x > 0 && x < size - 1 && y > 0 && y < size - 1))
				{
					this->positions[positionIndex] = PositionData(position, vec3(sin(position.x * 2.0f * glm::pi<float>()), 0.0f, cos(position.x * 2.0f * glm::pi<float>())));
					positionIndex++;
				}
				position.x += delta;
			}
			position.x = -1.0f;
			position.y += delta;
		}
		position.y = -1.0f;
		position.z += delta;
	}
	/*
	// number_of_objects is a single paramter as (for now) the function call is generic. So,
	// synthesize a reasonable row and column count.
	int size = int(sqrt(number_of_objects));
	this->Clear();
	this->Resize(size * size);
	// The grid is designed to be over a -1 to 1 square. The consumer can scale these as desired.


	vec3 p(-1.0f, -1.0f, 0.0f);

	int i, y;
	for (y = 0, i = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			this->positions[i++] = PositionData(p, vec3(sin(p.x * 2.0f * glm::pi<float>()), 0.0f, cos(p.x * 2.0f * glm::pi<float>())));
			p.x += delta;
		}
		p.x = -1.0f;
		p.y += delta;
	}*/
	return true;
}
