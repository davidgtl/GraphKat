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

    //glUniform1f(glGetUniformLocation(csprogShader, "roll"), interp*10);
    sdf_prog->setUniform("eye", vec3(0, 0, 2));
    sdf_prog->setUniform("ray00", vec3(0, 0, 0));
    sdf_prog->setUniform("ray01", vec3(0, 1, 0));
    sdf_prog->setUniform("ray11", vec3(1, 1, 0));
    sdf_prog->setUniform("ray10", vec3(1, 0, 0));

    glDispatchCompute(texture.width / 8, texture.height / 8, 1); //1024 512^2 threads in blocks of 16^2*/

    Image::draw();
}

SDF_Renderer::SDF_Renderer(vec2 origin, vec2 size, float z)
        : texture(1024, 1024, GL_RGBA32F), Image(&texture, origin, size, z),
          sdf_prog(&ShaderLoader::programMap["sdf"]) {
}
