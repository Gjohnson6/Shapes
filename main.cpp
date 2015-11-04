#include <iostream>
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
#include "BezierCamera.h"

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
vector<Instance> instances;
freetype::font_data our_font;

Disc disc1(64, pi<float>() * 1.5f, 0.25f, 0.125f);
Disc disc2(64, pi<float>() * 2.0f , 0.25f , 0.0f);
Disc disc3(128 , pi<float>() * 1.5f , 1.0f , 0.0f);
Disc ring(64, pi<float>() * 2.0f, 0.25f, 0.24f);
Cylinder cylinder1(64, 8, pi<float>() * 2.0f, 1.0f, 1.0f);
Cylinder cylinder2(64 , 8 , pi<float>() * 2.0f , 1.0f , 1.0f);
Plane plane1(8 , 8);
Plane plane2(2 , 2);
Cube cube;
GridConstellation gc;
BezierCamera cam;
BezierCamera cam2;

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

inline void UpdateTime()
{
	Window::current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
}

void TestUpdateCube(struct Shape::Data & data , float current_time, void * blob)
{
	data.vertices = data.vbackup;
	float delta = sin(current_time * 2.0f);
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
/*
void measure_text(font_t* font, const char* msg, float* width, float* height) {
int i, c;

if (!msg) {
return;
}

if (width) {
// Width of a text rectangle is a sum advances for every glyph in a string
*width = 0.0f;

for (i = 0; i < strlen(msg); ++i)
{
	c = msg[i];
	*width += font->advance[c];
}
	}

	if (height)
	{
		// Height of a text rectangle is a high of a tallest glyph in a string
		*height = 0.0f;

		for (i = 0; i < strlen(msg); ++i)
		{
			c = msg[i];

			if (*height < font->height[c])
			{
				*height = font->height[c];
			}
		}
	}
}*/

void AdaptFreetype(freetype::font_data font , mat4 & model_matrix , mat4 & view_matrix , mat4 & projection_matrix, vector<string> & strings, float x, float y)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(value_ptr(view_matrix * model_matrix));
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(value_ptr(projection_matrix));
	for (vector<string>::iterator i = strings.begin(); i < strings.end(); i++)
	{
		freetype::print(font , x , y , "%s" , (*i).c_str());
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
				window->ReshapeFunc(w , h);
			}
			else
			{
				window->size = ivec2(w , h);
				window->aspect = float(w) / float(h);
			}
		}
	}
}

void CloseFunc()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window != nullptr)
	{
		if (window->CloseFunc != nullptr)
		{
			window->CloseFunc();
		}
		else
		{
			window->handle = BAD_GL_VALUE;
		}
	}
}

