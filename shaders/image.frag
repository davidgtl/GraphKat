#version 430 core

in vec3 v_color;
in vec2 uv;

out vec4 out_color;

layout(binding = 0) uniform sampler2D image;

void main() {
    out_color = texture(image, uv);
}