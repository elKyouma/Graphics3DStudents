#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D image;
layout(binding = 0, rgba16f) uniform writeonly image2D output_image;


vec3 linear_to_srgb(vec3 c) {
    return mix(12.92 * c, 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055, step(0.0031308, c));
}

vec3 srgb_to_linear(vec3 c) {
    return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    if (p.x >= size.x || p.y >= size.y) {
        return;
    }
    
    vec4 color = texelFetch(image, p, 0);
    imageStore(output_image, p, vec4(srgb_to_linear(1.0 - linear_to_srgb(color.rgb)), color.a));
}