void KeyboardFunc(unsigned char c, int x, int y)
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window == nullptr)
		return;

	if (window->KeyboardFunc != nullptr)
	{
		window->KeyboardFunc(c , x , y);
	}

	switch (c)
	{
		case 'p':
			window->is_paused = !window->is_paused;
			UpdateTime();
			if (window->is_paused)
			{
				// We have just been paused. Store away the current time.
				window->time_when_paused = Window::current_time;
			}
			else
			{
				// We have just been unpaused. Add the elapsed time since
				// we were paused to the total time spent paused. This will
				// be subtracted from future gets of the current time.
				window->time_spent_paused += (Window::current_time - window->time_when_paused);
			}
			break;

		case '+':
			window->fovy++;
			if (window->fovy > 90.0f)
				window->fovy = 90.0f;
			break;

		case '-':
			window->fovy--;
			if (window->fovy < 2.0f)
				window->fovy = 2.0f;
			break;

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
	mat4 m = rotate(mat4() , radians(window->LocalTime() * 30.0f) , vec3(0.0f , 1.0f , 0.2f));
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
			glFrontFace(GL_CCW);
			disc3.Draw(false);
			glFrontFace(GL_CW);

			break;
		case 2:
			plane1.Draw(false);
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
	vec3 ambient = vec3(0.1f, 0.1f, 0.1f);
	vec3 specular = vec3(1.0f, 1.0f, 1.0f);
	vec3 diffuse = vec3(0.0f, 0.0f, 0.8f);

	glClearColor(crimson.r, crimson.g, crimson.b, crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	mat4 model_matrix = rotate(mat4(), radians(window->LocalTime() * 30.0f), vec3(0.0f, 1.0f, 0.0f));
	model_matrix = rotate(model_matrix, radians(5.0f), vec3(1.0f, 1.0f, 1.0f));
	model_matrix = scale(model_matrix, vec3(0.7f, 0.7f, 0.7f));
	mat4 view_matrix = lookAt(vec3(0.0f, 0.0f, 15.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy), window->aspect, window->near_distance, window->far_distance);
	phong_shader.Use(model_matrix, view_matrix, projection_matrix);
	phong_shader.SetMaterial(diffuse, specular, 64.0f, ambient);
	phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
	cube.Draw(false);
	phong_shader.UnUse();
	glutSwapBuffers();
	cube.UpdateValues(TestUpdateCube, window->LocalTime(), nullptr);

}

void DisplayDisc()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f , 0.0f , 0.0f , 1.0f);
	vec3 ambient = vec3(0.1f , 0.1f , 0.1f);
	vec3 specular = vec3(1.0f , 1.0f , 1.0f);
	vec3 diffuse = vec3(0.0f , 0.0f , 0.8f);

	glClearColor(crimson.r , crimson.g , crimson.b , crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	//glEnable(GL_CULL_FACE);
	mat4 model_matrix = rotate(mat4() , radians(window->LocalTime() * 40.0f) , vec3(0.0f , 1.0f , 0.0f));
	vec3* spline = cam.GetSpline();
	vector<vec3> points = cam.GetTPoints();
	vec3 lookat;
	vec3 pos = cam.GetCameraPosition(lookat);

	model_matrix = scale(model_matrix , vec3(3.0f , 3.0f , 3.0f));
	mat4 view_matrix = lookAt(pos, lookat, vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy) , window->aspect , .01f , window->far_distance);
	phong_shader.Use(model_matrix , view_matrix , projection_matrix);
	phong_shader.SetMaterial(diffuse , specular , 16.0f , ambient);
	phong_shader.SetLightPosition(vec3(0.0f , 0.0f , 1000.0f));
	disc3.Draw(false);
	phong_shader.UnUse();
	
	
	for (int i = 0; i < points.size(); i++)
	{
		vec3 displaySpline = points[i];
		model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if(displayBezier)
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
		if(displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}
	//phong_shader.UnUse();
	glutSwapBuffers();
	disc3.UpdateValues(TestUpdateDisc , window->LocalTime(), nullptr);
}

void DisplayPlane()
{
	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0, 0, window->size.x, window->size.y);
	vec4 crimson(0.6f , 0.0f , 0.0f , 1.0f);
	vec3 ambient = vec3(0.1f , 0.1f , 0.1f);
	vec3 specular = vec3(1.0f , 1.0f , 1.0f);
	vec3 diffuse = vec3(0.0f , 0.0f , 0.8f);

	glClearColor(crimson.r , crimson.g , crimson.b , crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	mat4 model_matrix = rotate(mat4() , radians(window->LocalTime() * 40.0f) , vec3(0.0f , 1.0f , 0.0f));
	model_matrix = scale(model_matrix , vec3(3.0f , 3.0f , 3.0f));
	mat4 view_matrix = lookAt(vec3(0.0f , 0.0f , 10.0f) , vec3(0.0f , 0.0f , 0.0f) , vec3(0.0f , 1.0f , 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy) , window->aspect , window->near_distance , window->far_distance);
	phong_shader.Use(model_matrix , view_matrix , projection_matrix);
	phong_shader.SetMaterial(diffuse , specular , 64.0f , ambient);
	phong_shader.SetLightPosition(vec3(0.0f , 0.0f , 1000.0f));
	plane2.Draw(false);
	phong_shader.UnUse();
	glutSwapBuffers();
}

