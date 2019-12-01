#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <stdio.h>
#include <sstream>
#include "io.h"
#include "ogl/PointModel.h"
#include "ogl/Shader.h"
#include "ogl/Plane.h"
#include "ogl/Texture.h"
#include "ogl/ProgramShader.h"
#include "messages.h"
#include "ogl/FontRenderer.h"
#include "ogl/ShaderLoader.h"
#include "wmgr/LayoutManager.h"
#include <thread>
#include <chrono>
#include <map>
#include <boost/any.hpp>


using namespace std;
using namespace glm;

int keyPressed[512];
bool captured = false;
LayoutManager lmgr;
vec2 screenSize;
vec2 windowSize;

bool invalidated = true;
bool resized = true;

GLfloat *values, *der1, *der2, *der3, *der4;

vec2 absToRel(vec2 win, vec2 compPos, vec2 compSize) {
    return (win - compPos) / compSize;
}

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
vec2 mp;
double a_xpos = 0.0f, a_ypos = 0.0f;
float mouseSensitivity = 0.001;

void calcDerivatives() {
    for (int i = 1; i < 299; i++)
        der1[i] = 0.5 + values[i + 1] - values[i - 1];
    der1[299] = 0.5 + 0;
    der1[0] = 0.5 + 0;

    for (int i = 1; i < 299; i++)
        der2[i] = 0.5 + der1[i + 1] - der1[i - 1];
    der2[299] = 0.5 + 0;
    der2[0] = 0.5 + 0;

    for (int i = 1; i < 299; i++)
        der3[i] = 0.5 + der2[i + 1] - der2[i - 1];
    der3[299] = 0.5 + 0;
    der3[0] = 0.5 + 0;

    for (int i = 1; i < 299; i++)
        der4[i] = 0.5 + der3[i + 1] - der3[i - 1];
    der4[299] = 0.5 + 0;
    der4[0] = 0.5 + 0;
}

void updateValues() {

    vec2 local = absToRel(mp, vec2(0.0, 0.0), vec2(1, 0.2));

    double indv = local.x;
    indv = indv < 0 ? 0 : indv;
    indv = indv > 1 ? 1 : indv;

    int index = (int) (indv * 299);

    values[index] = local.y;

    calcDerivatives();
}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {

    if (!captured) {
        lxpos = xpos;
        lypos = ypos;
        //invalidate();
        return;
    }

    mp = vec2(xpos, ypos) / windowSize;
    mp.y = 1 - mp.y;

    double dy = lypos - ypos;//inverted Y
    double dx = lxpos - xpos;//inverted X


    updateValues();
    lxpos = xpos;
    lypos = ypos;
    invalidate();
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            cout << "mevent " << captured << endl;
            captured = !captured;
            //glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
        }
    }
}

float offX, offY;

void echo();

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    offX -= xoffset * 0.01;
    offY += yoffset * 0.01;
    invalidate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    //glViewport(0, 0, width, height);
    windowSize = vec2(width, height);
    lmgr.updateWindowSize(windowSize);
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
    lmgr.updateScreenSize(screenSize);
}

