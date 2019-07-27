#version 330

in vec3 v_color;
in vec2 uv;

out vec4 out_color;


void main() {
    vec3 final_color = vec3(uv * v_color.xy, v_color.z);
    out_color = vec4(final_color, 1.0);
}