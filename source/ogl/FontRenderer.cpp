//
// Created by David Ghitulescu on 2019-07-30.
//
#include "FontRenderer.h"
#include "messages.h"

#define GLM_FORCE_SWIZZLE

#include <glm/glm.hpp>
#include <ft2build.h>
#include <math.h>
#include <stdlib.h>

#include FT_FREETYPE_H
#include "ogl/Plane.h"
#include "ogl/Shader.h"
#include "ogl/Texture.h"
#include "ogl/ProgramShader.h"

using namespace std;
using namespace glm;


FontRenderer::FontRenderer(const char *fontPath, const int atlas_size, int pt, int dpi) {
    this->atlas_size = atlas_size;

    fontAtlas = new Texture(atlas_size, atlas_size);
    textPlane = new Plane(1, true);

    Shader textVertShader("shaders/text.vert", Shader::VERTEX_SHADER);
    Shader textFragShader("shaders/text.frag", Shader::FRAGMENT_SHADER);
    textShader = ProgramShader(textVertShader, textFragShader);
    glCheckError();

    initFonts(fontPath, *fontAtlas, atlas_size, atlas_size, &atlas_charHeight, &atlas_charWidth, pt, dpi);
}


FontRenderer::~FontRenderer() {
    delete textPlane;
    delete fontAtlas;
}


void FontRenderer::createFontAtlas(const char *fontPath, int width, int height, int pt, int dpi, unsigned char **image,
                                   vec4 **charBounds,
                                   int *charHeight, int *charWidth) {

    *image = new unsigned char[width * height];
    *charBounds = new vec4[256];
    memset(*image, 0, width * height);

    FT_Library library;
    FT_Face face;

    FT_GlyphSlot slot;
    FT_Matrix matrix;                 /* transformation matrix */
    FT_Vector pen;                    /* untransformed origin  */


    if (FT_Init_FreeType(&library))
        fatal_error("Could not initialize freetype");

    if (FT_New_Face(library, fontPath, 0, &face))
        fatal_error("Could not load font");

    if (FT_Set_Char_Size(face, pt * 64, 0, dpi, 0))
        fatal_error("Could not set font size");


    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    double angle = 0.0;
    matrix.xx = (FT_Fixed) (cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed) (-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed) (sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed) (cos(angle) * 0x10000L);

    int scale = 100;
    pen.x = 0;
    pen.y = 0;
    int maxLineY = 0;
    *charHeight = 0;
    *charWidth = 0;
    const int char_padding = 2;
    for (char c = ' ' + 1; c <= '~'; c++) {
        FT_Set_Transform(face, &matrix, &pen);

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            fatal_error("Could not load font characters");

        if (pen.x + slot->bitmap_left + slot->bitmap.width >= width) {
            pen.x = 0;
            pen.y = maxLineY;
            maxLineY = 0;
        }

        int x = pen.x;// + dpi - slot->bitmap_left;
        int y = pen.y + slot->metrics.vertAdvance / scale - slot->bitmap_top;

        FT_Int i, j, p, q;
        FT_Int x_max = x + slot->bitmap.width;
        FT_Int y_max = y + slot->bitmap.rows;


        for (i = x, p = 0; i < x_max; i++, p++) {
            for (j = y, q = 0; j < y_max; j++, q++) {
                if (i < 0 || j < 0 || i >= width || j >= height)
                    continue;

                (*image)[j * width + i] |= slot->bitmap.buffer[q * slot->bitmap.width + p];
            }
        }

        (*charBounds)[c] = vec4(1.0 * x / width, 1.0 * y / height,
                                1.0 * (x_max - x) / width, 1.0 * (y_max - y) / height);

        pen.x += slot->bitmap.width + char_padding;
        maxLineY = std::max(maxLineY, y_max);
        *charHeight = std::max(*charHeight, (int) slot->bitmap.rows);
        *charWidth = std::max(*charWidth, (int) slot->bitmap.width);
        //printf("%c: %3d %3d %3d %5lu\n", c, slot->bitmap_left, slot->bitmap_top, slot->bitmap.width, slot->advance.x);

    }

    (*charBounds)[' '] = vec4(1.0 * (width - *charWidth) / width, 1.0 * (height - *charHeight) / height,
                              1.0 * (*charWidth) / width, 1.0 * (*charHeight) / height);

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}

void
FontRenderer::initFonts(const char *fontPath, Texture texture, int width, int height, int *charHeight, int *charWidth,
                        int pt, int dpi) {

    unsigned char *atlas;
    createFontAtlas(fontPath, width, height, pt, dpi, &atlas, &charBounds, charHeight, charWidth);
    texture.bindTexture();
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED,
                    GL_UNSIGNED_BYTE, atlas);
    delete[] atlas;

}

vec2 FontRenderer::getCharsize(char character, float atlasSize, float charHeight, vec2 size) {
    return charBounds[character].zw() * atlasSize / charHeight * size;
}

void FontRenderer::drawText(const char *text, vec2 origin, vec2 size, vec4 color, int align) {
    textShader.use();
    fontAtlas->bindTexture();

    glUniform4fv(glGetUniformLocation(textShader, "color"), 1, (GLfloat *) &color);

    vec2 pen;
    float maxWidth = getCharsize(' ', atlas_size, atlas_charHeight, size).x;

    float textWidth = strlen(text) * maxWidth;

    if (align == 0)
        pen = origin - vec2(textWidth / 2, 0);
    else if (align == 1)
        pen = origin - vec2(textWidth, 0);
    else
        pen = origin;

    while (*text != 0) {
        vec2 charSize = getCharsize(*text, atlas_size, atlas_charHeight, size);
        float dw = maxWidth - charSize.x;
        pen += vec2(dw / 2, 0);
        textPlane->updateVertices(pen, charSize);

        glUniform4fv(glGetUniformLocation(textShader, "charBounds"), 1, (GLfloat *) (&charBounds[*text]));

        textPlane->draw();

        text++;
        pen += vec2(charSize.x, 0);
        pen += vec2(dw - dw / 2, 0);
    }
}
