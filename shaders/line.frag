#version 330

in vec3 v_color;
in vec2 uv;

out vec4 out_color;

uniform vec2 offset;

#define PI 3.14159265358979

vec3 sdfPrev(vec3 col, float t, float tick){
    return col * (mod(t, tick)/tick);
}

float line(vec2 p, vec2 a, vec2 b){
    p = p - a;
    vec2 n = (b - a)/length(b-a);
    return sqrt(length(p)*length(p) - dot(p, n)*dot(p, n));
}

void main() {
    float values[9] = float[](0.1, 0.1, 0.1, 0.9, 0.5, 0.7, 0.6, 0.6, 0.6);
    int len = 6;

    int index = int(floor(uv.x * len) + 1);

    float t = mod(uv.x*len, 1.0);

    float cv = mix(values[index], values[index+1], t);
    float dv = abs(values[index + 1] - values[index]);

    float blur = 0.0001;
    float width = 0.006;

    vec2 llp = vec2(1.0*(index-1)/len, values[(index-1)]);
    vec2 lp = vec2(1.0*index/len, values[index]);
    vec2 np = vec2(1.0*(index+1)/len, values[index+1]);
    vec2 nnp = vec2(1.0*(index+2)/len, values[index+2]);

    float l0 = line(uv, llp, lp);
    float l1 = line(uv, lp, np);
    float l2 = line(uv, np, nnp);

    float outv = 1 - pow(l0 * l1 * l2, 1.0/3);

    //outv = smoothstep(width - blur, width, outv);
    //out_color = vec4(sdfPrev(vec3(1.0), outv, 0.05), 1.0);
    out_color = clamp(vec4(1.0)*outv + (1-outv)*vec4(0, 0, 0, 1.0), 0, 1);

    vec3 color = vec3(0.1, 0.01, 1);

    out_color = vec4(pow(out_color.r, 1/color.r),
    pow(out_color.g, 1/color.g),
    pow(out_color.b, 1/color.b),
    out_color.a);
    //out_color = vec4(0.3, 0.7, 0.9, 1.0)*t_col + (1-t_col)*vec4(1.0, 1.0, 1.0, 0.3);
}