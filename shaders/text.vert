#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 v_uvs;

out vec2 uvs;

void main(void) {
    gl_Position = vec4(position, 1);
    uvs = v_uvs;
}
