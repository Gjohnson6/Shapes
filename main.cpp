#include <iostream>
#include <iomanip>
#include <time.h>
#include "disc.h"
#include "cylinder.h"
#include "plane.h"
#include "phong_shader.h"
#include "constant_shader.h"
#include "ilcontainer.h"
#include "instance.h"
#include "window.h"
#include "my_freetype.h"
#include "grid_constellation.h"
#include "open_cube_constellation.h"
#include "closed_cube_constellation.h"
#include "BezierCamera.h"
#include "torus.h"
#include "glm\gtc\noise.hpp"


using namespace std;
using namespace glm;

//#define	FULL_SCREEN
#define	MOVE
//#define	SHOW_NORMALS

#ifdef USE_STEREO
#define	DISPLAY_MODE	(GLUT_RGBA | GLUT_DEPTH | GLUT_STEREO)
#else
#define	DISPLAY_MODE	(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE)
#endif // USE_STEREO

const int NUMBER_OF_OBJECTS = 512;
int numShapesOptions[] = { 8, 26,56, 98, 152, 218, 296, 386, 488 };
int shapeIndex = 0;
int numShapes = 8;
vector<Instance> instances;
freetype::font_data our_font;

Disc disc1(64, pi<float>() * 1.5f, 0.25f, 0.125f);
Disc disc2(64, pi<float>() * 2.0f, 0.25f, 0.0f);
Disc disc3(128, pi<float>() * 2.0f, 1.0f, 0.0f);
Disc ring(64, pi<float>() * 2.0f, 0.25f, 0.24f);
Cylinder cylinder1(32, 8, pi<float>() * 2.0f, 1.0f, 1.0f);
Cylinder cylinder2(4, 2, pi<float>() * 2.0f, 1.0f, 0.5f);
Plane plane1(8, 8);
Plane plane2(64, 64);
Cube cube;
GridConstellation gc;
OpenCubeConstellation occ;
ClosedCubeConstellation ccc;
BezierCamera cam;
BezierCamera cam2;
Torus torus(.5f, 1, 20, 20);

vec3 eye(0.0f, 0.0f, 15.0f);
vec3 cop(0.0f, 0.0f, 0.0f);
vec3 up(0.0f, 1.0f, 0.0f);

PhongShader phong_shader;
ConstantShader constant_shader;
vector<ShaderInitializer> shaders;
vector<Window> windows;
vector<ILContainer> textures;
vector<string> texture_file_names;

bool displayBezier = false;

void TestUpdateCube(struct Shape::Data & data, float current_time, void * blob)
{
	data.vertices = data.vbackup;
	float delta = sin(current_time * 2.0f);
	float theta = cos(current_time * 2.0f);
	for (int i = 0; i < 4; i++)
	{
		data.vertices[i] = vec3(data.vertices[i].x, data.vertices[i].y, delta + 2.0f);
		data.vertices[i + 4] = vec3(delta + 2.0f, data.vertices[i + 4].y, data.vertices[i + 4].z);
		data.vertices[i + 8] = vec3(data.vertices[i + 8].x, data.vertices[i + 8].y, -delta - 2.0f);
		data.vertices[i + 12] = vec3(-delta - 2.0f, data.vertices[i + 12].y, data.vertices[i + 12].z);
		data.vertices[i + 16] = vec3(data.vertices[i + 16].x, delta + 2.0f, data.vertices[i + 16].z);
		data.vertices[i + 20] = vec3(data.vertices[i + 20].x, -delta - 2.0f, data.vertices[i + 20].z);
	}
}

void testUpdateTorus(struct Shape::Data & data, float current_time, void * blob)
{
	
}

void TestUpdatePlane(struct Shape::Data & data, float current_time, void * blob)
{
	ivec2 dimensions = *((ivec2 *)blob);
	int i = 0;
	data.vertices = data.vbackup;
	vector<vec3> & v = data.vertices;
	for (int y = 0; y <= dimensions.y; y++)
	{
		for (int x = 0; x <= dimensions.x; x++, i++)
			v[i] = vec3(v[i].x, v[i].y, cos(v[i].y * 2.0f + current_time * 2.0f) * sin(v[i].x * 2.0f + current_time * 2.0f) / 2.0f);
	}
}

