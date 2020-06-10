//
// Created by david on 09/06/2020.
//

#include "Text.h"

Text::Text(FontRenderer &fr, string text, vec2 origin, vec2 size, float z, vec4 color, int align)
        : text(text), _origin(origin), _size(size), _z(z), color(color), align(align), fr(&fr) {

}

void Text::draw() {
    fr->drawText(text.c_str(), origin(), size(), color, align);
}

void Text::update_text(string str) {
    this->text = str;
}
