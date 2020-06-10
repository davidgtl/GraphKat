//
// Created by david on 10/06/2020.
//

#include "Image.h"

Image::Image(Texture *texture, vec2 origin, vec2 size, float z)
        : IPlanePrimitive("image", origin, size, z), _texture(texture) {
}

void Image::draw() {
    shader->use();
    _texture->bindTexture(0);
    IPlanePrimitive::draw();
}
