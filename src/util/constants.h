#pragma once

#include <glm/vec3.hpp>

namespace math {
    inline constexpr float PI = 3.14159265358979323846f;
    inline constexpr float EPSILON = 1e-4f;
}

namespace material {
    namespace metal {

        constexpr glm::vec3 F0_GOLD = glm::vec3(1.000f, 0.766f, 0.336f);
        constexpr glm::vec3 F0_SILVER = glm::vec3(0.972f, 0.960f, 0.915f);
        constexpr glm::vec3 F0_COPPER = glm::vec3(0.955f, 0.637f, 0.538f);
        constexpr glm::vec3 F0_ALUMINIUM = glm::vec3(0.913f, 0.921f, 0.925f);
        constexpr glm::vec3 F0_IRON = glm::vec3(0.560f, 0.570f, 0.580f);

        constexpr glm::vec3 ETA_GOLD_RGB = glm::vec3(0.160875f, 0.608000f, 1.345875f);
        constexpr glm::vec3 KAPPA_GOLD_RGB = glm::vec3(4.087687f, 2.120000f, 1.814625f);
        constexpr float ETA_GOLD_R = 0.160875f;
        constexpr float ETA_GOLD_G = 0.608000f;
        constexpr float ETA_GOLD_B = 1.345875f;
        constexpr float KAPPA_GOLD_R = 4.087687f;
        constexpr float KAPPA_GOLD_G = 2.120000f;
        constexpr float KAPPA_GOLD_B = 1.814625f;

    }

    namespace dielectric {

        constexpr glm::vec3 F0_DIELECTRIC = glm::vec3(0.04f);
        constexpr glm::vec3 F0_WATER = glm::vec3(0.02f);
        constexpr glm::vec3 F0_PLASTIC = glm::vec3(0.04f);
        constexpr glm::vec3 F0_DIAMOND = glm::vec3(0.17f);
        constexpr glm::vec3 F0_GLAS = glm::vec3(0.06f);

    }
}