void TestUpdateDisc(struct Shape::Data & data, float current_time, void * blob)
{
	data.vertices = data.vbackup;
	float theta = (2 * pi<float>()) / (data.vertices.size() - 1);
	for (size_t i = 0; i < data.vertices.size() - 1; i++)
	{
		data.vertices[i + 1] = vec3(data.vertices[i + 1].x, data.vertices[i + 1].y, sin(theta*i * 6 + current_time * 6.0f) / 14.0f);
	}
}

void TestUpdate(struct Shape::Data & data, float current_time, void * blob)
{
	data.vertices = data.vbackup;
	for (vector<vec3>::iterator iter = data.vertices.begin(); iter < data.vertices.end(); iter++)
		(*iter) = (*iter) * vec3(1.0f, cos(current_time) + 1.01f, 1.0f);
}

void AdaptFreetype(freetype::font_data font, mat4 & model_matrix, mat4 & view_matrix, mat4 & projection_matrix, vector<string> & strings, float x, float y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(view_matrix * model_matrix));
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(projection_matrix));
	for (vector<string>::iterator i = strings.begin(); i < strings.end(); i++)
	{
		freetype::print(font, x, y, "%s", (*i).c_str());
	}
}

void ReshapeFunc(int w, int h)
{
	// The idea here is that Windows are now objects. If a window
	// has defined its own ReshapeFunc, call it. Otherwise supply
	// a standard behavior. The same holds true for each of the
	// other glut callbacks.
	if (h > 0)
	{
		Window * window = Window::FindCurrentWindow(windows);
		if (window != nullptr)
		{
			if (window->ReshapeFunc != nullptr)
			{
				window->ReshapeFunc(w, h);
			}
			else
			{
				window->size = ivec2(w, h);
				window->aspect = float(w) / float(h);
			}
		}
	}
}

// If a window specific close function is defined, it is called. After that,
// the handle to the provoking window is initialized to BAD_GL_VALUE.
void CloseFunc()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window != nullptr)
	{
		if (window->CloseFunc != nullptr)
		{
			window->CloseFunc();
		}
		window->handle = BAD_GL_VALUE;
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window == nullptr)
		return;

	if (window->KeyboardFunc != nullptr)
	{
		window->KeyboardFunc(c, x, y);
	}

	switch (c)
	{
	case '-':
		shapeIndex > 0 ? shapeIndex-- : shapeIndex;
		occ.Initialize(numShapesOptions[shapeIndex]);
		break;
	case '+':
		shapeIndex < 8 ? shapeIndex++ : shapeIndex;
		occ.Initialize(numShapesOptions[shapeIndex]);
		break;
	case 'n':
		window->draw_normals = !window->draw_normals;
		break;

	case 'p':
		if (!window->is_paused)
		{
			// We are being paused. 
			// Store away the current time.
			window->time_when_paused = Window::CurrentTime();

			//cout << left << setw(10) << "P when: " << setprecision(4) << window->time_when_paused;
			//cout << " spent: " << setprecision(4) << window->time_spent_paused;
			//cout << " Local: " << setprecision(4) << window->LocalTime() << endl;
		}
		else
		{
			// We have just been unpaused. Add the elapsed time since
			// we were paused to the total time spent paused. This will
			// be subtracted from future gets of the current time.
			//bug here
			float elapsed_time_this_pause = Window::CurrentTime() - window->time_when_paused;
			assert(elapsed_time_this_pause > 0);
			window->time_spent_paused += elapsed_time_this_pause;

			//cout << left << setw(10) << "U when: " << setprecision(4) << window->time_when_paused;
			//cout << " spent: " << setprecision(4) << window->time_spent_paused;
			//cout << " Local: " << setprecision(4) << window->LocalTime();
			//cout << " Current: " << setprecision(6) << Window::CurrentTime() << endl;
		}
		window->is_paused = !window->is_paused;
		break;

	//case '+':
	//	window->fovy++;
	//	if (window->fovy > 90.0f)
	//		window->fovy = 90.0f;
	//	break;

	//case '-':
	//	window->fovy--;
	//	if (window->fovy < 2.0f)
	//		window->fovy = 2.0f;
	//	break;

	case 'w':
		window->wireframe = !window->wireframe;
		break;
	case 'b':
		displayBezier = !displayBezier;
		break;
	case 'x':
	case 27:
		glutLeaveMainLoop();
		return;
	}
}

