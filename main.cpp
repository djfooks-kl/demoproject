#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>

#ifdef EMSCRIPTEN
	#include <emscripten/emscripten.h>
	#include <emscripten/html5.h>
	#include <GLES2/gl2.h>
#else
	#include <glad/gles2.h>
	#define GLFW_INCLUDE_NONE
	#include <GLFW/glfw3.h>
#endif
namespace
{
	struct GLUserData
	{
	    GLuint m_Program;
	    GLuint m_VBO;
	    GLuint m_Texture;

	    std::chrono::steady_clock::time_point m_LastFrameTime;
	    double m_Time = 0.0;
	};
}

void Update(GLUserData& /*glUserData*/)
{
	std::cout << "Hello world!\n";
}

#ifdef EMSCRIPTEN
namespace
{
	void EmscriptenUpdate(void* userData)
	{
		GLUserData& glUserData = *static_cast<GLUserData*>(userData);
		Update(glUserData);
	}
}
#else
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

	bool InitWindow(GLUserData& glUserData)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		GLFWwindow* window = glfwCreateWindow(640, 480, "My Title", NULL, NULL);
		if (!window)
		{
			return false;
		}

		glfwMakeContextCurrent(window);
		gladLoadGLES2(glfwGetProcAddress);
		glfwSwapInterval(1);

		glfwSetKeyCallback(window, key_callback);

		while (!glfwWindowShouldClose(window))
		{
			Update(glUserData);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		glfwDestroyWindow(window);
		return true;
	}
}
#endif

int main()
{
    GLUserData glUserData;
    glUserData.m_LastFrameTime = std::chrono::steady_clock::now();

#ifdef EMSCRIPTEN
    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.alpha = 0;
    attr.depth = 1;
    attr.stencil = 0;
    attr.antialias = 1;
    attr.majorVersion = 2;
    attr.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);
    emscripten_webgl_make_context_current(ctx);

	emscripten_set_main_loop_arg(EmscriptenUpdate, static_cast<void*>(&glUserData), 0, 1);
#else
	if (!glfwInit())
	{
		std::cout << "Initialization failed\n";
		return 1;
	}

	glfwSetErrorCallback(error_callback);
	InitWindow(glUserData);
	glfwTerminate();
#endif
	return 0;
}