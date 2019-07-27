#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec2 uv;
out vec3 v_color;

void main() {
    gl_Position = vec4(position, 1.0);
    v_color = color;
    uv = vec2(0.5, 0.5);
}