void DrawScene(Window * window)
{
	phong_shader.GLReturnedError("DrawScene() - entering");
#ifdef MOVE
	mat4 m = rotate(mat4(), radians(window->LocalTime() * 30.0f), vec3(0.0f, 1.0f, 0.2f));
	m = translate(m, vec3(0.0f, 11.5f * cos(window->LocalTime() * 0.5f) + 2.0f, 11.5f * sin(window->LocalTime() * 0.5f) + 2.0f));
#else
	mat4 m;
#endif // MOVE

	mat4 view_matrix = lookAt(vec3(m * vec4(eye, 1.0f)), cop, up);
	mat4 model_matrix;
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);

	vec3 z_axis = vec3(0.0f, 0.0f, 1.0f);
	vec3 y_axis = vec3(0.0f, 1.0f, 0.0f);
	vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	float c_offset = radians(45.0f);

	glViewport(0, 0, window->size.x, window->size.y);

	const int count_of_shapes = 4;

	for (unsigned int i = 0; i < instances.size(); i++)
	{
		model_matrix = translate(mat4(), instances[i].position);
		model_matrix = rotate(model_matrix, radians(window->LocalTime() * instances[i].rate) + instances[i].offset, y_axis);
		if (i % count_of_shapes == 3)
			model_matrix = scale(model_matrix, vec3(0.25f, 0.25f, 0.25f));

		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(instances[i].diffuse, specular, 32.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));

		switch (i % count_of_shapes)
		{
		case 0:
			disc1.Draw(false);
			break;
		case 1:
			disc3.Draw(false);
			break;
		case 2:
			plane2.Draw(false);
			break;
		case 3:
			cube.Draw(false);
			break;
		}
		phong_shader.UnUse();

#ifdef SHOW_NORMALS
		if (i == 0)
		{
			constant_shader.Use(model_matrix, view_matrix, projection_matrix);
			constant_shader.SetMaterial(vec3(0.0f, 0.0f, 0.8f), specular, 128.0f, vec3(1.0f, 0.0f, 0.0f));
			disc1.Draw(true);
			constant_shader.UnUse();
		}
#endif
	}

	model_matrix = mat4();
	mat4 mz = model_matrix;
	model_matrix = scale(model_matrix, vec3(0.5f, 0.5f, 16.0f));

	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(vec3(0.0f, 0.0f, 0.8f), specular, 128.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 1000.0f, 0.0f));
	cylinder1.Draw(false);
	phong_shader.UnUse();

#ifdef SHOW_NORMALS
	constant_shader.Use(model_matrix, view_matrix, projection_matrix);
	constant_shader.SetMaterial(vec3(0.0f, 0.0f, 0.8f), specular, 128.0f, vec3(1.0f, 1.0f, 1.0f));
	cylinder.Draw(true);
	constant_shader.UnUse();
#endif

	model_matrix = rotate(mz, radians(90.0f), y_axis);
	model_matrix = scale(model_matrix, vec3(0.5f, 0.5f, 16.0f));
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(vec3(1.0f, 0.0f, 0.0f), specular, 128.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 1000.0f, 0.0f));
	cylinder1.Draw(false);
	phong_shader.UnUse();

	model_matrix = rotate(mz, radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
	model_matrix = scale(model_matrix, vec3(0.5f, 0.5f, 16.0f));
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(vec3(0.0f, 1.0f, 0.0f), specular, 128.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 1000.0f, 0.0f));
	cylinder1.Draw(false);
	phong_shader.UnUse();

	cylinder1.UpdateValues(TestUpdate, window->LocalTime(), nullptr);
}

