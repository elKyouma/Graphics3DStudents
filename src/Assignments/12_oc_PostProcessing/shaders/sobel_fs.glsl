#version 450 core

layout(location = 0) out vec4 vFragColor;

layout(binding = 0) uniform sampler2D image;

vec3 srgb_gamma_correction(vec3 color) {
    color = clamp(color, 0.0, 1.0);
    color = mix(color * 12.92, (1.055 * pow(color, vec3(1.0 / 2.4))) - 0.055, step(0.0031308, color));
    return color;
}

// The luminance of the pixel with the window coordinates `pixel`.
float l(ivec2 pixel) {
    ivec2 size = textureSize(image, 0);
    vec3 color = texelFetch(image, clamp(pixel, ivec2(0), size - 1), 0).rgb;
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    ivec2 p = ivec2(gl_FragCoord.xy);
    // Right column minus left column.
    float gx = (l(p + ivec2(1, -1)) + 2.0 * l(p + ivec2(1, 0)) + l(p + ivec2(1, 1)))
             - (l(p + ivec2(-1, -1)) + 2.0 * l(p + ivec2(-1, 0)) + l(p + ivec2(-1, 1)));
    // Upper row minus lower row.
    float gy = (l(p + ivec2(-1, 1)) + 2.0 * l(p + ivec2(0, 1)) + l(p + ivec2(1, 1)))
             - (l(p + ivec2(-1, -1)) + 2.0 * l(p + ivec2(0, -1)) + l(p + ivec2(1, -1)));

    float edge = clamp(length(vec2(gx, gy)), 0.0, 1.0);

    vec4 color = texelFetch(image, p, 0);
    vFragColor = vec4(srgb_gamma_correction((1.0 - edge) * color.rgb), color.a);
}