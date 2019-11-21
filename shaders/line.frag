#version 330
#define MAX_V 300
in vec3 v_color;
in vec2 uv;

uniform vec3 color_line;
uniform vec3 color_region;
uniform float width;
uniform float blur;
uniform float values[MAX_V];
uniform float lower[MAX_V];
uniform float upper[MAX_V];
uniform int vlength;

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

float sdfPrev(float t, float tick){
    return mod(t, tick)/tick;
}

float line(vec2 p, vec2 a, vec2 b){
    //float res;
    vec2 dortho;
    vec2 ap = p - a;
    float balen = 0.0000001+length(b-a);

    vec2 n = (b - a)/balen;

    float proj = dot(ap, n);
    if (proj < 0)
    dortho = ap;
    else if (proj > balen)
    dortho = p-b;
    else
    dortho = ap - n*dot(ap, n);

    //res = sqrt(length(ap)*length(ap) - proj * proj);

    return sign(dortho.y) * length(dortho);
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


//get line from uniforms
float uline(int index, int wline){
    if (index < 0)
    index = 0;
    if (index >= vlength)
    index = vlength - 1;

    if (wline == 0)
    return line(uv, vec2(1.0*index/vlength, values[index]), vec2(1.0*(index+1)/vlength, values[index+1]));
    if (wline == -1)
    return line(uv, vec2(1.0*index/vlength, lower[index]), vec2(1.0*(index+1)/vlength, lower[index+1]));
    if (wline == 1)
    return line(uv, vec2(1.0*index/vlength, upper[index]), vec2(1.0*(index+1)/vlength, upper[index+1]));

    return 0.0;
}

float lerpLines(int index, int line, int mode){
    float cl = uline(index, line);
    float nl = uline(index-1, line);
    float pl = uline(index+1, line);

    float finalLine;
    if (mode == 1){
        finalLine = max(0.0, cl);
        if (pl > 0)
        finalLine = min(finalLine, pl);
        if (nl > 0)
        finalLine = min(finalLine, nl);
    } else if (mode == -1){
        finalLine = min(0.0, cl);
        if (pl < 0)
        finalLine = max(finalLine, pl);
        if (nl < 0)
        finalLine = max(finalLine, nl);
    } else {
        finalLine = min(abs(cl), min(abs(nl), abs(pl)));
    }
    return finalLine;
}

void main() {

    vec4 cTransparent = vec4(0);
    vec4 cLine = vec4(color_line, 1.0);
    vec4 cRegBorder = vec4(color_region, 0.7);
    vec4 cRegFill = vec4(color_region, 0.4);


    int index = int(floor(uv.x * vlength));

    float line_lower = lerpLines(index, -1, 1);
    float line_upper = lerpLines(index, 1, -1);
    float line_current = lerpLines(index, 0, 0);

    float region = min(abs(line_lower), abs(line_upper));

    float cl_border = smoothstep(width/2-blur, width/2, line_current);
    float reg_border = smoothstep(0, blur, region) - smoothstep(width, width+blur, region);
    float reg_fill = smoothstep(width, width+blur, region);

    vec4 final_color = mix(cTransparent, cRegBorder, reg_border) +
    mix(cTransparent, cRegFill, reg_fill);
    final_color = mix(final_color, cLine, 1-cl_border);
    //float final_blend = max(lc_blend, reg_blend);

    //out_color = vec4(sdfPrev(outv, 0.1)*vec3(1.0, 1, 1), 1);
    //out_color += vec4(sdfPrev(med, 0.1)*vec3(0, 1.0, 0), 1);
    out_color = final_color;
}