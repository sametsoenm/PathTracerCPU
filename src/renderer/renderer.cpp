#include "renderer.h"
#include "intersectables/sphere.h"
#include "util/sampling.h"
#include "util/onb.h"

#include <glm/geometric.hpp>

#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <atomic>
#include <future>

using namespace sampling;

Renderer::Renderer(Scene* scene) :
	_fb(settings::IMAGE_WIDTH, settings::IMAGE_HEIGHT),
	_scene(scene),
	_sampleIdx(0) {
	_accum.resize(_fb.width() * _fb.height(), glm::vec3(0.0f));

	_tilesX = (_fb.width() + settings::TILE_SIZE - 1) / settings::TILE_SIZE;
	_tilesY = (_fb.height() + settings::TILE_SIZE - 1) / settings::TILE_SIZE;
	_tileCount = _tilesX * _tilesY;

}

void Renderer::render(settings::RenderStrategy strategy) {

	switch (strategy) {
	case settings::RenderStrategy::SINGLE_THREAD:
		render_single_thread();
		break;
	case settings::RenderStrategy::MULTI_THREAD_FIXED_ROWS:
		render_multi_thread_fixed_rows();
		break;
	case settings::RenderStrategy::MULTI_THREAD_ATOMIC_ROWS:
		render_multi_thread_atomic_rows();
		break;
	case settings::RenderStrategy::MULTI_THREAD_TILES:
		render_multi_thread_tiles();
		break;
	case settings::RenderStrategy::INTERACTIVE:
		render_interactive();
		break;
	default:
		render_multi_thread_tiles();
	}
	

}

