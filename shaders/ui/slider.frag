#version 330

in vec3 v_color;
in vec2 uv;

uniform vec3 color;
uniform int shape;
uniform int filled;
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

float border(float blur, float lower, float upper){
    float delta = upper - lower;
    float result = -smoothstep(1.0, 1.0 - blur, lower) + smoothstep(1.0, 1.0 - blur, upper);
    return result;
}


vec2 cscale(vec2 x, vec2 center, vec2 scale){
    return (x - center) * scale + center;
}

vec3 sdfPrev(vec3 col, float t, float tick){
    return col * (mod(t, tick)/tick);
}

void main() {
    vec3 final_color = vec3(0.0);
    vec2 center = vec2(0.5, 0.5);

    float blur = 0.07;
    float t, tu;

    if (shape == 0){
        t = df_square(cscale(uv, center, vec2(1.0, 1.0)));
        tu = df_square(cscale(uv, center, 1 + border_size));
    } else {
        t = df_circle(cscale(uv, center, vec2(1.0, 1.0)));
        tu = df_circle(cscale(uv, center, 1 + border_size));
    }

    float ti = smoothstep(1 - blur, 1, tu);
    float to = smoothstep(1 - blur, 1, t);
    float opb = 0.9;
    float opi = 0.3;
    if (filled == 1){
        opb = 0.9;
        opi = 0.7;
    }

    out_color = vec4(color, opb) * (ti - to) + vec4(color, opi) * (1-ti);

}