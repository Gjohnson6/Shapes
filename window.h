#pragma once
#include <vector>
#include "GL/glew.h"
#include "GL/freeglut.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>

#ifndef BAD_GL_VALUE
#define	BAD_GL_VALUE	(GLuint(-1))
#endif // !BAD_GL_VALUE

class Window
{
public:
	Window(char * window_name, void(*DisplayFunc)() , void(*KeyboardFunc)(unsigned char c , int x , int y) , void(*ReshapeFunc)(int w , int h), void (*CloseFunc)(), glm::ivec2 size, float fovy, float near_distance, float far_distance);

	static Window * FindCurrentWindow(std::vector<Window> & windows);
	static void PostAllRedisplays(std::vector<Window> & windows);
	static void InitializeWindows(std::vector<Window> & windows , void(*DisplayFunc)(void) , void(*KeyboardFunc)(unsigned char , int , int) , void(*CloseFunc)(void) , void(*ReshapeFunc)(int , int) , void(*IdleFunc)());

	void SetWindowTitle(std::string new_title) { this->SetWindowTitle(new_title.c_str()); }
	void SetWindowTitle(char * new_title) { glutSetWindowTitle(new_title); }

	void(*DisplayFunc)();
	void(*KeyboardFunc)(unsigned char c , int x , int y);
	void(*ReshapeFunc)(int w , int h);
	void(*CloseFunc)();

	static float UpdateTime()
	{
		Window::current_time = float(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
		return Window::current_time;
	}

	static float CurrentTime()
	{
		return Window::current_time;
	}

	float LocalTime()
	{
		return (this->is_paused) ? (this->time_when_paused - this->time_spent_paused) : (Window::CurrentTime() - this->time_spent_paused);
	}

	char * window_name;
	GLuint handle;
	glm::ivec2 size;
	bool wireframe;
	bool draw_normals;
	float aspect;
	float fovy;
	float near_distance;
	float far_distance;
	float time_spent_paused;
	float time_when_paused;
	bool is_paused;

private:
	static float current_time;
};