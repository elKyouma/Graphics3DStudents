#version 450 core

layout(location=0) in  vec4 a_vertex_position;
layout(location=3) in  vec4 a_vertex_texcoord;
layout(location=5) in  vec4 a_vertex_color;

layout(location=0) out vec4 v_Color;
layout(location=1) out vec2 v_TexCoord;



layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
};

void main() {
    v_Color = a_vertex_color;
    v_TexCoord = a_vertex_texcoord.xy;
    gl_Position =  PVM * a_vertex_position;
}
