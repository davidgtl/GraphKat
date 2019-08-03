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
    float res;
    vec2 ap = p - a;
    float balen = 0.0000001+length(b-a);

    vec2 n = (b - a)/balen;

    float proj = dot(ap, n);
    if (proj < 0)
    res = 0;//length(ap);
    else if (proj > balen)
    res = 0;//length(p-b);
    else
    res = sqrt(length(ap)*length(ap) - proj * proj);

    return res;
}

void main() {
    float values[8] = float[](0.1, 0.1, 0.9, 0.5, 0.7, 0.6, 0.6, 0.6);
    int len = 6;

    int index = int(floor(uv.x * len) + 1);

    float t = mod(uv.x*len, 1.0);

    float cv = mix(values[index], values[index+1], t);
    float dv = abs(values[index + 1] - values[index]);

    float blur = 1.5;
    float width = 100;

    float med = 0.0;
    vec2 lp, np;
    for (int i = 0; i <= 7; i++){
        lp = vec2(1.0*i/len, values[i]);
        np = vec2(1.0*(i+1)/len, values[i+1]);
        med += line(uv, lp, np);
    }

    float outv = pow(med, 1.0/len);

    //outv = smoothstep(width-blur, width, outv);
    //outv = smoothstep(width-blur, width, outv);
    out_color = vec4(sdfPrev(vec3(1.0), outv, 0.1), 1.0);
    //out_color = vec4(1.0)*outv + (1-outv)*vec4(0, 0, 0, 1.0);

    vec3 color = vec3(1.0, 0.5, 0.1);

    out_color = vec4(pow(out_color.r, 1/color.r),
    pow(out_color.g, 1/color.g),
    pow(out_color.b, 1/color.b),
    out_color.a);
    //out_color = vec4(0.3, 0.7, 0.9, 1.0)*t_col + (1-t_col)*vec4(1.0, 1.0, 1.0, 0.3);
}