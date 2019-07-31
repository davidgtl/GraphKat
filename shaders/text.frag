#version 330

uniform sampler2D tex;
uniform vec4 color;
uniform vec4 charBounds;

in vec2 uvs;
out vec4 out_color;

void main(void) {
    out_color = vec4(1, 1, 1, 0.1 + texture(tex, uvs * charBounds.zw + charBounds.xy).r) * color;
}