//
// Created by david on 10/06/2020.
//

#include "SDF_Renderer.h"
#include "inputs/Keyboard.h"
#include "messages.h"

#define PI glm::pi<float>()

void SDF_Renderer::compute_axis(vec3 &right, vec3 &forward, vec3 &up) {
    right = vec3(cos(yaw - PI / 2), 0, sin(yaw - PI / 2));
    forward = normalize(vec3(cos(yaw), sin(pitch), sin(yaw)));
    up = cross(right, forward);
}

void SDF_Renderer::draw() {
    texture.bindImage(0);//FIXME: query uniforms for location
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, obj_index);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, obj_data);
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

    //FIXME: uncomment me after the uniforms are used
    /*sdf_prog->setUniform("eye", eye_pos);
    sdf_prog->setUniform("ray00", forward - right - up);
    sdf_prog->setUniform("ray01", forward - right + up);
    sdf_prog->setUniform("ray11", forward + right + up);
    sdf_prog->setUniform("ray10", forward + right - up);*/

    //if(recompute)
        glDispatchCompute(texture.width / 8, texture.height / 8, 1); //1024 512^2 threads in blocks of 16^2*/

    Image::draw();
}

SDF_Renderer::SDF_Renderer(vec2 origin, vec2 size, float z, void *index, int index_size, void *data, int data_size,
                           glm::vec3 bound_min, glm::vec3  bound_max, int levels)
        : texture(1024, 1024, GL_RGBA32F), Image(&texture, origin, size, z),
          sdf_prog(&ShaderLoader::programMap["sdf"]) {
    sdf_prog->use();
    eye_pos = vec3(0, 5, -5);
    pitch = -0.56;
    yaw = 1.68;

    glGenBuffers(1, &obj_data);
    glGenBuffers(1, &obj_index);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, obj_index);
    glBufferData(GL_SHADER_STORAGE_BUFFER, index_size, index, GL_DYNAMIC_COPY);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, obj_data);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data_size, data, GL_DYNAMIC_COPY);
    //FIXME: uncomment me after the uniforms are used
    //sdf_prog->setUniform("data_size", data_size);
    //sdf_prog->setUniform("index_size", index_size);
    sdf_prog->setUniform("bound_min", bound_min);
    sdf_prog->setUniform("bound_max", bound_max);
    sdf_prog->setUniform("levels", levels);
}

void SDF_Renderer::on_move_captured(glm::vec2 dpos) {
    pitch -= dpos.y * 0.01;
    yaw -= dpos.x * 0.01;

    if (pitch >= PI / 2)
        pitch = PI / 2;

    if (pitch <= -PI / 2)
        pitch = -PI / 2;

    //std::cout << format("pitch ", pitch, ", yaw ", yaw, "\n");
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

void SDF_Renderer::update_z(float z) {
    sdf_prog->use();
    sdf_prog->setUniform("cut_z", z);
}
