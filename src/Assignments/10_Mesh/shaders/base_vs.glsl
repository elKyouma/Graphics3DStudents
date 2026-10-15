#version 450 core

layout(location=0) in  vec4 a_vertex_position;
layout(location=5) in  vec4 a_vertex_color;

layout(location=0) out vec4 v_Color;



layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
};

void main() {
    v_Color = a_vertex_color;
    gl_Position =  PVM * a_vertex_position;
}
