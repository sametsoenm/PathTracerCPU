#include "framebuffer.h"

void Framebuffer::set(size_t x, size_t y, const glm::vec3& color) {
	_buffer[y * _width + x] = color;
}

glm::vec3 Framebuffer::get(size_t x, size_t y) const {
	return _buffer[y * _width + x];
}

void Framebuffer::clear(const glm::vec3& color = glm::vec3(0.0f)) {
	std::fill(_buffer.begin(), _buffer.end(), color);
}

size_t Framebuffer::width() const {
	return _width;
}

size_t Framebuffer::height() const {
	return _height;
}