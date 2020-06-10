//
// Created by david on 10/06/2020.
//

#include "SDF_Renderer.h"

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
    vec3 fov = vec3(2, 1, 1);
    vec3 right = vec3(cos(yaw), 0, sin(yaw));
    vec3 forward = normalize(vec3(cos(yaw), sin(pitch), sin(yaw)));
    vec3 up = cross(right, forward);
    forward += eye_pos;
    right *= fov.x;
    up *= fov.y;
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
    eye_pos = vec3(0, 0, 0);
    pitch = 0;
    yaw = 0;
}

void SDF_Renderer::on_move_captured(glm::vec2 dpos) {
    pitch += dpos.x * 0.1;
    yaw += dpos.y * 0.1;
}
