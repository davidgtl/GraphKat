#version 330

in vec3 v_color;
in vec2 uv;

out vec4 out_color;

uniform vec2 offset;

#define PI 3.14159265358979

vec3 sdfPrev(vec3 col, float t, float tick){
    return col * (mod(t, tick)/tick);
}

void main() {
    float values[7] = float[](0.1, 0.1, 0.9, 0.5, 0.7, 0.6, 0.6);
    int len = 6;

    int index = int(floor(uv.x * len));

    float t = mod(uv.x*len, 1.0);

    float cv = mix(values[index], values[index+1], t);
    float dv = abs(values[index + 1] - values[index]);

    float blur = 0.0001;
    float width = 0.006;

    vec2 lp = vec2(1.0*index/len, values[index]);
    vec2 np = vec2(1.0*(index+1)/len, values[index+1]);

    vec2 p = (uv - vec2(0.5, 0.5));

    float alpha = 1.0 * 0 / 180 * PI;

    vec2 n = vec2(cos(alpha), sin(alpha));

    float outv = length(p/length(p));// - n*dot(uv, n));

    //vec2 n = (np-lp)/length(np-lp);

    //float t_col = length((uv - lp) - n * dot(uv - lp, n));
    //float t_col = dot(uv - lp, np - lp)/length(np-lp);

    //t_col = smoothstep(width-blur, width, t_col);
    out_color = vec4(sdfPrev(vec3(1.0), outv, 0.1), 1.0);
    //ut_color = vec4(1.0)*outv + (1-outv)*vec4(0, 0, 0, 1.0);
    //out_color = vec4(0.3, 0.7, 0.9, 1.0)*t_col + (1-t_col)*vec4(1.0, 1.0, 1.0, 0.3);
}