void DisplayCube()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);
	vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.6f, 0.0f, 0.0f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.8f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 30.0f), vec3(0.1f, 1.0f, 0.1f));
	model_matrix = rotate(model_matrix, radians(5.0f), vec3(1.0f, 1.0f, 1.0f));
	model_matrix = scale(model_matrix, vec3(0.7f, 0.7f, 0.7f));
	mat4 view_matrix = lookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(diffuse, specular, 64.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
	phong_shader.SelectSubroutine(PhongShader::SHADER_TOY_1);
	phong_shader.SetGlobalTime(Window::CurrentTime());
	phong_shader.EnableTexture(textures[3], 0);
	cube.Draw(false);
	phong_shader.UnUse();
	if (window->draw_normals)
	{
		constant_shader.Use(model_matrix, view_matrix, projection_matrix);
		constant_shader.SetMaterial(diffuse, specular, 64.0f, vec3(1.0f, 1.0f, 1.0f));
		cube.Draw(true);
		constant_shader.UnUse();
	}
	glutSwapBuffers();
	cube.UpdateValues(TestUpdateCube, window->LocalTime(), nullptr);

}

void DisplayDisc()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);
	vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(0.0f, 0.0f, 0.3f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.9f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 40.0f), vec3(0.0f, 1.0f, 0.0f));
	vec3* spline = cam.GetSpline();
	vector<vec3> points = cam.GetTPoints();
	vec3 lookat;
	vec3 pos = cam.GetCameraPosition(lookat);

	model_matrix = scale(model_matrix, vec3(3.0f, 3.0f, 3.0f));
	mat4 view_matrix = lookAt(pos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, .01f, window->far_distance);

	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(diffuse, specular, 128.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
	phong_shader.SelectSubroutine(PhongShader::BASIC_PHONG);
	phong_shader.EnableTexture(textures[2], 0);
	torus.Draw(false);
	phong_shader.UnUse();

	if (window->draw_normals)
	{
		constant_shader.Use(model_matrix, view_matrix, projection_matrix);
		constant_shader.SetMaterial(diffuse, specular, 32.0f, vec3(1.0f, 1.0f, 1.0f));
		torus.Draw(true);
		constant_shader.UnUse();
	}

	for (int i = 0; i < points.size(); i++)
	{
		vec3 displaySpline = points[i];
		model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if (displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}

	for (int i = 0; i < 4; i++)
	{
		vec3 displaySpline = spline[i];
		model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if (displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}
	glutSwapBuffers();
	torus.UpdateValues(testUpdateTorus, window->LocalTime(), (void *)&plane2.Dimensions());
}

void DisplayPlane()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);
	vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.8f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 0.0f), vec3(0.0f, 1.0f, 0.0f));
	model_matrix = scale(model_matrix, vec3(3.0f, 3.0f, 3.0f));

	mat4 lightMatrix = rotate(mat4(), radians(window->LocalTime() * 0.0f), vec3(0.0f, 1.0f, 0.0f));
	vec3 light_pos = vec3(lightMatrix * vec4(0.0f, 0.0f, 10000.0f, 1.0f));

	mat4 view_matrix = lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(diffuse, specular, 64.0f, ambient);
	phong_shader.SetLightPosition(light_pos);
	phong_shader.SelectSubroutine(PhongShader::SHADER_TOY_1);
	phong_shader.EnableTexture(textures[0], 0);
	//plane2.Draw(false);
	torus.Draw(false);
	phong_shader.UnUse();

	//if (window->draw_normals)
	//{
	//	constant_shader.Use(model_matrix, view_matrix, projection_matrix);
	//	constant_shader.SetMaterial(diffuse, specular, 64.0f, vec3(1.0f, 1.0f, 1.0f));
	//	plane2.Draw(true);
	//	constant_shader.UnUse();
	//}
	glutSwapBuffers();
	//plane2.UpdateValues(TestUpdatePlane, window->LocalTime(), (void *)&plane2.Dimensions());
}

