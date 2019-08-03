#version 330

in vec3 v_color;
in vec2 uv;

out vec4 out_color;

uniform vec2 offset;

void main() {
    float values[7] = float[](0.1, 0.3, 0.8, 0.5, 0.7, 0.6, 0.6);
    float ranges[7] = float[](0.3, 0.3, 0.1, 0.2, 0.4, 0.5, 0.6);
    int len = 6;

    int index = int(uv.x * len);

    float t = mod(uv.x*len, 1.0);

    float cv = mix(values[index], values[index+1], t);

    float blur = 0.001;
    float t_col = 1 - smoothstep(cv - blur, cv + blur, uv.y);
    out_color = vec4(0.3, 0.7, 0.9, 1.0)*t_col + (1-t_col)*vec4(1.0, 1.0, 1.0, 0.3);
}