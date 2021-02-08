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
#include <algorithm> 

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* Keep the global state inside this struct */
static struct {
	glm::dvec2 mouse_position;
	glm::ivec2 screen_dimensions = glm::ivec2(960, 960);
	GLint key;
	GLint action;
	bool roverCam;
	bool collision;
	
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

/*Functions*/
void checkCollision(glm::vec3 my_rover, glm::vec3 rover2, glm::vec3 rover3);
void print(glm::vec3 vector);
void checkCollision2(glm::vec3 cube1p, glm::vec3 cube1n, glm::vec3 cube2p, glm::vec3 cube2n, glm::vec3 cube3p, glm::vec3 cube3n);

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
	glBlendColor(0.5, 0.5, 0.5, 1);

	/* MESHES CREATION */
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indicies;
	GenerateParametricShapeFrom2D(positions, normals, indicies, uvs, ParametricHalfCircle, 512 ,512 );
	VAO sphereVAO(positions, normals, uvs, indicies);

	positions.clear();
	normals.clear();
	indicies.clear();
	uvs.clear();
	GenerateParametricShapeFrom2D(positions, normals, indicies, uvs, ParametricCircle, 512, 512);
	VAO wheelVAO(positions, normals, uvs, indicies);

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
	{ { 1, -1, -1 }
		,{ 1 , -1 , 1}
	,{ -1, -1, 1}
	,{ -1, -1, -1}
	, {1, 1, -0.999999}
	, {-1, 1, -1}
	, {-1, 1, 1}
	, {0.999999, 1, 1}
	, {1, -1, -1}
	, {1, 1, -0.999999}
	, {0.999999, 1, 1}
	, {1, -1, 1}
	, {1, -1, 1}
	, {0.999999, 1, 1}
	, {-1, 1, 1}
	, {-1, -1, 1}
	, {-1, -1, 1}
	, {-1, 1, 1}
	, {-1, 1, -1}
	, {-1, -1, -1}
	, {1, 1, -0.999999}
	, {1, -1, -1}
	, {-1, -1, -1}
	, {-1, 1, -1} },
	{
	{0,-1,0}
	,{0,-1,0}
	,{0,-1,0}
	,{0,-1,0}
	,{0,1,0}
	,{0,1,0}
	,{0,1,0}
	,{0,1,0}
	,{1,0,0}
	,{1,0,0}
	,{1,0,0}
	,{1,0,0}
	,{-0,-0,1}
	,{-0,-0,1}
	,{-0,-0,1}
	,{-0,-0,1}
	,{-1,-0,-0}
	,{-1,-0,-0}
	,{-1,-0,-0}
	,{-1,-0,-0}
	,{0,0,-1}
	,{0,0,-1}
	,{0,0,-1}
	,{0,0,-1}
	},
	{ {1, 0}
		, {1, 1},
		{0, 1}
		, {0, 0}
		, {0, 0},
		{1, 0}
		, {1, 1}
		, {0, 1}
		, {1, 0}
		, {1, 1}
		, {0, 1}
		, {0, 0}
		, {0, 1}
		, {0, 0}
		, {1, 0}
		, {1, 1}
		, {1, 0}
		, {1, 1}
		, {0, 1}
		, {0, 0}
		, {0, 1}
		, {0, 0}
		, {1, 0}
		, {1, 1} },
	{
			0, 1, 2,
			0, 2, 3,
			4, 5, 6,
			4, 6, 7,
			8,9,10,
			8,10,11,
			12,13,14,
			12,14,15,
			16,17,18,
			16,18,19,
			20,21,22,
			20,22,23	
			}
	);

	//TEXTURES
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

	GLuint mars_texture;
	glGenTextures(1, &mars_texture);

	if (x * n % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, mars_texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x, y, 0, n == 3? GL_RGB:GL_RGBA, GL_UNSIGNED_BYTE, texture_data
	);

	glGenerateMipmap(GL_TEXTURE_2D);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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

	GLuint stars_texture;
	glGenTextures(1, &stars_texture);

	if (x * n2 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, stars_texture);
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

	GLuint rover_texture;
	glGenTextures(1, &rover_texture);

	if (x * n3 % 4 != 0)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}

	glBindTexture(GL_TEXTURE_2D, rover_texture);
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

	GLuint clear_texture;
	glGenTextures(1, &clear_texture);

	glBindTexture(GL_TEXTURE_2D, clear_texture);
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
	
	//WHEEL
	auto filename5 = "wheel.jpg";

	int x5, y5, n5;
	unsigned char *texture_data5 = stbi_load(filename5, &x5, &y5, &n5, 0);

	GLuint wheel_texture;
	glGenTextures(1, &wheel_texture);

	glBindTexture(GL_TEXTURE_2D, wheel_texture);
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		x5, y5, 0, GL_RGB, GL_UNSIGNED_BYTE, texture_data5
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	stbi_image_free(texture_data5);


	/* Creating Programs */	
	GLuint program = CreateProgramFromSources(
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
uniform sampler2D u_texture;

in vec3 vertex_position;
in vec3 vertex_normal;
in vec2 vertex_uvs;

out vec4 out_color;

void main()
{
/*
	vec4 color = texture(u_texture, vertex_uvs).rgba;
	if (color.a < 0.1)
		discard;

	vec3 ambient = 0.05 * color.rgb;
	vec3 light_direction = -normalize(normalize(vec3(-1, -1, 1)) - vertex_position);
	vec3 surface_normal = normalize(vertex_normal);
	float diffuse_intensity = max(0, dot(light_direction, surface_normal));
	vec3 diffuse = diffuse_intensity * color.rgb;
	vec3 view_dir = vec3(0, 0, -1);	
	vec3 reflectDir = reflect(-lightcwasd_direction, surface_normal);
	float spec = 0.0;
    spec = pow(max(dot(view_dir, reflectDir), 0.0), 8.0);
    

	vec3 specular = vec3(0.3) * spec; 
	out_color = vec4(ambient + diffuse + specular, color.a);

*/
	vec4 color = vec4(0);

	vec3 surface_position = vertex_position;
	vec3 surface_normal = normalize(vertex_normal);
	vec2 surface_uvs = vertex_uvs;

	vec4 surface_color = texture(u_texture, surface_uvs).rgba;
	if (surface_color.a < 0.1)
		discard;
	vec3 ambient_color = vec3(1);
	color += vec4(ambient_color,1) * surface_color;

/*
	vec3 light_direction = normalize(vec3(-1, -1, 1));
	vec3 to_light = -normalize(light_direction - surface_position);
	vec3 light_color = vec3(0.5);

	float diffuse_intensity = max(0, dot(to_light, surface_normal));
	color += diffuse_intensity * vec4(light_color,1) * surface_color;
*/
	out_color = color;
}
		)FRAGMENT");

	if (program == NULL)
	{
		glfwTerminate();
		return -1;
	}

	glUseProgram(program);

	auto texture_location = glGetUniformLocation(program, "u_texture");
	glUniform1i(texture_location, 0);
	auto mouse_location = glGetUniformLocation(program, "u_mouse_position");
	auto u_transform_location = glGetUniformLocation(program, "u_transform");

	glm::mat4 rover_rotate(1.0);
	glm::mat4 rover_rotate2(1.0);
	glm::mat4 rover_rotate3(1.0);

	glm::vec3 my_rover_pos(3.0);
	glm::vec3 rover2_pos(3.0);
	glm::vec3 rover3_pos(3.0);

	float rot_x = 0.0f, rot_y = 0.0f;
	float cam_x = 0.0f,cam_y=0.0f, cam_z = -3.0f;
	Globals.roverCam = false;

	glm::dvec3 chasing_pos = glm::dvec3(-1.05,0,0);

	float difference_x = 10;
	float difference_y = 10;

	float chasing1 = 0;
	float chasing2 = 0;
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0,0,0, 1);

		//Projection style
		auto projection = glm::perspective(glm::radians(45.f), 1.f, 0.1f, 10.f);
		
		//MOUSE
		glm::dvec2 normalized_mouse = Globals.mouse_position / glm::dvec2(Globals.screen_dimensions);
		normalized_mouse.y = 1. - normalized_mouse.y;
		normalized_mouse.x = normalized_mouse.x * 2. - 1.;
		normalized_mouse.y = normalized_mouse.y * 2. - 1.;

		//MARS TRANSFORMATION		
		/*Mars matrix scales it by 0.7*/   
		auto mars_transform = glm::scale(glm::vec3(0.7));

		//WHEEL TRANSFORM
		glm::mat4 FL_wheel_transform(1.0);
		FL_wheel_transform = glm::scale(glm::vec3(0.35));
		FL_wheel_transform = glm::translate(glm::vec3(0.5, 0.5, -0.35)) * FL_wheel_transform;
		FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));

		glm::mat4 FR_wheel_transform(1.0);
		FR_wheel_transform = glm::scale(glm::vec3(0.35));
		FR_wheel_transform = glm::translate(glm::vec3(-0.5, 0.5, -0.35)) * FR_wheel_transform;
		FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));

		glm::mat4 BL_wheel_transform(1.0);
		BL_wheel_transform = glm::scale(glm::vec3(0.35));
		BL_wheel_transform = glm::translate(glm::vec3(0.5, -0.5, -0.35)) * BL_wheel_transform;
		BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));

		glm::mat4 BR_wheel_transform(1.0);
		BR_wheel_transform = glm::scale(glm::vec3(0.35));
		BR_wheel_transform = glm::translate(glm::vec3(-0.5, -0.5, -0.35)) * BR_wheel_transform;
		BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(90.f), glm::vec3(0, 0, 1));

		//ROVER TRANSFORMATION
		//rover transform scales by 0.08, translates to outside of mars and rotates based on key press
		glm::mat4 rover_transform(1.0);
		rover_transform = glm::scale(glm::vec3(0.08));
		rover_transform = glm::translate(glm::vec3(1.05, 0, 0)) * rover_transform;

		glm::mat4 rotation(1.0);

		int speed = 2;
		if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_W && !Globals.collision)
		{
			rotation = glm::rotate(glm::radians(float(speed)), glm::vec3(0, 0, 1));
			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime()*50)), glm::vec3(0, 1, 0)) ;
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));

		}
		if (Globals.key == GLFW_KEY_S && Globals.action != GLFW_RELEASE && !Globals.collision)
		{
			rotation = glm::rotate(glm::radians(float(-speed)), glm::vec3(0, 0, 1));
			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * -50)), glm::vec3(0, 1, 0));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * -50)), glm::vec3(0, 1, 0));
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * -50)), glm::vec3(0, 1, 0));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * -50)), glm::vec3(0, 1, 0));
		}
		if (Globals.action != GLFW_RELEASE && Globals.key == GLFW_KEY_A && !Globals.collision)
		{
			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(-20.f), glm::vec3(0, 0, 1));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(-20.f), glm::vec3(0, 0, 1));
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(-20.f), glm::vec3(0, 0, 1));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(-20.f), glm::vec3(0, 0, 1));


			rotation = glm::rotate(glm::radians(float(speed)), glm::vec3(0, 1, 0));
			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));

		}
		if (Globals.key == GLFW_KEY_D && Globals.action != GLFW_RELEASE && !Globals.collision)
		{
			rotation = glm::rotate(glm::radians(float(-speed)), glm::vec3(0, 1, 0));
			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(20.f), glm::vec3(0, 0, 1));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(20.f), glm::vec3(0, 0, 1));
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(20.f), glm::vec3(0, 0, 1));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(20.f), glm::vec3(0, 0, 1));

			FL_wheel_transform = FL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BR_wheel_transform = BR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			FR_wheel_transform = FR_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
			BL_wheel_transform = BL_wheel_transform * glm::rotate(glm::radians(float(glfwGetTime() * 50)), glm::vec3(0, 1, 0));
		}

		rover_rotate = rover_rotate * rotation;
		rover_transform = rover_rotate * rover_transform;

		//CAMERAS TRANSFORMATION

		//rover cam

		glm::vec3 camera_position = mars_transform * rover_transform * glm::vec4(3, -2, 0, 1);
		my_rover_pos = mars_transform * rover_transform * glm::vec4(1.05, 0, 0, 1);
		glm::vec3 rover_position_mouse = mars_transform * rover_transform * glm::vec4(1.05 + normalized_mouse.y, 0, 0 + normalized_mouse.x, 1);
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
			camera_transform = glm::lookAt(
				glm::vec3(camera_position),
				glm::vec3(rover_position_mouse),
				glm::vec3(up_direction)
			);
		}

		//ROVER 2 -- moves automatically
		
		glm::mat4 rotation2(1.0);

		glm::mat4 rover_transform2(1.0);
		rover_transform2 = glm::scale(glm::vec3(0.08));
		rover_transform2 = glm::translate(glm::vec3(-1.05, 0, 0)) * rover_transform2;
		if (!Globals.collision)
		{
			rotation2 = glm::rotate(glm::radians(float(0.5)), glm::vec3(0, 0, 1));
		}
		rover_rotate2 = rover_rotate2 * rotation2;
		rover_transform2 = rover_rotate2 * rover_transform2;

		rover2_pos = mars_transform * rover_transform2 * glm::vec4(-1.05, 0, 0, 1);

		//ROVER 3

		glm::mat4 rotation3(1.0);

		glm::mat4 rover_transform3(1.0);
		rover_transform3 = glm::scale(glm::vec3(0.08));
		rover_transform3 = glm::translate(glm::vec3(0, 0, -1.05)) * rover_transform3;

		difference_x = my_rover_pos.x - rover3_pos.x;
		difference_y = my_rover_pos.y - rover3_pos.y;

		//int y_angle = my_rover_pos.y - rover3_pos.y;
		float y_angle = atan2(my_rover_pos.z, my_rover_pos.y);// -atan2(rover3_pos.y, rover3_pos.y));
		float z_angle = atan2(my_rover_pos.x, my_rover_pos.z);// -atan2(rover3_pos.z, rover3_pos.z));

		glm::mat4 d(1.0);

		if (!Globals.collision)
		{
			chasing1 = glm::mix(y_angle,0.f , 0.99f);
			chasing2 = glm::mix(z_angle,0.f , 0.99f);
			rotation3 = glm::rotate(y_angle*0.001f, glm::vec3(0, 0, 1));
			if (rotation3 != d)
				rotation3 =glm::rotate(z_angle*0.001f, glm::vec3(0, 1, 0)) * rotation3;
			else if (z_angle != 0)
				rotation3 =  glm::rotate(z_angle*0.001f, glm::vec3(0, 1, 0));
			if (y_angle > 0) {}
		}
		/*
		if (!Globals.collision && (my_rover_pos.z * rover3_pos.z > 0) && (abs(difference_x)>0.06))
		{
			if (difference_x < 0)
				rotationx = glm::rotate(glm::radians(float(0.5)), glm::vec3(0, 1, 0));
			else if (difference_x > 0 )
				rotationx = glm::rotate(glm::radians(float(-0.5)), glm::vec3(0, 1, 0));
		}

		if (!Globals.collision && (my_rover_pos.z * rover3_pos.z > 0) && (abs(difference_y) > 0.06))
		{
			if (rover3_pos.x < 0)
			{
				if (difference_y < 0 )
					rotationy = glm::rotate(glm::radians(float(0.5)), glm::vec3(0, 0, 1));
				else if (difference_y >= 0)
					rotationy = glm::rotate(glm::radians(float(-0.5)), glm::vec3(0, 0, 1));
			}
			else
			{
				if (difference_y < 0)
					rotationy = glm::rotate(glm::radians(float(-0.5)), glm::vec3(0, 0, 1));
				else if (difference_y >= 0)
					rotationy = glm::rotate(glm::radians(float(0.5)), glm::vec3(0, 0, 1));
			}
		}

		if (rotationx == glm::mat4(1.0))
			rover_rotate3 = rover_rotate3 * rotationy;
		else
			rover_rotate3 = rover_rotate3 * rotationy * rotationx;
*/
		rover_rotate3 = rover_rotate3 * rotation3;
		rover_transform3 = rover_rotate3 * rover_transform3;

		rover3_pos = mars_transform * rover_transform3 *  glm::vec4(0,0, - 1.05, 1);

		
		//MARS
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mars_texture);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * mars_transform));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(normalized_mouse)));
		glBindVertexArray(sphereVAO.id);
		glDrawElements(GL_TRIANGLES, sphereVAO.element_array_count, GL_UNSIGNED_INT, NULL);

		rover_transform = rover_transform * glm::rotate(glm::radians(90.f), glm::vec3(0, 1, 0));
		rover_transform2 = rover_transform2 * glm::rotate(glm::radians(270.f), glm::vec3(0, 1, 0));
		rover_transform3 = rover_transform3 * glm::rotate(glm::radians(180.f), glm::vec3(0, 1, 0));
		rover_transform3 = rover_transform3 * glm::rotate(glm::radians(90.f), glm::vec3(0,0,1)) ;


		auto drawRover = [&](glm::mat4 modelMatrix)
		{
			//ROVER
			glBindTexture(GL_TEXTURE_2D, rover_texture);
			glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(modelMatrix * glm::scale(glm::vec3(0.5))));
			glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(normalized_mouse)));
			glBindVertexArray(cubeVAO.id);
			glDrawElements(GL_TRIANGLES, cubeVAO.element_array_count, GL_UNSIGNED_INT, NULL);

			//WHEELS
			glBindTexture(GL_TEXTURE_2D, wheel_texture);
			glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(modelMatrix* FL_wheel_transform));
			glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(normalized_mouse)));
			glBindVertexArray(wheelVAO.id);
			glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);

			glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(modelMatrix * FR_wheel_transform));
			glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);

			glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(modelMatrix * BR_wheel_transform));
			glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);

			glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(modelMatrix * BL_wheel_transform));
			glDrawElements(GL_TRIANGLES, wheelVAO.element_array_count, GL_UNSIGNED_INT, NULL);
		};

		drawRover(projection * camera_transform * mars_transform * rover_transform);
		drawRover(projection * camera_transform * mars_transform * rover_transform2);
		drawRover(projection * camera_transform * mars_transform * rover_transform3);


		float scaleFactor = 0.055;
		glm::vec3 myCubePosn = glm::translate(my_rover_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(-1, -1, -1, 1);
		glm::vec3 myCubePosp = glm::translate(my_rover_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(1,1,1,1);
		glBindTexture(GL_TEXTURE_2D, clear_texture);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * glm::translate(my_rover_pos) * glm::scale(glm::vec3(scaleFactor))));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(0, 0)));
		glBindVertexArray(cubeVAO.id);
		glDrawElements(GL_TRIANGLES, cubeVAO.element_array_count, GL_UNSIGNED_INT, NULL);


		glm::vec3 myCubePos3n = glm::translate(rover3_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(-1, -1, -1, 1);
		glm::vec3 myCubePos3p = glm::translate(rover3_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(1, 1, 1, 1);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * glm::translate(rover3_pos) * glm::scale(glm::vec3(scaleFactor))));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(0, 0)));
		glDrawElements(GL_TRIANGLES, cubeVAO.element_array_count, GL_UNSIGNED_INT, NULL);

		glm::vec3 myCubePos2n = glm::translate(rover2_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(-1, -1, -1, 1);
		glm::vec3 myCubePos2p = glm::translate(rover2_pos) * glm::scale(glm::vec3(scaleFactor)) * glm::vec4(1, 1, 1, 1);
		glUniformMatrix4fv(u_transform_location, 1, GL_FALSE, glm::value_ptr(projection * camera_transform * glm::translate(rover2_pos) * glm::scale(glm::vec3(scaleFactor))));
		glUniform2fv(mouse_location, 1, glm::value_ptr(glm::vec2(0, 0)));
		glDrawElements(GL_TRIANGLES, cubeVAO.element_array_count, GL_UNSIGNED_INT, NULL);

		checkCollision2(myCubePosn, myCubePosp, myCubePos2n, myCubePos2p, myCubePos3n, myCubePos3p);

		//STARS
		glBindTexture(GL_TEXTURE_2D, stars_texture);
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

void checkCollision2(glm::vec3 cube1p, glm::vec3 cube1n, glm::vec3 cube2p, glm::vec3 cube2n, glm::vec3 cube3p, glm::vec3 cube3n)
{
	float minX = std::min(cube1p.x, cube1n.x);
	float maxX = std::max(cube1p.x, cube1n.x);
	float minY = std::min(cube1p.y, cube1n.y);
	float maxY = std::max(cube1p.y, cube1n.y);
	float minZ = std::min(cube1p.z, cube1n.z);
	float maxZ = std::max(cube1p.z, cube1n.z);

	float minX2 = std::min(cube2p.x, cube2n.x);
	float maxX2 = std::max(cube2p.x, cube2n.x);
	float minY2 = std::min(cube2p.y, cube2n.y);
	float maxY2 = std::max(cube2p.y, cube2n.y);
	float minZ2 = std::min(cube2p.z, cube2n.z);
	float maxZ2 = std::max(cube2p.z, cube2n.z);

	float minX3 = std::min(cube3p.x, cube3n.x);
	float maxX3 = std::max(cube3p.x, cube3n.x);
	float minY3 = std::min(cube3p.y, cube3n.y);
	float maxY3 = std::max(cube3p.y, cube3n.y);
	float minZ3 = std::min(cube3p.z, cube3n.z);
	float maxZ3 = std::max(cube3p.z, cube3n.z);

	if ((minX <= maxX2 && maxX >= minX2) &&
		(minY <= maxY2 && maxY >= minY2) &&
		(minZ <= maxZ2 && maxZ >= minZ2))
	{
		Globals.collision = true;
	}
	else if ((minX <= maxX3 && maxX >= minX3) &&
		(minY <= maxY3 && maxY >= minY3) &&
		(minZ <= maxZ3 && maxZ >= minZ3))
	{
		Globals.collision = true;
	}
	else
	{
		Globals.collision = false;
	}

	

};


void checkCollision(glm::vec3 my_rover, glm::vec3 rover2, glm::vec3 rover3)
{
	GLfloat distance1 = glm::pow((glm::pow((my_rover.x - rover2.x), 2) + glm::pow((my_rover.y - rover2.y), 2) + glm::pow((my_rover.z - rover2.z), 2)), 0.5);
	GLfloat distance2 = glm::pow((glm::pow((my_rover.x - rover3.x), 2) + glm::pow((my_rover.y - rover3.y), 2) + glm::pow((my_rover.z - rover3.z), 2)), 0.5);

	if (distance1 > 0.11 && distance2 > 0.1)
		Globals.collision = false;
	else {
		Globals.collision = true;
	};

}

void print(glm::vec3 vector)
{
	std::cout << vector.x << "  " << vector.y << "   " << vector.z << std::endl; 
}

