#pragma once

#include <cstddef>
#include <limits>
#include <unordered_map>
#include <string_view>

namespace settings {

	enum class RenderStrategy {
		SINGLE_THREAD,
		MULTI_THREAD_FIXED_ROWS,
		MULTI_THREAD_ATOMIC_ROWS,
		MULTI_THREAD_TILES,
		INTERACTIVE
	};

	enum class FogMode {
		SINGLE_SCATTER,
		MULTIPLE_SCATTER
		// after adding ne type also adjust the corresp. array!!!
	};

	enum class RendererType {
		SIMPLE,
		PBR
		// after adding ne type also adjust the corresp. array!!!
	};

	enum class SceneType {
		CORNELL_BOX,
		SPHERES,
		OG,
		SUZANNE,
		EMPTY
		// after adding ne type also adjust the corresp. array!!!
	};

	enum class ShadingMode {
		FLAT,
		PHONG,
		GOURAUD
	};

	constexpr std::string_view to_string(RendererType t) {
		switch (t) {
		case RendererType::SIMPLE: return "Simple";
		case RendererType::PBR: return "PBR";
		default: return "Unkown";
		}
	}

	constexpr std::string_view to_string(SceneType t) {
		switch (t) {
		case SceneType::CORNELL_BOX: return "Cornell Box";
		case SceneType::SPHERES: return "Spheres";
		case SceneType::OG: return "OG";
		case SceneType::SUZANNE: return "Suzanne";
		case SceneType::EMPTY: return "Empty";
		default: return "Unkown";
		}
	}

	constexpr std::string_view to_string(FogMode m) {
		switch (m) {
		case FogMode::SINGLE_SCATTER: return "Single Scattering";
		case FogMode::MULTIPLE_SCATTER: return "Multiple Scattering";
		default: return "Unkown";
		}
	}

	constexpr const char* FILE_NAME = "output.png";

	constexpr size_t IMAGE_WIDTH = 500;
	constexpr size_t IMAGE_HEIGHT = 400;
	constexpr float ASPECT_RATIO = 
		static_cast<float>(IMAGE_WIDTH) / static_cast<float>(IMAGE_HEIGHT);

	constexpr float T_MIN = 1e-5f;
	constexpr float T_MAX = std::numeric_limits<float>::max();
	constexpr size_t BVH_LEAF_PRIMITIVE_COUNT = 10;

	constexpr size_t MAX_RAY_DEPTH = 5;
	constexpr size_t SAMPLES_PER_PIXEL = 1;
	constexpr size_t THREAD_COUNT = 32;
	constexpr size_t TILE_SIZE = 32;

	constexpr bool FOGGY = false;
	constexpr float FOG_DENSITY = 0.2f;
	constexpr bool ENV_MAP = true;
	constexpr const char* ENV_MAP_ON_START = "Indoor";

	constexpr ShadingMode SHADING_MODE = ShadingMode::FLAT;

}