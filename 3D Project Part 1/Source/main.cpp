#include <iostream>
#include <vector>

#define GLM_FORCE_LEFT_HANDED
#include "GLM/glm.hpp"
#include "GLM/common.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include "GLM/gtc/random.hpp"
#include "GLAD/glad.h"
#include "GLFW/glfw3.h"
#include "opengl_utilities.h"
#include "mesh_generation.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* Keep the global state inside this struct */
static struct {
	glm::dvec2 mouse_position;
	glm::ivec2 screen_dimensions = glm::ivec2(960, 960);
	GLint key;
	GLint action;
	bool roverCam;
	
} Globals;

/* GLFW Callback functions */
static void ErrorCallback(int error, const char* description)
{
	std::cerr << "Error: " << description << std::endl;
}

static void CursorPositionCallback(GLFWwindow* window, double x, double y)
{
	Globals.mouse_position.x = x;
	Globals.mouse_position.y = y;
}

static void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
	Globals.screen_dimensions.x = width;
	Globals.screen_dimensions.y = height;

	glViewport(0, 0, width, height);
}

static void keyPressedCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	Globals.key = key;
	Globals.action = action;

	if (key != GLFW_KEY_W || key != GLFW_KEY_S || key != GLFW_KEY_A || key != GLFW_KEY_D ||
		key != GLFW_KEY_R || key != GLFW_KEY_F || key != GLFW_KEY_UP || key != GLFW_KEY_DOWN ||
		key != GLFW_KEY_RIGHT || key != GLFW_KEY_LEFT)
	{
		if (key == GLFW_KEY_C && Globals.action == GLFW_RELEASE)
		{
			Globals.roverCam = !Globals.roverCam;
		}
	}
}

