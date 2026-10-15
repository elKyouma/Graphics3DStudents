#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler2D image;
layout(binding = 0, rgba16f) uniform writeonly image2D output_image;

// The l function from sobel_fs.glsl.

// The luminance of the pixel with the window coordinates `pixel`.


void main() {
    ivec2 p = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(output_image);
    if (p.x >= size.x || p.y >= size.y) {
        return;
    }
    
    vec2 uv = vec2(p) / vec2(size);
    float effect = 1.0 - smoothstep(0.0, 1.0, length(uv - 0.5));
    
    vec3 color = texelFetch(image, clamp(p, ivec2(0), size - 1), 0).rgb;

    imageStore(output_image, p, vec4(color * effect, 1.0));
}