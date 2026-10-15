#version 450 core

layout(location=0) in vec4 v_Color;
layout(location=0) out vec4 vFragColor;

void main() {
    vFragColor = v_Color;
}
