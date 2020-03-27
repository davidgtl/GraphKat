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
#include <Foo.h>
#include <Bar.h>
#include <random>

using namespace std;
using namespace glm;

int keyPressed[512];
bool captured = false;
LayoutManager lmgr;
vec2 screenSize;
vec2 windowSize;
vec2 windowNorm;

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

Context *last_ctx = nullptr;

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    if (xpos < 0 || ypos < 0 || xpos >= windowSize.x || ypos >= windowSize.y)
        return;
    auto ctx = EGV(Context::Root, MainScene/hitmap, BitMap2D<Context>*)->getFirst(lxpos, windowSize.y - lypos);

    if (!captured) {
        lxpos = xpos;
        lypos = ypos;

        if (last_ctx != nullptr)
            ESV(last_ctx, color, vec3(0.3, 0.3, 1.0));
        if (ctx != nullptr)
            ESV(ctx, color, vec3(1.0, 0.3, 0.3));
        last_ctx = ctx;

        invalidate();
        return;
    }

    mp = vec2(xpos, ypos)/windowSize;
    mp.y = 1 - mp.y;

    double dy = lypos - ypos;//inverted Y
    double dx = lxpos - xpos;//inverted X

    if (ctx != nullptr)
        ESV(ctx, value,
            (float) ((xpos/windowSize.x - EGV(ctx, primitive/origin, vec2).x)/EGV(ctx, primitive/size, vec2).x));

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


            Context *clicked_object = EGV(Context::Root, MainScene/hitmap, BitMap2D<Context>*)->getFirst(lxpos,
                                                                                                         windowSize.y -
                                                                                                         lypos);

            cout << "Clicked context: "
                 << clicked_object
                 << "\n";
            if (clicked_object != nullptr)
                ESV(clicked_object, color, vec3(1.0, 1.0, 0.3));
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
    windowNorm = windowSize/glm::min(windowSize.x, windowSize.y);
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


Context *createSlider(vec2 origin, vec2 size, float z) {

    auto context = Plane::CreatePlane(origin, size, z, CGV(Context::Root, shaders/slider));

    float minv = glm::min(size.x, size.y);
    ECV(context, uv_norm, size*windowNorm/minv);
    float g_bw = 1/glm::min((size*windowSize).x, (size*windowSize).y);
    ECV(context, g_bw, g_bw*3);
    ECV(context, g_tr, g_bw*2);

    return context;
}

Context *createButton(vec2 origin, vec2 size, float z) {
    auto context = Plane::CreatePlane(origin, size, z, CGV(Context::Root, shaders/button));

    float minv = glm::min(size.x, size.y);
    ECV(context, uv_norm, size*windowNorm/minv);
    float g_bw = 1/glm::min((size*windowSize).x, (size*windowSize).y);
    ECV(context, g_bw, g_bw*3);
    ECV(context, g_tr, g_bw*2);

    return context;
}

/*
 * Supports only monospaced fonts, size is side of square
 */
Context *createText(vec2 midstart, float size, float z) {

}

void renderScene(Context *in_ctx, Context *out_ctx) {
    for (auto obj : CGV(in_ctx, objects)->contexts())
        EE(obj.second, render);
}

Context *buildScene() {
    auto scene_ctx = CCV(Context::Root, MainScene);

    Context *p1 = createSlider(vec2(0.1, 0.1), vec2(0.2, 0.05), 0.1f);
    Context *p2 = createSlider(vec2(0, 0.2), vec2(1, 0.2), 0.1f);
    Context *p3 = createSlider(vec2(0, 0.4), vec2(1, 0.2), 0.1f);
    Context *p4 = createSlider(vec2(0, 0.6), vec2(1, 0.2), 0.1f);
    Context *p5 = createSlider(vec2(0, 0.8), vec2(1, 0.2), 0.1f);
    Context *p6 = createButton(vec2(0.3, 0.1), vec2(0.1, 0.05), 0.1f);

    ECV(p1, color, vec3(0.3, 0.8, 1.0));
    ECV(p2, color, vec3(0.3, 0.8, 1.0));
    ECV(p3, color, vec3(0.3, 0.8, 1.0));
    ECV(p4, color, vec3(0.3, 0.8, 1.0));
    ECV(p5, color, vec3(0.3, 0.8, 1.0));
    ECV(p6, color, vec3(0.3, 0.8, 1.0));

    ECV(p1, shape, 1);
    ECV(p1, filled, 0);
    ECV(p1, border_size, vec2(0.01f, 0.01f));
    ECV(p3, filled, 1);
    ECV(p4, filled, 0);
    ECV(p5, filled, 1);
    ECV(p6, filled, 1);

    ECV(p1, value, 0.0f);
    ECV(p2, value, 0.0f);
    ECV(p3, value, 0.0f);
    ECV(p4, value, 0.0f);
    ECV(p5, value, 0.0f);
    ECV(p5, value, 0.0f);

    auto Obj_ctx = CCV(scene_ctx, objects);
    Obj_ctx->adoptContext(p1);
    Obj_ctx->adoptContext(p2);
    Obj_ctx->adoptContext(p3);
    Obj_ctx->adoptContext(p4);
    Obj_ctx->adoptContext(p5);
    Obj_ctx->adoptContext(p6);

    ECV(scene_ctx, hitmap, new BitMap2D<Context>(10, windowSize.x, windowSize.y));//FIXME: @100 nothing works

    Context::Root->pretty_print();

    Layouts::PopulateHitmap(scene_ctx, nullptr);

    ECV(scene_ctx, render, new ComputeNode(scene_ctx, nullptr, renderScene));

    return scene_ctx;
}

