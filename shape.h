#pragma once

#include <vector>
#include <assert.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/noise.hpp>

#ifndef BAD_GL_VALUE
#define	BAD_GL_VALUE	(GLuint(-1))
#endif // !BAD_GL_VALUE

class Shape
{
public:

	const float NORMAL_LENGTH_DIVISOR = 8.0f;

	// UpdateValues() needs access to all these - by wrapping them in this struct we
	// can pass the struct's address rather than all these individually. For passing
	// custom data, use the blob parameter.
	struct Data
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> vbackup;
		std::vector<glm::vec4> colors;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> textures;
		std::vector<GLuint> indices;
		std::vector<glm::vec3> normal_visualization_coordinates;
	};

	// This class allows all the vertex attributes to be initialized in a loop
	// rather than one at a time.
	class ShapeInfo
	{
	public:
		ShapeInfo(GLuint c, GLuint h, void * d, GLuint s, GLuint ne, GLuint t, GLuint n) :
			channel_number(c), handle(h), data(d),
			sizeof_element(s), number_of_elements(ne), type(t),
			number_of_type(n)
		{
		}

		GLuint channel_number;
		GLuint handle;
		void * data;
		GLuint sizeof_element;
		GLuint number_of_elements;
		GLuint type;
		GLuint number_of_type;
	};

	enum
	{
		VERTICES = 0,
		COLORS,
		NORMALS,
		TEXTURE_COORDINATES,
		LAST_VALUE
	};

	Shape() {}

	// THIS USED TO BE ABSTRACT. It has been refactored into the Shape class as most implementations are the same.
	// The default call Draw() will draw faces (not normals) made from triangles. If you need something else,  for
	// example with Cube (GL_QUADS), then add the optional arguements.
	virtual void Draw(bool draw_normals = false, GLuint what_to_draw = GL_TRIANGLES);
	virtual void UpdateValues(void(*Update)(struct Data & data, float current_time, void * blob), float current_time, void * blob);

protected:

	std::vector<ShapeInfo> si;
	// This is the OpenGL code provided by this base class responsible
	// for hooking up the user's data to OpenGL.
	virtual bool CommonGLInitialization();
	// This is where the user will initialize their data structures including
	// their vertex attribute arrays.
	virtual bool PreGLInitialize() = 0;
	virtual void NonGLTakeDown() = 0;
	// RecomputeNormals will be called after the user's callback
	// called by UpdateValues. The idea is that the user's callback
	// moves around the vertices, then RecomputeNormals adjusts the
	// normals to be right again. It must be implemented in each shape
	// because that programmer knows how the vertices are connected
	// (which is the basis of recomputing).
	virtual void RecomputeNormals() = 0;
	virtual bool PostGLInitialize();
	virtual void GLTakeDown();

	// All the goodies have been moved into a struct so as to make it more convenient to
	// pass around in UpdateValues(). 
	struct Data data;

	GLuint vertex_array_handle;
	GLuint index_array_handle;
	GLuint normal_array_handle;
	GLuint normal_visualization_handle;

	bool GLReturnedError(char * s);

	glm::vec4 RandomColor(glm::vec4 & previous_color, float min = -0.1f , float max = 0.1f);

	inline float Random(float min , float max)
	{
		return (float(rand()) / float(RAND_MAX)) * (max - min) + min;
	}

};
