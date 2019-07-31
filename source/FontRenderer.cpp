//
// Created by David Ghitulescu on 2019-07-30.
//
#include "FontRenderer.h"
#include "messages.h"
#include <ft2build.h>
#include <math.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include FT_FREETYPE_H

using namespace std;
using namespace glm;

void createFontAtlas(int width, int height, int pt, int dpi, unsigned char **image, vec4 **charBounds) {

    *image = new unsigned char[width * height];
    *charBounds = new vec4[256];
    memset(*image, 0, width * height);

    FT_Library library;
    FT_Face face;

    FT_GlyphSlot slot;
    FT_Matrix matrix;                 /* transformation matrix */
    FT_Vector pen;                    /* untransformed origin  */


    char filename[] = "fonts/Roboto_Mono/RobotoMono-Regular.ttf";                           /* first argument     */


    if (FT_Init_FreeType(&library))
        fatal_error("Could not initialize freetype");

    if (FT_New_Face(library, filename, 0, &face))
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
    int height_max = 0;
    const int char_padding = 2;
    for (char c = ' '; c <= '~'; c++) {
        FT_Set_Transform(face, &matrix, &pen);

        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            fatal_error("Could not load font characters");

        if (pen.x + slot->bitmap_left + slot->bitmap.width >= width) {
            pen.x = 0;
            pen.y = height_max;
            height_max = 0;
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
        height_max = std::max(height_max, y_max);
        //printf("%c: %3d %3d %3d %5lu\n", c, slot->bitmap_left, slot->bitmap_top, slot->bitmap.width, slot->advance.x);

    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
}
