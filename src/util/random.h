#pragma once

#include <cstdint>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <random>

class RNG {
public:
    RNG()
        : RNG(std::random_device{}()) {
    }

    explicit RNG(std::uint32_t seed)
        : _engine(seed),
        _dist01(0.0f, 1.0f) {
    }

    float next_float() {
        return _dist01(_engine);
    }

    float next_float(float min, float max) {
        return min + (max - min) * next_float();
    }

    std::uint32_t next_uint() {
        return _engine();
    }

    glm::vec2 next_vec2() {
        return glm::vec2(next_float(), next_float());
    }

    glm::vec3 next_vec3() {
        return glm::vec3(next_float(), next_float(), next_float());
    }

private:
    std::mt19937 _engine;
    std::uniform_real_distribution<float> _dist01;
};