void Renderer::render_single_thread() {
	auto start = std::chrono::steady_clock::now();
	RNG rng(1122);

	for (size_t y = 0; y < _fb.height(); y++) {
		//std::clog << "\rProgress: " << (100 - 100 * (_fb.height() - y) / _fb.height()) << "% " << ' ' << std::flush;
		for (size_t x = 0; x < _fb.width(); x++) {

			glm::vec3 result(0.0f);
			for (size_t sample = 0; sample < settings::SAMPLES_PER_PIXEL; sample++) {

				const float x_jit = static_cast<float>(x) + rng.next_float() - 0.5f;
				const float y_jit = static_cast<float>(y) + rng.next_float() - 0.5f;
				Ray r = _scene->cam().generateRay(x_jit, y_jit, _fb.width(), _fb.height(), rng.next_vec2());
				result += traceRay(r, rng);
			}
			result /= static_cast<float>(settings::SAMPLES_PER_PIXEL);
			_fb.set(x, y, result);

		}
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<double>(end - start);
	std::cout << "Render time single thread: " << duration.count() << " seconds\n";
}

void Renderer::render_multi_thread_fixed_rows() {
	auto start = std::chrono::steady_clock::now();
	
	std::vector<std::thread> threads;
	threads.reserve(settings::THREAD_COUNT);

	const size_t height = _fb.height();
	const size_t width= _fb.width();
	const size_t rows_per_thread = height / settings::THREAD_COUNT;

	for (size_t thread_id = 0; thread_id < settings::THREAD_COUNT; thread_id++) {
		const size_t start_y = thread_id * rows_per_thread;
		const const size_t end_y = (thread_id == settings::THREAD_COUNT - 1)
			? height
			: start_y + rows_per_thread;

		threads.emplace_back([this, width, height, start_y, end_y, thread_id]() {
			RNG rng(1122 + static_cast<unsigned int>(thread_id));
			for (size_t y = start_y; y < end_y; y++) {
				for (size_t x = 0; x < width; x++) {

					glm::vec3 result(0.0f);
					for (size_t sample = 0; sample < settings::SAMPLES_PER_PIXEL; sample++) {

						const float x_jit = static_cast<float>(x) + rng.next_float() - 0.5f;
						const float y_jit = static_cast<float>(y) + rng.next_float() - 0.5f;
						Ray r = _scene->cam().generateRay(x_jit, y_jit, width, height, rng.next_vec2());
						result += traceRay(r, rng);
					}
					result /= static_cast<float>(settings::SAMPLES_PER_PIXEL);
					_fb.set(x, y, result);

				}
			}
		});
	}
	for (auto& t : threads) {
		t.join();
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<double>(end - start);
	std::cout << "Render time multithread: " << duration.count() << " seconds\n";
	
}

void Renderer::render_multi_thread_atomic_rows() {
	auto start = std::chrono::steady_clock::now();

	std::atomic<size_t> next_row{ 0 };
	std::atomic<size_t> rows_done{ 0 }; // for progress bar

	std::vector<std::thread> threads;
	threads.reserve(settings::THREAD_COUNT);

	const size_t height = _fb.height();
	const size_t width = _fb.width();

	for (size_t thread_id = 0; thread_id < settings::THREAD_COUNT; thread_id++) {
		
		threads.emplace_back([this, width, height, &next_row, &rows_done, thread_id]() {
			while (true) {

				const size_t y = next_row.fetch_add(1);
				if (y >= height)
					break;

				RNG rng(1122 + static_cast<unsigned int>(y));

				for (size_t x = 0; x < width; x++) {

					glm::vec3 result(0.0f);
					for (size_t sample = 0; sample < settings::SAMPLES_PER_PIXEL; sample++) {

						const float x_jit = static_cast<float>(x) + rng.next_float() - 0.5f;
						const float y_jit = static_cast<float>(y) + rng.next_float() - 0.5f;
						Ray r = _scene->cam().generateRay(x_jit, y_jit, width, height, rng.next_vec2());
						result += traceRay(r, rng);
					}
					result /= static_cast<float>(settings::SAMPLES_PER_PIXEL);
					_fb.set(x, y, result);

				}
				rows_done.fetch_add(1);
			}
		});
	}

	// log progession
	while (rows_done.load() < height) {
		const size_t done = rows_done.load();
		const int progress = static_cast<int>(100 * done / height);

		std::clog << "\rProgress: " << progress << "% " << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for (auto& t : threads) {
		t.join();
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<double>(end - start);
	std::cout << "Render time multithread: " << duration.count() << " seconds\n";

}

void Renderer::render_multi_thread_tiles() {
	auto start = std::chrono::steady_clock::now();

	std::atomic<size_t> next_tile{ 0 };
	std::atomic<size_t> tiles_done{ 0 }; // for progress bar

	std::vector<std::thread> threads;
	threads.reserve(settings::THREAD_COUNT);

	const size_t height = _fb.height();
	const size_t width = _fb.width();
	const size_t tiles_x = (width + settings::TILE_SIZE - 1) / settings::TILE_SIZE;
	const size_t tiles_y = (height + settings::TILE_SIZE - 1) / settings::TILE_SIZE;
	const size_t tile_count = tiles_x * tiles_y;

	for (size_t thread_id = 0; thread_id < settings::THREAD_COUNT; thread_id++) {

		threads.emplace_back(
			[this, width, height, tiles_x, tile_count, &next_tile, &tiles_done, thread_id]() {

			while (true) {

				const size_t tile_idx = next_tile.fetch_add(1);
				if (tile_idx >= tile_count)
					break;

				RNG rng(1122 + static_cast<unsigned int>(tile_idx));

				const size_t tile_x = tile_idx % tiles_x;
				const size_t tile_y = tile_idx / tiles_x;

				const size_t start_x = tile_x * settings::TILE_SIZE;
				const size_t start_y = tile_y * settings::TILE_SIZE;

				const size_t end_x = std::min(start_x + settings::TILE_SIZE, width);
				const size_t end_y = std::min(start_y + settings::TILE_SIZE, height);

				for (size_t y = start_y; y < end_y; ++y) {
					for (size_t x = start_x; x < end_x; ++x) {

						glm::vec3 result(0.0f);
						for (size_t sample = 0; sample < settings::SAMPLES_PER_PIXEL; ++sample) {

							const float x_jit = static_cast<float>(x) + rng.next_float() - 0.5f;
							const float y_jit = static_cast<float>(y) + rng.next_float() - 0.5f;
							Ray r = _scene->cam().generateRay(x_jit, y_jit, width, height, rng.next_vec2());
							result += traceRay(r, rng);
						}
						result /= static_cast<float>(settings::SAMPLES_PER_PIXEL);
						_fb.set(x, y, result);
					}
				}
				tiles_done.fetch_add(1);
			}
		});
	}

	// log progession
	while (tiles_done.load() < tile_count) {
		const size_t done = tiles_done.load();
		const int progress = static_cast<int>(100 * done / tile_count);

		std::clog << "\rProgress: " << progress << "% " << std::flush;
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	for (auto& t : threads) {
		t.join();
	}
	auto end = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<double>(end - start);
	std::cout << "Render time multithread: " << duration.count() << " seconds\n";

}

void Renderer::render_interactive() {
	if (!_isRendering)
		return;

	std::atomic<size_t> next_tile{ 0 };

	std::vector<std::future<void>> tasks;
	tasks.reserve(settings::THREAD_COUNT);

	const size_t height = _fb.height();
	const size_t width = _fb.width();

	for (size_t thread_id = 0; thread_id < settings::THREAD_COUNT; thread_id++) {
		tasks.emplace_back(
			std::async(std::launch::async,
				[this, width, height, &next_tile, thread_id]() {

					while (true) {
						const size_t tile_idx = next_tile.fetch_add(1);
						if (tile_idx >= _tileCount) {
							break;
						}

						RNG rng(
							1122u
							+ static_cast<unsigned int>(tile_idx)
							+ static_cast<unsigned int>(_sampleIdx) 
							* static_cast<unsigned int>(_tileCount)
						);

						const size_t tile_x = tile_idx % _tilesX;
						const size_t tile_y = tile_idx / _tilesX;

						const size_t start_x = tile_x * settings::TILE_SIZE;
						const size_t start_y = tile_y * settings::TILE_SIZE;

						const size_t end_x = std::min(start_x + settings::TILE_SIZE, width);
						const size_t end_y = std::min(start_y + settings::TILE_SIZE, height);

						for (size_t y = start_y; y < end_y; y++) {
							for (size_t x = start_x; x < end_x; x++) {

								const float x_jit = static_cast<float>(x) + rng.next_float() - 0.5f;
								const float y_jit = static_cast<float>(y) + rng.next_float() - 0.5f;

								Ray r = _scene->cam().generateRay(x_jit, y_jit, width, height, rng.next_vec2());
								glm::vec3 result = traceRay(r, rng);

								const size_t idx = y * width + x;
								_accum[idx] += result;
								_fb.set(x, y, _accum[idx] / static_cast<float>(_sampleIdx + 1));
							}
						}
					}
				}
			)
		);
	}

	for (auto& task : tasks) {
		task.get();
	}

	_sampleIdx++;
}

const Framebuffer& Renderer::buffer() const {
	return _fb;
}

void Renderer::resetAccumulation() {
	_sampleIdx = 0;
	std::fill(_accum.begin(), _accum.end(), glm::vec3(0.0f));
}
