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

// 16 + 2*2 = 20 pixels in each dimension
const int gaussianSize = 20;
shared vec3 tile[57][57];

void main() {
    ivec2 localID = ivec2(gl_LocalInvocationID.xy);
    ivec2 groupID = ivec2(gl_WorkGroupID.xy);
    ivec2 size = imageSize(output_image);

   // Global position of this invocation's output pixel
    ivec2 pixel = groupID * 16 + localID;

    // Cooperative loading: 256 threads load 400 tile pixels.
    for (int y = localID.y - gaussianSize; y <= gaussianSize + 16; y += 16) {
        for (int x = localID.x - gaussianSize; x <= gaussianSize + 16; x += 16) {
            ivec2 p = groupID * 16 + ivec2(x, y);
            tile[y + gaussianSize][x + gaussianSize] = l(p);
        }
    }

    barrier();


    // Right column minus left column.
    vec3 value = vec3(0.0);

    for(int y = localID.y - gaussianSize; y <= localID.y +gaussianSize; y++) {
        for(int x = localID.x - gaussianSize; x <= localID.x + gaussianSize; x++) {
            value += tile[y + gaussianSize][x + gaussianSize];
        }
    }
    value /= (2*gaussianSize + 1) * (2*gaussianSize + 1);


    imageStore(output_image, pixel, vec4(value, 1.0));
}