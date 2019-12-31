#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_SWIZZLE
#define GLM_FORCE_AVX

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
#include "dataflow/ComputeNode.h"
#include "dataflow/Context.h"
#include "dataflow/Endpoint.h"
#include <nodeprims/Math.h>
#include <nodeprims/Shaders.h>
#include <thread>
#include <chrono>
#include <map>
#include <boost/any.hpp>
#include <boost/variant.hpp>
#include <testbench.h>

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
            //glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
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

Context *createPlane(vec2 origin, vec2 size, float z, Context *shader_ctx) {
    auto context = new Context();
    auto primitive = context->createSubContext("primitive");
    auto shader = context->createSubContext("shader");

    primitive->createEndpoint("origin", origin);
    primitive->createEndpoint("size", size);
    primitive->createEndpoint("z", z);
    primitive->createEndpoint("plane", Plane(primitive));

    shader->linkContext(shader_ctx);


    return context;
}

void buildUI() {
    // ComputeNode root;
    /*root.func = [](Context* in_ctx, Context* out_ctx){
        in_ctx->createEndpoint("hello");
    };
    root.func = doNothing;*/


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

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);

    auto shaders = ShaderLoader::LoadShaders("shaders/shaders.xml");

    /*ProgramShader baseShader = shaders["base"];
    ProgramShader markerShader = shaders["marker"];
    ProgramShader lineShader = shaders["line"];*/

    Context *lineShader = shaders->context("line");

    //Plane plane1 = Plane(0.0);
    //Plane plane2 = Plane(0.1, true);
    Context *p1 = createPlane(vec2(0, 0), vec2(1, 0.2), 0.1f, lineShader);
    Context *p2 = createPlane(vec2(0, 0.2), vec2(1, 0.2), 0.1f, lineShader);
    Context *p3 = createPlane(vec2(0, 0.4), vec2(1, 0.2), 0.1f, lineShader);
    Context *p4 = createPlane(vec2(0, 0.6), vec2(1, 0.2), 0.1f, lineShader);
    Context *p5 = createPlane(vec2(0, 0.8), vec2(1, 0.2), 0.1f, lineShader);

    auto Obj_ctx = Context::Root->createSubContext("objects");
    Obj_ctx->adoptContext(p1);
    Obj_ctx->adoptContext(p2);
    Obj_ctx->adoptContext(p3);
    Obj_ctx->adoptContext(p4);
    Obj_ctx->adoptContext(p5);


    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    int vlen = 300;
    vector<float> lower(vlen);
    vector<float> values(vlen);
    vector<float> upper(vlen);

    srand(static_cast <unsigned> (time(0)));

    values[0] = randf();
    lower[0] = randf()*values[0];
    upper[0] = randf()*(1 - values[0]) + values[0];

    for (int i = 1; i < vlen; i++) {
        values[i] = 0.2*(randf() - 0.5)*values[i - 1] + values[i - 1];
        lower[i] = values[i] - 0.2*(randf())*lower[i - 1];
        upper[i] = values[i] + 0.2*(randf())*upper[i - 1];

        upper[i] += upper[i - 1];
        upper[i] /= 2;

        lower[i] += lower[i - 1];
        lower[i] /= 2;

        lower[i] = 0.49;
        upper[i] = 0.51;
    }

    ECV(p1, color_line, vec3(0.3, 0.8, 1.0));
    ECV(p1, color_bg, vec4(0.8, 0.8, 0.0, 1.0));
    ECV(p1, color_region, vec3(1.0, 0.8, 0.3));
    ECV(p1, width, 0.01f);
    ECV(p1, blur, 0.005f);
    ECV(p1, values, values);
    ECV(p1, lower, lower);
    ECV(p1, vlength, vlen - 1);

    Context::Root->pretty_print();

    vec2 p3_brd;
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

            Shaders::BindUniforms(p1, nullptr);
            /*ProgramShader ls = EGV(p1, shader/_program, ProgramShader);
            glUniform3f(glGetUniformLocation(ls, "color_line"), 0.3, 0.8, 1.0);
            glUniform3f(glGetUniformLocation(ls, "color_region"), 1.0, 0.8, 0.3);
            glUniform1f(glGetUniformLocation(ls, "width"), lmgr.sisc(2, 2).x);
            glUniform1f(glGetUniformLocation(ls, "blur"), lmgr.sisc(0.5, 0.5).x);

            glUniform1fv(glGetUniformLocation(ls, "values"), vlen, values.data());
            glUniform1i(glGetUniformLocation(ls, "vlength"), vlen - 1);
            glUniform1fv(glGetUniformLocation(ls, "lower"), vlen, lower.data());*/
            EGV(p1, primitive/plane, Plane).draw();

            Shaders::BindUniforms(p2, nullptr);
            EGV(p2, primitive/plane, Plane).draw();

            Shaders::BindUniforms(p3, nullptr);
            EGV(p3, primitive/plane, Plane).draw();

            Shaders::BindUniforms(p4, nullptr);
            EGV(p4, primitive/plane, Plane).draw();

            Shaders::BindUniforms(p5, nullptr);
            EGV(p5, primitive/plane, Plane).draw();


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