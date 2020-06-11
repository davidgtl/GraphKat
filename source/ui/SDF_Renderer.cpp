//
// Created by david on 10/06/2020.
//

#include "SDF_Renderer.h"
#include "inputs/Keyboard.h"

#define PI glm::pi<float>()

void SDF_Renderer::compute_axis(vec3 &right, vec3 &forward, vec3 &up) {
    right = vec3(cos(yaw - PI / 2), 0, sin(yaw - PI / 2));
    forward = normalize(vec3(cos(yaw), sin(pitch), sin(yaw)));
    up = cross(right, forward);
}

void SDF_Renderer::draw() {
    texture.bindImage(0);//FIXME: query uniforms for location
    sdf_prog->use();

    /*Calculate the forward rays for the corners of the camera projection screen*/
//            mat4 cam = getCamera();
//            mat4 invcam = inverse(cam);
//
//            vec4 calc = invcam * vec4(-1, -1, 0, 1); calc /= calc.w;
//            vec3 ray00 = vec3(calc);
//            calc = invcam * vec4(-1, 1, 0, 1); calc /= calc.w;
//            vec3 ray01 = vec3(calc);
//            calc = invcam * vec4(1, -1, 0, 1); calc /= calc.w;
//            vec3 ray10 = vec3(calc);
//            calc = invcam * vec4(1, 1, 0, 1); calc /= calc.w;
//            vec3 ray11 = vec3(calc);

    // screen radius x, y, depth of near plane
    vec3 right, forward, up;
    compute_axis(right, forward, up);

    float screen_dist = 0.001;
    // 90 deg fov
    float fovx = 80 / 180.0 * PI;
    float fovy = 80 / 180.0 * PI;

    right *= tan(fovx / 2) * screen_dist;
    forward = eye_pos + forward * screen_dist;
    up *= tan(fovy / 2) * screen_dist * size().y / size().x;

    sdf_prog->setUniform("eye", eye_pos);
    sdf_prog->setUniform("ray00", forward - right - up);
    sdf_prog->setUniform("ray01", forward - right + up);
    sdf_prog->setUniform("ray11", forward + right + up);
    sdf_prog->setUniform("ray10", forward + right - up);

    glDispatchCompute(texture.width / 8, texture.height / 8, 1); //1024 512^2 threads in blocks of 16^2*/

    Image::draw();
}

SDF_Renderer::SDF_Renderer(vec2 origin, vec2 size, float z)
        : texture(1024, 1024, GL_RGBA32F), Image(&texture, origin, size, z),
          sdf_prog(&ShaderLoader::programMap["sdf"]) {
    eye_pos = vec3(0, 5, -5);
    pitch = 0;
    yaw = 0;
}

void SDF_Renderer::on_move_captured(glm::vec2 dpos) {
    pitch -= dpos.y * 0.01;
    yaw -= dpos.x * 0.01;

    if (pitch >= PI / 2)
        pitch = PI / 2;

    if (pitch <= -PI / 2)
        pitch = -PI / 2;
}

void SDF_Renderer::on_tick(float dt_ms) {
    bool w = win_keybd->is_down(GLFW_KEY_W);
    bool a = win_keybd->is_down(GLFW_KEY_A);
    bool s = win_keybd->is_down(GLFW_KEY_S);
    bool d = win_keybd->is_down(GLFW_KEY_D);
    bool shift = win_keybd->is_down(GLFW_KEY_LEFT_SHIFT);
    bool ctrl = win_keybd->is_down(GLFW_KEY_LEFT_CONTROL);

    vec3 speed(3.0 / 1000, 3.0 / 1000, 3.0 / 1000);

    if (w || a || s || d || shift || ctrl) {
        vec3 right, forward, up;
        compute_axis(right, forward, up);

        if (w) eye_pos += forward * speed.z * dt_ms;
        if (s) eye_pos -= forward * speed.z * dt_ms;
        if (a) eye_pos -= right * speed.x * dt_ms;
        if (d) eye_pos += right * speed.x * dt_ms;
        if (shift) eye_pos -= up * speed.y * dt_ms;
        if (ctrl) eye_pos += up * speed.y * dt_ms;
    }

}
