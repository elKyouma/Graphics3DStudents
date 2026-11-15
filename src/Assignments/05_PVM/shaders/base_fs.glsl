#version 450 core

layout(location=0) in vec4 v_Color;
layout(location=0) out vec4 vFragColor;

layout(std140, binding = 0) uniform Mixer {
    float strength;
    vec3  mix_color;
};

void main() {
    vFragColor = v_Color;
    vFragColor.rgb = mix(v_Color.rgb, mix_color, clamp(strength, 0.0, 1.0));
}
