#pragma once

typedef unsigned int GLuint;

class Demo
{
public:
	void Init();
	void Update(double time, float deltaTime);

private:
    GLuint m_Program;
    GLuint m_VBO;
    GLuint m_Texture;
};