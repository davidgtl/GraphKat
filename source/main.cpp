#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE
//#define GLM_FORCE_AVX

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#include "io.h"
#include "ogl/PointModel.h"
#include "ogl/Shader.h"
#include "ogl/Plane.h"
#include "ogl/Texture.h"
#include "ogl/ProgramShader.h"
#include "messages.h"
#include "ogl/FontRenderer.h"
#include "ogl/ShaderLoader.h"
#include "wmgr/WindowLayout.h"
#include <nodeprims/Math.h>
#include <nodeprims/Shaders.h>
#include <nodeprims/Layouts.h>
#include <utils/BitSet.h>
#include <thread>
#include <chrono>
#include <map>
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <testbench.h>
#include <utils/BitMap2D.h>
#include <Foo.h>
#include <Bar.h>
#include <random>
#include "typing/TypeInfo.h"
#include "ui/Slider.h"
#include "ui/Text.h"
#include "ui/Button.h"
#include "ui/Marker.h"
#include "ui/IMouseEvents.h"
#include "ui/Image.h"
#include "ui/SDF_Renderer.h"
#include "utils/Timer.h"
#include "inputs/Keyboard.h"
#include <unordered_map>
#include "sdf_library/object_loader.h"

using namespace std;
using namespace glm;

int keyPressed[512];
bool captured = false;

bool invalidated = true;
bool resized = true;

BitMap2D<IMouseInteractable> *hitmap = nullptr;

vec2 absToRel(vec2 win, vec2 compPos, vec2 compSize) {
    return (win - compPos) / compSize;
}

vec2 winrel(vec2 pos) {
    return pos / win_layout.windowSize;
}

vec2 winrel(float x, float y) {
    return vec2(x, y) / win_layout.windowSize;
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

    invalidate();

}

//dt is in seconds
void keyHeldCallback(double dt) {
    float dtf = (float) dt;

}


double lxpos, lypos;
double a_xpos = 0.0f, a_ypos = 0.0f;
float mouseSensitivity = 0.001;
IMouseInteractable *last_elem = nullptr;

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {

    auto elem = last_elem;
    if (!captured && !(xpos < 0 || ypos < 0 || xpos >= win_layout.windowSize.x || ypos >= win_layout.windowSize.y))
        elem = hitmap->getFirst(xpos, win_layout.windowSize.y - ypos);

    if (elem != nullptr) {
        if (captured)
            elem->on_move_captured(vec2(xpos - lxpos, ypos - lypos));
        else
            elem->on_move(winrel(xpos, ypos));
    }

    if (last_elem != elem && last_elem != nullptr)
        last_elem->on_leave();

    last_elem = elem;

    lxpos = xpos;
    lypos = ypos;
    invalidate();
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        //if (button == GLFW_MOUSE_BUTTON_LEFT) {

        if (last_elem != nullptr)
            last_elem->on_button(button, action, mods);

        invalidate();
        //}
    }
}

float offX, offY;

void echo();

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    offX -= xoffset * 0.01;
    offY += yoffset * 0.01;
    auto elem = last_elem;

    if (last_elem != nullptr)
        last_elem->on_scroll(vec2(offX, offY));

    invalidate();
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    //glViewport(0, 0, width, height);
    win_layout.updateWindowSize(vec2(width, height));
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

    win_layout.updateScreenSize(vec2(mode->width, mode->height));
}

GLFWwindow *initialize(int width, int height) {
    int glfwInitRes = glfwInit();
    if (!glfwInitRes) {
        fprintf(stderr, "Unable to initialize GLFW\n");
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *window = glfwCreateWindow(width, height, "SDF demo", nullptr, nullptr);
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

    framebuffer_size_callback(window, win_layout.windowSize.x, win_layout.windowSize.y);

    return window;
}

float randf() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}


vector<IRenderable *> buildScene() {

    //auto *p1 = new Marker(vec3(0.3, 0.8, 1.0), 1, 0, vec2(0.01f, 0.01f),
    //        vec2(0.1, 0.1), vec2(0.2, 0.05), 0.1f);
    auto *p2 = new Slider(vec3(0.3, 0.8, 1.0), 0, vec2(0.8, 0.8), vec2(0.15, 0.05), 0.1f);

    Layouts::PopulateHitmap(*hitmap, {p2});

    //return {p2};
    return {};
}

