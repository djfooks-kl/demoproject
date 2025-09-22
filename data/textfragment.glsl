#version 300 es
precision mediump float;

in vec2 textureUV;
in vec3 color;
out vec4 FragColor;

uniform sampler2D inTexture;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    vec3 texRGB = texture(inTexture, textureUV).rgb;
    float sigDist = median(texRGB.r, texRGB.g, texRGB.b) - 0.5;
    float alpha = clamp(sigDist / fwidth(sigDist) + 0.5, 0.0, 1.0);

    FragColor = vec4(color, alpha);
}
