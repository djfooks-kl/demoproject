#include <iostream>
#include <chrono>
#include <thread>

#ifndef EMSCRIPTEN
 #include <glad/gles2.h>
 #define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace
{
	void error_callback(int /*error*/, const char* description)
	{
		fprintf(stderr, "Error: %s\n", description);
	}

	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

bool InitWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window)
	{
		return false;
	}

	glfwMakeContextCurrent(window);
#ifndef EMSCRIPTEN
	gladLoadGLES2(glfwGetProcAddress);
#endif
	glfwSwapInterval(1);

	glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window))
	{
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	return true;
}

int main()
{
	if (!glfwInit())
	{
		std::cout << "Initialization failed\n";
		return 1;
	}

	glfwSetErrorCallback(error_callback);

	InitWindow();

	glfwTerminate();
	return 0;
}