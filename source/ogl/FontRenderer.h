//
// Created by David Ghitulescu on 2019-07-30.
//
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include "Plane.h"
#include "ProgramShader.h"
#include "Texture.h"

#pragma once
using namespace glm;

class FontRenderer {
public:
    int atlas_size = 512;
    int atlas_charHeight, atlas_charWidth;

    FontRenderer(int atlas_size, int pt, int dpi);

    ~FontRenderer();

    void drawText(const char *text, vec2 origin, vec2 size, vec4 color, int align);

private:
    void initFonts(Texture texture, int width, int height, int *charHeight, int *charWidth, int pt, int dpi);

    void createFontAtlas(int width, int height, int pt, int dpi, unsigned char **image, vec4 **charBounds,
                         int *charHeight, int *charWidth);

    vec2 getCharsize(char character, float atlasSize, float charHeight, vec2 size);

    vec4 *charBounds;
    Plane *textPlane;
    ProgramShader textShader;
    Texture *fontAtlas;
};

