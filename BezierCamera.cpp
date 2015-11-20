#include "BezierCamera.h"
#include <random>
#include <iostream>
#include "disc.h"
#include "phong_shader.h"
#include "glm\glm.hpp"
#include <time.h>

BezierCamera::BezierCamera()
{
	srand((time(NULL)));
	tPoints.resize(101);
	for (int i = 0; i < 4; i++)
	{
		currentSpline[i].z = rand() % 150 - 75.f;
		currentSpline[i].x = rand() % 150 - 75.f;
		currentSpline[i].y = rand() % 150 - 75.f;
	}

	//This will start the random path
	generateNextSpline();
}


BezierCamera::~BezierCamera()
{
}

//Randomly generates the next spline
void BezierCamera::generateNextSpline()
{
	//The first point of a spline is always the last point of the previous spline
	nextSpline[0] = currentSpline[3];

	//The second point must be opposite the third point of current spline
	nextSpline[1] = -(currentSpline[2] - currentSpline[3]) + nextSpline[0];

	//The third and fourth points are random.
	float xVal = rand() % 150 - 75.f;
	float yVal = rand() % 150 - 75.f;
	float zVal = rand() % 150 - 75.f;
	nextSpline[2] = vec3(xVal, yVal, zVal);

	xVal = rand() % 150 - 75.f;
	yVal = rand() % 150 - 75.f;
	zVal = rand() % 150 - 75.f;
	nextSpline[3] = vec3(xVal, yVal, zVal);
}

//This finds 50 points along the bezier curve, which are used for finding the camera's position and for displaying the curve
void BezierCamera::findtValues()
{
	for (int i = 0; i <= 50; i++)
	{
		littleT = i / 50.f;
		tPoints.at(i) = deCastleJau(currentSpline[0], currentSpline[1], currentSpline[2], currentSpline[3], littleT, vec3(0,0,0));
		tPoints.at(i + 50) = deCastleJau(nextSpline[0], nextSpline[1], nextSpline[2], nextSpline[3], littleT, vec3(0,0,0));
	}
	littleT = 0.f;
}

//Using t, find how far alone the bezier curve the camera is. lookat is set to p5 because it's on a tangent to the curve
vec3 BezierCamera::deCastleJau(vec3 & firstPoint, vec3 & secondPoint, vec3 & thirdPoint, vec3 & fourthPoint, float t, vec3 & lookat)
{
	vec3 p1 = lerp(firstPoint, secondPoint, t);
	vec3 p2 = lerp(secondPoint, thirdPoint, t);
	vec3 p3 = lerp(thirdPoint, fourthPoint, t);
							 
	vec3 p4 = lerp(p1, p2, t);
	vec3 p5 = lerp(p2, p3, t);
	lookat = p5;
								 
	vec3 p6 = this->lerp(p4, p5, t);
	return p6;
}

//Returns all the 
vector<vec3> BezierCamera::GetTPoints()
{
	findtValues();
	return tPoints;
}

//Linear interpolation between two points
vec3 BezierCamera::lerp(vec3 & p1, vec3 & p2, float & t)
{
	vec3 pos = p1 + (p2 - p1) * t;
	return pos;
}

vec3* BezierCamera::GetSpline()
{
	return nextSpline;
}

//This function updates the camera position and returns it and the point where the camera will be looking
vec3 BezierCamera::GetCameraPosition(vec3& lookat)
{
	cameraPreviousPos = cameraPos;
	//50 because that's how many segments there are in the spline
	double distances[100] = { 0 };//This will contain the distances of points from the first point in the current spline
	double dividedDistances[100] = { 0 };
	double distance;
	double distanceSum = 0.f;
	vec3 diff;
	for (int i = 0; i < 99; i++)
	{
		diff = tPoints[i + 1] - tPoints[i];
		distance = sqrt(dot(diff, diff));
		distances[i + 1] = distance + distances[i];
	}
	float bigTDelta = .05f / distances[50];//bigTDelta is how far along the spline the camera will move per frame. .05f is how many units we want the camera to move.
	for (int i = 0; i < 100; i++)
	{
		dividedDistances[i] = distances[i] / distances[50];
	}

	//Goes through each point, if the point is past bigT, then that point's distance is tAfter and the point before's distance is tBefore
	for (int i = 0; i < 99; i++)
	{
		if (bigT < dividedDistances[i])
		{
			double tBefore = dividedDistances[i - 1];
			double tAfter = dividedDistances[i];
			double numerator = bigT - (bigT - bigTDelta);
			double denominator = dividedDistances[i] - dividedDistances[i - 1];

			float t = (bigT / denominator) * (tAfter - tBefore);
			cameraPos = deCastleJau(currentSpline[0], currentSpline[1], currentSpline[2], currentSpline[3], t, lookat);
			break;
		}
	}

	bigT += bigTDelta;
	if (bigT >= 1.0f)//If bigT has gone past 1.0, then the camera has moved into the next spline.
	{
		currentSpline[0] = nextSpline[0];
		currentSpline[1] = nextSpline[1];
		currentSpline[2] = nextSpline[2];
		currentSpline[3] = nextSpline[3];
		generateNextSpline();
		bigT = 0.0f;//This shouldn't bee 0.0f
	}
	return cameraPos;
}

