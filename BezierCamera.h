#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "phong_shader.h"

using namespace glm;
using namespace std;

class BezierCamera
{
public:
	BezierCamera();
	~BezierCamera();
	vec3* GetSpline(); 
	vec3 GetCameraPosition(vec3 & lookat);
	vector<vec3> GetTPoints();

private:
	vec3 currentSpline[4];
	vec3 nextSpline[4];
	vec3 previousSpline[4];
	vec3 cameraPos;
	vec3 cameraPreviousPos;

	void generateNextSpline();
	void findtValues();
	vec3 lerp(vec3& p1, vec3& p2, float& t);
	vec3 linearInterpolate(vec3 & firstPoint, vec3 & secondPoint, vec3 & thirdPoint, vec3 & fourthPoint, float t);
	void GetSmallTs(vec3 & t1, vec3 & t2);

	float bigT = 0.f;
	float littleT = 0.f;
	float tBefore = 0.f;
	float tAfter = 0.f;
	float distanceToMove = 1.0f;
	vector<vec3> tPoints;
};

