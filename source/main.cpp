#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <stdio.h>
#include "io.h"
#include "ogl/PointModel.h"
#include "ogl/Shader.h"
#include "ogl/Plane.h"
#include "ogl/Texture.h"
#include "ogl/ProgramShader.h"
#include "messages.h"
#include "ogl/FontRenderer.h"
#include "wmgr/LayoutManager.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace glm;

int keyPressed[512];
bool captured = false;
vec2 screenSize;
vec2 windowSize;

bool invalidated = true;
bool resized = true;

void invalidate() {
    invalidated = true;
}

//region Callbacks
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    float amount = 0.01;
    if (action == GLFW_PRESS) {
        keyPressed[key] = 1;

        if (key == GLFW_KEY_C) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            captured = true;
        }

        if (key == GLFW_KEY_ESCAPE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            captured = false;
        }
    }

    if (action == GLFW_RELEASE)
        keyPressed[key] = 0;

}

//dt is in seconds
void keyHeldCallback(double dt) {
    float dtf = (float) dt;

}

double lxpos, lypos;
double a_xpos = 0.0f, a_ypos = 0.0f;
float mouseSensitivity = 0.001;

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {

    if (!captured) {
        lxpos = xpos;
        lypos = ypos;
        return;
    }


    double dy = lypos - ypos;//inverted Y
    double dx = lxpos - xpos;//inverted X


    lxpos = xpos;
    lypos = ypos;
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            captured = !captured;
            glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
}

float offX, offY;

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    offX -= xoffset * 0.01;
    offY += yoffset * 0.01;
    invalidate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    //glViewport(0, 0, width, height);
    windowSize = vec2(width, height);
    resized = true;
    invalidate();
}

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}
//endregion

void updateScreenSize() {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    screenSize = vec2(mode->width, mode->height);
}

vec2 sis(vec2 size) {
    return size * (screenSize.x / 1000.0f / windowSize);
}

vec2 sis(float size) {
    return vec2(size, size) * (screenSize.x / 1000.0f / windowSize);
}

vec2 sis(float sx, float sy) {
    return sis(vec2(sx, sy));
}

vec2 lsis(vec2 size, vec2 region) {
    return size * (screenSize.x / 1000.0f / windowSize / region);
}

vec2 lsis(float sx, float sy, vec2 region) {
    return lsis(vec2(sx, sy), region);
}

GLFWwindow *initialize(int width, int height) {
    int glfwInitRes = glfwInit();
    if (!glfwInitRes) {
        fprintf(stderr, "Unable to initialize GLFW\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "GraphKat", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Unable to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    int gladInitRes = gladLoadGL();
    if (!gladInitRes) {
        fprintf(stderr, "Unable to initialize glad\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    framebuffer_size_callback(window, windowSize.x, windowSize.y);

    return window;
}

float randf() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(errorCallback);

    windowSize = vec2(1280, 720);
    GLFWwindow *window = initialize(windowSize.x, windowSize.y);
    if (!window) {
        return 0;
    }
    updateScreenSize();


    Plane plane1 = Plane(vec2(0.1, 0.1), vec2(0.8, 0.8), 0.0);
    Plane plane2 = Plane(vec2(0.0, 0.0), sis(100, 100), 0.1, true);
    Plane plane4 = Plane(vec2(0.2, 0.2), sis(500, 300), 0.2, false);
    Plane plane3 = Plane(0.3, false);

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);


    Shader vertShader("shaders/base.vert", Shader::VERTEX_SHADER);
    Shader fragShader("shaders/base.frag", Shader::FRAGMENT_SHADER);
    ProgramShader baseShader(vertShader, fragShader);
    glCheckError();


    Shader markerFragShader("shaders/marker.frag", Shader::FRAGMENT_SHADER);
    ProgramShader markerShader(vertShader, markerFragShader);
    glCheckError();

    Shader lineFragShader("shaders/line.frag", Shader::FRAGMENT_SHADER);
    ProgramShader lineShader(vertShader, lineFragShader);
    glCheckError();

    Shader verticalRangeFragShader("shaders/verticalRange.frag", Shader::FRAGMENT_SHADER);
    ProgramShader verticalRangeShader(vertShader, verticalRangeFragShader);
    glCheckError();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int vlen = 300;
    auto *values = new GLfloat[vlen];
    auto *lower = new GLfloat[vlen];
    auto *upper = new GLfloat[vlen];

    srand(static_cast <unsigned> (time(0)));

    values[0] = randf();
    lower[0] = randf() * values[0];
    upper[0] = randf() * (1 - values[0]) + values[0];

    for (int i = 1; i < vlen; i++) {
        values[i] = 0.2 * (randf() - 0.5) * values[i - 1] + values[i - 1];
        lower[i] = values[i] - 0.2 * (randf()) * lower[i - 1];
        upper[i] = values[i] + 0.2 * (randf()) * upper[i - 1];

        upper[i] += upper[i - 1];
        upper[i] /= 2;

        lower[i] += lower[i - 1];
        lower[i] /= 2;
    }

    vec2 p3_brd;
    resized = true;
    while (!glfwWindowShouldClose(window)) {
        if (resized) {
            printf("win: %f %f screen: %f %f\n", windowSize.x, windowSize.y, screenSize.x, screenSize.y);
            plane2.updateVertices(vec2(0.0, 0.0), sis(200, 200));
            plane4.updateVertices(vec2(0.0, 0.0), sis(600, 500));
            plane3.updateVertices(vec2(0.4, 0.2), sis(10, 10));

            p3_brd = lsis(2, 2, plane3.size);

            resized = false;
        }
        if (invalidated) {
            glClear(GL_COLOR_BUFFER_BIT);

            //screen.update(offX, offY);
            baseShader.use();
            glUniform2f(glGetUniformLocation(baseShader, "offset"), offX, offY);
            //screen.draw();
            plane1.draw();


            markerShader.use();
            glUniform3f(glGetUniformLocation(markerShader, "color"), 0.3, 0.8, 1.0);
            glUniform2fv(glGetUniformLocation(markerShader, "border_size"), 1,
                         reinterpret_cast<const GLfloat *>(&p3_brd));
            glUniform1i(glGetUniformLocation(markerShader, "shape"), 1);
            glUniform1i(glGetUniformLocation(markerShader, "filled"), 1);
            plane3.draw();

            lineShader.use();
            glUniform3f(glGetUniformLocation(lineShader, "color_line"), 0.3, 0.8, 1.0);
            glUniform3f(glGetUniformLocation(lineShader, "color_region"), 1.0, 0.8, 0.3);
            glUniform1f(glGetUniformLocation(lineShader, "width"), sis(2, 2).x);
            glUniform1f(glGetUniformLocation(lineShader, "blur"), sis(0.5, 0.5).x);

            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, values);
            glUniform1i(glGetUniformLocation(lineShader, "vlength"), vlen - 1);
            glUniform1fv(glGetUniformLocation(lineShader, "lower"), vlen, lower);
            glUniform1fv(glGetUniformLocation(lineShader, "upper"), vlen, upper);
            plane4.draw();


            textRenderer.drawText("Ana are $50", sis(100, 100), sis(8), vec4(1, 0, 0.5, 1), -1);

            glfwSwapBuffers(window);
            invalidated = false;
        } else {
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