void snap_to_zero(vec3 &p) {
    float zero = 0.000001;
    if (abs(p.x) < zero) p.x = 0;
    if (abs(p.y) < zero) p.y = 0;
    if (abs(p.z) < zero) p.z = 0;
}

void gen_sphere() {
    using namespace glm;

    cout << "Planified sphere: \n";
    vec3 origin(0, 0, 0);
    float radius = 1;
    float pi = glm::pi<float>();
    for (int p = 0; p < 4; p++)
        for (int y = 0; y < 8; y++) {
            float pitch = -pi / 2 + pi / 8 + pi * p / 4;
            float yaw = 0 + 2 * pi / 16 + 2 * pi * y / 8;
            vec3 normal = normalize(vec3(cos(yaw), sin(pitch), sin(yaw)));
            vec3 cp = radius * normal;

            snap_to_zero(normal);
            snap_to_zero(cp);

            cout << format("{",
                           "{", cp.x, ",", cp.y, ",", cp.z, "},",
                           "{", normal.x, ",", normal.y, ",", normal.z, "}",
                           "},\n");
        }

    cout << "\n\n";

}


int main(int argc, char *argv[]) {
    void *scene_index, *scene_data;
    int scene_index_size, scene_data_size;
    vec3 bound_min, bound_max;
    int levels;

    if (argc == 1) {
        load_obj("models/monkey_0.obj",
                 scene_index, scene_index_size, scene_data, scene_data_size,
                 bound_min, bound_max, levels);
    } else if (argc == 2) {
        load_bin(std::string(argv[1]),
                 scene_index, scene_index_size, scene_data, scene_data_size,
                 bound_min, bound_max, levels);
        printf("loaded index_size: %d, data_size: %d\n", scene_index_size, scene_data_size);

    } else if (argc == 3) {
        timer t;
        convert_obj(std::string(argv[1]), std::string(argv[2]));
        t.tick();
        printf("converted %s to %s in %f ms\n", argv[1], argv[2], t.ms());
        exit(0);
    }
    //gen_sphere(); exit(0);
    glfwSetErrorCallback(errorCallback);

    win_layout.updateWindowSize(vec2(1024, 1024));
    GLFWwindow *window = initialize(win_layout.windowSize.x, win_layout.windowSize.y);
    if (!window) {
        return 0;
    }
    updateScreenSize();
    framebuffer_size_callback(window, win_layout.windowSize.x, win_layout.windowSize.y);
    win_keybd = new Keyboard(window);
    glClearColor(0.094f, 0.086f, 0.109f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    ShaderLoader::LoadShaders("shaders/shaders.xml");

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);


    hitmap = new BitMap2D<IMouseInteractable>(10, win_layout.windowSize.x, win_layout.windowSize.y);
    auto renderables = buildScene();

    auto mouse_text = Text(textRenderer, "uninit",
                           win_layout.sisc(80, 80), win_layout.sisc(8), 0.1f, vec4(1, 0.8, 0.5, 1), -1);

    //renderables.push_back(&mouse_text);

    auto sdf_renderer = SDF_Renderer(vec2(0.05, 0.05), vec2(0.9, 0.9), 0, scene_index, scene_index_size, scene_data,
                                     scene_data_size, bound_min, bound_max, levels);
    Layouts::PopulateHitmap(*hitmap, &sdf_renderer);
    renderables.push_back(&sdf_renderer);

    resized = true;
    timer time;
    while (!glfwWindowShouldClose(window)) {
        if (resized) {
            printf("win: %f %f screen: %f %f\n", win_layout.windowSize.x, win_layout.windowSize.y,
                   win_layout.screenSize.x, win_layout.screenSize.y);
            resized = false;
        }
        if (invalidated) {
            sdf_renderer.update_z(lxpos / 1024.0);
            time.tick();
            sdf_renderer.on_tick(time.ms());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            mouse_text.update_text(format("x: ", lxpos, ", y: ", lypos));

            for (auto &r : renderables)
                r->draw();

            glfwSwapBuffers(window);
            invalidated = false;
        }
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}