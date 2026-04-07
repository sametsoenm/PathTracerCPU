#pragma once

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include "constants.h"

namespace sampling {

    inline glm::vec2 sample_uniform_disk(const glm::vec2& u) {
        const float r = glm::sqrt(u.x);
        const float phi = 2.0f * math::PI * u.y;

        const float x = r * glm::cos(phi);
        const float y = r * glm::sin(phi);

        return glm::vec2(x, y);
    }

    inline glm::vec3 sample_cosine_hemisphere(const glm::vec2& u) {
        const float r = glm::sqrt(u.x);
        const float phi = 2.0f * math::PI * u.y;

        const float x = r * glm::cos(phi);
        const float y = r * glm::sin(phi);
        const float z = glm::sqrt(glm::max(0.0f, 1.0f - x * x - y * y));

        return glm::vec3(x, y, z);
    }

    inline glm::vec3 sample_uniform_hemisphere(const glm::vec2& u) {
        const float z = u.x;
        const float r = glm::sqrt(glm::max(0.0f, 1.0f - z * z));
        const float phi = 2.0f * math::PI * u.y;

        return glm::vec3(
            r * glm::cos(phi),
            r * glm::sin(phi),
            z
        );
    }

    inline glm::vec3 sample_uniform_sphere(const glm::vec2& u) {
        const float z = 1.0f - 2.0f * u.x;
        float r = glm::sqrt(glm::max(0.0f, 1.0f - z * z));
        const float phi = 2.0f * math::PI * u.y;

        return glm::vec3(
            r * glm::cos(phi),
            r * glm::sin(phi),
            z
        );
    }

    inline float pdf_cosine_hemisphere(float cos_theta) {
        return (cos_theta > 0.0f) ? cos_theta / math::PI : 0.0f;
    }

    inline float pdf_uniform_hemisphere() {
        return 0.5f / math::PI;
    }

    inline float pdf_uniform_sphere() {
        return 0.25f / math::PI;
    }

    inline float sample_isotropic_phase_func() {
        return 0.25f / math::PI;
    }

    inline float pdf_isotropic_phase_func() {
        return 0.25f / math::PI;
    }

    inline float power_heuristic(const float pdf1, const float pdf2) {
        const float a = pdf1 * pdf1;
        const float b = pdf2 * pdf2;
        return a / (a + b);
    }
}