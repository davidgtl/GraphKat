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
#include <thread>
#include <chrono>
#include <ft2build.h>
#include FT_FREETYPE_H

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

struct character_info {
    float ax; // advance.x
    float ay; // advance.y

    float bw; // bitmap.width;
    float bh; // bitmap.rows;

    float bl; // bitmap_left;
    float bt; // bitmap_top;

    float tx; // x offset of glyph in texture coordinates
} c[128];
/* you might as well save this value as it is needed later on */
int atlas_width, atlas_height;

#define WIDTH   480
#define HEIGHT  240
/* origin is the upper left corner */
unsigned char image[HEIGHT][WIDTH];


/* Replace this function with something useful. */

void
draw_bitmap(FT_Bitmap *bitmap,
            FT_Int x,
            FT_Int y) {
    FT_Int i, j, p, q;
    FT_Int x_max = x + bitmap->width;
    FT_Int y_max = y + bitmap->rows;


    /* for simplicity, we assume that `bitmap->pixel_mode' */
    /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */

    for (i = x, p = 0; i < x_max; i++, p++) {
        for (j = y, q = 0; j < y_max; j++, q++) {
            if (i < 0 || j < 0 ||
                i >= WIDTH || j >= HEIGHT)
                continue;

            image[j][i] |= bitmap->buffer[q * bitmap->width + p];
        }
    }
}


void
show_image(void) {
    int i, j;


    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++)
            putchar(image[i][j] == 0 ? ' '
                                     : image[i][j] < 128 ? '|'
                                                         : '#');
        putchar('\n');
    }
}

void doTheFont() {
    FT_Library library;
    FT_Face face;

    FT_GlyphSlot slot;
    FT_Matrix matrix;                 /* transformation matrix */
    FT_Vector pen;                    /* untransformed origin  */
    FT_Error error;

    char *filename;
    char *text;

    double angle;
    int target_height;
    int n, num_chars;


    filename = "fonts/Roboto_Mono/RobotoMono-Regular.ttf";                           /* first argument     */
    text = "Freetype";                           /* second argument    */
    num_chars = strlen(text);
    angle = 25 / 180.0 * 3.1425;      /* use 25 degrees     */
    target_height = HEIGHT;

    error = FT_Init_FreeType(&library);              /* initialize library */
    /* error handling omitted */

    error = FT_New_Face(library, filename, 0, &face);/* create face object */
    /* error handling omitted */

    /* use 50pt at 100dpi */
    error = FT_Set_Char_Size(face, 40 * 64, 0,
                             100, 0);                /* set character size */
    /* error handling omitted */

    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    /* set up matrix */
    matrix.xx = (FT_Fixed) (cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed) (-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed) (sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed) (cos(angle) * 0x10000L);

    /* the pen position in 26.6 cartesian space coordinates; */
    /* start at (300,200) relative to the upper left corner  */
    pen.x = 700;
    pen.y = 300;

    for (n = 0; n < num_chars; n++) {
        /* set transformation */
        FT_Set_Transform(face, &matrix, &pen);

        /* load glyph image into the slot (erase previous one) */
        error = FT_Load_Char(face, text[n], FT_LOAD_RENDER);
        if (error)
            continue;                 /* ignore errors */

        /* now, draw to our target surface (convert position) */
        draw_bitmap(&slot->bitmap,
                    slot->bitmap_left,
                    target_height - slot->bitmap_top);

        /* increment pen position */
        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    //show_image();

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void initFonts(Texture texture) {
    /*FT_Library ft;

    if (FT_Init_FreeType(&ft))
        fatal_error("Could not init freetype library");

    FT_Face face;

    if (FT_New_Face(ft, "fonts/Cousine/Cousine-Regular.ttf", 0, &face))
        fatal_error("Could not open font");

    FT_Set_Pixel_Sizes(face, 0, 48);

    FT_GlyphSlot g = face->glyph;
    int w = 0;
    int h = 0;

    for (int i = 32; i < 128; i++) {
        if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            fatal_error("Loading character failed");

        w = g->bitmap.width;
        h = std::max(h, (int) g->bitmap.rows);
    }

    atlas_height = h;
    atlas_width = w;

    texture.bindTexture();
    int x = 0;
    glCheckError();
    //for (int i = 32; i < 128 && x < 500; i++) {
    //start = 62-64
    for (int c = 77; x < 500; c++) {
        int index = FT_Get_Char_Index(face, c);
        if (FT_Load_Char(face, index, FT_LOAD_RENDER))
            fatal_error("Failed to load characters");

        glCheckError();
        glTexSubImage2D(GL_TEXTURE_2D, 0, x + g->bitmap_left, g->bitmap_top, 20, g->bitmap.rows, GL_RED,
                        GL_UNSIGNED_BYTE,
                        g->bitmap.buffer);
        glCheckError();

        cout << c <<  " " << g->bitmap.width << endl;
        for (int y = 0; y < g->bitmap.rows; y++) {
            for (int x = 0; x < g->bitmap.width; x++) {
                cout << (g->bitmap.buffer[y * g->bitmap.width + x] > 127 ? "_" : "#");
            }
            cout << endl;
        }


        x += g->bitmap.width;
    }*/

    texture.bindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, WIDTH, HEIGHT, GL_RED,
                    GL_UNSIGNED_BYTE,
                    image);

}

void render_text(const char *text, float x, float y, float sx, float sy) {
    struct point {
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

        /* Advance the cursor to the start of the next character */
        x += c[*p].ax * sx;
        y += c[*p].ay * sy;

        /* Skip glyphs that have no pixels */
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

    glCheckError();
}

int main(int argc, char *argv[]) {
    glfwSetErrorCallback(errorCallback);

    GLFWwindow *window = initialize();
    if (!window) {
        return 0;
    }

    Texture fontAtlas(512, 512);
    doTheFont();
    initFonts(fontAtlas);

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