void DisplayGrid()
{
	//cout << "dg\n";

	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);
	vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.8f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	vector<Constellation::PositionData> & pd = occ.GetPositionData();

	mat4 s = scale(mat4(), vec3(50.0f, 50.0f, 50.0f));
	mat4 view_matrix = lookAt(vec3(0.0f, 0.0f, 150.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	//mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);


	vec3* spline = cam.GetSpline();
	vector<vec3> points = cam.GetTPoints();
	vec3 lookat;
	vec3 pos = cam.GetCameraPosition(lookat);

	mat4 model_matrix = scale(model_matrix, vec3(3.0f, 3.0f, 3.0f));
	//mat4 view_matrix = lookAt(pos, lookat, vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, .01f, window->far_distance);





	mat4 r = rotate(mat4(), radians(window->LocalTime() * 0.0f), vec3(0.0f, 1.0f, 0.0f));

	for (vector<Constellation::PositionData>::iterator iter = pd.begin(); iter < pd.end(); iter++)
	{
		mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 20.0f), vec3(0.0f, 1.0f, 0.0f));
		model_matrix = translate(model_matrix, vec3(s * vec4((*iter).location, 1.0f)));

		// Beginning of orientation code.
		//
		// There is an assumption here (we are aligning z axes) that the shape you're building have
		// a natural facing down the z axis.
		//
		// The following orients the object's z axis along the axis held in outward_direction_vector.
		// target_dir gets that value. The difference in direction from the z axis to the desired direction
		// is captured by the dot product. The angle is retrieved with the acos. Then, if there's anything 
		// to do (I suspect the if statement is NOT needed), a rotation axis is made by the cross product
		// (rotating about it will swing the z axes around). Finally, the rotation is done.
		vec3 target_dir = normalize((*iter).outward_direction_vector);
		float rot_angle = acos(dot(target_dir, vec3(0.0f, 0.0f, 1.0f)));
		if (fabs(rot_angle) > glm::epsilon<float>())
		{
			vec3 rot_axis = normalize(cross(target_dir, vec3(0.0f, 0.0f, 1.0f)));
			model_matrix = rotate(model_matrix, rot_angle, rot_axis);
		}
		// End of orientation code.

		model_matrix = scale(model_matrix, vec3(2.0f, 2.0f, 1.0f));
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 64.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		phong_shader.SelectSubroutine(PhongShader::BASIC_PHONG);
		//phong_shader.EnableTexture(textures[1], 0);
		torus.Draw(false);
		phong_shader.UnUse();
		if (window->draw_normals)
		{
			constant_shader.Use(model_matrix, view_matrix, projection_matrix);
			constant_shader.SetMaterial(diffuse, specular, 64.0f, vec3(1.0f, 1.0f, 1.0f));
			torus.Draw(true);
			constant_shader.UnUse();
		}
		// Animate the rotation of the objects within the grid.
		(*iter).outward_direction_vector = vec3(r * vec4((*iter).outward_direction_vector, 1.0f));
	}

	for (int i = 0; i < points.size(); i++)
	{
		vec3 displaySpline = points[i];
		model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if (displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}

	for (int i = 0; i < 4; i++)
	{
		vec3 displaySpline = spline[i];
		model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if (displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}
	glutSwapBuffers();
}


void DisplayCylinder()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);

	vector<string> strings;
	strings.push_back("This is a test.");

	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);
	vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.8f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 30.0f), vec3(0.0f, 1.0f, 0.0f));
	model_matrix = scale(model_matrix, vec3(3.0f, 3.0f, 3.0f));

	mat4 view_matrix = lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(diffuse, specular, 64.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
	phong_shader.SelectSubroutine(PhongShader::SHADER_TOY_1);
	phong_shader.EnableTexture(textures[1], 0);
	torus.Draw(false);
	phong_shader.UnUse();
	if (window->draw_normals)
	{
		constant_shader.Use(model_matrix, view_matrix, projection_matrix);
		constant_shader.SetMaterial(diffuse, specular, 64.0f, vec3(1.0f, 1.0f, 1.0f));
		torus.Draw(true);
		constant_shader.UnUse();
	}
	AdaptFreetype(our_font, scale(mat4(), vec3(0.01f, 0.01f, 0.01f)), lookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f)), perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance), strings, -400, 0);
	glutSwapBuffers();
}

