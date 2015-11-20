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
	//Begin with the camera moving forward starting at positiion 0.0f, 0.0f, 150.0f
	//All 4 points in the current spline will be in a line on the z axis with 5 units between them.
	tPoints.resize(101);
	for (int i = 0; i < 4; i++)
	{
		currentSpline[i].z = rand() % 150 - 75.f;
		currentSpline[i].x = rand() % 150 - 75.f;
		currentSpline[i].y = rand() % 150 - 75.f;
	}

	//This will start the random path
	generateNextSpline();
	//displayCurve();
}


BezierCamera::~BezierCamera()
{
}

void BezierCamera::generateNextSpline()
{
	//The first point of a spline is always the last point of the previous spline
	nextSpline[0] = currentSpline[3];

	//The second point must be opposite the third point of current spline
	nextSpline[1] = -(currentSpline[2] - currentSpline[3]) + nextSpline[0];

	//The third and fourth points are semi-random. The camera shouldn't be moving too eratically so it will be somewhat limited. It also has to not pass through objects

	//range from -5-20 on zvalue to encourage forward movement
	float xVal = rand() % 150 - 75.f;
	float yVal = rand() % 150 - 75.f;
	float zVal = rand() % 150 - 75.f;
	nextSpline[2] = vec3(xVal, yVal, zVal);

	//range from -5-10 around nextspline[2]
	xVal = rand() % 150 - 75.f;
	yVal = rand() % 150 - 75.f;
	zVal = rand() % 150 - 75.f;
	nextSpline[3] = vec3(xVal, yVal, zVal);
}

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
vec3 BezierCamera::deCastleJau(vec3 & firstPoint, vec3 & secondPoint, vec3 & thirdPoint, vec3 & fourthPoint, float t, vec3 & lookat)
{
	vec3 p1 = this->lerp(firstPoint, secondPoint, t);
	vec3 p2 = this->lerp(secondPoint, thirdPoint, t);
	vec3 p3 = this->lerp(thirdPoint, fourthPoint, t);
								 
	vec3 p4 = this->lerp(p1, p2, t);
	vec3 p5 = this->lerp(p2, p3, t);
	lookat = p5;
								 
	vec3 p6 = this->lerp(p4, p5, t);
	return p6;
}
vector<vec3> BezierCamera::GetTPoints()
{
	findtValues();
	return tPoints;
}

vec3 BezierCamera::lerp(vec3 & p1, vec3 & p2, float & t)
{
	vec3 pos = p1 + (p2 - p1) * t;
	return pos;
}

vec3* BezierCamera::GetSpline()
{
	return nextSpline;
}

void BezierCamera::GetSmallTs(vec3& t1, vec3& t2)
{

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
	float bigTDelta = .05f / distances[50];
	for (int i = 0; i < 100; i++)
	{
		dividedDistances[i] = distances[i] / distances[50];
	}

	for (int i = 0; i < 99; i++)
	{
		if (bigT < dividedDistances[i])
		{
			double tBefore = dividedDistances[i - 1];
			double tAfter = dividedDistances[i];
			double other = i / 51.f;
			double numerator = bigT - (bigT - bigTDelta);
			double denominator = dividedDistances[i] - dividedDistances[i - 1];

			float t = (bigT / denominator) * (tAfter - tBefore);
			cameraPos = deCastleJau(currentSpline[0], currentSpline[1], currentSpline[2], currentSpline[3], t, lookat);
			break;
		}
	}

	bigT += bigTDelta;
	diff = cameraPos - cameraPreviousPos;
	if (bigT >= 1.0f)
	{
		currentSpline[0] = nextSpline[0];
		currentSpline[1] = nextSpline[1];
		currentSpline[2] = nextSpline[2];
		currentSpline[3] = nextSpline[3];
		generateNextSpline();
		bigT = 0.0f;
	}
	return cameraPos;
}

