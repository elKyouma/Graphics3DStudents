#version 450 core

layout(location=0) in vec4 v_Color;
layout(location=1) in vec2 v_TexCoord;
layout(location=0) out vec4 vFragColor;

uniform sampler2D map_Kd;

layout(std140, binding=0) uniform KdMaterial {
    vec4 Kd;
    bool use_vertex_colors;
    bool use_map_Kd;
};

vec3 srgb_gamma_correction(vec3 color) {
   color = clamp(color, 0.0, 1.0);
   color = mix(color * 12.92, (1.055 * pow(color, vec3(1.0 / 2.4))) - 0.055, step(0.0031308, color));
   return color;
}

void main() {
    vec4 texture_color = texture(map_Kd, v_TexCoord);

    vec4 color = use_vertex_colors ? v_Color * Kd : Kd;

    color = use_map_Kd ? texture_color * color : color;

    vFragColor.a = color.a;
    vFragColor.rgb = srgb_gamma_correction(color.rgb);
}