void DisplayGrid()
{
	//cout << "dg\n";

	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glViewport(0 , 0 , window->size.x , window->size.y);
	vec4 crimson(0.6f , 0.0f , 0.0f , 1.0f);
	vec3 ambient = vec3(0.1f , 0.1f , 0.1f);
	vec3 specular = vec3(1.0f , 1.0f , 1.0f);
	vec3 diffuse = vec3(0.0f , 0.0f , 0.8f);

	glClearColor(crimson.r , crimson.g , crimson.b , crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);

	vector<Constellation::PositionData> & pd = gc.GetPositionData();
	vec3* spline = cam2.GetSpline();
	vector<vec3> points = cam.GetTPoints();
	vec3 lookat;
	vec3 pos = cam.GetCameraPosition(lookat);


	mat4 s = scale(mat4() , vec3(50.0f , 50.0f , 1.0f));
	mat4 view_matrix = lookAt(pos, lookat, vec3(0.0f, 1.0f, 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy) , window->aspect , window->near_distance , window->far_distance);

	/*
	*/
	for (int i = 0; i < points.size(); i++)
	{
		vec3 displaySpline = points[i];
		mat4 model_matrix = mat4();
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
		mat4 model_matrix = mat4();
		model_matrix = translate(model_matrix, displaySpline);
		phong_shader.Use(model_matrix, view_matrix, projection_matrix);
		phong_shader.SetMaterial(diffuse, specular, 16.0f, ambient);
		phong_shader.SetLightPosition(vec3(0.0f, 0.0f, 1000.0f));
		if (displayBezier)
			ring.Draw(false);
		phong_shader.UnUse();
	}

	for (vector<Constellation::PositionData>::iterator iter = pd.begin(); iter < pd.end(); iter++)
	{
		mat4 model_matrix = rotate(mat4() , radians(window->LocalTime() * 40.0f) , vec3(0.0f , 1.0f , 0.0f));
		model_matrix = translate(model_matrix , vec3(s * vec4((*iter).location , 1.0f)));
		vec3 target_dir = normalize((*iter).outward_direction_vector);
		float rot_angle = acos(dot(target_dir , vec3(0.0f, 0.0f, 1.0f)));
		if (fabs(rot_angle) > glm::epsilon<float>())
		{
			vec3 rot_axis = normalize(cross(target_dir , vec3(0.0f, 0.0f, 1.0f)));
			model_matrix = rotate(model_matrix, rot_angle , rot_axis);
		}
		model_matrix = scale(model_matrix , vec3(2.0f, 2.0f, 1.0f));
		phong_shader.Use(model_matrix , view_matrix , projection_matrix);
		phong_shader.SetMaterial(diffuse , specular , 64.0f , ambient);
		phong_shader.SetLightPosition(vec3(0.0f , 0.0f , 1000.0f));
		plane2.Draw(false);
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

	vec4 crimson(0.6f , 0.0f , 0.0f , 1.0f);
	vec3 ambient = vec3(0.1f , 0.1f , 0.1f);
	vec3 specular = vec3(1.0f , 1.0f , 1.0f);
	vec3 diffuse = vec3(0.0f , 0.0f , 0.8f);

	glClearColor(crimson.r , crimson.g , crimson.b , crimson.a);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CW);
	mat4 model_matrix = rotate(mat4() , radians(window->LocalTime() * 30.0f) , vec3(0.0f, 1.0f, 0.0f));
	model_matrix = scale(model_matrix , vec3(3.0f , 3.0f , 3.0f));

	mat4 view_matrix = lookAt(vec3(0.0f , 0.0f , 10.0f) , vec3(0.0f , 0.0f , 0.0f) , vec3(0.0f , 1.0f , 0.0f));
	mat4 projection_matrix = perspective(radians(window->fovy) , window->aspect , window->near_distance , window->far_distance);
	phong_shader.Use(model_matrix , view_matrix , projection_matrix);
	phong_shader.SetMaterial(diffuse , specular , 64.0f , ambient);
	phong_shader.SetLightPosition(vec3(0.0f , 0.0f , 1000.0f));
	cylinder2.Draw(false);
	phong_shader.UnUse();
	AdaptFreetype(our_font , scale(mat4(), vec3(0.01f, 0.01f, 0.01f)) , lookAt(vec3(0.0f , 0.0f , 10.0f) , vec3(0.0f , 0.0f , 0.0f) , vec3(0.0f , 1.0f , 0.0f)) , perspective(radians(window->fovy) , window->aspect , window->near_distance , window->far_distance) , strings , -400 , 0);
	glutSwapBuffers();
}

