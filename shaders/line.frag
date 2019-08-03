#version 330

in vec3 v_color;
in vec2 uv;

uniform vec3 color;
uniform float width;
uniform float blur;

out vec4 out_color;

uniform vec2 offset;

#define PI 3.14159265358979

// exponential smooth min (k = 32);
float smine(float a, float b, float k)
{
    float res = exp2(-k*a) + exp2(-k*b);
    return -log2(res)/k;
}

// polynomial smooth min (k = 0.1);
float sminl(float a, float b, float k)
{
    float h = clamp(0.5+0.5*(b-a)/k, 0.0, 1.0);
    return mix(b, a, h) - k*h*(1.0-h);
}

// power smooth min (k = 8);
float sminp(float a, float b, float k)
{
    a = pow(a, k); b = pow(b, k);
    return pow((a*b)/(a+b), 1.0/k);
}

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
    res = length(ap);
    else if (proj > balen)
    res = length(p-b);
    else
    res = sqrt(length(ap)*length(ap) - proj * proj);

    return res;
}

vec3 colorpow(vec3 color, float lum){
    return vec3(pow(lum, 1/color.r),
    pow(lum, 1/color.g),
    pow(lum, 1/color.b));
}

vec3 colorscheme(vec3 color, vec3 shadows, float lum){
    vec3 tint = mix(shadows, color, lum);
    return colorpow(tint, pow(lum, 2));
}

void main() {
    float values[8] = float[](0.1, 0.1, 0.9, 0.5, 0.7, 0.6, 0.6, 0.6);
    int len = 6;

    int index = int(floor(uv.x * len) + 1);

    float t = mod(uv.x*len, 1.0);

    float cv = mix(values[index], values[index+1], t);
    float dv = abs(values[index + 1] - values[index]);

    float med = 1.0;
    vec2 lp, np;
    for (int i = 0; i <= 6; i++){
        lp = vec2(1.0*i/len, values[i]);
        np = vec2(1.0*(i+1)/len, values[i+1]);
        med = min(line(uv, lp, np), med);
    }

    float outv = med;

    outv = smoothstep(width-blur, width, outv);
    out_color = mix(vec4(color, 1.0), vec4(0), outv);

    vec2 p = uv;// - vec2(0.5, 0.5);
    float plen = mod(length(p), 0.1)/0.1;
    vec3 tint1 = vec3(1.0, 0.5, 0.1);
    vec3 tint2 = vec3(0.1, 0.1, 1.0)*1.5;
    out_color = vec4(colorscheme(tint1, tint2, plen), 1);
}