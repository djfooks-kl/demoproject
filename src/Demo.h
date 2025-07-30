#pragma once

#include <flecs/flecs.h>

typedef unsigned int GLuint;

class Demo
{
public:
	void Init();
	void Update(double time, float deltaTime);

private:
    flecs::world m_World;

    GLuint m_Program;
    GLuint m_VBO;
    GLuint m_Texture;

    bool m_ContextMenuOpen = false;
};