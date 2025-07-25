#include "WindowApp.h"

#include <iostream>

#include "GLFWLib.h"

WindowApp::~WindowApp()
{
}

WindowApp::WindowApp()
	: BaseApp()
{
}

bool WindowApp::Init()
{
	glfwSwapInterval(1);
    return true;
}

bool WindowApp::RunInternal(GLFWwindow* window)
{
	printf("Running WindowApp...\n");
	while (!glfwWindowShouldClose(window))
	{
		Update();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return true;
}
