#version 300 es
precision mediump float;

in float textureIndex;
out vec4 FragColor;

uniform sampler2D ourTexture;

void main()
{
    vec4 color = texture(ourTexture, vec2(textureIndex, 0.0));
    FragColor = color;
}
