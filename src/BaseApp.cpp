#include "BaseApp.h"

#include <array>
#include <chrono>
#include <iostream>

#include "GLFWLib.h"

namespace
{
	void key_callback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (key == GLFW_KEY_W && action == GLFW_PRESS)
			std::cout << "yay!\n";
	}

	void error_callback(int /*error*/, const char* description)
	{
		std::cerr << "Error: " << description << "\n";
	}
}

void BaseApp::Update()
{
	glfwPollEvents();
}

bool BaseApp::Run()
{
	Init();
	if (!glfwInit())
	{
		std::cout << "Initialization failed\n";
		return false;
	}

	glfwSetErrorCallback(error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
	if (!window)
	{
		return false;
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	RunInternal(window);
	glfwDestroyWindow(window);
	glfwTerminate();
	return true;
}