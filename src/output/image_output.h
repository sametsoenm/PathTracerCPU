#pragma once

#include <string>
#include <vector>

#include <glm/vec3.hpp>
#include "framebuffer/framebuffer.h"

namespace img::output {
	void output_image(const std::string& filename, const Framebuffer& fb);

}
std::vector<uint8_t> framebuffer_to_rgb8(const Framebuffer& fb);