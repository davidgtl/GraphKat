#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "io.h"
#include "PointModel.h"
#include "Shader.h"
#include "Plane.h"
#include "Texture.h"
#include "ProgramShader.h"
#include "messages.h"
#include "FontRenderer.h"
#include <thread>
#include <chrono>

using namespace std;

int keyPressed[512];
bool captured = false;
int winWidth = 1280;
int winHeight = 720;

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
    winWidth = width;
    winHeight = height;
    glViewport(0, 0, width, height);
    resized = true;
    invalidate();
}

void errorCallback(int error, const char *description) {
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}
//endregion


GLFWwindow *initialize() {
    int glfwInitRes = glfwInit();
    if (!glfwInitRes) {
        fprintf(stderr, "Unable to initialize GLFW\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(1280, 720, "GraphKat", nullptr, nullptr);
    if (!window) {
        fprintf(stderr, "Unable to create GLFW window\n");
        glfwTerminate();
        return nullptr;
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    int gladInitRes = gladLoadGL();
    if (!gladInitRes) {
        fprintf(stderr, "Unable to initialize glad\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    return window;
}

void initFonts(Texture texture, int width, int height, int pt, int dpi) {

    unsigned char *atlas = createFontAtlas(width, height, pt, dpi);
    texture.bindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED,
                    GL_UNSIGNED_BYTE, atlas);

}

void render_text(const char *text, float x, float y, float sx, float sy) {
    /*struct point {
        GLfloat x;
        GLfloat y;
        GLfloat s;
        GLfloat t;
    } coords[6 * strlen(text)];

    int n = 0;

    for (const char *p = text; *p; p++) {
        float x2 = x + c[*p].bl * sx;
        float y2 = -y - c[*p].bt * sy;
        float w = c[*p].bw * sx;
        float h = c[*p].bh * sy;

        x += c[*p].ax * sx;
        y += c[*p].ay * sy;

        if (!w || !h)
            continue;

        coords[n++] = (point) {x2, -y2, c[*p].tx, 0};
        coords[n++] = (point) {x2 + w, -y2, c[*p].tx + c[*p].bw / atlas_width, 0};
        coords[n++] = (point) {x2, -y2 - h, c[*p].tx, c[*p].bh / atlas_height};
        coords[n++] = (point) {x2 + w, -y2, c[*p].tx + c[*p].bw / atlas_width, 0};
        coords[n++] = (point) {x2, -y2 - h, c[*p].tx, c[*p].bh / atlas_height};
        coords[n++] = (point) {x2 + w, -y2 - h, c[*p].tx + c[*p].bw / atlas_width, c[*p].bh / atlas_height};
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, n);

    glCheckError();*/
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(errorCallback);

    GLFWwindow *window = initialize();
    if (!window) {
        return 0;
    }

    Texture fontAtlas(512, 512);
    initFonts(fontAtlas, 512, 512, 20, 100);

    PointModel screen(2 * winWidth, 2 * winHeight);
    Plane plane1 = Plane(vec2(0.1, 0.1), vec2(0.8, 0.8), 0.0);
    Plane plane2 = Plane(vec2(0.0, 0.0), vec2(512.0 / 1280.0, 512.0 / 720.0), 0.1, true);

    Shader vertShader("shaders/base.vert", Shader::VERTEX_SHADER);
    Shader fragShader("shaders/base.frag", Shader::FRAGMENT_SHADER);
    ProgramShader baseShader(vertShader, fragShader);
    glCheckError();

    Shader textVertShader("shaders/text.vert", Shader::VERTEX_SHADER);
    Shader textFragShader("shaders/text.frag", Shader::FRAGMENT_SHADER);
    ProgramShader textShader(textVertShader, textFragShader);
    glCheckError();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    while (!glfwWindowShouldClose(window)) {
        if (resized) {
            //screen.updateSize(2*winWidth, 2*winHeight);
            resized = false;
        }
        if (invalidated) {
            glClear(GL_COLOR_BUFFER_BIT);

            //screen.update(offX, offY);
            baseShader.use();
            glUniform2f(glGetUniformLocation(baseShader, "offset"), offX, offY);
            //screen.draw();
            plane1.draw();

            textShader.use();
            fontAtlas.bindTexture();
            glUniform4f(glGetUniformLocation(textShader, "color"), 0.0, 0.0, 0.0, 1.0);
            plane2.draw();

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