GLFWwindow *initialize(int width, int height) {
    int glfwInitRes = glfwInit();
    if (!glfwInitRes) {
        fprintf(stderr, "Unable to initialize GLFW\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
    cout << int();
    exit(0);

    glfwSetErrorCallback(errorCallback);

    windowSize = vec2(1280, 720);
    GLFWwindow *window = initialize(windowSize.x, windowSize.y);
    if (!window) {
        return 0;
    }
    updateScreenSize();
    framebuffer_size_callback(window, windowSize.x, windowSize.y);

    //Plane plane1 = Plane(0.0);
    //Plane plane2 = Plane(0.1, true);
    Plane plane4 = Plane(0.2, false);
    Plane plane41 = Plane(0.2, false);
    Plane plane42 = Plane(0.2, false);
    Plane plane43 = Plane(0.2, false);
    Plane plane44 = Plane(0.2, false);
    Plane plane3 = Plane(0.3, false);

    lmgr.addPlane(plane4, {0, 0}, {0.0, 0.8});
    lmgr.addPlane(plane41, {0, 0}, {0.2, 0.6});
    lmgr.addPlane(plane42, {0, 0}, {0.4, 0.4});
    lmgr.addPlane(plane43, {0, 0}, {0.6, 0.2});
    lmgr.addPlane(plane44, {0, 0}, {0.8, 0.0});
    lmgr.addPlane(plane3, LayoutConstraint::sl(0.4, wsize(10, wsize::sis)),
                  LayoutConstraint::sl(0.2, wsize(10, wsize::sis)));

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);

    auto shaders = ShaderLoader::LoadShaders("shaders/shaders.xml");

    ProgramShader baseShader = shaders["base"];
    ProgramShader markerShader = shaders["marker"];
    ProgramShader lineShader = shaders["line"];

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int vlen = 300;
    values = new GLfloat[vlen];
    der1 = new GLfloat[vlen];
    der2 = new GLfloat[vlen];
    der3 = new GLfloat[vlen];
    der4 = new GLfloat[vlen];
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

        lower[i] = 0.49;
        upper[i] = 0.51;
    }

    vec2 p3_brd;
    resized = true;
    while (!glfwWindowShouldClose(window)) {
        if (resized) {
            printf("win: %f %f screen: %f %f\n", windowSize.x, windowSize.y, screenSize.x, screenSize.y);

            lmgr.calculateMetrics(plane4);
            lmgr.calculateMetrics(plane41);
            lmgr.calculateMetrics(plane42);
            lmgr.calculateMetrics(plane43);
            lmgr.calculateMetrics(plane44);
            lmgr.calculateMetrics(plane3);
            resized = false;
        }
        if (invalidated) {
            glClear(GL_COLOR_BUFFER_BIT);

            //screen.update(offX, offY);
            baseShader.use();
            glUniform2f(glGetUniformLocation(baseShader, "offset"), offX, offY);
            //screen.draw();
            //plane1.draw();


            markerShader.use();
            glUniform3f(glGetUniformLocation(markerShader, "color"), 0.3, 0.8, 1.0);
            glUniform2fv(glGetUniformLocation(markerShader, "border_size"), 1,
                         reinterpret_cast<const GLfloat *>(&p3_brd));
            glUniform1i(glGetUniformLocation(markerShader, "shape"), 1);
            glUniform1i(glGetUniformLocation(markerShader, "filled"), 1);
            //plane3.draw();

            lineShader.use();
            glUniform3f(glGetUniformLocation(lineShader, "color_line"), 0.3, 0.8, 1.0);
            glUniform3f(glGetUniformLocation(lineShader, "color_region"), 1.0, 0.8, 0.3);
            glUniform1f(glGetUniformLocation(lineShader, "width"), lmgr.sisc(2, 2).x);
            glUniform1f(glGetUniformLocation(lineShader, "blur"), lmgr.sisc(0.5, 0.5).x);

            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, values);
            glUniform1i(glGetUniformLocation(lineShader, "vlength"), vlen - 1);
            glUniform1fv(glGetUniformLocation(lineShader, "lower"), vlen, lower);
            glUniform1fv(glGetUniformLocation(lineShader, "upper"), vlen, upper);
            plane4.draw();

            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, der1);
            plane41.draw();
            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, der2);
            plane42.draw();
            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, der3);
            plane43.draw();
            glUniform1fv(glGetUniformLocation(lineShader, "values"), vlen, der4);
            plane44.draw();


            stringstream str;

            str << "x: " << lxpos << ", y: " << lypos;
            //textRenderer.drawText(str.str().c_str(), lmgr.sisc(100, 100), lmgr.sisc(8), vec4(1, 0, 0.5, 1), -1);

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