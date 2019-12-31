//
// Created by david on 26/12/2019.
//
#include "Shaders.h"
#include <ogl/UniformType.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <messages.h>
#include <ogl/ProgramShader.h>

using std::vector;
using glm::value_ptr;
using glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4;

/*
 * Strcuture:
 * ./UnifromTypes/* UniformTypes
 * ./* Endpoints with same name
 */
void Shaders::BindUniforms(Context *in_ctx, Context *out_ctx) {
    /*
     *             glUniform3f(glGetUniformLocation(markerShader, "color"), 0.3, 0.8, 1.0);
            glUniform2fv(glGetUniformLocation(markerShader, "border_size"), 1,
                         reinterpret_cast<const GLfloat *>(&p3_brd));

                         glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceTrMatrix"), 1, GL_FALSE,
			value_ptr(computeLightSpaceTrMatrix()));
     */
    auto type_ctx = in_ctx->context("shader");
    auto unif_ctx = type_ctx->context("uniforms");
    EGV(type_ctx, program, ProgramShader).use();
    for (const auto &endp : unif_ctx->endpoints()) {
        auto unif_type = endp.second->value<UniformType>();
        auto gltype = unif_type.type;

        if (!in_ctx->hasEndpoint(endp.first)) continue;

        if (gltype == GL_FLOAT && unif_type.count == 1) {
            glUniform1f(unif_type.location, in_ctx->endpoint(endp.first)->value<float>());

        } else if (gltype == GL_FLOAT && unif_type.count > 1) {
            auto buffy = in_ctx->endpoint(endp.first)->value<vector<float>>();
            glUniform1fv(unif_type.location, buffy.size(), buffy.data());

        } else if (gltype == GL_FLOAT_VEC2 && unif_type.count == 1) {
            glUniform2fv(unif_type.location, 1, value_ptr(in_ctx->endpoint(endp.first)->value<vec2>()));

        } else if (gltype == GL_FLOAT_VEC2 && unif_type.count > 1) {
            auto buffy = in_ctx->endpoint(endp.first)->value<vector<vec2>>();
            glUniform2fv(unif_type.location, buffy.size(), value_ptr(buffy[0]));

        } else if (gltype == GL_FLOAT_VEC3 && unif_type.count == 1) {
            glUniform3fv(unif_type.location, 1, value_ptr(in_ctx->endpoint(endp.first)->value<vec3>()));

        } else if (gltype == GL_FLOAT_VEC3 && unif_type.count > 1) {
            auto buffy = in_ctx->endpoint(endp.first)->value<vector<vec3>>();
            glUniform3fv(unif_type.location, buffy.size(), value_ptr(buffy[0]));

        } else if (gltype == GL_FLOAT_VEC4 && unif_type.count == 1) {
            glUniform4fv(unif_type.location, 1, value_ptr(in_ctx->endpoint(endp.first)->value<vec4>()));

        } else if (gltype == GL_FLOAT_VEC4 && unif_type.count > 1) {
            auto buffy = in_ctx->endpoint(endp.first)->value<vector<vec4>>();
            glUniform4fv(unif_type.location, buffy.size(), value_ptr(buffy[0]));

        } else if (gltype == GL_INT && unif_type.count == 1) {
            glUniform1i(unif_type.location, in_ctx->endpoint(endp.first)->value<int>());

        } else fatal_error(("Shaders::BindUniforms type not found for: " + endp.first).c_str());
    }

}
/*
 * #define GL_INT_VEC2 0x8B53
 * #define GL_INT_VEC3 0x8B54
 * #define GL_INT_VEC4 0x8B55
 * #define GL_FLOAT_MAT2 0x8B5A
 * #define GL_FLOAT_MAT3 0x8B5B
 * #define GL_FLOAT_MAT4 0x8B5C
 * #define GL_INT 0x1404
 * #define GL_UNSIGNED_BYTE 0x1401
 * #define GL_SHORT 0x1402
 * #define GL_UNSIGNED_SHORT 0x1403
 * #define GL_INT 0x1404
 * #define GL_UNSIGNED_INT 0x1405
 * #define GL_BOOL 0x8B56
 *
 * #define GL_BOOL_VEC2 0x8B57
 * #define GL_BOOL_VEC3 0x8B58
 * #define GL_BOOL_VEC4 0x8B59
 */
