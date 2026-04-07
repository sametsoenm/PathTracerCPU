#include "path_tracer_app.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gui/imgui.h"

#include <chrono>
#include <iostream>

PathTracerApp::PathTracerApp() : 
	_window(settings::IMAGE_WIDTH, settings::IMAGE_HEIGHT, "BathTracer"),
	_scenes(),
	_pbrRenderer(nullptr),
	_simpleRenderer(nullptr),
	_display(settings::IMAGE_WIDTH, settings::IMAGE_HEIGHT),
	_gui(_window.handle()) {

	_scenes.push_back({ 
		std::string(settings::to_string(settings::SceneType::SPHERES)), 
		std::make_unique<Scene>(settings::SceneType::SPHERES) 
		});
	_scenes.push_back({
		std::string(settings::to_string(settings::SceneType::CORNELL_BOX)),
		std::make_unique<Scene>(settings::SceneType::CORNELL_BOX)
		});
	_scenes.push_back({
		std::string(settings::to_string(settings::SceneType::OG)),
		std::make_unique<Scene>(settings::SceneType::OG)
		});
	_scenes.push_back({
		std::string(settings::to_string(settings::SceneType::SUZANNE)),
		std::make_unique<Scene>(settings::SceneType::SUZANNE)
		});
	_scenes.push_back({
		std::string(settings::to_string(settings::SceneType::EMPTY)),
		std::make_unique<Scene>(settings::SceneType::EMPTY)
		});
	_activeSceneIdx = 0;

	_pbrRenderer.setScene(_scenes[_activeSceneIdx].scene.get());
	_simpleRenderer.setScene(_scenes[_activeSceneIdx].scene.get());

	_activeRenderer = &_simpleRenderer;
	_activeRendererType = _activeRenderer->getType();
}

void PathTracerApp::run() {

	while (!_window.shouldClose()) {
		processInput();
		update();
		render();

		_window.swapBuffers();
		_window.pollEvents();
	}
}

void PathTracerApp::processInput() {
	if (glfwGetKey(_window.handle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		_window.setShouldClose(true);
	}

	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	bool cameraChanged = false;

	const float scrollY = _window.consumeScrollOffsetY();
	if (scrollY != 0.0f) {
		_scenes[_activeSceneIdx].scene->cam().zoom(scrollY);
		cameraChanged = true;
	}

	double mouseDx = 0.0;
	double mouseDy = 0.0;
	if (_window.consumeMouseDelta(mouseDx, mouseDy)) {
		constexpr float rotationSpeed = 0.005f;

		const float yaw = -static_cast<float>(mouseDx) * rotationSpeed;
		const float pitch = -static_cast<float>(mouseDy) * rotationSpeed;

		if (_window.isLeftMouseButtonDown()) {
			_scenes[_activeSceneIdx].scene->cam().orbit(yaw, pitch);
			cameraChanged = true;
		}
		else if (_window.isRightMouseButtonDown()) {
			_scenes[_activeSceneIdx].scene->cam().look(yaw, pitch);
			cameraChanged = true;
		}
		_activeRenderer->setIsRendering(true);
	}

	if (cameraChanged) {
		_activeRenderer->resetAccumulation();
	}
}

void PathTracerApp::update() {

	if (_activeRenderer->getType() != _activeRendererType) {
		setActiveRenderer(_activeRendererType);
		_activeRenderer->resetAccumulation();
		_activeRenderer->setIsRendering(true);
	}
	if (_sceneHasChanged) {
		setActiveScene(_activeSceneIdx);
		_sceneHasChanged = false;
		_activeRenderer->resetAccumulation();
		_activeRenderer->setIsRendering(true);
	}
	auto t0 = std::chrono::steady_clock::now();
	_activeRenderer->render();
	auto t1 = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration<double, std::milli>(t1 - t0);
	std::cout << "Render time: " << duration.count() << " miliseconds\n";

}

void PathTracerApp::render() {
	//auto t0 = std::chrono::steady_clock::now();
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	_gui.beginFrame();

	settings::RendererType nextType = _activeRendererType;
	size_t nextSceneIdx = _activeSceneIdx;
	_gui.draw(*_activeRenderer, nextType, _scenes, nextSceneIdx);
	if (nextType != _activeRendererType)
		_activeRendererType = nextType;
	if (nextSceneIdx != _activeSceneIdx) {
		_activeSceneIdx = nextSceneIdx;
		_sceneHasChanged = true;
	}


	_display.display(_activeRenderer->buffer());

	_gui.endFrame();
	//auto t1 = std::chrono::steady_clock::now();
	//auto duration = std::chrono::duration<double>(t1 - t0);
	//std::cout << "Display time: " << duration.count() << " seconds\n";
}

void PathTracerApp::setActiveRenderer(const settings::RendererType type) {
	switch (type) {
	case settings::RendererType::SIMPLE:
		_activeRenderer = &_simpleRenderer;
		break;
	case settings::RendererType::PBR:
		_activeRenderer = &_pbrRenderer;
		break;
	default:
		_activeRenderer = &_simpleRenderer;
	}
}

void PathTracerApp::setActiveScene(const size_t idx) {
	_simpleRenderer.setScene(_scenes[idx].scene.get());
	_pbrRenderer.setScene(_scenes[idx].scene.get());

}