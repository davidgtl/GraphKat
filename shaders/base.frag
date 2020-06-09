#version 330

in vec3 v_color;
in vec2 uv;

out vec4 out_color;

uniform vec2 offset;

void main() {
    vec2 dv = abs(uv - vec2(0.5, 0.5) + offset);

    vec3 final_color = vec3(dv, 0.0);
    // out_color = vec4(final_color, 1.0);
    out_color = vec4(1.0, 0.2, 0.2 + 0.001 * offset.x, 1.0);
}