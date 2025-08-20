#version 300 es
precision mediump float;

in vec2 textureUV;
out vec4 FragColor;

uniform sampler2D inTexture;

uniform float u_pxrange;
uniform float u_fontSize;
uniform float u_weight;
uniform vec3 u_color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main()
{
    //vec4 color = texture(inTexture, textureUV);
    //FragColor = color;

    float smoothing = clamp(2.0 * u_pxrange / u_fontSize, 0.0, 0.5);

    vec2 textureCoord = textureUV * 2.;
    vec3 texSample = texture(inTexture, textureUV).rgb;
    float dist = median(texSample.r, texSample.g, texSample.b);

    float alpha;
    vec3 color;

    alpha = smoothstep(u_weight - smoothing, u_weight + smoothing, dist);
    color = u_color * alpha;

    vec4 text = vec4(color, alpha);
    FragColor = text;
}