void indirect_tb() {
    vector<int> original;
    vector<int> first;
    vector<int> second;
    vector<int> third;

    int N = 100000000;

    for (int i = 0; i < N; i++) {
        original.push_back(i);
        first.push_back(original[i]);
        second.push_back(first[i]);
        third.push_back(second[i]);
    }


    int total = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += original[first[second[third[i]]]];

    }

    auto t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    total = 0;
    t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += original[first[i]];
    }

    t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    total = 0;
    t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += original[first[second[i]]];
    }

    t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    total = 0;
    t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += original[i];
    }

    t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    fatal_error();
}

#define CSIZE(ctype) if(t == typeid(ctype)) return sizeof(ctype);

unsigned int csize(type_index t) {
    CSIZE(char);
    CSIZE(unsigned char);
    CSIZE(short);
    CSIZE(unsigned short);
    CSIZE(int);
    CSIZE(unsigned int);
    CSIZE(long);
    CSIZE(unsigned long);
    CSIZE(long long);
    CSIZE(unsigned long long);
    CSIZE(float);
    CSIZE(double);
}

#define CMAPSIZE(ctype) {std::type_index(typeid(ctype)), sizeof(ctype)}
std::unordered_map<type_index, unsigned int> types_map = {
        CMAPSIZE(char),
        CMAPSIZE(unsigned char),
        CMAPSIZE(short),
        CMAPSIZE(unsigned short),
        CMAPSIZE(int),
        CMAPSIZE(unsigned int),
        CMAPSIZE(long),
        CMAPSIZE(unsigned long),
        CMAPSIZE(long long),
        CMAPSIZE(unsigned long long),
        CMAPSIZE(float),
        CMAPSIZE(double)
};

void stdmap_vs_ifs_tb() {
    vector<int> original;
    type_index types[] = {typeid(char), typeid(unsigned char), typeid(short), typeid(unsigned short), typeid(int),
                          typeid(unsigned int),
                          typeid(long), typeid(unsigned long), typeid(long long), typeid(unsigned long long),
                          typeid(float), typeid(double)};

    int N = 1000000;


    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 11); // distribution in range [1, 6]


    for (int i = 0; i < N; i++) {
        original.push_back(dist(rng));
    }


    int total = 0;
    auto t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += types_map[types[original[i]]];

    }

    auto t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    total = 0;
    t0 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; i++) {
        total += csize(types[original[i]]);
    }

    t1 = std::chrono::high_resolution_clock::now();

    cout << "Avg time:"
         << static_cast<int>(std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count()) / 1000
         << ",    ans: " << total << "\n";

    fatal_error();
}


int main(int argc, char *argv[]) {
    TB::fancy_access();
    fatal_error();

    TB::deallocating();
    fatal_error();

    TB::run_anyvector();
    fatal_error();
    indirect_tb();
    GLuint i;
    cin >> i;

    Bar b;
    b.c = 100;
    Foo f = Foo(b);
    b.c = 200;
    cout << f.bar.c;
    fatal_error();

    std::string_view names[] = {"ana", "are", "mere", "djkfhksjfdfnrekfnkerjnferbhvbivnvnrjnjkdnkfjvndkjg"};

    cout << sizeof(names) << "/" << sizeof(std::string_view) << "\n";
    for (int i = 0; i < sizeof(names) / sizeof(std::string_view); i++)
        cout << names[i] << "\n";

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

    cout << "\n";

    /* boost::variant<int, std::string> u(7);
     std::cout << u; // output: hello world

     int result = boost::apply_visitor(my_visitor(), u);
     std::cout << result; // output: 11 (i.e., length of "hello world")

     exit(0);*/
    glfwSetErrorCallback(errorCallback);

    windowSize = vec2(1280, 720);
    windowNorm = windowSize/glm::min(windowSize.x, windowSize.y);
    GLFWwindow *window = initialize(windowSize.x, windowSize.y);
    if (!window) {
        return 0;
    }
    updateScreenSize();
    framebuffer_size_callback(window, windowSize.x, windowSize.y);

    glClearColor(0.094f, 0.086f, 0.109f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    ShaderLoader::LoadShaders("shaders/shaders.xml");

    FontRenderer textRenderer = FontRenderer("fonts/Source_Code_Pro/SourceCodePro-Regular.ttf", 512, 20, 200);

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
            textRenderer.drawText(str.str().c_str(), lmgr.sisc(80, 80), lmgr.sisc(8), vec4(1, 0.8, 0.5, 1), -1);

            //textRenderer.

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