#pragma once
#include <glm/glm.hpp>
#include <vector>

class Framebuffer {
public:

	Framebuffer(size_t __width, size_t __height) :
		_width(__width), _height(__height), _buffer(__width * __height, glm::vec3(0.0f)) { }

	void set(size_t x, size_t y, const glm::vec3& color);
	glm::vec3 get(size_t x, size_t y) const;
	void clear(const glm::vec3& color);

	size_t width() const;
	size_t height() const;

	static constexpr uint8_t CHANNELS = 3;

private:
	size_t _width;
	size_t _height;

	std::vector<glm::vec3> _buffer;
};