void DisplayFunc()
{
	Window::UpdateTime();
	glFrontFace(GL_CW);
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);

	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glPolygonMode(GL_FRONT_AND_BACK, (window->wireframe ? GL_LINE : GL_FILL));

	if (window->DisplayFunc != nullptr)
	{
		window->DisplayFunc();
		return;
	}

#ifdef STEREO
	glDrawBuffer(GL_BACK);
#endif
	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	DrawScene(window);
	glutSwapBuffers();
}

void TimerFunc(int period)
{
	glutTimerFunc(1000 / 60, TimerFunc, 1000 / 60);
	Window::PostAllRedisplays(windows);
}

void IdleFunc()
{
	Window::PostAllRedisplays(windows);
}

bool InitializeTextures()
{
	texture_file_names.push_back("c1.jpg");
	texture_file_names.push_back("c2.jpg");
	texture_file_names.push_back("c3.jpg");
	texture_file_names.push_back("c4.jpg");

	textures.resize(texture_file_names.size());
	for (size_t i = 0; i < texture_file_names.size(); i++)
	{
		if (textures[i].Initialize(texture_file_names[i].c_str()) == false)
		{
			cerr << "Failed to load texture: " << texture_file_names[i] << endl;
			return false;
		}
	}
	// Then, enable texturing, bind texture unit, bind texture and away you go.
	return true;
}

int main(int argc, char * argv[])
{
	srand(unsigned int(time(NULL)));

	// glutInit must be the first thing to use OpenGL
	glutInit(&argc, argv);

	// Initializes the Develeoper's Imaging Library. This is the library that will be used to load images in different formats
	// for use as textures. This library is old - others are better in some ways but unusable in others.
	ilInit();

	// Add a line here for every shader defined. This will take care of loading and unloading.
	shaders.push_back(ShaderInitializer(&phong_shader, "per-fragment-phong.vs.glsl", "per-fragment-phong.fs.glsl", "phong shader failed to initialize"));
	shaders.push_back(ShaderInitializer(&constant_shader, "constant.vs.glsl", "constant.fs.glsl", "phong shader failed to initialize"));

	// Adds objects to the world. These instances are for the massive mashup of shapes.
	Instance::DefineInstances(instances, NUMBER_OF_OBJECTS);

	glutInitWindowSize(512, 512);
	glutInitWindowPosition(-1, -1);
	glutInitDisplayMode(DISPLAY_MODE);

	// By setting this action, control is returned to our program when glutLeaveMainLoop is called. Without this, the program exits.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	// By setting this option, all windows created in the program share the same OpenGL context. This means all buffers and shaders and such need be instantiated only once.
	glutSetOption(GLUT_RENDERING_CONTEXT, GLUT_USE_CURRENT_CONTEXT);

	// This is the grid constellation initialization. The preferred argument is n^2.
	occ.Initialize(numShapes);

	// This vector is used to initialize all the window objects. 
	//windows.push_back(Window("Basic Shape Viewer" , nullptr , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 100.0f));
	windows.push_back(Window("Cylinder", DisplayCylinder, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 100.0f));
	windows.push_back(Window("Plane", DisplayPlane, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 100.0f));
	windows.push_back(Window("Disc", DisplayDisc, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 100.0f));
	windows.push_back(Window("Cube", DisplayCube, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 100.0f));
	windows.push_back(Window("Grid", DisplayGrid, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 400.0f));
	Window::InitializeWindows(windows, DisplayFunc, KeyboardFunc, CloseFunc, ReshapeFunc, IdleFunc);

	// This must be called AFTER an OpenGL context has been built.
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		cerr << "Hit enter to exit:";
		cin.get();
		return 0;
	}

	// This must be called AFTER initializing GLEW - else non of the 
	// new GL features will be found.
	if (!ShaderInitializer::Initialize(&shaders))
	{
		cerr << "Hit enter to exit:";
		cin.get();
		return 0;
	}

	our_font.init("c:\\windows\\fonts\\Candarai.ttf", 128);

	// Add any textures needed here. This will someday be replaced with a function
	// doing the same thing but taking its list of textures from a URI.
	if (!InitializeTextures())
	{
		cerr << "Hit enter to exit:";
		cin.get();
		return 0;
	}

