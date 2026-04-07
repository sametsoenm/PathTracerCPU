#include "image_output.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

inline float linear_to_srgb(float x) {
    x = glm::clamp(x, 0.0f, 1.0f);
    if (x <= 0.0031308f)
        return 12.92f * x;
    return 1.055f * std::pow(x, 1.0f / 2.4f) - 0.055f;
}

void img::output::output_image(
	const std::string& filename, 
    const Framebuffer& fb)
{
    std::vector<uint8_t> img(fb.width() * fb.height() * Framebuffer::CHANNELS);

    for (int y = 0; y < fb.height(); y++) {
        for (int x = 0; x < fb.width(); x++) {
            int index = (y * fb.width() + x) * Framebuffer::CHANNELS;

            const glm::vec3 color = glm::clamp(fb.get(x, y), 0.0f, 1.0f);

            img[index + 0] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.r));
            img[index + 1] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.g));
            img[index + 2] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.b));
        }
    }

    const int stride_in_bytes = static_cast<int>(fb.width() * Framebuffer::CHANNELS);

    if (stbi_write_png(
        filename.c_str(), 
        fb.width(), fb.height(), 
        Framebuffer::CHANNELS, 
        img.data(), 
        stride_in_bytes)) 
    {
        std::cout << "Image successfully saved as '" << filename << "'.\n";
        system(("start " + filename).c_str());
    }
    else {
        std::cerr << "An error occured while writing the PNG file.\n";
    }
}

std::vector<uint8_t> framebuffer_to_rgb8(const Framebuffer& fb) {
    std::vector<uint8_t> img(fb.width() * fb.height() * Framebuffer::CHANNELS);

    for (int y = 0; y < fb.height(); y++) {
        for (int x = 0; x < fb.width(); x++) {
            int index = (y * fb.width() + x) * Framebuffer::CHANNELS;

            const glm::vec3 color = glm::clamp(fb.get(x, y), 0.0f, 1.0f);

            img[index + 0] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.r));
            img[index + 1] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.g));
            img[index + 2] = static_cast<uint8_t>(255.999f * linear_to_srgb(color.b));
        }
    }

    return img;
}