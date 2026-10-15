#version 450 core

layout(location = 0) out vec4 vFragColor;

layout(binding = 0) uniform sampler2D image;

vec3 srgb_gamma_correction(vec3 color) {
    color = clamp(color, 0.0, 1.0);
    color = mix(color * 12.92, (1.055 * pow(color, vec3(1.0 / 2.4))) - 0.055, step(0.0031308, color));
    return color;
}

void main() {
    vec4 color = texelFetch(image, ivec2(gl_FragCoord.xy), 0);
    vFragColor = vec4(srgb_gamma_correction(color.rgb), color.a);
}