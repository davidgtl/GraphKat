#version 330

in vec3 v_color;
in vec2 uv;

uniform vec3 color;
uniform vec2 border_size;

out vec4 out_color;

float df_square(vec2 uv){
    vec2 duv = abs(uv - vec2(0.5, 0.5));
    return max(duv.x, duv.y)/0.5;
}


float df_circle(vec2 uv){
    vec2 duv = abs(uv - vec2(0.5, 0.5));
    duv = duv * duv;
    return sqrt(duv.x + duv.y)/0.5;
}

void main() {
    float t = df_circle((uv - vec2(0.5, 0.5)) * (1 + border_size) + vec2(0.5, 0.5));
    if (t < 1)
    out_color = vec4(1.0, 1.0, 1.0, 0.5);
    else
    out_color = vec4(color, 0.9);
}