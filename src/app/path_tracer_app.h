#pragma once

#include "renderer/renderer.h"
#include "platform/window.h"
#include "platform/display.h"
#include "gui/gui_layer.h"

class PathTracerApp {
public:
	PathTracerApp();
	void run();

private:
	void processInput();
	void update();
	void render();

	void setActiveRenderer(const settings::RendererType type);
	void setActiveScene(const size_t idx);

	Window _window;
	Display _display;
	GuiLayer _gui;

	size_t _activeSceneIdx = 0;
	bool _sceneHasChanged = false;
	std::vector<SceneEntry> _scenes;
	Renderer* _activeRenderer = nullptr;
	settings::RendererType _activeRendererType;
	PBRRenderer _pbrRenderer;
	SimpleRenderer _simpleRenderer;
};