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

    vec4 color = texelFetch(image, p, 0);

    // Right column minus left column.
    float gx = (l(p + ivec2(1, -1)) + 2.0 * l(p + ivec2(1, 0)) + l(p + ivec2(1, 1)))
             - (l(p + ivec2(-1, -1)) + 2.0 * l(p + ivec2(-1, 0)) + l(p + ivec2(-1, 1)));
    // Upper row minus lower row.
    float gy = (l(p + ivec2(-1, 1)) + 2.0 * l(p + ivec2(0, 1)) + l(p + ivec2(1, 1)))
             - (l(p + ivec2(-1, -1)) + 2.0 * l(p + ivec2(0, -1)) + l(p + ivec2(1, -1)));

    float edge = clamp(length(vec2(gx, gy)), 0.0, 1.0);

    imageStore(output_image, p, vec4((1.0 - edge) * color.rgb, color.a));
}