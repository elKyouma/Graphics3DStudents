#version 450 core

void main() {
    // The vertices (-1,-1), (3,-1) and (-1,3) make a triangle covering the whole [-1,1]x[-1,1] square.
    vec2 position = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2) * 2.0 - 1.0;
    gl_Position = vec4(position, 0.0, 1.0);
}