void DisplayFunc()
{
	Window::current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	vec4 crimson(0.6f, 0.0f, 0.0f, 1.0f);

	Window * window = Window::FindCurrentWindow(windows);
	if (window->handle == BAD_GL_VALUE)
		return;

	glPolygonMode(GL_FRONT_AND_BACK , (window->wireframe ? GL_LINE : GL_FILL));

	if (!window->is_paused)
	{
		if (window->DisplayFunc != nullptr)
		{
			window->DisplayFunc();
			return;
		}

		#ifdef STEREO
		glDrawBuffer(GL_BACK);
		#endif
		glClearColor(crimson.r , crimson.g , crimson.b , crimson.a);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CW);
		//glEnable(GL_CULL_FACE);
		DrawScene(window);
		glutSwapBuffers();
	}
}

void TimerFunc(int period)
{
	glutTimerFunc(1000 / 60 , TimerFunc , 1000 / 60);
	Window::PostAllRedisplays(windows);
}

void IdleFunc()
{
	Window::PostAllRedisplays(windows);
}

void InitializeWindows()
{
	for (unsigned int i = 0; i < windows.size(); i++)
	{
		windows[i].handle = glutCreateWindow(windows[i].window_name);
		glutReshapeFunc(ReshapeFunc);
		glutCloseFunc(CloseFunc);
		glutDisplayFunc(DisplayFunc);
		glutKeyboardFunc(KeyboardFunc);
		if (i == 0)
		{
			//glutTimerFunc(1000 / 60 , TimerFunc , 1000 / 60);
			glutIdleFunc(IdleFunc);
		}
	}
}
int main(int argc, char * argv[])
{
	srand(unsigned int(time(NULL)));
	// glutInit must be the first thing to use OpenGL
	//BezierCamera cam = BezierCamera();

	glutInit(&argc, argv);
	// Initializes the Develeoper's Imaging Library
	ilInit();
	// Add a line here for every shader defined. This will
	// take care of loading and unloading.
	shaders.push_back(ShaderInitializer(&phong_shader, "per-fragment-phong.vs.glsl", "per-fragment-phong.fs.glsl", "phong shader failed to initialize"));
	shaders.push_back(ShaderInitializer(&constant_shader, "constant.vs.glsl", "constant.fs.glsl", "phong shader failed to initialize"));
	// Adds objects to the world.
	Instance::DefineInstances(instances, NUMBER_OF_OBJECTS);

	glutInitWindowSize(512, 512);
	glutInitWindowPosition(-1, -1);
	glutInitDisplayMode(DISPLAY_MODE);
	// By setting this action, control is returned to our program when glutLeaveMainLoop
	// is called. Without this, the program exits.
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE , GLUT_ACTION_CONTINUE_EXECUTION);
	// By setting this option, all windows created in the program share the same OpenGL context.
	// This means all buffers and shaders and such need be instantiated only once.
	glutSetOption(GLUT_RENDERING_CONTEXT , GLUT_USE_CURRENT_CONTEXT);

	gc.Initialize(512);

	windows.push_back(Window("Basic Shape Viewer" , nullptr , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 100.0f));
	windows.push_back(Window("Cylinder" , DisplayCylinder , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 100.0f));
	windows.push_back(Window("Plane" , DisplayPlane , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 100.0f));
	windows.push_back(Window("Disc" , DisplayDisc , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 400.0f));
	windows.push_back(Window("Cube", DisplayCube, nullptr, nullptr, nullptr, ivec2(512, 512), 50.0f, 1.0f, 100.0f));
	windows.push_back(Window("Grid" , DisplayGrid , nullptr , nullptr , nullptr , ivec2(512 , 512) , 50.0f , 1.0f , 400.0f));

	InitializeWindows();
	//our_font.init("c:\\windows\\fonts\\Candarai.ttf" , 128);
	//texture_file_names.push_back("DSC_0337.jpg");
	//texture_file_names.push_back("DSC_0338.jpg");
	//texture_file_names.push_back("DSC_1233.jpg");
	//texture_file_names.push_back("DSC_1623.jpg");

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

#ifdef FULL_SCREEN
	glutFullScreen();
#endif

	// This must be called AFTER an OpenGL context has been built.
	if (glewInit() != GLEW_OK)
	{
		cerr << "GLEW failed to initialize." << endl;
		cerr << "Hit enter to exit:";
		cin.get();
		return 0;
	}

	if (!ShaderInitializer::Initialize(&shaders))
		exit(0);

	glutMainLoop();

	ShaderInitializer::TakeDown(&shaders);
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