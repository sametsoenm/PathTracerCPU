#pragma once

#include "rendersettings.h"

#include <vector>

struct GLFWwindow;

class Renderer;
struct SceneEntry;

class GuiLayer {
public:
    GuiLayer(GLFWwindow* window);
    ~GuiLayer();

    void beginFrame();
    void draw(Renderer& renderer, settings::RendererType& activeRenderType,
        const std::vector<SceneEntry>& scenes, size_t& activeSceneIdx);
    void endFrame();

private:
    char _filenameBuffer[256] = "output.png";
};