#include "window.h"

float Window::current_time;

Window::Window(char * window_name,  void(*DisplayFunc)() , void(*KeyboardFunc)(unsigned char c , int x , int y), void(*SpecialFunc)(int, int, int), void(*ReshapeFunc)(int w , int h) , void(*CloseFunc)() , glm::ivec2 size , float fovy , float near_distance , float far_distance)
{
	this->window_name = window_name;
	this->DisplayFunc = DisplayFunc;
	this->KeyboardFunc = KeyboardFunc;
	this->SpecialFunc = SpecialFunc;
	this->ReshapeFunc = ReshapeFunc;
	this->CloseFunc = CloseFunc;
	this->size = size;
	this->fovy = fovy;
	this->near_distance = near_distance;
	this->far_distance = far_distance;
	this->wireframe = false;
	this->instructions = true;
	this->draw_normals = false;
	this->time_when_paused = 0.0f;
	this->time_spent_paused = 0.0f;
	this->full_screen = false;
	this->is_paused = false;
	this->blur = false;
	this->handle = BAD_GL_VALUE;
	this->shaderNum = 4;
}

Window * Window::FindCurrentWindow(std::vector<Window> & windows)
{
	GLuint current_window = glutGetWindow();

	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i].handle == current_window)
			return &windows[i];
	}
	return nullptr;
}

void Window::PostAllRedisplays(std::vector<Window> & windows)
{
	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i].handle != BAD_GL_VALUE)
		{
			glutSetWindow(windows[i].handle);
			glutPostRedisplay();
		}
	}
}

void Window::InitializeWindows(std::vector<Window> & windows, void(*DisplayFunc)(void) , void(*KeyboardFunc)(unsigned char , int , int), void(*SpecialFunc)(int, int, int), void(*CloseFunc)(void) , void(*ReshapeFunc)(int , int) , void(*IdleFunc)())
{
	for (unsigned int i = 0; i < windows.size(); i++)
	{
		if (windows[i].handle != BAD_GL_VALUE)
			continue;

		windows[i].handle = glutCreateWindow(windows[i].window_name);
		glutReshapeFunc(ReshapeFunc);
		glutCloseFunc(CloseFunc);
		glutDisplayFunc(DisplayFunc);
		glutKeyboardFunc(KeyboardFunc);
		glutSpecialFunc(SpecialFunc);
		if (i == 0)
		{
			//glutTimerFunc(1000 / 60 , TimerFunc , 1000 / 60);
			glutIdleFunc(IdleFunc);
		}
	}
}
