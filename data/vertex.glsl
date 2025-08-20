#version 300 es
precision mediump float;

layout (location = 0) in vec2 position;
layout (location = 1) in vec2 inTextureUV;
out vec2 textureUV;

uniform vec2 transform;

uniform float u_fontSize;

void main()
{
    gl_Position = vec4((position + transform) * u_fontSize, 0.0, 1.0);
    textureUV = inTextureUV;
}