#ifdef FULL_SCREEN
	glutFullScreen();
#endif

	glutMainLoop();

	ShaderInitializer::TakeDown(&shaders);
	ILContainer::TakeDown(textures);
	return 0;
}

/*
Correct method
The Toe - in method while
giving workable stereo
pairs is not correct, it
also introduces vertical
parallax which is most
noticeable for objects
in the outer field of
view.The correct method
is to use what is
sometimes known as the
"parallel axis
asymmetric frustum
perspective projection".
In this case the view
vectors for each camera
remain parallel and a
glFrustum() is used to
describe the perspective
projection.
3D Stereo Rendering Using OpenGL(and GLUT) ???4 / 6
http://astronomy.swin.edu.au/~pbourke/opengl/stereogl/ 2005-3-31

// Misc stuff
ratio = window.aspect;
radians = radians(window.fovy);
wd2 = near * tan(radians);
ndfl = near / camera.focallength;

// Clear the buffers
glDrawBuffer(GL_BACK_LEFT);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
if (stereo) {
glDrawBuffer(GL_BACK_RIGHT);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

if (stereo) {
mat4 projection_matrix;

// Derive the two eye positions
CROSSPROD(camera.vd, camera.vu, r);
normalise(r);
r *= camera.eyesep / 2.0f;

glMatrixMode(GL_PROJECTION);
glLoadIdentity();
float left = -ratio * wd2 - 0.5 * camera.eyesep * ndfl;
float right = ratio * wd2 - 0.5 * camera.eyesep * ndfl;
float top = wd2;
float bottom = -wd2;
projection_matrix = frustum(left, right, bottom, top, near, far);

glMatrixMode(GL_MODELVIEW);
glDrawBuffer(GL_BACK_RIGHT);
glLoadIdentity();
gluLookAt(camera.vp.x + r.x, camera.vp.y + r.y, camera.vp.z + r.z,
camera.vp.x + r.x + camera.vd.x,
camera.vp.y + r.y + camera.vd.y,
camera.vp.z + r.z + camera.vd.z,
camera.vu.x, camera.vu.y, camera.vu.z);
MakeLighting();
MakeGeometry();



glMatrixMode(GL_PROJECTION);
glLoadIdentity();
left = -ratio * wd2 + 0.5 * camera.eyesep * ndfl;
right = ratio * wd2 + 0.5 * camera.eyesep * ndfl;
top = wd2;
bottom = -wd2;
frustum(left, right, bottom, top, near, far);

glMatrixMode(GL_MODELVIEW);
glDrawBuffer(GL_BACK_LEFT);
glLoadIdentity();
gluLookAt(camera.vp.x - r.x, camera.vp.y - r.y, camera.vp.z - r.z,
camera.vp.x - r.x + camera.vd.x,
camera.vp.y - r.y + camera.vd.y,
camera.vp.z - r.z + camera.vd.z,
camera.vu.x, camera.vu.y, camera.vu.z);
MakeLighting();
MakeGeometry();
*/