int main(int argc, char* argv[])
{
	/* Set GLFW error callback */
	glfwSetErrorCallback(ErrorCallback);

	/* Initialize the library */
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	/* Create a windowed mode window and its OpenGL context */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(
		Globals.screen_dimensions.x, Globals.screen_dimensions.y,
		"Ranem Elshanawany", NULL, NULL
	);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	/* Move window to a certain position [do not change] */
	glfwSetWindowPos(window, 10, 50);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	/* Enable VSync */
	glfwSwapInterval(1);

	/* Load OpenGL extensions with GLAD */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;

	}

	/* Set GLFW Callbacks */
	glfwSetCursorPosCallback(window, CursorPositionCallback);
	glfwSetWindowSizeCallback(window, WindowSizeCallback);
	glfwSetKeyCallback(window, keyPressedCallback);

	/* Configure OpenGL */
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* Creating Meshes */
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indicies;
	GenerateParametricShapeFrom2D(positions, normals, indicies, uvs, ParametricHalfCircle, 512 ,512 );
	VAO sphereVAO(positions, normals, uvs, indicies);

	VAO quadVAO(
	{
		{ -3, -3, 1.9 },
		{ +3, -3, 1.9 },
		{ +3, +2, 1.9 },
		{ -3, +3, 1.9 }
	},
	{
		{ 0, 0, -1 },
		{ 0, 0, -1 },
		{ 0, 0, -1 },
		{ 0, 0, -1 }
	},
	{
		{ 0, 0 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0, 1 }
	},
	{
		0, 1, 2,
		0, 2, 3
	}
	);

	VAO cubeVAO(
	{
		{ -0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, 0.5f, -0.5f },
		{ 0.5f, 0.5f, -0.5f },
		{ -0.5f, 0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, 0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ -0.5f, -0.5f, 0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ -0.5f, 0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, -0.5f, 0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, -0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ 0.5f, -0.5f, 0.5f },
		{ -0.5f, -0.5f, 0.5f },
		{ -0.5f, -0.5f, -0.5f },
		{ -0.5f, 0.5f, -0.5f },
		{ 0.5f, 0.5f, -0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ -0.5f, 0.5f, 0.5f },
		{ -0.5f, 0.5f, -0.5f }
	},
	{
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ 0.0f, 0.0f, 1.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	},
	{ { 0.25f, 0.25f},
	{ 0.75f, 0.25f},
	{ 0.75f, 0.75f},
	{ 0.75f, 0.75f},
	{ 0.25f, 0.75f},
	{ 0.25f, 0.25f },
	{ 0.25f, 0.25f },
	{ 0.75f, 0.25f },
	{ 0.75f, 0.75f},
	{ 0.75f, 0.75f},
	{ 0.25f, 0.75f},
	{ 0.25f, 0.25f },
	{ 0.25f, 0.75f},
	{ 0.25f, 0.75f},
	{ 0.25f, 0.25f },
	{ 0.25f, 0.25f },
	{ 0.25f, 0.25f },
	{ 0.25f, 0.75f},
	{ 0.75f, 0.75f},
	{ 0.75f, 0.75f},
	{ 0.75f, 0.25f},
	{ 0.75f, 0.25f},
	{ 0.75f, 0.25f},
	{ 0.75f, 0.75f},
	{ 0.25f, 0.25f },
	{ 0.75f, 0.25f},
	{ 0.75f, 0.25f },
	{ 0.75f, 0.25f},
	{ 0.25f, 0.25f },
	{ 0.25f, 0.25f },
	{ 0.25f, 0.75f},
	{ 0.75f, 0.75f},
	{ 0.75f, 0.75f },
	{ 0.75f, 0.75f},
	{ 0.25f, 0.75f },
	{ 0.25f, 0.75f} },
	{
		0, 1, 2,
		3, 4, 5,
		6, 7, 8,
		9, 10, 11,
		12, 13, 14,
		15, 16, 17,
		18, 19, 20,
		21, 22, 23,
		24, 25, 26,
		27, 28, 29,
		30, 31, 32,
		33, 34, 35
	}
	);

	auto filename = "texture.jpg";
	auto filename2 = "starryskylarge2.jpg";
	auto filename3 = "rover2.jpg";

	stbi_set_flip_vertically_on_load(true);
	int x, y, n;
	int x2, y2, n2;
	int x3, y3, n3;
	unsigned char *texture_data = stbi_load(filename, &x, &y, &n, 0);

	if (texture_data == NULL)
	{
		std::cout << "Texture " << filename << " failed to load." << std::endl;
		std::cout << "Error: " << stbi_failure_reason() << std::endl;
	}
	else
	{
		std::cout << "Success: Loading Texture Completed. X:" << x << " Y: " << y << " N:" << n << std::endl;
	}

	GLuint texture;
	glGenTextures(1, &texture);

	if (x * n % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x, y, 0, n == 3? GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, texture_data
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);


	if (x * n % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	stbi_image_free(texture_data);

	unsigned char *texture_data2 = stbi_load(filename2, &x2, &y2, &n2, 0);
	if (texture_data2 == NULL)
	{
		std::cout << "Texture " << filename2 << " failed to load." << std::endl;
		std::cout << "Error: " << stbi_failure_reason() << std::endl;
	}
	else
	{
		std::cout << "Success: Loading Texture Completed. X:" << x2 << " Y: " << y2 << " N:" << n2 << std::endl;
	}

	GLuint texture2;
	glGenTextures(1, &texture2);

	if (x * n2 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x2, y2, 0, n2 == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture_data2
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if (x * n2 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	stbi_image_free(texture_data2);

	unsigned char *texture_data3 = stbi_load(filename3, &x3, &y3, &n3, 0);

	GLuint texture3;
	glGenTextures(1, &texture3);

	if (x * n3 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, texture3);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x3, y3, 0, n3 == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, texture_data3
	);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	if (x * n3 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	stbi_image_free(texture_data3);

	//TRANSPARENT TEXTURE
	auto filename4 = "empty.png";

	int x4, y4, n4;
	unsigned char *texture_data4 = stbi_load(filename4, &x4, &y4, &n4, 0);

	GLuint texture4;
	glGenTextures(1, &texture4);

	glBindTexture(GL_TEXTURE_2D, texture4);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x4, y4, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data4
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	stbi_image_free(texture_data4);


	/* Creating Programs */	
	GLuint color = CreateProgramFromSources(
		R"VERTEX(
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uvs;


uniform mat4 u_transform;

out vec3 vertex_position;
out vec3 vertex_normal;
out vec2 vertex_uvs;

void main()
{
	gl_Position = u_transform * vec4(a_position, 1);
	vertex_normal = vec3(u_transform * vec4(a_normal, 0));
	vertex_position = vec3(gl_Position);
	vertex_uvs = a_uvs;
}
		)VERTEX",

		R"FRAGMENT(
#version 330 core

uniform vec2 u_mouse_position;
//uniform vec3 u_color;
uniform sampler2D u_texture;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_uvs;

out vec4 out_color;

void main()
{
	vec3 color = vec3(0);

	vec3 surface_position = vertex_position;
	vec3 surface_normal = normalize(vertex_normal);
	vec2 surface_uvs = vertex_uvs;

	vec4 surface_color = texture(u_texture, surface_uvs).rgba;
	if (surface_color.a < 0.1)
		discard;
	/*vec3 ambient_color = vec3(0.7);
	color += ambient_color * surface_color;*/

/*	vec3 light_direction = normalize(vec3(-1, -1, 1));
	vec3 to_light = -normalize(light_direction - surface_position);
	vec3 light_color = vec3(0.5);

	float diffuse_intensity = max(0, dot(to_light, surface_normal));
	color += diffuse_intensity * light_color * surface_color;*/

	out_color = vec4(surface_color);
}
		)FRAGMENT");

	if (color == NULL)
	{
		glfwTerminate();
		return -1;
	}

	glUseProgram(color);
	auto texture_location = glGetUniformLocation(color, "u_texture");
	glUniform1i(texture_location, 0);

	auto mouse_location = glGetUniformLocation(color, "u_mouse_position");
	auto u_transform_location = glGetUniformLocation(color, "u_transform");

	glm::mat4 rover_rotate(1.0); 
	float rot_x = 0.0f, rot_y = 0.0f;
	float cam_x = 0.0f,cam_y=0.0f, cam_z = -3.0f;
	Globals.roverCam = false;


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0,0,0, 1);

		//MOUSE
		glm::dvec2 normalized_mouse = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
		normalized_mouse.y = 1. - normalized_mouse.y;
		normalized_mouse.x = normalized_mouse.x * 2. - 1.;
		normalized_mouse.y = normalized_mouse.y * 2. - 1.;

		auto projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 10.f);

		//MARS TRANSFORMATION		
		/*Mars matrix moves everything to -0.5 on z axis, and scales it by half*/
		glm::mat4 translate = glm::translate(glm::vec3(0, 0, 0));       
		auto mars_transform = translate *  glm::scale(glm::vec3(0.5));

		//ROVER TRANSFORMATION
		//rover transform scales, translates and rotates based on mars based on mars
		glm::mat4 rover_transform(1.0);
		rover_transform = glm::scale(glm::vec3(0.08));
		rover_transform = glm::translate(glm::vec3(1, 0, 0)) * rover_transform;

		glm::mat4 rotation(1.0);

		if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_W)
		{
			rotation = glm::rotate(glm::radians(float(5.0)), glm::vec3(0, 0, 1));
		}
		if (Globals.key == GLFW_KEY_S && Globals.action != GLFW_RELEASE)
		{
			rotation = glm::rotate(glm::radians(float(-5.0)), glm::vec3(0, 0, 1));
		}
		if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_A)
		{
			rotation = glm::rotate(glm::radians(float(5.0)), glm::vec3(0, 1, 0));

		}
		if (Globals.key == GLFW_KEY_D && Globals.action != GLFW_RELEASE)
		{
			rotation = glm::rotate(glm::radians(float(-5.0)), glm::vec3(0, 1, 0));
		}

		rover_rotate = rover_rotate * rotation;
		rover_transform = rover_rotate * rover_transform;

		//CAMERAS TRANSFORMATION
		glm::vec3 camera_position = mars_transform * rover_transform * glm::vec4(3, -2, 0, 1);
		glm::vec3 rover_position = mars_transform * rover_transform * glm::vec4(1, 0, 0, 1);
		glm::vec3 up_direction(0, 1, 0);
		
		glm::mat4 camera_transform(1.0);
		if (!Globals.roverCam)
		{
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_RIGHT)
			{
				cam_x += 0.01;
			}
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_LEFT)
			{
				cam_x -= 0.01;
			}
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_UP)
			{
				cam_z += 0.01;
			}
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_DOWN)
			{
				cam_z -= 0.01;
			}
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_R)
			{
				cam_y += 0.01;
			}
			if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_F)
			{
				cam_y -= 0.01;
			}

			camera_transform = glm::lookAt(
				glm::vec3(cam_x, cam_y, cam_z),
				glm::vec3(normalized_mouse, 0),
				glm::vec3(0, 1, 0));
		}
		else
		{
			/*if (camera_position.x < 0) {
				up_direction.y = -1;
				std::cout << "yoi issoyo" << std::endl;
			}
			else
			{
				up_direction.y = 1;
			}*/
			camera_transform = glm::lookAt(
				glm::vec3(camera_position),
				glm::vec3(rover_position),
				glm::vec3(up_direction)
			);
		}

		//MARS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * mars_transform));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(normalized_mouse)));
		glBindVertexArray(sphereVAO.id);
		glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);



		//rover_transform = glm::rotate(glm::radians(90.f), glm::vec3(1, 0, 0)) * rover_transform;
		rover_transform = rover_transform * glm::rotate(glm::radians(90.f), glm::vec3(0,1,0)) ;
		//ROVER
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture3);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * mars_transform * rover_transform));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(normalized_mouse)));
		glBindVertexArray(cubeVAO.id);
		glDrawElements(GL_TRIANGLES, cubeVAO.element_array_count, GL_UNSIGNED_INT, NULL);

		//STARS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);
		glm::mat4 background(1.0);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * background));
		glBindVertexArray(quadVAO.id);
		glDrawElements(GL_TRIANGLES, quadVAO.element_array_count, GL_UNSIGNED_INT, NULL);
		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

