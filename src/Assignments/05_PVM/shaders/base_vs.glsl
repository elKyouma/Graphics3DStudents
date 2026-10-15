#version 450 core

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec4 a_vertex_color;

layout(location=0) out vec4 v_Color;

layout(std140, binding = 0) uniform Mixer {
    float strength;
    vec3  mix_color;
};

layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
};

void main() {
    float clampedStrength = clamp(strength, 0.0f, 1.0f);
    v_Color = vec4(mix(a_vertex_color.rgb, mix_color, clampedStrength), 1.0f);
    gl_Position =  PVM * a_vertex_position;
}
