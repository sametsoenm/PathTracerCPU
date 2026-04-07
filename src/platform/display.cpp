#include "display.h"

#include <glm/common.hpp>
#include <glm/vec3.hpp>

#include "util/image.h"

#include <cmath>
#include <chrono>
#include <iostream>

Display::Display(int width, int height)
    : _shader(shaders::vertexShaderSrc, shaders::fragmentShaderSrc),
    _texture(width, height),
    _quad(),
    _pixels(static_cast<size_t>(width)* static_cast<size_t>(height)* Framebuffer::CHANNELS)
{
    _shader.use();
    _shader.setInt("uTexture", 0);
}

void Display::display(const Framebuffer& fb) {
    updatePixels(fb);

    _texture.upload(_pixels.data());
    _shader.use();
    _texture.bind(0);
    _quad.draw();
}

void Display::updatePixels(const Framebuffer& fb) {
    for (int y = 0; y < fb.height(); ++y) {
        for (int x = 0; x < fb.width(); ++x) {
            const int index = (y * fb.width() + x) * Framebuffer::CHANNELS;

            const glm::vec3 color = glm::clamp(fb.get(x, y), 0.0f, 1.0f);

            _pixels[index + 0] = static_cast<uint8_t>(255.999f * image::linear_to_srgb(color.r));
            _pixels[index + 1] = static_cast<uint8_t>(255.999f * image::linear_to_srgb(color.g));
            _pixels[index + 2] = static_cast<uint8_t>(255.999f * image::linear_to_srgb(color.b));
        }
    }
}