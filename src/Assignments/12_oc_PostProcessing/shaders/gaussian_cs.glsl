#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D image;
layout(binding = 0, rgba16f) uniform writeonly image2D output_image;

// The l function from sobel_fs.glsl.

// The luminance of the pixel with the window coordinates `pixel`.
vec3 l(ivec2 pixel) {
    ivec2 size = textureSize(image, 0);
    vec3 color = texelFetch(image, clamp(pixel, ivec2(0), size - 1), 0).rgb;
    return color;
}

void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    
    const int gaussianSize = 20;
    if (p.x >= size.x - gaussianSize || p.y >= size.y - gaussianSize || p.x < gaussianSize || p.y < gaussianSize) {
        return;
    }

    vec4 color = texelFetch(image, p, 0);

    // Right column minus left column.
    vec3 value = vec3(0.0);

    for(int y = -gaussianSize; y <= gaussianSize; y++) {
        for(int x = -gaussianSize; x <= gaussianSize; x++) {
            value += l(p + ivec2(x, y));
        }
    }
    value /= (2*gaussianSize + 1) * (2*gaussianSize + 1);


    imageStore(output_image, p, vec4(value, color.a));
}