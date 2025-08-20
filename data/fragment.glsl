#version 300 es
precision mediump float;

in vec2 textureUV;
out vec4 FragColor;

uniform sampler2D inTexture;

uniform float u_pxrange;
uniform float u_weight;
uniform vec3 u_color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
    vec2 unitRange = vec2(u_pxrange) / vec2(textureSize(inTexture, 0));
    vec2 screenTexSize = vec2(1.0) / fwidth(textureUV);
    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
}

void main() {
    vec3 msd = texture(inTexture, textureUV).rgb;
    float sd = median(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
    vec3 color = u_color * opacity;
    FragColor = vec4(color, opacity);
}
