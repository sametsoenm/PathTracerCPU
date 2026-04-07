#pragma once

#include <vector>

#include "shader_program.h"
#include "texture2d.h"
#include "screen_quad.h"
#include "framebuffer/framebuffer.h"

class Framebuffer;
class Texture2D;
class ScreenQuad;

class Display {
public:
    Display(const int width, const int height);
    void display(const Framebuffer& fb);

private:
    void updatePixels(const Framebuffer& fb);

    ShaderProgram _shader;
    Texture2D _texture;
    ScreenQuad _quad;
    std::vector<uint8_t> _pixels;
};