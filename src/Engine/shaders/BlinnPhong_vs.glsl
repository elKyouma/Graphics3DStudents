#version 450 core

layout(location=0) in  vec4 a_vertex_position;
layout(location=1) in  vec4 a_vertex_normal;
layout(location=2) in vec4 a_vertex_tangent;
layout(location=3) in  vec4 a_vertex_texcoord;
layout(location=5) in  vec4 a_vertex_color;

layout(location=0) out vec4 v_Color;
layout(location=1) out vec2 v_TexCoord;
layout(location=2) out vec3 v_Normal;
layout(location=3) out vec3 v_Position;
layout(location=4) out vec4 vertex_tangent_vs;

layout(std140, binding=1) uniform Transformations {
    mat4 PVM;
    mat4 VM;
    mat3 VM_normal;
};

void main() {
    v_Color = a_vertex_color;
    v_TexCoord = a_vertex_texcoord.xy;

    v_Normal = normalize(VM_normal * a_vertex_normal.xyz);

    vec3 tangent_vs = mat3(VM) * a_vertex_tangent.xyz;
    if (dot(tangent_vs, tangent_vs) > 0.0) {
        tangent_vs = normalize(tangent_vs);
    }
    vertex_tangent_vs = vec4(tangent_vs, a_vertex_tangent.w);

    vec4 position_vs = VM * a_vertex_position;
    v_Position = position_vs.xyz / position_vs.w;
    gl_Position =  PVM * a_vertex_position;
}
