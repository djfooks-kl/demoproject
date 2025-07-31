#version 300 es
precision mediump float;

layout (location = 0) in vec2 position;
layout (location = 1) in float inTextureIndex;
out float textureIndex;

uniform vec2 transform;

void main()
{
    gl_Position = vec4(position + transform, 0.0, 1.0);
    textureIndex = inTextureIndex;
}