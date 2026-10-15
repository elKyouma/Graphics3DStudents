#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D input_texture;
layout(binding = 0, rgba16f) uniform writeonly image2D output_image;

#define NEGATIVE

vec3 linear_to_srgb(vec3 c) {
    return mix(12.92 * c, 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055, step(0.0031308, c));
}

vec3 srgb_to_linear(vec3 c) {
    return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    if (pixel.x >= size.x || pixel.y >= size.y) {
        return;
    }
    
    vec4 color = texelFetch(input_texture, pixel, 0);
#ifdef NEGATIVE
    imageStore(output_image, pixel, vec4(srgb_to_linear(1.0 - linear_to_srgb(color.rgb)), color.a));
#else
    float luminance = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    imageStore(output_image, pixel, vec4(luminance, luminance, luminance, color.a));
#endif

}