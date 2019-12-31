#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE
//#define GLM_FORCE_AVX

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
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
#include "dataflow/ComputeNode.h"
#include "dataflow/Context.h"
#include "dataflow/Endpoint.h"
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

using namespace std;
using namespace glm;

int keyPressed[512];
bool captured = false;
LayoutManager lmgr;
vec2 screenSize;
vec2 windowSize;

bool invalidated = true;
bool resized = true;

vec2 absToRel(vec2 win, vec2 compPos, vec2 compSize) {
    return (win - compPos)/compSize;
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


void updateValues() {

    vec2 local = absToRel(mp, vec2(0.0, 0.0), vec2(1, 0.2));

    double indv = local.x;
    indv = indv < 0 ? 0 : indv;
    indv = indv > 1 ? 1 : indv;

    int index = (int) (indv*299);

}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {

    if (!captured) {
        lxpos = xpos;
        lypos = ypos;
        //invalidate();
        return;
    }

    mp = vec2(xpos, ypos)/windowSize;
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

            cout << "Clicked context: "
                 << EGV(Context::Root, MainScene/hitmap, BitMap2D<Context>*)->getFirst(lxpos, lypos) << "\n";
            EGV(Context::Root, MainScene/hitmap, BitMap2D<Context>*)->getFirst(lxpos, lypos)->endpoint("color")->update(
                    vec3(1.0, 0.3, 0.3));
            //EGV(Context::Root, MainScene/hitmap, BitMap2D<Context>*)->prettyPrint();

            //glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            invalidate();
        }
    }
}

float offX, offY;

void echo();

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    offX -= xoffset*0.01;
    offY += yoffset*0.01;
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
    return static_cast <float> (rand())/static_cast <float> (RAND_MAX);
}


class my_visitor : public boost::static_visitor<float> {
public:
    float operator()(int i) const {
        return i;
    }

    float operator()(const std::string &str) const {
        return str.length();
    }
};

void doNothing(Context *in_ctx, Context *out_ctx) {
    in_ctx->createEndpoint("hello");
};

void renderPlane(Context *in_ctx, Context *out_ctx) {
    Shaders::BindUniforms(in_ctx, nullptr);
    EGV(in_ctx, primitive/plane, Plane).draw();
}

Context *createPlane(vec2 origin, vec2 size, float z, Context *shader_ctx) {
    auto context = new Context();
    auto primitive = CCV(context, primitive);
    auto shader = CCV(context, shader);

    ECV(primitive, origin, origin);
    ECV(primitive, size, size);
    ECV(primitive, z, z);
    ECV(primitive, plane, Plane(primitive));

    shader->linkContext(shader_ctx);

    ECV(context, render, new ComputeNode(context, nullptr, renderPlane));

    return context;
}

void renderScene(Context *in_ctx, Context *out_ctx) {
    for (auto obj : CGV(in_ctx, objects)->contexts())
        EE(obj.second, render);

}

Context *buildScene() {
    auto shaders = CGV(Context::Root, shaders);
    auto scene_ctx = CCV(Context::Root, MainScene);

    Context *planeShader = CGV(shaders, slider);

    Context *p1 = createPlane(vec2(0.1, 0.1), vec2(0.2, 0.05), 0.1f, planeShader);
    Context *p2 = createPlane(vec2(0, 0.2), vec2(1, 0.2), 0.1f, planeShader);
    Context *p3 = createPlane(vec2(0, 0.4), vec2(1, 0.2), 0.1f, planeShader);
    Context *p4 = createPlane(vec2(0, 0.6), vec2(1, 0.2), 0.1f, planeShader);
    Context *p5 = createPlane(vec2(0, 0.8), vec2(1, 0.2), 0.1f, planeShader);

    ECV(p1, color, vec3(0.3, 0.8, 1.0));
    ECV(p2, color, vec3(0.3, 0.8, 1.0));
    ECV(p3, color, vec3(0.3, 0.8, 1.0));
    ECV(p4, color, vec3(0.3, 0.8, 1.0));
    ECV(p5, color, vec3(0.3, 0.8, 1.0));
    ECV(p1, shape, 1);
    ECV(p1, filled, 0);
    ECV(p1, border_size, vec2(0.01f, 0.01f));
    ECV(p3, filled, 1);
    ECV(p4, filled, 0);
    ECV(p5, filled, 1);

    auto Obj_ctx = CCV(scene_ctx, objects);
    Obj_ctx->adoptContext(p1);
    Obj_ctx->adoptContext(p2);
    Obj_ctx->adoptContext(p3);
    Obj_ctx->adoptContext(p4);
    Obj_ctx->adoptContext(p5);

    ECV(scene_ctx, hitmap, new BitMap2D<Context>(10, 1920, 1080));

    Layouts::PopulateHitmap(scene_ctx, nullptr);

    ECV(scene_ctx, render, new ComputeNode(scene_ctx, nullptr, renderScene));

    return scene_ctx;
}

int main(int argc, char *argv[]) {
    auto in_ctx = new Context();
    auto out_ctx = new Context();

    in_ctx->createEndpoint("fstart", 2.0f);
    in_ctx->createEndpoint("fend", 8.0f);
    in_ctx->createEndpoint("tstart", 0.0f);
    in_ctx->createEndpoint("tend", 100.0f);
    in_ctx->createEndpoint("x", 0.4f);
    out_ctx->createEndpoint("res");

    Math::LinMapUnit(in_ctx, out_ctx);

    cout << "result: " << out_ctx->endpoint("res")->value<float>();

    /* boost::variant<int, std::string> u(7);
     std::cout << u; // output: hello world

     int result = boost::apply_visitor(my_visitor(), u);
     std::cout << result; // output: 11 (i.e., length of "hello world")

     exit(0);*/
    glfwSetErrorCallback(errorCallback);

    windowSize = vec2(1280, 720);
    GLFWwindow *window = initialize(windowSize.x, windowSize.y);
    if (!window) {
        return 0;
    }
    updateScreenSize();
    framebuffer_size_callback(window, windowSize.x, windowSize.y);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);

    ShaderLoader::LoadShaders("shaders/shaders.xml");

    Context *mainScene = buildScene();

    Context::Root->pretty_print();

    resized = true;
    while (!glfwWindowShouldClose(window)) {
        if (resized) {
            printf("win: %f %f screen: %f %f\n", windowSize.x, windowSize.y, screenSize.x, screenSize.y);
            resized = false;
        }
        if (invalidated) {
            glClear(GL_COLOR_BUFFER_BIT);

            //screen.update(offX, offY);
            //baseShader.use();
            //glUniform2f(glGetUniformLocation(baseShader, "offset"), offX, offY);
            //screen.draw();
            //plane1.draw();


            /*markerShader.use();
            glUniform3f(glGetUniformLocation(markerShader, "color"), 0.3, 0.8, 1.0);
            glUniform2fv(glGetUniformLocation(markerShader, "border_size"), 1,
                         reinterpret_cast<const GLfloat *>(&p3_brd));
            glUniform1i(glGetUniformLocation(markerShader, "shape"), 1);
            glUniform1i(glGetUniformLocation(markerShader, "filled"), 1);*/
            //plane3.draw();


            EE(mainScene, render);

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