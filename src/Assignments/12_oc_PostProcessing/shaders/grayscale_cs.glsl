#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D image;
layout(binding = 0, rgba16f) uniform writeonly image2D output_image;

// The l function from sobel_fs.glsl.

// The luminance of the pixel with the window coordinates `pixel`.
float l(ivec2 pixel) {
    ivec2 size = textureSize(image, 0);
    vec3 color = texelFetch(image, clamp(pixel, ivec2(0), size - 1), 0).rgb;
    return dot(color, vec3(0.2126, 0.7152, 0.0722));
}

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    if (p.x >= size.x || p.y >= size.y) {
        return;
    }
    
    float luminance = l(p);

    imageStore(output_image, p, vec4(luminance, luminance, luminance, 1.0));
}