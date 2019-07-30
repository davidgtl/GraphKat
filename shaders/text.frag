#version 330

uniform sampler2D tex;
uniform vec4 color;

in vec2 uvs;
out vec4 out_color;

void main(void) {
    out_color = vec4(1, 1, 1, texture(tex, uvs).r) * color;
}