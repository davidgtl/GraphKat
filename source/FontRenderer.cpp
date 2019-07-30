//
// Created by David Ghitulescu on 2019-07-30.
//
#include "FontRenderer.h"
#include "messages.h"
#include <ft2build.h>
#include <math.h>
#include <stdlib.h>
#include FT_FREETYPE_H

unsigned char *createFontAtlas(int width, int height, int pt, int dpi) {

    auto image = new unsigned char[width * height];
    memset(image, 0, width * height);

    FT_Library library;
    FT_Face face;

    FT_GlyphSlot slot;
    FT_Matrix matrix;                 /* transformation matrix */
    FT_Vector pen;                    /* untransformed origin  */

    char a_start = ' ';
    char a_end = '~';
    int ascii_length = a_end - a_start + 1;
    char filename[] = "fonts/Roboto_Mono/RobotoMono-Regular.ttf";                           /* first argument     */
    char *text = new char[ascii_length];
    for (char c = a_start; c <= a_end; c++) {
        text[c - a_start] = c;
    }

    if (FT_Init_FreeType(&library))
        fatal_error("Could not initialize freetype");

    if (FT_New_Face(library, filename, 0, &face))
        fatal_error("Could not load font");

    if (FT_Set_Char_Size(face, 40 * 64, 0, 100, 0))
        fatal_error("Could not set font size");


    /* cmap selection omitted;                                        */
    /* for simplicity we assume that the font contains a Unicode cmap */

    slot = face->glyph;

    double angle = 0.0;
    matrix.xx = (FT_Fixed) (cos(angle) * 0x10000L);
    matrix.xy = (FT_Fixed) (-sin(angle) * 0x10000L);
    matrix.yx = (FT_Fixed) (sin(angle) * 0x10000L);
    matrix.yy = (FT_Fixed) (cos(angle) * 0x10000L);

    pen.x = 0;
    pen.y = 0;

    for (int n = 0; n < ascii_length; n++) {
        FT_Set_Transform(face, &matrix, &pen);

        if (FT_Load_Char(face, text[n], FT_LOAD_RENDER))
            fatal_error("Could not load font characters");

        int x = slot->bitmap_left;
        int y = slot->bitmap_top;

        FT_Int i, j, p, q;
        FT_Int x_max = x + slot->bitmap.width;
        FT_Int y_max = y + slot->bitmap.rows;

        if (x_max >= width) {
            x = 0;
            y += slot->bitmap.rows;
        }

        for (i = x, p = 0; i < x_max; i++, p++) {
            for (j = y, q = 0; j < y_max; j++, q++) {
                if (i < 0 || j < 0 || i >= width || j >= height)
                    continue;

                image[j * width + i] |= slot->bitmap.buffer[q * slot->bitmap.width + p];
            }
        }

        pen.x += slot->advance.x;
        pen.y += slot->advance.y;
    }

    FT_Done_Face(face);
    FT_Done_FreeType(library);
    return image;
}
