#version 330

in vec3 v_color;
in vec2 uv;

uniform vec3 color;
uniform float value;

uniform vec2 g_uv_norm;
uniform float g_bw;
uniform float g_tr;

out vec4 out_color;

float outside(float k, float bw, float blur, float x){
    return smoothstep(k+bw*0.5, k+bw*0.5+blur, x);
}

float inside(float k, float bw, float blur, float x){
    return 1.0 - smoothstep(k-bw*0.5-blur, k-bw*0.5, x);
}

float bump(float k, float bw, float blur, float x){
    return smoothstep(k-bw*0.5-blur, k-bw*0.5, x) - smoothstep(k+bw*0.5, k+bw*0.5+blur, x);
}

float df_square(vec2 uv, vec2 size){
    vec2 duv = abs(uv) - size;
    if (duv.y > 0.0 && duv.x > 0.0)
    return length(duv);
    return max(duv.x, duv.y);
}

float df_circle(vec2 uv, float r){
    return length(uv) - r;
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

vec4 fblend(vec4 acc, vec3 col, float alpha){
    return vec4(acc.rgb * (1.0-alpha) + col * alpha, min(alpha + (1.0-alpha)*acc.a, 1.0));
}

void main() {
    vec2 nuv = uv*g_uv_norm;
    vec4 final_color = vec4(0.0);
    vec2 center = vec2(0.5, 0.5);

    float blur = 0.07;
    if (value > 0.5){
        final_color += 0.0001;
    }
    float t, tu;

    /*if (shape == 0){
        t = df_square(cscale(uv, vec2(1.0, 1.0)));
        tu = df_square(cscale(uv, 1 + border_size));
    } else {
        t = df_circle(cscale(uv, vec2(1.0, 1.0)));
        tu = df_circle(cscale(uv, 1 + border_size));
    }*/

    vec2 sq_size = vec2(0.4, 0.4) * g_uv_norm;
    float sq_dist = df_square(nuv-g_uv_norm*0.5, sq_size);
    float circ_dist = df_circle(nuv-0.5 - vec2(value * sq_size.x*2.0 - 0.2, 0.0), 0.0);


    final_color = fblend(final_color, color, inside(0.2, g_bw, g_tr, sq_dist));

    out_color = final_color;

}