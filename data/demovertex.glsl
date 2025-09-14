#version 300 es
precision mediump float;

layout (location = 0) in vec2 position;
layout (location = 1) in float inTextureIndex;
out float textureIndex;

uniform mat4 viewProjection;

void main()
{
    gl_Position = viewProjection * vec4(position, 0.0, 1.0);
    textureIndex = inTextureIndex;
}